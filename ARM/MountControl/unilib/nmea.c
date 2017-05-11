#include "include.h"

#define NMEA_ARGS_QTY_MAX    17

#define NMEA_QUERY_NAME_LEN  5  // w/o null

typedef struct
{
	char name[NMEA_QUERY_NAME_LEN];
	void (*callback)(nmea_arg_t*);
	int  fields_qty;
	int  field_type[NMEA_ARGS_QTY_MAX];
} nmea_query_description_t;

static const nmea_query_description_t nmea_query_dscs[3] =
{	//NAME    CALLBACK          QTY  TYPES:   1     2          3        4         5         6         7          8          9           10        11         12        13        14        15         16         17
	{"GPRMC", gps_on_nmea_gprmc, 12, {NMF_HHMMSSMS, NMF_CHAR, NMF_POS,  NMF_CHAR, NMF_POS,  NMF_CHAR, NMF_FLOAT, NMF_FLOAT, NMF_DDMMYY, NMF_NONE, NMF_NONE,  NMF_CHAR}},
	{"GPGGA", gps_on_nmea_gpgga, 14, {NMF_HHMMSSMS, NMF_POS,  NMF_CHAR, NMF_POS,  NMF_CHAR, NMF_INT,  NMF_INT,   NMF_FLOAT, NMF_FLOAT,  NMF_CHAR, NMF_FLOAT, NMF_CHAR, NMF_NONE, NMF_NONE}},
	{"GPGSA", gps_on_nmea_gpgsa, 17, {NMF_CHAR,     NMF_CHAR, NMF_INT,  NMF_INT,  NMF_INT,  NMF_INT,  NMF_NONE,  NMF_NONE,  NMF_NONE,   NMF_NONE, NMF_NONE,  NMF_NONE, NMF_NONE, NMF_NONE, NMF_FLOAT, NMF_FLOAT, NMF_FLOAT}}
};

//=============================================================================
static uint8_t char2bin4(char c)
{
	if      ((c>='0')&&(c<='9'))  c = (c - '0');
	else if ((c>='A')&&(c<='F'))  c = (c - 'A') + 10;
	else if ((c>='a')&&(c<='f'))  c = (c - 'a') + 10;
	else                          c = 0;

	return (c);
}

//=============================================================================
static char* nmea_rx_str(uart_t* const uart)
{
	enum
	{
		NMEA_STRLEN_MIN =   7U,  // ttsss,x (tid[2] + sid[3] + ',' + data[1..74] )
		NMEA_STRLEN_MAX =  80U   // NMEA strings should be 80 characters or less (w/o CR-LF)
	};

	enum  // STATES
	{
		NMEA_IDLE = 0,
		NMEA_DATA = 1,
		NMEA_CRC  = 2,
		NMEA_CRLF = 3
	};

	static char    str[NMEA_STRLEN_MAX+1]; // + terminator
	static size_t  len;
	static uint8_t crc;
	static int     n;
	static int     state = NMEA_IDLE;
	int            c;

	while ((c=uart_getc(uart)) >= 0)
	{
//		dprintf("%c", c);

		if (c=='$')
		{
			//dprintf("\nNMEA-START\n");

			state = NMEA_DATA;
			len   = 0;
			crc   = 0;
			n     = 0;
			continue;
		}

		switch (state)
		{
			case NMEA_DATA:
			{							
				if (c == '*')
				{
					state = NMEA_CRC;
					//dprintx(crc);
				}
				else if ((len<NMEA_STRLEN_MAX) && isprint(c))
				{
					str[len++] = c;    // read NMEA data-string (w/o tokens: $ * <CR> <LF>)
					crc ^= c;
				}
				else
				{
//					dprintf("NMEA: BAD STR\n");

					state = NMEA_IDLE;

					return (NULL);         // reading maximum NMEA_STRLEN_MAX chars at once
				}
				break;
			}

			case NMEA_CRC:
			{
				n ^= (1<<2);  // 4,0,4,0...

				crc ^= char2bin4(c) << n; // xor crc by hi&low tetrads of rx_rcr

				if (!n)
				{
					state = NMEA_CRLF;					
				}
				break;
			}

			case NMEA_CRLF:
			{
				static const char crlf[2] = "\r\n"; // <CR><LF> - end of NMEA packet

				if (c==crlf[n])
				{                             
					if (!(n^=1))  // 1,0,1,0...
					{
						state = NMEA_IDLE;

						str[len] = '\0'; // terminator

						if (crc)
						{
//							dprintf("NMEA_BAD_CRC\n", c);
						}

						return (((len>=NMEA_STRLEN_MIN) && !crc)?  &str[0] : NULL);
					}						
					break;
				}
				// wo break fall to default
			}

			default:
			{
				state = NMEA_IDLE;
			}
		}
	}

	return (NULL);
}

//=============================================================================
static double nmea_str2pos(const char* const str) // str format: (G)GGMM.MM
{
	double deg_int;
	double deg_frac = modf(strtod(str, NULL)/100.0, &deg_int)*100.0/60.0; 

	return (deg_int + deg_frac);	// pure degrees [(G)GG.GGGG] format
}

//=============================================================================
static void nmea_scanf_args(const char* const str, nmea_arg_t* arg, int arg_type)
{
	if (str[0]==',')  // skip empty field
	{
		arg_type = NMF_NONE;
	}

	switch (arg->type = arg_type)
	{
		case NMF_FLOAT:
		{
			arg->x.f32 = atof(str);
			break;
		}

		case NMF_INT:
		{
			arg->x.i32 = atol(str);
			break;
		}

		case NMF_CHAR:
		{
			arg->x.c = str[0];
			break;
		}

		case NMF_HHMMSSMS:
		{
			for (uint_fast8_t i=0; i<3; i++) // HHMMSS + .M(S) later
			{
				arg->x.byte[i] = (str[i*2]-'0')*10 + str[i*2+1]-'0'; // atoi
			}

			char buf[] = "000";

			const char* p = &str[7]; // MS

			for (uint_fast8_t i=0; ((i<3) && isdigit(p[i])); i++)
			{
				buf[i] = p[i];
			}

            arg->x.time.ms = atol(&buf[0]);
			break;
		}

		case NMF_DDMMYY:
		{
			for (uint_fast8_t i=0; i<3; i++) // DDMMYY
			{
				arg->x.byte[i] = (str[i*2]-'0')*10 + str[i*2+1]-'0'; // atoi
			}
			break;
		}

		case NMF_POS: // (G)GGMM.MM  - latitude or longitude
		{
			arg->x.f64 = nmea_str2pos(str); // pure degrees [dd.dddd] format
			break;
		}

		default:
		{
			arg->type = NMF_NONE;
		}
	}
}

//=============================================================================
static nmea_arg_t* nmea_query_parser(char* str, const nmea_query_description_t* const qd)
{
	static nmea_arg_t args[NMEA_ARGS_QTY_MAX];

	for (int i=0; i<qd->fields_qty; i++)
	{
		str = strchr(str, ',');            // find coma

		if (!str++)                        // skip coma
		{
			return (NULL);                 // str is to short (coma not found)
		}

		nmea_scanf_args(str, &args[i], qd->field_type[i]);
	}

	return (&args[0]);
}

//=============================================================================
int nmea_task(uart_t* const uart)
{
	char* str = nmea_rx_str(uart);

	if (str)
	{
		for (int i=0; i<ARRAY_SIZE(nmea_query_dscs); i++)
		{
			if (!strncmp(str, nmea_query_dscs[i].name, NMEA_QUERY_NAME_LEN))  // first 5 chars is a query name
			{
				nmea_arg_t* args = nmea_query_parser(str, &nmea_query_dscs[i]);
				
				if (args)
				{
					nmea_query_dscs[i].callback(args);
				}
			}
		}
	}

	return (str!=0);
}

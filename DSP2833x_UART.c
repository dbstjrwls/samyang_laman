
#include "DSP2833x_Device.h"
#include "DSP2833x_28335_Define.h"
#include "DSP2833x_Examples.h"
#include "math.h"
#include "Main.h"
#include "Adc.h"
#include "Can.h"
#include "Ctrl.h"
#include "IO.h"
#include "Pie.h"
#include "Uart.h"
#include "I2c.h"
    
char asc2hex(char asc);
char hex2asc(char hex);
void write_txbuf_string(char *p);
void write_txbufc_string(char *p);
void write_txbuf_asc(char d);
void write_txbufc_asc(char d);
void write_txbuf_byte(char d);

void Debug_CMD(char commmand);
//void Debug_CMD_C(char command_c);

void Cmd_Ready_Message(void);
char Debug_cmd_parser (char *debug_cmd_rx);
char deci2asc(char hex);
int Calc_digit(BYTE ten_digit);
LONG Asci2num(BYTE *num_tmp, BYTE digit);

void Display_digit100(WORD Dis_digit);
void Display_Dot_digit100(WORD Dot_digit);
void Display_digit1000(WORD Dis_digit);
void Display_Dot_digit1000(WORD Dot_digit);
void Display_Realnum(float r_num, unsigned char n_dec);
void Display_Realnum_C(float r_num, unsigned char n_dec);

interrupt void ILLEGAL_ISR(void);
unsigned int hex2decimal(unsigned int hex_t);


BOOL UART_INTERRUPT = 0;
BOOL UART_INTERRUPT_C = 0;

int	debug_tx_pos = 0;
int	debug_tx_end = 0;
int	debug_txc_pos = 0;
int	debug_txc_end = 0;

unsigned int debug_cmd_index = 0;
char Debug_cmd_flag = 0;
unsigned int debug_cmd_index_c = 0;
char Debug_cmd_flag_c = 0;

char debug_cmd_rx[DEBUG_BUF_SIZE];
char debug_cmd_rxc[DEBUG_BUF_SIZE];
char debug_tx_buf[DEBUG_BUF_SIZE];
char debug_txc_buf[DEBUG_BUF_SIZE];

BYTE num_tmp  [IN_NUM];
BYTE num_tmp_c[IN_NUM];
BYTE num_tmp_1[IN_NUM];
BYTE num_tmp_2[IN_NUM];
BYTE num_tmp_3[IN_NUM];
BYTE num_tmp_4[IN_NUM];

unsigned int num_1_t = 0;
unsigned int num_2_t = 0;
unsigned int num_3_t = 0;
unsigned int num_4_t = 0;

char rxd = 0;
char rxd_c = 0;
char cmd_flag = 0;
BOOL Command_Start = 0;
BOOL sof_flag = 0;

char Debug_cmd_parser(char* debug_cmd_rx)
{
	unsigned int tmp = 0;
	int i = 0;
	char cmd = 0;
	char cmd2 = 0;  
	long num = 0;
	char digit = 0;

	for(i = 0; i < IN_NUM; i++ ) // IN_NUM == 10
	{
		num_tmp[i] = 0;
	}

	while( debug_cmd_rx[tmp] != END)	// Word Command Start, END == 0xFF
	{
		if(tmp == 0)
		{
			cmd = debug_cmd_rx[0] & 0xFF;
		}
		else if(tmp == 1)
		{
			cmd2 = debug_cmd_rx[1] & 0xFF;
		}
		else
		{
			digit++;	// Increase Digit Number
			if(tmp <= IN_NUM)
			{
				num_tmp[tmp - 2] = (char)debug_cmd_rx[tmp];
			}
		}
		tmp++;
	}
	//---------------------------------------------------------------------------------------------------------------
	num_tmp[tmp - 2] = END;
	tmp = 0;
	while( num_tmp[tmp] != END)	tmp++;
	
	if(digit != 0)
	{
		num = Asci2num(num_tmp, tmp);	// ASCII -> Number
		digit = 0;
	}
	else num = EOI;
	
	switch(cmd)
	{
		case 'A':
			cmd_flag = cmd;

			write_txbuf_string("\r\nEEPROM_WRITE\r\n");
			if(cmd2 == 0x20)
			{
				Write_Uint32(ADDR_EEPROM, (Uint32)num);
				delay_ms(10);
			}

			Cmd_Ready_Message();
			break;

		case 'B':
			cmd_flag = cmd;

			write_txbuf_string("\r\nEEPROM_READ\r\n");
			total_detect_cnt = Read_Uint32(ADDR_EEPROM);
			Display_Realnum(total_detect_cnt, 0);
			delay_ms(10);

			Cmd_Ready_Message();
			break;

		case 'C':
			write_txbuf_string("\r\n_LASER_HV\r\n");
			Write_AD5627_Data(0x38, 0x0001);
			delay_ms(100);

			if(cmd2 == 0x20) 
			{
				write_txbuf_string("\r\nHigh_Voltage_Set\r\n");
				Write_AD5627_Data(0x10, num);
			}
			delay_ms(10);
			Cmd_Ready_Message();

			break;

		case 'D':
			cmd_flag = cmd;

			if(cmd2 == 0x20) 
			{
				if(num == 1) 
				{
					OPTO_ANOD_HIGH();
					BEAM_15V_ON();
					delay_ms(100);
					BEAM_15V_OFF();
					BEAM_10V_ON();
					delay_ms(5);
					shutter_status = OPTO_COLT_STATUS();

					if(shutter_status == 0) 
						LED3_ON();
					else 
						LED3_OFF();
				}
				else 
				{
					OPTO_ANOD_LOW();
					BEAM_15V_OFF();
					BEAM_10V_OFF();
					delay_ms(5);
					shutter_status = OPTO_COLT_STATUS();

					if(shutter_status == 0) 
						LED3_ON();
					else 
						LED3_OFF();
				}		
			}

			break;

		case 'E':
			cmd_flag = cmd;

			if(cmd2 == 0x20) 
			{
				SET_TRIGGER_DLY(0xff);
			}

			break;
		       
		case 'F':
			if(Command_Start == 1) 
			{
				write_txbuf_string("\r\n_EX5_LASER_PORT_HIGH");
				CM_TRIG_HIGH();
				MODE_SEL_HIGH();
				PUMP_VALVE_OPEN();
				MAIN_VALVE_OPEN();
				RST_EX5_DISENABLE();
				F_INJECTOR_OPEN();
				PURGE_VALVE_OPEN();
				FILL_VALVE_OPEN();
				FAN_OFF();
				MOTOR2_ON();
				VACCUM_PUMP_ON();
				MOTOR1_ON();
				PEAK_RST_HIGH();
				OPTO_ANOD_HIGH();
			}

			break;

		case 'G':
			cmd_flag = cmd;

			if(cmd2 == 0x20) 
			{
				write_txbuf_string("\r\n_LASER_FILAMENT_MONITOR : ");
				Display_Realnum(AD_FILAMT_data, 1);
				write_txbuf_string("[V]");

				write_txbuf_string("\r\n_LASER_GAS-PRESSURE : ");
				Display_Realnum(AD_GASPRE_data, 1);
				write_txbuf_string("[V]");
			
				write_txbuf_string("\r\n_LASER_ENERGY_SENSOR : ");
				Display_Realnum(AD_ENERGY_data, 1);
				write_txbuf_string("[V]");
					
				write_txbuf_string("\r\n_LASER_HV_PEAK : ");
				Display_Realnum(AD_HVPEAK_data, 1);
				write_txbuf_string("[V]");

				write_txbuf_string("\r\n_LASER_ENERGY-MONITOR(UnCAL) : ");
				Display_Realnum(AD_ENMON_data, 1);
				write_txbuf_string("[V]");

				write_txbuf_string("\r\n_LASER_TEMP : ");
				Display_Realnum(AD_TEMP_data, 1);
				write_txbuf_string("[V]");

				write_txbuf_string("\r\n_CTRL_TEMP : ");
				Display_Realnum(AD_CONTROLTH_data, 1);
				write_txbuf_string("[C]");

				write_txbuf_string("\r\n_ENERGY_PEAK_VALUE : ");
				Display_Realnum(Mean_Peak, 1);
				write_txbuf_string("[V]");
				write_txbuf_string("\r\n");

				EX5_Laser_Fault = PWR_STATUS(); 

				if(EX5_Laser_Fault == 0) 
				{
					write_txbuf_string("\r\n_EX5_Laser_Fault");
					write_txbuf_string("\r\n_EX5_Laser_Fault");
				}
				else 
				{
					write_txbuf_string("\r\n_EX5_Laser_Ok");
				}
				write_txbuf_string("\r\n");

				if(inter_lock == 1) 
					write_txbuf_string("\r\n_EX5_LASER_INTERLOCK : HIGH");
				else 
					write_txbuf_string("\r\n_EX5_LASER_INTERLOCK : LOW");
				 
				write_txbuf_string("\r\n");

				if(Laser_Pwr_Complete == 1) 
				{
					write_txbuf_string("\r\n_EX5_LASER_PWR-5Minute_Ok");
				}
				else 
				{
					write_txbuf_string("\r\n_EX5_LASER_NOT_POWER");
				}
			}

			break;

		case 'H':
			cmd_flag = cmd;

			if(cmd2 == 0x20) 
			{
				if(num == 0) 
				{
					SET_TRIGGER_DLY(0x00); // 16.5~20nsec
				}
				else if (num == 1) 
				{
					SET_TRIGGER_DLY(0x80); // 1000
				}
				else if (num == 2) 
				{
					SET_TRIGGER_DLY(0x40); // 0100
				}
				else if (num == 3) 
				{
					SET_TRIGGER_DLY(0xC0); // 1100
				}
				else if (num == 4) 
				{		
					SET_TRIGGER_DLY(0x20); // 0010
				}
				else if (num == 5) 
				{		
					SET_TRIGGER_DLY(0xA0); // 1010
				}
				else if (num == 127) 
				{		
					SET_TRIGGER_DLY(0xFE); // 1010
				}
				else if (num == 255) 
				{
					SET_TRIGGER_DLY(0xff);
				}
			}

			break;
		
		case 'I' :
			cmd_flag = cmd;

			if(cmd2 == 0x20) 
			{
				if(num == 1) 
				{
					write_txbuf_string("\r\n_TRIGGER_SET_ON-10Hz");
					trig_gen_cmd = 1;	// Trigger-On
				}
				else 
				{
					write_txbuf_string("\r\n_TRIGGER_SET_OFF-10Hz");
					trig_gen_cmd = 0;	// Trigger-Off
				}
			}
			break;
		
		case 'J' :
			cmd_flag = cmd;

			if(cmd2 == 0x20) 
			{
				Trigger_Gen();
			}

			break;

		case 'K':
			if(Command_Start == 0) 
			{
				write_txbuf_string("\r\n_EX5_LASER_PORT_LOW");
				CM_TRIG_LOW();
				MODE_SEL_LOW();
				PUMP_VALVE_CLOSE();
				MAIN_VALVE_CLOSE();
				RST_EX5_ENABLE();
				F_INJECTOR_CLOSE();
				PURGE_VALVE_CLOSE();
				FILL_VALVE_CLOSE();
				FAN_ON();
				MOTOR2_OFF();
				VACCUM_PUMP_OFF();
				MOTOR1_OFF();
				PEAK_RST_LOW();
				OPTO_ANOD_LOW();
			}

			break;

		case 'L' :
			write_txbuf_string("\r\n_EX5-LASER READY");
			delay_ms(1000);
			write_txbuf_string("\r\n_EX5-LASER START");
			delay_ms(1000);
			RST_EX5_DISENABLE();
			MOTOR1_ON();

			break;

		case 'M' :
			write_txbuf_string("\r\n_EX5-LASER STOP");
			RST_EX5_ENABLE();

			break;

		case 'N' :
			if(num == 1) 
			{
				write_txbuf_string("\r\n_MOTOR2-ON");
				MOTOR2_ON();
			}
			else 
			{
				write_txbuf_string("\r\n_MOTOR2-OFF");
				MOTOR2_OFF();
			}
			break;
				
		case 'O' :
			if(num == 1) 
			{
				write_txbuf_string("\r\n_EX5_LASER_SYNC");
				sync_trig = 1;
				sync_iccd = 0;
				sync_emccd = 0;
			}
			else if(num == 2) 
			{
				write_txbuf_string("\r\n_ICCD_SYNC");
				sync_trig = 0;
				sync_iccd = 1;
				sync_emccd = 0;
			}
			else if(num == 3) 
			{
				write_txbuf_string("\r\n_EMCCD_SYNC");
				sync_trig = 0;
				sync_iccd = 0;
				sync_emccd = 1;
			}
			else 
			{
				sync_trig = 0;
				sync_iccd = 0;
				sync_emccd = 0;
			}

			break;
			 
		case 'P':
			write_txbuf_string("\r\n_Heliim_Vent_Sequence");
			Helium_Vent_Seq();

			Cmd_Ready_Message();
			break;                                  

		default :

			break;
	}

	Debug_cmd_flag   = 0;
	Debug_cmd_flag_c = 0;

	for(i = 0; i < DEBUG_BUF_SIZE; i++) {
		debug_cmd_rx[i]  = 0;
		debug_cmd_rxc[i] = 0;
	}

	return(1);
}

unsigned int R_FPGA_SRAM (unsigned int addr)
{
	unsigned int value = 0;
	value = (*((volatile  unsigned int *)0x100100 + addr));
	return(value);
}

char deci2asc(char hex)
{
	char da;

	da = hex & 0x0f;
    if((da >= 0) && (da <= 9))  return  ('0' + da);
    else return(0);
}

//---------------------------------------------------------------------------------------------------------------
void write_txbuf_string(char *p)
{
	char rd;

	while(rd = *p++)
	{             
		debug_tx_buf[debug_tx_end++] = rd;
		if(debug_tx_end >= DEBUG_BUF_SIZE - 1) debug_tx_end = 0;
	}
}

void write_txbufc_string(char *p)
{
	char rd;

	while(rd = *p++)
	{             
		debug_txc_buf[debug_txc_end++] = rd;
		if(debug_txc_end >= DEBUG_BUF_SIZE - 1) debug_txc_end = 0;
	}
}

void write_txbuf_asc(char d)
{
	debug_tx_buf[debug_tx_end++] = d;
	if(debug_tx_end >= DEBUG_BUF_SIZE - 1) debug_tx_end = 0;
}

void write_txbufc_asc(char d)
{
	debug_txc_buf[debug_txc_end++] = d;
	if(debug_txc_end >= DEBUG_BUF_SIZE - 1) debug_txc_end = 0;
}

//---------------------------------------------------------------------------------------------------------------
// ASCII -> number
LONG Asci2num(unsigned char* num_tmp, unsigned char digit)
{
	unsigned char i=0;
	unsigned char j=0;
	unsigned char k=0;
	long cal_num = 0;

	if(num_tmp[0] != '-')
	{
		for(i = digit; i > 0; i--)
		{
			j = num_tmp[i-1];
			if( (j >= 0x30 ) && ( j <= 0x39) )
			{
				j = j - 0x30;
			}
			else
			{
				return(0);
			}
			cal_num = cal_num + (j * Calc_digit(k));
			k++;
		}
	}
	else if(num_tmp[0] == '-')
	{
		for(i = digit; i > 1; i--)
		{
			j = num_tmp[i-1];
			if( (j >= 0x30 ) && ( j <= 0x39) )
			{
				j = j - 0x30;
			}
			else
			{
				return(0);
			}
			cal_num = cal_num + (j *  Calc_digit(k));
			k++;
		}
		cal_num = cal_num * -1;
	}

	return(cal_num);
}

//---------------------------------------------------------------------------------------------------------------
int Calc_digit(unsigned char ten_digit)
{
	char i = 0;
	int calc_digit = 1;

	for(i = 0; i < ten_digit; i++) calc_digit = calc_digit * 10;
	return(calc_digit);
}

//---------------------------------------------------------------------------------------------------------------
void Debug_CMD(char commmand)
{
	unsigned int IN_cmd = 0;
	unsigned char input_flag = 0;

	if(	Debug_cmd_flag == 1)
	{
		write_txbuf_string("Busy");
		return;
	}

	switch(commmand)
	{
 		case BACK :	// 0x08
				if( debug_cmd_index > 0)
				{
					debug_cmd_rx[debug_cmd_index - 1] = 0;
					debug_cmd_index--;

					write_txbuf_asc(BACK);
					write_txbuf_asc(' ');
					write_txbuf_asc(BACK);
				}
				break;

		case ESC :				
				break;

		case LF :
				break;

		case CR :
				if( debug_cmd_index == 0 )	// the case of no word
				{
					input_flag = 0;
					debug_cmd_rx[0] = 0;
				}
				else
				{
					debug_cmd_rx[debug_cmd_index] = END; // command end display
					for(IN_cmd = debug_cmd_index+1 ; IN_cmd < DEBUG_BUF_SIZE; IN_cmd++) // buffer clear
					{
						debug_cmd_rx[IN_cmd] = 0;
					}

					input_flag = 1;
					debug_cmd_index = 0;
				}
				Cmd_Ready_Message();
				break;
		
		default :
				if(((0x0 <= commmand) && (commmand < 0x3f))||((0x40 < commmand) && (commmand <= 0x5f))||((0x60 < commmand) && (commmand < 0x7b))||(commmand == 0x2d) ||(commmand == 0x20))
				{
					if(debug_cmd_index < DEBUG_BUF_SIZE)
					{
						debug_cmd_rx[debug_cmd_index] = commmand;
						debug_cmd_index++;
						write_txbuf_asc(commmand);
						input_flag = 0;
					}
					else debug_cmd_index = 0;
				}
				break;
	}

	if(input_flag == 1)
	{
		input_flag = 0;
		Debug_cmd_flag = 1;
		Debug_cmd_parser(debug_cmd_rx);
	}
}

//---------------------------------------------------------------------------------------------------------------
void Display_Realnum(float r_num, unsigned char n_dec)
{
	unsigned char i,j;
	unsigned char digit = 1;
	char index = 0;
	long r_tmp;
	char num_arr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	if(n_dec > 4) n_dec = 4;

	if(r_num < 0.0f)
	{
		*num_arr = '-';
		index++;
		r_num = -1.0f * r_num;
	}

	for(i = 0; i < n_dec; i++)
	{
		r_num *= 10.0f;
	}

	r_num += 0.5f;		
	r_tmp = (long)r_num;

	while(1)
	{
		r_tmp /= 10;
		if(r_tmp == 0) break;
		else	digit++;
	}

	r_tmp = (long)r_num;
		
	if(n_dec == 0)
	{
		for(j = 0; j < digit; j++)
		{
			*(num_arr + index + digit - j - 1) = deci2asc(r_tmp%10);
			r_tmp/=10;
		}
	}
	else
	{
		if(digit < n_dec + 1) digit = n_dec + 1;
		
		for(j = 0; j < digit; j++)
		{
			if(j == n_dec) 
			{
				*(num_arr + index + digit - j) = '.';
				index--;
			}
			*(num_arr + index + digit - j) = deci2asc(r_tmp%10);
			r_tmp/=10;
		}
	}
	write_txbuf_string(num_arr);
}

void Display_Realnum_C(float r_num, unsigned char n_dec)
{
	unsigned char i,j;
	unsigned char digit = 1;
	char index = 0;
	long r_tmp;
	char num_arr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	if(n_dec > 4) n_dec = 4;

	if(r_num < 0.0f)
	{
		*num_arr = '-';
		index++;
		r_num = -1.0f * r_num;
	}

	for(i = 0; i < n_dec; i++)
	{
		r_num *= 10.0f;
	}

	r_num += 0.5f;		
	r_tmp = (long)r_num;

	while(1)
	{
		r_tmp /= 10;
		if(r_tmp == 0) break;
		else	digit++;
	}

	r_tmp = (long)r_num;
		
	if(n_dec == 0)
	{
		for(j = 0; j < digit; j++)
		{
			*(num_arr + index + digit - j - 1) = deci2asc(r_tmp%10);
			r_tmp/=10;
		}
	}
	else
	{
		if(digit < n_dec + 1) digit = n_dec + 1;
		
		for(j = 0; j < digit; j++)
		{
			if(j == n_dec) 
			{
				*(num_arr + index + digit - j) = '.';
				index--;
			}
			*(num_arr + index + digit - j) = deci2asc(r_tmp%10);
			r_tmp/=10;
		}
	}
	write_txbufc_string(num_arr);
}

//---------------------------------------------------------------------------------------------------------------
void Display_digit100(WORD Dis_digit)
{
	Uint16 temp = 0;
	Uint16 temp1 = 0;

	temp = Dis_digit / 100;
	Dis_digit = Dis_digit % 100;

	if(temp !=0)
	{
		temp1 = deci2asc(temp);
		write_txbuf_asc(temp1);
	}

	temp = Dis_digit / 10;
	temp1 = deci2asc(temp);

	write_txbuf_asc(temp1);

	Dis_digit = Dis_digit % 10;
	temp1 = deci2asc(Dis_digit);

	write_txbuf_asc(temp1);
}

//---------------------------------------------------------------------------------------------------------------
void Display_digit1000(WORD Dis_digit)
{
	Uint16 temp = 0;
	Uint16 temp1 = 0;

	temp = Dis_digit / 1000;
	Dis_digit = Dis_digit % 1000;

	if(temp != 0)
	{
		temp1 = deci2asc(temp);
		write_txbuf_asc(temp1);
	}

	temp = Dis_digit / 100;
	Dis_digit = Dis_digit % 100;

	temp1 = deci2asc(temp);
	write_txbuf_asc(temp1);

	temp = Dis_digit / 10;
	temp1 = deci2asc(temp);

	write_txbuf_asc(temp1);

	Dis_digit = Dis_digit % 10;
	temp1 = deci2asc(Dis_digit);

	write_txbuf_asc(temp1);
}

//---------------------------------------------------------------------------------------------------------------
void Display_Dot_digit100(WORD Dot_digit)
{
	Uint16 temp = 0;
	Uint16 temp1 = 0;

	temp = Dot_digit / 100;
	Dot_digit = Dot_digit % 100;

	if(temp != 0)
	{
		temp1 = deci2asc(temp);
		write_txbuf_asc(temp1);
	}
	temp = Dot_digit / 10;
	temp1 = deci2asc(temp);

	write_txbuf_asc(temp1);
	write_txbuf_asc('.');

	Dot_digit = Dot_digit % 10;
	temp1 = deci2asc(Dot_digit);

	write_txbuf_asc(temp1);
}

//---------------------------------------------------------------------------------------------------------------
void Display_Dot_digit1000(WORD Dot_digit)
{
	Uint16 temp  = 0;
	Uint16 temp1 = 0;

	temp = Dot_digit / 1000;
	Dot_digit = Dot_digit % 1000;

	if(temp != 0)
	{
		temp1 = deci2asc(temp);
		write_txbuf_asc(temp1);
	}

	temp = Dot_digit / 100;
	Dot_digit = Dot_digit % 100;
	temp1 = deci2asc(temp);
	write_txbuf_asc(temp1);

	temp = Dot_digit / 10;
	temp1 = deci2asc(temp);

	write_txbuf_asc(temp1);
	write_txbuf_asc('.');

	Dot_digit = Dot_digit % 10;
	temp1 = deci2asc(Dot_digit);

	write_txbuf_asc(temp1);
}

//---------------------------------------------------------------------------------------------------------------
unsigned int hex2decimal(unsigned int hex_t)
{
	unsigned int hex = 0;
	unsigned int dec = 0;
	unsigned int a = 0;
	unsigned int b = 0;
	unsigned int c = 0;
	unsigned int d = 0;

	hex = hex_t; // stored value

	a = (  hex / 1000);
	b = (( hex % 1000) / 100);
	c = (((hex % 1000) % 100) / 10);
	d = (((hex % 1000) % 100) % 10);

	dec = a*4096 + b*256 + c*16 + d*1; // conversion value

	return(dec);
}

void RS422_TX()
{
	if(SciaRegs.SCICTL2.bit.TXRDY)
	{
		if(debug_tx_pos != debug_tx_end)
		{
			SciaRegs.SCITXBUF = debug_tx_buf[debug_tx_pos++];
			if(debug_tx_pos >= DEBUG_BUF_SIZE - 1)	debug_tx_pos = 0;
		}
	}
}

void RS422_TXC()
{
	if(ScicRegs.SCICTL2.bit.TXRDY)
	{
		if(debug_txc_pos != debug_txc_end)
		{
			ScicRegs.SCITXBUF = debug_txc_buf[debug_txc_pos++];
			if(debug_txc_pos >= DEBUG_BUF_SIZE - 1)	debug_txc_pos = 0;
		}
	}
}

void Cmd_Ready_Message()
{
	write_txbuf_asc(LF);
	write_txbuf_asc(CR);
	write_txbuf_asc('>');
}

void Init_array()
{
	Uint16 i = 0;
	
	for(i = 0; i < DEBUG_BUF_SIZE; i++)
	{
		debug_tx_buf[i] = 0;
		debug_txc_buf[i] = 0;
		debug_cmd_rx[i] = 0;
		debug_cmd_rxc[i] = 0;
	}
}

void Monitoring_RS422(void)
{
	RS422_TX();

	if(UART_INTERRUPT != 1) 
		return;
	else
	{
		UART_INTERRUPT = 0;
		Debug_CMD(rxd);
	}
}


void Monitoring_RS422C(void)
{
/*
	RS422_TXC();

	if(UART_INTERRUPT_C != 1) 
		return;
	else
	{
		UART_INTERRUPT_C = 0;
		Debug_CMD_C(rxd_c);
	}
	*/
}




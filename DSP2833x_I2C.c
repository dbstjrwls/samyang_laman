
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "DSP2833x_28335_Define.h"
#include "Main.h"
#include "Adc.h"
#include "Can.h"
#include "Ctrl.h"
#include "IO.h"
#include "Pie.h"
#include "Uart.h"
//

#define TIME_US		2000

unsigned char tmp_total_num[4] = {0, 0, 0, 0};
unsigned long tmp_total_data = 0;

unsigned char read_total_num[4] = {0, 0, 0, 0};
unsigned long read_total_data = 0;

unsigned char ad5627r_buffer[3] = {0, 0, 0};

unsigned int i2c_timeout = 0;

void I2CA_Init(void)
{
   // Initialize I2C
   I2caRegs.I2CSAR = 0x000E;		// Slave address
   I2caRegs.I2CPSC.all = 14;   		// Prescaler - need 7-12 Mhz on module clk (150/15 = 10MHz)

   I2caRegs.I2CCLKL = 10;			// NOTE: must be non zero
   I2caRegs.I2CCLKH = 5;			// NOTE: must be non zero
   I2caRegs.I2CIER.all = 0x24;		// Enable SCD & ARDY interrupts

   I2caRegs.I2CMDR.all = 0x0020;	// Take I2C out of reset
   									// Stop I2C when suspended
   I2caRegs.I2CFFTX.all = 0x6000;	// Enable FIFO mode and TXFIFO
   I2caRegs.I2CFFRX.all = 0x2040;	// Enable RXFIFO, clear RXFFINT,

   return;
}

void InitI2CGpio()
{
   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.
	GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;	   // Enable pull-up for GPIO25 (WP)
	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

/* Configure SCI pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be I2C functional pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;
	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // Configure GPIO32 for SDAA operation
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // Configure GPIO33 for SCLA operation

    EDIS;
}

void I2C_Release()
{
	if(i2c_error == 0)
	{
		I2caRegs.I2CMDR.bit.IRS = 0; 			// Reset/Disable
		i2c_error = 1;

		EALLOW;
		GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;	// Configure GPIO33
		GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1;		// Output
		EDIS;
		GpioDataRegs.GPBCLEAR.bit.GPIO33 = TRUE;
		delay_ms(1);
		GpioDataRegs.GPBSET.bit.GPIO33 = TRUE;

		InitI2CGpio();
		I2CA_Init();
	}
}

unsigned int Write_AD5627_Data(unsigned char addr, unsigned int data)	// command, data
{
	unsigned int slave_addr;
	unsigned char data_addr;

	if(addr >= 0xff) addr = 0;
//	slave_addr = 0x50 | ((addr >> 8) & 0x7);
//	slave_addr = I2C_SLAVE_ADDR | ((addr >> 8) & 0x7);
	slave_addr = I2C_AD5627_ADDR;	// Slave Address

	data_addr = addr & 0xff;		// Data Address

	// Wait until the STP bit is cleared from any previous master communication.
	// I2C 모듈 카운터가 0으로 될 때까지, 기다린다.
	i2c_timeout = 0;
	while(I2caRegs.I2CMDR.bit.STP == 1)
	{
		if(I2caRegs.I2CMDR.bit.STP == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			return(0);
		}
	}
	// Wait until bus-free status
	// BB 비트를 '1'로하여 BUSY 상태를 Clear
	i2c_timeout = 0;
	while(I2caRegs.I2CSTR.bit.BB == 1)
	{
		if(I2caRegs.I2CSTR.bit.BB == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			return(0);
		}
	}
	// Set Device(Slave) Address
	I2caRegs.I2CSAR = slave_addr;

	// Setup number of bytes to send
//	I2caRegs.I2CCNT = 2;
	I2caRegs.I2CCNT = 3;

	// Setup data to send
	I2caRegs.I2CDXR = data_addr & 0xff;		// Data Address
	I2caRegs.I2CDXR = (data >> 4) & 0xff;	// Write Data-1
	I2caRegs.I2CDXR = data & 0xf;		 	// Write Data-2

	// Send start as master transmitter with STT(=1), STP(=1), XA(=0), RM(=0)
	// I2C-Bus의 START 신호
	// 내부 I2C 카운터가 '0'일 때, DSP로 STOP 신호 전송
	// 7-bit address 모드, I2CCNT 레지스터로 보낼 데이터 수를 결정
	I2caRegs.I2CMDR.all = 0x6E20;

	// Wait until STOP condition is detected and clear STOP condition bit
	i2c_timeout = 0;
	while(I2caRegs.I2CSTR.bit.SCD == 0)	// 전송이 끝날때 까지 기다림
	{
		if(I2caRegs.I2CSTR.bit.SCD == 0)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			return(0);
		}
	}
	I2caRegs.I2CSTR.bit.SCD = 1;			// SCD에 '1'을 써서 SCD flag를 Clear

	delay_ms(1);
	return(1);
}

unsigned int Read_AD5627_Data(unsigned int cmd_t, unsigned int data_t)
{
	unsigned int slave_addr, data_addr;
	unsigned char data = 0;
	unsigned char i = 0;

//	slave_addr = 0x50 | ((addr >> 8) & 0x7);
//	slave_addr = I2C_SLAVE_ADDR | ((addr >> 8) & 0x7);
	slave_addr = I2C_AD5627_ADDR;
	data_addr = cmd_t & 0xffff;
	data = data_t;

	// Wait until the STP bit is cleared from any previous master communication.
	i2c_timeout = 0;
	while(I2caRegs.I2CMDR.bit.STP == 1)
	{
		if(I2caRegs.I2CMDR.bit.STP == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}
	// Wait until bus-free status
	i2c_timeout = 0;
	while(I2caRegs.I2CSTR.bit.BB == 1)
	{
		if(I2caRegs.I2CSTR.bit.BB == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}
	// Set Device(Slave) Address
	I2caRegs.I2CSAR = slave_addr;

	// Setup number of bytes to send
	I2caRegs.I2CCNT = 3;

// Send start as master transmitter with STT(=1), STP(=0), XA(=0), RM(=0)
	I2caRegs.I2CMDR.all = 0x2620;

	// Setup data to send
	I2caRegs.I2CDXR = data_addr & 0xff;		// Data Address, CMD-Byte
	I2caRegs.I2CDXR = (data >> 4) & 0xff;	// Write Data-1, MSB(8-Bit)
	I2caRegs.I2CDXR = data & 0xf;		 	// Write Data-2, LSB(4-Bit)
/*
	// Send start as master transmitter with STT(=1), STP(=0), XA(=0), RM(=0)
	I2caRegs.I2CMDR.all = 0x2620;
*/
	// Wait until ARDY status bit is set
	i2c_timeout = 0;
	while(I2caRegs.I2CSTR.bit.ARDY == 0)
	{
		if(I2caRegs.I2CSTR.bit.ARDY == 0)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}
	// Wait until the STP bit is cleared
	i2c_timeout = 0;
	while(I2caRegs.I2CMDR.bit.STP == 1)
	{
		if(I2caRegs.I2CMDR.bit.STP == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}

	// Set Device(Slave) Address
	I2caRegs.I2CSAR = slave_addr;

	// Setup number of bytes to read
	I2caRegs.I2CCNT = 3;

	// Send start as master receiver with STT(=1), STP(=1), XA(=0), RM(=0)
	I2caRegs.I2CMDR.all = 0x2C20;

	// Wait until STOP condition is detected and clear STOP condition bit

	i2c_timeout = 0;
	while(I2caRegs.I2CSTR.bit.SCD == 0)
	{
		if(I2caRegs.I2CSTR.bit.SCD == 0)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}
	I2caRegs.I2CSTR.bit.SCD = 1;
	//data = (WORD)(I2caRegs.I2CDRR & 0xffff);

	for(i = 0; i < 3; i++)
	{
		ad5627r_buffer[i] = (BYTE)(I2caRegs.I2CDRR & 0xff);
	}

	i2c_error = 1;
	delay_ms(1);
	return(data);
}

unsigned int Write_EEPROM_Data(unsigned int addr, unsigned char data)
{
	unsigned int slave_addr, data_addr;

	if(addr >= 0xffff) addr = 0;
//	slave_addr = 0x50 | ((addr >> 8) & 0x7);
//	slave_addr = I2C_SLAVE_ADDR | ((addr >> 8) & 0x7);
	slave_addr = I2C_SLAVE_ADDR;	// Slave Address

	data_addr = addr & 0xffff;		// Data Address

	// Wait until the STP bit is cleared from any previous master communication.
	// I2C 모듈 카운터가 0으로 될 때까지, 기다린다.
	i2c_timeout = 0;
	while(I2caRegs.I2CMDR.bit.STP == 1)
	{
		if(I2caRegs.I2CMDR.bit.STP == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			return(0);
		}
	}
	// Wait until bus-free status
	// BB 비트를 '1'曠臼?BUSY 상태를 Clear
	i2c_timeout = 0;
	while(I2caRegs.I2CSTR.bit.BB == 1)
	{
		if(I2caRegs.I2CSTR.bit.BB == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			return(0);
		}
	}
	// Set Device(Slave) Address
	I2caRegs.I2CSAR = slave_addr;

	// Setup number of bytes to send
//	I2caRegs.I2CCNT = 2;
	I2caRegs.I2CCNT = 3;

	// Setup data to send
	I2caRegs.I2CDXR = (data_addr >> 8) & 0xff; 	// Data Address
	I2caRegs.I2CDXR = data_addr & 0xff;		 	// Write Data
	I2caRegs.I2CDXR = data & 0xff;		 		// Write Data

	// Send start as master transmitter with STT(=1), STP(=1), XA(=0), RM(=0)
	// I2C-Bus의 START 신호
	// 내부 I2C 카운터가 '0'일 때, DSP로 STOP 신호 전송
	// 7-bit address 모드, I2CCNT 레지스터로 보낼 데이터 수를 결정
	I2caRegs.I2CMDR.all = 0x6E20;

	// Wait until STOP condition is detected and clear STOP condition bit
	i2c_timeout = 0;
	while(I2caRegs.I2CSTR.bit.SCD == 0);	// 전송이 끝날때 까지 기다림
	{
		if(I2caRegs.I2CSTR.bit.SCD == 0)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			return(0);
		}
	}
	I2caRegs.I2CSTR.bit.SCD = 1;			// SCD에 '1'을 써서 SCD flag를 Clear
	// Wait the Write-Cycle Time for EEPROM
	delay_ms(5);
	return(1);
}

unsigned char Read_EEPROM_Data(unsigned int addr)
{
	unsigned int slave_addr, data_addr;
	unsigned char data = 0;

//	slave_addr = 0x50 | ((addr >> 8) & 0x7);
//	slave_addr = I2C_SLAVE_ADDR | ((addr >> 8) & 0x7);
	slave_addr = I2C_SLAVE_ADDR;
	data_addr = addr & 0xffff;

	// Wait until the STP bit is cleared from any previous master communication.
	i2c_timeout = 0;
	while(I2caRegs.I2CMDR.bit.STP == 1)
	{
		if(I2caRegs.I2CMDR.bit.STP == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}
	// Wait until bus-free status
	while(I2caRegs.I2CSTR.bit.BB == 1)
	{
		if(I2caRegs.I2CSTR.bit.BB == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}
	// Set Device(Slave) Address
	I2caRegs.I2CSAR = slave_addr;

	// Setup number of bytes to send
	I2caRegs.I2CCNT = 2;

// Send start as master transmitter with STT(=1), STP(=0), XA(=0), RM(=0)
	I2caRegs.I2CMDR.all = 0x2620;

	// Setup data to send
//	I2caRegs.I2CDXR = data_addr; 				// Data Address
	I2caRegs.I2CDXR = (data_addr >> 8) & 0xff; 	// Data Address
	I2caRegs.I2CDXR = data_addr & 0xff;		 	// Write Data
/*
	// Send start as master transmitter with STT(=1), STP(=0), XA(=0), RM(=0)
	I2caRegs.I2CMDR.all = 0x2620;
*/
	// Wait until ARDY status bit is set
	i2c_timeout = 0;
	while(I2caRegs.I2CSTR.bit.ARDY == 0)
	{
		if(I2caRegs.I2CSTR.bit.ARDY == 0)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}
	i2c_timeout = 0;
	// Wait until the STP bit is cleared
	while(I2caRegs.I2CMDR.bit.STP == 1)
	{
		if(I2caRegs.I2CMDR.bit.STP == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}
	// Set Device(Slave) Address
	I2caRegs.I2CSAR = slave_addr;

	// Setup number of bytes to read
	I2caRegs.I2CCNT = 1;

	// Send start as master receiver with STT(=1), STP(=1), XA(=0), RM(=0)
	I2caRegs.I2CMDR.all = 0x2C20;

	// Wait until STOP condition is detected and clear STOP condition bit
	i2c_timeout = 0;
	while(I2caRegs.I2CSTR.bit.SCD == 0)
	{
		if(I2caRegs.I2CSTR.bit.SCD == 1)
			i2c_timeout++;
		if(i2c_timeout >= TIME_US)
		{
			i2c_timeout = 0;
			i2c_error = 0;
			return(0);
		}
	}
	I2caRegs.I2CSTR.bit.SCD = 1;

	data = (BYTE)(I2caRegs.I2CDRR & 0xff);

	i2c_error = 1;
	return(data);
}

void Write_float32(unsigned int index, float data32)
{
	unsigned char data_tmp[2];
	unsigned char data[4];

	data_tmp[0] = *(unsigned char  *)&data32;
	data_tmp[1] = *((unsigned char *)&data32 + 1);

	data[0] = data_tmp[0] & 0xff;
	data[1] = (data_tmp[0] >> 8) & 0xff;
	data[2] = data_tmp[1] & 0xff;
	data[3] = (data_tmp[1] >> 8) & 0xff;

	Write_EEPROM_Data(4 * index, data[0]);
	I2C_Release();
	Write_EEPROM_Data(4 * index + 1, data[1]);
	I2C_Release();
	Write_EEPROM_Data(4 * index + 2, data[2]);
	I2C_Release();
	Write_EEPROM_Data(4 * index + 3, data[3]);
	I2C_Release();
}

float Read_float32(unsigned int index)
{
	float data32;
//	unsigned long data32_tmp;
	unsigned char data_char[4];
	unsigned char data16[2];

	data_char[0] = Read_EEPROM_Data(4 * index);
	I2C_Release();
	data_char[1] = Read_EEPROM_Data(4 * index + 1);
	I2C_Release();
	data_char[2] = Read_EEPROM_Data(4 * index + 2);
	I2C_Release();
	data_char[3] = Read_EEPROM_Data(4 * index + 3);
	I2C_Release();

	data16[0] =	((data_char[1] & 0xff) << 8);
	data16[0] +=  data_char[0];
	data16[1] =	((data_char[3] & 0xff) << 8);
	data16[1] +=  data_char[2];

	data32 = *(float *)&data16;

	return data32;
}

void Write_Uint32(unsigned int index, Uint32 data)
{
	tmp_total_data = data;

	tmp_total_num[0] = (data >> 24) & 0xff;
	tmp_total_num[1] = (data >> 16) & 0xff;
	tmp_total_num[2] = (data >>  8) & 0xff;
	tmp_total_num[3] =  data & 0xff;

	Write_EEPROM_Data(4 * index,     tmp_total_num[0]);
	I2C_Release();
	Write_EEPROM_Data(4 * index + 1, tmp_total_num[1]);
	I2C_Release();
	Write_EEPROM_Data(4 * index + 2, tmp_total_num[2]);
	I2C_Release();
	Write_EEPROM_Data(4 * index + 3, tmp_total_num[3]);
	I2C_Release();
}

Uint32 Read_Uint32(unsigned int index)
{
	Uint32 data32 = 0;
	read_total_data = 0;

	read_total_num[0] = Read_EEPROM_Data(4 * index);
	I2C_Release();
	read_total_num[1] = Read_EEPROM_Data(4 * index + 1);
	I2C_Release();
	read_total_num[2] = Read_EEPROM_Data(4 * index + 2);
	I2C_Release();
	read_total_num[3] = Read_EEPROM_Data(4 * index + 3);
	I2C_Release();

	read_total_data |=  read_total_num[3] & 0xFF;
	read_total_data |=  read_total_num[2] <<  8;

	data32 = read_total_num[1];
	read_total_data |=  data32 << 16;

	data32 = 0;
	data32 = read_total_num[0];
	read_total_data |=  data32 << 24;

	return read_total_data;
}

void Write_Uint16(unsigned int index, Uint16 data)
{
	unsigned char tmp[2];

	tmp[0] = (data >> 8) & 0xff;
	tmp[1] = data & 0xff;

	Write_EEPROM_Data(4 * index,tmp[0]);
	I2C_Release();
	Write_EEPROM_Data(4 * index+1, tmp[1]);
	I2C_Release();
}

Uint16 Read_Uint16(unsigned int index)
{
	Uint16 data16;
	unsigned char data_char[2];

	data_char[0] = Read_EEPROM_Data(4*index);
	I2C_Release();
	data_char[1] = Read_EEPROM_Data(4*index + 1);
	I2C_Release();

	data16 = data_char[1];
	data16 += (data_char[0] << 8);

	return data16;
}


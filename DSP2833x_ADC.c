
#include "DSP2833x_Device.h"     	// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   	// DSP2833x Examples Include File
#include "DSP2833x_28335_Define.h"

#include "Main.h"
#include "Can.h"
#include "Ctrl.h"
#include "IO.h"
#include "Pie.h"
#include "Uart.h"
#include "I2c.h"

WORD ADC[ADCH];
BOOL ADC_COMPLETE = 0;
char TEST_TIMER_Start = 0;
long ADC_COUNT = 0;

float AD_FILAMT_buf = 0.0f;
float AD_FILAMT_data = 0.0f;
float AD_FILAMT_buf_old = 0.0f;
float AD_FILAMT_data_old = 0.0f;

float AD_GASPRE_buf = 0.0f;
float AD_GASPRE_data = 0.0f;
float AD_GASPRE_buf_old = 0.0f;
float AD_GASPRE_data_old = 0.0f;

float AD_ENERGY_buf = 0.0f;
float AD_ENERGY_data = 0.0f;
float AD_ENERGY_buf_old = 0.0f;
float AD_ENERGY_data_old = 0.0f;

float AD_HVPEAK_buf = 0.0f;
float AD_HVPEAK_data = 0.0f;
float AD_HVPEAK_buf_old = 0.0f;
float AD_HVPEAK_data_old = 0.0f;

float AD_ENMON_buf = 0.0f;
float AD_ENMON_data = 0.0f;
float AD_ENMON_buf_old = 0.0f;
float AD_ENMON_data_old = 0.0f;

float AD_TEMP_buf = 0.0f;
float AD_TEMP_data = 0.0f;
float AD_TEMP_buf_old = 0.0f;
float AD_TEMP_data_old = 0.0f;

float AD_PEAK_buf = 0.0f;
float AD_PEAK_data = 0.0f;
float AD_PEAK_buf_old = 0.0f;
float AD_PEAK_data_old = 0.0f;

float AD_CONTROLTH_buf = 0.0f;
float AD_CONTROLTH_data = 0.0f;
float AD_CONTROLTH_buf_old = 0.0f;
float AD_CONTROLTH_data_old = 0.0f;

BOOL peak_trigger_flag = 0;
BOOL sync_trigger_flag = 0;

float Mean_Peak = 0.0f;
float Mean_Peak_t = 0.0f;

void ADC_START(void)
{
	AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
}

float Logarithm(float x)
{
	int y = 0;
	float temp = 0.0f;

	while((x > 1.34) || (x < 0.65))
	{
		if(x < 0.65)
		{
			x*=2;
			y--;
		}
		else if(x > 1.34)
		{
			x/=2;
			y++;
		}
	}
	temp = x - 1.0f + (y * LN2);
	return temp;
}

float Temper_ADC(unsigned short adc_val, char sensor_num)
{
	float tmp = 0.0f;

	if (adc_val <= ADCLO) adc_val = 0;
	else adc_val = adc_val - ADCLO;

	tmp = ((float)adc_val) * (3.0/4096.0);					// adc digital value to Voltage
	tmp = (R_FACTOR * BIAS_V * 1000.0 / tmp) - 1000.0;	 	// Voltage to NTC R-value
	tmp = tmp / 5000.0;

	if(sensor_num == 0)
	{
		tmp = (1.0 / T_25) + Logarithm(tmp) / B_VALUE;		// find 1/T
	}
	else if(sensor_num == 1)
	{
		tmp = (1.0 / T_25) + Logarithm(tmp) / B_VALUE_IGBT;	// find 1/T
	}
	tmp = 1.0 / tmp - 273.15;

	return tmp;
}

void LPF_H(float *Input, float *Output, float *PastInput, float *PastOutput, float limit)
{
	if(limit < 0.0f)	limit *= -1.0f;

	if(*Input > (*PastOutput) + limit)
	{
		*Input = (*PastOutput) + limit;
	}
	else if(*Input < (*PastOutput) - limit)
	{
		*Input = (*PastOutput) - limit;
	}
	*Output = Betha_H * ((*Input) + (*PastInput)) + Alpha_H * (*PastOutput);
	*PastOutput = *Output;
	*PastInput 	= *Input;
}

void LPF_L(float *Input, float *Output, float *PastInput, float *PastOutput, float limit)
{
	if(limit < 0.0f)	limit *= -1.0f;

	if(*Input > (*PastOutput) + limit)
	{
		*Input = (*PastOutput) + limit;
	}
	else if(*Input < (*PastOutput) - limit)
	{
		*Input = (*PastOutput) - limit;
	}
	*Output = Betha_L * ((*Input) + (*PastInput)) + Alpha_L * (*PastOutput);
	*PastOutput = *Output;
	*PastInput = *Input;
}

void LPF_D(double *Input, double *Output, double *PastInput, double *PastOutput)
{
	*Output = Betha_D * ((*Input) + (*PastInput)) + Alpha_D * (*PastOutput);
	*PastOutput = *Output;
	*PastInput = *Input;
}

void ADC_Conversion()       // monitoring에서만 사용.
{
	// ADC Channel : 0, LASER_FILAMT
	AD_FILAMT_buf = ((float)ADC[LASER_FILAMT] - (float)ADCLO);
	AD_FILAMT_buf = (((AD_FILAMT_buf / 4096) * 3.0)) * 1.667;
	LPF_H(&AD_FILAMT_buf, &AD_FILAMT_data, &AD_FILAMT_buf_old, &AD_FILAMT_data_old, 5.0f);

	// ADC Channel : 1, LASER_GASPRE
	AD_GASPRE_buf = ((float)ADC[LASER_GASPRE] - (float)ADCLO);
	AD_GASPRE_buf = (((AD_GASPRE_buf / 4096) * 3.0)) * 1.667;
//	LPF_H(&AD_GASPRE_buf, &AD_GASPRE_data, &AD_GASPRE_buf_old, &AD_GASPRE_data_old, 5.0f);
	LPF_H(&AD_GASPRE_buf, &AD_GASPRE_data, &AD_GASPRE_buf_old, &AD_GASPRE_data_old, 0.1f);

	// ADC Channel : 2, LASER_ENERGY
	AD_ENERGY_buf = ((float)ADC[LASER_ENERGY] - (float)ADCLO);
	AD_ENERGY_buf = (((AD_ENERGY_buf / 4096) * 3.0)) * 1.667;
	LPF_H(&AD_ENERGY_buf, &AD_ENERGY_data, &AD_ENERGY_buf_old, &AD_ENERGY_data_old, 5.0f);

	// ADC Channel : 3, LASER_HVPEAK
	AD_HVPEAK_buf = ((float)ADC[LASER_HVPEAK] - (float)ADCLO);
	AD_HVPEAK_buf = (((AD_HVPEAK_buf / 4096) * 3.0)) * 1.667;
	LPF_H(&AD_HVPEAK_buf, &AD_HVPEAK_data, &AD_HVPEAK_buf_old, &AD_HVPEAK_data_old, 5.0f);

	// ADC Channel : 4, LASER_ENMON
	AD_ENMON_buf = ((float)ADC[LASER_ENMON] - (float)ADCLO);
	AD_ENMON_buf = (((AD_ENMON_buf / 4096) * 3.0)) * 1.667;
	LPF_H(&AD_ENMON_buf, &AD_ENMON_data, &AD_ENMON_buf_old, &AD_ENMON_data_old, 5.0f);

	// ADC Channel : 5, LASER_TEMP
	AD_TEMP_buf = ((float)ADC[LASER_TEMP] - (float)ADCLO);
	AD_TEMP_buf = (((AD_TEMP_buf / 4096) * 3.0)) * 1.667;
	LPF_H(&AD_TEMP_buf, &AD_TEMP_data, &AD_TEMP_buf_old, &AD_TEMP_data_old, 5.0f);

	// ADC Channel : 6, CONTROL_TH
	AD_CONTROLTH_buf = DIG2VOLT * ((float)ADC[CTRL_TEMP] - (float)ADCLO);
	AD_CONTROLTH_buf = (AD_CONTROLTH_buf - 424.0f) / 6.25f;		// LM60 °ü°è½Ä V (mV) = 6.25 * T(C) + 424mV
	LPF_H(&AD_CONTROLTH_buf, &AD_CONTROLTH_data, &AD_CONTROLTH_buf_old, &AD_CONTROLTH_data_old, 100.f);

	// ADC Channel : 7, PEAK_VALUE
	AD_PEAK_buf = ((float)ADC[PEAK_VALUE] - (float)ADCLO);
	AD_PEAK_buf = ((AD_PEAK_buf / 4096) * 3.0);
	LPF_H(&AD_PEAK_buf, &AD_PEAK_data, &AD_PEAK_buf_old, &AD_PEAK_data_old, 3.0f);
}




extern void ADC_START(void);
extern float Temper_ADC(unsigned short adc_val, char sensor_num);
extern float Logarithm(float x);
extern void LPF_H(float *Input, float *Output, float *PastInput, float *PastOutput, float limit);
extern void LPF_L(float *Input, float *Output, float *PastInput, float *PastOutput, float limit);
extern void LPF_D(double *Input, double *Output, double *PastInput, double *PastOutput);
extern void ADC_Conversion();

extern WORD ADC[ADCH];
extern BOOL ADC_COMPLETE;
extern char TEST_TIMER_Start;
extern long ADC_COUNT;

extern float AD_FILAMT_buf;
extern float AD_FILAMT_data;
extern float AD_FILAMT_buf_old;
extern float AD_FILAMT_data_old;

extern float AD_GASPRE_buf;
extern float AD_GASPRE_data;
extern float AD_GASPRE_buf_oldf;
extern float AD_GASPRE_data_old;

extern float AD_ENERGY_buf;
extern float AD_ENERGY_data;
extern float AD_ENERGY_buf_old;
extern float AD_ENERGY_data_old;

extern float AD_HVPEAK_buf;
extern float AD_HVPEAK_data;
extern float AD_HVPEAK_buf_old;
extern float AD_HVPEAK_data_old;

extern float AD_ENMON_buf;
extern float AD_ENMON_data;
extern float AD_ENMON_buf_old;
extern float AD_ENMON_data_old;

extern float AD_TEMP_buf;
extern float AD_TEMP_data;
extern float AD_TEMP_buf_old;
extern float AD_TEMP_data_old;

extern float AD_PEAK_buf;
extern float AD_PEAK_data;
extern float AD_PEAK_buf_old;
extern float AD_PEAK_data_old;

extern float AD_CONTROLTH_buf;
extern float AD_CONTROLTH_data;
extern float AD_CONTROLTH_buf_old;
extern float AD_CONTROLTH_data_old;

extern BOOL peak_trigger_flag;
extern BOOL sync_trigger_flag;

extern float Mean_Peak;
extern float Mean_Peak_t;


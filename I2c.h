
extern void I2CA_Init(void);
extern void InitI2CGpio();
extern void I2C_Release();

extern unsigned int Write_AD5627_Data(unsigned char addr, unsigned int data);
extern unsigned int Read_AD5627_Data(unsigned int cmd_t, unsigned int data_t);

extern unsigned int Write_EEPROM_Data(unsigned int addr, unsigned char data);
extern unsigned char Read_EEPROM_Data(unsigned int addr);

extern void Write_float32(unsigned int index, float data32);
extern float Read_float32(unsigned int index);

extern void Write_Uint32(unsigned int index, Uint32 data);
extern Uint32 Read_Uint32(unsigned int index);

extern void Write_Uint16(unsigned int index, Uint16 data);
extern Uint16 Read_Uint16(unsigned int index);

extern unsigned char tmp_total_num[4];
extern unsigned long tmp_total_data;

extern unsigned char read_total_num[4];
extern unsigned long read_total_data;

extern unsigned char ad5627r_buffer[3];

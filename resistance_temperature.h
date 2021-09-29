
#ifndef RESISTANCE_TEMPERATURE_H
#define RESISTANCE_TEMPERATURE_H

//Resistance temperature comparison struct
typedef struct TempResistance
{
	int  temperature; //temperature value
	int  lower;//Resistance range lower
	int  upper;//Resistance range upper
}TempResistance_T;

inline int calculate_temperature_offset(int resistance, int lower, int upper);
int get_temperature(int resistance, TempResistance_T* pTR, int count, int start, int end, void* pTemp);
int pt1000_get_temperature(int resistance, void* pTemp);
void test_get_temp();

#endif//RESISTANCE_TEMPERATURE_H
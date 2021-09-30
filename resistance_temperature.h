
#ifndef RESISTANCE_TEMPERATURE_H
#define RESISTANCE_TEMPERATURE_H

#define GET_TEMP_TEST 1

//Resistance temperature comparison struct
typedef struct TempResistance
{
	int  temperature; //temperature value
	int  lower;//Resistance range lower
	int  upper;//Resistance range upper
}TempResistance_T;

#define FIND_RANGE 10

#define PT1000_LOW_LIMIT_RESISTANCE   8030 //-50 C
#define PT1000_LOW_LIMIT_TEMPERATURE  -50

#define PT1000_HIGH_LIMIT_RESISTANCE  14606 //120 C
#define PT1000_HIGH_LIMIT_TEMPERATURE  120

inline int calculate_temperature_offset(int resistance, int lower, int upper);
int get_temperature(int resistance, TempResistance_T* pTR, int count, int start, int end, void* pTemp);
inline int pt1000_check_limit(int resistance, void* pTemp);
int pt1000_get_temperature(int resistance, void* pTemp);

#if GET_TEMP_TEST
void test_get_temp();
#endif

#endif//RESISTANCE_TEMPERATURE_H

#ifndef RESISTANCE_TEMPERATURE_C
#define RESISTANCE_TEMPERATURE_C

#include "resistance_temperature.h"
#include "resistance_temperature_table.def"

#define PT1000_TEMP_RESISTANCE_COUNT  (sizeof(pt1000_temp_resistance)/sizeof(TempResistance_T))

inline int calculate_temperature_offset(int resistance, int lower, int upper)
{
	int distance = (resistance - lower)*10;
	int step = (upper - lower) ;
	if (distance == 0 || distance < step||step==0) return 0;
	return distance / step;
}

int check_and_set_temp(int resistance, TempResistance_T* pTR, int i, void* pTemp) //i:index of resistance_temperature_table 
{
	int offset;
	char* p = (char*)pTemp;
	//case 1:
	if ((i - 1) >= 0 && resistance < pTR[i].lower && resistance > pTR[i - 1].upper)
	{
		p[0] = (char)pTR[i].temperature;
		p[1] = 0;
		return 1;
	}
	//case 2:
	if (resistance >= pTR[i].lower && resistance <= pTR[i].upper)
	{
		p[0] = (char)pTR[i].temperature;
		offset = calculate_temperature_offset(resistance, pTR[i].lower, pTR[i].upper);

		if (pTR[i].temperature >= 100)
		{
			p[0] += offset;
			p[1] = 0;
		}
		else
		{
			p[0] += (offset / 10);
			p[1] = (offset % 10);
		}

		return 1;
	}
	return 0;
}
int get_temperature(int resistance, TempResistance_T * pTR,int count,int start, int end,void * pTemp,int direction)
{
	int i;

	if (start < 0) start = 0; //check start index
	if (end > count) end = count;//check end
	//printf("direction=%d\n", direction);//test ok
	if (direction == 0) //search from start to end
	{
		for (i = start; i < count && i < end; i++)
		{
			if (1 == check_and_set_temp(resistance, pTR, i, pTemp)) break;
		}
		if (i >= count || i >= end) return -1;//not find
	}
	else  //search from end-1 to start
	{
		for (i = end-1; i >=0 && i >= start; i--)
		{
			if (1 == check_and_set_temp(resistance, pTR, i, pTemp)) break;
		}
		if (i < 0 || i < start) return -1;//not find
	}
	return i;//return index
}

inline int pt1000_check_limit(int resistance, void* pTemp)
{
	char* p = (char*)pTemp;
	if (resistance <= PT1000_LOW_LIMIT_RESISTANCE)
	{
		p[0] = PT1000_LOW_LIMIT_TEMPERATURE;
		p[1] = 0;
		return 1;
	}

	if (resistance >= PT1000_HIGH_LIMIT_RESISTANCE)
	{
		p[0] = PT1000_HIGH_LIMIT_TEMPERATURE;
		p[1] = 0;
		return 2;
	}
	return 0;
}

int often_index = INIT_OFTEN_INDEX;//30 C

int pt1000_get_temperature(int resistance, void* pTemp)
{
	int ret_index=0, start, end;
	int count = PT1000_TEMP_RESISTANCE_COUNT;

	if (pt1000_check_limit(resistance, pTemp)) return -1;//limit temp is setting

	if (1 == check_and_set_temp(resistance, pt1000_temp_resistance, often_index, pTemp)) return often_index; //found data in often index

	if (resistance > pt1000_temp_resistance[often_index].upper)//Find UP
	{
		start = often_index + 1;
		end = count;
		ret_index = get_temperature(resistance, pt1000_temp_resistance, count, start, end, pTemp, FIND_DIRECTION_INC);
	}
	
	if (resistance < pt1000_temp_resistance[often_index].lower)//Find Down
	{
		end = often_index;
		start = 0;
		ret_index = get_temperature(resistance, pt1000_temp_resistance, count, start, end, pTemp, FIND_DIRECTION_DEC);
	}

	if (ret_index > 0&& ret_index<(count-2)) often_index = ret_index;
	//printf("often_index=%d\n", often_index);
	return ret_index; //not found;
}

#if GET_TEMP_TEST
void test_get_temp() //test ok
{
	char buffer[2];
	char data[5][2];
	int resistance = 8039;//-49.1 C
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n",(int) buffer[0],(int) buffer[1]); //output: buffer[0]=-49,buffer[1]=1

    resistance = 11832;//47.2 C
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n", (int)buffer[0], (int)buffer[1]); //output: buffer[0]=47,buffer[1]=2

	resistance = 11705;//44C
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n", (int)buffer[0], (int)buffer[1]); //output: buffer[0]=44,buffer[1]=0

	resistance = 14153;//108
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n", (int)buffer[0], (int)buffer[1]); //output: buffer[0]=108,buffer[1]=0

	resistance =8700;//-33
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n", (int)buffer[0], (int)buffer[1]); //output: buffer[0]=-33,buffer[1]=8

	resistance = 12704;//69.9
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n", (int)buffer[0], (int)buffer[1]); //output: buffer[0]=70,buffer[1]=0

	resistance = 12688;//69.5
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n", (int)buffer[0], (int)buffer[1]); //output: buffer[0]=69,buffer[1]=5

	resistance = 10000;//0
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n", (int)buffer[0], (int)buffer[1]); //output: buffer[0]=0,buffer[1]=0
	
	resistance = 14606;//high limit
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n", (int)buffer[0], (int)buffer[1]); //output: buffer[0]=120,buffer[1]=0

	resistance = 100;//low limit
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n", (int)buffer[0], (int)buffer[1]); //output: buffer[0]=-50,buffer[1]=0
	//while (1)
	{
	resistance = 8800;
	pt1000_get_temperature(resistance, (void*)data[0]);
	printf("8800:data[0][0]=%d,%d,data[0][1]=%d\n", (int)data[0][0], (unsigned char)data[0][0], (int)data[0][1]); 

	resistance = 9410;
	pt1000_get_temperature(resistance, (void*)data[1]);
	printf("9410:data[1][0]=%d,0x%02X,data[1][1]=%d\n", (int)data[1][0], (unsigned char)data[1][0], (int)data[1][1]); 
	
	resistance = 9450;
	pt1000_get_temperature(resistance, (void*)data[2]);
	printf("9450:data[2][0]=%d,%d,data[2][1]=%d\n", (int)data[2][0], (unsigned char)data[2][0], (int)data[2][1]); 

	resistance = 13760;
	pt1000_get_temperature(resistance, (void*)data[3]);
	printf("13760:data[3][0]=%d,0x%02X,data[3][1]=%d\n", (int)data[3][0], (unsigned char)data[3][0], (int)data[3][1]); 
	}
}
#endif //GET_TEMP_TEST

#endif//RESISTANCE_TEMPERATURE_C

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

int get_temperature(int resistance, TempResistance_T * pTR,int count,int start, int end,void * pTemp)
{
	int i,offset;
	char* p = (char*)pTemp;

	if (start < 0) start = 0; //check start index

	for (i = start; i < count&&i<end; i++)
	{
		//case 1:
		if ((i - 1) >= 0&&resistance < pTR[i].lower && resistance > pTR[i-1].upper)
		{
			p[0] = (char)pTR[i].temperature;
			p[1] = 0;
			break;
		}
		//case 2:
		if (resistance >= pTR[i].lower && resistance <= pTR[i].upper)
		{
			p[0] =(char) pTR[i].temperature;
			offset = calculate_temperature_offset(resistance, pTR[i].lower, pTR[i].upper);
			
			if (pTR[i].temperature >= 100)
			{
				p[0] += offset;
				p[1] = 0;
			}
			else
			{
				p[0] += (offset / 10);
				p[1] = (offset%10);
			}

			break;
		}
	}

	if (i >= count || i >= end) return -1;//not find
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

int often_index = 80;//30 C

int pt1000_get_temperature(int resistance, void* pTemp)
{
	int ret_index;
	int start;
	int end;
	int count = PT1000_TEMP_RESISTANCE_COUNT;

	if (pt1000_check_limit(resistance, pTemp)) return -1;//limit temp is setting

	if (often_index < FIND_RANGE) often_index = FIND_RANGE; //check often index
	if ((often_index + FIND_RANGE) > count) often_index = count - FIND_RANGE;

	start = often_index - FIND_RANGE;
	end = often_index + FIND_RANGE;

	ret_index =get_temperature(resistance, pt1000_temp_resistance, count, start, end, pTemp);
	if (ret_index >= 0)//Already found temp
	{
		if(ret_index>= FIND_RANGE) often_index = ret_index;
	}
	else//Not found temp ,Find up 
	{
		start = often_index + FIND_RANGE; 
		end = count;
		ret_index = get_temperature(resistance, pt1000_temp_resistance, count, start, end, pTemp);

		if (ret_index >= 0)//Already found temp up
		{
			if (ret_index >= FIND_RANGE) often_index = ret_index;
		}
		else//Not found temp ,Find Down
		{
			start = 0;
			end= often_index - FIND_RANGE;
			ret_index = get_temperature(resistance, pt1000_temp_resistance, count, start, end, pTemp);

			if (ret_index >= 0)//Already found temp up
			{
				if (ret_index >= FIND_RANGE) often_index = ret_index;
			}
			else
			{
				return -1;//not find 
			}
		}
	}

	return ret_index;
}

#if GET_TEMP_TEST
void test_get_temp() //test ok
{
	char buffer[2];
	char data[5][2];
	int resistance = 11832;//47.2 C
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n",(int) buffer[0],(int) buffer[1]); //output: buffer[0]=47,buffer[1]=2

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
	resistance = 8800;//low limit
	pt1000_get_temperature(resistance, (void*)data[0]);
	printf("8800:data[0][0]=%d,%d,data[0][1]=%d\n", (int)data[0][0], (unsigned char)data[0][0], (int)data[0][1]); //output: buffer[0]=-50,buffer[1]=0

	resistance = 9410;//low limit
	pt1000_get_temperature(resistance, (void*)data[1]);
	printf("9410:data[1][0]=%d,0x%02X,data[1][1]=%d\n", (int)data[1][0], (unsigned char)data[1][0], (int)data[1][1]); //output: buffer[0]=-50,buffer[1]=0
	
	resistance = 9450;//low limit
	pt1000_get_temperature(resistance, (void*)data[2]);
	printf("9450:data[2][0]=%d,%d,data[2][1]=%d\n", (int)data[2][0], (unsigned char)data[2][0], (int)data[2][1]); //output: buffer[0]=-50,buffer[1]=0

	resistance = 13760;//low limit
	pt1000_get_temperature(resistance, (void*)data[3]);
	printf("13760:data[3][0]=%d,0x%02X,data[3][1]=%d\n", (int)data[3][0], (unsigned char)data[3][0], (int)data[3][1]); //output: buffer[0]=-50,buffer[1]=0
	}
}
#endif //GET_TEMP_TEST

#endif//RESISTANCE_TEMPERATURE_C
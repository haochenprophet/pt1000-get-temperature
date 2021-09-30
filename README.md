# pt1000-get-temperature

Resistance temperature comparison table of PT1000 temperature sensor .
Embedded system development, obtain temperature through thermistor .

usage:

#include <stdio.h>
void main()
{
	test_get_temp();
}


	char buffer[2];

	int resistance = 11832;//47.2 C
	pt1000_get_temperature(resistance, (void*)buffer);
	printf("buffer[0]=%d,buffer[1]=%d\n",(int) buffer[0],(int) buffer[1]); //output: buffer[0]=47,buffer[1]=2
  
  

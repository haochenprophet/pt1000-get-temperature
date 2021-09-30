
typedef enum
{
	DefaultSensor = 0,
}SensorType;

typedef enum
{
	DefaultPoint = 0,
	UpPoint,
	DownPoint,
	LeftPoint,
	RightPoint,
}MeasuringPoint;

typedef struct TempDeviation
{
	int  type;//sensor type
	int  location;//Measuring point
	int  lower;//temperature range lower
	int  upper;//temperature range upper
	int  adjust; //Adjusted value
}TempDeviation_T;

TempDeviation_T temp_deviation[] = {
	{DefaultSensor,DefaultPoint,-100,-50,-3},
	{DefaultSensor,DefaultPoint,-50,-10,-2},
	{DefaultSensor,DefaultPoint,-9,40,0},
	{DefaultSensor,DefaultPoint,41,60,1},
	{DefaultSensor,DefaultPoint,61,80,2},
	{DefaultSensor,DefaultPoint,81,100,3},
	{DefaultSensor,DefaultPoint,100,150,3},
};

#define TEMP_DEVIATION_COUNT (sizeof(temp_deviation)/sizeof(TempDeviation_T))

int adjust_temperature(SensorType type, MeasuringPoint point,int temp)
{
	int i;
	for (i = 0; i < TEMP_DEVIATION_COUNT; i++)
	{
		if (temp_deviation[i].type != type) continue;
		if (temp_deviation[i].location != point) continue;
		if(temp<temp_deviation[i].lower||temp>temp_deviation[i].upper)  continue;
		temp += temp_deviation[i].adjust;
	}
	return temp;
}
/*
void temp_test()
{
	int temp = 50;
	printf("temp=%d,adjust_temperature=%d\n", temp, adjust_temperature(DefaultSensor, DefaultPoint, temp));
}
*/
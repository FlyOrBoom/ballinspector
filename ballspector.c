#pragma config(Sensor, in1,    vis1, sensorReflection)
#pragma config(Sensor, in2,    inf1, sensorLineFollower)
#pragma config(Sensor, in3,    inf2, sensorLineFollower)
#pragma config(Sensor, dgtl11, enc1, sensorQuadEncoder)
#pragma config(Motor,  port1,  led1, tmotorVexFlashlight, openLoop, reversed)
#pragma config(Motor,  port2,  ser1, tmotorServoStandard, openLoop)
#pragma config(Motor,  port10, mot1, tmotorVex269_HBridge, openLoop)

#define square(n) n*n

int type;

int dist;
int minDist;

enum sensor { Vis, Inf, nSensors };
enum material { Empty, Blue, Metal, White, Bite, Wood, Clear, nMaterials };

// Known material data to match against
int known[nMaterials][nSensors] = {
	{  107,   84},
	{  578, 1423},
	{ 2609, 1119},
	{  875,  957},
	{  416, 1145},
	{ 2742,  623},
	{  112, 1229},
};
// Normalization factors for converting sensor data to same range as knowns
int norm[nSensors] = { 300, 4000 };

// Averaging an array of sensor values
#define nSamples 200
int arr[nSamples][nSensors];
int index;
int avg[nSensors];
int sum[nSensors];

int i;

task main()
{
	SensorValue[enc1] = 0; // Reset encoder

	while(true)
	{
		// Raise scanner to catch ball
		while(SensorValue[enc1] < 0) motor[mot1] = 128;
		wait(0.1);
		motor[mot1] = 0;
		
		// Release ball from queue
		motor[ser1] = -127;
		wait(1);
		motor[ser1] = 127;
		wait(1);

		// Turn on flashlight
		motor[led1] = -127;
		
		// Sample until type is not 0, but have a minimum of samples first
		for(index = 0; type == 0 || index < nSamples; index++)
		{
			// Add new samples to arrays
			arr[index % nSamples][Vis] = SensorValue[vis1];
			arr[index % nSamples][Inf] = SensorValue[inf1] + SensorValue[inf2];

			// Find sums of arrays
			sum[Vis] = 0;
			sum[Inf] = 0;
			for(i = 0; i < nSamples; i++)
			{
				sum[Vis] += arr[i][Vis];
				sum[Inf] += arr[i][Inf];
			}

			// Take averages
			avg[Vis] = sum[Vis] * 1000 / norm[Vis] / nSamples;
			avg[Inf] = sum[Inf] * 1000 / norm[Inf] / nSamples;

			// Find best type match
			minDist = 1E9;
			type = 0;
			for(i = 0; i < (int)nMaterials; i++)
			{
				dist = square(avg[Vis] - known[i][Vis]) + square(avg[Inf] - known[i][Inf]);

				if(dist > minDist) continue;
				
				minDist = dist;
				type = i;
			}

		}

		// Turn off flashlight
		motor[led1] = 0;
		
		// Lower scanner to drop ball
		while(SensorValue[enc1] > -200) motor[mot1] = -128;
		motor[mot1] = 0;
		wait(1);
	}
}

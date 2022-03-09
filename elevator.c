#pragma config(Sensor, dgtl1,  D1, sensorTouch)
#pragma config(Sensor, dgtl2,  D2, sensorTouch)
#pragma config(Sensor, dgtl3,  D3, sensorTouch)
#pragma config(Sensor, dgtl10, L1, sensorDigitalOut)
#pragma config(Sensor, dgtl11, L2, sensorDigitalOut)
#pragma config(Sensor, dgtl12, L3, sensorDigitalOut)
#pragma config(Motor,  port1,  M1, tmotorVex393_HBridge, openLoop)
#define safeTimer T1
#define moveTimer T2

// BEGIN User configurable parameters

#define nFloors 3

static int button[nFloors] = {
	D1,
	D2,
	D3,
};
static int light[nFloors] = {
	L1,
	L2,
	L3,
};
static int pressed[nFloors];
static int selected[nFloors];

#define upTimeLimit 	 1500
#define downTimeLimit  1000
#define holdTimeLimit  1000
#define safeTimeLimit 20000

#define upPower -100
#define downPower 15
#define holdPower -25

// END User configurable parameters
int currentFloor = 0;
int targetFloor = 0;
int deltaFloor = 0;

int moveTimeLimit = 0;

int power = holdPower;

task main() {

	while (1) {
		setMotor(M1, power);

		// Select the floors on button press,
		// and also reset the safety clock
		for (int f = 0; f < nFloors; f++) {
			pressed[f] = SensorValue[button[f]];
			if (pressed[f]) {
				selected[f] = 1;
				resetTimer(safeTimer);
			}
			if (f == currentFloor){
				turnLEDOff(light[f]);
				} else {
				turnLEDOn(light[f]);
			}
		}

		// Go to ground floor if safe time exceeded
		if (time1[safeTimer] > safeTimeLimit) {
			for (int f = 0; f < nFloors; f++) {
				selected[f] = 0;
			}
			selected[0] = 1;
			resetTimer(safeTimer);
		}

		// Stop moving once floor is exceeded
		if (time1[moveTimer] > moveTimeLimit) {
			power = holdPower;
		}

		// Find another target after stopping a while
		if (time1[moveTimer] > moveTimeLimit + holdTimeLimit) {
			resetTimer(moveTimer);

			currentFloor = targetFloor;

			// Turn off current floor's switch
			selected[currentFloor] = 0;

			// Goes through selected floors and target the lowest one
			for (int f = 0; f < nFloors; f++) {
				if (selected[f]) {
					targetFloor = f;
					break;
				}
			}

			deltaFloor = targetFloor - currentFloor;

			if (deltaFloor > 0) {
				power = upPower;
				moveTimeLimit = upTimeLimit;
				} else if (deltaFloor < 0) {
				power = downPower;
				moveTimeLimit = downTimeLimit;
				} else {
				power = holdPower;
				moveTimeLimit = holdTimeLimit;
			}
			moveTimeLimit *= abs(deltaFloor);
		}
	}
}

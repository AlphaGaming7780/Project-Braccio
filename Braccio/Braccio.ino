 /*
  testBraccio90.ino

 testBraccio90 is a setup sketch to check the alignment of all the servo motors
 This is the first sketch you need to run on Braccio
 When you start this sketch Braccio will be positioned perpendicular to the base
 If you can't see the Braccio in this exact position you need to reallign the servo motors position

 Created on 18 Nov 2015
 by Andrea Martino

 This example is in the public domain.
 */

#include <Braccio.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

int m1 = 0, m2 = 90, m3 = 90, m4 = 90;

bool manu = false;

void setup() {  
	//Initialization functions and set up the initial position for Braccio
	//All the servo motors will be positioned in the "safety" position:
	//Base (M1):90 degrees
	//Shoulder (M2): 45 degrees
	//Elbow (M3): 180 degrees
	//Wrist vertical (M4): 180 degrees
	//Wrist rotation (M5): 90 degrees
	//gripper (M6): 10 degrees
  	Serial.begin(115200);
	while (!Serial) {
	  	// will pause Zero, Leonardo, etc until serial console opens
		delay(1);
	}
	Braccio.begin();
	Serial.println("Hello!");
	// Initialize the INA219.
	// By default the initialization will use the largest range (32V, 2A).  However
	// you can call a setCalibration function to change this range (see comments).
	if (! ina219.begin()) {
		Serial.println("Failed to find INA219 chip");
		while (1) { delay(10); }
	}
	// To use a slightly lower 32V, 1A range (higher precision on amps):
	//ina219.setCalibration_32V_1A();
	// Or to use a lower 16V, 400mA range (higher precision on volts and amps):
	ina219.setCalibration_16V_400mA();

	Serial.println("Measuring voltage and current with INA219 ...");

}

void loop() {

	float busvoltage = 0;
	float current_mA = 0;
	float power_mW = 0;


	busvoltage = ina219.getBusVoltage_V();
	current_mA = ina219.getCurrent_mA();
	power_mW = ina219.getPower_mW();

	
	Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
	Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
	Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
	Serial.println("");


	/*
	Step Delay: a milliseconds delay between the movement of each servo.  Allowed values from 10 to 30 msec.
	M1=base degrees. Allowed values from 0 to 180 degrees
	M2=shoulder degrees. Allowed values from 15 to 165 degrees
	M3=elbow degrees. Allowed values from 0 to 180 degrees
	M4=wrist vertical degrees. Allowed values from 0 to 180 degrees
	M5=wrist rotation degrees. Allowed values from 0 to 180 degrees
	M6=gripper degrees. Allowed values from 10 to 73 degrees. 10: the toungue is open, 73: the gripper is closed.
	*/
	
	// the arm is aligned upwards  and the gripper is closed

	// Serial.println(analogRead(A0));

	// if(analogRead(A0) > 520) {
	if(manu) {
		BrasManu();
	} else {
		otto();
	}
	 
}

void move(int m1, int m2, int m3, int m4) {
	Braccio.ServoMovement(10, m1+10, m2-1, m3+7, m4+6, 90, 73);
}


void BrasManu() {
	m1 += map(analogRead(A0), 0, 1023, -10, 10);
	m2 += map(analogRead(A1), 0, 1023, -10, 10);
	m3 += map(analogRead(A2)-8, 0, 1023, -10, 10);
	m4 += map(analogRead(A3)-4, 0, 1023, -10, 10);

	Serial.println(analogRead(A2)-8);
	Serial.println(analogRead(A3)-4);

	if(m1 > 180) m1 = 180;
	if(m1 < 0) m1 = 0;
	if(m2 > 180) m2 = 180;
	if(m2 < 0) m2 = 0;
	if(m3 > 180) m3 = 180;
	if(m3 < 0) m3 = 0;
	if(m4 > 180) m4 = 180;
	if(m4 < 0) m4 = 0;

	move(m1, m2, m3, m4);
}

void otto() {
	m1 += map(analogRead(A0) - analogRead(A1), -1023, 1023, -10, 10);
	m4 += map(analogRead(A2) - analogRead(A3), -1023, 1023, -10, 10);
	// m3 += map(analogRead(A2), 0, 1023, -10, 10);
	// m4 += map(analogRead(A3), 0, 1023, -10, 10);
	Serial.print("M1 = ");
	Serial.println(m1);
	Serial.print("M4 = ");
	Serial.println(m4);

	if(m1 > 180) m1 = 180;
	if(m1 < 0) m1 = 0;
	if(m4 > 180) m4 = 180;
	if(m4 < 0) m4 = 0;

	move(m1, m2, m3, m4);
}
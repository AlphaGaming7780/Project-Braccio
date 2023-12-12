//Include all the lbraries we need

#include <Braccio.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal.h>

//Define the current/voltage sensor we use
Adafruit_INA219 ina219;

//Define each servo of the braccio
Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

//Define all the variables we need
//const int cannot change
//int can change
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 7, d7 = 8,  _timer = 100;
int m1 = 0, m2 = 90, m3 = 90, m4 = 90, angle = -90, cpt_ina = 0;
//attribute each pins of the lcd screens
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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
  	Serial.begin(115200); 	//initialize serial communication
	Braccio.begin();		//initialize braccio
	lcd.begin(16, 2);		//initialize lcd screen

	// Initialize the INA219.
	// By default the initialization will use the largest range (32V, 2A).  However
	// you can call a setCalibration function to change this range (see comments).
	if (! ina219.begin()) {
		Serial.println("Failed to find INA219 chip");
	}

	// To use a slightly lower 32V, 1A range (higher precision on amps):
	//ina219.setCalibration_32V_1A();
	// Or to use a lower 16V, 400mA range (higher precision on volts and amps):
	ina219.setCalibration_16V_400mA();

	Serial.println("Measuring voltage and current with INA219 ...");
	lcd.print("HELLO WORLD");

}

void loop() {

	//cpt_ina is a variable used as a delay. We wanted to not use the delay function because it can block the braccio
	//The program decrement cpt_ina each time it complete a loop
	if(cpt_ina == 0){
		// use serial monitor for debug mode
		// Serial.print("Bus Voltage:   "); Serial.print(ina219.getBusVoltage_V()); Serial.println(" V");
		// Serial.print("Current:       "); Serial.print(ina219.getCurrent_mA()); Serial.println(" mA");
		// Serial.print("Power:         "); Serial.print(ina219.getPower_mW()); Serial.println(" mW");
		// Serial.print("");
		lcd.clear(); //clear the screen
		lcd.setCursor(0,0); //write first character in the first place
		lcd.print("U="); lcd.print(ina219.getBusVoltage_V()); lcd.print("V "); //print the voltage value "U=x.xxV". We cannot put space between character, the place is limited on our screen 16x2
		lcd.print("W="); lcd.print(ina219.getPower_mW()); lcd.print("mW"); //print the power value "W=x.xxmW".
		lcd.setCursor(0,1); //set cursor on the second line of the screen.
		lcd.print("I="); lcd.print(ina219.getCurrent_mA()); lcd.print("mA"); //print the current value "I=x.xxmA"
		cpt_ina = _timer; //reset delay value

	}
	
	
	// delay(500);


	/*
	Step Delay: a milliseconds delay between the movement of each servo.  Allowed values from 10 to 30 msec.
	M1=base degrees. Allowed values from 0 to 180 degrees
	M2=shoulder degrees. Allowed values from 15 to 165 degrees
	M3=elbow degrees. Allowed values from 0 to 180 degrees
	M4=wrist vertical degrees. Allowed values from 0 to 180 degrees
	M5=wrist rotation degrees. Allowed values from 0 to 180 degrees
	M6=gripper degrees. Allowed values from 10 to 73 degrees. 10: the toungue is open, 73: the gripper is closed.
	*/
	
	if(manu) {
		BrasManu();
	} else {
		otto();
	}
	cpt_ina--;
	 
}

// Function that actualize each servo position
void move(int m1, int m2, int m3, int m4) {
	Braccio.ServoMovement(10, m1+10, m2-1, m3+7, m4+6, 90, 73);
}


void BrasManu() {
	//remap each joystick axis value to angle value
	m1 += map(analogRead(A0), 0, 1023, -10, 10);
	m2 += map(analogRead(A1), 0, 1023, -10, 10);
	m3 += map(analogRead(A2)-8, 0, 1023, -10, 10);
	m4 += map(analogRead(A3)-4, 0, 1023, -10, 10);

	// Use serial monitor for debug mode
	// Serial.println(analogRead(A2)-8);
	// Serial.println(analogRead(A3)-4);

	// Avoid servo to go over limits
	if(m1 > 180) m1 = 180;
	if(m1 < 0) m1 = 0;
	if(m2 > 180) m2 = 180;
	if(m2 < 0) m2 = 0;
	if(m3 > 180) m3 = 180;
	if(m3 < 0) m3 = 0;
	if(m4 > 180) m4 = 180;
	if(m4 < 0) m4 = 0;

	// Call move function
	move(m1, m2, m3, m4);
}

void otto() {
	angle += map(analogRead(A1) - analogRead(A0), -1023, 1023, -45, 45);
	m1 += map(analogRead(A3) - analogRead(A2), -1023, 1023, -45, 45);

	if(angle > 135) angle = 135;
	if(angle < 45) angle = 45;

	if(m1 > 180) m1 = 180;
	if(m1 < 0) m1 = 0;

	// Split angle value for each servo on the vertical axis
	m2 = 90+(angle-90)/3;
	m3 = 90+(angle-90)/3;
	m4 = 90+(angle-90)/3;

	// Use serial monitor for debug mode
	// Serial.print("M1 = ");
	// Serial.println(m1);
	// Serial.print("M2 = ");
	// Serial.println(m2);
	// Serial.print("M3 = ");
	// Serial.println(m3);
	// Serial.print("M4 = ");
	// Serial.println(m4);

	//delay(100);
	Serial.println(("A0 : %i", analogRead(A0)));
	Serial.println(("A1 : %i", analogRead(A1)));
	Serial.println(("A2 : %i", analogRead(A2)));
	Serial.println(("A3 : %i", analogRead(A3)));

	// Call move function
	move(m1, m2, m3, m4);

}
//Include all the lbraries we need

#include <Braccio.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal_I2C.h>

#define MANU_SPEED 5;

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
const int _timer = 5;
int m1 = 90, m2 = 90, m3 = 90, m4 = 90, angle = 90, cpt_ina = 0, vdelay;
float voltage, power, current;
//attribute each pins of the lcd screens
LiquidCrystal_I2C lcd(0x3F, 16, 2);

bool maintien, manu = true, lumi = true;

void setup() {  

	pinMode(2, INPUT);
	pinMode(3, INPUT);
	pinMode(4, INPUT);
	pinMode(5, INPUT);
	pinMode(7, INPUT);
	pinMode(8, OUTPUT);
	pinMode(13, INPUT);


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
 	lcd.init();		//initialize lcd screen
	lcd.backlight();
	lcd.clear(); //clear the screen
	lcd.setCursor(0,0); 
	lcd.print("Starting...");

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

}

void loop() {

	if(!digitalRead(2) && !maintien) {
		maintien = true;
		manu = !manu;
	} else if (!digitalRead(13) && !maintien) {
		maintien = true;
		lumi = !lumi;
	} else if(maintien && digitalRead(2) && digitalRead(13)){
		maintien = false;
	}

	//cpt_ina is a variable used as a delay. We wanted to not use the delay function because it can block the braccio
	//The program decrement cpt_ina each time it complete a loop
	if(cpt_ina == 0 && !lumi){
		// use serial monitor for debug mode
		voltage = ina219.getBusVoltage_V();
		power = ina219.getPower_mW();
		current = ina219.getCurrent_mA();

		lcd.clear(); //clear the screen
		lcd.setCursor(0,0); //write first character in the first place
		lcd.print("U="); lcd.print(voltage); lcd.print("V "); //print the voltage value "U=x.xxV". We cannot put space between character, the place is limited on our screen 16x2
		lcd.print("W="); lcd.print(power); lcd.print("mW"); //print the power value "W=x.xxmW".
		lcd.setCursor(0,1); //set cursor on the second line of the screen.
		lcd.print("I="); lcd.print(current); lcd.print("mA"); //print the current value "I=x.xxmA"
		cpt_ina = _timer; //reset delay value
	} else if(cpt_ina == 0) {
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("P0="); lcd.print(((float)analogRead(A0)/1023)*5); lcd.print("V ");
		lcd.print("P2="); lcd.print(((float)analogRead(A2)/1023)*5); lcd.print("V");
		lcd.setCursor(0,1);
		lcd.print("P1="); lcd.print(((float)analogRead(A1)/1023)*5); lcd.print("V ");
		lcd.print("P3="); lcd.print(((float)analogRead(A3)/1023)*5); lcd.print("V");
		cpt_ina = _timer; //reset delay value
	}
	cpt_ina--;

	digitalWrite(8, manu);


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
}

// Function that actualize each servo position
void move(int vdelay, int m1, int m2, int m3, int m4) {
	int delayBraccio = vdelay >= 30 ? vdelay-30 : vdelay;
	Braccio.ServoMovement(delayBraccio, m1+10, m2-1, m3+7, m4+6, 90, 73);
	delay(vdelay >= 30 ? vdelay-30 : 0);
	if(vdelay >= 20) delay(vdelay/2);
}


void BrasManu() {

	if(!digitalRead(3)) {
		m1 -= MANU_SPEED;
	} else if (!digitalRead(4)) {
		m1 += MANU_SPEED;
	} else if (!digitalRead(5)) {
		angle -= MANU_SPEED;
	} else if (!digitalRead(7)) {
		angle += MANU_SPEED;
	}


	// Avoid servo to go over limits
	if(angle > 225) angle = 225;
	if(angle < -45) angle = -45;

	if(m1 > 180) m1 = 180;
	if(m1 < 0) m1 = 0;

	m2 = 90+(angle-90)/3;
	m3 = 90+(angle-90)/3;
	m4 = 90+(angle-90)/3;

	// Call move function
	move(30, m1, m2, m3, m4);
}

void otto() {
	int diffH = map((analogRead(A2) - analogRead(A3) + 30), -1023, 1023, -90, 90);
	int diffV = map((analogRead(A0) - analogRead(A1) - 15), -1023, 1023, -90, 90);
	angle += diffV;
	m1 += diffH;
	vdelay = map((diffH+diffV)/2, -45, 45, 90, 10);

	// Serial.println(map((analogRead(A2) - analogRead(A3))*2, -1023, 1023, -45, 45));
	// Serial.println(vdelay);

	if(angle > 225) angle = 225;
	if(angle < -45) angle = -45;

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
	// Serial.println(("A0 : %i", analogRead(A0)));
	// Serial.println(("A1 : %i", analogRead(A1)));
	// Serial.println(("A2 : %i", analogRead(A2)));
	// Serial.println(("A3 : %i", analogRead(A3)));

	// Call move function
	move(vdelay, m1, m2, m3, m4);

}
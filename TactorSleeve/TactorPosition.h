/**************************************************************************************\
* TactorPosition.h
* Written By Alex Tatro

* Version: 1.5
* Details: Addition of alternate boundary shapes (Circle, Triangle, Rectangle) and 
*			randomization of boundary when program is started. Allows user the
*			ability to determin the boundary shape by exploring with their hand.
*
\**************************************************************************************/

#include <iostream>
#include <iomanip> 
#include <string.h>
#include "Leap.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <strsafe.h>
#include <math.h>
#include <cstdlib> 
#include <ctime>

void HandActions(float, float, float, float);
void RectanglePos(float, float, float);
void TrianglePos(float, float, float);
void CirclePos(float, float, float);
void GripFlag(float);
void FlagActions();
void AlertMotorSignal(int);
void WarningMotorSignal(int);
void BoundaryMotorSignal(int);
void Display(float, float, float, float);
void InFieldMotorSignal();
void exit();

//Flags
#define NO_ACTION				1
#define IN_FIELD				2

#define ALERT_LEFT				3
#define ALERT_LEFT_TOP			4
#define ALERT_TOP				5
#define ALERT_TOP_RIGHT			6
#define ALERT_RIGHT				7
#define ALERT_RIGHT_BOTTOM		8
#define ALERT_BOTTOM			9
#define ALERT_BOTTOM_LEFT		10

#define WARNING_LEFT			11
#define WARNING_LEFT_TOP		12
#define WARNING_TOP				13
#define WARNING_TOP_RIGHT		14
#define WARNING_RIGHT			15
#define WARNING_RIGHT_BOTTOM	16
#define WARNING_BOTTOM			17
#define WARNING_BOTTOM_LEFT		19

#define BOUNDARY_LEFT			20
#define BOUNDARY_LEFT_TOP		21
#define BOUNDARY_TOP			22
#define BOUNDARY_TOP_RIGHT		23
#define BOUNDARY_RIGHT			24
#define BOUNDARY_RIGHT_BOTTOM	25
#define BOUNDARY_BOTTOM			26
#define BOUNDARY_BOTTOM_LEFT	27

#define HAND_FLAT				28
#define HAND_FIST				29

#define LEFT					30
#define TOP						31
#define RIGHT					32
#define BOTTOM					33

#define portName	"\\\\.\\COM5"

//Motor Amplitues & Frequencies
#define AlertAmp		1000
#define AlertFreq		750
#define WarnAmp			2000
#define WarnFreq		750
#define BoundAmp		4000
#define BoundFreq		750

//Rectangular Ranges
const int rangeLeft = -200;
const int rangeTop = 400;
const int rangeRight = 200;
const int rangeBottom = 200;

//Circular Ranges
const int rangeRadial = 200;
const int ycenter = 300;
const double trans1 = 0.57735; //slope of transition points (30deg, 60deg, etc)
const double trans2 = 1.73205;

//Triangular Ranges
const int yintercept = 100;




const int rangeWarning = 50;
const int rangeAlert = 75;

// Status Flags
int shape = 0;
int status = 0;
int gripflag = HAND_FIST;
int flag = 1;

// Counters implemented to slow signal transmission to protect arduino buffer
unsigned long counter = 0;
#define maxCount	0
#define maxCount2	0


	HANDLE hComm;

void MotorControllerInit(void){

	hComm = CreateFile(portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}

void HandActions(float x, float y, float z, float grip){
	srand(time(0));
	if (shape == 0){
		shape = rand() % 3 + 1;
		//std::cout << "Boundary Shape: " << shape << "\n";
	
		switch (shape){
		case 1:
			std::cout << "Boundary Shape: RECTANGLE\n";
			break;
		case 2:
			std::cout << "Boundary Shape: TRIANGLE\n";
			break;
		case 3:
			std::cout << "Boundary Shape: CIRCLE\n";
			break;
		}
	}

		switch (shape){
		case 1:
			RectanglePos(x, y, z);
			break;
		case 2:
			TrianglePos(x, y, z);
			break;
		case 3:
			CirclePos(x, y, z);
			break;
		}
	GripFlag(grip);
	FlagActions();
}

void RectanglePos(float x, float y, float z){
	// if statatments to determine if hand position is approaching boundaries
	// hierarchy:  boundary -> warning -> alert

	if (x < rangeLeft){ flag = BOUNDARY_LEFT; }
	else if (x < rangeLeft + rangeWarning){ flag = WARNING_LEFT; }
	else if (x < rangeLeft + rangeAlert){ flag = ALERT_LEFT; }

	if (y > rangeTop){ flag = BOUNDARY_TOP; }
	else if (y > rangeTop - rangeWarning){ flag = WARNING_TOP; }
	else if (y > rangeTop - rangeAlert){ flag = ALERT_TOP; }

	if (x > rangeRight){ flag = BOUNDARY_RIGHT; }
	else if (x > rangeRight - rangeWarning){ flag = WARNING_RIGHT; }
	else if (x > rangeRight - rangeAlert){ flag = ALERT_RIGHT; }

	if (y < rangeBottom){ flag = BOUNDARY_BOTTOM; }
	else if (y < rangeBottom + rangeWarning){ flag = WARNING_BOTTOM; }
	else if (y < rangeBottom + rangeAlert){ flag = ALERT_BOTTOM; }

	if (x > rangeLeft + rangeAlert && x < rangeRight - rangeAlert && y < rangeTop - rangeAlert && 
		y > rangeBottom + rangeAlert) { flag = IN_FIELD; }
}

void TrianglePos(float x, float y, float z){
	float d1, d2;

	d1 = y - 2 * x;
	d2 = y + 2 * x;

	if (x >= 0){
		if (y > rangeTop) { flag = BOUNDARY_TOP; }
		else if (y > rangeTop - rangeWarning) { flag = WARNING_TOP; }
		else if (y > rangeTop - rangeAlert) { flag = ALERT_TOP; }
		else
			if (d1 < yintercept) { flag = BOUNDARY_RIGHT_BOTTOM; }
			else if (d1 < yintercept + rangeWarning) { flag = WARNING_RIGHT_BOTTOM; }
			else if (d1 < yintercept + rangeAlert) { flag = ALERT_RIGHT_BOTTOM; }
			else { flag = IN_FIELD; }
	}
	else if (x < 0){
		if (y > rangeTop) { flag = BOUNDARY_TOP; }
		else if (y > rangeTop - rangeWarning) { flag = WARNING_TOP; }
		else if (y > rangeTop - rangeAlert) { flag = ALERT_TOP; }
		else
			if (d2 < yintercept) { flag = BOUNDARY_BOTTOM_LEFT; }
			else if (d2 < yintercept + rangeWarning) { flag = WARNING_BOTTOM_LEFT; }
			else if (d2 < yintercept + rangeAlert) { flag = ALERT_BOTTOM_LEFT; }
			else { flag = IN_FIELD; }
	}
	else { std::cout << "Positioning Error\n"; }
}

void CirclePos( float x,  float y,  float z){
	float r, slope;

	slope = (y-ycenter) / (x-0);
	r = sqrt((x)*(x)+(y-ycenter)*(y-ycenter));
	
	if (r > rangeRadial){ 
		if (x > 0){
			if (slope >= trans2){ flag = BOUNDARY_TOP; }
			else if (slope < trans2 && slope >= trans1){ flag = BOUNDARY_TOP_RIGHT; }
			else if (slope < trans1 && slope >= (-1)*trans1){ flag = BOUNDARY_RIGHT; }
			else if (slope < (-1)*trans1 && slope >= (-1)*trans2){ flag = BOUNDARY_RIGHT_BOTTOM; }
			else if (slope < (-1)*trans2){ flag = BOUNDARY_BOTTOM; }
		}
		else if (x < 0){
			if (slope >= trans2){ flag = BOUNDARY_BOTTOM; }
			else if (slope < trans2 && slope >= trans1){ flag = BOUNDARY_BOTTOM_LEFT; }
			else if (slope < trans1 && slope >= (-1)*trans1){ flag = BOUNDARY_LEFT; }
			else if (slope < (-1)*trans1 && slope >= (-1)*trans2){ flag = BOUNDARY_LEFT_TOP; }
			else if (slope < (-1)*trans2){ flag = BOUNDARY_TOP; }
		}
		else if (x == 0){
			if (y > ycenter){ flag = BOUNDARY_TOP; }
			else if (y < ycenter) { flag = BOUNDARY_BOTTOM; }
		}
		else 
			std::cout << "Radial Range Error\n";
		
	}
	else if (r > rangeRadial - rangeWarning){ 
		if (x > 0){
			if (slope >= trans2){ flag = WARNING_TOP; }
			else if (slope < trans2 && slope >= trans1){ flag = WARNING_TOP_RIGHT; }
			else if (slope < trans1 && slope >= (-1)*trans1){ flag = WARNING_RIGHT; }
			else if (slope < (-1)*trans1 && slope >= (-1)*trans2){ flag = WARNING_RIGHT_BOTTOM; }
			else if (slope < (-1)*trans2){ flag = WARNING_BOTTOM; }
		}
		else if (x < 0){
			if (slope >= trans2){ flag = WARNING_BOTTOM; }
			else if (slope < trans2 && slope >= trans1){ flag = WARNING_BOTTOM_LEFT; }
			else if (slope < trans1 && slope >= (-1)*trans1){ flag = WARNING_LEFT; }
			else if (slope < (-1)*trans1 && slope >= (-1)*trans2){ flag = WARNING_LEFT_TOP; }
			else if (slope < (-1)*trans2){ flag = WARNING_TOP; }
		}
		else if (x == 0){
			if (y > ycenter){ flag = WARNING_TOP; }
			else if (y < ycenter) { flag = WARNING_BOTTOM; }
		}
		else
			std::cout << "Radial Range Error\n";
	
	}
	else if (r > rangeRadial - rangeAlert){ 
		if (x > 0){
			if (slope >= trans2){ flag = ALERT_TOP; }
			else if (slope < trans2 && slope >= trans1){ flag = ALERT_TOP_RIGHT; }
			else if (slope < trans1 && slope >= (-1)*trans1){ flag = ALERT_RIGHT; }
			else if (slope < (-1)*trans1 && slope >= (-1)*trans2){ flag = ALERT_RIGHT_BOTTOM; }
			else if (slope < (-1)*trans2){ flag = ALERT_BOTTOM; }
		}
		else if (x < 0){
			if (slope >= trans2){ flag = ALERT_BOTTOM; }
			else if (slope < trans2 && slope >= trans1){ flag = ALERT_BOTTOM_LEFT; }
			else if (slope < trans1 && slope >= (-1)*trans1){ flag = ALERT_LEFT; }
			else if (slope < (-1)*trans1 && slope >= (-1)*trans2){ flag = ALERT_LEFT_TOP; }
			else if (slope < (-1)*trans2){ flag = ALERT_TOP; }
		}
		else if (x == 0){
			if (y > ycenter){ flag = ALERT_TOP; }
			else if (y < ycenter) { flag = ALERT_BOTTOM; }
		}
		else
			std::cout << "Radial Range Error\n";
	}
	else { flag = IN_FIELD; }
}

void GripFlag(float grip){
	if (grip == 1)
		gripflag = HAND_FIST;
	else if (grip < 1)
		gripflag = HAND_FLAT;
	else
		std::cout << "Fist Status Error\n";
}

void FlagActions(){
	DWORD bytesSend;
	switch (gripflag){
	case HAND_FIST:	

		break;
	case HAND_FLAT:
		if (status == flag){ }
			else{
				switch (flag)
				{
				case IN_FIELD:
					counter++;
					if (counter > maxCount){
						InFieldMotorSignal();
						status = IN_FIELD;
						counter = 0;
					}
					break;
				case ALERT_LEFT:	
					counter++;
					if (counter > maxCount){
						AlertMotorSignal(LEFT);
						status = ALERT_LEFT;
						counter = 0;
					}
					break;
				case ALERT_LEFT_TOP:
					counter++;
					if (counter > maxCount){
						AlertMotorSignal(LEFT);
						AlertMotorSignal(TOP);
						status = ALERT_LEFT_TOP;
						counter = 0;
					}
					break;
				case ALERT_TOP:
					counter++;
					if (counter > maxCount){
						AlertMotorSignal(TOP);
						status = ALERT_TOP;
						counter = 0;
					}
					break;
				case ALERT_TOP_RIGHT:
					counter++;
					if (counter > maxCount){
						AlertMotorSignal(TOP);
						AlertMotorSignal(RIGHT);
						status = ALERT_TOP_RIGHT;
						counter = 0;
					}
					break;
				case ALERT_RIGHT:
					counter++;
					if (counter > maxCount){
						AlertMotorSignal(RIGHT);
						status = ALERT_RIGHT;
						counter = 0;
					}
					break;
				case ALERT_RIGHT_BOTTOM:
					counter++;
					if (counter > maxCount){
						AlertMotorSignal(RIGHT);
						AlertMotorSignal(BOTTOM);
						status = ALERT_RIGHT_BOTTOM;
						counter = 0;
					}
					break;
				case ALERT_BOTTOM:
					counter++;
					if (counter > maxCount){
						AlertMotorSignal(BOTTOM);
						status = ALERT_BOTTOM;
						counter = 0;
					}
					break;
				case ALERT_BOTTOM_LEFT:
					counter++;
					if (counter > maxCount){
						AlertMotorSignal(BOTTOM);
						AlertMotorSignal(LEFT);
						status = ALERT_BOTTOM_LEFT;
						counter = 0;
					}
					break;

				case WARNING_LEFT:
					counter++;
					if (counter > maxCount){
						WarningMotorSignal(LEFT);
						status = WARNING_LEFT;
						counter = 0;
					}
					break;
				case WARNING_LEFT_TOP:
					counter++;
					if (counter > maxCount){
						WarningMotorSignal(LEFT);
						WarningMotorSignal(TOP);
						status = WARNING_LEFT_TOP;
						counter = 0;
					}
					break;
				case WARNING_TOP:
					counter++;
					if (counter > maxCount){
						WarningMotorSignal(TOP);
						status = WARNING_TOP;
						counter = 0;
					}
					break;
				case WARNING_TOP_RIGHT:
					counter++;
					if (counter > maxCount){
						WarningMotorSignal(TOP);
						WarningMotorSignal(RIGHT);
						status = WARNING_TOP_RIGHT;
						counter = 0;
					}
					break;
				case WARNING_RIGHT:
					counter++;
					if (counter > maxCount){
						WarningMotorSignal(RIGHT);
						status = WARNING_RIGHT;
						counter = 0;
					}
					break;
				case WARNING_RIGHT_BOTTOM:
					counter++;
					if (counter > maxCount){
						WarningMotorSignal(RIGHT);
						WarningMotorSignal(BOTTOM);
						status = WARNING_RIGHT_BOTTOM;
						counter = 0;
					}
					break;
				case WARNING_BOTTOM:
					counter++;
					if (counter > maxCount){
						WarningMotorSignal(BOTTOM);
						status = WARNING_BOTTOM;
						counter = 0;
					}
					break;
				case WARNING_BOTTOM_LEFT:
					counter++;
					if (counter > maxCount){
						WarningMotorSignal(BOTTOM);
						WarningMotorSignal(LEFT);
						status = WARNING_BOTTOM_LEFT;
						counter = 0;
					}
					break;

				case BOUNDARY_LEFT:
					counter++;
					if (counter > maxCount){
						BoundaryMotorSignal(LEFT);
						status = BOUNDARY_LEFT;
						counter = 0;
					}
					break;
				case BOUNDARY_LEFT_TOP:
					counter++;
					if (counter > maxCount){
						BoundaryMotorSignal(LEFT);
						BoundaryMotorSignal(TOP);
						status = BOUNDARY_LEFT_TOP;
						counter = 0;
					}
					break;
				case BOUNDARY_TOP:
					counter++;
					if (counter > maxCount){
						BoundaryMotorSignal(TOP);
						status = BOUNDARY_TOP;
						counter = 0;
					}
					break;
				case BOUNDARY_TOP_RIGHT:
					counter++;
					if (counter > maxCount){
						BoundaryMotorSignal(TOP);
						BoundaryMotorSignal(RIGHT);
						status = BOUNDARY_TOP_RIGHT;
						counter = 0;
					}
					break;
				case BOUNDARY_RIGHT:
					counter++;
					if (counter > maxCount){
						BoundaryMotorSignal(RIGHT);
						status = BOUNDARY_RIGHT;
						counter = 0;
					}
					break;
				case BOUNDARY_RIGHT_BOTTOM:
					counter++;
					if (counter > maxCount){
						BoundaryMotorSignal(RIGHT);
						BoundaryMotorSignal(BOTTOM);
						status = BOUNDARY_RIGHT_BOTTOM;
						counter = 0;
					}
					break;
				case BOUNDARY_BOTTOM:
					counter++;
					if (counter > maxCount){
						BoundaryMotorSignal(BOTTOM);
					status = BOUNDARY_BOTTOM;
					counter = 0;
					}
					break;
				case BOUNDARY_BOTTOM_LEFT:
					counter++;
					if (counter > maxCount){
						BoundaryMotorSignal(BOTTOM);
						BoundaryMotorSignal(LEFT);
						status = BOUNDARY_BOTTOM_LEFT;
						counter = 0;
					}
					break;

				default:
					std::cout << "SWITCH STATEMENT CAPTURE ERROR\n";
					break;
				}
			}
			break;
	}
	
}

void InFieldMotorSignal(){
	DWORD bytesSend;
	std::cout << "IN FIELD\n";
	WriteFile(hComm, "T()", 3, &bytesSend, 0);
}

void AlertMotorSignal(int direction){
	DWORD bytesSend;
	switch (direction)
	{
	case LEFT:
		std::cout << "Activate LEFT motor ALERT signal.\n";
		WriteFile(hComm, "R(0,1,1500,10)", 14, &bytesSend, 0);
		break;
	case TOP:
		std::cout << "Activate TOP motor ALERT signal.\n";
		WriteFile(hComm, "R(1,1,1500,10)", 14, &bytesSend, 0);
		break;
	case RIGHT:
		std::cout << "Activate RIGHT motor ALERT signal.\n";
		WriteFile(hComm, "R(2,1,1500,10)", 14, &bytesSend, 0);
		break;
	case BOTTOM:
		std::cout << "Activate BOTTOM motor ALERT signal.\n";
		WriteFile(hComm, "R(3,1,1500,10)", 14, &bytesSend, 0);
		break;
	default:
		break;
	}
}

void WarningMotorSignal(int direction){
	DWORD bytesSend;
	switch (direction)
	{
	case LEFT:
		std::cout << "Activate LEFT motor WARNING signal.\n";
		WriteFile(hComm, "R(0,1,2000,10)", 14, &bytesSend, 0);
			break;
	case TOP:
		std::cout << "Activate TOP motor WARNING signal.\n";
		WriteFile(hComm, "R(1,1,2000,10)", 14, &bytesSend, 0);
			break;
	case RIGHT:
		std::cout << "Activate RIGHT motor WARNING signal.\n";
		WriteFile(hComm, "R(2,1,2000,10)", 14, &bytesSend, 0);
			break;
	case BOTTOM:
		std::cout << "Activate BOTTOM motor WARNING signal.\n";
		WriteFile(hComm, "R(3,1,2000,10)", 14, &bytesSend, 0);
			break;
	default:
		break;
	}
}

void BoundaryMotorSignal(int direction){
	DWORD bytesSend;
	switch (direction)
	{
	case LEFT:
		std::cout << "Activate LEFT motor BOUNDARY signal.\n";
		WriteFile(hComm, "R(0,2,4000,20)", 14, &bytesSend, 0);
			break;
	case TOP:
		std::cout << "Activate TOP motor BOUNDARY signal.\n";
		WriteFile(hComm, "R(1,2,4000,20)", 14, &bytesSend, 0);
			break;
	case RIGHT:
		std::cout << "Activate RIGHT motor BOUNDARY signal.\n";
		WriteFile(hComm, "R(2,2,4000,20)", 14, &bytesSend, 0);
			break;
	case BOTTOM:
		std::cout << "Activate BOTTOM motor BOUNDARY signal.\n";
		WriteFile(hComm, "R(3,2,4000,20)", 14, &bytesSend, 0);
			break;
	default:
		break;
	}
}

void Display(float x, float y, float z, float grip) {
	std::cout  << std::setprecision(3) << "\n Hand Position: [ " << std::setw(6) << x << " ," 
		<< std::setw(6) << y << " , " << std::setw(6) << z << " ]"
		<< std::setw(6) << "   Grip: " << grip;
}

void exit(){
	DWORD bytesSend;
	WriteFile(hComm, "A()", 3, &bytesSend, 0);
	}
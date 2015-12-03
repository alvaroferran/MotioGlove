#include "BNO055.h"
#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 3); // RX, TX

#define A 0X28  //I2C address selection pin LOW
#define B 0x29  //                          HIGH

BNO055 sensor(A);

int pinButton=10;
int btn;
float x,y,z;

int sensorMin=10;
int sensorMax=40;


void setup(){
    Wire.begin();
    mySerial.begin(19200);
    sensor.init();
    pinMode(pinButton, INPUT_PULLUP);
}


float map(float vx, float v1, float v2, float n1, float n2){
    // v1 start of range
    // v2 end of range
    // vx the starting number between the range
    float percentage = (vx-v1)/(v2-v1);
    // n1 start of new range
    // n2 end of new range
    return (n2-n1)*percentage+n1;
}


void loop(){    
    sensor.readEul();

    x=sensor.euler.y;
    y=sensor.euler.z;
    
    btn=digitalRead(pinButton);    //roll

    //Resting zone
    if(x<sensorMin && x>-sensorMin) x=0; 
    if(y<sensorMin && y>-sensorMin) y=0; 

    //Map from angles to values between -1 and 1
    if(x<-sensorMin) x=map(x,-sensorMin,-sensorMax,0,1);
    if(x>sensorMin) x=map(x,sensorMin,sensorMax,0,-1);
    if(y<-sensorMin) y=map(y,-sensorMin,-sensorMax,0,1);
    if(y>sensorMin) y=map(y,sensorMin,sensorMax,0,-1);
    y=map(y,-1,1,1,-1);
    
    //Limit maximum values in all axes
    if(x<-1) x=-1;
    if(x>1) x=1;
    if(y<-1) y=-1;
    if(y>1) y=1;

    //If button pressed set roll angle to yaw
    if(btn1==0){
        z=y;
        y=0;
        x=0;
    }
    else z=0;

    //Send values
    mySerial.print("(");
    mySerial.print(x,2);
    mySerial.print(";");
    mySerial.print(y,2);
    mySerial.print(";");
    mySerial.print(z,2);
    mySerial.println(")");

    delay(100);
}


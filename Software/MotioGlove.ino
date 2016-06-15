#include "BNO055.h"
#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 3); // RX, TX

#define A 0X28  //I2C address selection pin LOW
#define B 0x29  //                          HIGH

BNO055 sensor(A);

int pinButton=10;



void setup(){
    Wire.begin();
    mySerial.begin(19200);
    Serial.begin(19200);
    sensor.init();
    pinMode(pinButton, INPUT_PULLUP);
}



double map(double vx, double v1, double v2, double n1, double n2){
    // v1 start of range, v2 end of range, vx the starting number between the range
    double percentage = (vx-v1)/(v2-v1);
    // n1 start of new range, n2 end of new range
    return (n2-n1)*percentage+n1;
}



void normalizeAxes(double *aAxes){
    int sensorMin=10, sensorMax=40;
    double aX=aAxes[0];
    double aY=aAxes[1];
    //Resting zone
    if(aX<sensorMin && aX>-sensorMin) aX=0;
    if(aY<sensorMin && aY>-sensorMin) aY=0;
    //Map from angles to aX values between -1 and 1
    if(aX<-sensorMin) aX=map(aX,-sensorMin,-sensorMax,0,1);
    if(aX>sensorMin)  aX=map(aX,sensorMin,sensorMax,0,-1);
    if(aY<-sensorMin) aY=map(aY,-sensorMin,-sensorMax,0,1);
    if(aY>sensorMin)  aY=map(aY,sensorMin,sensorMax,0,-1);
    aY=map(aY,-1,1,1,-1);
    //Limit maximum values in all axes
    if(aX<-1) aX=-1;
    if(aX>1)  aX=1;
    if(aY<-1) aY=-1;
    if(aY>1)  aY=1;

    aAxes[0]=aX;
    aAxes[1]=aY;
}



void loop(){
    double x,y,z;
    int btn;

    sensor.readEul();
    x=sensor.euler.y;
    y=sensor.euler.z;

    double axes[2];
    axes[0]=x; axes[1]=y;
    normalizeAxes(&axes[0]);
    x=axes[0]; y=axes[1];

    btn=digitalRead(pinButton);
    //If button pressed set roll angle to yaw
    if(btn==0){
        z=y;
        y=0;
        x=0;
    }
    else z=0;

    // Send values
    mySerial.print("(");
    mySerial.print(x,2);
    mySerial.print(";");
    mySerial.print(y,2);
    mySerial.print(";");
    mySerial.print(z,2);
    mySerial.println(")");

    delay(100);
}

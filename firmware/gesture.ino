#define DEBUG
// This #include statement was automatically added by the Particle IDE.
#include "SparkFun_APDS9960.h"

// Interrupt Pin
#define APDS9960_INT    D3

SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

void setup(){
    // Set interrupt pin as input
    pinMode(APDS9960_INT, INPUT);
    
    // Initialize Serial port
    Serial.begin(9600);
    
    delay(3000);

    // Initialize interrupt service routine
    attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
    
    if(apds.init()){
        Serial.println(F("Gesture Sensor initialized."));
    } else {
        Serial.println(F("Gesture Sensor failed."));
    }
    
    if(apds.enableGestureSensor(true)){
        Serial.println(F("Gesture sensor is now running"));
    } 
}

void loop() {
    if(isr_flag == 1){
        detachInterrupt(APDS9960_INT);
        
        handleGesture();
        
        isr_flag = 0;
        
        attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
    }
}

void interruptRoutine(){
    isr_flag = 1;
}

void handleGesture(){
    if(apds.isGestureAvailable()){
        char szEvent[8];
        
        szEvent[0] = 0;
        
        switch ( apds.readGesture() ) {
            case DIR_UP:            strcpy(szEvent, "UP");      break;
            case DIR_DOWN:          strcpy(szEvent, "DOWN");    break;
            case DIR_LEFT:          strcpy(szEvent, "LEFT");    break;
            case DIR_RIGHT:         strcpy(szEvent, "RIGHT");   break;
        }
        
        if(strlen(szEvent) > 0){
            Serial.println(szEvent);
            
            publishEvent(szEvent);
        }
    }
}

void publishEvent(char* szEvent){
    Spark.publish("GESTURE-EVENT", szEvent, 60, PRIVATE);
}
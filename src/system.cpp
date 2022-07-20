
#include "main.h"

void sys_init(){

    Serial.begin(115200);
 
  pinMode(M1_DIR_PIN,OUTPUT); 
  pinMode(M1_PULSE_PIN,OUTPUT); 
  pinMode(M1_LAZER_PIN,INPUT);

  pinMode(M2_DIR_PIN,OUTPUT); 
  pinMode(M2_PULSE_PIN,OUTPUT); 
  pinMode(M2_LAZER_PIN,INPUT);
  pinMode(0,OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN,1000000);   // join i2c bus
    
  attachInterrupt(M1_LAZER_PIN, lazer1_int, CHANGE);
  attachInterrupt(M2_LAZER_PIN, lazer2_int, CHANGE);
 
  step_init();
  pwm_init();
 
}


void sys_loop(){

  
}
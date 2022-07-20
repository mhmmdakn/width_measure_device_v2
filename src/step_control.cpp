

#include "main.h"

double a1=1000; 
double a2=64000;
double a3=64000;
double a3_t=0.015;

double min_hiz=80.0;
double mid_hiz=400;  
double max_hiz=10000; 
double rampa_x=(pow(max_hiz,2)-pow(min_hiz,2))/(2*a2);
double rampa_2x=2*rampa_x;
double hiz1=min_hiz;
double hiz2=min_hiz;

ulong micros_t=0;



int step1_calisma_modu=0;
int step2_calisma_modu=0;

bool setpin=false;

hw_timer_t * timer0 = NULL;
hw_timer_t * timer1 = NULL;
hw_timer_t * timer2 = NULL;
hw_timer_t * timer3 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


 

double step1_position_measured=0;
double step1_position_actual=0;
int dir1=1;
bool step1_low_en=false;
ulong step1_high_t=0;
ulong step1_low_t=0;
ulong step1_interval=0;


double step2_position_measured=0;
double step2_position_actual=0;
int dir2=1;
bool step2_low_en=false;
ulong step2_high_t=0;
ulong step2_low_t=0;
ulong step2_interval=0;



void step_init(){
  step1_calisma_modu=1;
  step2_calisma_modu=1;

  timer1 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, 10, true);
  timerAlarmEnable(timer1);
}


void IRAM_ATTR onTimer1() {
  timerStop(timer1);

  step_pulse();
  
  timerStart(timer1);
  
}

void IRAM_ATTR lazer1_int() {
  
  
  if(digitalRead(M1_LAZER_PIN)==LOW && step1_calisma_modu==1)
  step1_position_measured = step1_position_actual;
  a3=hiz1/a3_t;
  step1_calisma_modu=3;
  
}

void IRAM_ATTR lazer2_int() {
  
  if(digitalRead(M2_LAZER_PIN)==LOW && step2_calisma_modu==1)
  step2_position_measured = step2_position_actual;
  a3=hiz2/a3_t;
  step2_calisma_modu=3;
  
}

void step_pulse(){
        

 micros_t = micros();
  
  if((micros_t-step1_high_t)>=step1_interval){
      step1_high_t=micros_t;
      if(step1_calisma_modu==1){
        hiz1 +=(1/hiz1)*a1;
        if(hiz1>mid_hiz)
        step1_calisma_modu=2;
      }
      else if(step1_calisma_modu==2){
        hiz1 += (1/hiz1)*a2;
        
      }
      else if(step1_calisma_modu==3){
      
        hiz1 -= (1/hiz1)*a3;
        
        if(hiz1<min_hiz){
          hiz1=min_hiz;
          step1_calisma_modu=1;
          if(digitalRead(M1_LAZER_PIN)==LOW)
          dir1=1;
          else
          dir1=-1;

          if(dir1>0)
          digitalWrite(M1_DIR_PIN,HIGH);
          else
          digitalWrite(M1_DIR_PIN,LOW);

        }
      }
    

      if(hiz1<min_hiz){    
        step1_calisma_modu=0;
      }
        
      if(hiz1>max_hiz)
        hiz1=max_hiz;

    if(step1_calisma_modu!=0){
        digitalWrite(M1_PULSE_PIN,HIGH); 
        step1_position_actual += dir1;
        step1_interval=(1/hiz1)*1000000.0;
        step1_low_t=micros_t;
        step1_low_en=true;
    }
    else
    {
      hiz1=min_hiz;
    }

  }
  else if(step1_low_en){

    if((micros_t-step1_low_t)>=50){
      step1_low_en=false;
      digitalWrite(M1_PULSE_PIN,LOW);      
    }

  }

micros_t = micros();
  if((micros_t-step2_high_t)>=step2_interval){
      step2_high_t=micros_t;
      if(step2_calisma_modu==1){
        hiz2 +=(1/hiz2)*a1;
      if(hiz2>mid_hiz)
        step2_calisma_modu=2;
      }
      else if(step2_calisma_modu==2){
        hiz2 += (1/hiz2)*a2;
        
      }
      else if(step2_calisma_modu==3){
      
        hiz2 -= (1/hiz2)*a3;
        
        if(hiz2<min_hiz){
          hiz2=min_hiz;
          step2_calisma_modu=1;
          if(digitalRead(M2_LAZER_PIN)==LOW)
          dir2=1;
          else
          dir2=-1;

          if(dir2>0)
          digitalWrite(M2_DIR_PIN,HIGH);
          else
          digitalWrite(M2_DIR_PIN,LOW);

        }
      }
    

      if(hiz2<min_hiz){    
        step2_calisma_modu=0;
      }
        
      if(hiz2>max_hiz)
        hiz2=max_hiz;

    if(step2_calisma_modu!=0){
        digitalWrite(M2_PULSE_PIN,HIGH); 
        step2_position_actual += dir2;      
        step2_interval=(1/hiz2)*1000000.0;
        step2_low_t=micros_t;
        step2_low_en=true;
        
    }
    else
    {
      hiz2=min_hiz;
    }

  }
  else if(step2_low_en){
    if((micros_t-step2_low_t)>=50){
      step2_low_en=false;
      digitalWrite(M2_PULSE_PIN,LOW);      
    }

  }
       
}

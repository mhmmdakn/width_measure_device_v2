

#include "main.h"

// 1 tur pulse sayısı = 1024pulse
// dişli çevresi = 40mm


double a1=1000; 
double a2=64000;
double a3=64000;
double a3_t=0.015;

double min_hiz=80.0; //80.0
double mid_hiz=400;  //400
double max_hiz=8000; //10000
double home_hiz=2500;  
double home_hiz_yaklasma=80.0;  


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


 

double step1_position_measured_pulse=0.0;
double step1_position_actual_pulse=0.0;
int dir1=1;
bool step1_low_en=false;
ulong step1_high_t=0;
ulong step1_low_t=0;
ulong step1_interval=0;


double step2_position_measured_pulse=0;
double step2_position_actual_pulse=0;
int dir2=1;
bool step2_low_en=false;
ulong step2_high_t=0;
ulong step2_low_t=0;
ulong step2_interval=0;

TaskHandle_t home_task_handle;



void step_init(){

 
    
  pinMode(M1_DIR_PIN,OUTPUT); 
  pinMode(M1_PULSE_PIN,OUTPUT); 
  pinMode(M1_LAZER_PIN,INPUT);
  pinMode(M1_LIMIT_POZ,INPUT);
  pinMode(M1_LIMIT_NEG,INPUT);

  pinMode(M2_DIR_PIN,OUTPUT); 
  pinMode(M2_PULSE_PIN,OUTPUT); 
  pinMode(M2_LAZER_PIN,INPUT);
  pinMode(M2_LIMIT_POZ,INPUT);
  pinMode(M2_LIMIT_NEG,INPUT);

  timer1 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, 10, true);
  timerAlarmEnable(timer1);


  attachInterrupt(M1_LAZER_PIN, lazer1_int, CHANGE);
  attachInterrupt(M2_LAZER_PIN, lazer2_int, CHANGE);
  step1_calisma_modu=0;
  step2_calisma_modu=0;

  
}


void home_task(void * parameter ){
  
  systemState=homing;
  systemStatus.home_ok=0;
  step1_calisma_modu=0;
  step2_calisma_modu=0;
  
  digitalWrite(M2_DIR_PIN,LOW);  
  int m1_home_adim=0;
  int m2_home_adim=0;
  int c=0;
  long timeout=20000;
  long start_time=millis();
  
  while (!systemStatus.home_ok)
  {
    
    if(m1_home_adim==5 && m2_home_adim==5){
      systemState=runing;
      systemStatus.home_ok=1;
      step1_position_measured_pulse=0;
      step1_position_actual_pulse=0;
      step2_position_measured_pulse=0;
      step2_position_actual_pulse=0;

      step1_calisma_modu=3;
      step2_calisma_modu=3;
      break;
    }
    if((millis()-start_time)>timeout){
      systemState=error;
      systemError[home_error]=1;
      //todo: error.cpp de error algılanıp tepki verilecek. Reset sistemi oluşturulacak.
    }

    


    /////////////  M1   //////////////
    switch (m1_home_adim)
    {
      //todo:travel error eklenecek, adimlar için timeout eklenebilir.
    case 0:
      if(digitalRead(M1_LIMIT_NEG))
        m1_home_adim=2;
      else
        m1_home_adim=1;
      break;
    case 1:
      digitalWrite(M1_DIR_PIN,LOW);
      step1_calisma_modu=4;//hızlı geriye doğru ilerle
      if(digitalRead(M1_LIMIT_NEG)){//limiti görüdüğünde
        step1_calisma_modu=6;//rampalı dur
        m1_home_adim=2;
      }
        
      break;
    case 2:
      if(step1_calisma_modu==0){
        digitalWrite(M1_DIR_PIN,HIGH);
        step1_calisma_modu=5;//yavaş ileriye doğru ilerle
        m1_home_adim=3;
      }
      
    break;

    case 3:
      if(!digitalRead(M1_LIMIT_NEG)){//limiti görmediğinde
        step1_calisma_modu=0;//rampasız dur
        if(c<3){
          m1_home_adim=4;//tekrar et
          c++;
        }
        else{
          m1_home_adim=5;//tamamlandı
          
        }
      }
      break; 

    case 4:
      digitalWrite(M1_DIR_PIN,LOW);
      step1_calisma_modu=5;//hızlı geriye doğru ilerle
      if(digitalRead(M1_LIMIT_NEG)){//limiti görüdüğünde
        step1_calisma_modu=0;//rampalı dur
        m1_home_adim=2;
      }
        
      break;   
    
    default:
      break;
    }

    /////////////  M2   //////////////
    switch (m2_home_adim)
    {
      //todo:travel error eklenecek, adimlar için timeout eklenebilir.
    case 0:
      if(digitalRead(M1_LIMIT_NEG))
        m2_home_adim=2;
      else
        m2_home_adim=1;
      break;
    case 1:
      digitalWrite(M2_DIR_PIN,LOW);
      step2_calisma_modu=4;//hızlı geriye doğru ilerle
      if(digitalRead(M2_LIMIT_NEG)){//limiti görüdüğünde
        step2_calisma_modu=6;//rampalı dur
        m2_home_adim=2;
      }
        
      break;
    case 2:
      if(step2_calisma_modu==0){
        digitalWrite(M2_DIR_PIN,HIGH);
        step2_calisma_modu=5;//yavaş ileriye doğru ilerle
        m2_home_adim=3;
      }
      
    break;

    case 3:
      if(!digitalRead(M2_LIMIT_NEG)){//limiti görmediğinde
        step2_calisma_modu=0;//rampasız dur
        if(c<3){
          m2_home_adim=4;//tekrar et
          c++;
        }
        else{
          m2_home_adim=5;//tamamlandı
          
        }
      }
      break; 

    case 4:
      digitalWrite(M2_DIR_PIN,LOW);
      step2_calisma_modu=5;//hızlı geriye doğru ilerle
      if(digitalRead(M2_LIMIT_NEG)){//limiti görüdüğünde
        step2_calisma_modu=0;//rampalı dur
        m2_home_adim=2;
      }
        
      break;   
    
    default:
      break;
    }
 


    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  
  vTaskDelete(home_task_handle); 
  
}

void home_init(){
  xTaskCreatePinnedToCore(home_task,"HomeTask",1000,NULL,3,&home_task_handle,1);
  //todo: ram ve prirotiy kontrol edilecek.
}





void IRAM_ATTR onTimer1() {
  timerStop(timer1);

  step_pulse();
  
  timerStart(timer1);
  
}

void IRAM_ATTR lazer1_int() {

  if(systemState==runing)
   {
    if(digitalRead(M1_LAZER_PIN)==LOW)
    step1_position_measured_pulse = step1_position_actual_pulse;

    a3=hiz1/a3_t;
    step1_calisma_modu=3;
   }

  
  
}

void IRAM_ATTR lazer2_int() {

   if(systemState==runing){
    if(digitalRead(M2_LAZER_PIN)==LOW )
    step2_position_measured_pulse = step2_position_actual_pulse;

    a3=hiz2/a3_t;
    step2_calisma_modu=3;
   }
    

  
  
}

void step_pulse(){
        
///////////////////// STEP1 ///////////////////////
 micros_t = micros();
  
  if((micros_t-step1_high_t)>=step1_interval){
      step1_high_t=micros_t;

      
      if(step1_calisma_modu==1){//1. kalkış rampası eğim a1 en yüksek hızı mid_hiz
        hiz1 +=(1/hiz1)*a1;
        if(hiz1>mid_hiz)
        step1_calisma_modu=2;
      }      
      else if(step1_calisma_modu==2){//2. kalkış rampası eğim a2 en yüksek hızı max_hiz
        hiz1 += (1/hiz1)*a2;        
      }
      else if(step1_calisma_modu==3){ //iniş rampası eğim a3 en düşük hızı min_hiz      
        hiz1 -= (1/hiz1)*a3;        
        if(hiz1<min_hiz){
          hiz1=min_hiz;
          step1_calisma_modu=1;
          if(digitalRead(M1_LAZER_PIN)==LOW) //eksi yön,görüyorken lazer=LOW 
          dir1=-1;
          else                               //artı yön,görümüyorken lazer=HIGH 
          dir1=1;

          if(dir1<0)
          digitalWrite(M1_DIR_PIN,LOW); //eksi yön dir=LOW
          else
          digitalWrite(M1_DIR_PIN,HIGH); // artı yön dir=HIGH


        }
      }
      else if(step1_calisma_modu==4){
        hiz1 += (1/hiz1)*a1;
        if(hiz1>home_hiz)
          hiz1=home_hiz;
      }
      else if(step1_calisma_modu==5){
        hiz1 += (1/hiz1)*a1;
        if(hiz1>home_hiz_yaklasma)
          hiz1=home_hiz_yaklasma;
      }
      else if(step1_calisma_modu==6){
        hiz1 -= (1/hiz1)*a3;        
        if(hiz1<min_hiz){
          hiz1=min_hiz;
          step1_calisma_modu=0;
        }
      }
      
    

      if(hiz1<min_hiz){    
        step1_calisma_modu=0;
      }
        
    if(hiz1>max_hiz)
      hiz1=max_hiz;
    
    if(step1_calisma_modu!=0){
      if(systemStatus.m1.limit_ok){
        digitalWrite(M1_PULSE_PIN,HIGH); 
        step1_position_actual_pulse += dir1;
        step1_interval=(1/hiz1)*1000000.0;
        step1_low_t=micros_t;
        step1_low_en=true;
      }
      else{
        step1_calisma_modu=3;
      }
        
    }
    else// step1_calisma_modu==0
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
///////////////////// STEP2 ///////////////////////
micros_t = micros();
  if((micros_t-step2_high_t)>=step2_interval){
      step2_high_t=micros_t;

      //1. kalkış rampası eğim a1 en yüksek hızı mid_hiz
      if(step2_calisma_modu==1){
        hiz2 +=(1/hiz2)*a1;
      if(hiz2>mid_hiz)
        step2_calisma_modu=2;
      }
      //2. kalkış rampası eğim a2 en yüksek hızı max_hiz
      else if(step2_calisma_modu==2){
        hiz2 += (1/hiz2)*a2;
        
      }
      //iniş rampası eğim a3 en düşük hızı min_hiz
      else if(step2_calisma_modu==3){
      
        hiz2 -= (1/hiz2)*a3;
        
        if(hiz2<min_hiz){
          hiz2=min_hiz;
          step2_calisma_modu=1;
          if(digitalRead(M2_LAZER_PIN)==LOW)//eksi yön,görüyorken lazer=LOW
          dir2=-1;
          else                              //artı yön,görümüyorken lazer=HIGH
          dir2=1;

          if(dir2<0)
          digitalWrite(M2_DIR_PIN,LOW); //eksi yön dir=LOW
          else
          digitalWrite(M2_DIR_PIN,HIGH); // artı yön dir=HIGH

        }
      }
      else if(step2_calisma_modu==4){
        hiz2 += (1/hiz2)*a1;
        if(hiz2>home_hiz)
          hiz2=home_hiz;
      }
      else if(step2_calisma_modu==5){
        hiz2 += (1/hiz2)*a1;
        if(hiz2>home_hiz_yaklasma)
          hiz2=home_hiz_yaklasma;
      }
      else if(step2_calisma_modu==6){
        hiz2 -= (1/hiz2)*a3;        
        if(hiz2<min_hiz){
          hiz2=min_hiz;
          step2_calisma_modu=0;
        }
      }
    

      if(hiz2<min_hiz){    
        step2_calisma_modu=0;
      }
        
      if(hiz2>max_hiz)
        hiz2=max_hiz;

    if(step2_calisma_modu!=0){  

        if(systemStatus.m2.limit_ok){
        digitalWrite(M2_PULSE_PIN,HIGH); 
        step2_position_actual_pulse += dir2;
        step2_interval=(1/hiz2)*1000000.0;
        step2_low_t=micros_t;
        step2_low_en=true;
      }
      else{
        step2_calisma_modu=3;
      }
    }
    else// step2_calisma_modu==0
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


#include "main.h"

TON M1_LimitNeg_Timer(10);
TON M1_LimitPoz_Timer(50);
TON M2_LimitNeg_Timer(50);
TON M2_LimitPoz_Timer(50);





State systemState;
Status systemStatus;
bool systemError[Error_Count];

double referans_uzunluk_mm=2400;
double mm_pulse = 40.0/1024.0;





void system_task(void * parameter ){

    for (;;)
    {
      
      
      systemStatus.m1.limit_neg=digitalRead(M1_LIMIT_NEG);
      systemStatus.m1.limit_poz=digitalRead(M1_LIMIT_POZ);
      systemStatus.m1.lazer=digitalRead(M1_LAZER_PIN);
      systemStatus.m1.dir=digitalRead(M1_DIR_PIN);
      systemStatus.m2.limit_neg=digitalRead(M2_LIMIT_NEG);
      systemStatus.m2.limit_poz=digitalRead(M2_LIMIT_POZ);
      systemStatus.m2.lazer=digitalRead(M2_LAZER_PIN);   
      systemStatus.m2.dir=digitalRead(M2_DIR_PIN);


      if(systemState!=homing){
       
       systemStatus.m1.limit_ok=!((systemStatus.m1.limit_neg&&(!systemStatus.m1.dir))||(systemStatus.m1.limit_poz&&systemStatus.m1.dir));
       systemStatus.m2.limit_ok=!(systemStatus.m2.limit_neg&&(!systemStatus.m2.dir)||(systemStatus.m2.limit_poz&&systemStatus.m2.dir));
       systemStatus.limit_ok=systemStatus.m1.limit_ok&&systemStatus.m2.limit_ok;
      }
      else{
        systemStatus.m1.limit_ok=!(systemStatus.m1.limit_poz&&systemStatus.m1.dir);
        systemStatus.m2.limit_ok=!(systemStatus.m2.limit_poz&&systemStatus.m2.dir);
        systemStatus.limit_ok=systemStatus.m1.limit_ok&&systemStatus.m2.limit_ok;
      }
      vTaskDelay(10/portTICK_PERIOD_MS);
    }
    
}

void update_task(void * parameter ){

    double step_position_pulse=0.0;
    
    for (;;)
    {
      
      RA.writeUShort(step1_pos_meas_mm,step1_position_measured_pulse*mm_pulse);
      RA.writeUShort(step1_pos_actual_mm,step1_position_actual_pulse*mm_pulse);
      RA.writeUShort(step2_pos_meas_mm,step2_position_measured_pulse*mm_pulse);
      RA.writeUShort(step2_pos_actual_mm,step2_position_actual_pulse*mm_pulse);

      step_position_pulse = step1_position_measured_pulse+step2_position_measured_pulse; 
      RA.writeUShort(step_position_mm , step_position_pulse*mm_pulse);
      ushort s_pm_mm=RA.readUShort(step_position_mm);
      RA.writeUShort( en_mm,referans_uzunluk_mm - s_pm_mm);
      
      analogOut_Update();

      
      vTaskDelay(10/portTICK_PERIOD_MS);
    }
     
}

void sys_init(){

  Serial.begin(115200);
  pinMode(0,OUTPUT);
  digitalWrite(0,HIGH);

  systemStatus.home_ok=false;
  limit_init();
  register_init();
  //rs485_init();
  //i2c_init();
  step_init();
  analogOut_init(1);
  //wifi_webserver_init();
  //ethernet_init();
  xTaskCreatePinnedToCore(system_task,"SystemTask",1000,NULL,4,NULL,1);
  xTaskCreatePinnedToCore(update_task,"UpdateTask",1000,NULL,3,NULL,1); 

  home_init();
}


void sys_loop(){
  //read_vcc();
  //analogOut_Update();

  // Serial.print("M1_LimNeg:");
  // Serial.print(systemStatus.m1.limit_neg);
  // Serial.print("-M1_LimPoz:");
  // Serial.print(systemStatus.m1.limit_poz);
  // Serial.print("-M1_LimLazer:");
  // Serial.print(systemStatus.m1.lazer);
  // Serial.print("-M1_Dir:");
  // Serial.print(systemStatus.m1.dir);
  // Serial.print("-M2_LimNeg:");
  // Serial.print(systemStatus.m2.limit_neg);
  // Serial.print("-M2_LimPoz:");
  // Serial.println(systemStatus.m2.limit_poz);
  // Serial.print("-M2_LimLazer:");
  // Serial.println(systemStatus.m2.lazer);
  // Serial.print("-M2_Dir:");
  // Serial.print(systemStatus.m2.dir);

   // Serial.println(RA.readUShort(en_mm));

  // Serial.println("merhaba");
  // delay(1000);
}

#include "main.h"


State systemState;
Status systemStatus;
bool systemError[Error_Count];



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
      double _mm_pulse=RK.readDouble(mm_pulse);
      double _referans_uzunluk_mm=RK.readDouble(referans_uzunluk_mm);
      RA.writeUShort(step1_pos_meas_mm,step1_position_measured_pulse*_mm_pulse);
      RA.writeUShort(step1_pos_actual_mm,step1_position_actual_pulse*_mm_pulse);
      RA.writeUShort(step2_pos_meas_mm,step2_position_measured_pulse*_mm_pulse);
      RA.writeUShort(step2_pos_actual_mm,step2_position_actual_pulse*_mm_pulse);

      step_position_pulse = step1_position_measured_pulse+step2_position_measured_pulse; 
      RA.writeUShort(step_position_mm , step_position_pulse*_mm_pulse);
      ushort s_pm_mm=RA.readUShort(step_position_mm);
      RA.writeUShort( en_mm,_referans_uzunluk_mm - s_pm_mm);
      
      //analogOut_Update();
      
      vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void sys_init(){
  Serial.begin(115200);
  SPIFFS.begin();
  register_init();
  default_init();
  limit_init();
  //rs485_init();<---------------------------------------------------//aktif edilecek
  //ethernet_init();  
  analogOut_init(1,RK.readUShort(width_mode));
  wifi_init();
 
  // //i2c_init();

   xTaskCreatePinnedToCore(system_task,"SystemTask",1000,NULL,4,NULL,1);
   xTaskCreatePinnedToCore(update_task,"UpdateTask",1000,NULL,3,NULL,1); 
 
   step_init();
   home_init();

}

void sys_loop(){


   if(RK.readShort(set_init)==1){        
    RK.writeShort(set_init,0);        
    RK.commit();   
    //home_init();
  }
  else if(RK.readShort(set_init)==11){   
   RK.commit();
   ESP.restart();
  }
  wifi_loop();
  //ethernet_loop();
  //rs485_loop(); <---------------------------------------------------//aktif edilecek
  //read_vcc();
  analogOut_Update();


  // Serial.print("M1_LimNeg:");
  // Serial.print(systemStatus.m1.limit_neg);
  // Serial.print("-M1_LimPoz:");
  // Serial.print(systemStatus.m1.limit_poz);
  // Serial.print("-M1_Lazer:");
  // Serial.print(systemStatus.m1.lazer);
  // Serial.print("-M1_Dir:");
  // Serial.print(systemStatus.m1.dir);
  // Serial.print("-M2_LimNeg:");
  // Serial.print(systemStatus.m2.limit_neg);
  // Serial.print("-M2_LimPoz:");
  // Serial.println(systemStatus.m2.limit_poz);
  // Serial.print("-M2_Lazer:");
  // Serial.println(systemStatus.m2.lazer);
  // Serial.print("-M2_Dir:");
  // Serial.print(systemStatus.m2.dir);

   // Serial.println(RA.readUShort(en_mm));

  // Serial.println("merhaba");
  // delay(1000);
}
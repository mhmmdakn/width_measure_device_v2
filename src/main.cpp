
#include "main.h"
#include "ESPAsyncWebServer.h"

void setup()
{

  sys_init();
  esp_log_level_set("*", ESP_LOG_NONE);
}
int calisma_mode = 0;
int calisma_mode_temp = 0;

void loop()
{

  sys_loop();

  //   if (Serial.available()) {
  //   String num = Serial.readString();
  //   calisma_mode=num.toInt();
  //   Serial.print("Duty received:");
  //   Serial.println(num);

  // }
  //   if(calisma_mode!=calisma_mode_temp){
  //   step1_calisma_modu=calisma_mode;
  //   step2_calisma_modu=calisma_mode;

  //   calisma_mode_temp=calisma_mode;
  // }
}

#pragma once

#include <EEPROM.h>

   
  enum{
  
  eth_config_size    
  };///ETHERNET_CONFIG
  
enum{
  set_init=0,  
  slave_id=set_init+2,   
  sta_ssid=slave_id+2,
  sta_password=sta_ssid+32,
  ap_ssid=sta_password+32,
  ap_password=ap_ssid+32,
  webserver_username=ap_password+32,
  webserver_password=webserver_username+32,
  sta_ip=webserver_password+32,
  sta_subnet=sta_ip+4,
  sta_gateway=sta_subnet+4,
  sta_dns=sta_gateway+4,
  wifi_ntp=sta_dns+4,
  eth_ip=wifi_ntp+4,
  eth_subnet=eth_ip+4,  
  eth_gateway=eth_subnet+4,
  eth_dns=eth_gateway+4,
  eth_ntp=eth_dns+4,
  width_mode=eth_ntp+4,
  referans_uzunluk_mm=width_mode+2,
  mm_pulse=referans_uzunluk_mm+8,
  device_config_size,
  };///DEVICE_CONFIG
  
	enum{	
  step1_pos_meas_mm=0,
  step1_pos_actual_mm=step1_pos_meas_mm+2,
  step2_pos_meas_mm=step1_pos_actual_mm+2,
  step2_pos_actual_mm=step2_pos_meas_mm+2,
  step_position_mm=step2_pos_actual_mm+2, 
  en_mm=step_position_mm+2,
	device_register_size,
	};///DEVICE_REGISTER

class  RegisterCollection : public  EEPROMClass{

    public :
    bool begin(size_t size);
};

extern RegisterCollection RA;
extern EEPROMClass RK;
extern map<String,int> ht_enum;

 void register_init();
  

 



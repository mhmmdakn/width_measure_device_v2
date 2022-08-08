#include "main.h"

word device_register[device_register_size];
word device_config[device_config_size];
String wifi_begin[wifi_begin_size];
byte wifi_config[wifi_config_size][wifi_config_lenght];
byte eth_config[eth_config_size][eth_config_lenght];


RegisterCollection RA;
EEPROMClass RK;






bool RegisterCollection::begin(size_t size){
        _data=(uint8_t*) malloc(size);
        _size=size;
        return false;
}


void register_init(){
   
    RA.begin(512);
    RK.begin(512);
    
    // pair<int, JSONVar*> p;
    // p= std::make_pair(10, &en_anlik_val); 
    // en_anlik_val=100.0;
    //ht_param_kalici.insert( std::pair<int, JSONVar*>(10, &en_anlik_val));
    
    
  
}
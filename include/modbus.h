#pragma once
#include <Arduino.h>
#include <protocol.h>

enum CONNECTION_TYPE{
    WIFI_T,
    ETH_T,
    SERIAL_T
};


void mb_read_holding_register(int start_address,int number_of_value,int timeout=1000);
void mb_write_holding_register(int start_address,int number_of_value,int timeout=1000);
bool checkRX();


extern uint8_t data[128];
extern uint8_t send_data[128];
template<typename T> bool handleModbusData(T port,MB_FRAME_TYPE type,int delayTime=130){
    
    
    
    if (port.available()) 
    { 
        
        int len=0;
        while(port.available()){
            data[len] = (uint8_t)port.read();
            len++;
            if(type==RTU){
                delayMicroseconds(delayTime);
            }
            
        }
        
        if(len>7){
            

            int send_len=ReceiveParsing(data,len,send_data,type);
        
            port.write(send_data,send_len); 
        }
        
        
    }

    return true;
}


#include "main.h"

      RS485::RS485(HardwareSerial* serial){
         _serial=serial;
         
      }
      RS485::~RS485(){}
      void RS485::begin(long baudrate,int de_re_pin){

         _serial->begin(baudrate);
          _de_re_pin=de_re_pin;    
          pinMode(de_re_pin,OUTPUT);
          digitalWrite(de_re_pin,LOW);

      }


      int RS485::peek(){

         return _serial->peek();
	      
      }

      size_t RS485::write(uint8_t byte){
       
         return _serial->write(byte);
         
      }
      size_t RS485::write(uint8_t* buffer,size_t size){
         digitalWrite(_de_re_pin,HIGH);
         
         size_t s= _serial->write(buffer,size);
         _serial->flush();
         delayMicroseconds(260);
         digitalWrite(_de_re_pin,LOW);
         
         return s;
      }
      int RS485::read(){
        return _serial->read();
	      
      }
      int RS485::available(){
         return _serial->available();
	      
      }
      void RS485::flush(){
         return _serial->flush();
      }








// void sendPacket(unsigned char bufferSize)
// {
//   digitalWrite(TxEnablePin, HIGH);
		
//   for (unsigned char i = 0; i < bufferSize; i++)
//     (*ModbusPort).write(frame[i]);
		
// 	(*ModbusPort).flush();
	
// 	// allow a frame delay to indicate end of transmission
// 	delayMicroseconds(T3_5); 
	
//   digitalWrite(TxEnablePin, LOW);
// }
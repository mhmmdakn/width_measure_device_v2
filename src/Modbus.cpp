#include "main.h"

uint8_t data[128];
uint8_t send_data[128];
// template<typename T> bool handleModbusData(T port,MB_FRAME_TYPE type){

//     if (port)
//     {
//         Serial.println("0");
//         int len=0;
//         while(port.available()){
//             data[len] = (uint8_t)port.read();
//             len++;
//         }
//         Serial.println("1");
//         int send_len=ReceiveParsing(data,len,send_data,type);
//         Serial.println("2");

//         port.write(send_data,send_len);
//         Serial.println("3");

//     }

//     return true;
// }

// uint8_t tx_buffer[128];
// uint8_t rx_buffer[128];
// word modbus_start_address=0;
// byte byteFNRx = MB_FC_NONE;
// bool modbus_busy=false;

// bool checkRX(){

//   if (Wire.available()>0) {

//       byteFNRx=MB_FC_NONE;

//       int size =Wire.readBytes(rx_buffer,Wire.available());

//       // for (size_t i = 0; i <size ; i++)
//       // {
//       //   Serial.print(rx_buffer[i]);
//       // }
//       // Serial.println();

//       modbus_busy=false;

//       //// rutine Modbus TCP
//       byteFNRx = rx_buffer[MB_TCP_FUNC];

//      // Handle request

//      switch(byteFNRx) {
//         case MB_FC_NONE:
//         return false;
//         break;

//         case MB_FC_READ_REGISTERS: // 03 Read Holding Registers
//          // Serial.print("Value:");
//           for(int i = 0; i < (rx_buffer[MB_TCP_RX_BYTE_LEN]/2); i++)
//           {
//             byteFNRx = MB_FC_NONE;

//               byte tempLow=rx_buffer[(MB_TCP_RX_DATA+1) + (i * 2)];
//               byte tempHigh=rx_buffer[ MB_TCP_RX_DATA + (i * 2)];

//               device_register[modbus_start_address+i]=makeWord(tempHigh,tempLow);
//         //  Serial.print(device_register[modbus_start_address+i]);
//          // Serial.print(",");

//           }
//          // Serial.println();

//           digitalWrite(0,HIGH);
//           delayMicroseconds(10);
//           digitalWrite(0,LOW);

//         break;

//         case MB_FC_WRITE_REGISTER:
//         case MB_FC_WRITE_MULTIPLE_REGISTERS: // 06 Write Holding Register

//         break;

//     }

//   }
//  return true;

// }

// ulong modbus_t=0;
// ulong modbus_timeout=0;
// void mb_read_holding_register(int start_address,int number_of_value,int timeout)
// {

//   if(modbus_busy){
//     if((millis()-modbus_t)<modbus_timeout)
//       return;
//   }
//       digitalWrite(0,HIGH);
//       delayMicroseconds(10);
//       digitalWrite(0,LOW);
//   modbus_start_address=start_address;
//   modbus_t=millis();
//   modbus_timeout=timeout;
//   modbus_busy=true;
//   int ByteDataLength = number_of_value * 2;
//   int MessageLength = 11;
//   tx_buffer[MB_TCP_LEN] =highByte(ByteDataLength + 4);
//   tx_buffer[MB_TCP_LEN+1] =lowByte(ByteDataLength + 4);
//   tx_buffer[MB_TCP_FUNC]=MB_FC_READ_REGISTERS;
//   tx_buffer[MB_TCP_REGISTER_START]=highByte(start_address);
//   tx_buffer[MB_TCP_REGISTER_START+1]=lowByte(start_address);

//   tx_buffer[MB_TCP_REGISTER_NUMBER]=highByte(number_of_value);
//   tx_buffer[MB_TCP_REGISTER_NUMBER+1]=lowByte(number_of_value);

//   Wire.beginTransmission(4);
//   Wire.write(tx_buffer,MessageLength);
//   Wire.endTransmission();
//   Wire.requestFrom(4,ByteDataLength+9);

// }

// void mb_write_holding_register(int start_address,int number_of_value,int timeout)
// {

//  if(modbus_busy){
//     if((millis()-modbus_t)<modbus_timeout)
//       return;
//   }
//    modbus_start_address=start_address;
//   modbus_t=millis();
//   modbus_timeout=timeout;
//   modbus_busy=true;
//   int ByteDataLength=number_of_value * 2;
//   int MessageLength = ByteDataLength + 13;
//   tx_buffer[MB_TCP_FUNC]=MB_FC_WRITE_MULTIPLE_REGISTERS;
//   tx_buffer[MB_TCP_REGISTER_START]=highByte(start_address);
//   tx_buffer[MB_TCP_REGISTER_START+1]=lowByte(start_address);

//   tx_buffer[MB_TCP_LEN] =highByte(ByteDataLength + 7);
//   tx_buffer[MB_TCP_LEN+1] =lowByte(ByteDataLength + 7);

//   tx_buffer[MB_TCP_REGISTER_NUMBER] = highByte(number_of_value);
//   tx_buffer[MB_TCP_REGISTER_NUMBER+1] = lowByte(number_of_value);
//   tx_buffer[MB_TCP_BYTE_DATA_LEN] = ByteDataLength;
//   for(int i = 0; i < number_of_value; i++)
//   {
//   tx_buffer[ MB_TCP_WRITE_DATA + (i * 2)] = highByte(device_register[start_address + i]);// high byte
//   tx_buffer[MB_TCP_WRITE_DATA+1 + (i * 2)] = lowByte(device_register[start_address + i]);// low byte
//   }
//   Wire.beginTransmission(4);
//   Wire.write(tx_buffer,MessageLength);
//   Wire.endTransmission();
//   Wire.requestFrom(4,12); //fdvdf
// }
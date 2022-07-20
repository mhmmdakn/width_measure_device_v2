#pragma once

#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3 //implemented
#define MB_FC_WRITE_REGISTER 6 //implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 //implemented
#define MB_FC_USER 80 //implemented
//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4
//
// MODBUS MBAP offsets
//
#define MB_TCP_TID 0 //2byte
#define MB_TCP_PID 2 //2byte
#define MB_TCP_LEN 4 //2byte
#define MB_TCP_UID 6 //1byte
#define MB_TCP_FUNC 7 //1byte
#define MB_TCP_RX_BYTE_LEN 8 //1byte
#define MB_TCP_RX_DATA 9 //2byte
#define MB_TCP_REGISTER_START 8 //2byte
#define MB_TCP_REGISTER_NUMBER 10 //2byte
#define MB_TCP_BYTE_DATA_LEN 12 //1byte
#define MB_TCP_WRITE_DATA 13 //2byte


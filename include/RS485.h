#pragma once
#include <Arduino.h>

class  RS485 :  public Stream
{

private:
    int _de_re_pin;
    HardwareSerial* _serial;
public:

    RS485(HardwareSerial* serial);
    ~RS485();
    void begin(long baudrate,int de_re_pin);


    virtual int peek();
    virtual size_t write(uint8_t byte);
    virtual size_t write(uint8_t* buffer,size_t size);
    virtual int read();
    virtual int available();
    virtual void flush();
    operator bool() { return true; }

   // using Print::write;
};


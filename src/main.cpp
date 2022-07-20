#include <Arduino.h>
#include <Wire.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DAC_PWM1 26
#define DAC_PWM2 27

#define M1_LIMIT_POZ 39
#define M1_LIMIT_NEG 34
#define M1_LAZER_PIN 35
#define M1_DIR_PIN 32
#define M1_PULSE_PIN 33


#define M2_LIMIT_POZ 15
#define M2_LIMIT_NEG 2
#define M2_LAZER_PIN 4
#define M2_DIR_PIN 16
#define M2_PULSE_PIN 17



#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04

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



const int LEDPin = 25;  /* GPIO16 */

int dutyCycle;
/* Setting PWM Properties */
const int PWMFreq = 20000; /* 5 KHz */
const int PWMChannel = 0;
const int PWMResolution = 12;
const int MAX_DUTY_CYCLE = (int)(pow(2, PWMResolution) - 1);
static esp_adc_cal_characteristics_t *adc_chars;
static const adc1_channel_t channel = ADC1_CHANNEL_0;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;
#define NO_OF_SAMPLES   64          //Multisampling
#define DEFAULT_VREF    1100





word device_register[32];

uint8_t tx_buffer[128];
uint8_t rx_buffer[128];
word modbus_start_address=0;
byte byteFNRx = MB_FC_NONE;
bool modbus_busy=false;
bool request=false;
byte request_index=0;

 
double a1=1000; 
double a2=64000;
double a3=64000;
double a3_t=0.015;

double min_hiz=80.0;
double mid_hiz=400;  
double max_hiz=10000; 
double rampa_x=(pow(max_hiz,2)-pow(min_hiz,2))/(2*a2);
double rampa_2x=2*rampa_x;
double hiz1=min_hiz;
double hiz2=min_hiz;



int step1_calisma_modu=0;
int step2_calisma_modu=0;

bool setpin=false;

hw_timer_t * timer0 = NULL;
hw_timer_t * timer1 = NULL;
hw_timer_t * timer2 = NULL;
hw_timer_t * timer3 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


 

double step1_position_measured=0;
double step1_position_actual=0;
int dir1=1;
bool step1_low_en=false;
ulong step1_high_t=0;
ulong step1_low_t=0;
ulong step1_interval=0;


double step2_position_measured=0;
double step2_position_actual=0;
int dir2=1;
bool step2_low_en=false;
ulong step2_high_t=0;
ulong step2_low_t=0;
ulong step2_interval=0;


ulong micros_t=0;

TaskHandle_t task1;

void mb_read_holding_register(int start_address,int number_of_value,int timeout=1000);
void mb_write_holding_register(int start_address,int number_of_value,int timeout=1000);
void read_vcc();
bool checkRX();
void step_pulse();
void IRAM_ATTR onTimer1();
void IRAM_ATTR lazer1_int();
void IRAM_ATTR lazer2_int();


void setup() {
 
  Serial.begin(115200);
 
  pinMode(M1_DIR_PIN,OUTPUT); 
  pinMode(M1_PULSE_PIN,OUTPUT); 
  pinMode(M1_LAZER_PIN,INPUT);

  pinMode(M2_DIR_PIN,OUTPUT); 
  pinMode(M2_PULSE_PIN,OUTPUT); 
  pinMode(M2_LAZER_PIN,INPUT);
  pinMode(0,OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN,1000000);   // join i2c bus
    
  attachInterrupt(M1_LAZER_PIN, lazer1_int, CHANGE);
  attachInterrupt(M2_LAZER_PIN, lazer2_int, CHANGE);
 

  step1_calisma_modu=1;
  step2_calisma_modu=1;

  timer1 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, 10, true);
  timerAlarmEnable(timer1);

  ledcSetup(PWMChannel, PWMFreq, PWMResolution);
  /* Attach the LED PWM Channel to the GPIO Pin */
  ledcAttachPin(DAC_PWM1, PWMChannel);
 
  adc1_config_width(width);
  adc1_config_channel_atten(channel, atten);
}
 



float serial_target=0;
int rezonans=0;
bool led=false;
int tempduty=0;
int duty=2047;
void loop() {
 


    if (Serial.available()) {
      String num = Serial.readString();
      duty=num.toInt();
      Serial.print("Duty received:");
      Serial.println(num);
    }

    if(duty!=tempduty){
      ledcWrite(PWMChannel, duty);
      tempduty=duty;
    }


   read_vcc();
    
    mb_write_holding_register(0,1);

    checkRX();
  
  // if (Serial.available()) {
  //     String num = Serial.readStringUntil('\n');
  //     step1_calisma_modu=num.toInt();
  //     step2_calisma_modu=num.toInt();

  //     Serial.print("Num:");
  //     Serial.println(num);
  // }
  
    // Serial.print("Step1:");
    // Serial.println(step1_position_measured);
    // Serial.print("Step2:");
    // Serial.println(step2_position_measured);
    
 
}

void read_vcc(){

adc_chars = (esp_adc_cal_characteristics_t*)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

   uint32_t adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, width, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        device_register[0]=voltage;
        
        // Serial.print("Raw:");
        // Serial.print(adc_reading);
        // Serial.print("\tVoltage");
        // Serial.print(voltage);
        // Serial.println("mV");

       // printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
        
}


void IRAM_ATTR onTimer1() {
  timerStop(timer1);

  step_pulse();
  
  timerStart(timer1);
  
}

void IRAM_ATTR lazer1_int() {
  
  
  if(digitalRead(M1_LAZER_PIN)==LOW && step1_calisma_modu==1)
  step1_position_measured = step1_position_actual;
  a3=hiz1/a3_t;
  step1_calisma_modu=3;
  
}

void IRAM_ATTR lazer2_int() {
  
  if(digitalRead(M2_LAZER_PIN)==LOW && step2_calisma_modu==1)
  step2_position_measured = step2_position_actual;
  a3=hiz2/a3_t;
  step2_calisma_modu=3;
  
}

void step_pulse(){
        

 micros_t = micros();
  
  if((micros_t-step1_high_t)>=step1_interval){
      step1_high_t=micros_t;
      if(step1_calisma_modu==1){
        hiz1 +=(1/hiz1)*a1;
        if(hiz1>mid_hiz)
        step1_calisma_modu=2;
      }
      else if(step1_calisma_modu==2){
        hiz1 += (1/hiz1)*a2;
        
      }
      else if(step1_calisma_modu==3){
      
        hiz1 -= (1/hiz1)*a3;
        
        if(hiz1<min_hiz){
          hiz1=min_hiz;
          step1_calisma_modu=1;
          if(digitalRead(M1_LAZER_PIN)==LOW)
          dir1=1;
          else
          dir1=-1;

          if(dir1>0)
          digitalWrite(M1_DIR_PIN,HIGH);
          else
          digitalWrite(M1_DIR_PIN,LOW);

        }
      }
    

      if(hiz1<min_hiz){    
        step1_calisma_modu=0;
      }
        
      if(hiz1>max_hiz)
        hiz1=max_hiz;

    if(step1_calisma_modu!=0){
        digitalWrite(M1_PULSE_PIN,HIGH); 
        step1_position_actual += dir1;
        step1_interval=(1/hiz1)*1000000.0;
        step1_low_t=micros_t;
        step1_low_en=true;
    }
    else
    {
      hiz1=min_hiz;
    }

  }
  else if(step1_low_en){

    if((micros_t-step1_low_t)>=50){
      step1_low_en=false;
      digitalWrite(M1_PULSE_PIN,LOW);      
    }

  }

micros_t = micros();
  if((micros_t-step2_high_t)>=step2_interval){
      step2_high_t=micros_t;
      if(step2_calisma_modu==1){
        hiz2 +=(1/hiz2)*a1;
      if(hiz2>mid_hiz)
        step2_calisma_modu=2;
      }
      else if(step2_calisma_modu==2){
        hiz2 += (1/hiz2)*a2;
        
      }
      else if(step2_calisma_modu==3){
      
        hiz2 -= (1/hiz2)*a3;
        
        if(hiz2<min_hiz){
          hiz2=min_hiz;
          step2_calisma_modu=1;
          if(digitalRead(M2_LAZER_PIN)==LOW)
          dir2=1;
          else
          dir2=-1;

          if(dir2>0)
          digitalWrite(M2_DIR_PIN,HIGH);
          else
          digitalWrite(M2_DIR_PIN,LOW);

        }
      }
    

      if(hiz2<min_hiz){    
        step2_calisma_modu=0;
      }
        
      if(hiz2>max_hiz)
        hiz2=max_hiz;

    if(step2_calisma_modu!=0){
        digitalWrite(M2_PULSE_PIN,HIGH); 
        step2_position_actual += dir2;      
        step2_interval=(1/hiz2)*1000000.0;
        step2_low_t=micros_t;
        step2_low_en=true;
        
    }
    else
    {
      hiz2=min_hiz;
    }

  }
  else if(step2_low_en){
    if((micros_t-step2_low_t)>=50){
      step2_low_en=false;
      digitalWrite(M2_PULSE_PIN,LOW);      
    }

  }
       
}


bool checkRX(){



  if (Wire.available()>0) {

    
      byteFNRx=MB_FC_NONE;
    
      int size =Wire.readBytes(rx_buffer,Wire.available());

      // for (size_t i = 0; i <size ; i++)
      // {
      //   Serial.print(rx_buffer[i]);
      // }
      // Serial.println();

      modbus_busy=false;
     

      //// rutine Modbus TCP
      byteFNRx = rx_buffer[MB_TCP_FUNC];      
      
     
    
     
     // Handle request
     
     switch(byteFNRx) {
        case MB_FC_NONE:
        return false; 
        break;

        case MB_FC_READ_REGISTERS: // 03 Read Holding Registers
         // Serial.print("Value:");
          for(int i = 0; i < (rx_buffer[MB_TCP_RX_BYTE_LEN]/2); i++)
          {
            byteFNRx = MB_FC_NONE; 
            
            
            
              byte tempLow=rx_buffer[(MB_TCP_RX_DATA+1) + (i * 2)];
              byte tempHigh=rx_buffer[ MB_TCP_RX_DATA + (i * 2)];
              
              device_register[modbus_start_address+i]=makeWord(tempHigh,tempLow);
        //  Serial.print(device_register[modbus_start_address+i]);
         // Serial.print(",");
              
          }
         // Serial.println();
         
          digitalWrite(0,HIGH);
          delayMicroseconds(10);
          digitalWrite(0,LOW);

        break;
  
        case MB_FC_WRITE_REGISTER:      
        case MB_FC_WRITE_MULTIPLE_REGISTERS: // 06 Write Holding Register         
              
        break;
            
    }

  } 
 return true; 
     
}
 


ulong modbus_t=0;
ulong modbus_timeout=0;
void mb_read_holding_register(int start_address,int number_of_value,int timeout)
{

  if(modbus_busy){
    if((millis()-modbus_t)<modbus_timeout)
      return;
  }
      digitalWrite(0,HIGH);
      delayMicroseconds(10);
      digitalWrite(0,LOW);
  modbus_start_address=start_address;
  modbus_t=millis();
  modbus_timeout=timeout;
  modbus_busy=true;
  int ByteDataLength = number_of_value * 2;
  int MessageLength = 11;
  tx_buffer[MB_TCP_LEN] =highByte(ByteDataLength + 4);
  tx_buffer[MB_TCP_LEN+1] =lowByte(ByteDataLength + 4);
  tx_buffer[MB_TCP_FUNC]=MB_FC_READ_REGISTERS;
  tx_buffer[MB_TCP_REGISTER_START]=highByte(start_address);
  tx_buffer[MB_TCP_REGISTER_START+1]=lowByte(start_address);  
  
  tx_buffer[MB_TCP_REGISTER_NUMBER]=highByte(number_of_value);
  tx_buffer[MB_TCP_REGISTER_NUMBER+1]=lowByte(number_of_value);
  
  
  Wire.beginTransmission(4);
  Wire.write(tx_buffer,MessageLength);
  Wire.endTransmission();
  Wire.requestFrom(4,ByteDataLength+9);

}




void mb_write_holding_register(int start_address,int number_of_value,int timeout)
{

 if(modbus_busy){
    if((millis()-modbus_t)<modbus_timeout)
      return;
  }
   modbus_start_address=start_address;
  modbus_t=millis();
  modbus_timeout=timeout;
  modbus_busy=true;
  int ByteDataLength=number_of_value * 2;
  int MessageLength = ByteDataLength + 13;
  tx_buffer[MB_TCP_FUNC]=MB_FC_WRITE_MULTIPLE_REGISTERS;
  tx_buffer[MB_TCP_REGISTER_START]=highByte(start_address);  
  tx_buffer[MB_TCP_REGISTER_START+1]=lowByte(start_address);  

   
  tx_buffer[MB_TCP_LEN] =highByte(ByteDataLength + 7);
  tx_buffer[MB_TCP_LEN+1] =lowByte(ByteDataLength + 7); 

  tx_buffer[MB_TCP_REGISTER_NUMBER] = highByte(number_of_value);
  tx_buffer[MB_TCP_REGISTER_NUMBER+1] = lowByte(number_of_value);
  tx_buffer[MB_TCP_BYTE_DATA_LEN] = ByteDataLength;
  for(int i = 0; i < number_of_value; i++)
  {
  tx_buffer[ MB_TCP_WRITE_DATA + (i * 2)] = highByte(device_register[start_address + i]);// high byte
  tx_buffer[MB_TCP_WRITE_DATA+1 + (i * 2)] = lowByte(device_register[start_address + i]);// low byte
  }
  Wire.beginTransmission(4);
  Wire.write(tx_buffer,MessageLength);
  Wire.endTransmission();
  Wire.requestFrom(4,12); //fdvdf
}
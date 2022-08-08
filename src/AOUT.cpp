

#include <main.h>

int dutyCycle;
/* Setting PWM Properties */
const int PWMFreq = 20000; /* 5 KHz */
const int PWMResolution = 12;

const int MAX_DUTY_CYCLE = (int)(pow(2, PWMResolution) - 1);
static esp_adc_cal_characteristics_t *adc_chars;
static const adc1_channel_t channel = ADC1_CHANNEL_0;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;
#define NO_OF_SAMPLES   64          //Multisampling
#define DEFAULT_VREF    1100
static const int spiClk = 10000000; // 8 MHz
SPIClass adc_spi(HSPI);
int _mode=0;
int _module=0;

void analogOut_init(int module,int mode){

    _mode=mode;
    _module=module;

    if(module==0){
        //PWM_DAC1
        ledcSetup(0, PWMFreq, PWMResolution);    
        ledcAttachPin(DAC_PWM1, 0);
        //PWM_DAC2
        ledcSetup(2, PWMFreq, PWMResolution);    
        ledcAttachPin(DAC_PWM2, 2);
        //read vcc
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    }
    else if(module==1){
        pinMode(25,OUTPUT);
        digitalWrite(25,HIGH);
        adc_spi.begin();
    }
    
}

double mm_to_lsb(double mm){
    double offset=800;
    // <------1000mm+-----> <------800mm------> <---600mm+---> toplam 2400mm
    double lsb_mm=4095.0/1600.0;// 
    double lsb=(mm-offset)*lsb_mm;
    return lsb;
}
double mm_to_lsb1(double mm){    
    // <------1200mm-----> <------1200mm------> toplam 2400mm    
    double lsb_mm=4095.0/1200.0;
    double lsb=mm*lsb_mm;
    return lsb;
}

void analogOut_Update(){


uint16_t dac1_data=0;
uint16_t dac2_data=0;
uint16_t dac1_address=0x1000;
uint16_t dac2_address=0x5000;

    if(_mode==0){
        dac1_data=mm_to_lsb(RA.readUShort(en_mm));
        dac2_data=mm_to_lsb(RA.readUShort(en_mm));

    }
    else if(_mode==1){
        dac1_data=mm_to_lsb1(RA.readUShort(step1_pos_meas_mm));
        dac2_data=mm_to_lsb1(RA.readUShort(step1_pos_meas_mm));
    }
    else if(_mode==2){
        dac1_data=mm_to_lsb1(RA.readUShort(step1_pos_actual_mm));
        dac2_data=mm_to_lsb1(RA.readUShort(step2_pos_actual_mm));
    }
    

    if(_module==0){
        ledcWrite(0, dac1_data);
        ledcWrite(1, dac2_data);

    }
    else if(_module==1){
    //   cli();
        digitalWrite(25,LOW);
        delayMicroseconds(1);
        digitalWrite(25,HIGH);
        adc_spi.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE1));
    	adc_spi.transfer16(dac1_address|dac1_data);	
        digitalWrite(25,LOW);
        delayMicroseconds(1);
        digitalWrite(25,HIGH);
    	adc_spi.transfer16(dac2_address|dac2_data);	
    	adc_spi.endTransaction();
         
    // Serial.println(dac1_data);
    //    //delay(1000);
    //   sei();
    }
    
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
        
        Serial.print("Raw:");
        Serial.print(adc_reading);
        Serial.print("\tVoltage");
        Serial.print(voltage);
        Serial.println("mV");

       // printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
        
}
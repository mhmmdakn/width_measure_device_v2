

#include <main.h>

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

void pwm_init(){
     ledcSetup(PWMChannel, PWMFreq, PWMResolution);
  /* Attach the LED PWM Channel to the GPIO Pin */
  ledcAttachPin(DAC_PWM1, PWMChannel);
 
  adc1_config_width(width);
  adc1_config_channel_atten(channel, atten);
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
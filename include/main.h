#pragma once



#include "config.h"
#include <map>
using std::map;
using std::pair;
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebAuthentication.h>
#include <AsyncTCP.h>
#include <SPIFFSEditor.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <WiFi.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <Arduino_JSON.h>
#include "system.h"
#include "default.h"
#include "cpu_map.h"
#include "error.h"
#include "limit.h"
#include "protocol.h"
#include "register_control.h"
#include "step_control.h"
#include "communication.h"
#include "aout.h"
#include "webserver.h"
#include "delay_timer.h"
#include <EtherCard.h>
#include <SPI.h>
#include <SimpleModbusSlave.h>
#include <EEPROM.h>
#include <SPIFFS.h>
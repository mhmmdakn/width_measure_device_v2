#pragma once

#define ETHERNET_USE_ESP32
#define BOARD_TYPE      "ESP32"

#define USE_UIP_ETHERNET   true




void wifi_init();
void ethernet_init();
void rs485_init();
void i2c_init();
void spi_init();
void ethernet_loop();
void rs485_loop();
void wifi_loop();
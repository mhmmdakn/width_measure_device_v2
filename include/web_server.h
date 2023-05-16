#pragma once


#include "EthernetWebServer.hpp"
String dataTypeGet(String path);

void wifi_webserver_init();
void ethernet_webserver_init();

void wifi_websever_handle_client();
void ethernet_websever_handle_client();
double stringToDouble(String str);
int doubleDigitCount(double d);

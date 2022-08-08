#pragma once

#include "AsyncTCP.h"
#include "AsyncWebSocket.h"

extern AsyncWebServer server;


void wifi_webserver_init();

String arg(String name);
String arg(int i);
String argName(int i);
String dataTypeGet(String path);
bool hasArg(String  name) ;
String urlDecode(const String& text);
String processorRead(const String& var);
void processorWrite(String name,String value);
void registerUpdate();
void sendFile(fs::FS &fs,String filename);
void sendFileHTML(fs::FS &fs,String filename);
void handleNotFound(AsyncWebServerRequest *request);
void handlePage(AsyncWebServerRequest *request);
void handleSetKey(AsyncWebServerRequest * request);
void handleState(AsyncWebServerRequest *request);
void handleEthernet();
bool isAuthenticated(AsyncWebServerRequest *request);
bool loadFromSdCard(AsyncWebServerRequest *request,String path);
bool authentication(String req,const char* username,const char* password);
void requestAuthentication(String redirectUrl);
void parseArguments(String data);
bool parseRequest(String req);

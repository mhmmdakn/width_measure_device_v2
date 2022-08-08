
#include "main.h"
AsyncWebServer server(80);
String redirectURL = "/dashboard.html";
String realm = "realtekno";
void wifi_webserver_init(){

    ///WIFISERVER///
    server.addHandler(new SPIFFSEditor(SPIFFS, "admin", "admin"));
    server.on("/wifi_settings.html", handlePage);
    server.on("/device_settings.html", handlePage);
    server.on("/bakim.html", handlePage);    
    server.on("/keySet", handleSetKey);
    server.onNotFound(handleNotFound);
    server.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
      if (!index)
        Serial.printf("UploadStart: %s\n", filename.c_str());
      Serial.printf("%s", (const char *)data);
      if (final)
        Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
    });
    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      if (!index)
        Serial.printf("BodyStart: %u\n", total);
      Serial.printf("%s", (const char *)data);
      if (index + len == total)
        Serial.printf("BodyEnd: %u\n", total);
    });
    server.begin();
    Serial.println("HTTP server started");

  ///////////////

}

void handlePage(AsyncWebServerRequest *request)
{

  Serial.println(wifi_begin[log_username]);
  Serial.println(wifi_begin[log_password]);
  if (request->authenticate(wifi_begin[log_username].c_str(), wifi_begin[log_password].c_str(), realm.c_str()))
  {
    if (request->method() == HTTP_POST)
    {
      for (uint8_t i = 0; i < request->params(); i++)
      {
        AsyncWebParameter *p = request->getParam(i);
        processorWrite(p->name(), p->value());
      }
    }

    loadFromSdCard(request, request->url());
    return;
  }
  else
  {
    request->requestAuthentication(realm.c_str(), true, redirectURL);
    ///AsyncWebServerResponse * r = beginResponse(401,"text/html","<script> window.location.href='"+redirectUrl+"'</script>");//  requestAuthentication overload
    return;
  }
}

void handleSetKey(AsyncWebServerRequest *request)
{
  if (request->authenticate("realtekno", "8684", realm.c_str()))
  {
    if (request->method() == HTTP_GET)
    {
      for (uint8_t i = 0; i < request->params(); i++)
      {
        AsyncWebParameter *p = request->getParam(i);
        if (p->name() == "key")
          SPIFFS.open("/key.txt", FILE_WRITE).print(p->value());
      }
    }

    request->send(200, "text/plain", "OK");
  }
  request->requestAuthentication(realm.c_str(), true);
}

void handleNotFound(AsyncWebServerRequest *request)
{

  if (loadFromSdCard(request, request->url()))
  {
    return;
  }
  String message = "\nNo Handler\r\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nParameters: ";
  message += request->params();
  message += "\n";
  for (uint8_t i = 0; i < request->params(); i++)
  {
    AsyncWebParameter *p = request->getParam(i);
    message += String(p->name().c_str()) + " : " + String(p->value().c_str()) + "\r\n";
  }
  request->send(404, "text/plain", message);
  Serial.print(message);
}

void processorWrite(String name, String value)
{
//   detachInterrupt(digitalPinToInterrupt(G1_OUT_pin));
//   detachInterrupt(digitalPinToInterrupt(POS2_OUT_pin));
//   detachInterrupt(digitalPinToInterrupt(FS_pin));
//   detachInterrupt(digitalPinToInterrupt(COM_IN_pin));
//   detachInterrupt(digitalPinToInterrupt(plastik_buton_pin));
//   short address = name.toInt();
//   Serial.print(address);
//   Serial.print("--");
//   Serial.println((uint8_t)value.toInt());
//   if (address > 0)
//   {
//     if (address <= WIFI_BEGIN_END)
//       EEPROM.writeString(address, value);
//     else if (address <= WIFI_CONFIG_END)
//       EEPROM.writeByte(address, (uint8_t)value.toInt());
//     else if (address <= ETH_CONFIG_END)
//       EEPROM.writeByte(address, (uint8_t)value.toInt());
//     else if (address <= DEVICE_CONFIG_END)
//       EEPROM.writeShort(address, (int16_t)value.toInt());
//     else if (address <= PLASTIK_END)
//       EEPROM.writeShort(address, (int16_t)value.toInt());

//     EEPROM.commit();
//     registerUpdate();
//   }

//   if (macCode == code)
//   {
//     attachInterrupt(digitalPinToInterrupt(POS2_OUT_pin), POS2_INT, RISING);
//     attachInterrupt(digitalPinToInterrupt(FS_pin), FS_INT, FALLING);
//     if (device_config[tansiyon_modu] == 1)
//       attachInterrupt(digitalPinToInterrupt(G1_OUT_pin), G1_INT, RISING);
//     if (device_config[plastik_sayimi_mod] == 1)
//     {
//       attachInterrupt(digitalPinToInterrupt(plastik_buton_pin), PLASTIK_BUTON_INT, CHANGE);
//       attachInterrupt(digitalPinToInterrupt(COM_IN_pin), RU_RA_INT, FALLING);
//     }
//   }
}


String dataTypeGet(String path)
{
  String dataType = "text/plain";

  if (path.endsWith(".src"))
    path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".htm"))
    dataType = "text/html";
  else if (path.endsWith(".html"))
    dataType = "text/html";
  else if (path.endsWith(".css"))
    dataType = "text/css";
  else if (path.endsWith(".js"))
    dataType = "application/javascript";
  else if (path.endsWith(".png"))
    dataType = "image/png";
  else if (path.endsWith(".gif"))
    dataType = "image/gif";
  else if (path.endsWith(".jpg"))
    dataType = "image/jpeg";
  else if (path.endsWith(".ico"))
    dataType = "image/x-icon";
  else if (path.endsWith(".xml"))
    dataType = "text/xml";
  else if (path.endsWith(".pdf"))
    dataType = "application/pdf";
  else if (path.endsWith(".zip"))
    dataType = "application/zip";
  else if (path.endsWith(".csv"))
    dataType = "text/csv";
  else if (path.endsWith(".gz"))
    dataType = "application/x-gzip";
  return dataType;
}

bool loadFromSdCard(AsyncWebServerRequest *request, String path)
{

  AsyncWebServerResponse *response;
  if (path.endsWith("/"))
    path += "dashboard.html";
  String dataType = dataTypeGet(path);

  String pathWithGz = path + ".gz";
  if ((SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)))
  {
    if (SPIFFS.exists(pathWithGz))
    {
      path = pathWithGz;
      dataType = "application/x-gzip";
    }
  }

  if (!SPIFFS.exists(path.c_str()))
    return false;
   if (dataType == "text/html")
     response = request->beginResponse(SPIFFS, path, dataType, false, processorRead);
  else if(request->hasArg("download")){

      response = request->beginResponse(SPIFFS,path,"application/x-www-form-urlencoded;charset=UTF-8",true);
   
  }
  else{
   
    response = request->beginResponse(SPIFFS, path, dataType, false); 
     if(dataType=="application/x-gzip"){ 
       response->addHeader("Content-Encoding","gzip");   
      response->addHeader("Cache-Control","max-age=2592000"); 
      
    }
    else if(dataType=="image/png")
      response->addHeader("Cache-Control","max-age=2592000"); 
  } 
 
  request->send(response);

  return true;
}


String processorRead(const String &var)
{

//   short address = var.toInt();
//   if (address > 0)
//   {
//     if (address <= WIFI_BEGIN_END)
//       return EEPROM.readString(address);
//     else if (address <= WIFI_CONFIG_END)
//       return String(EEPROM.readByte(address));
//     else if (address <= ETH_CONFIG_END)
//       return String(EEPROM.readByte(address));
//     else if (address <= DEVICE_CONFIG_END)
//       return String(EEPROM.readShort(address));
//     else if (address <= PLASTIK_END)
//       return String(EEPROM.readShort(address));
//   }

//   return String();
}
#include "main.h"

#include "EthernetWebServer.h"
WebServer wifi_webserver(80);
EthernetWebServer ethernet_webserver(80);

template <class T>
void handlePage(T &server)
{

  if (!server.authenticate("admin", "realtekno"))
  {
    server.requestAuthentication();
    return;
  }

  if (server.method() == HTTP_POST)
  {

    return;
  }

  String path = server.uri();

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

  File dataFile = SPIFFS.open(path.c_str());
  if (dataFile.isDirectory())
  {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SPIFFS.open(path.c_str());
  }
  if (!dataFile)
  {
    String message = "\nNo Handler\r\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
      message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
    return;
  }

  if (server.hasArg("download"))
  {
    dataType = "application/octet-stream";
  }

  if (dataType != "text/html")
  {
    server.sendHeader("Cache-Control", "max-age=2592000");
  }

  server.streamFile(dataFile, "text/html");

  dataFile.close();
}



template <class T>
void handle_data(T &server)
{
  JSONVar jvar = new JSONVar();

  jvar["RSII"] = WiFi.RSSI();
  jvar["width_mode"] = RK.readUShort(width_mode);
  jvar["en_mm"] = RA.readUShort(en_mm);
  jvar["step1_pos_meas_mm"] = RA.readUShort(step1_pos_meas_mm);
  jvar["step2_pos_meas_mm"] = RA.readUShort(step2_pos_meas_mm);
  jvar["step1_pos_actual_mm"] = RA.readUShort(step1_pos_actual_mm);
  jvar["step2_pos_actual_mm"] = RA.readUShort(step2_pos_actual_mm);

  String jsonString = JSON.stringify(jvar);

  server.send(200, "text/plane", jsonString);
}



template <class T>
void processor_write(T &server)
{
  

  String message;
  for (size_t i = 0; i < server.args(); i++)
  {
    String var = server.argName(i);
    short param_index = var.indexOf('|');
    String param_name = var.substring(0, param_index);
    short type_index = var.indexOf('|', param_index + 1);
    char type = var.substring(param_index + 1, type_index)[0];
    short array_num = var.substring(type_index + 1, var.length()).toInt();

    String val = server.arg(i);
  
    int start_address = ht_enum.at(param_name) + array_num;
   
    
    switch (type)
    {
    case 'N':
      RK.writeString(start_address,val);
      break;
    case 'b':
      RK.writeByte(start_address,val.toInt());
      break;
    case 'S':
      RK.writeUShort(start_address,val.toInt());
      break;
    case 'I':
      RK.writeUInt(start_address,val.toInt());
      break;
    case 'd':
      RK.writeDouble(start_address,val.toDouble());
      break;
    default:
      break;
    }
    
  }

  if(RK.commit())
  server.send(200, "text/plain", "success");
  else
  server.send(200, "text/plain", "error");


}

template <class T>
void processor_read(T &server)
{
  
  String message;
  for (size_t i = 0; i < server.args(); i++)
  {
    String var = server.argName(i);
    short param_index = var.indexOf('|');
    String param_name = var.substring(0, param_index);
    short type_index = var.indexOf('|', param_index + 1);
    char type = var.substring(param_index + 1, type_index)[0];
    short array_num = var.substring(type_index + 1, var.length()).toInt();

    String val = "";

    int start_address = ht_enum.at(param_name) + array_num;
    
    switch (type)
    {
    case 'N':
      val = RK.readString(start_address);
      break;
    case 'b':
      val = RK.readByte(start_address);
      break;
    case 'S':
      val = RK.readUShort(start_address);
      break;
    case 'I':
      val = RK.readUInt(start_address);
      break;
    case 'd':
      val = RK.readDouble(start_address);
      break;
    default:
      break;
    }

    message += server.argName(i) + "=" + val + "&";
  }

  message = message.substring(0, message.length() - 1);
  server.send(200, "text/plain", message);
}

void wifi_webserver_init()
{
  wifi_webserver.on("/data", [](){ handle_data<WebServer>(wifi_webserver); });
  wifi_webserver.on("/processor_read", HTTP_POST, [](){ processor_read<WebServer>(wifi_webserver); });
  wifi_webserver.on("/processor_write", HTTP_POST, [](){ processor_write<WebServer>(wifi_webserver); });
  wifi_webserver.onNotFound([](){ handlePage<WebServer>(wifi_webserver); });
  wifi_webserver.begin();
}

void ethernet_webserver_init()
{  
  ethernet_webserver.on("/data", [](){ handle_data<EthernetWebServer>(ethernet_webserver); });
  ethernet_webserver.on("/processor_read", HTTP_POST, [](){ processor_read<EthernetWebServer>(ethernet_webserver); });
  ethernet_webserver.on("/processor_write", HTTP_POST, [](){ processor_write<EthernetWebServer>(ethernet_webserver); });
  ethernet_webserver.onNotFound([](){ handlePage<EthernetWebServer>(ethernet_webserver); });
  ethernet_webserver.begin();
}

void wifi_websever_handle_client()
{
  wifi_webserver.handleClient();
}

void ethernet_websever_handle_client()
{
  ethernet_webserver.handleClient();
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


// void handleNotFound(AsyncWebServerRequest *request)
// {

//   if (loadFromSdCard(request, request->url()))
//   {
//     return;
//   }
//   String message = "\nNo Handler\r\n";
//   message += "URI: ";
//   message += request->url();
//   message += "\nMethod: ";
//   message += (request->method() == HTTP_GET) ? "GET" : "POST";
//   message += "\nParameters: ";
//   message += request->params();
//   message += "\n";
//   for (uint8_t i = 0; i < request->params(); i++)
//   {
//     AsyncWebParameter *p = request->getParam(i);
//     message += String(p->name().c_str()) + " : " + String(p->value().c_str()) + "\r\n";
//   }
//   request->send(404, "text/plain", message);
//   #ifdef DEBUG_MODE
//   Serial.print(message);
//   #endif
// }

// void handlePage()
// {

// }
// void handleData(){
//   JSONVar myObject;
//   //  device_register[updt][0]=1;
//   //  cm.st_update(updt);
//   //****st_update in success ini seçenekli yapıp dene
//   //  while(!device_register_check())
//   //  delay(100);
//     myObject["RSII"]=WiFi.RSSI();
//     myObject["plc_sta"]=plc_status;
//     myObject["cwidth_val"]=word(cm.device_register[cwidth_val][1],cm.device_register[cwidth_val][0]);
//     myObject["mwidth_val"]=word(cm.device_register[mwidth_val][1],cm.device_register[mwidth_val][0]);
//     myObject["number_of_samples"]=word(cm.device_register[number_of_samples][1],cm.device_register[number_of_samples][0]);
//     myObject["fabric_ref"]=word(cm.device_register[fabric_ref][1],cm.device_register[fabric_ref][0]);

//     String jsonString = JSON.stringify(myObject);

//     request->send(200, "text/plane",jsonString);

// }
// void handleREG(){
//   JSONVar myObject;

//     if(request->hasArg("updt")){
//       cm.device_register[updt][0]=request->arg("updt").toInt();

//       if(cm.device_register[updt][0]==1)
//       {
//         cm.st_update(updt);
//         request->send(200, "text/plane","OK");
//         return;
//       }
//     }

//     int num=0;
//     if(request->hasArg("num"))
//       num=request->arg("num").toInt();

//     int ornekleme_sayisi=word(cm.device_register[number_of_samples][1],cm.device_register[number_of_samples][0]);

//     if(request->hasArg("samp"))
//       ornekleme_sayisi=request->arg("samp").toInt();

//     if((ornekleme_sayisi-num)<500)
//     for (int i = 0; i < (ornekleme_sayisi-num); i++)
//     {
//         myObject["device_reg"][i]=word(cm.device_register[i+num][1],cm.device_register[i+num][0]);
//         ESP.wdtFeed();
//     }

//     String jsonString = JSON.stringify(myObject);

//     request->send(200, "text/plane",jsonString);

// }
// void processorWrite(String name, String value)
// {
//   if(name!=""){
//     short kategori_index=name.indexOf('_');
//     String kategori = name.substring(0,kategori_index);
//     short register_index1=name.indexOf('_',kategori_index+1);
//     short register_num1 = name.substring(kategori_index+1,register_index1).toInt();
//     short register_num2 = name.substring(register_index1+1,name.length()).toInt();

//       if (kategori=="wb")
//         cm.wifi_begin_update(register_num1,value);
//       else if (kategori=="wc")
//         cm.wifi_config[register_num1][register_num2]=value.toInt();
//       else if (kategori=="dc")
//         cm.device_config_update(register_num1,value);
//   }
// }

// String processorRead(const String &var)
// {
//  //todo:last
//   if(var!=""){
//     short kategori_index=var.indexOf('_');
//     String kategori = var.substring(0,kategori_index);
//     short register_index1=var.indexOf('_',kategori_index+1);
//     short register_num1 = var.substring(kategori_index+1,register_index1).toInt();
//     short register_num2 = var.substring(register_index1+1,var.length()).toInt();

//     if (kategori=="wb")
//       return String((char*)cm.wifi_begin[register_num1]);
//     else if (kategori=="wc")
//       return String(cm.wifi_config[register_num1][register_num2]);
//     else if (kategori=="dc")
//       return String(cm.DCToStr(register_num1));
//     else if (kategori=="dr")
//       return String(cm.DRToStr(register_num1));
//   }
//     return String();
// }

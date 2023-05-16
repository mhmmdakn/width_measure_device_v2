

#include "main.h"

#include "EthernetWebServer.hpp"
bool wifi_durumu = false;
WiFiServer mb_tcp_wifi_server(502);

void wifi_init()
{

  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(false);       // Wifi OFF
  WiFi.softAPdisconnect(false); // Wifi already OFF

  WiFi.softAP(RK.readString(ap_ssid).c_str(), RK.readString(ap_password).c_str());
  WiFi.mode(WIFI_AP_STA);

  WiFi.config(RK.readUInt(sta_ip), RK.readUInt(sta_gateway), RK.readUInt(sta_subnet), RK.readUInt(sta_dns));
  WiFi.begin(RK.readString(sta_ssid).c_str(), RK.readString(sta_password).c_str());
  // WiFi.config(IPAddress(192,168,2,20),IPAddress(192,168,2,1),IPAddress(255,255,255,0));
  // WiFi.begin("REALTEKNO","realtekno2021");
  //  // Wait for connection
  uint8_t i = 0;
  while ((WiFi.status() != WL_CONNECTED) && i < 100)
  {
    delay(100);
    i++;
  }
  if (i == 100)
  {
    wifi_durumu = false;
    Serial.print("Could not connect to");
    Serial.println(RK.readString(sta_ssid));
    Serial.println(RK.readString(sta_password));
  }
  else
  {
    wifi_durumu = true;
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());    
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
    

  mb_tcp_wifi_server.begin();
  // server.onNotFound(handleNotFound);

  // server.begin();

  wifi_webserver_init();
}

#define CLIENT_MAX_COUNT 5
WiFiClient clients[CLIENT_MAX_COUNT];
bool client_add = 0;
void wifi_loop()
{

  WiFiClient client = mb_tcp_wifi_server.available();
  client_add = false;
  for (size_t i = 0; i < CLIENT_MAX_COUNT; i++)
  {

    if (clients[i])
    {
      if (clients[i].connected())
      {
        handleModbusData<WiFiClient>(clients[i], TCP);
      }
      else
      {
        clients[i].stop();
      }
    }
    else if (client)
    {
      if (!client_add)
      {
        clients[i] = client;
      }
      client_add = true;
    }
  }

  wifi_websever_handle_client();
}

uint8_t myMac[6];
EthernetServer mb_tcp_eth_server(502);

#define ETH_CS_PIN 25

void ethernet_init()
{
  WiFi.macAddress(myMac);
  myMac[0] = 0X70;
  myMac[1] = 0X69;
  myMac[2] = 0X69;
  SPI = SPIClass(HSPI);
  Ethernet.init(ETH_CS_PIN);
  Ethernet.begin(myMac, RK.readUInt(eth_ip), RK.readUInt(eth_dns), RK.readUInt(eth_gateway));
  
  mb_tcp_eth_server.begin();
  ethernet_webserver_init();
}

void ethernet_loop()
{
  EthernetClient client = mb_tcp_eth_server.available(); // returns first client which has data to read or a 'false' client
  handleModbusData<UIPClient>(client, TCP);
  ethernet_websever_handle_client();
}

RS485 rs485(&Serial);
void rs485_init()
{

  rs485.begin(115200, DE_RE_PIN);
}

void rs485_loop()
{
  handleModbusData<RS485>(rs485, RTU);
}

void i2c_init()
{
  Wire.begin(SDA_PIN, SCL_PIN, 1000000); // join i2c bus
}
void spi_init()
{
}
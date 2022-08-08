

#include "main.h"

bool wifi_durumu=false;

void wifi_init(){

  ///WIFI SETUP///
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(false);
    WiFi.disconnect(false);       // Wifi OFF
    WiFi.softAPdisconnect(false); // Wifi already OFF
    
    WiFi.softAP(wifi_begin[ap_ssid].c_str(), wifi_begin[ap_password].c_str());
    WiFi.mode(WIFI_AP_STA);    
    
    WiFi.config(IPAddress(wifi_config[sta_ip]), IPAddress(wifi_config[sta_gateway]), IPAddress(wifi_config[sta_subnet]), IPAddress(wifi_config[sta_dns]));
    WiFi.begin(wifi_begin[sta_ssid].c_str(), wifi_begin[sta_password].c_str());
   
    // Wait for connection
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i < 6)
    { 
      delay(500);
      i++ ;
    }
    if (i == 6)
    {
      wifi_durumu = false;
      Serial.print("Could not connect to");
      Serial.println(wifi_begin[sta_ssid]);
      WiFi.disconnect();
    }
    else
    {
      wifi_durumu = true;
      Serial.print("Connected! IP address: ");
      Serial.println(WiFi.localIP());
      WiFi.setAutoConnect(true);
      WiFi.setAutoReconnect(true);
    }

  /////////////
}




byte mymac[6];
const byte myMac[] PROGMEM = {0x70, 0x69, 0x69, 0x2D, 0x30, 0x31};
byte Ethernet::buffer[3000];
#define ETH_CS_PIN 25
void ethernet_init(){
Serial.println("merhaba");
    WiFi.macAddress(mymac);
    mymac[0] = 0X70;
    mymac[1] = 0X69;
    mymac[2] = 0X69;
    
    char macStr[18] = { 0 };
      sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mymac[0], mymac[1], mymac[2], mymac[3], mymac[4], mymac[5]);
    Serial.println(macStr);
    if (!ether.begin(sizeof Ethernet::buffer, mymac, ETH_CS_PIN))
    {
      Serial.println(F("Failed to initialize Ethernet controller"));
    }
    else
    Serial.println(F("Ethernet controller initialized"));
    
    
    // delay(200);
    // if(ether.isLinkUp()||!wifi_durumu)      
    // {
    //   if(ether.dhcpSetup()){          
    //     Serial.println(F("DHCP OK"));         
    //   }
    //   ethernet_durumu=true;
    // }
         if(ether.dhcpSetup()){          
        Serial.println(F("DHCP OK"));         
       }
    eth_config[eth_ip][0]=192;
    eth_config[eth_ip][1]=168;
    eth_config[eth_ip][2]=0;
    eth_config[eth_ip][3]=5;
    eth_config[eth_subnet][0]=255;
    eth_config[eth_subnet][1]=255;
    eth_config[eth_subnet][2]=255;
    eth_config[eth_subnet][3]=0;
    if (!ether.staticSetup(eth_config[eth_ip], eth_config[eth_gateway], eth_config[eth_dns], eth_config[eth_subnet]))
    {
      Serial.println(F("Failed to set IP address"));
    }
    
    
    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);
    ether.printIp("DNS: ", ether.dnsip);

    ether.printIp("Configured IP address:\t", ether.myip);
    Serial.println();
}

void rs485_init(){

  Serial.begin(115200); 
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);

}
void i2c_init(){
  Wire.begin(SDA_PIN, SCL_PIN,1000000);   // join i2c bus
}
void spi_init(){

}
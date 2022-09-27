#include "main.h"

void default_init()
{
     RK.writeUShort(set_init,11);
    if (RK.readShort(set_init) == -1 || RK.readUShort(set_init) == 11)
    {
        RK.writeUShort(set_init, 0);
        RK.writeString(sta_ssid, "REALTEKNO");
        RK.writeString(sta_password, "realtekno2021");
        RK.writeString(ap_ssid, "RT_ENOLCME");
        RK.writeString(ap_password, "12345678");
        RK.writeString(webserver_username, "admin");
        RK.writeString(webserver_password, "realtekno");
        RK.writeUInt(sta_ip, IPAddress(192, 168, 2, 23));
        RK.writeUInt(sta_subnet, IPAddress(255, 255, 255, 0));
        RK.writeUInt(sta_gateway, IPAddress(192, 168, 2, 1));
        RK.writeUInt(sta_dns, IPAddress(0, 0, 0, 0));
        RK.writeUInt(wifi_ntp, IPAddress(0, 0, 0, 0));
        RK.writeUInt(eth_ip, IPAddress(192, 168, 0, 23));
        RK.writeUInt(eth_subnet, IPAddress(255, 255, 255, 0));
        RK.writeUInt(eth_gateway, IPAddress(192, 168, 0, 1));
        RK.writeUInt(eth_dns, IPAddress(0, 0, 0, 0));
        RK.writeUInt(eth_ntp, IPAddress(0, 0, 0, 0));
        RK.writeUShort(width_mode, (short)0);
        RK.writeDouble(referans_uzunluk_mm, 2400.0);
        RK.writeDouble(mm_pulse, 40.0 / 800.0);

        RK.commit();
    }
}
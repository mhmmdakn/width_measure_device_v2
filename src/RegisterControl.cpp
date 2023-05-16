#include "main.h"

RegisterCollection RA;
EEPROMClass RK;
map<String, int> ht_enum;

bool RegisterCollection::begin(size_t size)
{
    _data = (uint8_t *)malloc(size);
    _size = size;
    return false;
}




void register_init()
{

    RA.begin(512);
    RK.begin(512);

    ht_enum.insert(pair<String, int>("set_init", set_init));
    ht_enum.insert(pair<String, int>("slave_id", slave_id));
    ht_enum.insert(pair<String, int>("sta_ssid", sta_ssid));
    ht_enum.insert(pair<String, int>("sta_password", sta_password));
    ht_enum.insert(pair<String, int>("ap_ssid", ap_ssid));
    ht_enum.insert(pair<String, int>("ap_password", ap_password));
    ht_enum.insert(pair<String, int>("webserver_username", webserver_username));
    ht_enum.insert(pair<String, int>("webserver_password", webserver_password));
    ht_enum.insert(pair<String, int>("sta_ip", sta_ip));
    ht_enum.insert(pair<String, int>("sta_subnet", sta_subnet));
    ht_enum.insert(pair<String, int>("sta_gateway", sta_gateway));
    ht_enum.insert(pair<String, int>("sta_dns", sta_dns));
    ht_enum.insert(pair<String, int>("wifi_ntp", wifi_ntp));
    ht_enum.insert(pair<String, int>("eth_ip", eth_ip));
    ht_enum.insert(pair<String, int>("eth_subnet", eth_subnet));
    ht_enum.insert(pair<String, int>("eth_gateway", eth_gateway));
    ht_enum.insert(pair<String, int>("eth_dns", eth_dns));
    ht_enum.insert(pair<String, int>("width_mode", width_mode));
    ht_enum.insert(pair<String, int>("referans_uzunluk_mm", referans_uzunluk_mm));
    ht_enum.insert(pair<String, int>("mm_pulse", mm_pulse));
    ht_enum.insert(pair<String, int>("speed_rate", speed_rate));
    ht_enum.insert(pair<String, int>("travel_limit_m1", travel_limit_m1));
    ht_enum.insert(pair<String, int>("travel_limit_m2", travel_limit_m2));


    // pair<int, JSONVar*> p;
    // p= std::make_pair(10, &en_anlik_val);
    // en_anlik_val=100.0;
    // ht_param_kalici.insert( std::pair<int, JSONVar*>(10, &en_anlik_val));
}
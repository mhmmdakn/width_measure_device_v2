#pragma once


void mb_read_holding_register(int start_address,int number_of_value,int timeout=1000);
void mb_write_holding_register(int start_address,int number_of_value,int timeout=1000);
bool checkRX();

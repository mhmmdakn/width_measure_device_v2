#pragma once


void IRAM_ATTR onTimer1();
void IRAM_ATTR lazer1_int();
void IRAM_ATTR lazer2_int();
void step_pulse();
void step_init();
void step_disable();
void step_enable();
void home_init();
extern double step1_position_measured_pulse;
extern double step1_position_actual_pulse;
extern double step2_position_measured_pulse;
extern double step2_position_actual_pulse;

extern int step1_calisma_modu;
extern int step2_calisma_modu;
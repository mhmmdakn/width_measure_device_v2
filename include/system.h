#pragma once



typedef enum{
    home_error,
    hardware_limit_error,
    Error_Count

}Error;

typedef struct{
    bool limit_ok;
    bool limit_neg;
    bool limit_poz;
    bool lazer;
    bool dir;
    int work_mode;
}Module_Status;


typedef enum {
  idle,
  homing,
  runing,
  stop,
  error,
  opening
 
  
}State;

typedef struct {
  bool home_ok;
  bool limit_ok;  
  bool position_ok;
  Module_Status m1;
  Module_Status m2;
}Status;


void sys_init();
void sys_loop();

extern State systemState;
extern Status systemStatus;
extern bool systemError[Error_Count];


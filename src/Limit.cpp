#include "main.h"

void limit_init()
{
    pinMode(M1_LIMIT_POZ, INPUT);
    pinMode(M1_LIMIT_NEG, INPUT);
    pinMode(M2_LIMIT_POZ, INPUT);
    pinMode(M2_LIMIT_NEG, INPUT);
}
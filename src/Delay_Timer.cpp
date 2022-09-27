#include "main.h"

TON::TON(long _pt_val)
{
    pt_val = _pt_val;
}

bool TON::EN(bool _in)
{

    if (_in)
    {

        if (!in_edge)
        {
            start_t = millis();
        }

        in_edge = true;
        if (et_val < pt_val)
        {
            et_val = millis() - start_t;
            out = false;
        }
        else
        {
            et_val = pt_val;
            out = true;
        }
    }
    else
    {
        out = false;
        et_val = 0;
        in_edge = false;
    }

    return out;
}

TON::~TON()
{
}
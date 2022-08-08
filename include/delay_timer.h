#pragma once

class TON
{
private:  
    unsigned long start_t=0;
    bool in_edge=false;
public:
    long pt_val=0;
    long et_val=0;
    bool in=0;
    bool out=0;

    TON(long pt_val);
    bool EN(bool in);

    ~TON();
};



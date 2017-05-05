#pragma once
#include <string>
#include <time.h>
#include <iostream>

using namespace std;

class Clock
{

private:
    string funcao;
    clock_t inicio;
public:
    Clock(const string &f);
    ~Clock();
};

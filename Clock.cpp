#include "Clock.h"


Clock::Clock(const string &f)
{
    inicio = clock();
    funcao = f;
}


Clock::~Clock()
{
    clock_t total = clock() - inicio;
    cout << "Tempo para a funcao " << funcao << ": (segundos) " << double(total) / CLOCKS_PER_SEC << endl;
}

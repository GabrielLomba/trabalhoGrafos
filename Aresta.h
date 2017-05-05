#pragma once
#include <string>

using namespace std;

class Aresta
{
private:
    int origem;
    int destino;
    int peso;
public:
    Aresta(int origem, int destino, int peso);
    const int getPeso() { return peso; };
    const int getDestino() { return destino; };
    const int getOrigem() { return origem; };
    ~Aresta();
};


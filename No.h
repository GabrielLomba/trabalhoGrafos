#pragma once
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "Aresta.h"

using namespace std;

class No
{
private:
    string id;
    int grau = 0;
    map<string, int> arestas2;
    vector<Aresta*> arestas;
public:
    No(string id);
    ~No();
    bool operator = (const No &d) { return this->id.compare(d.id) == 0; };
    int getGrau() { return grau; };
    string getId() { return id; };
    vector<Aresta*>* getArestas() { return &arestas; }; // retornar ponteiro para o vetor de arestas para evitar que o vetor seja copiado desnecessariamente
    void inserirAresta(Aresta* aresta);
    void removerAresta(int idDestino);
    Aresta* encontrarArestasComDestino(int idDestino);
};

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

using namespace std;

class No {
private:
    string id;
    int grau = 0;
    int peso = 0;
    unordered_map<int, int> *arestas;
public:
    No(string id, int peso);

    ~No();

    bool operator=(const No &d) { return this->id.compare(d.id) == 0; };

    int getGrau() { return grau; };

    int getPeso() { return peso; };

    string getId() { return id; };

    // como o getArestas é usado só para leitura, retornar ponteiro
    // para o vetor de arestas para evitar que o vetor seja copiado desnecessariamente
    unordered_map<int, int> *getArestas() { return arestas; };

    void inserirAresta(int destino, int peso);

    int removerAresta(int destino);

    void atualizarIndices(int indiceRemovido);

    pair<const int, int> *encontrarArestasComDestino(int destino);
};

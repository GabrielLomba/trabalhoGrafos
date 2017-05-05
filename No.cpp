#include "No.h"

No::No(string id) {
    this->id = id;
}

void No::inserirAresta(Aresta* aresta) {
    if (encontrarArestasComDestino(aresta->getDestino()) != NULL)  return; // caso a aresta já tenha sido inserida, retornar
    arestas.push_back(aresta);
    grau++;
}

void No::removerAresta(int idDestino) {
    for (int i = 0; i < arestas.size(); i++) {
        // no caso de multigrafo, podem haver múltiplas arestas. Portanto, devemos checar tanto o destino como o peso
        if (arestas[i]->getDestino() == idDestino) {
            delete(arestas[i]); // desalocar memória da aresta para depois remover do vetor
            arestas.erase(arestas.begin() + i);
            grau--;
            return;
        }
    }
}

Aresta* No::encontrarArestasComDestino(int idDestino) {
    for (int i = 0; i < arestas.size(); i++) {
        if (arestas[i]->getDestino() == idDestino) {
            return arestas[i];
        }
    }

    return NULL;
}

No::~No() {
    for (int i = 0; i < arestas.size(); i++) {
        delete(arestas[i]);
    }
}

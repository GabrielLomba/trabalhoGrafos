#include "No.h"

No::No(string id, int peso) {
    this->id = id;
    this->peso = peso;
    arestas = new unordered_map<int, int>();
}

void No::inserirAresta(int destino, int peso) {
    if (encontrarArestasComDestino(destino) != NULL) return; // caso a aresta já tenha sido inserida, retornar
    arestas->insert(make_pair(destino, peso));
    grau++;
}

// método retorna o número de arestas excluídas
int No::removerAresta(int destino) {
    int n = arestas->erase(destino);
    if (n != 0) grau--;
    return n;
}

// função auxiliar que atualiza os índices dos nós no map de arestas quando ocorre exclusão de nó
void No::atualizarIndices(int indiceRemovido) {
    // auxiliar que dirá se a aresta atual já foi reinserida. Como não tratamos multigrafo, um map de key int (destino)
    // e bool (foi reinserida) é suficiente
    unordered_map<int, bool> arestaReinserida;

    // como os containers key-value da STL usam keys constantes, é preciso remover e reinserir qualquer indice que
    // seja maior ou igual ao indiceRemovido (que são os nós que tiveram seus índices subtraídos.
    // O iterador não é
    unordered_map<int, int>::iterator it;
    for (it = arestas->begin(); it != arestas->end();) {
        if (it->first > indiceRemovido && arestaReinserida.find(it->first) == arestaReinserida.end()) {
            int indiceAnterior = it->first;
            int peso = it->second;
            arestas->erase(it++);
            (*arestas)[indiceAnterior - 1] = peso;
            arestaReinserida[indiceAnterior - 1] = true;
        } else {
            // como há deleções durante a iteração, é necessário realizar a incrementação aqui, para evitar
            // exceções dentro da hashtable
            ++it;
        }
    }
}

pair<const int, int> *No::encontrarArestasComDestino(int destino) {
    unordered_map<int, int>::iterator it = arestas->find(destino);

    if (it != arestas->end()) return &(*it);
    else return NULL;
}

No::~No() {
    delete (arestas);
}

#include "Grafo.h"
#include <sstream>
#include <queue>
#include <climits>
#include <algorithm>
#include <time.h>

// variáveis usadas na verificação de grafo bipartido
#define SEM_PARTICAO 0
#define PARTICAO_A 1
#define PARTICAO_B 2

#pragma region Construtor

// construtor padrão
Grafo::Grafo(string nomeArquivoEntrada, string nomeArquivoSaida) {
    lerArquivo(nomeArquivoEntrada);
    this->nomeArquivoSaida = nomeArquivoSaida;
}

//construtor auxiliar para gerar grafo transposto (usado para calcular as componentes fortemente conexas)
Grafo::Grafo(vector<string> ids, vector<tuple<int, int, int>> *arestas) {
    isDigrafo = true;

    nos = vector<No *>(ids.size());

    for (int i = 0; i < ids.size(); i++) {
        nos[i] = new No(ids[i]);
    }

    for (int i = 0; i < arestas->size(); i++) {
        // como é transposto, inserir as arestas invertidas
        nos[get<1>((*arestas)[i])]->inserirAresta(get<0>((*arestas)[i]), get<2>((*arestas)[i]));
    }
}

#pragma endregion

/*Nessa região estão desenvolvidas as funções responsáveis pela leitura e criação de arquivo*/
#pragma region  Arquivo

void Grafo::lerArquivo(string nomeArquivoEntrada) {
    ifstream infile(nomeArquivoEntrada);

    if (!infile.is_open()) {
        cout << "Nao foi possivel abrir arquivo " << nomeArquivoEntrada << endl;
        exit(EXIT_FAILURE);
    }

    string origem, destino, auxStr, line;
    getline(infile, auxStr);
    unsigned int numNos = (unsigned int) atoi(auxStr.c_str());

    nos = vector<No *>(numNos, NULL);
    vector<tuple<int, int, int>> arestas;

    map<pair<int, int>, bool> arestaMap;
    map<string, int>::iterator it;
    map<pair<int, int>, bool>::iterator itPair;
    int indiceProximo = 0, indiceOrigem, indiceDestino, peso;

    while (getline(infile, line)) {
        if (line[0] == ' ') {
            // quando há espaços em branco antes no input, devemos removê-los
            // caso observado em uma das instâncias passadas no trabalho
            unsigned int end = 0;
            while (line[end++] == ' ');
            line.erase(0, end - 1);
        }
        istringstream iss(line);
        getline(iss, origem, ' ');
        getline(iss, destino, ' ');

        if (!isPonderado) {
            // caso já tenha sido detectado que o grafo não é ponderado, podemos setar o peso para 1
            peso = 1;
        } else if (getline(iss, auxStr, ' ')) {
            // caso contrário, devemos tentar o peso. Caso tenha sucesso, podemos assumir
            // que o grafo é ponderado e setamos o peso de acordo
            peso = atoi(auxStr.c_str());
        } else {
            // caso não foi possível ler o peso, podemos assumir que o grafo não é ponderado e,
            // portanto, devemos setar o peso para 1
            isPonderado = false;
            peso = 1;
        }

        it = idMap.find(origem);

        if (it == idMap.end()) {
            idMap[origem] = indiceProximo;
            nos[indiceProximo] = new No(origem);
            indiceOrigem = indiceProximo; // caso o nó não exista ainda, precisamos setar novamente o indiceOrigem
            indiceProximo++;  // caso ambos origem e destino não existam, incrementar o i aqui fará com que os dois sejam criados adequadamente
        } else {
            indiceOrigem = it->second;
        }

        it = idMap.find(destino);

        if (it == idMap.end()) {
            idMap[destino] = indiceProximo;
            nos[indiceProximo] = new No(destino);
            indiceDestino = indiceProximo; // caso o nó não exista ainda, precisamos setar novamente o indiceDestino
            indiceProximo++;
        } else {
            indiceDestino = it->second;
        }

        if (!isDigrafo) { // caso já seja digrafo, não há necessidade de checar novamente
            //caso a aresta inversa já foi inserida, o iterator não terá chegado até o final. Portanto, será digrafo
            isDigrafo = arestaMap.find(make_pair(indiceDestino, indiceOrigem)) != arestaMap.end();
        }

        arestaMap[make_pair(indiceOrigem, indiceDestino)] = true;
        arestas.push_back(make_tuple(indiceOrigem, indiceDestino, peso));
    }

    // devemos checar por nós sem arestas e atribuí-los com um id default
    for (int i = numNos - 1; i >= 0; i--) {
        // como os ids foram lidos de uma forma incremental, caso encontremos um nó válido, temos certeza que todos os nós nulos já foram preenchidos
        if (nos[i] != NULL) break;
        string id = nomeDefaultNosSemAresta + to_string(numNos - i);
        nos[i] = new No(id);
        idMap.insert(make_pair(id, i));
    }

    No *aux;
    for (int i = 0; i < arestas.size(); i++) {
        aux = nos[get<0>(arestas[i])];
        aux->inserirAresta(get<1>(arestas[i]), get<2>(arestas[i]));

        // caso não seja digrafo e não seja um laço, devemos adicionar uma aresta no nó destino também
        if (!isDigrafo && get<0>(arestas[i]) != get<1>(arestas[i])) {
            aux = nos[get<1>(arestas[i])];
            // invertemos a ordem para facilitar a busca posteriormente
            aux->inserirAresta(get<0>(arestas[i]), get<2>(arestas[i]));
        }
    }
}

void Grafo::salvarArquivo() {
    ofstream infile(nomeArquivoSaida);

    if (!infile.is_open()) {
        cout << "Nao foi possivel escrever no arquivo " << nomeArquivoSaida << endl;
        return;
    }

    infile << nos.size() << endl;

    unordered_map<int, int> arestas;
    for (int i = 0; i < nos.size(); i++) {
        arestas = *(nos[i]->getArestas());
        for (auto aresta : arestas) {
            if (isDigrafo || aresta.first >= i) {
                // caso seja digrafo, todas as arestas devem ser escritas. Caso contrário,
                // somente as arestas que se relacionam com nós maiores devem ser adicionadas pois
                // uma das duas arestas geradas em grafos não direcionados já foi escrita
                infile << nos[i]->getId() << " " << nos[aresta.first]->getId();
                // caso seja ponderado, salvamos o peso também
                if (isPonderado) infile << " " << aresta.second;
                infile << endl;
            }
        }
    }

    infile.close();

    cout << "Grafo salvo com sucesso!\n";
}

#pragma endregion

/*Nessa região estão desenvolvidas as funções que irão manipular (Adicionar, Editar e Excluir) um nó e as funções que retornam informações de um nó*/
#pragma region Manipulação e Informação do Nó

void Grafo::inserirNo(string id) {
    int i = getIndexNo(id);
    if (i != -1) {
        cout << "O no " << id << " ja existe neste grafo! Favor informar outro id.\n";
        return;
    }

    No *no = new No(id);
    nos.push_back(no);
    idMap[id] = nos.size() - 1;
    cout << "No " << id << " inserido com sucesso!\n";
}

void Grafo::excluirNo(string id) {
    int indice = getIndexNo(id);
    if (indice == -1) {
        printMensagemNoInexistente(id);
        return;
    }

    delete (nos[indice]);  // desalocar mémoria do nó
    nos.erase(nos.begin() + indice);
    idMap.erase(id); // apagar a chave do id passado do map de IDs
    for (int i = 0; i < nos.size(); i++) {
        nos[i]->removerAresta(indice); // remover todas as arestas que tinham o nó excluído como destino
        nos[i]->atualizarIndices(indice); // como as arestas mantém os índices dos nós destino, precisamos atualizá-los
        idMap[nos[i]->getId()] = i; // também é necessário atualizar o map de ids
    }

    cout << "O no " << id << " e suas arestas foram removidos com sucesso!\n";
}

int Grafo::adicionarNoInexistente(string id) {
    // retorna o índice do nó caso o nó foi adicionado com sucesso e -1 caso contrário
    string escolha;
    do {
        cout << "\nNo " << id << " nao existe no grafo! Deseja adiciona-lo (s/n)? ";
        cin >> escolha;
        if (escolha.compare("s") == 0) {
            inserirNo(id);
            return nos.size() - 1;
        } else if (escolha.compare("n") == 0) {
            return -1;
        } else {
            cout << "Opção inválida!\n";
        }
    } while (escolha.compare("n") != 0 && escolha.compare("s") != 0);

    return -1;
}

void Grafo::grauNo(string id) {
    int indice = getIndexNo(id);
    if (indice == -1) {
        printMensagemNoInexistente(id);
        return;
    }

    if (isDigrafo) {
        int grauEntrada = 0;
        for (int i = 0; i < nos.size(); i++) {
            if (i == indice) continue;
            if (nos[i]->encontrarArestasComDestino(indice) != NULL) grauEntrada++;
        }

        cout << "Grau de entrada: " << grauEntrada << endl;
        cout << "Grau de saida: " << nos[indice]->getGrau() << endl;
    } else {
        cout << "Grau: " << nos[indice]->getGrau() << endl;
    }
}

int Grafo::getIndexNo(string id) {
    map<string, int>::iterator it = idMap.find(id);
    if (it == idMap.end()) return -1;
    else return it->second;
}

void Grafo::fechoTransitivoDireto(string id) {
    int indice = getIndexNo(id);
    if (indice == -1) {
        printMensagemNoInexistente(id);
        return;
    }

    cout << "Fecho Transitivo Direto: ";
    // o fecho transitivo direto de um nó é o mesmo conjunto alcançados pela busca em profundidade, partindo deste nó
    vector<bool> visitados(nos.size(), false);
    buscaEmProfundidadeAux(indice, &visitados, true);
    cout << endl;
}

void Grafo::fechoTransitivoIndireto(string id) {
    int indice = getIndexNo(id);
    if (indice == -1) {
        printMensagemNoInexistente(id);
        return;
    }

    vector<vector<int>> *distancias = floydAux();
    cout << "Fecho Transitivo Indireto: ";
    for (int i = 0; i < distancias->size(); i++) {
        if ((*distancias)[i][indice] != INT_MAX) cout << nos[i]->getId() << " ";
    }
    cout << endl;

    //desalocar memória da matriz de distâncias alocada em floydAux
    delete (distancias);
}

void Grafo::vizinhancaAberta(string id) {
    int indice = getIndexNo(id);
    if (indice == -1) {
        printMensagemNoInexistente(id);
        return;
    }

    // já que não lidamos com multigrafos, podemos simplesmente imprimir todas as arestas do nó
    unordered_map<int, int> arestas = *(nos[indice]->getArestas());
    if (arestas.size() == 0) {
        cout << "Nao ha vizinhos\n";
    } else {
        int i = 0;
        for (auto aresta : arestas) {
            if (i != 0 && i % 10 == 0) cout << "\n"; // imprimir 10 por linha
            cout << nos[aresta.first]->getId() << " ";
            i++;
        }
    }

    cout << endl;
}

void Grafo::vizinhancaFechada(string id) {
    int indice = getIndexNo(id);
    if (indice == -1) {
        printMensagemNoInexistente(id);
        return;
    }

    // já que não lidamos com multigrafos, podemos simplesmente imprimir todas as arestas do nó
    unordered_map<int, int> arestasAux = *(nos[indice]->getArestas());
    if (arestasAux.size() == 0) {
        cout << nos[indice]->getId() << endl;
    } else {
        // caso não haja laço neste nó, devemos imprimí-lo
        if (nos[indice]->encontrarArestasComDestino(indice) == NULL) cout << nos[indice]->getId() << " ";
        vizinhancaAberta(id);
    }
}

void Grafo::printMensagemNoInexistente(string id) {
    cout << "O no de id " << id << " nao existe no grafo!\n";
}

#pragma endregion

/*Nessa região estão desenvolvidas as funções que irão manipular(Adicionar e Excluir) uma aresta e retornar informações sobre a aresta*/
#pragma region Manipulação e Informação de Arestas

void Grafo::inserirAresta(string idOrigem, string idDestino, int peso) {
    int indiceOrigem = getIndexNo(idOrigem);
    if (indiceOrigem == -1) {
        if ((indiceOrigem = adicionarNoInexistente(idOrigem)) == -1) return;
    }

    int indiceDestino = getIndexNo(idDestino);
    if (indiceDestino == -1) {
        if ((indiceDestino = adicionarNoInexistente(idDestino)) == -1) return;
    }

    if (nos[indiceOrigem]->encontrarArestasComDestino(indiceDestino) != NULL) {
        cout << "Aresta ja existe no grafo!\n";
        return;
    }

    nos[indiceOrigem]->inserirAresta(indiceDestino, peso);
    // caso não seja digrafo e não seja um laço, devemos adicionar uma aresta no nó destino também
    if (!isDigrafo && indiceOrigem != indiceDestino) {
        // a ordem origem - destino é invertida para facilitar comparações posteriormente
        nos[indiceDestino]->inserirAresta(indiceOrigem, peso);
    }

    cout << "\nAresta inserida com sucesso!\n";
}

void Grafo::excluirAresta(string idOrigem, string idDestino) {
    int indiceOrigem = getIndexNo(idOrigem);

    if (indiceOrigem == -1) {
        printMensagemNoInexistente(idOrigem);
        return;
    }

    int indiceDestino = getIndexNo(idDestino);

    if (indiceDestino == -1) {
        printMensagemNoInexistente(idDestino);
        return;
    }

    // quando é digrafo e há somente uma aresta, podemos removê-la
    int n = nos[indiceOrigem]->removerAresta(indiceDestino);

    if (n == 0) {
        cout << "Aresta inexistente no grafo!\n";
    } else {
        if (!isDigrafo) {
            // quando não é digrafo e há duas arestas, podemos removê-las pois elas são equivalentes
            nos[indiceDestino]->removerAresta(indiceOrigem);
        }
        cout << "Aresta excluida com sucesso!\n";
    }
}

#pragma endregion

/*Nessa região há a implementação de um minHeap para auxiliar no algoritmo de Dijkstra.
 * Tirada de http://www.geeksforgeeks.org/greedy-algorithms-set-7-dijkstras-algorithm-for-adjacency-list-representation/ */
#pragma region MinHeap

// Structure to represent a min heap node
struct MinHeapNode {
    int v;
    int dist;
};

// Structure to represent a min heap
struct MinHeap {
    int size;      // Number of heap nodes present currently
    int *pos;     // This is needed for decreaseKey()
    struct MinHeapNode **array;
};

// A utility function to create a new Min Heap Node
struct MinHeapNode *newMinHeapNode(int v, int dist) {
    struct MinHeapNode *minHeapNode =
            (struct MinHeapNode *) malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// A utility function to create a Min Heap
struct MinHeap *createMinHeap(int capacity) {
    struct MinHeap *minHeap =
            (struct MinHeap *) malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *) malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->array =
            (struct MinHeapNode **) malloc(capacity * sizeof(struct MinHeapNode *));
    return minHeap;
}

// A utility function to swap two nodes of min heap. Needed for min heapify
void swapMinHeapNode(struct MinHeapNode **a, struct MinHeapNode **b) {
    struct MinHeapNode *t = *a;
    *a = *b;
    *b = t;
}

// A standard function to heapify at given idx
// This function also updates position of nodes when they are swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap *minHeap, int idx) {
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size &&
        minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;

    if (right < minHeap->size &&
        minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;

    if (smallest != idx) {
        // The nodes to be swapped in min heap
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];

        // Swap positions
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        // Swap nodes
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

// A utility function to check if the given minHeap is ampty or not
int isEmpty(struct MinHeap *minHeap) {
    return minHeap->size == 0;
}

// Standard function to extract minimum node from heap
struct MinHeapNode *extractMin(struct MinHeap *minHeap) {
    if (isEmpty(minHeap))
        return NULL;

    // Store the root node
    struct MinHeapNode *root = minHeap->array[0];

    // Replace root node with last node
    struct MinHeapNode *lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // Update position of last node
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;

    // Reduce heap size and heapify root
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// Function to decreasy dist value of a given vertex v. This function
// uses pos[] of min heap to get the current index of node in min heap
void decreaseKey(struct MinHeap *minHeap, int v, int dist) {
    // Get the index of v in  heap array
    int i = minHeap->pos[v];

    // Get the node and update its dist value
    minHeap->array[i]->dist = dist;

    // Travel up while the complete tree is not hepified.
    // This is a O(Logn) loop
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist) {
        // Swap this node with its parent
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        // move to parent index
        i = (i - 1) / 2;
    }
}

// A utility function to check if a given vertex
// 'v' is in min heap or not
bool isInMinHeap(struct MinHeap *minHeap, int v) {
    return minHeap->pos[v] < minHeap->size;
}

#pragma endregion

/*Nessa região estão desenvolvidos os algoritmos de caminho mínimo: Dijkstra e Floyd*/
#pragma region Caminho Mínimo

// Esta função calcula de fato as distâncias de acordo com o algoritmo de Dijstrka.
// Por utilizar minHeap para determinar o nó mais próximo, é uma função de complexidade O(ELogV)
int Grafo::dijkstraAux(int indiceOrigem, int indiceDestino) {
    int dist[nos.size()];      // array que conterá as distâncias partindo do nó de índice indiceOrigem para todos os outros do grafo

    // minHeap represents set E
    struct MinHeap *minHeap = createMinHeap(nos.size());

    // Inicializa min heap com todos os nós (seus índices, no caso) e dist com infinito
    for (int v = 0; v < nos.size(); ++v) {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    // Inicializar a distância da origem para a origem como 0
    dist[indiceOrigem] = 0;
    decreaseKey(minHeap, indiceOrigem, 0);

    // Inicialmente, o tamanho do minHeap é igual à quantidade de nós no grafo
    minHeap->size = nos.size();

    // Neste loop, minHeap contém todos os nós que
    // não tiveram suas distâncias mínimas finalizadas
    while (!isEmpty(minHeap)) {

        struct MinHeapNode *minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; // u é o índice do nó atual

        // Atualizar as distâncias mínimas de todos os adjacentes do nó de índice u
        for (auto aresta : *nos[u]->getArestas()) {
            int v = aresta.first;

            // caso a distância para v ainda não tenha sido finalizada (se v está no minHeap)
            // e a distância passando por u é menor do que a atual, atualizar a distância
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX &&
                aresta.second + dist[u] < dist[v]) {
                dist[v] = dist[u] + aresta.second;

                // também devemos atualizar o minHeap
                decreaseKey(minHeap, v, dist[v]);
            }
        }
    }

    return dist[indiceDestino];
}

int Grafo::dijkstra(string idOrigem, string idDestino) {
    int indiceOrigem = getIndexNo(idOrigem);
    if (indiceOrigem == -1) {
        printMensagemNoInexistente(idOrigem);
        return -1;
    }

    int indiceDestino = getIndexNo(idDestino);
    if (indiceDestino == -1) {
        printMensagemNoInexistente(idDestino);
        return -1;
    }
    // Retorna a distância de indiceorigem para indiceDestino;
    return dijkstraAux(indiceOrigem, indiceDestino);
}

vector<vector<int>> *Grafo::floydAux() {
    pair<const int, int> *arestaAux; // auxiliar que conterá a aresta do nó i ao j na primeira fase do Floyd
    // matriz que será preenchida pelo algoritmo de Floyd. Alocada no heap para suportar grafos maiores
    vector<vector<int>> *matrizDistancia = new vector<vector<int>>(nos.size(), vector<int>(nos.size()));

    //este loop preenche a matriz com os pesos das arestas existentes e seta 0 nos cominhos entre o no e ele mesmo
    for (int i = 0; i < nos.size(); i++) {
        for (int j = 0; j < nos.size(); j++) {
            if (i != j) {
                // aux recebe a aresta entre i e j (não tratamos multigrafos, como dito em aula)
                arestaAux = nos[i]->encontrarArestasComDestino(j);
                if (arestaAux != NULL) {
                    (*matrizDistancia)[i][j] = arestaAux->second;
                } else {
                    //caso não haja aresta entre i e j, o caminho entre eles é infinito
                    (*matrizDistancia)[i][j] = INT_MAX;
                }
            } else {
                (*matrizDistancia)[i][j] = 0;
            }
        }
    }

    //este loop itera procurando por distâncias menores usando outros nós como intermediários
    for (int i = 0; i < nos.size(); i++) {
        for (int j = 0; j < nos.size(); j++) {
            for (int k = 0; k < nos.size(); k++) {
                if (i == j) break; // caso seja o mesmo nó, ir para a proxima iteraçao
                // caso o nó intermediário seja um dos nós a se calcular a distância, podemos seguir
                // para a próxima iteração pois é impossível diminuir o caminho usando eles mesmos
                if (k == i || k == j) continue;

                // caso ainda não haja caminho do i ao k ou do k ao j, impossível criar caminho
                if ((*matrizDistancia)[i][k] == INT_MAX || (*matrizDistancia)[k][j] == INT_MAX) continue;

                int result =
                        (*matrizDistancia)[i][k] +
                        (*matrizDistancia)[k][j]; // calcular o caminho usando k como intermediário
                if ((*matrizDistancia)[i][j] > result) { // trocar caso seja maior
                    (*matrizDistancia)[i][j] = result;
                }
            }
        }
    }
    return matrizDistancia;
}

int Grafo::floyd(string idOrigem, string idDestino) {
    int indiceOrigem = getIndexNo(idOrigem);
    if (indiceOrigem == -1) {
        printMensagemNoInexistente(idOrigem);
        return -1;
    }

    int indiceDestino = getIndexNo(idDestino);
    if (indiceDestino == -1) {
        printMensagemNoInexistente(idDestino);
        return -1;
    }

    vector<vector<int>> *matrizDistancia = floydAux();
    int result = (*matrizDistancia)[indiceOrigem][indiceDestino];

    //desalocar memória alocada em floydAux
    delete (matrizDistancia);

    return result;
}

#pragma endregion

/*Nessa região estão presentes as funções responsáveis por realizar a busca no grafo */
#pragma region Busca

// um ponteiro para vector<bool> é usado para que todas as chamadas compartilhem do mesmo vector
// bool print true significa que devemos imprimir os valores visitados
void Grafo::buscaEmProfundidadeAux(int indiceOrigem, vector<bool> *visitado, bool print) {
    (*visitado)[indiceOrigem] = true;
    if (print) cout << nos[indiceOrigem]->getId() << " ";
    unordered_map<int, int> arestas = *(nos[indiceOrigem]->getArestas());
    for (auto aresta : arestas) {
        // caso o nó já tenha sido visitado, podemos continuar pois as arestas dele já estão sendo iteradas
        // caso contrário, devemos inciar a busca nele
        if (!(*visitado)[aresta.first])
            buscaEmProfundidadeAux(aresta.first, visitado, print);
    }
}

//Busca em pronfundidade iniciando no id informado pelo usuário
void Grafo::buscaEmProfundidade(string id) {
    int indiceInicio = getIndexNo(id);
    if (indiceInicio == -1) {
        printMensagemNoInexistente(id);
        return;
    }

    vector<bool> visitado(nos.size(), false);
    // primeiro, realizamos a busca em profundidade a partir do nó informado
    buscaEmProfundidadeAux(indiceInicio, &visitado, true);

    // caso o grafo não seja conectado, devemos checar e realizar a busca a partir de todas as componentes conexas
    for (int i = 0; i < nos.size(); i++) {
        if (visitado[i]) continue;  // caso o nó já tenha sido visitado, podemos pular
        buscaEmProfundidadeAux(i, &visitado, true);
    }

    cout << endl;
}

//Busca em largura iniciando no id informado pelo usuário
void Grafo::buscaEmLargura(string id) {
    int indiceInicio = getIndexNo(id);
    if (indiceInicio == -1) {
        printMensagemNoInexistente(id);
        return;
    }

    queue<int> filaVisitados;
    vector<bool> visitado(nos.size(), false);
    int indice;
    unordered_map<int, int> arestas;

    cout << "Busca em largura \n";
    // setar condições do loop para começar no indiceInicio passado
    // loop necessário para fazer com que todas as componentes sejam visitadas
    for (int i = indiceInicio; i < indiceInicio + nos.size(); i++) {
        if (visitado[i % nos.size()]) continue;
        filaVisitados.push(i % nos.size());
        while (!filaVisitados.empty()) {
            indice = filaVisitados.front();
            filaVisitados.pop();

            // Com o índice atual, podemos realizar a busca nas arestas
            arestas = *(nos[indice]->getArestas());

            if (!visitado[indice]) {
                cout << nos[indice]->getId() << " ";
                visitado[indice] = true;
            }

            for (auto aresta : arestas) {
                if (!visitado[aresta.first]) {
                    // À medida que são encontrados nós ainda não visitados, eles são adicionados à fila
                    filaVisitados.push(aresta.first);
                }
            }
        }
    }
}

#pragma endregion

/*Nessa região estão desenvolvidas as funções que verificam e retornam as características  e informações de um grafo*/
#pragma region Características do Grafo

bool Grafo::isTrivial() {
    return grauGrafo() == 1;
}

bool Grafo::isNulo() {
    bool result = true;
    for (int i = 0; i < nos.size(); i++) {
        if ((*(nos[i]->getArestas())).size() != 0) {
            result = false;
            break;
        }
    }
    return result;
}

bool Grafo::isKRegular(int k) {
    bool result = true;
    for (int i = 0; i < nos.size(); i++) {
        if (nos[i]->getGrau() != k) {
            result = false;
            break;
        }
    }
    return result;
}

bool Grafo::isCompleto() {
    vector<bool> destinos = vector<bool>(nos.size(), false);
    unordered_map<int, int> arestas;

    for (int i = 0; i < nos.size(); i++) {
        if (nos[i]->getGrau() != nos.size() - 1) return false;  // todos os nós precisam se conectar a n-1 nós
        arestas = *(nos[i]->getArestas());
        for (auto aresta : arestas) {
            if (destinos[aresta.first]) return false; // caso haja aresta paralela, não é completo
            if (i == aresta.first) return false; // caso haja laço, não é completo
            destinos[aresta.first] = true;
        }
        fill(destinos.begin(), destinos.end(), false);
    }
    return true;
}

// As duas condições de grafo euleriano são: todos os nós devem ter grau par e o grafo deve ser conexo
bool Grafo::isEuleriano() {
    bool result = true;
    for (int i = 0; i < nos.size(); i++) {
        if (nos[i]->getGrau() % 2 != 0) {
            // caso um nó tenha grau ímpar, o resultado é falso
            result = false;
            break;
        }

    }

    if (!result) return false; // caso já tenha sido detectado que o grafo não é euleriano, podemos retornar

    // caso contrário, devemos verificar a conectividade do grafo
    // caso, após uma passada da busca em profundidade, todos os nós tenham sido visitados, o grafo é conexo
    vector<bool> visitado(nos.size(), false);
    buscaEmProfundidadeAux(0, &visitado, false);

    for (int i = 0; i < visitado.size(); i++) {
        if (!visitado[i]) {
            result = false;
            break;
        }
    }
    return result;
}

bool Grafo::isMultigrafo() {
    unordered_map<int, int> arestas; // vector
    vector<bool> destinos = vector<bool>(nos.size(),
                                         false); // vector que guarda os destinos alcançados pelas arestas do nó atual
    bool result = false; // resultado da verificação

    for (int i = 0; i < nos.size(); i++) {
        arestas = *(nos[i]->getArestas());
        for (auto aresta : arestas) {
            if (i == aresta.first) return false; // multigrafo não tem laços

            if (destinos[aresta.first]) result = true; // caso já haja uma aresta para este nó, é multigrafo
            else destinos[aresta.first] = true;
        }
        fill(destinos.begin(), destinos.end(),
             false); // resetamos o vector destino após checarmos todas as arestas de um nó
    }
    return result;
}

bool Grafo::isSimples() {
    vector<bool> destinos = vector<bool>(nos.size(),
                                         false); // vector que guarda os destinos alcançados pelas arestas do nó atual
    unordered_map<int, int> arestas; // vector auxiliar para receber as arestas de cada nó dentro do loop

    for (int i = 0; i < nos.size(); i++) {
        arestas = *(nos[i]->getArestas());
        for (auto aresta : arestas) {
            if (nos[i]->getId().compare(nos[aresta.first]->getId()) == 0)
                return false; // caso haja laços, não é grafo simples
            if (destinos[aresta.first]) return false; // caso haja arestas paralelas, não é grafo simples

            destinos[aresta.first] = true;
        }
        fill(destinos.begin(), destinos.end(), false);
    }
    return true;
}

// método auxiliar para calcular as componentes fortemente conexas. É basicamente uma busca em profundidade retornando a ordem dos nós visitados na pilha passada
void Grafo::ordemBuscaProfundidade(int indice, vector<bool> *visitado, stack<int> &pilha) {
    (*visitado)[indice] = true;

    unordered_map<int, int> arestas = *(nos[indice]->getArestas());
    // Chamar recursivo para todas as arestas
    for (auto aresta : arestas) {
        // caso o nó já tenha sido visitado, podemos continuar pois as arestas dele já estão sendo iteradas
        // caso contrário, devemos chamar a função recursivamente nele
        if (!(*visitado)[aresta.first])
            ordemBuscaProfundidade(aresta.first, visitado, pilha);
    }

    // Pomos na pilha a ordem dos vérticas já processados
    pilha.push(indice);
}

int Grafo::componentesFortementeConexas() {
    vector<string> ids; // vector contendo todos os ids dos nós do grafo para criar o grafo transposto
    // vector que conterá todas as arestas do grafo para criar o grafo transposto
    // inicializado de tamanho 1 para que um SEGFAULT não seja gerado ao acessar arestas.end()
    vector<tuple<int, int, int>> *arestasGeral = new vector<tuple<int, int, int>>();

    unordered_map<int, int> arestas; // vector auxiliar que guarda as arestas do nó atual dentro do loop
    for (int i = 0; i < nos.size(); i++) {
        ids.push_back(nos[i]->getId());
        arestas = *(nos[i]->getArestas());
        for (auto aresta : arestas) arestasGeral->push_back(make_tuple(i, aresta.first, aresta.second));
    }

    // alocar memória para o grafo pois talvez requeira uma memória considerável
    Grafo *transposto = new Grafo(ids, arestasGeral);

    stack<int> pilha;
    vector<bool> visitados(nos.size(), false);
    int componentes = 0;

    // Primeiramente, recebemos na pilha os tempos de término dos nós na execução da busca em profundidade
    for (int i = 0; i < nos.size(); i++)
        if (!visitados[i])
            ordemBuscaProfundidade(i, &visitados, pilha);

    // Resetamos o vetor visitados para usá-lo no grafo transposto
    fill(visitados.begin(), visitados.end(), false);

    // Agora, precisamos processar todos os nós baseado na ordem da busca contida na pilha
    while (!pilha.empty()) {
        int v = pilha.top();
        pilha.pop();

        // Imprimir a componente fortemente conexa do vértice encontrado
        if (!visitados[v]) {
            componentes++;
            transposto->buscaEmProfundidadeAux(v, &visitados, true);
            cout << endl;
        }
    }

    // desalocar memória usada
    delete (transposto);
    delete (arestasGeral);

    return componentes;
}

void Grafo::showComponentesFortementeConexas() {
    if (isDigrafo) {
        cout << "Componentes fortemente conexas: \n";
        componentesFortementeConexas();
    } else {
        cout << "Componentes fortemente conexas so se aplicam em grafos direcionados!\n";
    }
}

bool Grafo::isBipartido() {
    // vector que conterá as partições de cada nó
    // Os valores possíveis no vector são 3: SEM_PARTICAO, PARTICAO_A e PARTICAO_B
    vector<int> bipartido(nos.size(), SEM_PARTICAO);
    unordered_map<int, int> arestas; // vector auxiliar para receber as arestas de cada nó dentro do loop

    for (int i = 0; i < nos.size(); i++) {
        // caso não tenha partição, pomos o nó na partição A
        if (bipartido[i] == SEM_PARTICAO) bipartido[i] = PARTICAO_A;

        arestas = *(nos[i]->getArestas());

        for (auto aresta : arestas) {
            //Caso tenha laço, o mesmo nó não pode estar em duas partições
            if (i == aresta.first) return false;

            if (bipartido[aresta.first] == SEM_PARTICAO) {
                // caso o adjacente não tenha partição ainda, pomos ele na outra partição
                if (bipartido[i] == PARTICAO_A) bipartido[aresta.first] = PARTICAO_B;
                else bipartido[aresta.first] = PARTICAO_A;
            } else {
                //Se o nó atual tiver um nó adjacente que esteja na mesma partição, o grafo não é bipartido
                if (bipartido[i] == bipartido[aresta.first]) return false;
            }
        }
    }
    return true;
}

void Grafo::complementar() {
    if (!isSimples()) {
        cout << "Nao existe grafo complementar para um grafo nao simples\n";
        return;
    }

    cout << "O grafo complementar eh G(V, E) onde: \n";
    cout << "V: { ";
    for (int i = 0; i < nos.size(); i++) {
        if (i % 10 == 0) cout << endl; // imprimir 10 por linha
        cout << nos[i]->getId() << " ";
    }
    cout << "}\n\nE: {";
    int count = 0;

    for (int i = 0; i < nos.size(); i++) {
        for (int j = 0; j < nos.size(); j++) {
            if (i == j) continue;
            if (nos[i]->encontrarArestasComDestino(j) == NULL && (isDigrafo || j > i)) {
                if (count % 10 == 0) cout << endl; // imprimir 10 por linha
                count++;
                cout << "(" << nos[i]->getId() << ", " << nos[j]->getId() << ") ";
            }
        }
    }

    cout << "}\n";
}

void Grafo::subGrafoInduzido(set<string> listaNo) {
    // alocar vetores no heap para poderem suportar mais dados
    vector<int> *nosInduzidos = new vector<int>(listaNo.size()); // vector que conterá os índices dos nós listados
    vector<tuple<int, int, int>> *arestasInduzidas = new vector<tuple<int, int, int>>(); // vector que conterá as arestas do subgrafo induzido
    pair<const int, int> *aresta; // vector auxiliar que conterá as arestas do nó dentro do loop

    set<string>::iterator it;
    int indiceNo, auxIndice = 0;
    for (it = listaNo.begin(); it != listaNo.end(); ++it) {
        if ((indiceNo = getIndexNo(*it)) != -1) {
            (*nosInduzidos)[auxIndice++] = indiceNo;
        } else {
            printMensagemNoInexistente(*it);
            return;
        }
    }

    // Insere as arestas induzidas no vector arestasInduzidas
    for (int i = 0; i < nosInduzidos->size(); i++) {
        for (int j = 0; j < nosInduzidos->size(); j++) {
            if (isDigrafo || (*nosInduzidos)[j] >= (*nosInduzidos)[i]) {
                aresta = nos[(*nosInduzidos)[i]]->encontrarArestasComDestino((*nosInduzidos)[j]);
                if (aresta != NULL)
                    arestasInduzidas->push_back(make_tuple((*nosInduzidos)[i], (*nosInduzidos)[j], aresta->second));
            }
        }
    }

    cout << "\nO subgrafo induzido resultante eh G(V,E) onde:" << endl << "V = { ";
    for (int i = 0; i < nosInduzidos->size(); i++) {
        if (i != 0 && i % 10 == 0) cout << "\n"; // imprimir 10 por linha
        cout << nos[(*nosInduzidos)[i]]->getId() + " ";
    }

    cout << "}" << endl << "E = { ";

    //string result = "";
    for (int i = 0; i < arestasInduzidas->size(); i++) {
        if (i != 0 && i % 10 == 0) cout << "\n"; // imprimir 10 por linha
        cout << "(" << nos[get<0>((*arestasInduzidas)[i])]->getId() << " , " <<
             nos[get<1>((*arestasInduzidas)[i])]->getId();
        if (isPonderado) {
            cout << ", " << get<2>((*arestasInduzidas)[i]);
        }
        cout << ") ";
    }

    cout << "}\n";

    //desalocar memória
    delete (nosInduzidos);
    delete (arestasInduzidas);
}

void Grafo::sequenciaDeGraus() {
    for (int i = 0; i < nos.size(); i++) {
        cout << nos[i]->getGrau() << " ";
    }
    cout << endl;
}

int Grafo::grauGrafo() {
    int max = -1;
    for (int i = 0; i < nos.size(); i++) {
        if (nos[i]->getGrau() > max) {
            max = nos[i]->getGrau();
        }
    }

    return max;
}

int Grafo::ordemGrafo() {
    return nos.size();
}

int Grafo::raioGrafo() {
    vector<int> e = excentricidade();
    int menor = 0; // O menor raio possível é 0

    for (int i = 0; i < e.size(); i++) {
        if (e[i] != INT_MAX && e[i] < menor) menor = e[i];
    }

    return menor;
}

int Grafo::diametroGrafo() {
    vector<int> e = excentricidade();
    int maior = 0; // O menor diâmetro possível é 0

    for (int i = 0; i < e.size(); i++) {
        if (e[i] != INT_MAX && e[i] > maior) maior = e[i];
    }

    return maior;
}

void Grafo::centroGrafo() {
    vector<int> e = excentricidade();
    int raio = raioGrafo();

    cout << "Centro:";
    for (int i = 0; i < e.size(); i++) {
        if (e[i] == raio) {
            cout << " " << nos[i]->getId();
        }
    }
    cout << endl;
}

void Grafo::periferiaGrafo() {
    vector<int> e = excentricidade();
    int diametro = diametroGrafo();

    cout << "Periferia:";
    for (int i = 0; i < e.size(); i++) {
        if (e[i] == diametro) {
            cout << " " << nos[i]->getId();
        }
    }
    cout << endl;
}

//método auxiliar que encontra as arestas pontes. O algoritmo faz uma busca em profundidade no grafo e, a partir da árvore criada,
// determina que a aresta (u, v) é ponte se não há alternativa para alcançar u ou um ancestral de u na subárvore que tem v como raiz.
// vector visitado guarda se um nó já foi visitado ou não
// vector descoberta guarda os tempos de descoberta dos nós na busca em profundidade
// vector min é o nó que guarda a "alcançabilidade" do grafo. 
// O valor min[v] indica o nó descoberto mais cedo na árvore que é alcançável pela subárvore de raiz v
// vector pai guarda os pais dos nós
// bool existe é uma variável auxiliar para imprimir a mensagem correta no método showArestasPonte()
// algoritmo retirado de: http://www.geeksforgeeks.org/bridge-in-a-graph/
void
Grafo::arestasPonteAux(int atual, vector<bool> *visitado, vector<int> *descoberta, vector<int> *min, vector<int> *pai,
                       bool *existe) {
    // variável estática que determina o tempo de descoberta
    static int tempo = 0;

    // Marca o nó atual como visitado
    (*visitado)[atual] = true;

    // Inicializa o tempo de descoberta e min
    (*descoberta)[atual] = (*min)[atual] = ++tempo;

    // Percorrer todas as arestas do nó atual
    unordered_map<int, int> arestas = *(nos[atual]->getArestas());
    for (auto aresta : arestas) {
        int adjacente = aresta.first;
        // Caso o adjacente não tenha sido visitado
        if (!(*visitado)[adjacente]) {
            (*pai)[adjacente] = atual;
            arestasPonteAux(adjacente, visitado, descoberta, min, pai, existe);

            // Checar se algum filho do adjacente tem conexão com u ou um ancestral de u
            if ((*min)[adjacente] < (*min)[atual]) (*min)[atual] = (*min)[adjacente];

            // Se o vértice descoberto mais cedo alcançável da subárvore abaixo do adjacente
            // está abaixo do nó atual, a aresta em questão é ponte
            if ((*min)[adjacente] > (*descoberta)[atual]) {
                cout << "(" << nos[atual]->getId() << ", " << nos[adjacente]->getId() << ")\n";
                *existe = true;
            }

        }

            // Atualizar o valor min do nó atual para as chamadas recursivas
        else if (adjacente != (*pai)[atual])
            if ((*descoberta)[adjacente] < (*min)[atual]) (*min)[atual] = (*descoberta)[adjacente];
    }
}

void Grafo::showArestasPonte() {
    // Inicializar os vetores necessários para chamar o método auxiliar que descobre as arestas pontes
    vector<bool> visitado(nos.size(), false);
    vector<int> descoberta(nos.size());
    vector<int> min(nos.size());
    vector<int> pai(nos.size(), -1);

    cout << "Arestas Ponte:\n";
    bool arestaPonteExiste = false;
    // Chamar a função auxiliar recursiva para encontrar as arestas ponte
    // na árvore gerada pela busca em profundidade com raiz em i
    for (int i = 0; i < nos.size(); i++)
        if (!visitado[i])
            arestasPonteAux(i, &visitado, &descoberta, &min, &pai, &arestaPonteExiste);

    if (!arestaPonteExiste) cout << "Nao ha arestas ponte\n";
}

//método auxiliar que encontra os nós de articulação. O algoritmo faz uma busca em profundidade no grafo e, a partir da árvore criada,
// determina que o nó atual é de articulação se o nó é raiz e tem 2 ou mais filhos ou se a alcançabilidade de um de seus filhos é maior
// do que o tempo no qual o nó atual foi descoberto
// vector visitado guarda se um nó já foi visitado ou não
// vector descoberta guarda os tempos de descoberta dos nós na busca em profundidade
// vector min é o nó que guarda a "alcançabilidade" do grafo. 
// O valor min[v] indica o nó descoberto mais cedo na árvore que é alcançável pela subárvore de raiz v
// vector pai guarda os pais dos nós
// bool existe é uma variável auxiliar para imprimir a mensagem correta no método showNoArticulacao()
// algoritmo retirado de: http://www.geeksforgeeks.org/articulation-points-or-cut-vertices-in-a-graph/
void
Grafo::noArticulacaoAux(int atual, vector<bool> *visitado, vector<int> *descoberta, vector<int> *min, vector<int> *pai,
                        bool *existe) {
    // variável estática que determina o tempo de descoberta
    static int tempo = 0;

    // Número de filhos do nó atual na árvore gerada pela busca em profundidade
    int filhos = 0;

    // Marca o nó atual como visitado
    (*visitado)[atual] = true;

    // Seta o tempo de descoberta do nó atual e a alcançabilidade do nó
    (*descoberta)[atual] = (*min)[atual] = ++tempo;

    // Percorrer todas as arestas do nó atual
    unordered_map<int, int> arestas = *(nos[atual]->getArestas());
    for (auto aresta : arestas) {
        int adjacente = aresta.first;

        // Se adjacente ainda não foi visitado, então setá-lo para filho do nó atual
        // na árvore da busca em profundidade e fazer uma chamada recursiva nele
        if (!(*visitado)[adjacente]) {
            filhos++;
            (*pai)[adjacente] = atual;
            noArticulacaoAux(adjacente, visitado, descoberta, min, pai, existe);

            // Checa se um nó da subávore com raiz adjacente tem conexão com o nó atual ou algum ancestral do nó atual
            if ((*min)[adjacente] < (*min)[atual]) (*min)[atual] = (*min)[adjacente];

            // O nó atual é de articulação nos seguintes casos

            // (1) O nó atual é a raiz da árvore de busca em profundidade e tem 2 ou mais filhos
            if ((*pai)[atual] == -1 && filhos > 1) {
                cout << nos[atual]->getId() << endl;
                *existe = true;
            }

            // (2) O nó não é raiz da árvore e a alcançabilidade de um de seus filhos
            // é maior que o tempo de descoberta do nó atual, pois este filho precisa do nó atual
            // para se comunicar com os ancestrais do nó atual
            if ((*pai)[atual] != -1 && (*min)[adjacente] >= (*descoberta)[atual]) {
                cout << nos[atual]->getId() << endl;
                *existe = true;
            }
        }

            // Atualiza a alcançabilidade do nó atual para as demais chamadas recursivas
        else if (adjacente != (*pai)[atual])
            if ((*descoberta)[adjacente] < (*min)[atual]) (*min)[atual] = (*descoberta)[adjacente];
    }
}

void Grafo::showNoArticulacao() {
    // Inicializar os vetores necessários para chamar o método auxiliar que descobre as arestas pontes
    vector<bool> visitado(nos.size(), false);
    vector<int> descoberta(nos.size());
    vector<int> min(nos.size());
    vector<int> pai(nos.size(),
                    -1); // valor inicializado com -1 para ser usado dentro do método auxiliar para detectar a raiz da árvore gerada pela busca em profundidade

    cout << "Nos de articulacao:\n";
    bool noArticulacaoExiste = false;
    // Chamar a função auxiliar recursiva para encontrar os nós de articulação
    // na árvore gerada pela busca em profundidade com raiz em i
    for (int i = 0; i < nos.size(); i++)
        if (!visitado[i])
            noArticulacaoAux(i, &visitado, &descoberta, &min, &pai, &noArticulacaoExiste);

    if (!noArticulacaoExiste) cout << "Nao ha nos de articulacao\n";
}

vector<int> Grafo::excentricidade() {
    vector<int> result(nos.size(), 0); // Valor mínimo para excentricidade é 0 que é o valor do nó para ele mesmo
    vector<vector<int>> *matrizDistancia = floydAux(); // matriz com caminhos mais curtos do grafo
    for (int i = 0; i < nos.size(); i++) {
        for (int j = 0; j < nos.size(); j++) {
            // pega o maior caminho mínimo válido para cada no
            if ((*matrizDistancia)[i][j] != INT_MAX && (*matrizDistancia)[i][j] > result[i])
                result[i] = (*matrizDistancia)[i][j];
        }
    }

    //desalocar memória da matriz de distâncias alocada em floydAux
    delete (matrizDistancia);

    return result;
}

void Grafo::printGrafo() {
    unordered_map<int, int> arestas;
    for (int i = 0; i < nos.size(); i++) {
        arestas = *(nos[i]->getArestas());
        cout << "No " << nos[i]->getId() << ":  ";

        int lineCount = 0;
        for (auto aresta : arestas) {
            if (isPonderado) {
                // caso seja ponderado, é necessário mostrar os pesos
                if (lineCount != 0 && lineCount % 10 == 0) cout << "\n"; // imprimir 10 por linha
                cout << "(" << nos[aresta.first]->getId() << ", " << aresta.second << ") ";
            } else {
                // caso contrário, somente mostrar os destinos das arestas
                if (lineCount != 0 && lineCount % 20 == 0) cout << "\n"; // imprimir 10 por linha
                cout << nos[aresta.first]->getId() << " ";
            }
            lineCount++;
        }
        cout << "\n";
    }
}

// Representação de sets disjuntos para ser usado no algoritmo de Kruskal ao gerar a árvore geradora mínima
struct SetDisjunto {
    vector<int> pai, rank;

    SetDisjunto(unsigned int n) {
        // Alocar memória e inicializar os vetores
        pai = vector<int>(n + 1);
        rank = vector<int>(n + 1);

        for (int i = 0; i <= n; i++) {
            pai[i] = i;
            rank[i] = 0;
        }
    }

    // Achar pai do nó u
    int acharPai(int u) {
        // Faz com que o pai dos nós no caminho de u --> pai[u] apontem para o pai de u
        if (u != pai[u])
            pai[u] = acharPai(pai[u]);
        return pai[u];
    }

    // União por rank
    void merge(int x, int y) {
        x = acharPai(x), y = acharPai(y);

        // Faz com que a árvore de menor rank seja subárvore da de maior rank
        if (rank[x] > rank[y]) pai[y] = x;
        else pai[x] = y;

        if (rank[x] == rank[y])
            rank[y]++;
    }
};

template<int M, template<typename> class F = std::less>
struct TupleCompare {
    template<typename T>
    bool operator()(T const &t1, T const &t2) {
        return F<typename tuple_element<M, T>::type>()(std::get<M>(t1), std::get<M>(t2));
    }
};

//Algoritmo de Kruskal auxiliar para cálculo da AGM
int Grafo::kruskalAux() {
    int pesoTotal = 0;

    vector<tuple<int, int, int>> arestasGeral; // vector que conterá todas as arestas do grafo
    for (int i = 0; i < nos.size(); i++)
        for (auto aresta : (*nos[i]->getArestas()))
            arestasGeral.insert(arestasGeral.end(), make_tuple(i, aresta.first, aresta.second));

    // ordena as arestas baseadas em seu peso
    sort(arestasGeral.begin(), arestasGeral.end(), TupleCompare<2>());

    SetDisjunto sd(nos.size());

    for (int i = 0; i < arestasGeral.size(); i++) {
        int u = get<0>(arestasGeral[i]);
        int v = get<1>(arestasGeral[i]);

        int set_u = sd.acharPai(u);
        int set_v = sd.acharPai(v);

        // Checar the a aresta selecionada está criando um ciclo ou não
        // Ciclo é criado se u e v são do mesmo set
        if (set_u != set_v) {
            // Aresta atual pertence à AGM
            cout << "(" << nos[u]->getId() << ", " << nos[v]->getId() << ")" << endl;
            pesoTotal += get<2>(arestasGeral[i]);
            // Unir os 2 sets
            sd.merge(set_u, set_v);
        }
    }

    return pesoTotal;
}

void Grafo::showArvoreGeradoraMinima() {
    cout << "As arestas formadoras da AGM sao: \n";
    int peso = kruskalAux();
    cout << "\nPeso da AGM eh " << peso << endl;
}

#pragma endregion

#pragma region Coberta Mínima de Vérticas Ponderados

void Grafo::showCoberturaGuloso() {
    pair<vector<No*>, int> solucao = construirSolucao(0.0f);

    cout << "Solucao encontrada pelo algoritmo guloso:\nS = {";
    for (int i = 0; i < solucao.first.size(); ++i) {
        if(i == 0) cout << solucao.first[i]->getId();
        else if (i % 20 == 0) cout << "\n" << solucao.first[i]->getId(); // imprime 20 por linha
        else cout << ", " << solucao.first[i]->getId();
    }

    cout << "}\n\nPeso Total: " << solucao.second << "\n";
}

void Grafo::showCoberturaGulosoRandomizado(double alpha, int numIteracoes) {
    pair<vector<No*>, int> solucao, melhorSolucao;

    // a princípio, o custo da melhor solução é infinito
    melhorSolucao.second = INT_MAX;

    // caso o alpha passado seja zero, podemos fazer uma única iteração pois o resultado não mudará (equivalente ao guloso comum)
    if(alpha == 0) numIteracoes = 1;



    for(int i = 0; i < numIteracoes; ++i){
        solucao = construirSolucao(alpha);
        if(solucao.second < melhorSolucao.second){
            melhorSolucao = solucao;
        }
    }

    cout << "Solucao encontrada pelo algoritmo guloso randomizado:\nS = {";
    for (int i = 0; i < melhorSolucao.first.size(); ++i) {
        if(i == 0) cout << melhorSolucao.first[i]->getId();
        else if (i % 20 == 0) cout << "\n" << melhorSolucao.first[i]->getId(); // imprime 20 por linha
        else cout << ", " << melhorSolucao.first[i]->getId();
    }

    cout << "}\n\nPeso Total: " << melhorSolucao.second << "\n";
}

//true -> no1 primeiro
struct comparatorNo {
    inline bool operator()(const pair<No *, int> &no1, const pair<No *, int> &no2) {
        if (no1.first->getPeso() == 0) {
            // caso ambos os pesos dos nós forem 0, o de maior grau relevante é melhor
            if (no2.first->getPeso() == 0) return no1.second > no2.second;

            // caso o peso do no2 não for 0 e o do no1 é 0, nó1 é melhor quando adiciona alguma aresta não atendida.
            // Portanto, caso o grau relevante de no1 seja diferente de 0, ele é melhor. Caso contrário, no2 é melhor
            return no1.second != 0;
        }

        //caso cheguemos até aqui, o peso do no2 é 0 e o do no1 não é. Portanto, podemos fazer o mesmo que acima e
        // retornar no2 se e somente se ele ainda tiver grau relevante
        if (no2.first->getPeso() == 0) return no2.second == 0;

        // caso ambos os pesos sejam positivos, comparamos baseado no grau relevante e no peso do nó
        // é necessário usar float para diferenciar nós candidatos de importância baixa com nós de importância nula
        float importancia1 = (float) no1.second / no1.first->getPeso();
        float importancia2 = (float) no2.second / no2.first->getPeso();

        return importancia1 > importancia2;
    }
};

// função auxiliar que, de fato, contruirá as soluções dos algoritmos gulosos
pair<vector<No*>, int> Grafo::construirSolucao(double alpha) {
    // vector que conterá os ponteiros dos nós do grafo e o grau relevante deles, isto é, o número de arestas
    // ainda não atendidas na cobertura mínima
    vector<pair<No *, int>> nosAux(nos.size());

    // vector que conterá todas as arestas do grafo no seguinte formato: origem, destino, atendida (na solução)
    vector<pair<int, int>> arestasNaoAtendidas;

    for (int i = 0; i < nos.size(); i++) {
        nosAux[i].first = nos[i];
        nosAux[i].second = nos[i]->getGrau();
        for (auto aresta : (*nos[i]->getArestas())) {
            if (isDigrafo || aresta.first >= i)
                arestasNaoAtendidas.insert(arestasNaoAtendidas.end(), make_pair(i, aresta.first));
        }
    }

    // solução é um pair consistindo do vetor de nós contidos na solução e um int que guarda o custo total da solução
    pair<vector<No*>, int> solucao;

    // usado na randomização dos índices
    default_random_engine generator( (unsigned int)time(0) );

    while (arestasNaoAtendidas.size() != 0) {
        //primeiramente, ordenamos o vetor de nós
        sort(nosAux.begin(), nosAux.end(), comparatorNo());

        //recupera o índice do nó a ser adicionado na solução
        int indice = 0;
        if(alpha != 0){
            int range = (int) (alpha * nosAux.size());
            while(nosAux[range].second == 0 && range != 0){
                // caso haja nós com importância 0 dentro do range de escolha randomizada, diminuir a range
                // até achar um nó com importância (nosAux[range].second !=0) ou caso cheguemos ao começo do vetor (range == 0)
                --range;
            }
            uniform_int_distribution<int>  distr(0, range);
            indice = distr(generator);
        }

        //como é guloso, escolhemos o melhor nó encontrado e atualizamos tanto a lista de nós quanto o peso total
        solucao.first.push_back(nosAux[indice].first);
        solucao.second += nosAux[indice].first->getPeso();

        // atualizamos as arestas, removendo as atendidas pelo nó adicionado e diminuindo o grau relevante dos
        // nós adjacentes ao adicionado
        atualizaNosEArestas(nosAux[indice].first, &arestasNaoAtendidas, &nosAux);

        // removemos o nó adicionado à solução
        nosAux.erase(nosAux.begin() + indice);
    }
    return solucao;
}

void Grafo::atualizaNosEArestas(No *noAdicionado, vector<pair<int, int>> *arestasGeral, vector<pair<No *, int>> *nosAux) {
    vector<pair<int, int>>::iterator it;
    int indiceNoAdicionado = idMap.find(noAdicionado->getId())->second;
    for (it = arestasGeral->begin(); it != arestasGeral->end();) {
        if (it->first == indiceNoAdicionado) {
            for (int i = 0; i < nosAux->size(); ++i) {
                if (nos[it->second]->getId() == (*nosAux)[i].first->getId()) {
                    --(*nosAux)[i].second;
                    break;
                }
            }
            it = arestasGeral->erase(it);
        } else if (it->second == indiceNoAdicionado) {
            for (int i = 0; i < nosAux->size(); ++i) {
                if (nos[it->first]->getId() == (*nosAux)[i].first->getId()) {
                    --(*nosAux)[i].second;
                    break;
                }
            }
            it = arestasGeral->erase(it);
        } else {
            // como há deleções durante a iteração, é necessário realizar a incrementação aqui, para evitar
            // exceções dentro da hashtable
            ++it;
        }
    }
}

#pragma endregion

#pragma region Destrutor

Grafo::~Grafo() {
    for (int i = 0; i < nos.size(); i++) {
        delete (nos[i]);
    }
}

#pragma endregion
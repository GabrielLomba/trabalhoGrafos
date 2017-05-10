#include "Grafo.h"
#include <sstream>
#include <queue>
#include <climits>
#include <algorithm>

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
Grafo::Grafo(vector<string> ids, vector<Aresta*> arestas) {
    isDigrafo = true;

    for (int i = 0; i < ids.size(); i++) {
        nos.push_back(new No(ids[i]));
    }

    for (int i = 0; i < arestas.size(); i++) {
        // como é transposto, inserir as arestas invertidas
        nos[arestas[i]->getDestino()]->inserirAresta(
                new Aresta(arestas[i]->getDestino(), arestas[i]->getOrigem(), arestas[i]->getPeso()));
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
    int numNos = atoi(auxStr.c_str());

    nos = vector<No*>(numNos, NULL);
    vector<Aresta*> arestas;

    map<string, int> idMap;
    map<pair<int, int>, bool> arestaMap;
    map<string, int>::iterator it;
    map<pair<int, int>, bool>::iterator itPair;
    int indiceProximo = 0, indiceOrigem, indiceDestino, peso;

    while (getline(infile, line)) {
        istringstream iss(line);
        getline(iss, origem, ' ');
        getline(iss, destino, ' ');

        if (!isPonderado) {
            // caso já tenha sido detectado que o grafo não é ponderado, podemos setar o peso para 1
            peso = 1;
        }
        else if (getline(iss, auxStr, ' ')) {
            // caso contrário, devemos tentar o peso. Caso tenha sucesso, podemos assumir
            // que o grafo é ponderado e setamos o peso de acordo
            peso = atoi(auxStr.c_str());
        }
        else {
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
        }
        else {
            indiceOrigem = it->second;
        }

        it = idMap.find(destino);

        if (it == idMap.end()) {
            idMap[destino] = indiceProximo;
            nos[indiceProximo] = new No(destino);
            indiceDestino = indiceProximo; // caso o nó não exista ainda, precisamos setar novamente o indiceDestino
            indiceProximo++;
        }
        else {
            indiceDestino = it->second;
        }

        Aresta* aresta = new Aresta(indiceOrigem, indiceDestino, peso);

        if (!isDigrafo) { // caso já seja digrafo, não há necessidade de checar novamente
            //caso a aresta inversa já foi inserida, o iterator não terá chegado até o final. Portanto, será digrafo
            isDigrafo = arestaMap.find(make_pair(indiceDestino, indiceOrigem)) != arestaMap.end();
        }

        arestaMap[make_pair(indiceOrigem, indiceDestino)] = true;
        arestas.push_back(aresta);
    }

    // devemos checar por nós sem arestas e atribuí-los com um id default
    for (int i = numNos - 1; i >= 0; i--) {
        // como os ids foram lidos de uma forma incremental, caso encontremos um nó válido, temos certeza que todos os nós nulos já foram preenchidos
        if (nos[i] != NULL) break;
        nos[i] = new No(nomeDefaultNosSemAresta + to_string(numNos - i));
    }

    No *aux;
    for (int i = 0; i < arestas.size(); i++) {
        aux = nos[arestas[i]->getOrigem()];
        aux->inserirAresta(arestas[i]);

        // caso não seja digrafo e não seja um laço, devemos adicionar uma aresta no nó destino também
        if (!isDigrafo && arestas[i]->getOrigem() != arestas[i]->getDestino()) {
            aux = nos[arestas[i]->getDestino()];
            // invertemos a ordem para facilitar a busca posteriormente
            aux->inserirAresta(new Aresta(arestas[i]->getDestino(), arestas[i]->getOrigem(), arestas[i]->getPeso()));
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

    vector<Aresta*> arestas;
    for (int i = 0; i < nos.size(); i++) {
        arestas = *(nos[i]->getArestas());
        for (int j = 0; j < arestas.size(); j++) {
            if (isDigrafo || arestas[j]->getDestino() >= i) {
                // caso seja digrafo, todas as arestas devem ser escritas. Caso contrário,
                // somente as arestas que se relacionam com nós maiores devem ser adicionadas pois
                // uma das duas arestas geradas em grafos não direcionados já foi escrita
                infile << arestas[j]->getOrigem() << " " << arestas[j]->getDestino() << " " << arestas[j]->getPeso() << endl;
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

    No* no = new No(id);
    nos.push_back(no);
    cout << "No " << id << " inserido com sucesso!\n";
}

void Grafo::excluirNo(string id) {
    int indice = getIndexNo(id);
    if (indice == -1) {
        printMensagemNoInexistente(id);
        return;
    }

    delete(nos[indice]);  // desalocar mémoria do nó
    nos.erase(nos.begin() + indice);
    for (int i = 0; i < nos.size(); i++) {
        nos[i]->removerAresta(indice); // remover todas as arestas que tinham o nó excluído como destino
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
        }
        else if (escolha.compare("n") == 0) {
            return -1;
        }
        else {
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
        int grauSaida = 0;
        for (int i = 0; i < nos.size(); i++) {
            if (i == indice) continue;
            if (nos[i]->encontrarArestasComDestino(indice) != NULL) grauSaida++;
        }

        cout << "Grau de entrada: " << nos[indice]->getGrau() << endl;
        cout << "Grau de saida: " << grauSaida << endl;
    }
    else {
        cout << "Grau: " << nos[indice]->getGrau() << endl;
    }
}

int Grafo::getIndexNo(string id) {
    for (int i = 0; i < nos.size(); i++) {
        if (nos[i]->getId().compare(id) == 0) {
            return i;
        }
    }

    return -1;
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

    vector<vector<int>> distancias = floydAux();
    cout << "Fecho Transitivo Indireto: ";
    for (int i = 0; i < distancias.size(); i++) {
        if (distancias[i][indice] != INT_MAX)  cout << nos[i]->getId() << " ";
    }
    cout << endl;
}

set<string> Grafo::vizinhancaAberta(string id) {
    int indice = getIndexNo(id);
    if (indice == -1) {
        printMensagemNoInexistente(id);
        return set<string>();
    }

    set<string> vizinhos; // set é usado para evitar duplicações nas vizinhanças
    vector<Aresta*> arestasAux = *(nos[indice]->getArestas());
    for (int i = 0; i < arestasAux.size(); i++) {
        vizinhos.insert(nos[arestasAux[i]->getDestino()]->getId());
    }

    return vizinhos;
}

set<string> Grafo::vizinhancaFechada(string id) {
    int index = getIndexNo(id);
    if (index == -1) {
        printMensagemNoInexistente(id);
        return set<string>();
    }

    set<string> vizinhos = vizinhancaAberta(id);
    if (!vizinhos.count(nos[index]->getId())) {
        vizinhos.insert(nos[index]->getId());
    }

    return vizinhos;
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
        if ((indiceOrigem = adicionarNoInexistente(idOrigem)) == -1)  return;
    }

    int indiceDestino = getIndexNo(idDestino);
    if (indiceDestino == -1) {
        if ((indiceDestino = adicionarNoInexistente(idDestino)) == -1)  return;
    }

    Aresta* aresta = nos[indiceOrigem]->encontrarArestasComDestino(indiceDestino);
    if (aresta != NULL) {
        cout << "Aresta ja existe no grafo!\n";
        return;
    }

    aresta = new Aresta(indiceOrigem, indiceDestino, peso);
    nos[indiceOrigem]->inserirAresta(aresta);
    // caso não seja digrafo e não seja um laço, devemos adicionar uma aresta no nó destino também
    if (!isDigrafo && indiceOrigem != indiceDestino) {
        // a ordem origem - destino é invertida para facilitar comparações posteriormente
        nos[indiceDestino]->inserirAresta(new Aresta(indiceDestino, indiceOrigem, peso));
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

    Aresta* arestaResult = nos[indiceOrigem]->encontrarArestasComDestino(indiceDestino);

    if (arestaResult == NULL) {
        cout << "Aresta inexistente no grafo!\n";
    }
    else {
        // quando é digrafo e há somente uma aresta, podemos removê-la

        // é necessário guardar peso numa variável local pois, no caso de grafos não direcionados,
        // após a remoção da primeira aresta, perderemos o valor de peso original
        int peso = arestaResult->getPeso();
        nos[indiceOrigem]->removerAresta(indiceDestino);
        if (!isDigrafo) {
            // quando não é digrafo e há duas arestas, podemos removê-las pois elas são equivalentes
            nos[indiceDestino]->removerAresta(indiceOrigem);
        }
        cout << "Aresta excluida com sucesso!\n";
    }
}
#pragma endregion

/*Nessa região estão desenvolvidos os algoritmos de caminho mínimo: Dijkstra e Floyd*/
#pragma region Caminho Mínimo
vector<int> Grafo::dijkstraAux(int indice) {
    vector<int> distancias(nos.size(), INT_MAX); // inicializa o vetor de distância com infinito
    vector<bool> visitados(nos.size(), false);
    // Distância da raiz recebe 0.
    distancias[indice] = 0;
    for (int i = 0; i < nos.size() - 1; i++) {
        // u recebe o indice da menor distância no vetor de Distâncias.
        int u = indiceMenorDistancia(distancias, visitados);
        visitados[u] = true;

        No* pont = nos[u];
        vector<Aresta*> arestas = *(pont->getArestas());

        for (int j = 0; j < arestas.size(); j++) {
            int auxIndice = arestas[j]->getDestino();

            if (!visitados[auxIndice] && distancias[u] != INT_MAX) {
                // Altera a distância se o adjacente não foi visitado e se há uma aresta que, se
                // somada à atual distâncias, seja menor que o valor da distancia atual para o adjacente.
                if (distancias[auxIndice] > distancias[u] + arestas[j]->getPeso())
                    distancias[auxIndice] = distancias[u] + arestas[j]->getPeso();

            }
        }
    }
    return distancias;
}

int Grafo::dijkstra(string idOrigem, string idDestino)
{
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
    return dijkstraAux(indiceOrigem)[indiceDestino];
}

vector<vector<int>> Grafo::floydAux() {
    Aresta* arestaAux; // auxiliar que conterá a aresta do nó i ao j na primeira fase do Floyd
    // matriz que será preenchida pelo algoritmo de Floyd
    vector<vector<int>> matrizDistancia(nos.size(), vector<int>(nos.size()));

    //este loop preenche a matriz com os pesos das arestas existentes e seta 0 nos cominhos entre o no e ele mesmo
    for (int i = 0; i != nos.size(); i++) {
        for (int j = 0; j < nos.size(); j++) {
            if (i != j) {
                // aux recebe todas as arestas entre os nos i e j
                arestaAux = nos[i]->encontrarArestasComDestino(j);
                if (arestaAux != NULL) {
                    //caso seja multigrafo, pegar a aresta de menor peso
                    matrizDistancia[i][j] = arestaAux->getPeso();
                }
                else {
                    //caso não haja arestas entre i e j, o caminho entre eles é infinito
                    matrizDistancia[i][j] = INT_MAX;
                }
            }
            else {
                matrizDistancia[i][j] = 0;
            }
        }
    }

    //este loop itera procurando por distâncias menores usando outros nós como intermediários
    for (int i = 0; i < nos.size(); i++) {
        for (int j = 0; j < nos.size(); j++) {
            for (int k = 0; k < nos.size(); k++) {
                if (i == j)  break; // caso seja o mesmo nó, ir para a proxima iteraçao
                // caso o nó intermediário seja um dos nós a se calcular a distância, podemos seguir para a próxima iteração pois é impossível diminuir o caminho usando eles mesmos
                if (k == i || k == j)  continue;

                // caso ainda não haja caminho do i ao k ou do k ao j, impossível criar caminho
                if (matrizDistancia[i][k] == INT_MAX || matrizDistancia[k][j] == INT_MAX)  continue;

                int result = matrizDistancia[i][k] + matrizDistancia[k][j]; // calcular o caminho usando k como intermediário
                if (matrizDistancia[i][j] > result) { // trocar caso seja maior
                    matrizDistancia[i][j] = result;
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

    return floydAux()[indiceOrigem][indiceDestino];
}

int Grafo::indiceMenorDistancia(vector<int> distancias, vector<bool> visitados) {

    int minimo = INT_MAX;
    int indice = 0;

    for (int i = 0; i < nos.size(); i++) {
        // caso o nó não tenha sido visitado ainda e sua distância é menor que o mínimo atual, trocar
        if (!visitados[i] && distancias[i] < minimo) {
            minimo = distancias[i];
            indice = i;
        }
    }
    return indice;
}

#pragma endregion

/*Nessa região estão presentes as funções responsáveis por realizar a busca no grafo */
#pragma region Busca

// um ponteiro para vector<bool> é usado para que todas as chamadas compartilhem do mesmo vector
// bool print true significa que devemos imprimir os valores visitados
void Grafo::buscaEmProfundidadeAux(int indiceOrigem, vector<bool> *visitado, bool print) {
    (*visitado)[indiceOrigem] = true;
    if (print)  cout << nos[indiceOrigem]->getId() << " ";
    vector<Aresta*> arestas = *(nos[indiceOrigem]->getArestas());
    for (int i = 0; i < arestas.size(); i++) {
        int indiceDestino = arestas[i]->getDestino();
        // caso o nó já tenha sido visitado, podemos continuar pois as arestas dele já estão sendo iteradas
        // caso contrário, devemos inciar a busca nele
        if (!(*visitado)[indiceDestino])
            buscaEmProfundidadeAux(indiceDestino, visitado, print);
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
        if (visitado[i])  continue;  // caso o nó já tenha sido visitado, podemos pular
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
    vector<Aresta*> arestasAux;

    cout << "Busca em largura \n";
    // setar condições do loop para começar no indiceInicio passado
    // loop necessário para fazer com que todas as componentes sejam visitadas
    for (int i = indiceInicio; i < indiceInicio + nos.size(); i++) {
        if (visitado[i % nos.size()])  continue;
        filaVisitados.push(i % nos.size());
        while (!filaVisitados.empty()) {
            indice = filaVisitados.front();
            filaVisitados.pop();

            // Com o índice atual, podemos realizar a busca nas arestas
            arestasAux = *(nos[indice]->getArestas());

            if (!visitado[indice]) {
                cout << nos[indice]->getId() << " ";
                visitado[indice] = true;
            }

            for (int k = 0; k < arestasAux.size(); k++) {
                int destino = arestasAux[k]->getDestino();
                if (!visitado[destino]) {
                    // À medida que são encontrados nós ainda não visitados, eles são imprimidos e adicionados à fila
                    cout << nos[destino]->getId() << " ";
                    visitado[destino] = true;
                    filaVisitados.push(destino);
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
    vector<Aresta*> arestas;
    int indexDestino;

    for (int i = 0; i < nos.size(); i++) {
        if (nos[i]->getGrau() != nos.size() - 1)  return false;  // todos os nós precisam se conectar a n-1 nós
        arestas = *(nos[i]->getArestas());
        for (int j = 0; j < arestas.size(); j++) {
            indexDestino = arestas[j]->getDestino();
            if (destinos[indexDestino])  return false; // caso haja aresta paralela, não é completo
            if (i == indexDestino)  return false; // caso haja laço, não é completo
            destinos[indexDestino] = true;
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

    if (!result)  return false; // caso já tenha sido detectado que o grafo não é euleriano, podemos retornar

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
    vector<Aresta*> arestas; // vector
    vector<bool> destinos = vector<bool>(nos.size(), false); // vector que guarda os destinos alcançados pelas arestas do nó atual
    int indiceDestino; // variável auxiliar que guarda o indice do nó de destino da aresta dentro do loop
    bool result = false; // resultado da verificação

    for (int i = 0; i < nos.size(); i++) {
        arestas = *(nos[i]->getArestas());
        for (int j = 0; j < arestas.size(); j++) {
            if (arestas[j]->getOrigem() == arestas[j]->getDestino())  return false; // multigrafo não tem laços

            indiceDestino = arestas[j]->getDestino();

            if (destinos[indiceDestino])  result = true; // caso já haja uma aresta para este nó, é multigrafo
            else destinos[indiceDestino] = true;
        }
        fill(destinos.begin(), destinos.end(), false); // resetamos o vector destino após checarmos todas as arestas de um nó
    }
    return result;
}

bool Grafo::isSimples() {
    vector<bool> destinos = vector<bool>(nos.size(), false); // vector que guarda os destinos alcançados pelas arestas do nó atual
    vector<Aresta*> arestas; // vector auxiliar para receber as arestas de cada nó dentro do loop
    int indiceDestino; // variável auxiliar que guarda o indice do nó de destino da aresta dentro do loop

    for (int i = 0; i < nos.size(); i++) {
        arestas = *(nos[i]->getArestas());
        for (int j = 0; j < arestas.size(); j++) {
            indiceDestino = arestas[j]->getDestino();

            if (nos[i]->getId().compare(nos[indiceDestino]->getId()) == 0)  return false; // caso haja laços, não é grafo simples
            if (destinos[indiceDestino])  return false; // caso haja arestas paralelas, não é grafo simples

            destinos[indiceDestino] = true;
        }
        fill(destinos.begin(), destinos.end(), false);
    }
    return true;
}

// método auxiliar para calcular as componentes fortemente conexas. É basicamente uma busca em profundidade retornando a ordem dos nós visitados na pilha passada
void Grafo::ordemBuscaProfundidade(int indice, vector<bool> *visitado, stack<int> &pilha) {
    (*visitado)[indice] = true;

    // Chamar recursivo para todas as arestas
    for (int i = 0; i < (*(nos[indice]->getArestas())).size(); i++) {
        int indiceDestino = (*(nos[i]->getArestas()))[i]->getDestino();
        // caso o nó já tenha sido visitado, podemos continuar pois as arestas dele já estão sendo iteradas
        // caso contrário, devemos chamar a função recursivamente nele
        if (!(*visitado)[indiceDestino])
            ordemBuscaProfundidade(indiceDestino, visitado, pilha);
    }

    // Pomos na pilha a ordem dos vérticas já processados
    pilha.push(indice);
}

int Grafo::componentesFortementeConexas() {
    vector<string> ids; // vector contendo todos os ids dos nós do grafo para criar o grafo transposto
    // vector que conterá todas as arestas do grafo para criar o grafo transposto
    // inicializado de tamanho 1 para que um SEGFAULT não seja gerado ao acessar arestas.end()
    vector<Aresta*> arestasGeral;

    vector<Aresta*> arestas; // vector auxiliar que guarda as arestas do nó atual dentro do loop
    for (int i = 0; i < nos.size(); i++) {
        ids.push_back(nos[i]->getId());
        arestas = *(nos[i]->getArestas());
        arestasGeral.insert(arestasGeral.end(), arestas.begin(), arestas.end());
    }

    // alocar memória para o grafo pois talvez requeira uma memória considerável
    Grafo* transposto = new Grafo(ids, arestasGeral);

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

    delete(transposto); // desaloca a memória do grafo transposto

    return componentes;
}

void Grafo::showComponentesFortementeConexas() {
    if (isDigrafo) {
        cout << "Componentes fortemente conexas: \n";
        componentesFortementeConexas();
    }
    else {
        cout << "Componentes fortemente conexas so se aplicam em grafos direcionados!\n";
    }
}

bool Grafo::isBipartido() {
    // vector que conterá as partições de cada nó
    // Os valores possíveis no vector são 3: SEM_PARTICAO, PARTICAO_A e PARTICAO_B
    vector<int> bipartido(nos.size(), SEM_PARTICAO);
    vector<Aresta*> arestas; // vector auxiliar para receber as arestas de cada nó dentro do loop
    int indiceDestino;

    for (int i = 0; i < nos.size(); i++) {
        // caso não tenha partição, pomos o nó na partição A
        if (bipartido[i] == SEM_PARTICAO) bipartido[i] = PARTICAO_A;

        arestas = *(nos[i]->getArestas());

        for (int j = 0; j < arestas.size(); j++) {
            if (i == arestas[j]->getOrigem()) {
                //Caso tenha laço, o mesmo nó não pode estar em duas partições
                if (arestas[j]->getOrigem() == arestas[j]->getDestino())  return false;

                indiceDestino = arestas[j]->getDestino();

                if (bipartido[indiceDestino] == SEM_PARTICAO) {
                    // caso o adjacente não tenha partição ainda, pomos ele na outra partição
                    if (bipartido[i] == PARTICAO_A)  bipartido[indiceDestino] = PARTICAO_B;
                    else  bipartido[indiceDestino] = PARTICAO_A;
                }
                else {
                    //Se o nó atual tiver um nó adjacente que esteja na mesma partição, o grafo não é bipartido
                    if (bipartido[i] == bipartido[indiceDestino]) return false;
                }
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
    for (int i = 0; i < nos.size(); i++){
        if(i % 10 == 0) cout << endl; // imprimir 10 por linha
        cout << nos[i]->getId() << " ";
    }
    cout << "}\n\nE: {";
    int count = 0;
    //alocar no heap pois podem ser muitas arestas
    Aresta* aux;
    for (int i = 0; i < nos.size(); i++) {
        for (int j = 0; j < nos.size(); j++) {
            if (i == j)  continue;
            aux = nos[i]->encontrarArestasComDestino(j);
            if (aux == NULL && (isDigrafo || j < i)) {
                if(count % 10 == 0) cout << endl; // imprimir 10 por linha
                count++;
                cout << "(" << nos[i]->getId() << ", " << nos[j]->getId() << ") ";
            }
        }
    }

    cout << "}\n";
}

void Grafo::subGrafoInduzido(set<string> listaNo) {
    vector<int> nosInduzidos; // vector que conterá os índices dos nós listados
    vector<Aresta*> arestasInduzidas; // vector que conterá as arestas do subgrafo induzido
    Aresta* aresta; // vector auxiliar que conterá as arestas do nó dentro do loop

    set<string>::iterator it;
    int indice;
    for (it = listaNo.begin(); it != listaNo.end(); ++it) {
        if ((indice = getIndexNo(*it)) != -1) {
            nosInduzidos.push_back(indice);
        }
        else {
            printMensagemNoInexistente(*it);
            return;
        }
    }

    // Insere as arestas induzidas no vector arestasInduzidas
    for (int i = 0; i < nosInduzidos.size(); i++) {
        for (int j = 0; j < nosInduzidos.size(); j++) {
            aresta = nos[nosInduzidos[i]]->encontrarArestasComDestino(nosInduzidos[j]);
            if (aresta != NULL) arestasInduzidas.push_back(aresta);
        }
    }

    // string auxiliar para printar o resultado corretamente
    string result = "O subgrafo induzido resultante eh G(V,E) onde:\nV = { ";
    for (int i = 0; i < nosInduzidos.size(); i++)  result += nos[nosInduzidos[i]]->getId() + ", ";
    result.erase(result.size() - 2, 1); // apagar última vírgula para mostrar corretamente
    result += "}\n";
    cout << result;
    result = "E = { ";
    for (int i = 0; i < arestasInduzidas.size(); i++) {
        result += "(" + nos[arestasInduzidas[i]->getOrigem()]->getId() + " , " + nos[arestasInduzidas[i]->getDestino()]->getId();
        if (isPonderado) result += ", " + arestasInduzidas[i]->getPeso();
        result += "), ";
    }
    result.erase(result.size() - 2, 1); // apagar última vírgula para mostrar corretamente
    result += "}\n";
    cout << result;
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
void Grafo::arestasPonteAux(int atual, vector<bool> *visitado, vector<int> *descoberta, vector<int> *min, vector<int> *pai, bool *existe) {
    // variável estática que determina o tempo de descoberta
    static int tempo = 0;

    // Marca o nó atual como visitado
    (*visitado)[atual] = true;

    // Inicializa o tempo de descoberta e min
    (*descoberta)[atual] = (*min)[atual] = ++tempo;

    // Percorrer todas as arestas do nó atual
    vector<Aresta*> arestas = *(nos[atual]->getArestas());
    for (int i = 0; i < arestas.size(); i++) {
        int adjacente = arestas[i]->getDestino();
        // Caso o adjacente não tenha sido visitado
        if (!(*visitado)[adjacente]) {
            (*pai)[adjacente] = atual;
            arestasPonteAux(adjacente, visitado, descoberta, min, pai, existe);

            // Checar se algum filho do adjacente tem conexão com u ou um ancestral de u
            if ((*min)[adjacente] < (*min)[atual])  (*min)[atual] = (*min)[adjacente];

            // Se o vértice descoberto mais cedo alcançável da subárvore abaixo do adjacente
            // está abaixo do nó atual, a aresta em questão é ponte
            if ((*min)[adjacente] > (*descoberta)[atual]) {
                cout << "(" << nos[atual]->getId() << ", " << nos[adjacente]->getId() << ")\n";
                *existe = true;
            }

        }

            // Atualizar o valor min do nó atual para as chamadas recursivas
        else if (adjacente != (*pai)[atual])
            if ((*descoberta)[adjacente] < (*min)[atual])  (*min)[atual] = (*descoberta)[adjacente];
    }
}

void Grafo::showArestasPonte()
{
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

    if (!arestaPonteExiste)  cout << "Nao ha arestas ponte\n";
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
void Grafo::noArticulacaoAux(int atual, vector<bool> *visitado, vector<int> *descoberta, vector<int> *min, vector<int> *pai, bool *existe) {
    // variável estática que determina o tempo de descoberta
    static int tempo = 0;

    // Número de filhos do nó atual na árvore gerada pela busca em profundidade
    int filhos = 0;

    // Marca o nó atual como visitado
    (*visitado)[atual] = true;

    // Seta o tempo de descoberta do nó atual e a alcançabilidade do nó
    (*descoberta)[atual] = (*min)[atual] = ++tempo;

    // Percorrer todas as arestas do nó atual
    vector<Aresta*> arestas = *(nos[atual]->getArestas());
    for (int i = 0; i < arestas.size(); i++) {
        int adjacente = arestas[i]->getDestino();

        // Se adjacente ainda não foi visitado, então setá-lo para filho do nó atual
        // na árvore da busca em profundidade e fazer uma chamada recursiva nele
        if (!(*visitado)[adjacente]) {
            filhos++;
            (*pai)[adjacente] = atual;
            noArticulacaoAux(adjacente, visitado, descoberta, min, pai, existe);

            // Checa se um nó da subávore com raiz adjacente tem conexão com o nó atual ou algum ancestral do nó atual
            if ((*min)[adjacente] < (*min)[atual])  (*min)[atual] = (*min)[adjacente];

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
            if ((*descoberta)[adjacente] < (*min)[atual])  (*min)[atual] = (*descoberta)[adjacente];
    }
}

void Grafo::showNoArticulacao() {
    // Inicializar os vetores necessários para chamar o método auxiliar que descobre as arestas pontes
    vector<bool> visitado(nos.size(), false);
    vector<int> descoberta(nos.size());
    vector<int> min(nos.size());
    vector<int> pai(nos.size(), -1); // valor inicializado com -1 para ser usado dentro do método auxiliar para detectar a raiz da árvore gerada pela busca em profundidade

    cout << "Nos de articulacao:\n";
    bool noArticulacaoExiste = false;
    // Chamar a função auxiliar recursiva para encontrar os nós de articulação
    // na árvore gerada pela busca em profundidade com raiz em i
    for (int i = 0; i < nos.size(); i++)
        if (!visitado[i])
            noArticulacaoAux(i, &visitado, &descoberta, &min, &pai, &noArticulacaoExiste);

    if (!noArticulacaoExiste)  cout << "Nao ha nos de articulacao\n";
}

vector<int> Grafo::excentricidade() {
    vector<int> result(nos.size(), 0); // Valor mínimo para excentricidade é 0 que é o valor do nó para ele mesmo
    vector<vector<int>> matrizDistancia = floydAux(); // matriz com caminhos mais curtos do grafo
    for (int i = 0; i < nos.size(); i++) {
        for (int j = 0; j < nos.size(); j++) {
            // pega o maior caminho mínimo válido para cada no
            if (matrizDistancia[i][j] != INT_MAX && matrizDistancia[i][j] > result[i])  result[i] = matrizDistancia[i][j];
        }
    }
    return result;
}


int Grafo::componentesConexas() {
    vector<bool> visitados(nos.size(), false);  // vector que conterá os status dos nós percorridos na busca em profundidade
    int componentes = 0;

    for (int i = 0; i < nos.size(); i++) {
        if (visitados[i])  continue;
        // caso encontremos nós ainda não visitados após a busca em profundidade de outros nós,
        // quer dizer eles não estão conectados e, portanto, constituem uma nova componente conexa
        componentes++;
        buscaEmProfundidadeAux(i, &visitados, false);
    }

    return componentes;
}

void Grafo::printGrafo() {
    vector<Aresta*> arestas;
    for (int i = 0; i < nos.size(); i++) {
        arestas = *(nos[i]->getArestas());
        cout << "No " << nos[i]->getId() << ":  ";
        for (int j = 0; j < arestas.size(); j++) {
            // só devem ser imprimidas as arestas com destino maior ou igual à origem ou todas, quando é digrafo.
            // Isso faz com que arestas não sejam duplicadas em grafos não direcinados
            if(isDigrafo || arestas[j]->getDestino() < i){
                cout << "(" << nos[i]->getId() << ", " << nos[arestas[j]->getDestino()]->getId();
                // caso seja ponderado, é necessário mostrar os pesos
                if (isPonderado)  cout << ", " << arestas[j]->getPeso();
                cout << ") ";
            }
        }
        cout << endl;
    }
}

// Representação de sets disjuntos para ser usado no algoritmo de Kruskal ao gerar a árvore geradora mínima
struct SetDisjunto {
    vector<int> pai, rank;

    SetDisjunto(int n)
    {
        // Alocar memória e inicializar os vetores
        pai = vector<int>(n + 1);
        rank = vector<int>(n + 1);

        for (int i = 0; i <= n; i++) {
            //todo elemento é pai de si mesmo e tem rank 0
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
        if (rank[x] > rank[y])  pai[y] = x;
        else  pai[x] = y;

        if (rank[x] == rank[y])
            rank[y]++;
    }
};

struct comparadorAresta { // usado para ordenar as arestas baseadas no peso (usado no kruskalAux)
    inline bool operator() (Aresta* aresta1, Aresta* aresta2) {
        return aresta1->getPeso() < aresta2->getPeso();
    }
};

//Algoritmo de Kruskal auxiliar para cálculo da AGM
int Grafo::kruskalAux() {
    int pesoTotal = 0;

    vector<Aresta*> arestasGeral; // vector que conterá todas as arestas do grafo
    for (int i = 0; i < nos.size(); i++)
        arestasGeral.insert(arestasGeral.end(), nos[i]->getArestas()->begin(), nos[i]->getArestas()->end());

    // ordena as arestas baseadas em seu peso
    sort(arestasGeral.begin(), arestasGeral.end(), comparadorAresta());

    SetDisjunto sd(nos.size());

    for (int i = 0; i < arestasGeral.size(); i++) {
        int u = arestasGeral[i]->getOrigem();
        int v = arestasGeral[i]->getDestino();

        int set_u = sd.acharPai(u);
        int set_v = sd.acharPai(v);

        // Checar the a aresta selecionada está criando um ciclo ou não
        // Ciclo é criado se u e v são do mesmo set
        if (set_u != set_v) {
            // Aresta atual pertence à AGM
            cout << "(" << nos[u]->getId() << ", " << nos[v]->getId() << ")" << endl;
            pesoTotal += arestasGeral[i]->getPeso();
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

#pragma region Destrutor
Grafo::~Grafo() {
    for (int i = 0; i < nos.size(); i++) {
        delete(nos[i]);
    }
}
#pragma endregion
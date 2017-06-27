#pragma once
#include "No.h"
#include "Aresta.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <stack>
#include <map>

using namespace std;

class Grafo
{
private:
    Grafo(vector<string> ids, vector<tuple<int, int, int>>* arestas); // construtor auxiliar usado no cálculo das componentes fortemente conexas
    vector<No*> nos;
    map<string, int> idMap;
    bool isDigrafo = false; // assumimos que um grafo é não direcionado por padrão
    bool isPonderado = true; // assumimos que um grafo é ponderado por padrão
    string nomeArquivoSaida;
    string nomeDefaultNosSemAresta = "NO_SEM_ARESTA";

    void lerArquivo(string nomeArquivoEntrada);
    int getIndexNo(string id);
    int adicionarNoInexistente(string id);
    bool isSimples();
    void printMensagemNoInexistente(string id);
    int componentesFortementeConexas();
    void ordemBuscaProfundidade(int indice, vector<bool> *visitado, stack<int> &pilha);
    void buscaEmProfundidadeAux(int indiceOrigem, vector<bool> *visitado, bool print);
    void noArticulacaoAux(int indice, vector<bool> *visitado, vector<int> *descoberta, vector<int> *min, vector<int> *pai, bool *existe);
    void arestasPonteAux(int indice, vector<bool> *visitado, vector<int> *descoberta, vector<int> *min, vector<int> *pai, bool *existe);
    vector<vector<int>>* floydAux();
    int dijkstraAux(int indiceOrigem, int indiceDestino);
    int kruskalAux();
    pair<vector<No*>, int> construirSolucao(double alpha);
    pair<vector<No*>, int> construirSolucaoRandomizada(double alpha, int numIteracoes);
    void atualizaNosEArestas(No *noAdicionado, vector<pair<int, int>> *arestasGeral, vector<pair<No*, int>> *nosAux);
public:
    Grafo(string nomeArquivoEntrada, string nomeArquivoSaida);
    ~Grafo();
    bool ehPonderado() { return isPonderado; };
    vector<int> excentricidade();
    void printGrafo();
    void salvarArquivo();
    void buscaEmProfundidade(string id);
    void buscaEmLargura(string id);
    void inserirNo(string id);
    void excluirNo(string id);
    void inserirAresta(string idOrigem, string idDestino, int peso);
    void excluirAresta(string idOrigem, string idDestino);
    void grauNo(string id);
    void vizinhancaAberta(string id);
    void vizinhancaFechada(string id);
    int grauGrafo();
    int ordemGrafo();
    void complementar();
    int dijkstra(string idOrigem, string idDestino);
    int floyd(string idOrigem, string idDestino);
    void fechoTransitivoDireto(string id);
    void fechoTransitivoIndireto(string id);
    void sequenciaDeGraus();
    bool isTrivial();
    bool isNulo();
    bool isMultigrafo();
    bool isCompleto();
    bool isEuleriano();
    void showNoArticulacao();
    void showArestasPonte();
    bool isKRegular(int k);
    bool isBipartido();
    void subGrafoInduzido(set<string> listaNo);
    int raioGrafo();
    int diametroGrafo();
    void centroGrafo();
    void periferiaGrafo();
    void showComponentesFortementeConexas();
    void showArvoreGeradoraMinima();
    void showCoberturaGuloso();
    void showCoberturaGulosoRandomizado(double alpha, int numIteracoes);
    void showCoberturaGulosoRandomizadoReativo(int numIteracoes, int blocoIteracoes);
};


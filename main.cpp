#include <iostream>
#include <vector>
#include <climits>
#include "Grafo.h"


using namespace std;

void showMainMenu();
void showEditMenu();
void showBuscaMenu();
void showInfoNoMenu();
void showInfoGrafoMenu();
void showMenuCaminhoMinimo();
void showInfoExcentricidadeMenu();
void showInfoVerificaInfoGrafoMenu();

Grafo *grafo;

int main(int argc, char **argv) {

    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <nome do arquivo de entrada> <nome do arquivo de saida>\n";
        exit(EXIT_FAILURE);
    }

    grafo = new Grafo(argv[1], argv[2]);

    showMainMenu(); //Exibir Menu Principal

    grafo->salvarArquivo();

    delete(grafo);
    return 0;
}

#pragma region Leitura e impressão
string getStringInput(string msg) {
    string id;
    cout << msg;
    cin >> id;
    return id;
}

static string getBooleanString(bool b) {
    if (b)  return "Sim\n";
    else return "Nao\n";
}
#pragma endregion

//Nessa região estão as funções que exibem as funções exitente no menu
#pragma region Menu
void cleanConsole() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\x1B[2J\x1B[H";
#endif
}

void showMainMenu() {
    int option = 0;
    while (option !=8) {
        cout << "Selecione uma das opcoes abaixo apertando seu respectivo numero\n";
        cout << "1. Imprimir grafo\n";
        cout << "2. Editar grafo\n";
        cout << "3. Informacoes Grafo\n";
        cout << "4. Informacoes No\n";
        cout << "5. Busca\n";
        cout << "6. Caminho Minimo\n";
        cout << "7. Salvar grafo\n";
        cout << "8. Sair\n\n";
        cout << "Opcao escolhida: ";
        cin >> option;

        cleanConsole();

        switch (option) {
            case 1:
                grafo->printGrafo();
                cout << endl;
                break;
            case 2:
                showEditMenu();
                break;
            case 3:
                showInfoGrafoMenu();
                break;
            case 4:
                showInfoNoMenu();
                break;
            case 5:
                showBuscaMenu();
                break;
            case 6:
                showMenuCaminhoMinimo();
                break;
            case 7:
                grafo->salvarArquivo();
                cout << endl;
                break;
            case 8:
                break;
            default:
                cout << "Opcao invalida! \n\n";
        }
    }
}

void showEditMenu() {
    int option = 0;
    while (option != 5) {
        cout << "Selecione uma das opcoes abaixo apertando seu respectivo numero\n";
        cout << "1. Inserir no\n";
        cout << "2. Inserir aresta\n";
        cout << "3. Excluir no\n";
        cout << "4. Excluir aresta\n";
        cout << "5. Voltar ao menu anterior\n\n";
        cout << "Opcao escolhida: ";
        cin >> option;

        cleanConsole();

        switch (option) {
            case 1:
                grafo->inserirNo(getStringInput("ID do novo no: "));
                break;
            case 2:
            {
                string idOrigem = getStringInput("ID do no de origem: ");
                string idDestino = getStringInput("ID do no de destino: ");
                int peso = 1; // peso é setado para 1 por padrão para grafos não ponderados. Caso seja ponderado, devemos lê-lo
                if(grafo->ehPonderado()){
                    string pesoInput = getStringInput("Peso da aresta: ");
                    peso = atoi(pesoInput.c_str());
                    if(peso == 0 && pesoInput != "0"){
                        // quando peso é 0 e o input não especificou 0, de fato, ocorreu erro na conversão
                        // da string para o inteiro. Neste caso, paramos
                        cout << "Peso invalido!\n";
                        break;
                    }
                }
                grafo->inserirAresta(idOrigem, idDestino, peso);
            }
                break;
            case 3:
                grafo->excluirNo(getStringInput("ID do no a ser excluido: "));
                break;
            case 4:
            {
                string idOrigem = getStringInput("ID do no de origem: ");
                string idDestino = getStringInput("ID do no de destino: ");
                grafo->excluirAresta(idOrigem, idDestino);
            }
                break;
            case 5:
                break;
            default:
                cout << "Opcao invalida! \n\n";
        }

        if (option != 5) {
            cout << endl;
        }
    }
}

void showInfoGrafoMenu() {
    int option = 0;
    while (option != 13) {
        cout << "Selecione uma das opcoes abaixo apertando seu respectivo numero\n";


        cout << "1. Grau do grafo\n";
        cout << "2. Informacoes sobre excentricidade\n";
        cout << "3. Mostrar arestas ponte\n";
        cout << "4. Mostrar a sequencia de graus\n";
        cout << "5. Mostrar nos de articulacao\n";
        cout << "6. Mostrar o complementar do grafo\n";
        cout << "7. Ordem do grafo\n";
        cout << "8. Subgrafo induzido\n";
        cout << "9. Verificar a k-regularidade do grafo\n";
        cout << "10. Verificar componentes fortemente conexas\n";
        cout << "11. Verificar propriedades do grafo\n";
        cout << "12. Mostrar arvore geradora minima\n";
        cout << "13. Voltar ao menu anterior\n\n";
        cout << "Opcao escolhida: ";

        cin >> option;

        cleanConsole();

        switch (option) {

            case 1:
                cout << "O grau do grafo eh " << grafo->grauGrafo() << endl;
                break;
            case 2:
                showInfoExcentricidadeMenu();
                break;
            case 3:
                grafo->showArestasPonte();
                break;
            case 4:
                cout << "Sequencia de graus do grafo: ";
                grafo->sequenciaDeGraus();
                cout << endl;
                break;
            case 5:
                grafo->showNoArticulacao();
                break;
            case 6:
                grafo->complementar();
                break;
            case 7:
                cout << "A ordem do grafo eh " << grafo->ordemGrafo() << endl;
                break;
            case 8:
            {
                set<string> listaNoInduzido;

                cout << "Digite FIM quando finalizar a digitacao dos nos!\n";

                while (true) {
                    string idNo = getStringInput("ID do no: ");
                    if (idNo == "FIM") break;
                    if (listaNoInduzido.count(idNo)) cout << "No ja presente na lista!\n";
                    else listaNoInduzido.insert(idNo);
                }

                grafo->subGrafoInduzido(listaNoInduzido);
            }
                break;
            case 9:
            {
                int k = atoi(getStringInput("K: ").c_str());
                if (grafo->isKRegular(k))  cout << "O grafo eh " << k << "-regular.\n";
                else  cout << "O grafo nao eh " << k << "-regular.\n";
            }
                break;

            case 10:
                grafo->showComponentesFortementeConexas();
                break;
            case 11:
                showInfoVerificaInfoGrafoMenu();
                break;
            case 12:
                grafo->showArvoreGeradoraMinima();
                break;
            case 13:
                break;
            default:
                cout << "Opcao invalida! \n\n";
        }

        if (option != 13) {
            cout << endl;
        }
    }
}

void showMenuCaminhoMinimo() {
    int option = 0;
    while (option != 3) {
        cout << "Selecione uma das opcoes abaixo apertando seu respectivo numero\n";
        cout << "1. Dijkstra\n";
        cout << "2. Floyd\n";
        cout << "3. Voltar ao menu anterior\n\n";
        cout << "Opcao escolhida: ";
        cin >> option;

        cleanConsole();

        switch (option) {
            case 1:
            {
                string idOrigem = getStringInput("ID do no de origem: ");
                string idDestino = getStringInput("ID do no de destino: ");
                int result = grafo->dijkstra(idOrigem, idDestino);
                if (result != -1) {
                    if (result == INT_MAX) {
                        cout << "Nao ha caminho entre o no " << idOrigem << " e o no " << idDestino << endl;
                    }
                    else {
                        cout << "O menor caminho entre o no " << idOrigem << " e o no " << idDestino << " eh " << result << endl;
                    }
                }
            }
                break;
            case 2:
            {
                string idOrigem = getStringInput("ID do no de origem: ");
                string idDestino = getStringInput("ID do no de destino: ");
                int result = grafo->floyd(idOrigem, idDestino);
                if (result != -1) {
                    if (result == INT_MAX) {
                        cout << "Nao ha caminho entre o no " << idOrigem << " e o no " << idDestino << endl;
                    }
                    else {
                        cout << "O menor caminho entre o no " << idOrigem << " e o no " << idDestino << " eh " << result << endl;
                    }
                }
            }
                break;
            case 3:
                break;
            default:
                cout << "Opcao invalida! \n\n";
        }

        if (option != 3) {
            cout << endl;
        }
    }
};

void showBuscaMenu() {
    int option = 0;
    while (option != 3) {
        cout << "Selecione uma das opcoes abaixo apertando seu respectivo numero\n";
        cout << "1. Largura\n";
        cout << "2. Profundidade\n";
        cout << "3. Voltar ao menu anterior\n\n";
        cout << "Opcao escolhida: ";
        cin >> option;

        cleanConsole();

        switch (option) {
            case 1:
                grafo->buscaEmLargura(getStringInput("ID do no inicial: "));
                break;
            case 2:
                grafo->buscaEmProfundidade(getStringInput("ID do no inicial: "));
                break;
            case 3:
                break;
            default:
                cout << "Opcao invalida! \n\n";
        }

        if (option != 3) {
            cout << endl;
        }
    }
};

void showInfoNoMenu() {
    int option = 0;
    while (option != 6) {
        cout << "Selecione uma das opcoes abaixo apertando seu respectivo numero\n";
        cout << "1. Fecho transitivo direto de um no\n";
        cout << "2. Fecho transitivo indireto de um no\n";
        cout << "3. Grau de um no\n";
        cout << "4. Vizinhanca aberta de um no\n";
        cout << "5. Vizinhanca fechada de um no\n";
        cout << "6. Voltar ao menu anterior\n\n";
        cout << "Opcao escolhida: ";
        cin >> option;

        cleanConsole();

        switch (option) {
            case 1:
                grafo->fechoTransitivoDireto(getStringInput("ID do no: "));
                grafo->excentricidade();
                break;
            case 2:
                grafo->fechoTransitivoIndireto(getStringInput("ID do no: "));
                break;
            case 3:
                grafo->grauNo(getStringInput("ID do no: "));
                break;
            case 4:
                grafo->vizinhancaAberta(getStringInput("ID do no: "));
                break;
            case 5:
                grafo->vizinhancaFechada(getStringInput("ID do no: "));
                break;
            case 6:
                break;
            default:
                cout << "Opcao invalida! \n\n";
        }

        if (option != 6) {
            cout << endl;
        }
    }
};

void showInfoExcentricidadeMenu() {
    int option = 0;
    while (option != 5) {
        cout << "Selecione uma das opcoes abaixo apertando seu respectivo numero\n";
        cout << "1. Centro do Grafo\n";
        cout << "2. Diametro do Grafo\n";
        cout << "3. Periferia do Grafo\n";
        cout << "4. Raio do Grafo\n";
        cout << "5. Voltar ao menu anterior\n\n";
        cout << "Opcao escolhida: ";
        cin >> option;

        cleanConsole();

        switch (option) {
            case 1:
                grafo->centroGrafo();
                break;
            case 2:
                cout << "Diametro do Grafo:" << grafo->diametroGrafo() << endl;
                break;
            case 4:
                cout << "Raio do Grafo:" << grafo->raioGrafo() << endl;
                break;
            case 3:
                grafo->periferiaGrafo();
                break;
            case 5:
                break;
            default:
                cout << "Opcao invalida! \n\n";
        }

        if (option != 5) {
            cout << endl;
        }
    }
};

void showInfoVerificaInfoGrafoMenu() {
    int option = 0;
    while (option != 7) {
        cout << "Selecione uma das opcoes abaixo apertando seu respectivo numero\n";
        cout << "1. Bipartido?\n";
        cout << "2. Completo?\n";
        cout << "3. Euleriano?\n";
        cout << "4. Multigrafo?\n";
        cout << "5. Nulo?\n";
        cout << "6. Trivial?\n";
        cout << "7. Voltar ao menu anterior\n\n";
        cout << "Opcao escolhida: ";
        cin >> option;

        cleanConsole();

        switch (option) {
            case 1:
                cout << "Bipartido: " << getBooleanString(grafo->isBipartido());
                break;
            case 2:
                cout << "Completo: " << getBooleanString(grafo->isCompleto());
                break;
            case 3:
                cout << "Euleriano: " << getBooleanString(grafo->isEuleriano());
                break;
            case 4:
                cout << "Multigrafo: " << getBooleanString(grafo->isMultigrafo());
                break;
            case 5:
                cout << "Nulo: " << getBooleanString(grafo->isNulo());
                break;
            case 6:
                cout << "Trivial: " << getBooleanString(grafo->isTrivial());
                break;
            case 7:
                break;
            default:
                cout << "Opcao invalida! \n\n";
        }

        if (option != 7) {
            cout << endl;
        }
    }
};

#pragma endregion


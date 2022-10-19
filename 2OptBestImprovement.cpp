#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <limits>
#include <fstream>

#define INF_POS std::numeric_limits<float>::max();

using namespace std;

class Vertice
{
public:
    float x, y; // coordenadas do vertice

    Vertice() {}

    Vertice(float i, float j)
    {
        x = i;
        y = j;
    }
};

// Remove o elemento de índice i da listaVertices em tempo constante.
// Não preserva a ordem original dos elementos
void remover(int i, vector<int> &listaVertices)
{
    int ultimoIdx = listaVertices.size() - 1;
    int aux = listaVertices[i];
    listaVertices[i] = listaVertices[ultimoIdx];
    listaVertices[ultimoIdx] = aux;
    listaVertices.pop_back();
}

class PCVSolver
{
private:
    vector<Vertice> vertices;
    int numVertices;
    vector<int> melhorCaminho;
    float melhorDistancia;

public:
    // Inicializa a classe com a lista e a quantidade de vértices
    PCVSolver(vector<Vertice> listaVertices, int n)
    {
        vertices = listaVertices;
        numVertices = n;
    }

    // Retorna a distância entre os vértices v1 e v2.
    float distancia(Vertice v1, Vertice v2)
    {
        float deltaX = v2.x - v1.x;
        float deltaY = v2.y - v1.y;

        return sqrt((deltaX * deltaX) + (deltaY * deltaY));
    }

    // Retorna a distância percorrida pelo caminho inteiro.
    float distanciaCaminho(vector<int> caminho)
    {
        int i;
        float distanciaTotal = 0;
        for (i = 0; i < (int)caminho.size() - 1; i++)
        {
            distanciaTotal += distancia(vertices[caminho[i]], vertices[caminho[i + 1]]);
        }
        distanciaTotal += distancia(vertices[caminho[i]], vertices[caminho[0]]);
        return distanciaTotal;
    }

    // Imprime todos os vértices do melhorCaminho
    void printCaminho()
    {
        for (int j = 0; j < (int)melhorCaminho.size(); j++)
        {
            cout << melhorCaminho[j] << " => ";
        }
        cout << melhorCaminho[0] << endl;
    }

    // Imprime as coordenadas de todos os vértices na tela
    void printVertices()
    {
        for (int j = 1; j < numVertices + 1; j++)
        {
            cout << j << " => x: " << vertices[j].x << " y: " << vertices[j].y << "\n";
        }
    }

    // Retorna o índice do vizinho mais próximo de verticeAtual
    // Retorna também o índice deste vizinho na lista de vértices disponíveis;
    int vizinhoMaisProximo(int verticeAtual, vector<int> &idxVerticesDisponiveis, int &idxListaDisponiveis)
    {
        float distanciaCalc, tamanhoMenorAresta;
        int idxMaisProximo;

        // tamanhoMenorAresta inicialmente possui o maior valor de float possível
        tamanhoMenorAresta = INF_POS;
        for (int i = 0; i < (int)idxVerticesDisponiveis.size(); i++)
        {
            distanciaCalc = distancia(vertices[verticeAtual], vertices[idxVerticesDisponiveis[i]]);
            if (distanciaCalc < tamanhoMenorAresta)
            {
                tamanhoMenorAresta = distanciaCalc;
                idxMaisProximo = idxVerticesDisponiveis[i];
                idxListaDisponiveis = i;
            }
        }
        return idxMaisProximo;
    }

    // Aplica o algoritmo do vizinho mais próximo para o PCV.
    float solveVizinhoMaisProximo(int verticeInicial)
    {
        // Inicializa a lista de vértices disponíveis
        vector<int> idxVerticesDisponiveis;
        for (int i = 1; i <= numVertices; i++)
        {
            idxVerticesDisponiveis.push_back(i);
        }

        int idxMaisProximo, idxRemover;
        melhorDistancia = 0;

        int idxVerticeAtual = verticeInicial;
        remover(idxVerticeAtual - 1, idxVerticesDisponiveis);
        while (idxVerticesDisponiveis.size() > 0)
        {
            melhorCaminho.push_back(idxVerticeAtual);
            // Seleciona o vizinho mais próximo
            idxMaisProximo = vizinhoMaisProximo(idxVerticeAtual, idxVerticesDisponiveis, idxRemover);
            idxVerticeAtual = idxMaisProximo;
            remover(idxRemover, idxVerticesDisponiveis);
        }
        melhorCaminho.push_back(idxVerticeAtual);
        melhorDistancia = distanciaCaminho(melhorCaminho);

        return melhorDistancia;
    }

    // Calcula a diferença entre as distâncias do caminho antes
    // e depois da operação de troca 2-opt entre os vértices
    // (caminho[i-1], caminho[i]) e (caminho[j], caminho[j+1]).
    // Caso a diferença seja negativa, o caminho em que a troca foi
    // realizada possui distância menor.
    // Assume que i < j.
    float diferencaTroca2Opt(vector<int> &caminho, int i, int j)
    {
        int iMenos = (numVertices + ((i - 1) % numVertices)) % numVertices;
        int jMais = (j + 1) % numVertices;
        float antes1 = distancia(vertices[caminho[i]], vertices[caminho[iMenos]]);
        float antes2 = distancia(vertices[caminho[j]], vertices[caminho[jMais]]);
        float depois1 = distancia(vertices[caminho[iMenos]], vertices[caminho[j]]);
        float depois2 = distancia(vertices[caminho[i]], vertices[caminho[jMais]]);

        return (depois1 + depois2) - (antes1 + antes2);
    }

    // Realiza a troca 2-opt no caminho dado.
    // Faz a troca dos vértices (caminho[i-1], caminho[i]) e
    // (caminho[j], caminho[j+1]), através da inversão dos
    // elementos de caminho[i] até caminho[j].
    void trocar2Opt(vector<int> &caminho, int i, int j)
    {
        while (i < j)
        {
            swap(caminho[i], caminho[j]);
            i++;
            j--;
        }
    }

    // Registra dados da iteração no arquivo arq
    void registraIteracao(ofstream &arq, clock_t tempoInicial, int iter, float distancia)
    {
        float tempoAtual = (clock() - tempoInicial) / (float)CLOCKS_PER_SEC;
        arq << "#" << endl;
        arq << "Iteracao: " << iter << endl;
        arq << "Distancia: " << distancia << endl;
        arq << "Tempo: " << tempoAtual << "s" << endl;
    }

    // Aplica o algoritmo 2-opt para o PCV.
    // A cada iteração, realiza as trocas que mais diminuem a distância do caminho
    float solve2OptBestImprovement()
    {
        clock_t tempoInicial = clock();
        
        // Gera caminho inicial
        solveVizinhoMaisProximo(1);
        float diferencaDistancia, melhorDiferencaDistancia;
        int contadorSemMelhora = 0, maximoSemMelhora = 1;
        int iter = 0, maxIteracoes = 10000;
        int i, j, melhor_i, melhor_j;

        ofstream arq;
        arq.open("logBestImprovement.txt", ios::out);

        // Cada vez que o 2-opt for executado sem melhorar a distância do caminho,
        // contadorSemMelhora é incrementado.
        // maximoSemMelhora indica o número máximo de vezes seguidas que o 2-opt
        // pode ser executado sem melhorar o resultado.
        while (contadorSemMelhora < maximoSemMelhora && iter < maxIteracoes)
        {
            contadorSemMelhora++;
            registraIteracao(arq, tempoInicial, iter, melhorDistancia);

            // Seleciona a melhor entre as soluções vizinhas.
            for (i = 1; i < numVertices; i++)
            {
                melhorDiferencaDistancia = 0;
                for (j = i + 1; j < numVertices; j++)
                {
                    diferencaDistancia = diferencaTroca2Opt(melhorCaminho, i, j);
                    // Se a troca resulta em uma redução da distância
                    if (diferencaDistancia < melhorDiferencaDistancia)
                    {
                        melhor_i = i;
                        melhor_j = j;
                        melhorDiferencaDistancia = diferencaDistancia;
                    }
                }
                // Se existe uma solução vizinha que resulta em redução da distância
                if (melhorDiferencaDistancia < 0)
                {
                    trocar2Opt(melhorCaminho, melhor_i, melhor_j);
                    melhorDistancia += melhorDiferencaDistancia;
                    contadorSemMelhora = 0;
                }
            }
            iter++;
        }

        registraIteracao(arq, tempoInicial, iter, melhorDistancia);
        arq.close();

        return melhorDistancia;
    }
};

// Separa a string em uma lista de tokens
// Tokens são trechos da string original separados por um espaço
vector<string> tokenizar(string str)
{
    vector<string> listaTokens;
    int i;
    string token = "";
    for (i = 0; i < (int)str.length(); i++)
    {
        if (str[i] == ' ')
        {
            if (!token.empty())
            {
                listaTokens.push_back(token);
                token = "";
            }
        }
        else
        {
            token = token + str[i];
        }
    }
    if (!token.empty())
    {
        listaTokens.push_back(token);
    }

    return listaTokens;
}

// Inicializa a lista de vértices utilizando linhas de entrada
// da linha de comando.
// Assume que a entrada está formatada de acordo com os exemplos.
// Inicializa a posição 0 da linha de vértices com um vértice vazio.
void inicializarPorTerminal(vector<Vertice> &listaVertices, int &tamanhoLista)
{
    string linhaEntrada;
    vector<string> listaTokens;

    for (int i = 0; i < 6; i++)
    {
        getline(cin, linhaEntrada);
    }

    // Declaração dos vértices
    // Primeiro vértice é vazio
    listaVertices.push_back(Vertice());
    tamanhoLista = 0;
    getline(cin, linhaEntrada);
    // Enquanto não achar um "EOF"
    while (linhaEntrada.find("EOF") == string::npos)
    {
        listaTokens = tokenizar(linhaEntrada);
        listaVertices.push_back(Vertice(stof(listaTokens[1]), stof(listaTokens[2])));
        tamanhoLista++;
        getline(cin, linhaEntrada);
    }
}

// Inicializa a lista de vértices utilizando um arquivo
// Assume que a entrada está formatada de acordo com os exemplos.
// Inicializa a posição 0 da linha de vértices com um vértice vazio.
// Retorna true caso o arquivo exista, false caso contrário
bool inicializarPorArquivo(string nomeArquivo, vector<Vertice> &listaVertices, int &tamanhoLista)
{
    ifstream arq(nomeArquivo);

    if (!arq.good())
        return false;

    string linhaEntrada;
    vector<string> listaTokens;

    for (int i = 0; i < 6; i++)
    {
        getline(arq, linhaEntrada);
    }

    // Declaração dos vértices
    // Primeiro vértice é vazio
    listaVertices.push_back(Vertice());
    tamanhoLista = 0;
    // Enquanto não for o fim do arquivo e não achar um "EOF"
    while (getline(arq, linhaEntrada) && linhaEntrada.find("EOF") == string::npos)
    {
        listaTokens = tokenizar(linhaEntrada);
        listaVertices.push_back(Vertice(stof(listaTokens[1]), stof(listaTokens[2])));
        tamanhoLista++;
    }

    return true;
}

int main(int argc, char **argv)
{
    vector<Vertice> listaVertices;
    int tamanhoLista;

    if (argc > 1)
    {
        if (inicializarPorArquivo(argv[1], listaVertices, tamanhoLista))
        {
            // cout << "Inicialização completa" << endl;
        }
        else
        {
            // cout << "Erro na leitura do arquivo" << endl;
            return 0;
        }
    }
    else
    {
        // cout << "Insira o caso de teste" << endl;
        inicializarPorTerminal(listaVertices, tamanhoLista);
    }

    PCVSolver pcvSolver(listaVertices, tamanhoLista);

    float resultado;
    // clock_t tempoInicial = clock();
    // cout << "Executando..." << endl;

    resultado = pcvSolver.solve2OptBestImprovement();

    // clock_t tempoFinal = clock();
    // float tempoTotal = (tempoFinal - tempoInicial) / (float)CLOCKS_PER_SEC;
    // cout << "Execução finalizada" << endl;
    // cout << "Tempo: " << tempoTotal << "s" << endl;
    // cout << "Resultado: ";
    cout << resultado << "\n";

    return 0;
}
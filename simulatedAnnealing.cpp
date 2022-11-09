#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <vector>
#include <omp.h>
#include <deque>
#include <cmath>
#include <ctime>
#include <limits>

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


// Função de Teste
// Imprime todos os vértices do caminho
void printCaminho(vector<int> &caminho)
{
    for (int i = 0; i < (int)caminho.size(); i++)
    {
        cout << caminho[i] << " => ";
    }
    cout << caminho[0] << endl;
}

// Função de Teste
// Imprime as coordenadas de todos os vértices na tela
void printVertices(vector<Vertice> vertices, int numVertices)
{
    for (int i = 0; i < numVertices; i++)
    {
        cout << i << " => x: " << vertices[i].x << " y: " << vertices[i].y << "\n";
    }
    cout << endl;
}

// Retorna a distância entre os vértices v1 e v2.
float distancia(Vertice v1, Vertice v2)
{
    float deltaX = v2.x - v1.x;
    float deltaY = v2.y - v1.y;

    return sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

// Retorna a distância percorrida pelo caminho inteiro.
double distanciaCaminho(vector<Vertice> &vertices, vector<int> &caminho)
{
    int i;
    double distanciaTotal = 0;
    for (i = 0; i < (int)caminho.size() - 1; i++)
    {
        distanciaTotal += distancia(vertices[caminho[i]], vertices[caminho[i + 1]]);
    }
    distanciaTotal += distancia(vertices[caminho[i]], vertices[caminho[0]]);
    return distanciaTotal;
}

// Gera um caminho inicial, começando no vértice 1 e passando sequencialmente
// pelos vértices 2, 3, 4, ..., até o último.
vector<int> caminhoInicialSequencial(int numVertices)
{
    vector<int> caminho(numVertices);
    iota(begin(caminho), end(caminho), 0);

    return caminho;
}

// Gera um caminho inicial aleatório que passa por todos os vértices
// uma única vez.
vector<int> caminhoInicialAleatorio(int numVertices)
{
    vector<int> caminho(numVertices);
    iota(begin(caminho), end(caminho), 0);
    random_shuffle(caminho.begin(), caminho.end());
    return caminho;
}

// Calcula a diferença entre as distâncias do caminho antes
// e depois da operação de troca 2-opt entre os vértices
// (caminho[i-1], caminho[i]) e (caminho[j], caminho[j+1]).
// Caso a diferença seja negativa, o caminho em que a troca foi
// realizada possui distância menor.
// Presume que i < j.
float diferencaTroca2Opt(vector<Vertice> &vertices, vector<int> &caminho, int i, int j, int numVertices)
{
    int iMenos = (i - 1) % numVertices;
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
// Presume que i < j.
void trocar2Opt(vector<int> &caminho, int i, int j)
{
    while (i < j)
    {
        swap(caminho[i], caminho[j]);
        i++;
        j--;
    }
}

// Aplica o algoritmo simulated annealing para o PCV.
// Retorna o caminho obtido na última iteração
double simulatedAnnealing2Opt(vector<Vertice> &vertices, vector<int> &caminho, float &t, float taxaEsfriamento, int maxIteracoes)
{
    unsigned long int iter;
    float randFloat;
    int i, j;
    int numVertices = caminho.size();
    double custoTroca, melhorDistancia = distanciaCaminho(vertices, caminho);

    for (iter = 0; iter < maxIteracoes; iter++)
    {
        i = (rand() % (numVertices - 1)) + 1;
        j = i + (rand() % (numVertices - i));
        custoTroca = diferencaTroca2Opt(vertices, caminho, i, j, numVertices);
        // Float aleatório entre 0 e 1
        randFloat = ((float)rand() / (RAND_MAX));
        if ((custoTroca > 0) || (exp(custoTroca / t) > randFloat))
        {
            melhorDistancia += custoTroca;
            trocar2Opt(caminho, i, j);
            // cout << "dist geracao " << iter << ": " << melhorDistancia << endl;
            // cout << "    dist de vdd: " << distanciaCaminho(caminho) << endl;
            // cout << "    exp: " << (exp(custoTroca / t)) << " ; rand: " << randFloat << endl;
            // cout << "    custo troca: " << custoTroca << endl;

        }
    }
    t *= taxaEsfriamento;
    // cout << "t final: " << t << endl;
    // cout << "iter: " << iter << "/" << maxIteracoes << endl;
    // cout << "dist: " << melhorDistancia << endl;
    // cout << "dist de vdd: " << distanciaCaminho(caminho) << endl;
    return melhorDistancia;
}

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
void inicializar(vector<Vertice> &listaVertices, int &numVertices)
{
    string linhaEntrada;
    vector<string> listaTokens;

    for (int i = 0; i < 6; i++)
    {
        getline(cin, linhaEntrada);
    }

    // Declaração dos vértices
    numVertices = 0;
    getline(cin, linhaEntrada);
    // Enquanto não achar um "EOF"
    while (linhaEntrada.find("EOF") == string::npos)
    {
        listaTokens = tokenizar(linhaEntrada);
        listaVertices.push_back(Vertice(stof(listaTokens[1]), stof(listaTokens[2])));
        numVertices++;
        getline(cin, linhaEntrada);
    }
}

// Inicializa a lista de vértices utilizando um arquivo
// Presume que a entrada está formatada de acordo com os exemplos.
// Retorna true caso o arquivo exista, false caso contrário
bool inicializarPorArquivo(string nomeArquivo, vector<Vertice> &listaVertices, int &numVertices)
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
    numVertices = 0;
    // Enquanto não for o fim do arquivo e não achar um "EOF"
    while (getline(arq, linhaEntrada) && linhaEntrada.find("EOF") == string::npos)
    {
        listaTokens = tokenizar(linhaEntrada);
        listaVertices.push_back(Vertice(stof(listaTokens[1]), stof(listaTokens[2])));
        numVertices++;
    }

    return true;
}

double simulatedAnnealingParalelo(vector<Vertice> listaVertices, int numVertices, int numThreads){
    omp_set_num_threads(numThreads);
    int iteracoes = 0, p = 100;
    float t = 100, taxaEsfriamento = 0.99;
    double melhorDistancia = 0;
    int contSemMelhoria = 0, maxSemMelhoria = 10;
    vector<int> melhorCaminho;

    // primeira iteração em paralelo
    // gera soluções iniciais aleatórias e executa o simulated annealing,
    // armazenando apenas a melhor solução
    #pragma omp parallel for
    for(int i=0; i<numThreads; i++){
        vector<int> caminho = caminhoInicialAleatorio(numVertices);
        float tThread = t;
        simulatedAnnealing2Opt(listaVertices, caminho, tThread, taxaEsfriamento, p);
        double distancia = distanciaCaminho(listaVertices, caminho);
        printf("\tresultado = %f; from thread = %d\n", distancia, omp_get_thread_num());
        #pragma omp critical
        {
            if(distancia > melhorDistancia) {
                melhorDistancia = distancia;
                melhorCaminho = caminho;
            }
        }
    }
    printf("\t\tMelhor resultado = %f\n", melhorDistancia);
    
    while(contSemMelhoria < maxSemMelhoria){
        cout << "iter: " << iteracoes << " / t: " << t << endl;
        int melhorThread;
        // faz p iterações do simulated annealing em paralelo
        #pragma omp parallel for
        for(int i=0; i<numThreads; i++){
            vector<int> caminho = melhorCaminho;
            float tThread = t;
            double distancia = simulatedAnnealing2Opt(listaVertices, caminho, tThread, taxaEsfriamento, p);
            printf("\tresultado = %f; from thread = %d\n", distancia, omp_get_thread_num());
            
            #pragma omp critical
            {
                if(distancia > melhorDistancia) {
                    melhorDistancia = distancia;
                    melhorCaminho = caminho;
                    melhorThread = i;
                    contSemMelhoria = -1;
                }
            }

        }
        printf("\t\tMelhor resultado = %f -> thread = %d\n", melhorDistancia, melhorThread);

        t *= taxaEsfriamento;
        iteracoes+=p;
        contSemMelhoria++;
    }

    return melhorDistancia;
}

int main()
{
    vector<Vertice> listaVertices;
    int numVertices;
    // inicializar(listaVertices, numVertices);
    string nomeArq = "casosTeste/d18512.tsp";
    inicializarPorArquivo(nomeArq, listaVertices, numVertices);
    double resultado;
    int numThreads = 4;

    srand(time(NULL));
    double start = omp_get_wtime(); 
    resultado = simulatedAnnealingParalelo(listaVertices, numVertices, numThreads);
    double end = omp_get_wtime();
    float tempoTotal = end-start;

    cout << "Tempo de Execução: " << tempoTotal << "s" << endl;
    cout << "Resultado Final: " << resultado << "\n";

    return 0;
}
#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <ctime>
#include <limits>
#include <thread>
#include <mpi.h>
#include <random>
#include <float.h>

using namespace std;

random_device dev;
mt19937 rng(dev());
uniform_real_distribution<double> dist_real(0.0, 1.0);

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
double distanciaCaminho(vector<Vertice> &vertices, int caminho[])
{
    int i;
    double distanciaTotal = 0;
    for (i = 0; i < (int)vertices.size() - 1; i++)
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
    shuffle(caminho.begin(), caminho.end(), rng);
    return caminho;
}

// Calcula a diferença entre as distâncias do caminho antes
// e depois da operação de troca 2-opt entre os vértices
// (caminho[i-1], caminho[i]) e (caminho[j], caminho[j+1]).
// Caso a diferença seja negativa, o caminho em que a troca foi
// realizada possui distância menor.
// Presume que i < j.
float diferencaTroca2Opt(vector<Vertice> &vertices, int caminho[], int i, int j, int numVertices)
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
void trocar2Opt(int caminho[], int i, int j)
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
double simulatedAnnealing2Opt(vector<Vertice> &vertices, int caminho[], float &t, float taxaEsfriamento, int maxIteracoes)
{
    unsigned long int iter;
    float randFloat;
    int i, j;
    int numVertices = vertices.size();
    double custoTroca, melhorDistancia = distanciaCaminho(vertices, caminho);

    for (iter = 0; iter < maxIteracoes; iter++)
    {
        i = (rand() % (numVertices - 1)) + 1;
        j = i + (rand() % (numVertices - i));
        custoTroca = diferencaTroca2Opt(vertices, caminho, i, j, numVertices);
        // Float aleatório entre 0 e 1
        randFloat = dist_real(rng);
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

double main_thread(int numThreads, vector<Vertice> listaVertices, int numVertices) {
    int ierr;
    MPI_Status status;
    double info[3];

    int iteracoes = 0;
    double t = 100, taxaEsfriamento = 0.99, p = 100;
    double melhorDistancia = 0;
    int contSemMelhoria = 0, maxSemMelhoria = 10;
    int melhor_thread = 0;
    int caminho[numVertices];

    info[0] = t;
    info[1] = taxaEsfriamento;
    info[2] = p;
    double thread_distance;

    // primeira iteração em paralelo
    // envia os valores de temperatura para gerar os caminhos iniciais
    for(int i = 1;i < numThreads;i++) ierr = MPI_Send (info, 3, MPI_DOUBLE, i, 55, MPI_COMM_WORLD );

    // recebe os dados iniciais das threads
    for(int i = 1;i < numThreads;i++) {
        ierr = MPI_Recv (&thread_distance, 1, MPI_DOUBLE, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        cout << "Received Distance:" << thread_distance << " from " << i << "\n";

        if(thread_distance > melhorDistancia) {
            melhorDistancia = thread_distance;
            melhor_thread = i;
        }
    }

    cout << "\t\tMelhor resultado = " << melhor_thread << " total = " << melhorDistancia << endl; 

    // request best thread path
    for(int i = 1;i < numThreads;i++) ierr = MPI_Send(&melhor_thread, 1, MPI_INT, i, i, MPI_COMM_WORLD);
    
    // receive best path
    ierr = MPI_Recv(caminho, numVertices, MPI_INT, melhor_thread, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    // send best path
    for(int i = 1;i < numThreads;i++) ierr = MPI_Send(caminho, numVertices, MPI_INT, i, i, MPI_COMM_WORLD);

    // for(int i = 0;i < numVertices;i++) {
    //     cout << caminho[i] << " ";
    // }
    // cout << endl;
    
    while(contSemMelhoria < maxSemMelhoria) {
        cout << "Iter: " << iteracoes << endl;
        info[0] = t;
        info[1] = taxaEsfriamento;
        info[2] = p;
        // send temperature info
        for(int i = 1;i < numThreads;i++) ierr = MPI_Send (info, 3, MPI_DOUBLE, i, i, MPI_COMM_WORLD );

        // receive threads distances
        for(int i = 1;i < numThreads;i++) {
            ierr = MPI_Recv (&thread_distance, 1, MPI_DOUBLE, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            cout << "Received Distance:" << thread_distance << " from " << i << "\n";

            if(thread_distance > melhorDistancia) {
                melhorDistancia = thread_distance;
                melhor_thread = i;
                contSemMelhoria = -1;
            }
        }

        cout << "\t\tMelhor resultado = " << melhor_thread << " total = " << melhorDistancia << endl; 

        // request best thread path
        for(int i = 1;i < numThreads;i++) ierr = MPI_Send(&melhor_thread, 1, MPI_INT, i, i, MPI_COMM_WORLD);
        
        // receive best path
        ierr = MPI_Recv(caminho, numVertices, MPI_INT, melhor_thread, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // send best path
        for(int i = 1;i < numThreads;i++) ierr = MPI_Send(caminho, numVertices, MPI_INT, i, i, MPI_COMM_WORLD);

        t *= taxaEsfriamento;
        iteracoes += p;
        contSemMelhoria++;
        
        // send stop criterea
        for(int i = 1;i < numThreads;i++) ierr = MPI_Send(&contSemMelhoria, 1, MPI_INT, i, i, MPI_COMM_WORLD);
    }

    return melhorDistancia;
}

void slave_thread(vector<Vertice> listaVertices, int numVertices) {
    int ierr, count, rank, num_procs;
    MPI_Status status;

    double info[3];

    ierr = MPI_Comm_rank ( MPI_COMM_WORLD, &rank );

    ierr = MPI_Comm_size ( MPI_COMM_WORLD, &num_procs );

    ierr = MPI_Recv (info, 3, MPI_DOUBLE, 0, 55, MPI_COMM_WORLD, &status);

    float t = info[0], taxaEsfriamento = info[1];
    int p = info[2];

    vector<int> caminhoInicial = caminhoInicialAleatorio(numVertices);
    double distancia = simulatedAnnealing2Opt(listaVertices, &caminhoInicial[0], t, taxaEsfriamento, p);
    // printf("\tresultado = %f from thread = %d\n", distancia, rank);
    
    // send distance
    ierr = MPI_Send (&distancia, 1, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);

    // await best thread reply
    int thread = -1;
    ierr = MPI_Recv (&thread, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    // send path if best
    if(thread == rank) ierr = MPI_Send(&caminhoInicial[0], numVertices, MPI_INT, 0, rank, MPI_COMM_WORLD);

    // receive best path
    int caminho[numVertices];
    ierr = MPI_Recv (caminho, numVertices, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    
    int contSemMelhoria = 0, maxSemMelhoria = 10;

    while(contSemMelhoria < maxSemMelhoria) {
        // receive temperature info
        ierr = MPI_Recv (info, 3, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD, &status);

        t = info[0], taxaEsfriamento = info[1];
        p = info[2];

        // exec simulated annealing
        distancia = simulatedAnnealing2Opt(listaVertices, caminho, t, taxaEsfriamento, p);

        // send distance
        ierr = MPI_Send (&distancia, 1, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);

        // await best thread reply
        ierr = MPI_Recv (&thread, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // send path if best
        if(thread == rank) ierr = MPI_Send(&caminhoInicial[0], numVertices, MPI_INT, 0, rank, MPI_COMM_WORLD);

        // receive best path
        int caminho[numVertices];
        ierr = MPI_Recv (caminho, numVertices, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // receive stop criterea
        ierr = MPI_Recv (&contSemMelhoria, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }
}

int main(int argc, char *argv[] ) {
    vector<Vertice> listaVertices;
    int numVertices;
    
    string nomeArq = "casosTeste/pr1002.tsp";
    inicializarPorArquivo(nomeArq, listaVertices, numVertices);
    double resultado;

    srand(time(NULL));
    double start = time(NULL);

    int rank, num_procs, ierr;

    ierr = MPI_Init ( &argc, &argv );

    ierr = MPI_Comm_rank ( MPI_COMM_WORLD, &rank );

    ierr = MPI_Comm_size ( MPI_COMM_WORLD, &num_procs );

    if(rank == 0) {
        double resultado = main_thread(num_procs, listaVertices, numVertices);

        double end = time(NULL);
        float tempoTotal = end-start;

        cout << "Tempo de Execução: " << tempoTotal << "s" << endl;
        cout << "Resultado Final: " << fixed << resultado << "\n";
    } else {
        slave_thread(listaVertices, numVertices);
    }
    
    cout << "rank = " << rank << " ended" << endl;
    MPI_Finalize ( );

    return 0;
}
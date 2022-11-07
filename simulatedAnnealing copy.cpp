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

class PCVSolver
{
private:
    vector<Vertice> vertices;
    int numVertices;
    vector<int> melhorCaminho;
    double melhorDistancia;
    float t, taxaEsfriamento;


public:
    // Inicializa a classe com a lista e a quantidade de vértices
    PCVSolver(vector<Vertice> listaVertices, int n, bool random, float tInicial, float taxa)
    {
        vertices = listaVertices;
        numVertices = n;
        random ? melhorCaminho = caminhoInicialAleatorio() : melhorCaminho = caminhoInicialSequencial();
        melhorDistancia = distanciaCaminho(melhorCaminho);
        t = tInicial;
        taxaEsfriamento = taxa;
    }

    PCVSolver(){}

    void setMelhorCaminho(vector<int> &caminho){
        melhorCaminho = caminho;
    }

    vector<int> getMelhorCaminho(){
        return melhorCaminho;
    }

    void setMelhorDistancia(double distancia){
        melhorDistancia = distancia;
    }

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
    void printVertices()
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

    // Gera um caminho inicial, começando no vértice 1 e passando sequencialmente
    // pelos vértices 2, 3, 4, ..., até o último.
    vector<int> caminhoInicialSequencial()
    {
        vector<int> caminho(numVertices);
        iota(begin(caminho), end(caminho), 0);

        return caminho;
    }

    // Gera um caminho inicial aleatório que passa por todos os vértices
    // uma única vez.
    vector<int> caminhoInicialAleatorio()
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
    float diferencaTroca2Opt(vector<int> &caminho, int i, int j)
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
    // A cada iteração realiza uma troca 2-opt no ciclo.
    float solveSimulatedAnnealing2Opt(int maxIteracoes)
    {
        unsigned long int iter;
        float custoTroca, randFloat;
        int i, j;

        for (iter = 0; iter < maxIteracoes; iter++)
        {
            i = (rand() % (numVertices - 1)) + 1;
            j = i + (rand() % (numVertices - i));
            custoTroca = diferencaTroca2Opt(melhorCaminho, i, j);
            // Float aleatório entre 0 e 1
            randFloat = ((float)rand() / (RAND_MAX));
            if ((custoTroca > 0) || (exp(custoTroca / t) > randFloat))
            {
                melhorDistancia += custoTroca;
                trocar2Opt(melhorCaminho, i, j);
                // cout << "dist geracao " << iter << ": " << melhorDistancia << endl;
                // cout << "   dist de vdd: " << distanciaCaminho(melhorCaminho) << endl;
                // cout << "    exp: " << (exp(custoTroca / t)) << " ; rand: " << randFloat << endl;
                // cout << "    custo troca: " << custoTroca << endl;

            }
        }
        t *= taxaEsfriamento;
        // cout << "t final: " << t << endl;
        // cout << "iter: " << iter << "/" << maxIteracoes << endl;
        // cout << "dist: " << melhorDistancia << endl;
        // cout << "dist de vdd: " << distanciaCaminho(melhorCaminho) << endl;
        return melhorDistancia;
    }
};

// Separa a string em uma lista de tokens
// Tokens são trechos da string original separados por um espaço
vector<string> tokenizar(string str)
{
    vector<string> listaTokens;
    string token;
    string del = " ";
    int start, end = -1*del.size();
    do {
        start = end + del.size();
        end = str.find(del, start);
        token = str.substr(start, end - start);
        listaTokens.push_back(token);
    } while (end != -1);

    return listaTokens;
}

// Inicializa a lista de vértices utilizando linhas de entrada
// da linha de comando.
// Assume que a entrada está formatada de acordo com os exemplos.
void inicializar(vector<Vertice> &listaVertices, int &tamanhoLista)
{
    string linhaEntrada;
    vector<string> listaTokens;

    for (int i = 0; i < 6; i++)
    {
        getline(cin, linhaEntrada);
    }

    // Declaração dos vértices
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
// Presume que a entrada está formatada de acordo com os exemplos.
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

double simulatedAnnealingParalelo(vector<Vertice> listaVertices, int tamanhoLista, int numThreads){
    omp_set_num_threads(numThreads);

    vector<PCVSolver> pcvSolvers(numThreads);
    vector<double> resultados(numThreads, 0.0);
    int iteracoes = 10000, p = 100;
    float tInicial = 100, taxaEsfriamento = 0.98;
    float melhorResultado;

    // inicializar em paralelo
    #pragma omp parallel for
    for(int i=0; i<numThreads; i++){
        pcvSolvers[i] = PCVSolver(listaVertices, tamanhoLista, true, tInicial, taxaEsfriamento);
    }

    while(iteracoes>0){
        // faz p iterações do simulated annealing em paralelo
        #pragma omp parallel for
        for(int i=0; i<numThreads; i++){
            resultados[i] = pcvSolvers[i].solveSimulatedAnnealing2Opt(p);
            printf("    resultado = %f; from thread = %d\n", resultados[i],
                omp_get_thread_num());
        }
        // pega o indice do maior resultado
        int max = distance(resultados.begin(),max_element(resultados.begin(), resultados.end()));
        vector<int> melhorCaminho = pcvSolvers[max].getMelhorCaminho();
        melhorResultado = resultados[max];
        // atualiza todas as intâncias com o melhor resultado
        #pragma omp parallel for
        for(int i=0; i<numThreads; i++){
            pcvSolvers[i].setMelhorCaminho(melhorCaminho);
            pcvSolvers[i].setMelhorDistancia(melhorResultado);

        }
        printf("Melhor resultado = %f -> thread = %d\n", melhorResultado, max);

        iteracoes-=p;
    }

    return melhorResultado;
}

int main()
{
    vector<Vertice> listaVertices;
    int tamanhoLista;
    // inicializar(listaVertices, tamanhoLista);
    string nomeArq = "casosTeste/att48.txt";
    inicializarPorArquivo(nomeArq, listaVertices, tamanhoLista);
    double resultado;
    int numThreads = 4;

    srand(time(NULL));
    clock_t tempoInicial = clock();
    resultado = simulatedAnnealingParalelo(listaVertices, tamanhoLista, numThreads);
    clock_t tempoFinal = clock();
    float tempoTotal = (tempoFinal - tempoInicial) / (float)CLOCKS_PER_SEC;

    cout << "Tempo de Execução: " << tempoTotal << "s" << endl;
    cout << "Resultado Final: " << resultado << "\n";

    return 0;
}
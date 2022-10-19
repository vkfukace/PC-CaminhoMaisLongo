#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <ctime>
#include <limits>

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

    // Gera um caminho inicial, começando no vértice 1 e passando sequencialmente
    // pelos vértices 2, 3, 4, ..., até o último.
    // Retorna a distância total do caminho.
    vector<int> gerarCaminhoInicialSequencial()
    {
        vector<int> caminho;
        int i;
        for (i = 1; i <= numVertices; i++)
        {
            caminho.push_back(i);
        }
        return caminho;
    }

    // Imprime as coordenadas de todos os vértices na tela
    void printVertices()
    {
        for (int j = 1; j < numVertices + 1; j++)
        {
            cout << j << " => x: " << vertices[j].x << " y: " << vertices[j].y << "\n";
        }
    }

    // Calcula a diferença entre as distâncias do caminho antes
    // e depois da operação de troca entre os vértices caminho[i] e caminho[j].
    // Caso a diferença seja negativa, o caminho em que a troca foi
    // realizada possui distância menor.
    float diferencaTrocaVertices(vector<int> &caminho, int i, int j){
        int iMenos1 = (numVertices + ((i - 1) % numVertices)) % numVertices;
        int iMais1 = (i + 1) % numVertices;
        int jMenos1 = (numVertices + ((j - 1) % numVertices)) % numVertices;
        int jMais1 = (j + 1) % numVertices;

        int v_i = caminho[i], v_iMenos1 = caminho[iMenos1], v_iMais1 = caminho[iMais1];
        int v_j = caminho[j], v_jMenos1 = caminho[jMenos1], v_jMais1 = caminho[jMais1];

        if(iMais1 == j){
            // Distâncias antes da troca
            float dAntesI = distancia(vertices[v_iMenos1], vertices[v_i]);
            float dAntesJ = distancia(vertices[v_j], vertices[v_jMais1]);
            // Distâncias depois da troca
            float dDepoisI = distancia(vertices[v_iMenos1], vertices[v_j]);
            float dDepoisJ = distancia(vertices[v_i], vertices[v_jMais1]);
            return (dDepoisI  + dDepoisJ) - (dAntesI  + dAntesJ);
        } else if (jMais1 == i){
            // Distâncias antes da troca
            float dAntesI = distancia(vertices[v_i], vertices[v_iMais1]);
            float dAntesJ = distancia(vertices[v_jMenos1], vertices[v_j]);
            // Distâncias depois da troca
            float dDepoisI = distancia(vertices[v_j], vertices[v_iMais1]);
            float dDepoisJ = distancia(vertices[v_jMenos1], vertices[v_i]); 
            return (dDepoisI + dDepoisJ) - (dAntesI + dAntesJ);
        } else {
            // Distâncias antes da troca
            float dAntesI = distancia(vertices[v_iMenos1], vertices[v_i]) + distancia(vertices[v_i], vertices[v_iMais1]);
            float dAntesJ = distancia(vertices[v_jMenos1], vertices[v_j]) + distancia(vertices[v_j], vertices[v_jMais1]);
            // Distâncias depois da troca
            float dDepoisI = distancia(vertices[v_iMenos1], vertices[v_j]) + distancia(vertices[v_j], vertices[v_iMais1]);
            float dDepoisJ = distancia(vertices[v_jMenos1], vertices[v_i]) + distancia(vertices[v_i], vertices[v_jMais1]);
            return (dDepoisI + dDepoisJ) - (dAntesI + dAntesJ);
        }

    }

    // Troca os vértices em caminho[i] e caminho[j]
    void trocarVertices(vector<int> &caminho, int i, int j)
    {
        int aux = caminho[i];
        caminho[i] = caminho[j];
        caminho[j] = aux;
    }

    // Aplica o algoritmo simulated annealing para o PCV.
    // A cada iteração realiza uma troca entre dois vértices do ciclo.
    float solveSimulatedAnnealingTrocaSimples(){
        melhorCaminho = gerarCaminhoInicialSequencial();
        melhorDistancia = distanciaCaminho(melhorCaminho);
        // K é constante que multiplica o número de iterações máximas
        int k = 1;
        unsigned long int iter,  maxIteracoes = (k * pow(numVertices, 2));
        float tInicial = 500.0, tFinal = 0.001, taxaEsfriamento = 0.9999;
        float t = tInicial, custoTroca, randFloat;
        int i, j;
        srand(time(NULL));

        for (iter = 0; iter < maxIteracoes && t > tFinal; iter++) {
            i = rand() % numVertices;
            j = rand() % numVertices;
            custoTroca = diferencaTrocaVertices(melhorCaminho, i, j);
            // Float aleatório entre 0 e 1
            randFloat = ((float) rand() / (RAND_MAX));
            if ((custoTroca < 0) || (exp(-custoTroca/t) > randFloat)){
                melhorDistancia += custoTroca;
                trocarVertices(melhorCaminho, i, j);
            t *= taxaEsfriamento;
            } 
        }
        return melhorDistancia;
    }
    
    // Calcula a diferença entre as distâncias do caminho antes
    // e depois da operação de troca 2-opt entre os vértices
    // (caminho[i-1], caminho[i]) e (caminho[j], caminho[j+1]).
    // Caso a diferença seja negativa, o caminho em que a troca foi
    // realizada possui distância menor.
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

    // Aplica o algoritmo simulated annealing para o PCV.
    // A cada iteração realiza uma troca 2-opt no ciclo.
    float solveSimulatedAnnealing2Opt(){
        melhorCaminho = gerarCaminhoInicialSequencial(); // ################### USAR NEAREST
        melhorDistancia = distanciaCaminho(melhorCaminho);
        // K é constante que multiplica o número de iterações máximas
        int k = 1;
        unsigned long int iter,  maxIteracoes = (k * pow(numVertices, 2));
        float tInicial = 500.0, tFinal = 0.001, taxaEsfriamento = 0.9999;
        float t = tInicial, custoTroca, randFloat;
        int i, j;
        srand(time(NULL));

        for (iter = 0; iter < maxIteracoes && t > tFinal; iter++) {
            i = rand() % numVertices;
            j = i + (rand() % (numVertices - i));
            custoTroca = diferencaTroca2Opt(melhorCaminho, i, j);
            // Float aleatório entre 0 e 1
            randFloat = ((float) rand() / (RAND_MAX));
            if ((custoTroca < 0) || (exp(-custoTroca/t) > randFloat)){
                melhorDistancia += custoTroca;
                trocar2Opt(melhorCaminho, i, j);
                t *= taxaEsfriamento;
            } 
        }
        cout << "t: " << t << endl;
        cout << "iter: "<< iter << "/" << maxIteracoes << endl;
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
void inicializar(vector<Vertice> &listaVertices, int &tamanhoLista)
{
    string linhaEntrada;
    vector<string> listaTokens;

    for(int i = 0; i < 6; i++){
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

int main()
{
    vector<Vertice> listaVertices;
    int tamanhoLista;
    inicializar(listaVertices, tamanhoLista);
    PCVSolver pcvSolver(listaVertices, tamanhoLista);

    float resultado;
    clock_t tempoInicial = clock();

    resultado = pcvSolver.solveSimulatedAnnealing2Opt();
    
    clock_t tempoFinal = clock();
    float tempoTotal = (tempoFinal-tempoInicial)/(float)CLOCKS_PER_SEC;

    cout << tempoTotal << "s" << endl;
    cout << resultado << "\n";

    return 0;
}
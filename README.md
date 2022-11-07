# MOA-AlgoritmosHeuristicos

## Modelagem e Otimização Algorítmica - CC UEM 2022

Implementação de dois algoritmos heurísticos de busca e dois algoritmos de construção para o Problema do Caixeiro Viajante.

### Execução

Para executar, existem 2 modos de entrada: por arquivo e por terminal.

Para realizar entrada pelo terminal, digite o comando "./\<nomeDoAlgoritmo\>" no terminal, onde \<nomeDoAlgoritmo\> pode ser um dos seguintes valores:

- nearestNeighbor
- randomInsertion
- 2OptFirstImprovement
- 2OptBestImprovement
  Ex: ./randomInsertion

A seguir, copie e cole o caso de teste no terminal.

Para realizar entrada por arquivo, passe o nome do arquivo de entrada como parâmetro.

Ex1: "./\<nomeDoAlgoritmo\> \<nomeDoArquivo\>"
Ex2: ./2OptBestImprovement pla7397.tsp

Presume-se que os casos de teste seguem o formato descrito em http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp95.pdf, com a última linha contendo "EOF".

O tempo de execução dependende do tamanho do caso de teste.

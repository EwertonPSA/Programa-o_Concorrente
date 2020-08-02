# SSC0143 Programação Concorrente 

## Detalhes sobre o projeto
Desenvolvimento de trabalho de programação concorrente em MPI com linguagem c utilizando clusters. Esse trabalho busca aplicar os conceitos teóricos aprendidos durante a matéria, como por exemplo particionar as tarefas computacionais a serem realizadas para agilizar o processamento das informações. A [especificação do trabalho](https://github.com/EwertonPSA/Programacao_Concorrente/blob/master/Especifica%C3%A7%C3%A3o%20do%20trabalho.pdf) apresenta o problema a ser solucionado.

Para perceber as diferenças no tempo de processamento das informações, foi feito um código para computar as informaçes em apenas uma maquina(chamado [studentsseq.c](https://github.com/EwertonPSA/Programacao_Concorrente/blob/master/studentsseq.c)) e um para computar em diferentes maquinas no cluster(chamado [studentspar.c](https://github.com/EwertonPSA/Programacao_Concorrente/blob/master/studentspar.c)). O arquivo [my_hostfile](https://github.com/EwertonPSA/Programacao_Concorrente/blob/master/my_hostfile) informa as maquinas disponiveis no cluster e a quantidade de nós que será usada em cada maquina para o processamento em paralelo.

## Desenvolvimento do projeto
Este projeto foi separado em 2 partes: 

1) Compreender o projeto e elaborar um [documento (PCAM.pdf)](https://github.com/EwertonPSA/Programacao_Concorrente/blob/master/pcam.pdf), descrevendo como as tarefas serão particionadas entre as maquinas disponíveis no cluster, a fim de solucionar o problema.

2) Implementar a solução em linguagem c utilizando biblioteca MPI.

Uma das motivações desse projeto foi diminuir ao máximo o tempo de processamento em paralelo para obtenção do resultado. O projeto foi finalizado com sucesso, tendo um dos menores tempos de processamento da turma e uma boa documentação.

#SSC0143 Programação Concorrente 

Desenvolvimento de trabalho de programação concorrente em MPI com linguagem c utilizando clusters. Esse trabalho busca aplicar os conceitos teóricos aprendidos durante a matéria, como por exemplo particionar as tarefas computacionais a serem realizadas para agilizar o processamento das informações. 

Para perceber as diferenças no tempo de processamento das informações, foi feito um código para computar as informaçes em apenas uma maquina(chamado studentsseq.c) e um para computar em diferentes maquinas no cluster(chamado studentspar.c). O arquivo my_hostfile informa as maquinas disponiveis no cluster e a quantidade de nós que será usada em cada maquina para o processamento em paralelo.  

Este projeto é separado em 2 partes: 

1) Compreender o projeto e elaborar um documento escrito (estabelecido como PCAM.pdf no repositório) descrevendo como as tarefas serão particionadas entre as maquinas disponíveis no cluster, a fim de solucionar o problema.

2) Implementar a solução em linguagem c utilizando biblioteca MPI.


O projeto foi finalizado com sucesso, tendo um dos menores tempos de processamento da turma e uma boa documentação.

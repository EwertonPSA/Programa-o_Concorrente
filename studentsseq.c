/*
    TRABALHO CONCORRENTES TURMA 2 GRUPO 5
        
        Ewerton Patrick Silva do Amaral   NUSP 10346975
        Leonardo Moreira Kobe             NUSP: 9778623
        Luiz Miguel di Mano Saraiva       NUSP:10425420
        Matheus Bernardes dos Santos      NUSP: 9277979
        Pedro Francisco Darela Neto       NUSP: 10295624

*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <time.h>
#include<mpi.h>

struct dados{
    int min;
    int max;
    double mediana;
    double media;
    double desvioPadrao;
};

struct melhorCidade{
    int regiao;
    int cidade;
    float media;
};

struct melhorRegiao{
    int regiao;
    float media;
};


/*
    Acreditamos que fazer uma funçao para todos os calculos ao invez de 5  fazia sentido em nosso programa pois as chamada de funçao
    podem ter um tempo consideravel pela ordem de grandeza de nossos resultados, ao testar verificamos(verificameos  media de tempo 
    para um numero de dados grande)isso entao resolvemos implementar uma funçao ao invez de 5, poderiamos fazer sem funçao, que apresentou
    os melhores resultados(pela media de tempo para um conjunto de dados de tamanho 10^8) mas o codigo nao ia ficar modular e iria dificultar a leitura e entendimento etao escolhemos
    pela segunda opcao
    Uma coisa que notamos tambem é que se o programa estiver rodando com o navegador(piora com varias abas abertas) aberto a porcentagem de ganho do paralelismo cai drasticamente
    outra coisa e que na hr do print do exemplo do professor era para dar em uma regiao media de (43.63, 48.63 e 33.63) o nosso codigo da 43.625 mas o pc por algum motivo arredonda para 43.62
    48.62 e 33.62 nao sabiamos como arrumar isso, nao é o tipo de erro que conta nesse trabalho mas decidimos deixar avisado

*/
/*
    int tamanho -> quantidade de dados daquela amostra, 
    int contador[101] -> vetor responsavel por falar a quantidade de inteiros de cada valor que existe na amostra
    struct dados *dadosAux -> onde irao ser armazenados os os dados obtidos pelos calculos, usamos o ponteiro pq 
                              ja que e somente uma funçao ficaria mais pratico mudar os valores dentro do que retornar um endereço

*/
void FcalculaDados(int tamanho, int contador[101], struct dados *dadosAux){

    double soma = 0;
    int estouro = 100; //variavel ersponsavel para nao dar estouro de memoria quando haver muitos dados(a soma de 100 milhoes pode ser 10 bilhoes)()
    for(int i = 0; i< 101; i++){
        soma += i*contador[i]/(double)estouro; // Pega a soma do vetor para usar para a media
    }

    /*****CALCULA MEDIANA*****/
    double mediana, aux;
    int j;
    if((tamanho)%2 == 0){
        aux = 0;
        j = 0;
        while(aux < (tamanho)/2){
            aux += contador[j];
            j++;
        }
        mediana = j;
        while(aux < ((tamanho)/2)+1){
            aux += contador[j];
            j++;
        }
        mediana +=j;
        mediana /= 2;
        mediana--;

    }else{
        aux = 0;
        j = 0;
        while(aux < ((tamanho)/2)+1){
            aux += contador[j];
            j++;
        }
        mediana = j;
        mediana--;
    }
    dadosAux->mediana = mediana;
    ///FIM DO CALCULO DA MEDIANA////

    /*****CALCULA VALOR MAXIMO*****/
    j = 0;
    while(contador[j] == 0){
        j++;
    }
    dadosAux->min = j;
    ///FIM DO CALCULO DO VALOR MAXIMO///

    /*****CALCULA VALOR MINIMO*****/
    j = 100;
    while(contador[j] == 0){
        j--;
    }
    dadosAux->max = j;
    //FIM DO CALCULO DO VALOR MINIMO//

    /*****CALCULA MEDIA*****/
    dadosAux->media = (((double)soma/(double)tamanho))*estouro; // lembrando da variavel estouro que acegura que nao vai estourar o valor da variavel   
    ///FIM DO CALCULO DA MEDIA///

    /*****CALCULA DESVIO PADRAO*****/
    aux = 0;
    if(tamanho > 1){       // caso o tamanho seja 1 nao existe variancia
        for(int j = 0; j < 101; j++){
            aux += contador[j]*(pow(j - (dadosAux->media), 2))/estouro;
            contador[j] = 0;
        }   
        aux /= (double)(tamanho) - 1;
        dadosAux->desvioPadrao =  sqrt(aux*estouro);
    }else{
        dadosAux->desvioPadrao = 0;
    }
    
    ///FIM DO CALCULO DO DESVIO PADRAO

}

int main(int argc, char* argv[]){
    double tempoInicio, tempoFim; // variaveis usada para verificar o tempo de execucao
    int R ;
    int C ;
    int A;
    int SEED; // nRegios, nCidades, nAlunos SEED do rand
    scanf("%d %d %d %d", &R, &C, &A, &SEED);


    //usado para pegar a quantidade de cada valor na amostra( tipo o countingSort )
    int contadorCidade[101];
    int contadorRegiao[101];
    int contadorPais[101];

    //estrutura responsavel por grardar os valores dos dados calculados(media, mediana ...etc) de cada amostra, usada para printar os valores
    struct dados* dadosCidade;
    dadosCidade = (struct dados*)malloc(sizeof(struct dados) * C * R);

    struct dados* dadosRegiao;
    dadosRegiao= (struct dados*)malloc(sizeof(struct dados) * R);
    

    //estrutura pra guardar os dados da melhor cidade e regiao;
    struct melhorCidade melhorCidade;
    struct melhorRegiao melhorRegiao;

    melhorCidade.media = 0;
    melhorRegiao.media = 0;

    srand(SEED);
    
    //ALOCA DINAMICAMENTE A MATRIZ DE DADOS/NOTAS
    int** matrizNotas;
    matrizNotas = (int**)malloc(sizeof(int*) * C*R);
    for(int i = 0; i < C*R; i++){
        matrizNotas[i] = (int*)malloc(sizeof(int) * A);
    }

    //INCIALIZA A MATRIZ COM VALORES PSEUDOALEATORIOS
    for(int i = 0; i < R*C; i++){
        for(int j = 0; j < A; j++){
            matrizNotas[i][j] = (rand() % 101);
        }
    }

    ///INCIALIZACAO DOS VETORES CONTADORES
    for(int j = 0; j < 101; j++){
        contadorPais[j] = 0;
        contadorCidade[j] = 0;
        contadorRegiao[j] = 0;
    }

    int auxRegiao;
    double soma = 0; // soma pq nso rodavamos varias vezes o programa para verificar a media de tempo

    tempoInicio = MPI_Wtime();

    for(auxRegiao = 0; auxRegiao < R; auxRegiao++){
        for(int i = auxRegiao*C ; i < (auxRegiao + 1)*C; i++){
            for(int j = 0; j < A; j++){
                contadorCidade[matrizNotas[i][j]]++;
            }   
            for(int w = 0; w < 101; w++){
                 contadorRegiao[w] += contadorCidade[w];
            }
            //funcao que calcula os dados da amostra(Cidade)
            FcalculaDados(A, contadorCidade, &dadosCidade[i]);
        }
        for(int w = 0; w < 101; w++){
            contadorPais[w] += contadorRegiao[w];
        }
        //funcao que calcula os dados da amostra(REGIAO)
        FcalculaDados(A*C, contadorRegiao, &dadosRegiao[auxRegiao]);
    }
    struct dados dadosPais;

    //funcao que calcula os dados da amostra(PAIS)
    FcalculaDados(A*C*R, contadorPais, &dadosPais);

    for(int i = 0; i< C*R; i++){
        if(melhorCidade.media < dadosCidade[i].media){
            melhorCidade.cidade = i%C;
            melhorCidade.regiao = i/C;
            melhorCidade.media = dadosCidade[i].media;
        }
    }

    for(int i = 0; i< R; i++){
        if(melhorRegiao.media < dadosRegiao[i].media){
            melhorRegiao.regiao = i;
            melhorRegiao.media = dadosRegiao[i].media;
        }
    }

    //fim da contagem do programa
    tempoFim = MPI_Wtime();
    soma += tempoFim - tempoInicio ;

    for(int i = 0; i < R; i++){
        for(int j = C*i; j < (C*(i+1)); j++){
            printf("Reg %d - Cid %d: menor:%d, maior:%d, mediana:%.2lf, media:%.2lf, DP:%.2lf\n", i, j%(C), dadosCidade[j].min, dadosCidade[j].max,
                   dadosCidade[j].mediana, dadosCidade[j].media, dadosCidade[j].desvioPadrao);
        }
        printf("\n");
    }
    for(int i = 0; i < R; i++){
        printf("Reg %d menor:%d, maior:%d, mediana:%.2lf, media:%.2lf, DP:%.2lf\n", i, dadosRegiao[i].min, dadosRegiao[i].max,
                   dadosRegiao[i].mediana, dadosRegiao[i].media, dadosRegiao[i].desvioPadrao);
    }
    printf("\n");
    printf("Brasil: menor:%d, maior:%d, mediana:%.2lf, media:%.2lf, DP:%.2lf\n", dadosPais.min, dadosPais.max,
                   dadosPais.mediana, dadosPais.media, dadosPais.desvioPadrao);
    printf("\n");
    printf("Melhor regiao: Regiao %d\n", melhorRegiao.regiao);
    printf("Melhor cidade: Regiao %d, Cidade %d\n", melhorCidade.regiao, melhorCidade.cidade);
    printf("\n");
   
    printf("Tempo de resposta sem considerar E/S, em segundos: %lfs\n ", (soma));
    for(int i = 0; i< C*R; i++){
        free(matrizNotas[i]);
    }
    free(matrizNotas);
    free(dadosCidade);
    free(dadosRegiao);
    return 0;
}

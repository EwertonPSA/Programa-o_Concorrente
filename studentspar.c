/*
    TRABALHO CONCORRENTES TURMA 2 GRUPO 5
        
w
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
	int rank, nproc;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	if(rank == 0){
		scanf("%d %d %d %d", &R, &C, &A, &SEED);
	}

	//Nao entra na computacao esses Bcast pois os demais processos
	//Necessitam dessas informacoes para alocar os resultados
	MPI_Bcast( &R, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast( &C, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast( &A, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast( &SEED, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//usado para pegar a quantidade de cada valor na amostra( tipo o countingSort )
	int contadorCidade[101];
	int contadorRegiao[101];
	int contadorPais[101];
	//O endereco do conteudo que vai receber a reducao nao pode
	//Ser o mesmo que vai ser utilizado para reduzir, 
	//Assim tivemos que criar a variavel contadorTemporarioPais,
	//O root Recebe as informacoes de todas regioes com uma reducao para computar o brasil
	int contadorTemporarioPais[101];

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
	int numRegionEachProcess = R/nproc + (nproc > R);//Acrescentado +1 caso nproc for maior que R, assim evita que tenha 0 nessa variavel
	int* matrizNotas;
	int* numDataEachProcess;
	int* displacementMatNotas;

	//Com relacao ao sequencial
	//a matriz do codigo paralelo foi transformada para um vetor
	//Para aproveitar as funcionalidades scatterv e gatherv do MPI
	matrizNotas = (int*)malloc(sizeof(int) * C*R*A);
	numDataEachProcess = (int*)malloc(sizeof(int) * nproc);
	displacementMatNotas = (int*)malloc(sizeof(int) * nproc);
	
	if(rank == 0){
		//DIVIDI AS REGIOES ENTRE OS PROCESSOS
		srand(SEED);
		int w = 0, h = 1, j = 0;

		//INCIALIZA A MATRIZ COM VALORES PSEUDOALEATORIOS
		for(int i = 0, w; i < R*C*A; i++){
			matrizNotas[i] = (rand() % 101);
		}
	}

	for(int i = 0; i < nproc; i++){
		numDataEachProcess[i] = 0;
		displacementMatNotas[i] = 0;
	}

	///INCIALIZACAO DOS VETORES CONTADORES
	for(int j = 0; j < 101; j++){
		contadorPais[j] = 0;
		contadorCidade[j] = 0;
		contadorRegiao[j] = 0;
		contadorTemporarioPais[j] = 0;
	}
	int auxRegiao;
	double soma = 0; // soma pq nso rodavamos varias vezes o programa para verificar a media de tempo
	tempoInicio = MPI_Wtime();
	if(rank == 0){//Distribuicao de carga entre os processos
		int i = 0;
		//Definindo o deslocamento dados e a quantidade de conteudo que cada processo pegara
		//Esse FOR vai ateh o penultimo processo ou ateh que o total de deslocamento
		//De dados nao ultrapasse o tamanho do vetor matNotas
		for(i = 0; i < nproc-1 && numRegionEachProcess*i*C*A < R*C*A; i++){
			numDataEachProcess[i] = numRegionEachProcess*C*A;
			displacementMatNotas[i] = (numRegionEachProcess*i)*C*A;
		}
		//No ultimo processo disponivel eh repassado o restante
		numDataEachProcess[i] = R*C*A - i*numRegionEachProcess*C*A;
		displacementMatNotas[i] = (i)*numRegionEachProcess*C*A;
	}

	//REPASSA PROS DEMAIS PROCESSOS OS DADOS, POSTERIORMENTE
	//ELES SERAO REAPROVEITADOS PRA VOLTAR PRO ROOT OS DADOSCIDADE E DADOSREGIAO 
	//COM OS RESULTADOS POR UM GATHERV, ALEM DE USAR USADO NO SCATTERV PRA PEGAR AS INFOS CORRETAS
	MPI_Bcast(numDataEachProcess, nproc, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displacementMatNotas, nproc, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(&matrizNotas[0], numDataEachProcess, displacementMatNotas, MPI_INT,
			&matrizNotas[0], numDataEachProcess[rank], MPI_INT, 0, MPI_COMM_WORLD);

	for(int k = 0; k < numDataEachProcess[rank]; k+=C*A){//PULA AS REGIOES
		for(int i = 0; i < C*A; i+=A){//PULA AS CIDADES
			for(int j = 0; j < A; j++){//PERCORRE ALUNOS
				contadorCidade[matrizNotas[k+i+j]]++;
			}

			//Reducao dos resultados da cidade para regiao
			for(int w = 0; w < 101; w++){
				contadorRegiao[w] += contadorCidade[w];
			}
			//k+i/A=posicao inicial da cidade dentro de uma regiao desconsiderando alunos
			FcalculaDados(A, contadorCidade, &dadosCidade[(k+i)/A]);
		}
		//Reducao dos resultados da regiao para pais
		for(int w = 0; w < 101; w++){
			contadorPais[w] += contadorRegiao[w];
		}
		FcalculaDados(A*C, contadorRegiao, &dadosRegiao[k/(C*A)]);	
	}
	MPI_Reduce(contadorPais, contadorTemporarioPais, 101, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	struct dados dadosPais;
	//funcao que calcula os dados da amostra(PAIS)
	FcalculaDados(A*C*R, contadorTemporarioPais, &dadosPais);
		
	//Definimos os deslocamentos e tamanho dos dados de dadosCidade que serao usados no gatherv
	for(int i = 0; i < nproc; i++){
		//Repassa o numero de cidades de acordo com o numero de regioes
		//distibuida nos processos, multiplicamos pelo tamanho da struct dados
		//Para considerar as informacoes como bytes, assim nao precisamos criar uma estrutura em MPI
		//E o deslocamento das informacoes na variavel dadosCidade sao incluidas corretamente
		numDataEachProcess[i] = (numDataEachProcess[i]/A)*sizeof(struct dados);
		displacementMatNotas[i] = (displacementMatNotas[i]/A)*sizeof(struct dados);
	}

	//root recebe as informacoes em dadosCidade computadas pelos processo
	MPI_Gatherv(dadosCidade, numDataEachProcess[rank], MPI_CHAR, dadosCidade, 
			numDataEachProcess, displacementMatNotas, MPI_CHAR, 0, MPI_COMM_WORLD);

	//Definimos os deslocamento e tamanho dos dados de dadosRegiao que serao usados no gatherv
	//Como anteriormente ja foi tratado a relacao de bytes considerando struct dados
	//Basta apenas desconsiderarmos as informacoes das cidades para repassar dadosRegiao
	for(int i = 0; i < nproc; i++){
		numDataEachProcess[i] = numDataEachProcess[i]/C;
		displacementMatNotas[i] = displacementMatNotas[i]/C;
	}

	MPI_Gatherv(dadosRegiao, numDataEachProcess[rank], MPI_CHAR, dadosRegiao, 
			numDataEachProcess, displacementMatNotas, MPI_CHAR, 0, MPI_COMM_WORLD);

	if(rank == 0){
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
	}
	tempoFim = MPI_Wtime();

	//fim da contagem do programa
	soma += tempoFim - tempoInicio ;

	if(rank == 0){
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
	}
	free(displacementMatNotas);
	free(numDataEachProcess);
	free(matrizNotas);
	free(dadosCidade);
	free(dadosRegiao);
	MPI_Finalize();
	return 0;
}

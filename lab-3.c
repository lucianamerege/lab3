#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

long long int dim; //dimensao do vetor de entrada
long long int nthreads;
float *vetor;
double limSup, limInf;

void * tarefa(void *arg) {
    long long int id = (long int) arg; //identificador da thread
    long long int *quantLocal; //variavel local da quantidade de elementos
    quantLocal = (long long int*) malloc(sizeof(double));
    if(quantLocal==NULL) {exit(1);}
    long long int tamBloco = dim/nthreads;  //tamanho do bloco de cada thread 
    long long int ini = id * tamBloco; //elemento inicial do bloco da thread
    long long int fim; //elemento final(nao processado) do bloco da thread
    if(id == nthreads-1) fim = dim;
    else fim = ini + tamBloco; //trata o resto se houver
    //soma os elementos do bloco da thread
    for(long long int i=ini; i<fim; i++){
        if(vetor[i]< limSup && vetor[i]> limInf){
            *quantLocal += 1;
        }
    }
        
    //retorna o resultado da soma local
    pthread_exit((void *) quantLocal); 
}

int main (int argc, char* argv[]){

    double ini, fim, deltaSeq, deltaConc, desempenho; //tomada de tempo
    pthread_t *tid; //identificadores das threads no sistema
    long long int *retorno, resultSeq = 0, resultConc = 0; //valor de retorno das threads

    if(argc<3) {
        printf("Digite: %s <numero de elementos> <numero de threads>\n", argv[0]);
        return 1;
    }

    dim = atoll(argv[1]);
    nthreads = atoll(argv[2]);

    vetor = (float *) malloc(sizeof(float) * dim);
    if (vetor == NULL) {printf("ERRO--malloc\n"); return 2;} //isso é para caso ele não consiga alocar espaço na memória para essa matriz.

    for (long long int i=0; i<dim; i++){
        vetor[i] = ((float)rand()/(float)(RAND_MAX)) * (rand()%100); //populando o vetor com valores randomicos
    }

    printf("Informe o limiar superior: ");
    scanf("%lf", &limSup);
    printf("\nInforme o limiar inferior: ");
    scanf("%lf", &limInf);

    GET_TIME(ini);

    for(long long int i=0; i<dim; i++){
        if(vetor[i]< limSup && vetor[i]> limInf){
            resultSeq++;
        }
    }

    GET_TIME(fim);
    deltaSeq = fim - ini;

    GET_TIME(ini);

    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}

    //criacao das threads
    for(long long int i=0; i<nthreads; i++) {
        if(pthread_create(tid+i, NULL, tarefa, (void*) i)){
            puts("ERRO--pthread_create"); return 3;
        }
    }

    //aguardar o termino das threads
    for(long int i=0; i<nthreads; i++) {
        if(pthread_join(*(tid+i), (void**) &retorno)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
        //soma global
        resultConc += *retorno;
    }

    if(resultConc==resultSeq){
        printf("Resultados iguais!\n");
    }else{
        puts("ERRO--resultados incompatíveis\n");
        return 3;
    }

    GET_TIME(fim);
    deltaConc = fim - ini;


    printf("Seq %lf\n", deltaSeq);
    printf("Conc %lf\n", deltaConc);
    printf("Aceleração(Tempo Sequencial / Tempo Concorrente):%lf\n", (deltaSeq/deltaConc));

    //liberacao da memoria
    free(vetor);
    free(tid);

    return 0;
}

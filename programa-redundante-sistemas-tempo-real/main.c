//
//  main.c
//  programa-redundante-sistemas-tempo-real
//
//  Created by Luciana Adrião on 06/12/23.
//

/*
 Retornos das funçoes:
 fa() = 1
 fb() = 50
 fc() = 100

         Thread A
         |       ↑
         |       |                   y = fa(),v
         |       |                       fb()
         |       |                       fc()
         AB(1)   BA(50)
         |       |
         |       |
         ↓       |

         Thread B
         |       ↑
         |       |                   y = fb(), v
         |       |                       fa()
         |       |                       fc()
         BC(50)  CB(100)
         |       |
         |       |
         ↓       |

         Thread C
         |       ↑
         |       |                   y = fc(),v
         |       |                       fa()
         |       |                       fb()
         CA(100) AC(1)
         |       |
         |       |
         ↓       |

         Thread A
 */

#include <stdio.h>
#include <pthread.h>

// Array para os canais de comunicação
int canal[6] = {-1, -1, -1, -1, -1, -1};

// Função de envio que atribui um valor ao canal desejado.
void send_async(int *buf, int c) {
    canal[c] = *buf;
    return;
}
// Função de recebimento que obtém um valor do canal desejado.
void receive(int *buf, int c) {
    // Aguarda até que o canal contenha um valor diferente de vazio (valor -1) para continuar.
    while (canal[c] == -1);
    *buf = canal[c];
    canal[c] = -1;                      // Marca o canal como vazio (valor -1) após a leitura.
    return;
}

// Funções de retorno de valores
int fa(void) { return 1; }
int fb(void) { return 5; }
int fc(void) { return 10; }

// Canais
int AB = 0;
int BA = 1;
int BC = 2;
int CB = 3;
int CA = 4;
int AC = 5;

// MARK: - Thread A: realiza operações e comunicação via canal AB (canal[0])
void thread_A(void *threadno) {
    int y;
    int valor = 0;

// Loop principal
    do {
        y = fa();
        send_async(&y, AB);             // Envia o valor para o canal AB
        receive(&valor, CA);            // Recebe o valor do canal CA
        printf("TA executando FA - envia AB: %d e recebe CA: %d\n", y, valor);
    } while (valor == 100);

    if( valor == -5) {
        printf("Valor diferente do esperado na thread A: %d\n", valor);
    }

    // MARK: - Tratamento de falhas
    if (valor == -5) {
        // Tratamento para falha em B

        valor = 100;// Utilizando caminho por CA
        while (valor == 100) {
            printf("TA executando FA\n");
            y = fa();
            send_async(&y, AC);
            receive(&valor, CA);
        }
        while(1){
            fa();fb();fc();
            printf("TA executando Fa, fb, fc \n");
        }

    } else {
        // Tratamento para falha em C
        y = -5;                         // Setando a flag de erro
        do{
            send_async(&y, AB);
            printf("TA executando FC\n");
            fc();
            printf("TA executando FA\n");
            y = fa();
            receive(&valor, BA);
        } while(valor == 50);

        while(1){
            fa();fb();fc();
            printf("Ta executando fa, fb, fc \n");
        }
    }
}


// MARK: - Thread B: realiza operações e comunicação via canal BC (canal[2])
void thread_B(void* threadno) {
    int y, valor;
    receive(&valor, AB);                // Início da execução, recebe o valor do canal AB

    // Loop principal
    while(valor == 1) {
        // Comunicação via canal BC
        y = fb();
        send_async(&y, BC);
        receive(&valor, AB);
        printf("TB executando FB - envia BC: %d e recebe AB: %d \n", y, valor);
    };

    if( valor == -5) {
        printf("Valor diferente do esperado na thread B: %d \n", valor);
    }

    // MARK: - Tratamento de falhas
    if(valor == -5) {
        // Tratamento para falha em C

        valor = 1;                        // Utilizando caminho por AB
        while(valor == 1) {
            printf("TB executando FB \n");
            y = fb();
            send_async(&y, BA);
            receive(&valor, AB);
        };

        while(1){
            fa();fb();fc();
            printf("TB executando Fa, fb, fc \n");
        }
    } else {
        // Tratamento para falha em A
        y = -5;                         // Setando a flag de erro
        do{
            send_async(&y, BC);
            printf("TB executando FB \n");
            fa();
            printf("TB executando FA \n");
            y = fb();
            receive(&valor, CB);
        } while(valor == 100);

        while(1) {
            fa();fb();fc();
            printf("TB executando fa, fb, fc \n");
        }
    }
}

// MARK: - Thread C: realiza operações e comunicação via canal CA (canal[4])
void thread_C(void* threadno) {
    int y, valor;
    receive(&valor, BC);                // Início da execução, recebendo o valor do canal BC
    
    //  Loop principal
    while(valor == 50) {
        y = fc();
        send_async(&y, CA);
        receive(&valor, BC);
        printf("TC executando FC - envia CA: %d e recebe BC: %d \n", y, valor);
    };

    if(valor == -5) {
        printf("Valor diferente do esperado na thread C: %d \n", valor);
    }

    // MARK: - Tratamento de falhas
    if(valor == -5) {
        // Tratamento para falha em A
        valor = 50;
        while(valor == 50) {            // Valor esperado receber
            printf("TC executando FC \n");
            y = fc();
            send_async(&y, CB);
            receive(&valor, BC);
        };
        while(1) {
            fa();fb();fc();
            printf("TC executando fa, fb, fc \n");
        }
    } else {
        // Tratamento para falha em B
        y = -5;
        do {
            send_async(&y, CA);
            printf("TC executando FB \n");
            fb();
            printf("TC executando FC \n");
            y = fc();
            receive(&valor, AC);
        } while (valor == 1);

        while(1) {
            fa();fb();fc();
            printf("TC executando fa, fb, fc \n");
        }
    }
}



int main(int argc, const char * argv[]) {
    // Cria threads para emular o comportamento das tarefas
    pthread_t threadA, threadB, threadC;

    pthread_create(&threadA, NULL, thread_A, NULL);
    pthread_create(&threadB, NULL, thread_B, NULL);
    pthread_create(&threadC, NULL, thread_C, NULL);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
    return 0;
}

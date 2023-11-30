//
//  main.c
//  STR
//
//  Created by Luciana Adrião on 29/11/23.
//

// Este programa demonstra um sistema de comunicação entre threads usando um "mailbox" (caixa de correio) e funções de envio e recebimento sincronizadas.

/*
            Essa versão preenche a mailbox apenas duas vezes.
*/

// Incluindo as bibliotecas necessárias para trabalhar com threads e operações de entrada e saída.
#include <stdio.h>
#include <pthread.h>

// Declaração de um vetor de canais para comunicação entre threads. Cada canal representa uma área de comunicação entre threads.
int canal[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

// Função de envio que atribui um valor ao canal desejado.
void send_sync(int *buf, int c) {
    canal[c] = *buf;
    // Aguarda até que o canal esteja vazio (valor -1) para continuar.
    while (canal[c] != -1);
    return;
}

// Função de recebimento que obtém um valor do canal desejado.
void receive(int *buf, int c) {
    // Aguarda até que o canal contenha um valor diferente de vazio (valor -1) para continuar.
    while (canal[c] == -1);
    *buf = canal[c];
    // Marca o canal como vazio (valor -1) após a leitura.
    canal[c] = -1;
    return;
}

// Função que extrai os bytes de um valor e armazena-os em dois bytes separados.
void encontra_bytes(int valor, unsigned char *byte2, unsigned char *byte1) {
    *byte2 = ((valor >> 8) & 255);
    *byte1 = (valor & 255);
    return;
}

// Função que converte dois bytes em um único valor.
int converte_valor(unsigned char bytex2, unsigned char bytex1) {
    return bytex2 * 256 + bytex1;
}

// Thread de envio - gera valores e envia-os para o canal 0.
void *thread_A(void *threadno) {
    unsigned char id = 4;
    unsigned char msg = 10;
    int valor;
    valor = converte_valor(id, msg);
//    int x = 0;
    while (1) {
        send_sync(&valor, 0);
//        x++;
    }
    return NULL;
}
// Thread de envio - gera valores e envia-os para o canal 1.
void *thread_B(void *threadno) {
    unsigned char id = 5;
    unsigned char msg = 11;
    int valor;
    valor = converte_valor(id, msg);
//    int x = 0;
    while (1) {
        send_sync(&valor, 1);
//        x++;
    }
    return NULL;
}
// Thread de envio - gera valores e envia-os para o canal 2.
void *thread_C(void *threadno) {
    unsigned char id = 6;
    unsigned char msg = 12;
    int valor;
    valor = converte_valor(id, msg);
//    int x = 0;
    while (1) {
        send_sync(&valor, 2);
//        x++;
    }
    return NULL;
}
// Thread de envio - gera valores e envia-os para o canal 3.
void *thread_D(void *threadno) {
    unsigned char id = 7;
    unsigned char msg = 13;
    int valor;
    valor = converte_valor(id, msg);
//    int x = 0;
    while (1) {
        send_sync(&valor, 3);
//        x++;
    }
    return NULL;
}


// Thread de recebimento - recebe valores do canal 4, converte-os e imprime os bytes.
void *thread_E(void *threadno) {

    int valor = 0;
    while (valor != -1) {
        receive(&valor, 4);
//        encontra_bytes(valor, &id, &msg);
        printf("E: %d \n", valor);
    }
    return NULL;
}
// Thread de recebimento - recebe valores do canal 5, converte-os e imprime os bytes.
void *thread_F(void *threadno) {
    int valor = 0;
    while (valor != -1) {
        receive(&valor, 5);
//        encontra_bytes(valor, &id, &msg);
        printf("F: %d \n", valor);
    }
    return NULL;
}
// Thread de recebimento - recebe valores do canal 6, converte-os e imprime os bytes.
void *thread_G(void *threadno) {
    int valor = 0;
    while (valor != -1) {
        receive(&valor, 6);
//        encontra_bytes(valor, &id, &msg);
        printf("G: %d \n", valor);
    }
    return NULL;
}
// Thread de recebimento - recebe valores do canal 7, converte-os e imprime os bytes.
void *thread_H(void *threadno) {
    int valor = 0;
    while (valor != -1) {
        receive(&valor, 7);
//        encontra_bytes(valor, &id, &msg);
        printf("H: %d \n", valor);
    }
    return NULL;
}

// Thread de "mailbox" - recebe valores do canal 0 e, quando chegar ao fim do vetor, envia-os para o canal 4.
void *thread_mailbox(void *threadno) {
    int vetor_mailbox[12];int msgx; int canalx;
    int i, j;
    int limiter = 0;
    unsigned char id, msg;
    while (limiter < 2) {
        for (i = 0; i < 12; i+= 4) {
            receive(&vetor_mailbox[i], 0);              // Recebe no mailbox
            receive(&vetor_mailbox[i + 1], 1);          // Recebe no mailbox
            receive(&vetor_mailbox[i + 2], 2);          // Recebe no mailbox
            receive(&vetor_mailbox[i + 3], 3);          // Recebe no mailbox
        }

        if (&vetor_mailbox[11] != 0) {
            for (j = 0; j < 12; j++) {
                encontra_bytes(vetor_mailbox[j], &id, &msg);
                canalx = (int)id;
                msgx = (int)msg;

                send_sync(&msgx, canalx);
                vetor_mailbox[j] = 0;
            }
        }


        limiter++;
        printf("Loop: %d\n", limiter);
    }

    return NULL;
}

// Função principal - inicia as threads e espera que elas terminem antes de encerrar o programa.
int main(int argc, const char * argv[]) {
    // Cria threads para emular o comportamento das tarefas
    pthread_t threadA,threadB,threadC, threadD, threadE,threadF, threadG,threadH, threadMailBox;
    
    // Threads de send
    pthread_create(&threadA, NULL, thread_A, NULL);
    pthread_create(&threadB, NULL, thread_B, NULL);
    pthread_create(&threadC, NULL, thread_C, NULL);
    pthread_create(&threadD, NULL, thread_D, NULL);

    // Threads de receive
    pthread_create(&threadE, NULL, thread_E, NULL);
    pthread_create(&threadF, NULL, thread_F, NULL);
    pthread_create(&threadG, NULL, thread_G, NULL);
    pthread_create(&threadH, NULL, thread_H, NULL);
    pthread_create(&threadMailBox, NULL, thread_mailbox, NULL);

    // Espera que as threads terminem
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
    pthread_join(threadD, NULL);
    pthread_join(threadE, NULL);
    pthread_join(threadF, NULL);
    pthread_join(threadG, NULL);
    pthread_join(threadH, NULL);
    pthread_join(threadMailBox, NULL);

    return 0;
}

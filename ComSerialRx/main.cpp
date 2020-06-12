/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: marioandre
 *
 * Created on 16 de Agosto de 2018, 00:12
 */

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include "Enquadramento.h"
#include "ARQ.h"
#include <iomanip> 
#include <string.h>

using namespace std;

void dump(char * buffer, int len) {
    int m = 0, line = 0;

    while (m < len) {
        //printf("%02X: ", line * 16);

        for (int n = 0; n < 32 and m < len; n++, m++) {
            int x = (unsigned char) buffer[m]; //(unsigned char), não permite armazenar valores negativos, e portanto pode representar números em um intervalo que vai de 0 até 255.
            printf("%02X ", x);
        }
        puts("");
        line++;
    }
}

//void esvaziar(char * buffer, int len) { //versão do metodo dump em c++
//
//    int m = 0, line = 0;
//
//    while (m < len) {
//
//        cout << hex << setfill('0') << setw(2) << line * 16 << ": ";
//
//        for (int n = 0; n < 16 and m < len; n++, m++) {
//            int x = (unsigned char) buffer[m];
//            cout << hex << uppercase << setfill('0') << setw(2) << x << " "; // uppercase - transforma letra em maiuscula; setw(2)- Define a largura do campo a ser usada nas operações de saída igual a 2. setfill('0') - Define 0 como o caractere de preenchimento do fluxo.  
//        }
//        cout << "\n";
//        line++;
//    }
//}

void convert_str_array(string s, char *a) {
    int total = 0;
    //pegando caracteres da string e colocando no array
    while (s[total] != '\0') {
        a[total] = s[total];
        total++;
    }
}

//int num_caracter(char *str) {
//    int cont = 0;
//    for (; *str; str++) cont++;
//    return cont;
//}

void limpar_buffer(char * b, int len) {
    for (int j = 0; j < len; j++) {
        b[j] = 0;
    }
}

int main(int argc, char * argv[]) {
    Serial dev("/dev/ttyUSB0", B9600);
    //Serial dev("/dev/pts/2", B9600);

    int tam_min_bytes = 8;
    int tam_max_bytes = 256;

    Enquadramento proto(dev, tam_min_bytes, tam_max_bytes);

    ARQ subCaArq(proto);

    char quadroRx[tam_max_bytes];
    char quadroTx[tam_max_bytes];
    char payload[1500] = {};

    //converte string em char*
    //string mensagem = "abcedf1234567";
    //char *msg = new char[mensagem.length() + 1];
    //memcpy(msg, mensagem.c_str(), mensagem.length() + 1);

    bool lp = false;
    int i;
    int bytes = 0;

    while (lp == false) {

        limpar_buffer(quadroRx, tam_max_bytes);

        bytes = subCaArq.recebe(quadroRx);

        if (bytes != 0) {

            int n_bytes_pay = subCaArq.rapassa_payload(payload);

            cout << "Payload recebido(Aplicação): ";
            subCaArq.mostra_buffer(payload, n_bytes_pay);
            cout << endl;
            cout << "---------------------------------------" << endl << endl;

//            cout << "Recebeu " << bytes << " bytes: ";
//            cout.write(quadroRx, bytes);
//            cout << endl;
//            dump(quadroRx, bytes);
//            cout << "---------------------------------------" << endl;
//            cout << endl;

        }
    }

}


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: marioandre
 *
 * Created on 19 de Agosto de 2018, 18:51
 */

#include <iostream>
#include "Serial.h"
#include <sstream>
#include <stdio.h>
#include <string>
#include <cstring>
#include "Enquadramento.h"
#include "ARQ.h"

using namespace std;

/*
 * 
 */

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

int num_caracter(char *str) {
    int cont = 0;
    for (; *str; str++) cont++;
    return cont;
}

void convert_str_array(string s, char *a) {
    int total = 0;
    //pegando caracteres da string e colocando no array
    while (s[total] != '\0') {
        a[total] = s[total];
        total++;
    }
}

void mostra_buffer(char * buf) {

    for (int i = 0; i < num_caracter(buf); i++) {
        cout << buf[i];
    }
    cout << endl;
}

void limpar_buffer(char * b) {
    for (int j = 0; j < num_caracter(b); b++) {
        b[j] = 0;
    }
}

int main(int argc, char** argv) {
     Serial dev("/dev/ttyUSB1", B9600);
    //Serial dev("/dev/pts/1", B9600);
    //Serial rf(argv[1], B9600);
    //string msg = "um teste ...\r\n";

    int tam_min_bytes = 8;
    int tam_max_bytes = 256;
    int n;

    Enquadramento proto(dev, tam_min_bytes, tam_max_bytes);

    ARQ subCaArq(proto);

    string msg;
    string mensagem;
    char bufferTx[256] = {}; // Todos os elementos 0, em C++
    char bufferRx[256];

    int i;
    bool lp = false;

    while (lp == false) {

        cout << "Escolha uma das opções para enviar a mensagem desejada: " << endl;
        cout << "1 - abcedf1234567 " << endl;
        cout << "2 - abcedf~1234567 " << endl;
        cout << "3 - abcedf012}]9876 " << endl;
        cout << "4 - }~}}}~~~}}} " << endl;
        cout << "5 - a}ced~123[45 " << endl;
        cout << "6 - ~~~ed~123~567 " << endl;
        cout << "7 - Digite sua mensagem " << endl;
        cout << "8 - Sair " << endl;
        cout << "------------------------------" << endl;
        cout << "opção escolhida: ";
        cin >> i;
        cout << endl;

        switch (i) {
            case 1:
                msg = "abcedf1234567";

                break;
            case 2:
                msg = "abcedf~1234567"; //abcedf}^1234567

                break;
            case 3:
                msg = "abcedf012}9876"; //abcedf012}]9876

                break;
            case 4:
                msg = "}~}}}~~~}}}"; //}]}^}]}]}]}^}^}^}]}]}]

                break;
            case 5:
                msg = "a}ced~123[45"; //} a}]ced}^123[45          

                break;
            case 6:
                msg = "~~~ed~123~567";

                break;
            case 7:
                cout << "Digite a mensagem: ";
               
                limpar_buffer(bufferTx); //limpando o buffer
                cin.ignore(); //essa linha foi usada pro getline funcionar
                cin.getline(bufferTx, 256);

                break;
            case 8:
                lp = true;
                break;
            default:
                msg = "teste";
        }

        if (lp == false) {

            if (i != 7) {
                limpar_buffer(bufferTx); //limpando o buffer
                convert_str_array(msg, bufferTx); //converte o conteudo da string para o array
            }


            cout << "** Preparando envio de quadro **" << endl;
            cout << "--------------------------------" << endl;

            cout << "Payload(dados): ";
            mostra_buffer(bufferTx);

            subCaArq.envia(bufferTx, num_caracter(bufferTx));
            
            limpar_buffer(bufferRx); //limpando o buffer

            //n = subCaArq.recebe(bufferRx);
//            cout << "Recebeu " << n << " bytes: ";
//            cout.write(bufferRx, n);
//            cout << endl;
//            dump(bufferRx, n);
            cout << endl;
            cout << "---------------------------------------" << endl;
            cout << endl;
        }
    }

    return 0;
}




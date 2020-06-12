/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ARQ.cpp
 * Author: marioandre
 * 
 * Created on 26 de Setembro de 2018, 21:43
 */

#include "ARQ.h"

ARQ::ARQ() {
}

ARQ::ARQ(Enquadramento & a_enq) {
    enq = a_enq;
    estado = Ocioso;
}

ARQ::ARQ(const ARQ& orig) {
}

ARQ::~ARQ() {
}

void ARQ::handle(Evento & e) {

    switch (estado) {
        case Ocioso:
            //cout << "Estou no estado Ocioso" << endl;

            if (e.tipo == Payload) {
                cout << "Estou no evento payload" << endl;
                //
                //                cout << "add proto: ";
                //                add_proto(e.ptr, e.bytes);
                //                mostra_buffer(e.ptr);
                //
                //                cout << "add controle: ";
                //                val_ini_controle ^= 0b10000000; //XOR no valor para ficar alternando o bit 7
                //                add_controle(e.ptr, num_caracter(e.ptr), val_ini_controle);
                //                mostra_buffer(e.ptr);
                //
                //                enq.envia(e.ptr, num_caracter(e.ptr));
                //                //seta timeout
                //                estado = Espera;

            } else { //senao evento Quadro
                    
                    bytes_recebidos = enq.recebe(e.ptr);

                   
                    if (enq.timeout_ARQ == false) {

                        if (enq.crc_ok == true) {
                            
                            enq.crc_ok  = false;
                            
                            int n_bytes_quadro = enq.repassa_quadro(quadro);

                            cout << "Quadro sem CRC: ";
                            mostra_buffer(quadro, n_bytes_quadro);
                            uint8_t campo_controle = quadro[0];
                            uint8_t campo_proto = quadro[1];

                            cout << "Campo controle(HEX): ";
                            printf("%02X ", campo_controle);
                            cout << endl;
                            cout << "Campo proto(HEX): ";
                            printf("%02X ", campo_proto);
                            cout << endl;

                            if (!(campo_controle & 0b10000000)) { //testa o bit 7 - se for data
                                cout << "Tipo de quadro: Dados" << endl;
                                char ack[8] = {}; //inicia com todos os valoes 0
                                uint8_t campo_ack;

                                if (campo_controle & 0b00001000) { //testa o bit 3 - se for sequecia 1 
                                    cout << "Sequencia do quadro: 1" << endl;
                                    //sequencia 1

                                    if (quadro_seq_1 == ultimo_quadro_recebido) {

                                        cout << "Quadro com sequencia 1 ja recebido!" << endl;

                                        ack[0] = 0b10001000; //0x88
                                        campo_ack = ack[0];

                                        cout << endl;
                                        cout << "Tipo de resposta: ack1" << endl;
                                        cout << "Gerando ack1..." << endl;
                                        cout << "ack1: ";
                                        mostra_buffer(ack, 1);
                                        cout << "ack0(HEX): ";
                                        printf("%02X ", campo_ack);
                                        cout << endl;

                                        enq.envia(ack, 1);
                                        cout << "ack 1 enviado!" << endl << endl;
                                        limpar_buffer(payload, 1500);

                                    } else {

                                        ack[0] = 0b10001000; //0x88
                                        campo_ack = ack[0];

                                        cout << endl;
                                        cout << "Tipo de resposta: ack1" << endl;
                                        cout << "Gerando ack1..." << endl;
                                        cout << "ack1: ";

                                        mostra_buffer(ack, 1);
                                        cout << "ack0(HEX): ";
                                        printf("%02X ", campo_ack);
                                        cout << endl;

                                        enq.envia(ack, 1);
                                        cout << "ack 1 enviado!" << endl << endl;

                                        int n_bytes_payload = n_bytes_quadro - 2;

                                        pega_payload(n_bytes_quadro);
                                        ultimo_quadro_recebido = 1;

                                    }

                                } else { //testa o bit 3 - se for sequecia 0
                                    cout << "Sequencia do quadro: 0" << endl;
                                    //sequencia 0

                                    ack[0] = 0b10000000; //0x80
                                    campo_ack = ack[0];

                                    if (quadro_seq_0 == ultimo_quadro_recebido) {

                                        cout << "Quadro com sequencia 0 ja recebido!" << endl;

                                        cout << endl;
                                        cout << "Tipo de resposta: ack0" << endl;
                                        cout << "Gerando ack0..." << endl;
                                        cout << "ack0: ";
                                        mostra_buffer(ack, 1);
                                        cout << "ack0(HEX): ";
                                        printf("%02X ", campo_ack);
                                        cout << endl;

                                        enq.envia(ack, 1);
                                        cout << "ack 0 enviado!" << endl << endl;
                                        limpar_buffer(payload, 1500);

                                    } else {

                                        cout << endl;
                                        cout << "Tipo de resposta: ack0" << endl;
                                        cout << "Gerando ack0..." << endl;
                                        cout << "ack0: ";
                                        mostra_buffer(ack, 1);
                                        cout << "ack0(HEX): ";
                                        printf("%02X ", campo_ack);
                                        cout << endl;

                                        enq.envia(ack, 1);
                                        cout << "ack 0 enviado!" << endl << endl;
                                        pega_payload(n_bytes_quadro);
                                        ultimo_quadro_recebido = 0;

                                    }

                                }
                                estado = Ocioso;
                                quadro_chegou = true;
                            }

                        }
                    }
                //}
            }

            break;
        case Espera:
            cout << "Estou no estado Espera" << endl;

            //            int n;
            //            char bufRx[256];
            //
            //            n = enq.recebe(bufRx);
            //
            //            if (enq.crc_ok == true) {
            //                char * quadro;
            //
            //                enq.pega_quadro(quadro);
            //
            //                cout << "Quadro: ";
            //                mostra_buffer(quadro);
            //
            //                if (quadro[0] & 10000000) { //testa o bit 7 - se for 1 é ack
            //                    if (quadro[0] & 00001000) { //testa o bit 3
            //                        //sequencia 1 - ack 1
            //                        cout << "ack 1 recebido: " << endl;
            //                        quadro_chegou = true;
            //                        estado = Ocioso;
            //                    } else {
            //                        //sequencia 0 - ack0
            //                        cout << "ack 0 recebido: " << endl;
            //                        quadro_chegou = true;
            //                        estado = Ocioso;
            //                    }
            //                }
            //
            //
            //            }



            break;
    }
}

void ARQ::envia(char * buffer, int bytes) {

    //add_proto(buffer, bytes);
    //add_controle(buffer, num_caracter(buffer));

    //    for (int i = 0; i < num_caracter(buffer); i++) {
    //        cout << buffer[i];
    //    }
    //cout << endl;
    enq.envia(buffer, bytes);

    //    Evento te(Payload, buffer, bytes);
    //
    //    quadro_chegou = false;
    //
    //    while (quadro_chegou == false) {
    //
    //        handle(te); //maquina de estado do ARQ
    //
    //
    //    }


}

int ARQ::recebe(char * buffer) {

    Evento te(Quadro, buffer, 0);
  
    handle(te); //maquina de estado do ARQ
    
    return bytes_recebidos;
}

void ARQ::add_proto(char *buf, int b) {

    int posicaoZero = 0;

    for (int i = b; i > posicaoZero; i--) {
        buf[i] = buf[i - 1];
    }
    buf[0] = '1'; //proto - valor qualquer por enquanto

}

void ARQ::add_controle(char *buf, int b, char contr) {
    int posicaoZero = 0;

    for (int i = b; i > posicaoZero; i--) {
        buf[i] = buf[i - 1];
    }
    buf[0] = 0x00; //controle - 0x00 -> 0 em hex tem o bit 7 igual a zero
}

void ARQ::pega_payload(int len) {
    int id = 0;
    limpar_buffer(payload, 1500);

    for (int i = 2; i < len; i++) {
        payload[id] = this->quadro[i];
        id++;
    }

    n_bytes_payload = len - 2;

}

void ARQ::pega_controle() {

    controle = quadro[0];

}

void ARQ::pega_proto() {

    proto = this->quadro[1];

}

void ARQ::mostra_buffer(char * buf, int bytes) {

    for (int i = 0; i < bytes; i++) {
        cout << buf[i];
    }
    cout << endl;
}

void ARQ::limpar_buffer(char * b, int len) {
    for (int j = 0; j < len; j++) {
        b[j] = 0;
    }
}

int ARQ::rapassa_payload(char * p) {

    memcpy(p, this->payload, 1500);

    return n_bytes_payload;

}
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
            cout << "Estou no estado Ocioso" << endl;

            if (e.tipo == Payload) { //evento Payload
                cout << "Estou no evento payload" << endl;

                cout << "add proto: ";
                add_proto(e.ptr, e.bytes);
                mostra_buffer(e.ptr, e.bytes + 1);


                cout << "add controle: ";

                val_ini_controle ^= 0b00001000; //XOR no bit 3 para ficar alternando o bit em 0 e 1

                add_controle(e.ptr, (e.bytes + 1), val_ini_controle);
                mostra_buffer(e.ptr, (e.bytes + 2));

                cout << "Campo controle(HEX): ";
                printf("%02X ", val_ini_controle);
                cout << endl;
                
                cout << "Sequencia do quadro: ";
                if(val_ini_controle & 0b00001000){
                    cout << "1" << endl;
                }else{
                    cout << "0" << endl;
                }

                n_bytes_quadro_arq = e.bytes + 2;
                enq.set_timeout(tempo_timeout);
                enq.envia(e.ptr, n_bytes_quadro_arq);

                estado = Espera;

            } else { //senao evento Quadro
                cout << "Estou no evento Quadro" << endl;
                //                int n;
                //
                //
                //                n = enq.recebe(e.ptr);
                //
                //                if (enq.crc_ok == true) {
                //                    char * quadro;
                //
                //                    enq.pega_quadro(quadro);
                //
                //                    cout << "Quadro: ";
                //                    mostra_buffer(quadro);
                //
                //                    if (!(quadro[0] & 10000000)) { //testa o bit 7 - se for data
                //                        if (quadro[0] & 00001000) { //testa o bit 3
                //                            //sequencia 1
                //                            char ack[6];
                //                            ack[0] = 0x88;
                //
                //                            cout << "ack 1 enviado: " << endl;
                //                            enq.envia(ack, num_caracter(ack));
                //                            estado = Ocioso;
                //                            quadro_chegou = true;
                //                        } else {
                //                            //sequencia 0
                //                            char ack[6];
                //                            ack[0] = 0x80;
                //
                //                            cout << "ack 0 enviado: " << endl;
                //                            enq.envia(ack, num_caracter(ack));
                //                            estado = Ocioso;
                //                            quadro_chegou = true;
                //                        }
                //                    }
                //
                //
                //                }
            }

            break;
        case Espera:
            //cout << "Estou no estado Espera" << endl;

            if (e.tipo == Payload) { //evento Payload
                
                char bufRx[256];

                bytes_recebidos = enq.recebe(bufRx);

                
                if (enq.timeout_ARQ == false) {
                    //cout << "n: " << n << endl;
                    enq.reenviar = false;
                    num_timeout_ocorridos = 0;

                    if (enq.crc_ok == true) {

                        int n_bytes_quadro = enq.repassa_quadro(quadro);

                        cout << "Quadro sem CRC: ";
                        mostra_buffer(quadro, n_bytes_quadro);
                        uint8_t campo_controle_recebido = quadro[0];

                        cout << "Campo controle(HEX): ";
                        printf("%02X ", campo_controle_recebido);
                        cout << endl;
                       
                        if (campo_controle_recebido & 0b10000000) { //testa o bit 7 - se for 1 é ack
                            
                            cout << "Tipo de quadro: Ack" << endl << endl;

                            if ((val_ini_controle & 0b00001000) == (campo_controle_recebido & 0b00001000)) {
                                                                          
                                if (campo_controle_recebido & 0b00001000) { //testa o bit 3 , se for 1 é ack1
                                    //sequencia 1 - ack 1
                                    cout << "ack 1 recebido! " << endl;
                                    
                                } else { //testa o bit 3 , se for 0 é ack0
                                    //sequencia 0 - ack0
                                    cout << "ack 0 recebido! " << endl;
                                   
                                }
                                quadro_chegou = true;
                                estado = Ocioso;
                                enq.set_timeout(0); //cancela o timeout
                                
                            } else {
                                cout << "Ack recebido nao corresponde ao quadro enviado!" << endl;
                               
                                enq.reenviar = true;
                                
                                cout << "Setando timeout!" << endl;
                                enq.set_timeout(tempo_timeout);
                                
                                cout << "Reenviando o quadro!" << endl;
                                enq.envia(e.ptr, enq.n_bytes_buf_enviado);
                                   
                            }
                        } else { //senão é quadro de dados

                        }

                    }
                } else { //
                    enq.teste_timeout_arq = 0;
                    enq.timeout_ARQ = false;
                    Evento ev_time;
                    handle(ev_time);
                    enq.reenviar = true;
                    cout << "Reenviando o quadro!" << endl;
                    enq.envia(e.ptr, enq.n_bytes_buf_enviado);

                }
            }

            if (e.tipo == Quadro) { //evento Quadro

                
            }

            if (e.tipo == Timeout) { //Timeout
                num_timeout_ocorridos++;
                cout << "Timeout " << num_timeout_ocorridos << endl;
                cout << "O tempo de espera da resposta terminou!" << endl;
                cout << "Setando timeout!" << endl;
                enq.set_timeout(tempo_timeout);
                enq.c = 0;
                
            }

            break;
    }
}

void ARQ::envia(char * buffer, int bytes) {

    Evento te(Payload, buffer, bytes);

    quadro_chegou = false;

    while (quadro_chegou == false) {
        handle(te); //maquina de estado do ARQ
    }
}

int ARQ::recebe(char * buffer) {

    Evento te2(Quadro, buffer, 0);

    handle(te2); //maquina de estado do ARQ

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
   
    buf[0] = contr;
}

void ARQ::mostra_buffer(char * buf, int bytes) {

    for (int i = 0; i < bytes; i++) {
        cout << buf[i];
    }
    cout << endl;
}


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ARQ.h
 * Author: marioandre
 *
 * Created on 26 de Setembro de 2018, 21:43
 */

#ifndef ARQ_H
#define ARQ_H

#include "Enquadramento.h"

class ARQ {
public:
    
    
    
    ARQ();
    ARQ(Enquadramento & a_enq);
    ARQ(const ARQ& orig);
    virtual ~ARQ();
    
    void envia(char * buffer, int bytes);
    int recebe(char * buffer);
    void set_timeout(long tout);
    void add_proto(char *buf, int b);
    void add_controle(char *buf, int b, char contr);
    int num_caracter(char *str);
    void pega_quadro();
    void pega_payload(int len);
    void pega_controle();
    void pega_proto();
    void mostra_buffer(char * buf, int bytes);
    void limpar_buffer(char * b, int len);
    int rapassa_payload(char * buf);
   
     
private:
    
    Enquadramento enq;
    long timeout;
    char quadro[4094] = {}; // Todos os elementos 0, em C++
    char payload[1500] = {}; // Todos os elementos 0, em C++
    char controle;
    char proto;
    bool quadro_chegou = false;
    int estado;
    char val_ini_controle = 0xB2;
    int bytes_recebidos;
    bool ultimo_quadro_recebido = 1;
    bool quadro_seq_0 = 0;
    bool quadro_seq_1 = 1;
    int n_bytes_payload = 0;
    
    
   
  enum Estados {Ocioso, Espera};  
  enum TipoEvento {Payload, Quadro, Timeout};
 
  // esta struct descreve um Evento
  struct Evento {
    TipoEvento tipo;
    char * ptr;
    int bytes;
 
    // construtor sem parâmetros: cria um Evento Timeout
    Evento(){ tipo = Timeout;}
 
    // construtor com parâmetros: cria um evento Payload ou Quadro
    Evento(TipoEvento t, char * p, int len) : tipo(t), ptr(p), bytes(len) {}
  };
 
  // executa a MEF, passando como parâmetro um evento
  void handle(Evento & e);

};

#endif /* ARQ_H */


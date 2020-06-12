/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Enquadramento.h
 * Author: aluno
 *
 * Created on 16 de Agosto de 2018, 08:57
 */

#ifndef ENQUADRAMENTO_H
#define ENQUADRAMENTO_H

#include "Serial.h"
//#include<string>
#include <cstring>
#include <iostream>
#include <assert.h>
//#include <unistd.h>
//#include <stdlib.h> 



#define P       0x8408
#define PPPINITFCS16    0xffff  /* Initial FCS value */
#define PPPGOODFCS16    0xf0b8  /* Good final FCS value */

using namespace std;

class Enquadramento {

public:
    
    bool crc_ok = false;
    bool reenviar = false;
    int n_bytes_buf_enviado = 0;
    int c = 0;
    bool quadro_completo = false;
    int n_bytes_recebidos = 0;
    bool ocorreu_timeout = false;
    
    bool timeout_ARQ = false;
     int teste_timeout_arq = 0;
    
    Enquadramento();
    Enquadramento(Serial & dev, int bytes_min, int bytes_max);
    Enquadramento(const Enquadramento& orig);
    virtual ~Enquadramento();
    
    // envia o quadro apontado por buffer
    // o tamanho do quadro é dado por bytes 
    void envia(char * buffer, int bytes);

    // espera e recebe um quadro, armazenando-o em buffer
    // retorna o tamanho do quadro recebido
    int recebe(char * buffer);
    
    //void mostra_buffer(char * buf);
    void mostra_buffer(char * buf, int bytes);
    //void mostra_buffer_2(char * buf, int len);
    
    void pega_payload_crc();
    int repassa_quadro(char * q);
    
    void add_flag(char *m, int e);
    int num_caracter(char *str);
    void limpar_buffer(char * b);
    void limpar_buffer(char * b, int bytes);
    void verif_carac_escape(char * buf, int len);
    void pega_quadro(char * c);
    void set_timeout(int tempo);
    void dump(char * buffer, int len);
      
private:
    
    typedef unsigned short u16;
    static u16 fcstab[256];
    
    int min_bytes, max_bytes; // tamanhos mínimo e máximo de quadro
    Serial porta;
    
    char buffer[4096]; // quadros no maximo de 4 kB (hardcoded)
    char payload[1500] = {}; // Todos os elementos 0, em C++
    char valCRC[2] = {}; // Todos os elementos 0, em C++
    char quadro[4094]; //
    bool carac_especial;
     
    char quadro_sem_crc[4094]; //
    int num_carac_especial_rx = 0;
    int num_carac_especial_tx = 0;
    int num_bytes_quadro_sem_crc = 0;
    int num_de_quadros_recebidos = 0;
    int timeout_enq = 50;
    
    enum Estados {
        Ocioso, RX, RECEP, ESC  //Ocioso = 0; RX = 1; ESC = 2
    };

    // bytes recebidos pela MEF até o momento  
    int n_bytes;

    // estado atual da MEF
    int estado;
    
    // aqui se implementa a máquina de estados de recepção
    // retorna true se reconheceu um quadro completo
    bool handle(char byte);
    
    // verifica o CRC do conteúdo contido em "buffer". Os dois últimos 
    // bytes desse buffer contém o valor de CRC
    bool check_crc(char * buffer, int len);
 
    // gera o valor de CRC dos bytes contidos em buffer. O valor de CRC
    // é escrito em buffer[len] e buffer[len+1]
    void gen_crc(char * buffer, int len);
 
    // calcula o valor de CRC dos bytes contidos em "cp".
    // "fcs" deve ter o valor PPPINITFCS16
    // O resultado é o valor de CRC (16 bits)
    // OBS: adaptado da RFC 1662 (enquadramento no PPP)
    //uint16_t pppfcs16(uint16_t fcs, char * cp, int len);
    u16 pppfcs16(u16 fcs, char *cp, int len);
    
    void gerar_tabFCS();
  
};

#endif /* ENQUADRAMENTO_H */


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Enquadramento.cpp
 * Author: aluno
 * 
 * Created on 16 de Agosto de 2018, 08:57
 */

#include "Enquadramento.h"


Enquadramento::Enquadramento() {
}

Enquadramento::u16 Enquadramento::fcstab[256];

Enquadramento::Enquadramento(Serial& dev, int bytes_min, int bytes_max) {
    min_bytes = bytes_min;
    max_bytes = bytes_max;
    porta = dev;
    estado = Ocioso;

    gerar_tabFCS();
}

Enquadramento::Enquadramento(const Enquadramento& orig) {
}

Enquadramento::~Enquadramento() {
}

void Enquadramento::envia(char* buffer, int bytes) {

    //gera CRC e adciona no buffer  do quadro
    cout << "Gerando CRC: ";
    gen_crc(buffer, bytes);
    int n_bytes_buffer_com_crc = bytes + 2;
    mostra_buffer(buffer, n_bytes_buffer_com_crc);

    //verifica se quadro possui caracter de escape
    cout << "Verificando se há caracteres especais... " << endl;
    num_carac_especial_tx = 0;
    verif_carac_escape(buffer, n_bytes_buffer_com_crc);

    if (carac_especial == true) {
        cout << "Caracter especial encontrado!" << endl;
        cout << "Modificando quadro: ";
        mostra_buffer(buffer, n_bytes_buffer_com_crc + num_carac_especial_tx);
    } else {
        cout << "Nenhum caracter especial foi encontrado!" << endl;
    }

    cout << "add flag: ";
    add_flag(buffer, n_bytes_buffer_com_crc + num_carac_especial_tx);

    int n_bytes_buffer_final = n_bytes_buffer_com_crc + num_carac_especial_tx + 2;
    mostra_buffer(buffer, n_bytes_buffer_final);

    int n = porta.write(buffer, n_bytes_buffer_final);

    cout << endl;
    cout << "Enviou " << n << " bytes: ";
    for (int i = 0; i < n; i++) {
        cout << buffer[i];
    }
    cout << endl << endl;
}

int Enquadramento::recebe(char* buffer) {
    
    int bytes_recebidos = 0;
    int i = 0;
    char quadroRecebido[256] = {};
    teste_timeout_arq = 0;
    
    cout << "Aguardando recebimento de quadro..." << endl;
    cout << "*************************************" << endl;
    num_carac_especial_rx = 0;
    limpar_buffer(this->buffer, 4096);
    quadro_completo = false;
    
    while(quadro_completo == false){
        n_bytes_recebidos = porta.read(buffer, max_bytes, true);

        if (n_bytes_recebidos != 0) {
            
            bytes_recebidos += n_bytes_recebidos;
            
            for (int j = 0; j < n_bytes_recebidos; j++) {
                quadroRecebido[i] = buffer[j];
                
                cout << "Caracter recebido: ";
                mostra_buffer(buffer, n_bytes_recebidos);
                
                quadro_completo = handle(buffer[j]); //maquina de estado enquadramento   
                //cout << "quadro completo: " << quadro_completo << endl;
                i++;
            }
                    
        } else {
//            if(teste_timeout_arq == 0){
//                timeout_ARQ = true;
//                teste_timeout_arq = 1;
//            }
            ocorreu_timeout = true;
        }
    }
    
    cout << endl;
    cout << "Recebeu " <<bytes_recebidos << " bytes: ";
            dump(quadroRecebido, bytes_recebidos);
            cout << endl;

    if (quadro_completo == true) {
       
        num_de_quadros_recebidos++;
        cout << "-------------------------------------" << endl;
        cout << "## Quadro " << num_de_quadros_recebidos << " recebido ##" << endl;
        cout << "-------------------------------------" << endl;

        cout << "Quadro recebido: ";
        //mostra_buffer(buffer, n_bytes_recebidos);
        mostra_buffer(quadroRecebido, bytes_recebidos);


        cout << "Retirando flags..." << endl;

        int n_bytes_quadro_sem_flags = n_bytes;

        if (quadro_completo == true) {

            cout << "Quadro recebido sem flags: ";
            mostra_buffer(this->buffer, n_bytes_quadro_sem_flags);

            cout << "Checando CRC do quadro: ";
            crc_ok = check_crc(this->buffer, n_bytes_quadro_sem_flags);
        } else {
            cout << "Quadro recebido esta corrompido" << endl;
        }

        num_bytes_quadro_sem_crc = n_bytes_quadro_sem_flags - 2;

        if (crc_ok == true) {
            cout << "Retirando CRC do quadro... " << endl;
            for (int i = 0; i < (num_bytes_quadro_sem_crc); i++) {
                quadro_sem_crc[i] = this->buffer[i];
            }
        }

    }
     n_bytes_recebidos = bytes_recebidos;
    return n_bytes_recebidos;
    //return bytes_recebidos;
}

bool Enquadramento::handle(char byte) {
  


    switch (estado) {
        case Ocioso: // estado Ocioso
            if (byte == 0x7E) { //se o byte for ~ (0x7E)
                
                n_bytes = 0;
                estado = RX; // muda para RX
                quadro_completo = false;
                porta.set_timeout(timeout_enq); //seta timeout em 50ms
                ocorreu_timeout = false;
            } else {
                estado = Ocioso;
            }

            break;
        case RX: // estado RX
           

            if (ocorreu_timeout == false) {
                
                if (byte == 0x7E) { //se o byte for ~ (0x7E)

                    estado = RX; // muda para RX
                    
                } else if (byte == 0x7D) {
                    num_carac_especial_rx++;
                    estado = ESC;
                }else {
                    buffer[n_bytes] = byte;
                    n_bytes++;
                    estado = RECEP; // muda para ESC
                }
                quadro_completo = false;
                porta.set_timeout(timeout_enq); //seta timeout em 50ms
            } else {
                cout << "timeout - O proximo byte demorou pra chegar" << endl;
                estado = Ocioso;
                ocorreu_timeout = false;
            }
            break;

        case RECEP: // estado RX
            
            if (ocorreu_timeout == false) {
                if (byte == 0x7E) { //se o byte for (0x7E) ~
                    //copia buffe
                    estado = Ocioso;
                    quadro_completo = true;
                } else if (byte == 0x7D) { //se o byte for (0x7D) } 
                    num_carac_especial_rx++;
                    estado = ESC; // muda para ESC
                    quadro_completo = false;
                } else if (n_bytes > 4096) { //overflow ou timeout

                    estado = Ocioso;
                    quadro_completo = false;

                    n_bytes = 0; //representação de descartar o buffer

                } else {
                    buffer[n_bytes] = byte;
                    n_bytes++;
                    estado = RECEP;
                    quadro_completo = false;
                }
                porta.set_timeout(timeout_enq); //seta timeout em 50ms
            } else {
                cout << "timeout - O proximo byte demorou pra chegar" << endl;
                estado = Ocioso;
                ocorreu_timeout = false;
                n_bytes = 0; //representação de descartar o buffer
            }

            break;

        case ESC: // estado ESC

            if (ocorreu_timeout == false) {

                if (byte == 0x7E || byte == 0x7D || n_bytes_recebidos == 0) { //se for 0x7E ou 0x7D ou timeout
                    cout << "timeout" << endl;
                    n_bytes = 0; //representação de descartar o buffer 
                    estado = Ocioso; // muda para Ocioso
                } else {
                    char byte_xor;

                    byte_xor = byte ^ 0x20; // faz (byte XOR 0x20)

                    buffer[n_bytes] = byte_xor;
                    n_bytes++;
                    estado = RECEP;
                }
                quadro_completo = false;
                porta.set_timeout(timeout_enq); //seta timeout em 50ms
            } else {
                cout << "timeout - O proximo byte demorou pra chegar" << endl;
                estado = Ocioso;
                ocorreu_timeout = false;
                n_bytes = 0; //representação de descartar o buffer
            }
            break;
    }

    return quadro_completo;
}

bool Enquadramento::check_crc(char * buffer, int len) {
    u16 trialfcs;
    bool resultado;
    
    /* check on input */
    trialfcs = pppfcs16(PPPINITFCS16, buffer, len);

    if (trialfcs == PPPGOODFCS16) {
        //printf("Good FCS\n");
        cout << "Good FCS - O quadro esta OK" << endl;
        resultado = true;
    } else {
        // printf("Erro! quadro corrompido\n");
        cout << "Erro! quadro corrompido" << endl;
        resultado = false;
        cout << endl;
    }
    
    return resultado;
}

void Enquadramento::gen_crc(char * buffer, int len) {

    u16 trialfcs;

    /* add on output */
    trialfcs = pppfcs16(PPPINITFCS16, buffer, len);
    trialfcs ^= 0xffff; /* complement */
    buffer[len] = (trialfcs & 0x00ff); /* least significant byte first */
    buffer[len + 1] = ((trialfcs >> 8) & 0x00ff);


}

/*
 * Calculate a new fcs given the current fcs and the new data.
 */
Enquadramento::u16 Enquadramento::pppfcs16(u16 fcs, char *cp, int len) {

    assert(sizeof (u16) == 2);
    assert(((u16) - 1) > 0);
    while (len--)
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];

    return (fcs);
}

void Enquadramento::gerar_tabFCS() {
    unsigned int b, v;
    int i;

    for (b = 0;;) {
        v = b;
        for (i = 8; i--;)
            v = v & 1 ? (v >> 1) ^ P : v >> 1;

        fcstab[b] = (v & 0xFFFF);
        if (++b == 256) break;
    }
}

void Enquadramento::add_flag(char *m, int e) {

    int posicaoZero = 0;

    for (int i = e; i > posicaoZero; i--) {
        m[i] = m[i - 1];
    }

    m[0] = '~';
    m[e + 1] = '~';
}

void Enquadramento::mostra_buffer(char * buf, int bytes) {

    //cout << "Mensagem do quadro: ";
    for (int i = 0; i < bytes; i++) {
        cout << buf[i];
    }
    cout << endl;
}

int Enquadramento::repassa_quadro(char * quadro) {

    memcpy(quadro, this->quadro_sem_crc, 4094);

    return num_bytes_quadro_sem_crc;
}

void Enquadramento::limpar_buffer(char * b, int bytes) {
    for (int j = 0; j < bytes; j++) {
        b[j] = 0;
    }
}

void Enquadramento::verif_carac_escape(char * buf, int len) {
    for (int i = 0; i < len; i++) {

        if (buf[i] == 0x7E || buf[i] == 0x7D) {
            carac_especial = true;
            num_carac_especial_tx++;

            int posi = i + 1;
            char caracter_xor = buf[i] ^ 0x20; //XOR 0x20
            buf[i] = 0x7D; // caracter }

            for (int j = len; j > posi; j--) {
                buf[j] = buf[j - 1];
            }
            buf[posi] = caracter_xor;
        }

    }
}

void Enquadramento::dump(char * buffer, int len) {
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



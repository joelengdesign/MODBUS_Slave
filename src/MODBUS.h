#ifndef MODBUS_H
#define MODBUS_H
#include "MODBUSAux.h"

// classe filha
class MODBUS:public MODBUSAux{ // a classe MODBUS herda os métodos da classe MODBUSAux
    private:
        // As portas RE e DE do RS485 devem ser curtadas e conectadas a esta porta definida pelo projetista
        int _pinoRS485Comunicacao;

        // Método que calcula o Error Check
        uint16_t ErrorCheck(byte mensagem[], int tamanho);

        // método para organizar o pacote e serializar
        void OrganizePkg();

    public:
        // Método que seta o pino de controle digital do RS485 e o pino digital do led para teste
        MODBUS(int _pinoRS485Comunicacao);

        // Método que faz a requisição dos dados do UAC
        void EnviarPacote(byte EnderecoDoDispositivo,byte CodigoDaFuncao, uint16_t EnderecoInicial, uint16_t QuantityRegisters);
        
        // Método que recebe os pacotes do dispositivo mestre
        void ReceivePackage(byte QuantityRegisters, unsigned long TimeResponse);

        // método que é chamado para limpar os ponteiros
        byte* clearPackage(byte* ponteiro, int tamanho);

        // Método que verifica a integridade do pacote
        bool validacaoPacote(byte* pacote, int tamanho);
};

#endif
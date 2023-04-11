#ifndef MODBUSAux_H
#define MODBUSAux_H

// classe pai com métodos auxiliares
class MODBUSAux{
    public:
        // função que converte os valores hexadecimais do formato IEEE754 para float
        float IEEE754_HexToFloat(byte dado[], int n1, int n2, int n3, int n4);

        // função para printar pacotes
        void printar(byte pacote[], byte tamanho);
        
        // ponteiro para receber as variáveis do dispositivo mestre
        byte* PackageReceived;
        
        // ponteiro que recebe as variáveis com os bytes corrigidos da variável PackageReceived
        byte* ReadyPackage;

        // quantidade de registradores
        uint16_t QuantidadeDeRegistradores;
};

#endif
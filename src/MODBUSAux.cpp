#include <Arduino.h>
#include "MODBUSAux.h"

float MODBUSAux::IEEE754_HexToFloat(byte dado[], int n1, int n2, int n3, int n4){
  union{
    float valor_float;
    uint32_t valor_inteiro;
  }conversor;

  conversor.valor_inteiro = ((uint32_t)dado[n1] << 24) | 
                              ((uint32_t)dado[n2] << 16)|
                              ((uint32_t)dado[n3] << 8) | 
                              (uint32_t)dado[n4];
  return(conversor.valor_float);
}

// função para printar vetores em hexadecimal
void MODBUSAux::printar(byte pacote[], byte tamanho){
  for(byte i=0; i<tamanho;i++){
      if(pacote[i]<0x10){
        Serial.print("0");Serial.print(pacote[i],HEX); Serial.print(" ");
      }
      else{
        Serial.print(pacote[i],HEX); Serial.print(" ");
      }
    }
    Serial.println(""); Serial.println("------------------");
}
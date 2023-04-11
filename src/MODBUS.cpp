#include "MODBUS.h"
#include "MODBUSAux.h"


MODBUS::MODBUS(int _pinoRS485Comunicacao){
    pinMode(_pinoRS485Comunicacao, OUTPUT);
}

void MODBUS::EnviarPacote(byte DeviceAdress,
                        byte FunctionCode, uint16_t InitAdress,
                        uint16_t QuantityRegisters){
    int pinoRS485Comunicacao = _pinoRS485Comunicacao;

    byte ShippingCode[8];
    uint16_t ShippingErrorCheck;

    ShippingCode[0] = DeviceAdress;
    ShippingCode[1] = FunctionCode;
    ShippingCode[2] = InitAdress >> 8;
    ShippingCode[3] = InitAdress & 0xff;
    ShippingCode[4] = QuantityRegisters >> 8 & 0xff;
    ShippingCode[5] = QuantityRegisters & 0xff;

    ShippingErrorCheck = ErrorCheck(ShippingCode,sizeof(ShippingCode)-2);
    
    ShippingCode[6] = ShippingErrorCheck >> 8;
    ShippingCode[7] = ShippingErrorCheck & 0xff;

    Serial.println("");
    Serial.print("Pacote de Requisição: ");
    printar(ShippingCode,sizeof(ShippingCode));

    digitalWrite(pinoRS485Comunicacao,HIGH);
    delayMicroseconds(10);
    for (byte i=0; i<sizeof(ShippingCode);i++){
        Serial1.write(ShippingCode[i]);
    }
    delayMicroseconds(10);
    Serial1.flush();
    digitalWrite(pinoRS485Comunicacao,LOW); 
}

uint16_t MODBUS::ErrorCheck(byte pacote[], int tamanho) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < tamanho; pos++) {
        crc ^= (uint16_t)pacote[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

byte* MODBUS::clearPackage(byte* ponteiro, int tamanho) {
  memset(ponteiro, 0, tamanho * sizeof(byte));
  delete[] ponteiro;
  return nullptr;
}

void MODBUS::ReceivePackage(byte QuantityRegisters, unsigned long TimeResponse){
  const int ReceivedPackageSize = 5 + QuantityRegisters * 2;
  ReceivedPackage = new byte[ReceivedPackageSize];
  unsigned long startTime = millis(); // Armazena o tempo atual

  while (Serial.available() < ReceivedPackageSize) { // Aguarda até que todos os dados sejam recebidos
      // Verifica se o tempo limite foi atingido
      if (millis() - startTime >= TimeResponse)  break; // Sai do loop while se o tempo limite foi atingido
      delay(1); // Aguarda um milissegundo antes de verificar novamente
  }

  // Recebe os dados e depois verifica se a quantidade de dados recebidos é diferente do tamanho estabelecido
  // em caso afirmativo o ponteiro que recebe as variáveis é limpo
  if (Serial.readBytes(ReceivedPackage, QuantityRegisters) != QuantityRegisters) {
    ReceivedPackage = clearPackage(ReceivedPackage,ReceivedPackageSize);
  }

  OrganizePkg();
}

void MODBUS::OrganizePkg(){
  byte aplicacao = 0x04; // código da aplicação do sistema de baterias
  int SizeReadyPackage = ReceivedPackage[2];
  ReadyPackage = new byte(SizeReadyPackage); 
  bool valid = validacaoPacote(ReceivedPackage,SizePackage);

  if(valid){ // só entra neste bloco se o CRC do pacote de resposta da UACT CC estiver correto
    ReadyPackage[0] = aplicacao;

    // timestamp
    ReadyPackage[1] = ReceivedPackage[5];
    ReadyPackage[2] = ReceivedPackage[6];
    ReadyPackage[3] = ReceivedPackage[3];
    ReadyPackage[4] = ReceivedPackage[4];
    // variaveis de interesse
    for (byte i = 5; i<41; i=i+4){
      ReadyPackage[i] = ReceivedPackage[i+16]; //   21-5
      ReadyPackage[i+1] = ReceivedPackage[i+17]; // 22-5
      ReadyPackage[i+2] = ReceivedPackage[i+14]; // 19-5
      ReadyPackage[i+3] = ReceivedPackage[i+15]; // 20-5
    }

    // preenchendo com zero as variáveis com zeros
    for (byte i=41; i<SizeReadyPackage; i++){
      ReadyPackage[i] = 0x00;
    }
    }
    else{ // se o pacote não estiver correto reseta o arduino
      Serial.println("Pacote inválido");
      softwareReset::standard();
    }
    ReceivedPackage = clearPackage(ReceivedPackage,SizeReadyPackage);
}

// função booleana que verifica a integridade do pacote, de forma que retorna falso caso a integridade do pacote esteja comprometida
bool MODBUS::validacaoPacote(byte* pacoteRecebido, int tamanho){
  bool valid;
  valid = false;
  uint16_t ErrorCheckRebecido = (pacoteRecebido[tamanho - 1] << 8) | pacoteRecebido[tamanho - 2];
  
  // Serial.print("ErrorCheck Recebido: ");
  // Serial.println(ErrorCheckRebecido,HEX);
  
  uint16_t ErrorCheckCalculado = ErrorCheck(pacoteRecebido,tamanho-2); // calcula o ErrorCheck 
  // Serial.print("ErrorCheck Calculado: ");
  // Serial.println(ErrorCheckCalculado,HEX);

  // compara o ErrorCheck recebido e o ErrorCheck calculado
  if(ErrorCheckRebecido == ErrorCheckCalculado) valid = true; // se forem iguais retorna verdadeiro
  else valid = false; // se não, retorna falso

  return valid;
}


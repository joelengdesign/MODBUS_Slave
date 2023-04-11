// Exemplo para requisitar 2 variáaveis do dispositivo master
// exemplo criado em 11 de abril de 2023

#include <MODBUS.h> // biblioteca MODBUS criada para extração de dados da UACT CC
#include <MODBUSaux.h> // biblioteca MODBUS criada para extração de dados da UACT CC


const int quantVariablesRequest = 2;

// parâmetros do protocolo MODBUS
const byte DeviceAdress = 0x02; // endereço do dispositivo slave
const byte TypeRegisters = 0x04; // código de acesso dos registradores do tipo input
const uint16_t InitAdress = 0x0004; // endereço do registrador inicial
const uint16_t QuantRegisters = 2*0x002; // quantidade de registradores
const int SizeReadyPackage = QuantRegisters*2+5; // quantidade de bytes recebidos
byte ReadyPackage[SizeReadyPackage];
const int ReceivedPackageSize = 5+QuantRegisters*2;
const unsigned long dataTransmission = 115200;

// tempo de espera de 1 byte de acordo com a taxa de baudrate
const float ByteWaitTime = (8*1000/(dataTransmission)*1.5);

// tempo de espera do pacote total
const unsigned long PackageWaitTime = ceil(ByteWaitTime*ReceivedPackageSize);

// variaveis
float var1;
float var2;

const int pinoRS485 = 23; // pino de controle do RS485 para leitura e escrita entre o mestre e o escravo
MODBUS modbusMaster(pinoRS485);

// parâmetros dos dados recebidos da UAC CC
void printarVars(){
    // variaveis
    var1 = modbus.IEEE754_HexToFloat(ReadyPackage,1);
    var2 = modbus.IEEE754_HexToFloat(ReadyPackage,2);

    Serial.print("Var1:");
    Serial.println(TensaoBarramento);

    Serial.print("Var2:");
    Serial.println(Corrente1);
}



void setup() {
  Serial.begin(9600);
  Serial1.begin(dataTransmission);
  modbus.clearPackage(modbus.PackageReceived, ReceivedPackageSize);
}

void loop() {
    modbus.EnviarPacote(DeviceAdress,TypeRegisters,InitAdress,QuantRegisters); // envia a requisição
    modbus.ReceivePackage(QuantRegisters,PackageWaitTime); 
    modbus.OrganizePkg(); // organiza o pacote para transmissão LoRa
    
    memcpy(ReadyPackage, modbus.ReadyPackage, sizeof(ReadyPackage));

    printarVars();

    modbus.clearPackage(modbus.ReadyPackage, ReceivedPackageSize);

    memset(ReadyPackage, 0, sizeof(ReadyPackage));

    delay(5000); // definido pelo projetista
}
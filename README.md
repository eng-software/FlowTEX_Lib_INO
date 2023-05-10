# FlowTEX_Lib_INO
Biblioteca FlowTEX Serial e I2C para IDE Arduino

Validado usando ESP32 ESP-WROOM-32

No exmplo I2C são 2 sensores FlowTEX em paralelo um com endereço 0x20 e outro 0x21 fazendo 1000 leituras por segundo
Sendo SDA no GPIO 17 e SCL no GPIO 5

No exemplo Serial é um sensor FlowTEX fazendo uma leitura por segundo no canal serial 1
Sendo  TX1 o GPIO 26 e  RX1  o GPIO 27


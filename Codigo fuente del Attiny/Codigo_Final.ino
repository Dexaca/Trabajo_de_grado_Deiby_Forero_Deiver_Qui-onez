/* 
 * Author: Deiby Alexander Forero Calderon 2172317, Deiver Yair Quiñonez Osorio 2172312
 * Proyecto: Implementacion de un cargador inalambrico con autenticación.
 * Modificado para ATtiny85 y tarjeta RFID NFC
 */

// Pinout ATtiny85
// Definicion de pines para ATtiny85
#define PN532_SCK (2)   // PB2 (Pin 7)
#define PN532_MOSI (0)  // PB0 (Pin 5)
#define PN532_SS (3)    // PB3 (Pin 2)
#define PN532_MISO (1)  // PB1 (Pin 6)
#define WC (4)          // PB4 (Pin 3)

#include "tinyPN532.h"  //Incluye la librería tinyPN532 (Librería adaptada de adafruitPN532)
uint8_t uid[7];         // Declara un arreglo de 7 bytes para almacenar el UID del RFID
uint8_t uidLength;      // Declara una variable para almacenar el tamaño del UID

// Definición de estructura para UID
struct UIDEntry {
  uint8_t uid[7];     // Declara un arreglo de 7 bytes para almacenar el UID del RFID
  uint8_t length;     // Declara una variable para almacenar el tamaño del UID
};
// Arreglo de UIDs
// Lista de UIDs permitidos
const UIDEntry allowedUIDs[] = {
  { { 0xB3, 0xE9, 0xCF, 0xEE }, 4 },
  { { 0x04, 0xE9, 0xFF, 0x0A, 0x7B, 0x13, 0x90 }, 7 },
  { { 0x04, 0xE9, 0x00, 0x0A, 0x7B, 0x13, 0x94 }, 7 },
  { { 0xA7, 0x09, 0xC9, 0x01 }, 4 }
};
void setup() {

  pinMode(WC, OUTPUT);  //Define el pin del Cargador inalambrico como salida
  delay(300);
  nfc.begin();                                      // Inicializa el PN532
  uint32_t versiondata = nfc.getFirmwareVersion();  // Obtiene la versión de firmware del PN532
  if (!versiondata) {                               // Si no se pudo obtener la versión de firmware
    while (1) {
      delay(200);
    }
  }
  nfc.setPassiveActivationRetries(1);  // Configura reintentos infinitos para la activación pasiva de tarjetas
  nfc.SAMConfig();                     // Configura el tipo de tarjetas a aceptar
}

bool isAllowedUID(const uint8_t* uid, uint8_t uidLen) {
    // Iteración a través del arreglo allowedUIDs
  for (const UIDEntry& entry : allowedUIDs) {
    if (uidLen == entry.length && memcmp(uid, entry.uid, uidLen) == 0) {
      return true; // El UID coincide con uno de los UIDs permitidos
    }
  }
  return false;
}
void loop() {

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    if (isAllowedUID(uid, uidLength)) {
      digitalWrite(WC, HIGH);  // Encender el cargador si el UID es permitido
    } else {
      digitalWrite(WC, LOW);  // Apagar el cargador si el UID NO es permitido
    }
    uidLength = 0;  // Reinicia uidLength a cero después de procesar una tarjeta
  } else {
    digitalWrite(WC, LOW);  // Apaga el cargador si no se detecta el UID
  }
  delay(100);  // Pequeño retraso
}

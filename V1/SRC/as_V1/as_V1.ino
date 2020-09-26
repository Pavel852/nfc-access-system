#include <neotimer.h>
#include <SPI.h>
#include <Keypad.h>
#include <MFRC522.h>
#include "U8glib.h"

Neotimer mytimer = Neotimer(1000);
U8GLIB_SSD1306_128X64 mujOled(U8G_I2C_OPT_NONE);

long int prepis = 0;
unsigned long previousMillis = 0;


const long interval = 10000; 
String zprava;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
byte rowPins[ROWS] = {8,7,6,5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


// definování pinů pro SDA a RST
#define SDA_PIN 10
#define RST_PIN 9

// vytvoření instance RFID čtečky z knihovny
MFRC522 rfid(SDA_PIN, RST_PIN);

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
 SPI.begin();
  // inicializace komunikace s RFID čtečkou
  rfid.PCD_Init();
  
}

 

void loop() {
if (zprava != NULL){
   unsigned long currentMillis = millis();
     if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      zprava = "";
      
}

     }
    
  if (millis()-prepis > 100) {
    // následující skupina příkazů
    // obnoví obsah OLED displeje
    mujOled.firstPage();
    do {
      // funkce vykresli vykreslí žádaný obsah
      vykresli();
    } while( mujOled.nextPage() );
    // uložení posledního času obnovení
    prepis = millis();
  }
  
  char key = keypad.getKey();

  if (key != NO_KEY){
    Serial.println(key);
      mujOled.setPrintPos(0, 25);
      mujOled.print(key);

    }

  // put your main code here, to run repeatedly:

  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  // kontrola správného přečtení RFID tagu
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  // výpis informace o verzi RFID tagu
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print("RFID tag typu: ");
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // kontrola podporovaných typů RFID tagu
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println("Tento RFID tag neni podporovany (typ MIFARE Classic).");
    return;
  }
  
  // výpis adresy RFID tagu v hexa formátu
  Serial.print("Adresa RFID tagu: ");
  vypisHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  if(rfid.uid.uidByte[0] == 0xF9 & rfid.uid.uidByte[1] == 0x00 & rfid.uid.uidByte[2] == 0xBF & rfid.uid.uidByte[3] == 0x83) {

    Serial.println("Vitej!");
    zprava = "Vitejte!";
    
  }


  /* ukázka přidání dalšího tagu
  else if(rfid.uid.uidByte[0] == 0x?? & rfid.uid.uidByte[1] == 0x?? & rfid.uid.uidByte[2] == 0x?? & rfid.uid.uidByte[3] == 0x??) {
    Serial.println("Detekovan novy tag ..!");
  } 
   */
  else {
    Serial.println("Zamitnuto!");
      zprava = "Zamitnuto!";
  }
  Serial.println();
  // ukončení komunikace a jejího zabezpečení
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  


  
}
// podprogram pro výpis adresy RFID tagu v hexa formátu
void vypisHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  
  }

   void vykresli(void) {
  // nastavení písma, toto písmo umožní vypsat
  // přibližně 15x4 znaků
  mujOled.setFont(u8g_font_unifont);
  // nastavení pozice výpisu v pixelech
  // souřadnice jsou ve tvaru x, y
  // souřadnice 0, 0 je v levém horní rohu
  // OLED displeje, maximum je 128, 64
  mujOled.setPrintPos(0, 10);
  // výpis textu na zadanou souřadnici
  mujOled.print("SPSE Zatec");

  mujOled.setPrintPos(0, 30);
  if (zprava != NULL){
    mujOled.print(zprava);
    
}
  else{
    mujOled.print("Prilozte kartu!");
    }
    
 
  }

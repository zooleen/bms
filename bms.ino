/*
 *  This Sketch will putting commands to LTC3300-2 (addressable) via Arduino SPI port.
 *
 *  Message format is: 1 byte - address+command, 2 byte - 12 bit of data and 4 bit of CRC-4
 *
 *  Copyright Tesla Telecom ltd. 2020
 *
 */

#include <Arduino.h>
#include <stdint.h>
//#include "LT_SPI.h"
#include "LTC68042.h"
#include <SPI.h>

#include "crc4.c"

/*
 * -------------------------------------------------------------
 *
 *  ____  __  __  ___  _  _  ____  _  _     ___  __  __  ____
 * ( ___)(  )(  )/ __)( )/ )(_  _)( \( )   / __)(  \/  )(  _ \
 *  )__)  )(__)(( (__  )  (  _)(_  )  (   ( (__  )    (  )(_) )
 * (__)  (______)\___)(_)\_)(____)(_)\_)   \___)(_/\/\_)(____/
 *
 * -------------------------------------------------------------
 */

unsigned int Crc4(unsigned int in_m);
 
const uint8_t CSB = 10;

const byte writeCommand   =   0b10100000;
const byte readCommand    =   0b10100011;
const byte readStCommand  =   0b10100101;
const byte runCommand     =   0b10100110;

uint16_t writeDataRaw = 0b001100001000; // second discharge, fourth charge
uint16_t crc = Crc4(writeDataRaw << 4); 

uint16_t writeData;

uint16_t a[16];

void setup()
{
  writeData = (writeDataRaw << 4) | crc;

  SPCR |= MSTR;

  Serial.begin(115200);
  SPI.beginTransaction(SPISettings(10000, MSBFIRST, SPI_MODE3));
  SPI.begin();

  pinMode(CSB, OUTPUT);
  pinMode(12, INPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
}

void loop(void)
{
  Serial.print("Calculated writeData is: ");
  Serial.print(writeData, BIN);
  Serial.print("\n");


  // Write Balance Command
  Serial.println("Write command...");


  // sending 2 bytes of CMD (Addr + CC)
  digitalWrite(CSB, LOW);

  SPI.transfer(writeCommand);
  SPI.transfer(byte(writeData >> 8));
  SPI.transfer(byte(writeData));

  digitalWrite(CSB, HIGH);

for (int i =0 ; i < 3; i++)
{
  // Readback command
  Serial.println("Readback command..."); 
  digitalWrite(CSB, LOW);
  
  SPI.transfer(readCommand);
  Serial.print(SPI.transfer(0x00), BIN);
  Serial.print("\t");
  Serial.print(SPI.transfer(0x00), BIN);
  Serial.print("\t\n");

  digitalWrite(CSB, HIGH);
  delay(10);
}


for (int i =0 ; i < 3; i++)
{
  // Read Status command
  Serial.println("Readback command..."); 
  digitalWrite(CSB, LOW);
  
  SPI.transfer(readStCommand);
  Serial.print(SPI.transfer(0x00), BIN);
  Serial.print("\t");
  Serial.print(SPI.transfer(0x00), BIN);
  Serial.print("\t\n");

  digitalWrite(CSB, HIGH);
  delay(10);
}

  // Execute balance command 25 times for 1 second
Serial.println("Doing charge 25 sec...");

  for (int i = 0; i < 25; i++)
  {
    digitalWrite(CSB, LOW);
    SPI.transfer(runCommand);
      digitalWrite(CSB, HIGH);
    delay(500);
  }

}


/*
 * -------------------------------------------------------------
 *
 *  ____  __  __  ___  _  _  ____  _  _    ____  ____  ___
 * ( ___)(  )(  )/ __)( )/ )(_  _)( \( )  (  _ \( ___)/ __)
 *  )__)  )(__)(( (__  )  (  _)(_  )  (    )___/ )__)( (__
 * (__)  (______)\___)(_)\_)(____)(_)\_)  (__)  (____)\___)
 *
 * -------------------------------------------------------------
 */

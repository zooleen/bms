/*
 * This module has initialize LTC6804-2 and putting command to them via IsoSPI
 *
 * (isoSPI realized by DC2617a shield)
 *
 *
 */

#include <Arduino.h>
#include <stdint.h>
//#include "LT_SPI.h"
#include "LTC68042.h"
#include <SPI.h>

/*
  -------------------------------------------------------------

   ____  __  __  ___  _  _  ____  _  _     ___  __  __  ____
  ( ___)(  )(  )/ __)( )/ )(_  _)( \( )   / __)(  \/  )(  _ \
   )__)  )(__)(( (__  )  (  _)(_  )  (   ( (__  )    (  )(_) )
  (__)  (______)\___)(_)\_)(____)(_)\_)   \___)(_/\/\_)(____/

  -------------------------------------------------------------
*/
const uint8_t CSB = 10;

byte CMD[2] = {0x00, 0x00};


bool toAddress; // it is addressable message?
uint8_t addr; // four address bits
uint16_t command; // eleven command bits
uint16_t summary = 0x00;

uint16_t a[16];

void setup()
{
  SPCR |= MSTR;

  Serial.begin(115200);
  SPI.beginTransaction(SPISettings(20000, MSBFIRST, SPI_MODE3));
  SPI.begin();

  pinMode(CSB, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);



}

void loop(void)
{
  toAddress = 0; // it is addressable message?
  addr = 0b0000; // four address bits - A[3:0]
  command = 0b00000001000; // eleven command bits - CC[10:0]

  /*
          Broadcast or Addressed[15], address[14:11], Command[10:0] -- 16 total
  */
  summary |= (toAddress << 15) | (addr << 11) | (command << 0); // split all components together

  CMD[0] = summary >> 8;
  CMD[1] = summary;

  // the 'summary' is determined, so let's calculate it's PEC
  uint16_t pecCmd = pec15_calc(2, CMD);

  byte PEC[2] = {(pecCmd >> 8), (pecCmd)};
  Serial.print("SPCR is: \t");
  Serial.print(SPCR, BIN);
  Serial.println();

  Serial.print("SPDR is: \t");
  Serial.print(SPDR, BIN);
  Serial.println();

  Serial.print("SPSR is: \t");
  Serial.print(SPSR, BIN);
  Serial.println();

  Serial.print("CMD is: ");
  Serial.print(CMD[0], BIN);
  Serial.print("\t");
  Serial.print(CMD[1], BIN);
  Serial.println();

  Serial.print("PEC is: ");
  Serial.print(PEC[0], BIN);
  Serial.print("\t");
  Serial.print(PEC[1], BIN);
  Serial.println();
  delay(1500);
  while (1)
  {
    for ( uint16_t i = 0; i < 50; i++)
    {

      // sending 2 bytes of CMD (Addr + CC)
      digitalWrite(CSB, LOW);
      delayMicroseconds(75);

      a[0] = SPI.transfer(CMD[0]);
      a[1] = SPI.transfer(CMD[1]);

      // sending 2 bytes of CMD's PEC
      a[2] = SPI.transfer(PEC[0]);
      a[3] = SPI.transfer(PEC[1]);

      delayMicroseconds(75);
      digitalWrite(CSB, HIGH);

      for (uint8_t i = 0; i < 4; i++)
      {
        Serial.println(a[i]);
      }


      /*

          // sending 1 byte of message lenght
          byte lenght = 0x06;
          SPI.transfer(lenght);

          SPI.transfer(0x1A);
          SPI.transfer(0xC7);
          SPI.transfer(0x77);
          SPI.transfer(0x1A);
          SPI.transfer(0xC7);
          SPI.transfer(0x77);
      */

      //delayMicroseconds(75);



    }


    //Serial.print(SPI.transfer(0x1C));
  }
}

/*
  -------------------------------------------------------------

   ____  __  __  ___  _  _  ____  _  _    ____  ____  ___
  ( ___)(  )(  )/ __)( )/ )(_  _)( \( )  (  _ \( ___)/ __)
   )__)  )(__)(( (__  )  (  _)(_  )  (    )___/ )__)( (__
  (__)  (______)\___)(_)\_)(____)(_)\_)  (__)  (____)\___)

  -------------------------------------------------------------
*/

/* How-to calculate this shit? Answer: including external library. */

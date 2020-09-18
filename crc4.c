/*
 *  Calculate CRC-4 for 16 bits of data
 *
 *  Author: Constantine Pinjenin
 *
 *  Copyright Tesla Telecom Ltd. 2020
 */

unsigned int Crc4(unsigned int in_m)
{
  for (int i = 0; i < 12; i++)
  {
    if (in_m & 0x8000)
      in_m ^= (0x13 << 11);
    in_m = in_m << 1;
  }
  in_m = in_m >> 12;
  return in_m ^= 0xF; //
}

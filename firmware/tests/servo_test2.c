/*
 * Test servo motora SG90, fini koraci: gdje servo prestaje reagovati
 * Ploča: EasyPIC Fusion v7, dsPIC33FJ256GP710A, mikroC PRO for dsPIC
 * Takt projekta 8 MHz. Biblioteke nisu potrebne.
 *
 * Spajanje servoa:
 *   smeđa (ili crna)       -> GND (zajednički sa EasyPIC-om)
 *   crvena                 -> +5V sa JoyCar zaglavlja
 *   narandžasta (ili žuta) -> RD0 na EasyPIC-u
 *
 * Program ide od jednog kraja do drugog u 9 koraka, svaki traje 3 s.
 * LED diode (PORTB) pokazuju broj koraka (1-9) binarno:
 *   korak 1 = 0.50 ms    korak 2 = 0.75 ms    korak 3 = 1.00 ms
 *   korak 4 = 1.25 ms    korak 5 = 1.50 ms    korak 6 = 1.75 ms
 *   korak 7 = 2.00 ms    korak 8 = 2.25 ms    korak 9 = 2.50 ms
 * Zatim RB7: povratak na sredinu (1.50 ms), pa sve ispočetka.
 *
 * Zapisati: pri kojim koracima se servo pomjeri, a pri kojim stoji.
 */

#define SIG     LATD0_bit
#define SIG_DIR TRISD0_bit

// Jedan impuls zadate širine, pa pauza do ukupno oko 20 ms
void impuls(char korak) {
  SIG = 1;
  switch (korak) {
    case 0: Delay_us(500);  break;
    case 1: Delay_us(750);  break;
    case 2: Delay_us(1000); break;
    case 3: Delay_us(1250); break;
    case 4: Delay_us(1500); break;
    case 5: Delay_us(1750); break;
    case 6: Delay_us(2000); break;
    case 7: Delay_us(2250); break;
    case 8: Delay_us(2500); break;
  }
  SIG = 0;
  Delay_ms(18);
}

// Drži položaj oko 3 s (150 impulsa)
void drzi(char korak, unsigned char led) {
  unsigned char i;
  LATB = led;
  for (i = 0; i < 150; i++) impuls(korak);
}

void main() {
  char k;

  AD1PCFGL = 0xFFFF;
  AD1PCFGH = 0xFFFF;
  TRISB = 0x0000;
  LATB  = 0x00;
  SIG_DIR = 0;
  SIG = 0;
  Delay_ms(1000);

  while (1) {
    for (k = 0; k <= 8; k++) drzi(k, k + 1);   // koraci 1-9
    drzi(4, 0x80);                             // sredina, RB7
  }
}

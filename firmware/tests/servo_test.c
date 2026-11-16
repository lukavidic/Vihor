/*
 * Test servo motora SG90 sa PWM signalom od 3.3 V
 * Ploča: EasyPIC Fusion v7, dsPIC33FJ256GP710A, mikroC PRO for dsPIC
 * Takt projekta 8 MHz. Biblioteke nisu potrebne.
 *
 * Spajanje servoa (tri žice):
 *   smeđa (ili crna)       -> GND (zajednički sa EasyPIC-om i izvorom napajanja)
 *   crvena                 -> +5V sa JoyCar zaglavlja (NE na 3.3 V sa EasyPIC-a)
 *   narandžasta (ili žuta) -> RD0 na EasyPIC-u (signal, 3.3 V)
 *
 * Program šalje impulse svakih 20 ms. Širina impulsa određuje položaj.
 * Svaki korak traje 3 s. LED diode (PORTB) pokazuju korak:
 *   RB0 = 1.5 ms (sredina)
 *   RB1 = 1.0 ms
 *   RB2 = 2.0 ms
 *   RB3 = 1.5 ms (sredina)
 *   RB4 = 0.5 ms (krajnji položaj, prošireni opseg)
 *   RB5 = 2.5 ms (drugi krajnji položaj, prošireni opseg)
 *   RB6 = 1.5 ms (sredina), pa sve ispočetka
 *
 * Ako servo u koracima RB4 ili RB5 zuji i ne može doći do položaja,
 * to je preko njegovog mehaničkog kraja: zapiši i prekini test.
 */

#define SIG     LATD0_bit
#define SIG_DIR TRISD0_bit

// Jedan impuls zadate širine, pa pauza do ukupno oko 20 ms
void impuls(char korak) {
  SIG = 1;
  switch (korak) {
    case 0: Delay_us(500);  break;
    case 1: Delay_us(1000); break;
    case 2: Delay_us(1500); break;
    case 3: Delay_us(2000); break;
    case 4: Delay_us(2500); break;
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
  AD1PCFGL = 0xFFFF;
  AD1PCFGH = 0xFFFF;
  TRISB = 0x0000;
  LATB  = 0x00;
  SIG_DIR = 0;
  SIG = 0;
  Delay_ms(1000);

  while (1) {
    drzi(2, 0x01);   // 1.5 ms
    drzi(1, 0x02);   // 1.0 ms
    drzi(3, 0x04);   // 2.0 ms
    drzi(2, 0x08);   // 1.5 ms
    drzi(0, 0x10);   // 0.5 ms
    drzi(4, 0x20);   // 2.5 ms
    drzi(2, 0x40);   // 1.5 ms
  }
}

/*
 * Test JoyCar motora preko I2C (PWM kontroler na adresi 0x70)
 * Ploča: EasyPIC Fusion v7, MCU dsPIC33FJ256GP710A, mikroC PRO for dsPIC
 * U Library Manageru uključi biblioteku I2C.
 *
 * VOZILO PODIGNI SA STOLA (točkovi u zraku) prije uključivanja!
 *
 * Spajanje (micro:bit NE smije biti u utoru):
 *   SCL1 (na ovom MCU-u RG2, provjeri oznaku na ploči) -> pin SCL na JoyCar zaglavlju
 *   SDA1 (na ovom MCU-u RG3, provjeri oznaku na ploči) -> pin SDA na JoyCar zaglavlju
 *   GND EasyPIC-a -- GND JoyCar-a
 *   Bez dodatnih otpornika: JoyCar već ima pull-up na 3.3 V.
 *
 * Šta program radi:
 *   Redom uključuje po jedan PWM kanal (registri 0x02, 0x03, 0x04, 0x05)
 *   na 1.5 s, pa pauza 1.5 s. LED diode (PORTB) pokazuju koji je kanal aktivan:
 *     RB0 = registar 0x02, RB1 = 0x03, RB2 = 0x04, RB3 = 0x05
 *   RB7 upaljen = I2C greška (uređaj nije odgovorio).
 *
 * Za svaki kanal zapiši: koji točak se vrti (lijevi/desni) i u kom smjeru.
 */

#define LED_TRIS  TRISB
#define LED_LAT   LATB

#define PWM_ADDR_W  0xE0   // adresa 0x70 pomjerena ulijevo, bit pisanja = 0
#define BRZINA      120    // 0-255

unsigned greska = 0;

void pwm_write(unsigned char reg, unsigned char val) {
  I2C1_Start();
  if (I2C1_Write(PWM_ADDR_W)) greska = 1;
  if (I2C1_Write(reg))        greska = 1;
  if (I2C1_Write(val))        greska = 1;
  I2C1_Stop();
  Delay_ms(2);
}

void svi_stop() {
  pwm_write(0x02, 0);
  pwm_write(0x03, 0);
  pwm_write(0x04, 0);
  pwm_write(0x05, 0);
}

void main() {
  unsigned char i;

  AD1PCFGL = 0xFFFF;   // svi pinovi digitalni
  AD1PCFGH = 0xFFFF;
  LED_TRIS = 0x0000;
  LED_LAT  = 0x00;

  I2C1_Init(100000);   // 100 kHz za test (JoyCar primjer koristi 400 kHz)
  Delay_ms(500);

  // Inicijalizacija PWM kontrolera (iz hwsw-interface.md, odjeljak 4.1)
  pwm_write(0x00, 0x01);   // MODE1: normalan rad
  pwm_write(0xE8, 0xAA);   // LEDOUT: sva 4 izlaza pod PWM kontrolom
  svi_stop();

  while (1) {
    for (i = 0; i < 4; i++) {
      LED_LAT = (1 << i) | (greska ? 0x80 : 0x00);
      pwm_write(0x02 + i, BRZINA);
      Delay_ms(1500);
      svi_stop();
      LED_LAT = greska ? 0x80 : 0x00;
      Delay_ms(1500);
    }
  }
}

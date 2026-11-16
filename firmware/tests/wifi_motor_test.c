/*
 * Test: WiFi i motori zajedno (START pokreće motore, STOP ih zaustavlja)
 * Ploča: EasyPIC Fusion v7, dsPIC33FJ256GP710A, mikroC PRO for dsPIC
 * Biblioteke: UART i I2C. Takt projekta 8 MHz.
 *
 * VOZILO DRŽI PODIGNUTO, točkovi u zraku.
 *
 * Spajanje:
 *   ESP-01: TX -> RF4, RX -> RF5, VCC/EN/RST -> 3.3 V, GND. Modul je na 9600 baud.
 *   JoyCar: SCL1 (RG2) -> SCL, SDA1 (RG3) -> SDA, GND -- GND.
 *   SW12 prekidači 1 i 2 na OFF.
 *
 * Na računaru pokrenuti: python tcp_server2.py
 *
 * LED diode (PORTB):
 *   RB0 = ESP odgovara        RB1 = spojen na WiFi
 *   RB2 = TCP veza            RB3 = poslan prvi log
 *   RB4 = primljen START, motori rade
 *   RB5 = primljen STOP, motori stali
 *   RB7 = greška na koraku iza zadnje upaljene LED
 */

// ----------- PODESI OVO -----------
#define WIFI_SSID   "ime_mreze"
#define WIFI_PASS   "lozinka"
#define PC_IP       "192.168.8.2"
#define PC_PORT     "8084"
// ----------------------------------

#define LED_LAT     LATB
#define PWM_ADDR_W  0xE0
#define BRZINA      150

char wait_for(char *s, unsigned int timeout_ms) {
  unsigned int i = 0;
  unsigned long t = 0, limit;
  char c;
  limit = (unsigned long)timeout_ms * 20;
  while (t < limit) {
    if (U2STA.F1) U2STA.F1 = 0;
    if (UART2_Data_Ready()) {
      c = UART2_Read();
      if (c == s[i]) i++;
      else i = (c == s[0]) ? 1 : 0;
      if (s[i] == 0) return 1;
    } else { Delay_us(50); t++; }
  }
  return 0;
}

void pwm_write(unsigned char reg, unsigned char val) {
  I2C1_Start();
  I2C1_Write(PWM_ADDR_W);
  I2C1_Write(reg);
  I2C1_Write(val);
  I2C1_Stop();
  Delay_ms(2);
}

void motori(unsigned char rn, unsigned char rf, unsigned char ln, unsigned char lf) {
  pwm_write(0x02, rn);
  pwm_write(0x03, rf);
  pwm_write(0x04, ln);
  pwm_write(0x05, lf);
}

void greska() {
  motori(0, 0, 0, 0);
  LED_LAT = LED_LAT | 0x80;
  while (1) ;
}

// Šalje jedan red loga preko TCP veze. Vraća 1 ako je modul potvrdio slanje.
char posalji(char *msg) {
  unsigned char n = 0;
  while (msg[n]) n++;
  UART2_Write_Text("AT+CIPSEND=");
  UART2_Write('0' + (n / 10));
  UART2_Write('0' + (n % 10));
  UART2_Write_Text("\r\n");
  if (!wait_for(">", 3000)) return 0;
  UART2_Write_Text(msg);
  return wait_for("SEND OK", 5000);
}

void main() {
  AD1PCFGL = 0xFFFF;
  AD1PCFGH = 0xFFFF;
  TRISB = 0x0000;
  LED_LAT = 0x00;

  // UART prema ESP-01: 9600 baud na taktu 8 MHz
  UART2_Init(9600);
  U2MODE.F15 = 0;
  U2MODE.F3  = 1;
  U2BRG      = 103;
  U2MODE.F15 = 1;
  U2STA.F10  = 1;

  // I2C prema JoyCar ploči
  I2C1_Init(100000);
  Delay_ms(500);
  pwm_write(0x00, 0x01);
  pwm_write(0xE8, 0xAA);
  motori(0, 0, 0, 0);

  Delay_ms(2000);

  UART2_Write_Text("AT\r\n");
  if (!wait_for("OK", 2000)) greska();
  LED_LAT = 0x01;

  UART2_Write_Text("AT+CWMODE=1\r\n");
  if (!wait_for("OK", 2000)) greska();

  UART2_Write_Text("AT+CWJAP=\"");
  UART2_Write_Text(WIFI_SSID);
  UART2_Write_Text("\",\"");
  UART2_Write_Text(WIFI_PASS);
  UART2_Write_Text("\"\r\n");
  if (!wait_for("OK", 20000)) greska();
  LED_LAT = 0x03;

  UART2_Write_Text("AT+CIPSTART=\"TCP\",\"");
  UART2_Write_Text(PC_IP);
  UART2_Write_Text("\",");
  UART2_Write_Text(PC_PORT);
  UART2_Write_Text("\r\n");
  if (!wait_for("OK", 10000)) greska();
  LED_LAT = 0x07;

  if (!posalji("[0.0s] INFO SYS: spreman\r\n")) greska();
  LED_LAT = 0x0F;

  // Čekanje komande START (do 60 s)
  if (!wait_for("START", 60000)) greska();
  motori(0, BRZINA, 0, BRZINA);
  LED_LAT = 0x1F;
  posalji("[0.0s] INFO NAV: STATE=DRIVE\r\n");

  // Čekanje komande STOP (do 60 s); bez nje se motori svejedno gase
  if (!wait_for("STOP", 60000)) greska();
  motori(0, 0, 0, 0);
  LED_LAT = 0x3F;
  posalji("[0.0s] INFO NAV: STATE=IDLE\r\n");

  while (1) ;
}

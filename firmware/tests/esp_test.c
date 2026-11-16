/*
 * Test ESP-01: AT komande, spajanje na WiFi, TCP veza sa računarom
 * Ploča: EasyPIC Fusion v7, MCU dsPIC33FJ256GP710A, mikroC PRO for dsPIC
 * U Library Manageru uključi biblioteku UART.
 *
 * Spajanje ESP-01 (JoyCar nije potreban):
 *   ESP VCC   -> 3.3 V   (NIKAKO 5 V)
 *   ESP EN    -> 3.3 V   (CH_PD, bez toga modul ne radi)
 *   ESP GND   -> GND
 *   ESP RX    <- U2TX (na ovom MCU-u RF5, provjeri u mikroC UART primjeru)
 *   ESP TX    -> U2RX (na ovom MCU-u RF4, provjeri u mikroC UART primjeru)
 *
 * LED diode (PORTB) pokazuju dokle je test stigao:
 *   RB0 = ESP odgovara na AT
 *   RB1 = station mod postavljen
 *   RB2 = spojen na WiFi
 *   RB3 = TCP veza sa računarom
 *   RB4 = poruka poslana
 *   RB5 = primljena komanda START sa računara
 *   RB7 = greška na koraku koji slijedi iza zadnje upaljene LED
 */

// ----------- PODESI OVO -----------
#define WIFI_SSID   "ime_mreze"
#define WIFI_PASS   "lozinka"
#define PC_IP       "192.168.1.100"
#define PC_PORT     "8084"
// ----------------------------------

#define LED_TRIS  TRISB
#define LED_LAT   LATB

// Čeka da se u prijemu pojavi tekst s. Vraća 1 ako se pojavio, 0 ako je isteklo vrijeme.
char wait_for(char *s, unsigned int timeout_ms) {
  unsigned int i = 0;
  unsigned long t = 0;
  unsigned long limit;
  char c;

  limit = (unsigned long)timeout_ms * 20;   // koraci od 50 us
  while (t < limit) {
    if (U2STA.F1) U2STA.F1 = 0;             // OERR: očisti prekoračenje prijema
    if (UART2_Data_Ready()) {
      c = UART2_Read();
      if (c == s[i]) i++;
      else i = (c == s[0]) ? 1 : 0;
      if (s[i] == 0) return 1;
    } else {
      Delay_us(50);
      t++;
    }
  }
  return 0;
}

void greska() {
  LED_LAT = LED_LAT | 0x80;
  while (1) ;
}

void main() {
  AD1PCFGL = 0xFFFF;
  AD1PCFGH = 0xFFFF;
  LED_TRIS = 0x0000;
  LED_LAT  = 0x00;

  UART2_Init(115200);
  Delay_ms(2000);                 // ESP-01 se pokreće

  // 1) Da li ESP odgovara
  UART2_Write_Text("AT\r\n");
  if (!wait_for("OK", 2000)) greska();
  LED_LAT = 0x01;

  // 2) Station mod
  UART2_Write_Text("AT+CWMODE=1\r\n");
  if (!wait_for("OK", 2000)) greska();
  LED_LAT = 0x03;

  // 3) Spajanje na WiFi (može trajati do 20 s)
  UART2_Write_Text("AT+CWJAP=\"");
  UART2_Write_Text(WIFI_SSID);
  UART2_Write_Text("\",\"");
  UART2_Write_Text(WIFI_PASS);
  UART2_Write_Text("\"\r\n");
  if (!wait_for("OK", 20000)) greska();
  LED_LAT = 0x07;

  // 4) TCP veza prema računaru
  UART2_Write_Text("AT+CIPSTART=\"TCP\",\"");
  UART2_Write_Text(PC_IP);
  UART2_Write_Text("\",");
  UART2_Write_Text(PC_PORT);
  UART2_Write_Text("\r\n");
  if (!wait_for("OK", 10000)) greska();
  LED_LAT = 0x0F;

  // 5) Slanje jedne log poruke (27 znakova)
  UART2_Write_Text("AT+CIPSEND=27\r\n");
  if (!wait_for(">", 3000)) greska();
  UART2_Write_Text("[0.0s] INFO SYS: ESP test\r\n");
  if (!wait_for("SEND OK", 5000)) greska();
  LED_LAT = 0x1F;

  // 6) Čekanje komande START sa računara (do 60 s)
  if (!wait_for("START", 60000)) greska();
  LED_LAT = 0x3F;

  while (1) ;
}

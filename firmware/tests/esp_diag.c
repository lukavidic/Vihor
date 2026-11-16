/*
 * Dijagnostika ESP-01: na kojoj brzini odgovara i da li se resetuje
 * Ploča: EasyPIC Fusion v7, MCU dsPIC33FJ256GP710A, mikroC PRO for dsPIC
 * Biblioteka: UART. Spajanje isto kao za esp_test.c (U2TX -> ESP RX, U2RX <- ESP TX).
 *
 * Rezultat na LED diodama (PORTB):
 *   RB0 = ESP odgovara na AT pri 115200
 *   RB1 = ESP odgovara na AT pri 9600
 *   RB2 = ESP odgovara na AT pri 57600
 *   RB3 = ESP odgovara na AT pri 38400
 *
 *   Ako ne odgovara ni na jednoj brzini, program sluša poruke pri resetu (74880):
 *   RB7 + RB4       = modul se resetuje, uzrok 1 (uključenje napajanja)
 *   RB7 + RB5       = uzrok 2 (vanjski reset, pin RST)
 *   RB7 + RB4 + RB5 = uzrok 3 (softverski reset)
 *   RB7 + RB6       = uzrok 4 (watchdog)
 *   samo RB7        = modul šalje nešto, ali nije poruka o resetu
 *   RB6 + RB7 trepću = modul ne šalje ništa što možemo pročitati
 */

#define LED_TRIS  TRISB
#define LED_LAT   LATB

char zadnji = 0;   // znak koji je stigao odmah iza traženog teksta

char wait_for(char *s, unsigned int timeout_ms, char uzmi_sljedeci) {
  unsigned int i = 0;
  unsigned long t = 0;
  unsigned long limit;
  char c;
  char nadjen = 0;

  limit = (unsigned long)timeout_ms * 20;   // koraci od 50 us
  while (t < limit) {
    if (U2STA.F1) U2STA.F1 = 0;             // očisti prekoračenje prijema
    if (UART2_Data_Ready()) {
      c = UART2_Read();
      if (nadjen) { zadnji = c; return 1; }
      if (c == s[i]) i++;
      else i = (c == s[0]) ? 1 : 0;
      if (s[i] == 0) {
        if (!uzmi_sljedeci) return 1;
        nadjen = 1;
      }
    } else {
      Delay_us(50);
      t++;
    }
  }
  return nadjen;
}

char probaj_at() {
  Delay_ms(200);
  UART2_Write_Text("AT\r\n");
  return wait_for("OK", 1500, 0);
}

void main() {
  AD1PCFGL = 0xFFFF;
  AD1PCFGH = 0xFFFF;
  LED_TRIS = 0x0000;
  LED_LAT  = 0x00;

  Delay_ms(3000);            // ESP se pokreće

  UART2_Init(115200);
  if (probaj_at()) { LED_LAT = 0x01; while (1) ; }

  UART2_Init(9600);
  if (probaj_at()) { LED_LAT = 0x02; while (1) ; }

  UART2_Init(57600);
  if (probaj_at()) { LED_LAT = 0x04; while (1) ; }

  UART2_Init(38400);
  if (probaj_at()) { LED_LAT = 0x08; while (1) ; }

  // Ne odgovara na AT. Slušamo poruke koje ESP šalje pri resetu (74880 baud).
  UART2_Init(74880);
  if (wait_for("rst cause:", 8000, 1)) {
    LED_LAT = 0x80 | ((zadnji - '0') << 4);
    while (1) ;
  }

  // Šalje li išta na 74880?
  if (wait_for("e", 3000, 0)) { LED_LAT = 0x80; while (1) ; }

  while (1) {
    LED_LAT = 0xC0; Delay_ms(300);
    LED_LAT = 0x00; Delay_ms(300);
  }
}

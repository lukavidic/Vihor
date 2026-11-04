# Interfejs sistema (HW/SW)
 
**Projekat:** Autonomna mobilna platforma sa integrisanim elektromagnetskim topom
**Verzija:** 0.1 (nacrt)
**Svrha:** Zajednički dogovor hardverskog i softverskog tima: napajanje, pinovi mikrokontrolera, konektori i komunikacioni protokol. Svaka promjena pinova ili protokola ide prvo kroz izmjenu ovog dokumenta, uz saglasnost oba tima.
 
Vrijednosti označene sa **TBD** treba potvrditi (profesor, JoyCar dokumentacija, datasheet).
 
---
 
## 1. Ključne komponente
 
| Komponenta | Izbor | Status |
|---|---|---|
| Mikrokontroler | PIC24FJ64GA702 (SOIC-28) | Gotovo potvrđeno, moguć sličan (npr. GA002) |
| WiFi modul | ESP8266 (ESP-01), AT komande preko UART-a | Potvrđeno |
| Senzor za prepreke | HC-SR04 (ultrazvučni) | Potvrđeno |
| Senzor za metu | HC-SR04 ili IR modul (npr. IR distance click) | **TBD** (profesor) |
| Motori | JoyCar, sa JoyCar pločom koja sadrži drajvere | Interfejs **TBD** |
| Napajanje | 4 × 1.5 V AA baterije | Potvrđeno |
| Top | Elektromagnetski top | Još nije dostupan, rezervisani pinovi |
 
---
 
## 2. Napajanje
 
**Jedini izvor napajanja za cijeli sistem su 4 × 1.5 V AA baterije** (motori i JoyCar ploča, logika, senzori, WiFi i top).
 
| Napon | Izvor | Potrošači | Procjena struje |
|---|---|---|---|
| VBAT, oko 4.0–6.4 V (4 × AA, zavisno od napunjenosti) | Baterije | JoyCar ploča i motori (direktno iz VBAT), ulaz svih regulatora na našoj ploči | Motori: **TBD** (nominalno i blokirano) |
| 3.3 V | LDO sa malim padom napona (npr. AP2112, MCP1700) ili buck | PIC, ESP8266, IR modul (ako je 3.3 V) | PIC oko 30 mA, ESP8266 do oko 300 mA (vršno), ukupno planirati ≥ 500 mA |
| 5 V | Boost konvertor (ako je potreban) | HC-SR04 (radi na 5 V, oko 15 mA po senzoru) | oko 30–40 mA |
| Napon topa | Boost konvertor/punjač kondenzatora na našoj ploči, iz VBAT | Kondenzatori i zavojnica topa | Ograničena struja punjenja, **TBD** |
 
### 2.1 Ograničenja baterije
 
- 4 × AA daju malo napona kad se isprazne (oko 4 V), pa regulator za 3.3 V mora imati mali pad napona (npr. AP2112, MCP1700). Klasični LM1117 ima pad oko 1.2 V i bio bi na granici.
- AA baterije imaju veliku unutrašnju otpornost (ukupno oko 1 Ω za četiri alkalne). Pri velikoj struji (pokretanje ili blokiranje motora, punjenje topa, WiFi šiljci) napon baterije pada, pa se PIC i ESP8266 mogu resetovati (brownout). Zajedničko napajanje je zato najveći rizik sistema.
### 2.2 Mjere zaštite: hardver
 
- Veliki kondenzator (npr. 470–1000 µF) na VBAT, dodatnih 100–470 µF uz ESP8266 i 100 nF uz svaki pin napajanja.
- Punjač topa ima ograničenu struju punjenja (oko 0.3–0.5 A, **TBD**), da ne sruši napon baterije.
- Logika (3.3 V) ima vlastiti regulator dovoljnog kapaciteta, a razvod motora i topa je fizički odvojen od logike, uz mase spojene u jednoj tački.
- Mjerenje napona baterije (ADC, pin RA1) za zaštitu.
- Preklopnik (ili konektor) za isključenje cijelog napajanja.
- Opciono: dioda ili preklopnik za razdvajanje, ako se pokaže da motori ruše napon.
### 2.3 Mjere zaštite: firmver
 
- Top se puni samo kad motori nisu pod velikim opterećenjem (npr. dok vozilo stoji u TRACK).
- Punjenje se prekida ako napon baterije padne ispod praga (**TBD**).
- Pri niskom naponu baterije vozilo se zaustavlja, šalje `WARN` log, a top se razoružava.
### 2.4 Testiranje prije konačnog dizajna
 
- Izmjeriti pad napona baterije pod opterećenjem (motori JoyCar-a, WiFi, punjenje topa).
- Uporediti sa punjivim NiMH AA baterijama (niža unutrašnja otpornost, oko 4.8 V) i svježim alkalnim.
---
 
## 3. Tabela pinova mikrokontrolera (PIC24FJ64GA702, SOIC-28)
 
Raspodjela je usklađena sa datasheetom (tabela funkcija pinova). PIC ima 21 I/O pin (RA0–RA4 i RB0–RB15), a u ovoj raspodjeli su svi zauzeti ili rezervisani, pa **nema slobodnog prostora** za nove funkcije. Ako se pojavi potreba za još pinova, treba razmisliti o većem PIC-u iz iste porodice.
 
| Fizički pin | Port | Funkcija | Smjer | Posebne mogućnosti pina | Napomena |
|---|---|---|---|---|---|
| 1 | MCLR | Reset / programiranje | ulaz | | Pull-up, programski konektor |
| 2 | RA0 | Mjerenje napona kondenzatora topa | ulaz (ADC) | AN0, RP26 | Razdjelnik napona + zaštitna Zener dioda |
| 3 | RA1 | Mjerenje napona baterije VBAT | ulaz (ADC) | AN1, RP27 | Razdjelnik napona; ako IR modul bude analogni, treba drugi ADC pin (**TBD**) |
| 4 | RB0 | Programiranje (PGD1) | ulaz/izlaz | AN2, RP0 | Rezervisano za programer |
| 5 | RB1 | Programiranje (PGC1) | ulaz/izlaz | AN3, RP1 | Rezervisano za programer |
| 6 | RB2 | JoyCar komanda 1 | izlaz | AN4, RP2, SDA2 | **TBD**, pin se može remapirati (PWM, UART) ili koristiti kao I2C2 |
| 7 | RB3 | JoyCar komanda 2 | izlaz | AN5, RP3, SCL2 | **TBD**, pin se može remapirati ili koristiti kao I2C2 |
| 9 | RA2 | LED (status) | izlaz | OSCI | Interni oscilator (FRC), pin je slobodan |
| 10 | RA3 | Top: punjenje (omogući punjenje) | izlaz | OSCO | Rezervisano za top, samo digitalni izlaz |
| 11 | RB4 | Top: rezerva | **TBD** | SOSCI, RP4 | Rezervisano za top |
| 12 | RA4 | Top: ispaljivanje (okidanje) | izlaz | SOSCO | Rezervisano za top, preko izolacije/drivera, samo digitalni izlaz |
| 14 | RB5 | Dugme START/STOP | ulaz | RP5, 5 V tolerantan | Prekid INT1 preko RP5 |
| 15 | RB6 | HC-SR04 #1 (prepreke): TRIG | izlaz | RP6, 5 V tolerantan | |
| 16 | RB7 | HC-SR04 #1 (prepreke): ECHO | ulaz | RP7, INT0, 5 V tolerantan | Prekid INT0 na ivici signala, 5 V signal direktno |
| 17 | RB8 | I2C SCL1 (rezerva) | **TBD** | RP8, SCL1, 5 V tolerantan | Ako IR modul ili JoyCar koriste I2C |
| 18 | RB9 | I2C SDA1 (rezerva) | **TBD** | RP9, SDA1 | Ako IR modul ili JoyCar koriste I2C, **nije** 5 V tolerantan |
| 21 | RB10 | HC-SR04 #2 (meta): TRIG | izlaz | RP10, 5 V tolerantan | |
| 22 | RB11 | HC-SR04 #2 (meta): ECHO | ulaz | RP11, 5 V tolerantan | 5 V signal direktno, prekid se mapira preko PPS-a |
| 23 | RB12 | UART1 RX ← ESP8266 TX | ulaz | RP12 | |
| 24 | RB13 | UART1 TX → ESP8266 RX | izlaz | RP13 | |
| 25 | RB14 | JoyCar komanda 3 | izlaz | RP14 | **TBD**, pin se može remapirati (PWM, UART) |
| 26 | RB15 | JoyCar komanda 4 | izlaz | RP15 | **TBD**, pin se može remapirati (PWM, UART) |
| 13, 28 | VDD, AVDD | Napajanje 3.3 V | | | Uz svaki pin kondenzator 100 nF |
| 8, 19, 27 | VSS, AVSS | Masa | | | |
| 20 | VCAP | Kondenzator za jezgru | | | Prema datasheetu (npr. 10 µF) |
 
Napomene o pinovima:
- **5 V tolerantni ulazi** su prema datasheetu samo RB5, RB6, RB7, RB8, RB10 i RB11. ECHO signali HC-SR04 (5 V) zato su na RB7 i RB11 i mogu ići direktno, uz zaštitni serijski otpornik (npr. 1 kΩ). Ostali pinovi nisu 5 V tolerantni.
- **Remapirani (RP) pinovi** su RB0–RB15 (RP0–RP15), RA0 (RP26) i RA1 (RP27). Pinovi **RA2, RA3 i RA4 nemaju RP funkciju**, pa su dodijeljeni isključivo digitalnim signalima (LED, punjenje i okidanje topa). Zato JoyCar komande (koje mogu tražiti PWM, UART ili I2C) idu na RB2, RB3, RB14 i RB15.
- **Oscilator:** pinovi RA2 (OSCI), RA3 (OSCO), RA4 (SOSCO) i RB4 (SOSCI) su I/O samo ako se koristi interni oscilator (FRC) i ako je sekundarni oscilator isključen u konfiguraciji.
- **Analogni pinovi** (AN0–AN9) se moraju podesiti kao digitalni (ANSx = 0) kada se koriste kao I/O.
- Dva ultrazvučna senzora ne smiju mjeriti istovremeno (međusobne smetnje), pa ih firmver čita naizmjenično.
- JoyCar komande su samo rezervacija: tačan broj pinova (npr. 2 PWM + 2 smjera, ili I2C/UART) ovisi o JoyCar ploči.
- ESP8266 pin EN (CH_PD) ide preko otpornika na 3.3 V.
---
 
## 4. Interfejs prema JoyCar ploči (motori)
 
**Status: TBD, treba istražiti dokumentaciju JoyCar-a.**
 
JoyCar ima ploču sa drajverima motora, a naša ploča joj zadaje komande na logičkom nivou. Potrebno saznati:
 
| Pitanje | Odgovor |
|---|---|
| Način komandovanja (PWM + smjer, I2C, UART ili drugo) | TBD |
| Logički nivo ulaza (3.3 V ili 5 V) | TBD |
| Potreban broj pinova | TBD |
| Napaja li se JoyCar ploča iz iste baterije | TBD |
| Konektor (tip, raspored) | TBD |
| Pretvarač nivoa 3.3 V ↔ 5 V potreban | TBD |
 
---
 
## 5. Konektor za top (rezervisano)
 
Top još nije dostupan, pa se predviđa konektor sa sljedećim signalima. Raspored se može promijeniti kad se top dobije.
 
| Pin konektora | Signal | Smjer (iz ploče) | Napon |
|---|---|---|---|
| 1 | VBAT / napajanje topa (iz zajedničkih baterija, preko prekidača i osigurača) | izlaz | VBAT, oko 4–6.4 V (**TBD** da li se punjač nalazi na ploči ili na topu) |
| 2 | GND | | |
| 3 | CHARGE, omogući punjenje | izlaz | 3.3 V logika |
| 4 | FIRE, okidanje | izlaz | 3.3 V logika (preko drivera) |
| 5 | VCAP_SENSE, napon kondenzatora | ulaz | 0–3.3 V (razdjelnik) |
| 6 | Rezerva | | |
 
Napomene:
- Predvidjeti test tačke i jumpere na linijama topa, da se top može odvojiti od ploče.
- Predvidjeti automatsko pražnjenje kondenzatora (bleeder otpornik).
---
 
## 6. I2C
 
Koristi se samo ako IR modul ili JoyCar ploča to traže.
 
| Uređaj | Adresa | Napomena |
|---|---|---|
| IR distance click (ako je I2C) | **TBD** | Provjeriti u datasheetu |
| JoyCar ploča (ako je I2C) | **TBD** | |
 
HC-SR04 ne koristi I2C.
 
---
 
## 7. Komunikacioni protokol
 
### 7.1 Veza
- **WiFi:** ESP8266 u station modu, spaja se na mrežu preko AT komandi (`AT+CWMODE=1`, `AT+CWJAP`).
- **UART:** PIC ↔ ESP8266, 115200 baud, 8N1.
- **TCP:** vozilo je **klijent**, aplikacija na računaru je **server** (port **TBD**, npr. 8084).
- SSID, lozinka i IP adresa računara čuvaju se u konfiguracionoj datoteci (`config.h`), a ne u ovom dokumentu.
### 7.2 Komande (aplikacija → vozilo)
Poruke su tekstualne i završavaju znakom novog reda (`\n`).
 
| Komanda | Značenje |
|---|---|
| `START\n` | Pokreće obje mašine stanja |
| `STOP\n` | Zaustavlja vozilo, razoružava top, vraća mašine u IDLE |
 
### 7.3 Log poruke (vozilo → aplikacija)
 
```
[vrijeme s] NIVO modul: poruka\n
```
 
Primjeri:
```
[12.4s] INFO  STATE=TRACK dist=830mm
[13.1s] INFO  FIRE
[13.2s] ERROR CAP voltage low
```
 
| Polje | Opis |
|---|---|
| vrijeme | sekunde od starta, npr. `12.4s` |
| NIVO | `INFO`, `WARN`, `ERROR` |
| poruka | slobodan tekst ili `KLJUČ=vrijednost` parovi |
 
---
 
## 8. Otvorene stavke
 
| # | Stavka | Ko saznaje |
|---|---|---|
| 1 | Konačna potvrda MCU-a (PIC24FJ64GA702 ili sličan) | Profesor |
| 2 | Tačan model IR modula za metu i njegov interfejs | Profesor |
| 3 | Interfejs prema JoyCar ploči (pinovi, nivoi, napajanje) | HW tim, JoyCar dokumentacija |
| 4 | Napon i struja topa | Kad se top dobije |
| 5 | ~~Da li su pinovi PIC-a za ECHO 5 V tolerantni~~ Riješeno: RB7 i RB11 su 5 V tolerantni | |
| 6 | Konačno mapiranje RPn pinova za JoyCar komande (zavisi od interfejsa JoyCar ploče) | HW + SW tim |
| 7 | Potrošnja struje i pad napona baterije pod opterećenjem (motori, punjenje topa, WiFi) sa 4 × AA, odabir NiMH ili alkalnih | HW tim, mjerenje |
| 8 | Prag napona baterije za zaustavljanje i prekid punjenja topa | HW + SW tim |

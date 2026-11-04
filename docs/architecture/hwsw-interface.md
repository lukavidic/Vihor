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
| Motori | JoyCar, sa JoyCar pločom koja sadrži PWM kontroler | I2C, adresa 0x70 (vidi odjeljak 4) |
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
| 3 | RA1 | Napon baterije (JoyCar kontakt P2, jumper BAT) | ulaz (ADC) | AN1, RP27 | JoyCar već ima djelitelj napona na P2; provjeriti opseg (**TBD**) |
| 4 | RB0 | Programiranje (PGD1) | ulaz/izlaz | AN2, RP0 | Rezervisano za programer |
| 5 | RB1 | Programiranje (PGC1) | ulaz/izlaz | AN3, RP1 | Rezervisano za programer |
| 6 | RB2 | JoyCar kontakt P0 (LED farovi), opciono | izlaz | AN4, RP2, SDA2 | Opciono, inače rezerva |
| 7 | RB3 | JoyCar kontakt P16 (zujalica), opciono | izlaz | AN5, RP3, SCL2 | Opciono, inače rezerva |
| 9 | RA2 | LED (status) | izlaz | OSCI | Interni oscilator (FRC), pin je slobodan |
| 10 | RA3 | Top: punjenje (omogući punjenje) | izlaz | OSCO | Rezervisano za top, samo digitalni izlaz |
| 11 | RB4 | Top: rezerva | **TBD** | SOSCI, RP4 | Rezervisano za top |
| 12 | RA4 | Top: ispaljivanje (okidanje) | izlaz | SOSCO | Rezervisano za top, preko izolacije/drivera, samo digitalni izlaz |
| 14 | RB5 | Dugme START/STOP | ulaz | RP5, 5 V tolerantan | Prekid INT1 preko RP5 |
| 15 | RB6 | HC-SR04 #1: TRIG (JoyCar kontakt P8) | izlaz | RP6, 5 V tolerantan | Senzor na US zaglavlju JoyCar ploče |
| 16 | RB7 | HC-SR04 #1: ECHO (JoyCar kontakt P12) | ulaz | RP7, INT0, 5 V tolerantan | Prekid INT0 na ivici signala, 5 V signal direktno |
| 17 | RB8 | I2C SCL1: JoyCar kontakt P19 | izlaz | RP8, SCL1, 5 V tolerantan | PWM kontroler (0x70) i IO ekspander (0x38) |
| 18 | RB9 | I2C SDA1: JoyCar kontakt P20 | ulaz/izlaz | RP9, SDA1 | Isto kao SCL, pin **nije** 5 V tolerantan |
| 21 | RB10 | HC-SR04 #2: TRIG | izlaz | RP10, 5 V tolerantan | Vlastito ožičenje i napajanje 5 V |
| 22 | RB11 | HC-SR04 #2 (meta): ECHO | ulaz | RP11, 5 V tolerantan | 5 V signal direktno, prekid se mapira preko PPS-a |
| 23 | RB12 | UART1 RX ← ESP8266 TX | ulaz | RP12 | |
| 24 | RB13 | UART1 TX → ESP8266 RX | izlaz | RP13 | |
| 25 | RB14 | JoyCar kontakt P1 (servo 1), opciono | izlaz | RP14 | Opciono, inače rezerva |
| 26 | RB15 | Rezerva | **TBD** | RP15 | Slobodan pin |
| 13, 28 | VDD, AVDD | Napajanje 3.3 V | | | Uz svaki pin kondenzator 100 nF |
| 8, 19, 27 | VSS, AVSS | Masa | | | |
| 20 | VCAP | Kondenzator za jezgru | | | Prema datasheetu (npr. 10 µF) |
 
Napomene o pinovima:
- **5 V tolerantni ulazi** su prema datasheetu samo RB5, RB6, RB7, RB8, RB10 i RB11. ECHO signali HC-SR04 (5 V) zato su na RB7 i RB11 i mogu ići direktno, uz zaštitni serijski otpornik (npr. 1 kΩ). Ostali pinovi nisu 5 V tolerantni.
- **Remapirani (RP) pinovi** su RB0–RB15 (RP0–RP15), RA0 (RP26) i RA1 (RP27). Pinovi **RA2, RA3 i RA4 nemaju RP funkciju**, pa su dodijeljeni isključivo digitalnim signalima (LED, punjenje i okidanje topa). JoyCar se komanduje preko I2C (RB8 i RB9), pa su pinovi RB2, RB3, RB14 i RB15 slobodni i ostavljeni kao rezerva.
- **Oscilator:** pinovi RA2 (OSCI), RA3 (OSCO), RA4 (SOSCO) i RB4 (SOSCI) su I/O samo ako se koristi interni oscilator (FRC) i ako je sekundarni oscilator isključen u konfiguraciji.
- **Analogni pinovi** (AN0–AN9) se moraju podesiti kao digitalni (ANSx = 0) kada se koriste kao I/O.
- Dva ultrazvučna senzora ne smiju mjeriti istovremeno (međusobne smetnje), pa ih firmver čita naizmjenično.
- I2C linije (RB8 i RB9) zahtijevaju pull-up otpornike (obično 2.2–4.7 kΩ na 3.3 V). Provjeriti postoje li već na JoyCar ploči, da se ne dupliraju.
- ESP8266 pin EN (CH_PD) ide preko otpornika na 3.3 V.
---
 
## 4. Interfejs prema JoyCar ploči (motori)
 
JoyCar ploča je namijenjena za BBC micro:bit, koji je I2C **master**, a ostali uređaji na ploči su **slave**. Naša ploča (PIC) preuzima ulogu mastera. Prema uputstvu, na ploči postoje dva I2C uređaja:
 
| Uređaj | I2C adresa | Namjena |
|---|---|---|
| PWM kontroler | **0x70** | Upravljanje motorima (registri 0x02–0x05) |
| IO ekspander | **0x38** | Infracrveni senzori na JoyCar ploči (zaglavlja OBSTCL L/R i LINE TRK L/M/R) |
 
### 4.1 Inicijalizacija PWM kontrolera (I2C adresa 0x70)
 
Iz primjera koda za micro:bit (Python) vidi se tačan redoslijed I2C poruka. I2C magistrala radi na **400 kHz**. Pri startu se šalju dvije poruke na adresu 0x70:
 
| Redoslijed | Bajtovi (registar, vrijednost) | Značenje |
|---|---|---|
| 1 | `0x00, 0x01` | Registar MODE1 = 0x01: normalan rad (bez sleep moda), uključen odgovor na all-call adresu |
| 2 | `0xE8, 0xAA` | Registar LEDOUT (0x08) sa uključenim auto-inkrementom (0xE0 + 0x08), vrijednost 0xAA: sva četiri izlaza rade pod pojedinačnom PWM kontrolom |
 
Struktura registara odgovara 4-kanalnom PWM/LED kontroleru **PCA9633** (ili kompatibilnom), čija je all-call adresa 0x70. To je zaključak iz koda i nije potvrđeno u uputstvu, pa provjeriti čip na ploči i njegov datasheet.
 
### 4.2 Komandovanje motorima
 
Brzina svakog izlaza se zadaje jednom I2C porukom `[registar, vrijednost]`, gdje je vrijednost PWM **0–255**:
 
| Parametar | Registar | Motor | Smjer |
|---|---|---|---|
| PWM0 | 0x02 | desni | nazad |
| PWM1 | 0x03 | desni | naprijed |
| PWM2 | 0x04 | lijevi | nazad |
| PWM3 | 0x05 | lijevi | naprijed |
 
Za jedan korak upravljanja šalju se četiri poruke: `[0x02, PWM0]`, `[0x03, PWM1]`, `[0x04, PWM2]`, `[0x05, PWM3]`. Kombinacije iz primjera koda:
 
| Manevar | PWM0 | PWM1 | PWM2 | PWM3 | Napomena |
|---|---|---|---|---|---|
| Naprijed | 0 | v | 0 | v | oba motora naprijed |
| Nazad | v | 0 | v | 0 | oba motora nazad |
| Okret ulijevo u mjestu | 0 | v | v | 0 | desni naprijed, lijevi nazad |
| Okret udesno u mjestu | v | 0 | 0 | v | desni nazad, lijevi naprijed |
| Stop | 0 | 0 | 0 | 0 | jako kočenje |
| Meko kočenje | 255 | 255 | 255 | 255 | iz uputstva |
 
Skretanje u vožnji: spoljašnji točak ide brzinom `v`, a unutrašnji brzinom `0.7 · v · (r / 5)`, gdje je `r` radijus od 1 do 5. Npr. skretanje ulijevo naprijed: `PWM1 = v`, `PWM3 = unutrašnja brzina`.
 
Napomena o nedoslijednosti u izvoru: komentar u primjeru koda kaže da su PWM0 i PWM1 lijevi motor, ali primjeri vožnje (npr. skretanje ulijevo smanjuje PWM3) i kompenzacija (`biasR` se primjenjuje na PWM0 i PWM1) odgovaraju gornjoj tabeli, gdje su PWM0 i PWM1 desni motor. Tabela uputstva ("Lijevo" pokreće samo jedan kanal) također se razlikuje od primjera koda. **Smjerove treba provjeriti na stvarnom vozilu** prije fiksiranja u firmveru.
 
Kompenzacija različite brzine motora u primjeru koda: vrijednost se množi faktorom `(100 − bias) / 100` (zasebno za desni i lijevi motor). Preuzeti u firmver kao dvije konstante.
 
### 4.3 Šta još treba saznati
 
| Pitanje | Odgovor |
|---|---|
| Brzina I2C magistrale | 400 kHz (iz primjera koda) |
| Logički nivo I2C linija (3.3 V ili 5 V) | TBD (micro:bit je 3.3 V) |
| Pull-up otpornici na JoyCar ploči | TBD |
| Potvrda čipa PWM kontrolera (PCA9633) i smjerova motora | TBD, test na vozilu |
| Napaja li se naša ploča sa JoyCar ploče (izlazi 3V3, 5V i V_BAT na zaglavlju) i koliku struju daju | TBD |
 
### 4.4 IO ekspander (adresa 0x38): senzori na JoyCar ploči
 
IO ekspander na JoyCar ploči prikuplja signale svih senzora i vraća ih kao **jedan bajt**. Svaki bit odgovara jednom pinu ekspandera, a bit je 1 (True) kada senzor detektuje.
 
| Bit | Pin ekspandera | Senzor |
|---|---|---|
| 0 | PIN 0 | Senzor brzine, lijevi |
| 1 | PIN 1 | Senzor brzine, desni |
| 2 | PIN 2 | Senzor linije, lijevi |
| 3 | PIN 3 | Senzor linije, srednji |
| 4 | PIN 4 | Senzor linije, desni |
| 5 | PIN 5 | Senzor prepreke, lijevi (Hindernis) |
| 6 | PIN 6 | Senzor prepreke, desni (Hindernis) |
| 7 | PIN 7 | Opcioni digitalni izlaz EOUT7 (slobodan za vlastite projekte) |
 
Primjer: bajt `1 0 1 1 0 1 1 1` (bit 7 do bit 0) znači da su detektovani: senzor brzine L i R, senzor linije L i R, senzor prepreke L i bit 7 je postavljen, a srednji senzor linije i desni senzor prepreke ne detektuju.
 
Mogućnosti:
- **Senzori prepreke** (bit 5 i 6) daju digitalnu informaciju (prepreka da ili ne), pa se mogu koristiti kao dodatna detekcija prepreka bez dodatnih pinova na našoj ploči. Domet treba provjeriti (**TBD**), jer se radi o infracrvenim senzorima kratkog dometa.
- **Senzori brzine** (bit 0 i 1) daju impulse pri okretanju točkova. Čitanje preko I2C-a je ograničeno brzinom magistrale, pa je za precizno brojanje impulsa potrebno ispitati dovoljnu učestalost očitavanja (**TBD**).
- Bit 7 (EOUT7) je slobodan digitalni izlaz, koji se može koristiti za vlastite potrebe (**TBD**).
---
 
### 4.5 Konektor prema JoyCar ploči i mapiranje micro:bit pinova
 
Naša ploča zamjenjuje micro:bit: ulazi u **ivični utor** (konektor za micro:bit) na JoyCar ploči. Prema šemi iz uputstva (stranica 48), JoyCar koristi sljedeće micro:bit kontakte:
 
| micro:bit kontakt | JoyCar funkcija | Naš PIC pin | Status |
|---|---|---|---|
| P0 | LED farovi (FL, FR, RL, RR) | RB2 | Opciono |
| P1 | Servo 1 | RB14 | Opciono |
| P2 | Servo 2 ili mjerenje baterije (ADC), bira se jumperom BAT/P2 | RA1 | Koristimo za napon baterije |
| P8 | US senzor: TRIG | RB6 | Koristimo (HC-SR04 #1) |
| P12 | US senzor: ECHO | RB7 | Koristimo (HC-SR04 #1) |
| P13, P14, P15 | SPI (SCK, MISO, MOSI) | nije povezano | Ne koristimo |
| P16 | Zujalica | RB3 | Opciono |
| P19, P20 | I2C: SCL i SDA (PWM kontroler 0x70, IO ekspander 0x38) | RB8, RB9 | Koristimo |
| 3V | Napajanje 3.3 V | | Vidi napomene o napajanju (provjeriti smjer) |
| GND | Masa | | |
 
Napomene:
- **Nisu svi kontakti obavezni.** Naša ploča treba imati kontakte (zlatne prste) samo za signale koje koristimo (P2, P8, P12, P19, P20, 3V, GND, a opciono P0, P1, P16). Raspored i razmak kontakata mora odgovarati micro:bitu (25 kontakata), pa se prepisuje sa stvarnog micro:bita ili njegovog crteža. Debljina ploče je 1.6 mm.
- **JoyCar ima samo jedno US zaglavlje** (P8 TRIG, P12 ECHO), pa se na njega spaja jedan HC-SR04. Drugi HC-SR04 se spaja direktno na našu ploču (RB10 i RB11) i napaja sa JoyCar zaglavlja (5V i GND).
- **ECHO signal** sa US zaglavlja (P12) ide u PIC na RB7, koji je 5 V tolerantan. Ako JoyCar zaglavlje daje 5 V signal, to je u redu, a ako daje 3.3 V, takođe.
- **Napon baterije (P2):** JoyCar mjeri bateriju preko jumpera BAT/P2. Jumper mora ostati na BAT (ne na servo 2), da bi PIC čitao napon baterije na pinu RA1 (ADC). Opseg napona na P2 provjeriti multimetrom (**TBD**).
- **Dodatni signali iz zaglavlja** (V_BAT, 5V, 3v3, GND) nisu na micro:bit utoru: spajaju se žicama ili posebnim konektorom na JoyCar zaglavlje (za napajanje ESP8266, drugog HC-SR04 i topa).
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
 
PIC je I2C master na modulu I2C1 (RB8 = SCL1, RB9 = SDA1). Magistrala je zajednička za JoyCar ploču i IR modul (ako je I2C).
 
| Uređaj | Adresa | Napomena |
|---|---|---|
| JoyCar PWM kontroler | 0x70 | Motori (vjerovatno PCA9633) |
| JoyCar IO ekspander | 0x38 | Senzori JoyCar ploče (brzina, linija, prepreke), jedan bajt |
| IR modul za metu (ako je I2C) | **TBD** | Provjeriti u datasheetu, ne smije se poklapati sa 0x70 i 0x38 |
 
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
| 3 | Nivo I2C, pull-up, napajanje sa JoyCar ploče, konektor i smjerovi motora (test) | HW tim, JoyCar uputstvo |
| 4 | Napon i struja topa | Kad se top dobije |
| 5 | ~~Da li su pinovi PIC-a za ECHO 5 V tolerantni~~ Riješeno: RB7 i RB11 su 5 V tolerantni | |
| 6 | Potrošnja struje i pad napona baterije pod opterećenjem (motori, punjenje topa, WiFi) sa 4 × AA, odabir NiMH ili alkalnih | HW tim, mjerenje |
| 7 | Prag napona baterije za zaustavljanje i prekid punjenja topa | HW + SW tim |

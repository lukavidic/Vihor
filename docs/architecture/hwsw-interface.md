# Interfejs sistema (HW/SW)
 
**Projekat:** Autonomna mobilna platforma sa integrisanim elektromagnetskim topom
**Verzija:** 0.23 (nacrt)
**Svrha:** Zajednički dogovor hardverskog i softverskog tima: napajanje, pinovi mikrokontrolera, konektori i komunikacioni protokol. Svaka promjena pinova ili protokola ide prvo kroz izmjenu ovog dokumenta, uz saglasnost oba tima.
 
Vrijednosti označene sa **TBD** treba potvrditi (profesor, JoyCar dokumentacija, datasheet).
 
---
 
## 1. Ključne komponente
 
| Komponenta | Izbor | Status |
|---|---|---|
| Mikrokontroler | PIC24FJ64GA702 (SOIC-28) | Potvrđeno |
| WiFi modul | ESP8266 (ESP-01), AT komande preko UART-a | Potvrđeno i ispitano (AT firmver 1.7.4, 9600 baud) |
| Senzor za prepreke | HC-SR04 (ultrazvučni) | Potvrđeno |
| Senzor za metu | IR Distance Click (MikroE, Sharp GP2Y0A60SZ0F, analogni izlaz). Po datasheetu 10–150 cm, izmjereno pouzdano otkrivanje do oko 80 cm (odjeljak 3.1) | Potvrđeno i ispitano |
| Kula | Okretna kula sa servo motorom SG90, na JoyCar izlazu Servo 1 (kontakt P1). Na kuli su top, senzor za metu i meta | Potvrđeno. Izmjereni opseg oko 90° (odjeljak 3.2); granica napona SG90 **TBD** |
| Motori | JoyCar, sa JoyCar pločom koja sadrži PWM kontroler | Potvrđeno |
| Napajanje | 4 × 1.5 V AA baterije, naponi 3v3, 5V i V_BAT sa zaglavlja JoyCar ploče | Potvrđeno |
| Top | Elektromagnetski top | Još nije dostupan, rezervisani pinovi |
 
---
 
## 2. Napajanje
 
Izvor napajanja za cijeli sistem su **4 × 1.5 V AA baterije** na JoyCar ploči. Naša ploča **nema vlastite regulatore**: sve napone uzima sa zaglavlja JoyCar ploče (pinovi **3v3, 5V, V_BAT i GND**), koji su izmjereni i imaju odgovarajuće vrijednosti.
 
| Napon | Izvor | Potrošači |
|---|---|---|
| 3.3 V | JoyCar zaglavlje (pin 3v3) | PIC, ESP8266, IR Distance Click |
| 5 V | JoyCar zaglavlje (pin 5V), na ploču preko J6 | Na našoj ploči ga ne koristi nijedan dio (HC-SR04 napaja JoyCar preko US zaglavlja) |
| V_BAT (oko 4.5–9 V, sa 4 × AA oko 6 V) | JoyCar zaglavlje (pin V_BAT) | Punjač kondenzatora topa |
| GND | JoyCar zaglavlje | Zajednička masa |
 
Napomene:
- Pinovi 3v3, 5V, V_BAT i GND nisu na ivičnom konektoru (micro:bit utoru), pa se na našu ploču spajaju konektorom **J6** (1 +3.3V, 2 +5V, 3 VBAT, 4 GND), žicama sa zaglavlja JoyCar ploče.
- Uz PIC su kondenzatori C4 (VDD) i C6 (AVDD), 100 nF. Uz ESP8266 su C1 (100 µF, elektrolitski), C2 (10 µF) i C3 (100 nF), jer povlači strujne šiljke pri slanju. Uz konektor J6 je C7 (10 µF), na mjestu gdje napon 3.3 V ulazi na ploču.
- Za sve naponske nivoe treba **provjeriti koliko struje JoyCar daje** (ESP8266 do oko 300 mA vršno), uz stvarno opterećenje.
- Na JoyCar ploči postoji prekidač ON/OFF koji isključuje cijeli sistem. Napon baterije se može mjeriti preko kontakta P2 (jumper BAT/P2, vidi odjeljak 4.5).
- Top se puni iz V_BAT. Punjenje velikom strujom može spustiti napon baterije i resetovati PIC, pa se punjenje radi ograničenom strujom i samo kad vozilo stoji (firmver), uz zaštitu pri niskom naponu baterije.
---
 
## 3. Tabela pinova mikrokontrolera (PIC24FJ64GA702, SOIC-28)
 
Raspodjela je usklađena sa datasheetom (tabela funkcija pinova). PIC ima 21 I/O pin (RA0–RA4 i RB0–RB15), a u ovoj raspodjeli su svi zauzeti ili rezervisani. Pinovi RB10 i RB11 nose reset ESP-01 modula i debug izlaz, pa su jedine rezerve RB4 (TOP_RESERVE, rezervisan za top) i RB15 (zujalica, upotreba opciona). Ako se pojavi potreba za više pinova, treba razmisliti o većem PIC-u iz iste porodice. Kolona „Napomena“ navodi oznake dijelova sa šeme (`hardware/Vihor`, list „Moduli“).
 
| Fizički pin | Port | Funkcija | Smjer | Posebne mogućnosti pina | Napomena |
|---|---|---|---|---|---|
| 1 | MCLR | Reset / programiranje | ulaz | | Pull-up R10 (10k), programski konektor J3 (pin 1) |
| 2 | RA0 | Mjerenje napona kondenzatora topa | ulaz (ADC) | AN0, RP26 | Signal CAP_V sa konektora topa J2 (pin 5) preko djelitelja R5/R7 i zener diode D1 (vrijednosti **TBD**) |
| 3 | RA1 | Napon baterije (JoyCar kontakt P2, jumper BAT) | ulaz (ADC) | AN1, RP27 | Sa J5 (pin 6) preko R11 (0 Ω). JoyCar već ima djelitelj napona na P2. Izmjereno: oko 2.6 V pri V_BAT od 6.45 V (ispod 3.3 V), pa se spaja direktno; R12 (mjesto za vlastiti djelitelj) ostaje neugrađen |
| 4 | RB0 | Programiranje (PGD1) | ulaz/izlaz | AN2, RP0 | Rezervisano za programer |
| 5 | RB1 | Programiranje (PGC1) | ulaz/izlaz | AN3, RP1 | Rezervisano za programer |
| 6 | RB2 | IR Distance Click: OUT (analogni izlaz) | ulaz (ADC) | AN4, RP2, SDA2 | Senzor za metu, konektor J1 (pin 3); napajanje 3.3 V (jumperi PWR SEL na 3V3) |
| 7 | RB3 | IR Distance Click: EN (uključenje senzora) | izlaz | AN5, RP3, SCL2 | Digitalni izlaz, konektor J1 (pin 4) |
| 9 | RA2 | LED (status) | izlaz | OSCI | LED D2 preko R9 (1k). Koristi se interni oscilator (FRC), pa je pin slobodan za I/O |
| 10 | RA3 | Top: punjenje (omogući punjenje) | izlaz | OSCO | Konektor topa J2 (pin 3), pull-down R3 (10k). Samo digitalni izlaz |
| 11 | RB4 | Top: rezerva | **TBD** | SOSCI, RP4 | Konektor topa J2 (pin 6), TOP_RESERVE |
| 12 | RA4 | Top: ispaljivanje (okidanje) | izlaz | SOSCO | Konektor topa J2 (pin 4), pull-down R4 (10k), preko izolacije/drivera na topu. Samo digitalni izlaz |
| 14 | RB5 | Dugme START/STOP | ulaz | RP5, 5 V tolerantan | Dugme SW1, pull-up R15 (10k). Prekid INT1 preko RP5 |
| 15 | RB6 | HC-SR04 #1: TRIG (JoyCar kontakt P8) | izlaz | RP6, 5 V tolerantan | J5 (pin 8) preko serijskog otpornika R18 (1k). Senzor na US zaglavlju JoyCar ploče. Linija je u mirovanju na skoro 5 V (izmjereno), pa R18 ograničava struju u pin |
| 16 | RB7 | HC-SR04 #1: ECHO (JoyCar kontakt P12) | ulaz | RP7, INT0, 5 V tolerantan | J5 (pin 7) preko serijskog otpornika R13 (1k). Prekid INT0 na ivici signala |
| 17 | RB8 | I2C SCL1: JoyCar kontakt P19 | izlaz | RP8, SCL1, 5 V tolerantan | J5 (pin 3). PWM kontroler (0x70) i IO ekspander (0x38). Test tačka TP5 |
| 18 | RB9 | I2C SDA1: JoyCar kontakt P20 | ulaz/izlaz | RP9, SDA1 | J5 (pin 2). Test tačka TP4. Pin **nije** 5 V tolerantan |
| 21 | RB10 | ESP_RST (reset ESP-01) | izlaz | RP10, 5 V tolerantan | Na pin RST modula U1. Aktivno nisko; na liniji je pull-up R2 (10 kΩ) |
| 22 | RB11 | DEBUG_TX (debug izlaz) | izlaz | RP11, 5 V tolerantan | UART2 TX preko PPS-a, izveden na debug konektor J4 (1 DEBUG_TX, 2 GND); brzina **TBD** |
| 23 | RB12 | UART1 RX ← ESP8266 TXD (mreža UART_RX) | ulaz | RP12 | Test tačka TP3 |
| 24 | RB13 | UART1 TX → ESP8266 RXD (mreža UART_TX) | izlaz | RP13 | Test tačka TP2 |
| 25 | RB14 | Servo kule: PWM → JoyCar kontakt P1 | izlaz | RP14 | J5 (pin 9). PWM signal (50 Hz) za servo SG90 kule. Servo 1 zaglavlje daje oko 6.4 V (isto kao V_BAT), granica napona SG90 **TBD** |
| 26 | RB15 | Zujalica: JoyCar kontakt P16 (mreža BUZZER) | izlaz | RP15 | J5 (pin 5). Upotreba opciona, inače rezerva |
| 13, 28 | VDD, AVDD | Napajanje 3.3 V | | | C4 (VDD, pin 13) i C6 (AVDD, pin 28), 100 nF. Test tačka TP6 na +3.3V |
| 8, 19, 27 | VSS, AVSS | Masa | | | |
| 20 | VCAP | Kondenzator za jezgru | | | C5 (10 µF) |
 
Napomene o pinovima:
- **5 V tolerantni ulazi** su prema datasheetu samo RB5, RB6, RB7, RB8, RB10 i RB11. ECHO signal HC-SR04 (5 V) zato je na RB7 i može ići direktno, uz zaštitni serijski otpornik R13 (1 kΩ). Ostali pinovi nisu 5 V tolerantni.
- **Remapirani (RP) pinovi** su RB0–RB15 (RP0–RP15), RA0 (RP26) i RA1 (RP27). Pinovi **RA2, RA3 i RA4 nemaju RP funkciju**, pa su dodijeljeni isključivo digitalnim signalima (LED, punjenje i okidanje topa). JoyCar se komanduje preko I2C (RB8 i RB9). Pinovi RB2 i RB3 su zauzeti za IR Distance Click (OUT i EN), RB10 za reset ESP-01, RB11 za debug UART, RB14 je servo kule (JoyCar kontakt P1), a RB15 je zujalica (JoyCar kontakt P16), koja se može ostaviti kao rezerva.
- **Oscilator:** pinovi RA2 (OSCI), RA3 (OSCO), RA4 (SOSCO) i RB4 (SOSCI) su I/O samo ako se koristi interni oscilator (FRC) i ako je sekundarni oscilator isključen u konfiguraciji.
- **Analogni pinovi** (AN0–AN9) se moraju podesiti kao digitalni (ANSx = 0) kada se koriste kao I/O.
- I2C linije (RB8 i RB9) zahtijevaju pull-up otpornike (obično 2.2–4.7 kΩ na 3.3 V). JoyCar ploča ih već ima (SDA i SCL su u mirovanju na 3.3 V), pa se na našoj ploči ne ugrađuju: mjesta R14 (SDA) i R16 (SCL) ostaju neugrađena (DNP).
- ESP8266 pinovi EN (CH_PD), RST, IO0 i IO2 idu preko otpornika od 10 kΩ (R1, R2, R6 i R8) na 3.3 V. RST je dodatno spojen na RB10, da PIC može resetovati modul.
- **Programiranje:** PIC se programira programatorom **PICkit 3** preko konektora J3 (1 MCLR, 2 +3.3V, 3 GND, 4 PGD1, 5 PGC1). Raspored pinova na J3 je isti kao na PICkit-u, pa se programator utakne direktno. Pri programiranju se ploča napaja sa JoyCar ploče, a ne iz programatora (u MPLAB-u isključiti napajanje ploče iz PICkit-a).
### 3.1 Senzor za metu: IR Distance Click
 
Modul proizvođača MikroElektronika, sa senzorom **Sharp GP2Y0A60SZ0F** (infracrveni, mjerenje trijangulacijom, talasna dužina 870 nm).
 
| Svojstvo | Vrijednost |
|---|---|
| Interfejs | **Analogni izlaz** (OUT) i pin za uključenje (EN). Nema I2C ni SPI. |
| Opseg | 10–150 cm po datasheetu; izmjereno pouzdano otkrivanje do oko 80 cm, praćenje do oko 50 cm (vidi mjerenja ispod) |
| Napajanje | 3.3 V ili 5 V, bira se jumperima PWR SEL (svi jumperi moraju biti na istoj strani) |
| Izlaz | Napon obrnuto proporcionalan udaljenosti (nelinearno), pa je potrebna kalibraciona tabela |
 
Povezivanje na PIC (konektor **J1**: 1 +3.3V, 2 GND, 3 IR_OUT, 4 IR_EN):
- **OUT** ide na analogni ulaz **RB2 (AN4)**. Modul se napaja sa **3.3 V** (jumperi PWR SEL na 3V3), da izlaz ne pređe 3.3 V (ADC PIC-a).
- **EN** ide na digitalni izlaz **RB3**.
- Snop je uzak, pa je modul pogodniji za određivanje smjera prema meti od ultrazvučnog senzora, a HC-SR04 ostaje za prepreke (JoyCar US zaglavlje, kontakti P8 i P12).
- Pošto se za metu koristi IR modul, **drugi HC-SR04 nije potreban**.

**Pinovi na pločici** (mikroBUS raspored): OUT je na mjestu pina AN, a EN na mjestu pina RST. Na istoj strani su još 3.3V i GND, a na drugoj strani 5V i GND. Ostali pinovi su nespojeni (NC).
 
**Izmjereno** (napajanje 3.3 V, ravna svijetla površina ispred senzora, napon na OUT mjeren multimetrom):
 
| Udaljenost | Napon na OUT |
|---|---|
| 3 cm | 2.0 V |
| 5 cm | 1.8 V |
| 10 cm | 1.63 V |
| 20 cm | 0.95 V |
| 30 cm | 0.70 V |
| 50 cm | 0.48 V |
| 85 cm | 0.33 V |
| 100 cm | 0.24 V |
| prazan prostor | ispod 0.2 V |
 
- Senzor radi na 3.3 V. Najveći izmjereni napon je 2.0 V, pa izlaz ne prelazi 3.3 V.
- Napon raste sve do 3 cm, pa bliska meta ne može izgledati kao daleka.
- Na 1 m signal je blizu vrijednosti za prazan prostor. Pouzdano otkrivanje mete je do oko 80 cm, a praćenje do oko 50 cm.
- Prag "meta postoji" i kalibraciona tabela određuju se sa stvarnom metom (**TBD**), jer tamna ili kosa površina daje manji napon.
- **Pin EN:** nespojen stoji na oko 2.8 V i senzor tada radi. Kad se spusti na GND (preko 1 kΩ), izlaz OUT pada na 0 V. Senzor se dakle uključuje jedinicom na RB3, a isključuje nulom.
### 3.2 Servo kule (SG90): izmjereno
 
Ispitano sa EasyPIC pločom: signal od 3.3 V direktno sa pina mikrokontrolera, napajanje servoa sa pina +5V na JoyCar zaglavlju, period signala oko 20 ms.
 
| Provjera | Rezultat |
|---|---|
| Reaguje li servo na signal od 3.3 V | Da |
| Ukupan ugao za impulse od 0.5 do 2.5 ms | Oko 90° (procjena odoka) |
| Sredina | Impuls od 1.5 ms |
 
- PIC može upravljati servom direktno sa RB14, bez pretvarača nivoa.
- Ukupan ugao od oko 90° znači da kula može pretražiti oko 45° lijevo i desno od sredine. Van tog ugla vozilo traži metu kretanjem (zahtjev FR-05).
- **Struja** (strujna kliješta, žica provučena 4 puta, bez opterećenja, napajanje 5 V): u mirovanju ispod rezolucije instrumenta, u pokretu oko 12 do 25 mA prosječno. Kliješta pokazuju prosjek i neprecizna su na malim strujama, pa vršna struja pri pokretanju i struja pod opterećenjem nisu izmjerene (**TBD**).
- Rad na 6.4 V (zaglavlje Servo 1) nije ispitan (**TBD**).
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
 
Napomena o nedoslijednosti u izvoru: komentar u primjeru koda kaže da su PWM0 i PWM1 lijevi motor, ali primjeri vožnje (npr. skretanje ulijevo smanjuje PWM3) i kompenzacija (`biasR` se primjenjuje na PWM0 i PWM1) odgovaraju gornjoj tabeli, gdje su PWM0 i PWM1 desni motor. Tabela uputstva ("Lijevo" pokreće samo jedan kanal) također se razlikuje od primjera koda. **Provjereno na vozilu** (test sa EasyPIC pločom, I2C na 100 kHz): 0x02 desni nazad, 0x03 desni naprijed, 0x04 lijevi nazad, 0x05 lijevi naprijed. Gornja tabela je tačna.
 
Kompenzacija različite brzine motora u primjeru koda: vrijednost se množi faktorom `(100 − bias) / 100` (zasebno za desni i lijevi motor). Preuzeti u firmver kao dvije konstante.
 
**Izmjereno na vozilu** (točkovi u zraku, svježe baterije, V_BAT oko 6.4 V, upravljano sa EasyPIC ploče):
 
| Provjera | Rezultat |
|---|---|
| Najmanja vrijednost pri kojoj desni točak krene | 90 do 110 (dva mjerenja) |
| Najmanja vrijednost pri kojoj lijevi točak krene | 80 do 90 (dva mjerenja) |
| Oba motora naprijed, vrijednost 150 | Vrte se približno jednako (procjena odoka) |
| Okret u mjestu, obje strane | Točkovi se vrte u suprotnim smjerovima |
| Zaustavljanje sa svim kanalima na 0 | Točkovi stanu brže |
| Zaustavljanje sa svim kanalima na 255 | Točkovi stanu sporije |
 
- Vrijednosti ispod praga ne pokreću motor, pa je koristan opseg otprilike od 120 do 255. Najmanja brzina u vožnji treba biti iznad praga, a ne na njemu.
- Lijevi motor krene prije desnog, pa za pravu vožnju treba kompenzacija (konstante iz gornjeg pasusa).
- Na podu i sa slabijim baterijama prag će biti viši. Provjeriti na prototipu (**TBD**).
 
### 4.3 Šta još treba saznati
 
| Pitanje | Odgovor |
|---|---|
| Brzina I2C magistrale | 400 kHz (iz primjera koda) |
| Logički nivo I2C linija (3.3 V ili 5 V) | 3.3 V (izmjereno) |
| Pull-up otpornici na JoyCar ploči | Postoje (SDA i SCL u mirovanju na 3.3 V) |
| Potvrda čipa PWM kontrolera (PCA9633) i smjerova motora | Smjerovi i inicijalizacija potvrđeni testom na vozilu. Oznaka čipa nije provjerena (TBD) |
| Napaja li se naša ploča sa JoyCar ploče (izlazi 3V3, 5V i V_BAT na zaglavlju) i koliku struju daju | Naponi izmjereni (3.3 V, 5 V, 6.45 V), struja TBD |
 
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
 
| micro:bit kontakt | JoyCar funkcija | Pin J5 | Mreža | Naš PIC pin | Status |
|---|---|---|---|---|---|
| P0 | LED farovi (FL, FR, RL, RR) | — | — | nije povezano | Ne koristimo |
| P1 | Servo 1 | 9 | SERVO_PWM | RB14 | Koristimo za servo kule. Izmjereno: P1 je spojen na signalni pin Servo 1 (donji pin, kolona 1) |
| P2 | Servo 2 ili mjerenje baterije (ADC), bira se jumperom BAT/P2 | 6 | VBAT_SENSE (preko R11) | RA1 | Koristimo za napon baterije |
| P8 | US senzor: TRIG | 8 | TRIG (preko R18) | RB6 | Koristimo (HC-SR04 #1). Izmjereno: P8 je spojen na pin T US zaglavlja. Sa utaknutim senzorom linija je u mirovanju na skoro 5 V |
| P12 | US senzor: ECHO | 7 | ECHO (preko R13) | RB7 | Koristimo (HC-SR04 #1). **Potvrđeno mjerenjem:** ECHO impuls stiže na P12 i prati udaljenost (EasyPIC ulaz spojen na P12 preko 1 kΩ, bez pull-up otpornika). Između pina E i P12 je čip oznake W1, pa zvučni test ne pokazuje vezu |
| P13, P14, P15 | SPI (SCK, MISO, MOSI) | — | — | nije povezano | Ne koristimo |
| P16 | Zujalica | 5 | BUZZER | RB15 | Opciono |
| P19 | I2C: SCL (PWM kontroler 0x70, IO ekspander 0x38) | 3 | SCL | RB8 | Koristimo |
| P20 | I2C: SDA (PWM kontroler 0x70, IO ekspander 0x38) | 2 | SDA | RB9 | Koristimo |
| 3V | Napajanje 3.3 V | 4 | EDGE_3V | — | Na +3.3V samo preko premosnice R17 (0 Ω, DNP), dok se ne provjeri smjer napajanja |
| GND | Masa | 1 | GND | — | Koristimo |
 
Napomene:
- **Nisu svi kontakti obavezni.** Naša ploča treba imati kontakte (zlatne prste) samo za signale koje koristimo (P1, P2, P8, P12, P19, P20, 3V, GND, a opciono P16), ukupno 9 kontakata na konektoru **J5** (footprint `vihor:MicroBit_EdgeFingers_9`). Raspored i razmak kontakata mora odgovarati micro:bitu (25 kontakata), pa se prepisuje sa stvarnog micro:bita ili njegovog crteža. Debljina ploče je 1.6 mm.
- **JoyCar ima samo jedno US zaglavlje** (P8 TRIG, P12 ECHO), pa se na njega spaja jedan HC-SR04. Drugi HC-SR04 nije potreban jer se za metu koristi IR Distance Click.
- **ECHO signal** sa US zaglavlja (P12) ide u PIC na RB7 preko serijskog otpornika R13 (1 kΩ). Izmjereno: P12 je u mirovanju na 0 V i čip W1 ga aktivno drži na nuli (9 mV i uz otpornik 10 kΩ prema 3.3 V), a pri mjerenju se na njemu pojavljuje impuls. Pull-up otpornik nije potreban. Nivo impulsa nije izmjeren direktno, ali djelitelj napona 1 kΩ / 2 kΩ ga je spustio ispod praga jedinice, što odgovara signalu od oko 3.3 V, a ne 5 V. Zato se na liniju ECHO **ne smije stavljati djelitelj napona**.
- **TRIG linija** (P8) je sa utaknutim senzorom u mirovanju na skoro 5 V. RB6 je prema dokumentu 5 V tolerantan, ali to nije provjereno u datasheetu (**TBD**), pa je na liniju dodat serijski otpornik R18 (1 kΩ).
- **Napon baterije (P2):** JoyCar mjeri bateriju preko jumpera BAT/P2. Jumper mora ostati na BAT (ne na servo 2), da bi PIC čitao napon baterije na pinu RA1 (ADC). Izmjereno: oko 2.6 V pri V_BAT od 6.45 V, što je omjer oko 0.4. Napon na P2 pada kako baterije slabe. Na našoj ploči signal ide preko R11 (0 Ω), a R12 ostaje neugrađen.
- **Dodatni signali iz zaglavlja** (V_BAT, 5V, 3v3, GND) nisu na micro:bit utoru: spajaju se konektorom J6 i žicama na JoyCar zaglavlje (napajanje PIC-a, ESP8266, IR senzora i topa).
---
 
## 5. Konektor za top (rezervisano)
 
Top još nije dostupan. Na ploči je predviđen konektor **J2** (pinski konektor 1 × 6) sa sljedećim signalima. Raspored se može promijeniti kad se top dobije.
 
| Pin konektora | Signal | Smjer (iz ploče) | Napon |
|---|---|---|---|
| 1 | VBAT, napajanje topa (iz zajedničkih baterija, preko osigurača F1 i kratkospojnika JP1) | izlaz | VBAT, oko 4–6.4 V (**TBD** da li se punjač nalazi na ploči ili na topu) |
| 2 | GND | | |
| 3 | TOP_CHARGE, omogući punjenje (RA3, pull-down R3) | izlaz | 3.3 V logika |
| 4 | TOP_FIRE, okidanje (RA4, pull-down R4) | izlaz | 3.3 V logika (preko drivera) |
| 5 | CAP_V, napon kondenzatora | ulaz | Napon kondenzatora (**TBD**). Na ploči ga djelitelj R5/R7 i zener D1 svode na VCAP_SENSE (0–3.3 V) za RA0 |
| 6 | TOP_RESERVE, rezerva (RB4) | **TBD** | 3.3 V logika |
 
Napomene:
- Napajanje topa se odvaja kratkospojnikom JP1. Test tačke na signalnim linijama topa nisu na šemi (**TBD**).
- Vrijednost osigurača F1 je **TBD** (zavisi od struje topa).
- Ako je napon kondenzatora visok, R5, J2 i razmaci na ploči moraju biti predviđeni za taj napon (**TBD**, kad se top dobije).
- Predvidjeti automatsko pražnjenje kondenzatora (bleeder otpornik). Na šemi ga nema (**TBD**).
---
 
## 6. I2C
 
PIC je I2C master na modulu I2C1 (RB8 = SCL1, RB9 = SDA1). Magistrala je zajednička za JoyCar ploču (PWM kontroler i IO ekspander).
 
| Uređaj | Adresa | Napomena |
|---|---|---|
| JoyCar PWM kontroler | 0x70 | Motori (vjerovatno PCA9633) |
| JoyCar IO ekspander | 0x38 | Senzori JoyCar ploče (brzina, linija, prepreke), jedan bajt |
 
Pull-up otpornici su na JoyCar ploči; mjesta R14 (SDA) i R16 (SCL) na našoj ploči ostaju neugrađena (DNP). Test tačke: TP4 (SDA) i TP5 (SCL).
 
HC-SR04 ne koristi I2C.
 
---
 
## 7. Komunikacioni protokol
 
### 7.0 Priprema ESP-01 modula i rezultat ispitivanja
 
**Priprema modula prije ugradnje.** Na ploči nema konektora za programiranje ESP-01, pa se svaki modul priprema prije ugradnje:
 
1. Provjeriti da modul ima AT firmver: nakon reseta šalje `ready`, a na `AT` odgovara `OK`. Modul koji smo dobili nije ga imao (na njemu je bio program koji trepće LED diodom).
2. Ako ga nema, upisati AT firmver alatom esptool (pin IO0 na GND pri resetu). Korišten je paket ESP8266 NonOS AT Bin V1.7.4.
3. Trajno postaviti brzinu: `AT+UART_DEF=9600,8,1,0,0`.
 
**Rezultat ispitivanja** (EasyPIC Fusion v7, dsPIC33FJ256GP710A umjesto našeg PIC-a, modul napajan sa 3.3 V):
 
| Korak | Rezultat |
|---|---|
| `AT` | `OK` |
| `AT+CWMODE=1` | `OK` |
| `AT+CWJAP` (spajanje na WiFi, 2.4 GHz) | Spojen |
| `AT+CIPSTART` (TCP prema računaru, port 8084) | Veza uspostavljena |
| `AT+CIPSEND` (log poruka) | Poruka primljena na računaru |
| Prijem komande `START` sa računara | Primljena na mikrokontroleru |
 
**WiFi i motori zajedno** (isti sklop, uz I2C vezu prema JoyCar ploči, vozilo podignuto):
 
| Korak | Rezultat |
|---|---|
| Vozilo šalje log `INFO SYS: spreman` | Primljen na računaru |
| Računar šalje `START` | Motori krenu, vozilo šalje `STATE=DRIVE` |
| Računar šalje `STOP` | Motori stanu, vozilo šalje `STATE=IDLE` |
| Napon 3.3 V na JoyCar zaglavlju dok motori rade | Stabilan (mjereno multimetrom, bez zabilježene vrijednosti) |
 
Ispitivanje na našoj ploči i sa PIC24FJ64GA702 ostaje za prototip.
 
### 7.1 Veza
- **WiFi:** ESP8266 u station modu, spaja se na mrežu preko AT komandi (`AT+CWMODE=1`, `AT+CWJAP`).
- **UART:** PIC ↔ ESP8266, **9600 baud**, 8N1. Brzina 115200 nije upotrebljiva na taktu od 8 MHz: odstupanje je oko 3.5 % i modul tada ne razumije komande (ispitano).
- **Debug UART:** RB11 (DEBUG_TX), UART2 preko PPS-a, na konektoru J4; samo slanje sa PIC-a, brzina **TBD**.
- **TCP:** vozilo je **klijent**, aplikacija na računaru je **server** (port **TBD**, u testu 8084).
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
[10.1s] WARN  BAT: Baterija ispod 30%
[10.2s] INFO  NAV: STATE=NAV_SLOW (Baterija < 30%)
[12.4s] INFO  STATE=TRACK dist=830mm
[13.1s] INFO  FIRE
[13.2s] ERROR CAP voltage low
[15.0s] ERROR BAT: Napon ispod 4.0V, ulazak u FAILSAFE
```
 
| Polje   | Opis                                                        |
|---------|-------------------------------------------------------------|
| vrijeme | sekunde od starta, npr. `12.4s`                             |
| NIVO    | `INFO`, `WARN`, `ERROR`                                     |
| modul   | Opciono, skraćenica modula (npr. `SYS`, `NAV`, `BAT`, `TUR`)|
| poruka  | slobodan tekst ili `KLJUČ=vrijednost` parovi                |
 
---

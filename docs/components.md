# Spisak komponenti

**Projekat:** Autonomna mobilna platforma sa integrisanim elektromagnetskim topom
**Verzija:** 0.2 (nacrt)
**Svrha:** Pregled svih komponenti sistema sa slikom, namjenom i osnovnim podacima. Detalji o pinovima i povezivanju su u [hwsw-interface.md](architecture/hwsw-interface.md), a o napajanju i blokovima u [hardware.md](architecture/hardware.md).

**Slike:** fotografije se stavljaju u folder `docs/images/` pod nazivom koji je naveden u oznaci slike uz komponentu. Dok slika ne postoji, prikazuje se prazan okvir. Najbolje su fotografije stvarnih dijelova koje imamo.

Vrijednosti označene sa **TBD** treba potvrditi.

---

## 1. Glavne komponente

### 1.1 Mikrokontroler: PIC24FJ64GA702

![PIC24FJ64GA702](images/pic24fj64ga702.jpg)

| Svojstvo | Vrijednost |
|---|---|
| Namjena | Upravljanje cijelim sistemom, mašine stanja |
| Kućište | SOIC-28 |
| Napon | 3.3 V |
| Interfejsi | I2C, UART, ADC, GPIO |
| Status | Potvrđeno |
| Dokumentacija | datasheet: (https://ww1.microchip.com/downloads/aemDocuments/documents/MCU16/ProductDocuments/DataSheets/PIC24FJ256GA705-Family-Data-Sheet-DS30010118E.pdf) |

### 1.2 WiFi modul: ESP-01 (ESP8266)

![ESP-01](images/esp-01.jpg)

| Svojstvo | Vrijednost |
|---|---|
| Namjena | Veza sa PC aplikacijom (WiFi, TCP) |
| Napon | 3.3 V, vršno do oko 300 mA |
| Interfejs | UART, 115200 baud, AT komande |
| Status | Potvrđeno |
| Dokumentacija | datasheet: (https://www.alldatasheet.com/datasheet-pdf/view/1179098/ETC2/ESP-01.html) |

### 1.3 Senzor za prepreke: HC-SR04

![HC-SR04](images/hc-sr04.jpg)

| Svojstvo | Vrijednost |
|---|---|
| Namjena | Detekcija prepreka (niski senzor) |
| Napon | 5 V |
| Interfejs | TRIG (ulaz), ECHO (izlaz, 5 V signal) |
| Priključak | JoyCar US zaglavlje (P8 TRIG, P12 ECHO) |
| Status | Potvrđeno |
| Dokumentacija | datasheet: (https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf) |

### 1.4 Senzor za metu: IR Distance Click

![IR Distance Click](images/ir-distance-click.jpg)

| Svojstvo | Vrijednost |
|---|---|
| Proizvođač | MikroElektronika |
| Senzor | Sharp GP2Y0A60SZ0F |
| Namjena | Detekcija mete, opseg 10–150 cm |
| Napon | 3.3 V (jumperi PWR SEL na 3V3) |
| Interfejs | Analogni izlaz OUT, pin EN |
| Status | Potvrditi sa profesorom |
| Dokumentacija | datasheet: (https://download.mikroe.com/documents/datasheets/GP2Y0A60SZxF.pdf) |

### 1.5 JoyCar platforma

![JoyCar](images/joycar.jpg)

| Svojstvo | Vrijednost |
|---|---|
| Proizvođač | Joy-IT |
| Sadržaj | Šasija, motori, ploča sa drajverima motora, senzori (linija, prepreke, brzina), držač baterija |
| I2C uređaji | PWM kontroler 0x70, IO ekspander 0x38 |
| Napajanje | 4 × AA, ulaz 4.5–9 V, prekidač ON/OFF |
| Zaglavlje | 3v3, 5V, V_BAT, V_LED, GND, EOUT7 |
| Namjena | Pogon i izvor napajanja za cijeli sistem |

### 1.6 Baterije: 4 × AA

![Baterije](images/baterije-aa.jpg)

| Svojstvo | Vrijednost |
|---|---|
| Namjena | Jedini izvor napajanja za cijeli sistem |
| Napon | Oko 4–6.4 V (4 × 1.5 V) |
| Napomena | Pad napona pod opterećenjem, vidi `hardware.md`, odjeljak 4.3 |

### 1.7 Elektromagnetski top (coilgun)

![Top](images/top.jpg)

| Svojstvo | Vrijednost |
|---|---|
| Namjena | Ispaljivanje projektila |
| Signali | CHARGE, FIRE, VCAP_SENSE (vidi `hwsw-interface.md`, odjeljak 5) |
| Status | **Još nije isporučen**, rezervisan konektor i pinovi |

---

## 2. Naša ploča (PCB)

![PCB](images/pcb.jpg)

| Dio | Opis | Napomena |
|---|---|---|
| Ivični konektor (zlatni prsti) | Zamjenjuje micro:bit u utoru JoyCar ploče | 1.6 mm, ENIG, raspored sa stvarnog micro:bita |
| Priključak napajanja | 3v3, 5V, V_BAT, GND sa JoyCar zaglavlja | Konektor ili žice |
| Programski konektor | MCLR, PGD1, PGC1 | Za programer |
| Konektor topa | VBAT, GND, CHARGE, FIRE, VCAP_SENSE | Rezervisano |

---

## 3. Pomoćne komponente

| Komponenta | Namjena | Količina | Napomena |
|---|---|---|---|
| Kondenzatori 100 nF | Blokiranje napajanja uz svaki pin napajanja | **TBD** | |
| Kondenzator 100–470 µF | Uz ESP8266, šiljci struje pri slanju | 1 | **TBD** vrijednost |
| Kondenzator VCAP (npr. 10 µF) | Jezgro PIC-a | 1 | Prema datasheetu |
| Otpornici za pull-up | I2C (SDA, SCL), MCLR, EN modula ESP8266 | **TBD** | Provjeriti postoje li već na JoyCar ploči |
| Serijski otpornik (npr. 1 kΩ) | Zaštita ulaza ECHO | 1 | |
| Dugme | START/STOP | 1 | |
| LED | Status | 1 | |
| Razdjelnik napona i Zener dioda | Mjerenje napona kondenzatora topa | **TBD** | |
| Programer za PIC | Programiranje mikrokontrolera | 1 | **TBD** model |
| Utičnica 2 × 4 | Za ESP-01 | 1 | |

---

## 4. Pregled napajanja i interfejsa

| Komponenta | Napon | Interfejs | Pinovi PIC-a |
|---|---|---|---|
| ESP-01 | 3.3 V | UART | RB12, RB13 |
| HC-SR04 | 5 V | TRIG, ECHO | RB6, RB7 |
| IR Distance Click | 3.3 V | OUT, EN | RB2, RB3 |
| JoyCar (motori, senzori) | V_BAT | I2C | RB8, RB9 |
| Top | **TBD** | CHARGE, FIRE, VCAP_SENSE | RA3, RA4, RA0 |

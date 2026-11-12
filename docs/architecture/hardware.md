# Hardverska arhitektura

**Projekat:** Autonomna mobilna platforma sa integrisanim elektromagnetskim topom
**Verzija:** 0.4 (nacrt)
**Svrha:** Opis hardverskih blokova ploče, njihovih veza, napajanja i konektora. Tačne pinove, I2C adrese i protokol definiše dokument `hwsw-interface.md`, pa se ovdje ne ponavljaju.

Vrijednosti označene sa **TBD** treba potvrditi mjerenjem ili datasheetom.

---

## 1. Pregled

Naša ploča (PCB) **zamjenjuje micro:bit** na JoyCar platformi: ubacuje se u ivični utor JoyCar ploče. JoyCar ploča već ima drajvere motora, senzore i vezu sa baterijama. Naša ploča ima mikrokontroler, WiFi modul, priključke za senzore, servo motor kule i interfejs za elektromagnetski top.

Osnovna pravila:
- Cijeli sistem radi sa **jednog izvora**, 4 × AA baterije na JoyCar ploči.
- Ploča **nema vlastite regulatore**: napone 3v3, 5V i V_BAT uzima sa zaglavlja JoyCar ploče.
- Motorima se upravlja komandama preko **I2C** (PIC je master).
- Top, senzor za metu i servo su na **okretnoj kuli**, a senzor za prepreke je fiksiran na šasiji.

---

## 2. Blok dijagram

```mermaid
flowchart TB
    APP["PC aplikacija"]
    ESP["ESP-01 (ESP8266)"]
    MCU["PIC24FJ64GA702"]
    CONN["Konektor topa"]

    subgraph JC["JoyCar ploča (4 × AA, napajanje svega)"]
        USH["US zaglavlje<br/>P8 TRIG, P12 ECHO"]
        HCSR["HC-SR04<br/>(senzor za prepreke)"]
        PWM["PWM kontroler 0x70<br/>+ drajveri"]
        IOX["IO ekspander 0x38"]
        MOT["Motori"]
        SRV["Izlaz za servo<br/>(P1)"]
    end

    subgraph KULA["Okretna kula"]
        SERVO["Servo motor kule"]
        IR["IR Distance Click<br/>(senzor za metu)"]
        TOP["Elektromagnetski top<br/>(još nije dostupan)"]
    end

    APP <-.->|"WiFi, TCP"| ESP
    ESP <-->|"UART 115200"| MCU
    MCU <-->|"I2C"| PWM
    MCU <-->|"I2C"| IOX
    PWM --> MOT
    MCU <-->|"TRIG / ECHO"| USH
    USH --- HCSR
    MCU -->|"PWM"| SRV
    SRV --> SERVO
    IR -->|"OUT analogno"| MCU
    MCU -->|"EN"| IR
    MCU -->|"CHARGE, FIRE"| CONN
    CONN -->|"VCAP_SENSE"| MCU
    CONN --- TOP
```

---

## 3. Komponente

| Blok | Komponenta | Napon | Interfejs | Namjena |
|---|---|---|---|---|
| Mikrokontroler | PIC24FJ64GA702 (SOIC-28) | 3.3 V | I2C, UART, ADC, GPIO | Upravljanje, mašine stanja |
| WiFi | ESP8266 (ESP-01) | 3.3 V | UART, AT komande | Veza sa aplikacijom |
| Senzor za prepreke | HC-SR04 | 5 V | TRIG, ECHO (5 V signal) | Detekcija prepreka, na šasiji |
| Senzor za metu | IR Distance Click (Sharp GP2Y0A60SZ0F) | 3.3 V | analogni OUT, EN | Detekcija mete, 10–150 cm, na kuli |
| Kula | Servo motor SG90 | oko 5 V (**TBD**) | PWM 50 Hz | Okretanje kule |
| Motori i drajveri | JoyCar ploča | V_BAT | I2C, 0x70 | Kretanje |
| Izvor | 4 × AA baterije | oko 4–6.4 V | zaglavlje | Napajanje svega |
| Top | Elektromagnetski top | **TBD** | CHARGE, FIRE, VCAP_SENSE | Gađanje, na kuli (još nije dostupan) |

Razlozi izbora su u `hwsw-interface.md`, odjeljak 1 i 3.1.

---

## 4. Napajanje

### 4.1 Struktura

```mermaid
flowchart LR
    B["4 × AA"] --> S["ON/OFF prekidač<br/>JoyCar"]
    S --> H["Zaglavlje JoyCar"]
    H -->|3v3| L["PIC, ESP8266, IR Click"]
    H -->|5V| U["HC-SR04"]
    H -->|V_BAT| T["Punjač topa"]
    S --> M["Drajveri motora"]
    S -.->|"TBD"| SV["Servo SG90"]
```

Naponi se ne stvaraju na našoj ploči. Pitanje je **koliko struje zaglavlje JoyCar ploče može dati**.

### 4.2 Potrošači

Struje nisu izmjerene (**TBD**), a treba ih provjeriti na prototipu:
- PIC24FJ64GA702 (3.3 V)
- ESP8266 (3.3 V), ima najveću potrošnju i skokove struje pri slanju
- IR Distance Click (3.3 V)
- HC-SR04 (5 V)
- Servo motor SG90 (napajanje oko 5 V, a odakle ga napajati je **TBD**; V_BAT sa svježim baterijama može preći 6 V)
- Punjenje topa (V_BAT)

### 4.3 Rizici

- **Pad napona baterije.** Motori, servo i punjenje topa povlače veliku struju, pa napon može pasti i resetovati PIC. Zato punjenje topa ide ograničenom strujom i samo dok vozilo stoji, a napon baterije se mjeri na RA1.
- **Kapacitet zaglavlja 3v3.** Ako JoyCar ne daje dovoljno struje za ESP8266, treba dodati kondenzator ili drugo rješenje.
- **Zajednička masa.** Sve komponente dijele GND sa JoyCar pločom, a uz napajanje svakog čipa ide kondenzator.

---

## 5. Pinovi

Potpuna tabela pinova je u `hwsw-interface.md`, odjeljak 3. Pregled grupa:

| Grupa | Pinovi | Veza |
|---|---|---|
| Programiranje | MCLR, RB0 (PGD1), RB1 (PGC1) | Programski konektor |
| I2C (JoyCar) | RB8 (SCL1), RB9 (SDA1) | PWM kontroler 0x70, IO ekspander 0x38 |
| UART (WiFi) | RB12 (RX), RB13 (TX) | ESP8266 |
| HC-SR04 | RB6 (TRIG), RB7 (ECHO, INT0) | JoyCar US zaglavlje |
| IR Distance Click | RB2 (OUT, AN4), RB3 (EN) | Senzor za metu |
| Servo kule | RB14 (PWM) | JoyCar izlaz za servo (P1) |
| Baterija | RA1 (AN1) | JoyCar kontakt P2 |
| Top | RA0 (napon kondenzatora), RA3 (punjenje), RA4 (ispaljivanje), RB4 (rezerva) | Konektor topa |
| Korisnički | RB5 (dugme), RA2 (LED) | |
| Rezerva | RB10, RB11 (slobodni), RB15 (zujalica, opciono) | |

Pin za servo (RB14) je usklađen sa `software.md` i upisan u `hwsw-interface.md` (v0.10).

---

## 6. Konektori i PCB

| Konektor | Namjena | Napomena |
|---|---|---|
| Ivični konektor (zlatni prsti) | Ulazi u micro:bit utor JoyCar ploče: P1 (servo), P2, P8, P12, P19, P20, 3V, GND, opciono P0 i P16 | 1.6 mm ploča, ENIG, raspored sa stvarnog micro:bita |
| Napajanje sa zaglavlja | 3v3, 5V, V_BAT, GND | Posebnim konektorom ili žicama |
| Programski konektor | MCLR, PGD1, PGC1, 3.3 V, GND | Za programer |
| ESP-01 | UART, 3.3 V, EN | Utičnica 2 × 4 |
| IR Distance Click | OUT, EN, 3.3 V, GND | Utičnica ili žice, vodi na kulu |
| Konektor topa | VBAT, GND, CHARGE, FIRE, VCAP_SENSE, rezerva | Raspored u `hwsw-interface.md`, odjeljak 5 |

PCB:
- Dvoslojna ploča, KiCad 8, fabrikacija u Kini.
- Oblik i dimenzije odgovaraju JoyCar platformi i micro:bit utoru (**TBD**, mjerenje).
- Test tačke na I2C, UART, 3.3 V, 5 V i V_BAT.
- Neugrađena mjesta za pull-up otpornike na SDA i SCL i premosnice na napajanju, da se greške mogu popraviti bez nove ploče.
- Top, senzor za metu i servo su na kuli, pa kablovi do njih moraju biti savitljivi.

---

## 7. Top: interfejs

Top još nije dostupan, pa se unaprijed predviđa samo interfejs:
- **CHARGE** (izlaz): uključuje punjenje kondenzatora.
- **FIRE** (izlaz): okida ispaljivanje, samo uz potvrđeno punjenje.
- **VCAP_SENSE** (ulaz, ADC): napon kondenzatora preko razdjelnika.
- Bleeder otpornik za pražnjenje, prekidač i osigurač na napajanju topa.
- Ograničenje napona kondenzatora (**TBD**).
- Jumperi na linijama topa, da se top može odvojiti od ploče.

---

## 8. Provjere na prototipu

| # | Šta provjeriti | Zašto |
|---|---|---|
| 1 | Napon i struja 3v3 i 5V zaglavlja pod opterećenjem (WiFi, motori, servo) | Da li zaglavlje izdržava sve potrošače |
| 2 | Pad napona baterije pod opterećenjem | Prag za zaštitu |
| 3 | SDA i SCL: logički nivo i pull-up otpornici | Da li dodati pull-up |
| 4 | Geometrija ivičnog konektora (razmak, širina i dužina prstiju) | Footprint |
| 5 | Napon na P2 (jumper BAT) | Skaliranje ADC-a |
| 6 | Smjerovi motora i raspored PWM kanala | Provjera tabele u `hwsw-interface.md` |
| 7 | Napajanje i struja servo motora kule | Da li ga zaglavlje može napajati |

---

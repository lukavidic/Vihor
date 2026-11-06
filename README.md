# Vihor

Autonomna mobilna platforma sa integrisanim elektromagnetskim topom. Vozilo se zasniva na JoyCar platformi (Joy-IT), a naša ploča (PCB) zamjenjuje micro:bit i upravlja kretanjem, senzorima, topom i WiFi komunikacijom. Vozilo autonomno traži i gađa pokretnu metu na suparničkom vozilu istog tipa, izbjegava prepreke, a START/STOP i log poruke ide preko WiFi-ja do aplikacije na računaru.

Projekat iz računarske elektronike (RE/MKS 26/27).

## Sistem u kratkim crtama

| Dio | Opis |
|---|---|
| Mikrokontroler | PIC24FJ64GA702 |
| WiFi | ESP8266 (ESP-01), AT komande preko UART-a |
| Senzori | HC-SR04 (prepreke), IR Distance Click (meta) |
| Vozilo | JoyCar (motori i drajveri na JoyCar ploči, I2C) |
| Napajanje | 4 × AA baterije, naponi sa zaglavlja JoyCar ploče |
| Top | Elektromagnetski top (coilgun), još nije isporučen |
| Aplikacija | Java (JavaFX), TCP server, komande START i STOP, prikaz logova |

## Dokumentacija

| Dokument | Sadržaj |
|---|---|
| [Specifikacija zahtjeva](docs/Specifikacija-zahtjeva.md) | Funkcionalni i nefunkcionalni zahtjevi, mašine stanja, format logova |
| [HW/SW interfejs](docs/architecture/hwsw-interface.md) | Napajanje, pinovi mikrokontrolera, JoyCar interfejs, komunikacioni protokol |
| [Hardverska arhitektura](docs/architecture/hardware.md) | Blok dijagram, napajanje, konektori, mjerenja |
| [Softverska arhitektura](docs/architecture/software.md) | Moduli firmvera, mašine stanja, aplikacija |
| [Spisak komponenti](docs/components.md) | Sve komponente sa slikama i osnovnim podacima |


## Organizacija rada

- Zadaci se vode na GitHub Projects tabli **Vihor SCRUM Board**, svaki zadatak je issue sa labelom `hardware` ili `software`.
- Grana `main` sadrži stabilnu verziju, a rad se objedinjuje u granu `dev`.
- Za svaki zadatak se pravi nova grana iz `dev`, imenovana kao `<broj-issuea>-kratak-opis` (npr. `3-hardverska-arhitektura-sistema`).
- Izmjene idu preko Pull Requesta prema `dev`, uz pregled barem jedne osobe. U opisu se navodi `Refs #<broj>`, a issue se zatvara ručno kad je zadatak gotov.
- Dokumenti nose broj verzije u zaglavlju, a ne u imenu grane ili fajla.

## Struktura repozitorija

```
.
├── README.md
├── LICENSE
└── docs/
    ├── Specifikacija-zahtjeva.md
    ├── components.md
    ├── images/                 slike komponenti
    └── architecture/
        ├── hwsw-interface.md
        ├── hardware.md
        └── software.md
```

Kad se dodaju hardverski projekat (KiCad), firmver i aplikacija, dobijaju svoje foldere (npr. `hardware/`, `firmware/`, `app/`).

## Licenca

Vidi [LICENSE](LICENSE).

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
| [Električna šema (PDF)](docs/Vihor.pdf) | Svi listovi šeme: naslovni list, blok šema i moduli |


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
├── .gitattributes              podešavanja gita za KiCad fajlove
├── .github/                    šabloni za issue i pull request
├── docs/
│   ├── Specifikacija-zahtjeva.md
│   ├── components.md
│   ├── Vihor.pdf               električna šema (svi listovi)
│   ├── images/                 slike komponenti
│   ├── resources/              dijagrami (mašine stanja, aktivnosti)
│   └── architecture/
│       ├── hwsw-interface.md
│       ├── hardware.md
│       └── software.md
├── hardware/
│   ├── Vihor/                  KiCad projekat (šema i ploča)
│   └── libs/                   simboli, footprinti i 3D modeli (PIC, ESP-01, ivični konektor)
└── firmware/
    └── tests/                  testni programi za EasyPIC i TCP server za test na računaru
```

Aplikacija dobija svoj folder (npr. `app/`) kad se doda u repozitorij.

## Stanje projekta

- **Dokumentacija:** specifikacija zahtjeva, HW/SW interfejs, hardverska arhitektura i spisak komponenti su usklađeni sa šemom i rezultatima mjerenja. Softverska arhitektura i imena stanja u specifikaciji usklađuju se nakon dogovora sa SW timom.
- **Šema:** pripremljena za izradu ploče (PR #47); vrijednosti dijelova za top (R5, R7, D1, F1) još nisu određene.
- **Ispitivanje dijelova:** na razvojnoj ploči EasyPIC Fusion v7 (umjesto našeg PIC-a) ispitani su ESP-01 (AT komande, WiFi, TCP, START i STOP), motori preko I2C i servo motor; testni programi su u `firmware/tests/`. IR senzor je izmjeren multimetrom (napon izlaza u zavisnosti od udaljenosti), a kod ultrazvučnog senzora su izmjereni signali TRIG i ECHO na JoyCar ploči. Rezultati su u `docs/architecture/hwsw-interface.md`.
- **Top:** još nije isporučen; interfejs je predviđen na ploči.

## Licenca

Vidi [LICENSE](LICENSE).

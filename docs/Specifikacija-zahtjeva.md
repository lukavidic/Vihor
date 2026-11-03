# Specifikacija zahtjeva (Requirements specification)
 
**Projekat:** Autonomna mobilna platforma sa integrisanim elektromagnetskim topom
**Verzija:** 0.4 (nacrt)
**Napomena:** Vrijednosti označene sa **TBD** potrebno je potvrditi sa predmetnim profesorom ili mjerenjem u toku razvoja.
 
---
 
## 1. Uvod
 
### 1.1 Svrha dokumenta
Dokument definiše funkcionalne i nefunkcionalne zahtjeve za autonomno vozilo zasnovano na JoyCar platformi, sa upravljačkom pločom koja ima integrisan elektromagnetski top (coilgun). Služi kao polazna tačka za projektovanje hardvera, firmvera i aplikacije.
 
### 1.2 Opis sistema
Sistem se sastoji od:
- **vozila** zasnovanog na JoyCar platformi, sa vlastitom upravljačkom pločom (PCB),
- **elektromagnetskog topa**, **mete** i **ToF senzora za metu**, koji su zajedno fiksirani na vozilu u istom smjeru i okreću se zajedno sa vozilom,
- **dva fiksna ToF senzora**: jedan na visini mete za detekciju suparničke mete, a drugi nisko na šasiji za detekciju prepreka,
- **WiFi modula** za komunikaciju,
- **PC/mobilne aplikacije** za pokretanje, zaustavljanje i prijem log poruka.
Vozilo autonomno pozicionira sebe i gađa **pokretnu metu** koja se nalazi na suparničkom autonomnom vozilu istog tipa.
 
### 1.3 Definicije
| Pojam | Značenje |
|---|---|
| ToF | Time-of-Flight, senzor za mjerenje udaljenosti |
| Coilgun | Elektromagnetski top, zavojnica koja ubrzava feromagnetni projektil |
| Meta | Detekciona površina na suparničkom vozilu, viša od prepreka na polju |
| Log poruka | Tekstualna poruka koju vozilo šalje aplikaciji |
| MCU | Mikrokontroler |
 
### 1.4 Učesnici
- **Korisnik/operater:** pokreće i zaustavlja vozilo i prati logove u aplikaciji.
- **Vozilo (sistem):** autonomno izvršava zadatak.
- **Suparničko vozilo:** pokretna meta i prepreka.
---
 
## 2. Funkcionalni zahtjevi
 
### 2.1 Upravljanje vozilom i kretanje
| ID | Zahtjev |
|---|---|
| FR-01 | Vozilo se kreće autonomno nakon komande START i zaustavlja se na komandu STOP. |
| FR-02 | Vozilo upravlja motorima JoyCar platforme (naprijed, nazad, skretanje, zaustavljanje). |
 
### 2.2 Detekcija prepreka i kontrola kolizije
| ID | Zahtjev |
|---|---|
| FR-03 | Vozilo ima dva fiksna ToF senzora, svaki sa jednom namjenom: **nisko postavljen senzor** za detekciju prepreka i **senzor na visini mete** za detekciju mete. Meta je viša od prepreka, pa senzor za metu ne reaguje na prepreke. |
| FR-04 | Kada nisko postavljeni ToF detektuje prepreku bližu od praga (**TBD** mm), vozilo se zaustavlja ili je zaobilazi. Izbjegavanje prepreke ima prioritet nad komandama kretanja koje dolaze iz traženja mete. |
 
### 2.3 Detekcija i praćenje mete
| ID | Zahtjev |
|---|---|
| FR-05 | Vozilo traži pokretnu metu (suparničko vozilo) okretanjem u mjestu ili kretanjem (**TBD**) i detektuje je ToF senzorom za metu. |
| FR-06 | Kada je meta detektovana, vozilo se okreće i pozicionira tako da je meta u liniji topa. Top i senzor za metu su fiksirani u istom smjeru, pa je poravnanje senzora ujedno i poravnanje topa. |
 
### 2.4 Elektromagnetski top
| ID | Zahtjev |
|---|---|
| FR-07 | Ploča puni kondenzatore topa do radnog napona (**TBD** V) i mjeri njihov napon. |
| FR-08 | Top ispaljuje projektil kada je meta poravnata i kondenzator napunjen, uz minimalni razmak između ispaljivanja (**TBD** s). |
 
### 2.5 Komunikacija i aplikacija
| ID | Zahtjev |
|---|---|
| FR-09 | Vozilo se povezuje na WiFi mrežu i uspostavlja vezu sa aplikacijom. |
| FR-10 | Aplikacija ima komande **START** i **STOP**. |
| FR-11 | Vozilo šalje log poruke (stanje, udaljenost, ispaljivanje, greške), a aplikacija ih prikazuje u realnom vremenu. |
 
### 2.6 Stanja sistema
Sistem radi kao **dvije nezavisne mašine stanja**, koje rade istovremeno. Komanda START pokreće obje, a STOP ih vraća u stanje IDLE.
 
**Vozilo (šasija)**, koristi nisko postavljeni ToF:
 
`IDLE → DRIVE ⇄ AVOID`
 
- DRIVE: vozilo se kreće.
- AVOID: prepreka je bliža od praga, pa se vozilo zaustavlja ili je zaobilazi, a zatim se vraća u DRIVE.
**Meta i top**, koristi ToF za metu:
 
`IDLE → SEARCH → TRACK → FIRE → SEARCH`
 
- SEARCH: vozilo traži metu (okreće se ili kreće).
- TRACK: meta je pronađena, vozilo se okreće dok nije poravnata sa topom. Ako se meta izgubi, vraća se u SEARCH.
- FIRE: meta je poravnata i kondenzator napunjen, pa top ispaljuje projektil, a zatim se vraća u SEARCH.
Obje mašine upravljaju istim motorima. Stanje AVOID ima prioritet: dok je aktivno, motorima upravlja izbjegavanje prepreke, a komande iz mašine mete se ignorišu. Kondenzator se puni u pozadini, bez posebnog stanja, a minimalni razmak između ispaljivanja je uslov u zahtjevu FR-08.
 
---
 
## 3. Nefunkcionalni zahtjevi
 
### 3.1 Performanse
| ID | Zahtjev |
|---|---|
| NFR-01 | **Odziv:** reakcija na prepreku u roku od **TBD** ms, izvršenje STOP komande u roku od **TBD** ms, a vrijeme od detekcije mete do poravnanja i ispaljivanja je najviše **TBD** s. |
| NFR-02 | **Tačnost:** efektivni domet i procenat pogodaka određuju se mjerenjem (**TBD**). |
 
### 3.2 Sigurnost
| ID | Zahtjev |
|---|---|
| NFR-03 | **Sigurnost:** napon kondenzatora je ograničen (**TBD**, preporuka ≤ 50 V), uz automatsko pražnjenje i prekidač za isključenje. Ispaljivanje nije moguće bez potvrđenog punjenja. |
 
### 3.3 Pouzdanost i otpornost na greške
| ID | Zahtjev |
|---|---|
| NFR-04 | **Failsafe:** pri gubitku WiFi veze vozilo se zaustavlja, a top se razoružava. |
| NFR-05 | **Pouzdanost:** greške se bilježe i šalju kao log poruke, a firmver koristi watchdog timer. |
 
### 3.4 Hardverska ograničenja
| ID | Zahtjev |
|---|---|
| NFR-06 | **Hardver:** ploča se uklapa u JoyCar platformu (dimenzije, napon baterije, konektori), dvoslojna je i izrađena u KiCadu. Napajanje logike je odvojeno od motora i topa. Oba ToF senzora dijele I2C magistralu, uz različite adrese (npr. preko XSHUT pina). |
 
### 3.5 Softverska ograničenja i održivost
| ID | Zahtjev |
|---|---|
| NFR-07 | **Firmver:** modularan (WiFi, ToF, motori, top, mašina stanja), dokumentovan Doxygen komentarima. |
| NFR-08 | **Aplikacija:** radi na standardnom PC-u (**TBD**) i prikazuje samo START, STOP i prozor sa logovima. |
| NFR-09 | **Održivost:** projekat je u Git repozitoriju, a podešavanja (WiFi, IP, pragovi) u jednoj konfiguracionoj datoteci. |
 
### 3.6 Okruženje
| ID | Zahtjev |
|---|---|
| NFR-10 | **Okruženje:** rad u zatvorenom prostoru, na ravnoj podlozi, uz stabilnu WiFi mrežu. |
 
---
 
## 4. Format log poruka (prijedlog)
 
```
[vrijeme s] NIVO modul: poruka
[12.4s] INFO  STATE=TRACK dist=830mm
[13.1s] INFO  FIRE
[13.2s] ERROR CAP voltage low
```
 
Komande aplikacije: `START\n`, `STOP\n`. Poruke se završavaju znakom novog reda.
 
---
 
## 5. Use-case dijagram
 
```mermaid
flowchart LR
    Op([Operater])
    Veh([Vozilo])
    Opp([Suparničko vozilo])
 
    subgraph Sistem["Aplikacija i vozilo"]
        UC1(Pokreni vozilo - START)
        UC2(Zaustavi vozilo - STOP)
        UC3(Pregledaj log poruke)
        UC4(Autonomno se kreći)
        UC5(Izbjegni koliziju)
        UC6(Traži i prati metu)
        UC7(Napuni top)
        UC8(Ispali projektil)
    end
 
    Op --> UC1
    Op --> UC2
    Op --> UC3
    Veh --> UC4
    Veh --> UC5
    Veh --> UC6
    Veh --> UC7
    Veh --> UC8
    Opp -.-> UC6
    UC4 -. uključuje .-> UC5
    UC8 -. uključuje .-> UC7
    UC8 -. uključuje .-> UC6
```
 
---
 

## 6. Praćenje (traceability)
 
| Funkcionalnost iz zadatka | Zahtjevi |
|---|---|
| Autonomno vozilo na JoyCar platformi | FR-01, FR-02 |
| Pozicioniranje i gađanje pokretne mete | FR-05, FR-06, FR-08 |
| Elektromagnetski top | FR-07, FR-08 |
| Upravljanje vozilom, kontrola kolizije, ToF | FR-02, FR-03, FR-04 |
| WiFi: START/STOP i log poruke | FR-09, FR-10, FR-11 |

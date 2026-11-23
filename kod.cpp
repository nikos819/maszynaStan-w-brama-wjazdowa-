
#include <IRremote.hpp>
#include <Servo.h>

// STANY
#define ST_ZAMKNIETA       1
#define ST_OTWARTA         2
#define ST_UCHYLONA_ZAM    3
#define ST_UCHYLONA_OTW    4
#define ST_ZAMYKANIE       5
#define ST_OTWIERANIE      6

// PINY
#define PIN_IR 2
#define PIN_PIR 3
#define PIN_SERWO1 9
#define PIN_SERWO2 10
#define PIN_LED 7

// CZAS PELNEGO RUCHU
const unsigned long CZAS_RUCHU = 10000;

// KOD PILOTA
const uint8_t KOD_PILOTA = 0x00;

// SERWA
Servo serwo1;
Servo serwo2;

// ZMIENNE
uint8_t state = ST_ZAMKNIETA;

uint8_t poprzedniRuch = 0;

unsigned long czasStartu = 0;
unsigned long ostatniRuch = 0;
unsigned long ostatnieOdswiezanie = 0;

unsigned long pozostalyCzas = 0;

bool pauzaPIR = false;

int pozycja = 0;
int pozycjaStart = 0;
int pozycjaCel = 0;

// POZYCJE SERW
const int POZ_ZAMKNIETA = 0;
const int POZ_OTWARTA = 90;

// CZESTOTLIWOSC AKTUALIZACJI SERW
const unsigned long INTERWAL_SERWA = 20;


// USTAWIENIE SERW
void ustawSerwa(int kat)
{
  serwo1.write(kat);
  serwo2.write(180 - kat);
}


// NAZWY STANOW
void pokazStan()
{
  switch (state)
  {
    case ST_ZAMKNIETA:
      Serial.println("stan: ZAMKNIETA");
      break;

    case ST_OTWARTA:
      Serial.println("stan: OTWARTA");
      break;

    case ST_UCHYLONA_ZAM:
      Serial.println("stan: UCHYLONA_ZAM");
      break;

    case ST_UCHYLONA_OTW:
      Serial.println("stan: UCHYLONA_OTW");
      break;

    case ST_ZAMYKANIE:
      Serial.println("stan: ZAMYKANIE");
      break;

    case ST_OTWIERANIE:
      Serial.println("stan: OTWIERANIE");
      break;
  }
}


// ROZPOCZECIE RUCHU
void rozpocznijRuch(uint8_t nowyStan, int cel)
{
  state = nowyStan;

  pozycjaStart = pozycja;
  pozycjaCel = cel;

  czasStartu = millis();
  ostatniRuch = millis();
  ostatnieOdswiezanie = millis();

  pozostalyCzas = CZAS_RUCHU;

  pauzaPIR = false;
  poprzedniRuch = 0;

  pokazStan();
}


// WZNOWIENIE RUCHU PO PIR
void wznowRuch()
{
  state = poprzedniRuch;

  // Aktualna pozycja staje sie nowym poczatkiem
  pozycjaStart = pozycja;

  // Zachowanie poprzedniego celu
  if (state == ST_OTWIERANIE)
  {
    pozycjaCel = POZ_OTWARTA;
  }
  else
  {
    pozycjaCel = POZ_ZAMKNIETA;
  }

  // Obliczenie czasu, ktory juz uplynal
  czasStartu = millis() - (CZAS_RUCHU - pozostalyCzas);

  ostatniRuch = millis();
  ostatnieOdswiezanie = millis();

  pauzaPIR = false;

  Serial.println("WZNOWIENIE RUCHU");

  pokazStan();
}


// OBSLUGA CZUJNIKA PIR
void obslugaPIR()
{
  if (digitalRead(PIN_PIR) == HIGH)
  {
    if (state == ST_OTWIERANIE ||
        state == ST_ZAMYKANIE)
    {
      // Zapamietanie kierunku
      poprzedniRuch = state;

      // Obliczenie pozostalego czasu
      unsigned long uplynelo = millis() - czasStartu;

      if (uplynelo >= CZAS_RUCHU)
        pozostalyCzas = 0;
      else
        pozostalyCzas = CZAS_RUCHU - uplynelo;

      // Zatrzymanie bramy
      pauzaPIR = true;

      if (state == ST_OTWIERANIE)
      {
        state = ST_UCHYLONA_OTW;
      }
      else
      {
        state = ST_UCHYLONA_ZAM;
      }

      ustawSerwa(pozycja);

      Serial.println("PIR: WYKRYTO RUCH!");
      Serial.println("BRAMA ZATRZYMANA");

      pokazStan();
    }
  }
}


// OBSLUGA PILOTA
void obslugaPilota()
{
  if (IrReceiver.decode())
  {
    uint8_t kod = IrReceiver.decodedIRData.command;

    if (kod == KOD_PILOTA)
    {
      Serial.println("sygnal");

      // Blokada ruchu podczas wykrywania ruchu przez PIR
      if (digitalRead(PIN_PIR) == LOW)
      {
        switch (state)
        {
          case ST_ZAMKNIETA:
            rozpocznijRuch(ST_OTWIERANIE, POZ_OTWARTA);
            break;

          case ST_OTWARTA:
            rozpocznijRuch(ST_ZAMYKANIE, POZ_ZAMKNIETA);
            break;

          case ST_OTWIERANIE:
            poprzedniRuch = 0;
            pauzaPIR = false;

            state = ST_UCHYLONA_OTW;

            ustawSerwa(pozycja);
            pokazStan();
            break;

          case ST_ZAMYKANIE:
            poprzedniRuch = 0;
            pauzaPIR = false;

            state = ST_UCHYLONA_ZAM;

            ustawSerwa(pozycja);
            pokazStan();
            break;

          case ST_UCHYLONA_OTW:
            if (pauzaPIR &&
                poprzedniRuch == ST_OTWIERANIE)
            {
              wznowRuch();
            }
            else if (!pauzaPIR)
            {
              rozpocznijRuch(ST_ZAMYKANIE, POZ_ZAMKNIETA);
            }
            break;

          case ST_UCHYLONA_ZAM:
            if (pauzaPIR &&
                poprzedniRuch == ST_ZAMYKANIE)
            {
              wznowRuch();
            }
            else if (!pauzaPIR)
            {
              rozpocznijRuch(ST_OTWIERANIE, POZ_OTWARTA);
            }
            break;
        }
      }
    }

    IrReceiver.resume();
  }
}


// AKTUALIZACJA RUCHU
void aktualizujRuch()
{
  if (state != ST_OTWIERANIE &&
      state != ST_ZAMYKANIE)
  {
    return;
  }

  unsigned long teraz = millis();
  unsigned long uplynelo = teraz - czasStartu;

  // KONIEC RUCHU
  if (uplynelo >= CZAS_RUCHU)
  {
    pozycja = pozycjaCel;
    ustawSerwa(pozycja);

    if (state == ST_OTWIERANIE)
      state = ST_OTWARTA;
    else
      state = ST_ZAMKNIETA;

    pauzaPIR = false;
    poprzedniRuch = 0;

    pokazStan();
    return;
  }

  // OBLICZANIE POZYCJI NA PODSTAWIE CZASU
  if (teraz - ostatniRuch >= INTERWAL_SERWA)
  {
    ostatniRuch = teraz;

    float postep = (float)uplynelo / CZAS_RUCHU;

    pozycja = pozycjaStart +
      (int)((pozycjaCel - pozycjaStart) * postep);

    ustawSerwa(pozycja);
  }
}


// ODLiCZANIE NA MONITORZE
void pokazOdliczanie()
{
  if (state != ST_OTWIERANIE &&
      state != ST_ZAMYKANIE)
  {
    return;
  }

  unsigned long teraz = millis();

  if (teraz - ostatnieOdswiezanie >= 1000)
  {
    ostatnieOdswiezanie = teraz;

    unsigned long uplynelo = teraz - czasStartu;
    unsigned long pozostalo =
      (CZAS_RUCHU - uplynelo + 999) / 1000;

    Serial.print("Pozostalo: ");
    Serial.print(pozostalo);
    Serial.println(" sekund");
  }
}


// LED
void aktualizujLED()
{
  digitalWrite(PIN_LED,
    (state == ST_OTWIERANIE ||
     state == ST_ZAMYKANIE) ? HIGH : LOW);
}


// SETUP
void setup()
{
  Serial.begin(9600);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PIR, INPUT);

  serwo1.attach(PIN_SERWO1);
  serwo2.attach(PIN_SERWO2);

  pozycja = POZ_ZAMKNIETA;
  ustawSerwa(pozycja);

  IrReceiver.begin(PIN_IR, ENABLE_LED_FEEDBACK);

  state = ST_ZAMKNIETA;

  Serial.println("==========================");
  Serial.println("brama uruchomiona");

  pokazStan();
}


// LOOP
void loop()
{
  // PIR ma najwyzszy priorytet
  obslugaPIR();

  obslugaPilota();

  aktualizujRuch();

  pokazOdliczanie();

  aktualizujLED();
}

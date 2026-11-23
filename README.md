# Symulacja bramy automatycznej – Arduino Uno R3

## Opis projektu

Projekt przedstawia symulację automatycznej bramy sterowanej za pomocą pilota na podczerwień. System wykorzystuje Arduino Uno R3, dwa mikroserwa oraz czujnik ruchu PIR.

Brama działa na zasadzie maszyny stanów (Finite State Machine), która kontroluje jej aktualne położenie i kierunek ruchu.

## Wykorzystane komponenty

- Arduino Uno R3
- Odbiornik podczerwieni IR
- Pilot IR
- 2x mikroserwo
- Czujnik ruchu PIR
- Dioda LED

## Funkcjonalności

- Sterowanie bramą za pomocą pilota IR.
- Otwieranie i zamykanie bramy.
- Zatrzymywanie bramy w trakcie ruchu.
- Wykrywanie ruchu za pomocą czujnika PIR.
- Wznawianie przerwanego ruchu po ponownym kliknięciu pilota.
- Kontrola czasu ruchu za pomocą funkcji millis().
- Sygnalizacja ruchu bramy za pomocą diody LED.
- Wyświetlanie aktualnego stanu i odliczania czasu w monitorze portu szeregowego.

## Maszyna stanów

System posiada 6 stanów:

- ZAMKNIETA
- OTWARTA
- UCHYLONA_ZAM
- UCHYLONA_OTW
- ZAMYKANIE
- OTWIERANIE

## Schemat połączeń

- Odbiornik IR – D2
- Czujnik PIR – D3
- Serwo 1 – D9
- Serwo 2 – D10
- Dioda LED – D7
- Masa – GND
- Zasilanie – 5V

## Technologie i biblioteki

- C++
- Arduino IDE
- IRremote
- Servo
- millis()

## Symulacja

Projekt został wykonany w środowisku Tinkercad.

## Cel projektu

Celem projektu jest praktyczne wykorzystanie Arduino, programowania w języku C++ oraz maszyny stanów do sterowania automatyczną bramą.

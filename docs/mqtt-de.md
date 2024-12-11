|Change Language: Deutsch, [English](mqtt.md)|

## MQTT Klient

#### MQTT Topics an die du senden kannst:

* tfmcu/+/cmd - Kommandos wie: up, down, stop, ...
     * Das Plus-Zeichen steht für Gruppe und Nummer des Empfängers (bei Bedarf auch ID):

```
       tfmcu/23/cmd       # Empfänger 3 aus Gruppe 2
       tfmcu/102030/cmd   # 6 stellige Hexadezimale-ID
       tfmcu/8090a023/cmd # 6 stellige Hexadezimale-ID + Gruppe + Empfänger


     Beispiele:

     FHEM Set-List Einträge:
        stop tfmcu/25/cmd $EVENT      # topic: 'tfmcu/25/cmd' data: 'stop'. Stoppt Empfänger 5 aus Gruppe 2.
        down tfmcu/101234/cmd $EVENT  # topic: 'tfmcu/101234/cmd' data: 'down'. Bewegt alle Empfänger an denen der Sender 101234 angemeldet ist.
        up tfmcu/25/cmd $EVENT        # topic: 'tfmcu/10/cmd' data: 'up'.  Bewegt alle Empfänger aus Gruppe 1.
        up tfmcu/25/cmd $EVENT        # topic: 'tfmcu/00/cmd' data: 'up'.  Bewegt die Empfänger aller Gruppen.
```


* tfmcu/+/pct - Positionen/Prozente 100 für offen, 0 für geschlossen,  ? zum abfragen der aktuellen Position

     * Das Plus-Zeichen hat die gleiche Bedeutung wie bei "tfmcu/+/cmd" oben beschrieben

     * Prozente zwischen 100 und 0 (wie 50%) funktionieren nur mit Empfängernummer aus einer Gruppe, nicht mit IDs.
     

* tfmcu/+/ipct - invertierte Positionen/Prozente 0 für offen, 100 für geschlossen,  ? zum abfragen der aktuellen Position

```
    Beispiel:
      topic: tfmcu/15/pct data: 33   # Bewege Empfänger 5 aus Gruppe 1 zur Position 33%
      topic: tfmcu/15/pct data: ?    # Frage aktuelle Position ab. Das Ergebnis wird im tfmcu/15/pct_out veröffentlich.
```


* tfmcu/cli  - Jedes [CLI](CLI.md) Kommmando kann an dieses Topic gesenet werden

     * Kommando könne in CLI text oder JSON format gesendet werden

     * Das Ergebnis des Kommandos wird im Topic "tfmcu/cli_out" im JSON format veröffentlicht

     * Wenn CLI text Format benutzt wird, dann jeweils nur ein Kommando und ohne Semikolon als Abschluss
     
     * Zur Vereinfachung mit FHEM Set-Liste: Du kannst Kommandos mit dem Wort 'cli' beginnen. Dieses wird ignoriert.


```
    Beispiel:
      topic: tfmcu/cli
         data: config all=?
         data: {"config":{"all":"?"}}
         data: cli mcu version=full
         data: {"mcu":{"version":"full"}}


     FHEM setlist:
       cli tfmcu/cli $EVENT          # data: 'config all=?'. This would send $EVENT 'cli config all=?', but the 'cli ' will be ignored.
       config tfmcu/cli $EVENT       # data: 'all=?'. This would send $EVENT 'config all=?'


```


 * tfmcu/gpo/+/level - send 0 or 1 to change level of output pin.  ? for query

```
    Example:
      topic: tfmcu/gpo/22/level data: 1   # Set pin to level 1 (High)
      topic: tfmcu/gpo/22/level data: ?   # Current pin-level will be published at topic tfmcu/gpi/22/level
```


#### MQTT Topics von denen du Daten/Ergebnisse erhalten kannst (Du kannst diese abonnieren/subscriben):

* tfmcu/cli_out - Ergebnis (in JSON Format) eines CLI Kommandos das an tfmcu/cli gesendet wurde.

* tfmcu/+/pct_out - Aktuelle Position eines Empfängers in Prozent (Plus Zeichen wie oben beschrieben), 100 für offen

* tfmcu/+/ipct_out - Aktuelle Position eines Empfängers in invertierten Prozent (Plus Zeichen wie oben beschrieben), 100 für geschlossen

* tfmc/gpi/+/level - Änderungen des Pegels (0,1) eines Eingangs-Pins (GPI) wenn dieser in der Konfiguration definiert wurde.

* tfmcu/rf/+/rx_out - Funk-Kommando (gesendet oder empfangen) als Text (stop, down, up, sun-down, sun-up, ...)

#### Beispiele

In der Weboberfläche (unter Fragezeichen->HSC) sind MQTT-Definitionen für HomeServer zu finde.

##### Beispiel: Definiere einen Empfänger mit FHEMs MQTT2_Device


~~~
defmod Rollo_25 MQTT2_DEVICE Rollo
attr Rollo_25 IODev mqtts
attr Rollo_25 autocreate 0
attr Rollo_25 readingList Rollo:tfmcu/status:.* status\
Rollo:tfmcu/25/pct_out:.* state
attr Rollo_25 room Tronferno
attr Rollo_25 setList stop:noArg tfmcu/25/cmd stop\
up:noArg tfmcu/25/cmd up\
down:noArg tfmcu/25/cmd down\
sun-down:noArg tfmcu/25/cmd sun-down\
pct:slider,0,5,100  tfmcu/25/pct $EVTPART1\
sun-auto:1,0 tfmcu/cli auto g=2 m=2 f=k  sun-auto=$EVTPART1
attr Rollo_25 webCmd up:stop:down:sun-down:pct
~~~

* mqtts - MQTT2_Server device name
* Rollo - MQTT Client ID configured in tronferno-mcu (default: tfmcu)
* 25 - group 2, motor 5

#### Info

* [MQTT client configuration](mcu_config.md)


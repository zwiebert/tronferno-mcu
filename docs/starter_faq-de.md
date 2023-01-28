## Tronferno FAQ Liste
  

#### Welche GPIOs muss ich in der Konfiguration einstellen?
In der Regel können relativ beliebige GPIOs in der Schaltung angeschlossen und entsprechend in der Konfiguration zugewiesen werden, solange diese vom verwendeten ESP32-Board nicht anderweitig beansprucht werden (z.B. von einem Ethernet Baustein auf dem Board).


#### Wozu können Handsender und Sonnensensoren in TronfernoMCU angemeldet und gespeichert werden?
Der Sinn dabei ist, die aktuelle Position eines Rollladens bestmöglich zu verfolgen. Leider gibt es mit Fernotron keine Möglichkeit einfach den Motor zu fragen welche Position er gerade hat. Also muss diese, soweit möglich, aus den Funk- und Timerdaten ermittelt werden.

#### Ich kann einen Sonnensensor nicht anmelden und er taucht nicht im Empfänger-Log auf wenn ich eine der Tasten drücke.
Vielleicht reicht die Sendeleistung nicht aus, da der Sensor aufgrund zu wenig Sonne noch nicht ausreichend aufgeladen wurde. Er kann zum Anmelden mit einer Lampe geladen werde, und/oder näher an den Empfänger gebracht werden.

#### Wie melde ich Handsender und Sonnensensoren in TronfernoMCU an?
Antwort: In Weboberfläche den Tab Zahnrad=>Alias öffnen. Den Button "Start Pairing" drücken. Nun auf dem Handsender STOP oder auf dem Sonnensensor: "SO POS INST" drücken. Das Gerät erscheint als ID in der Liste "All". Im darunter befindlichen Häkchen-Feld alle Empfänger auswählen welche dieser Sender bewegt (bei denen er mittels SET angemeldet wurde).

#### Wie stelle ich die Endpunkte ein.
* Weboberfläche Zahnrad->Empfänger->Endpunkte.
* Ein Hardwaretaster muss angeschlossen sein und konfiguriert sein:
   * Taster an einem GPIO oder GPI und GND anschließen.
   * Taster konfigurieren: Zahnrad->MCU->Allgemein->Elektrische-Ein-und-Ausgänge->Setz-Knopf
   * Bis GPIO33 des ESP32 wird der interne PullUp-Widerstand verwendet.
   * Reine GPIs (GPI34 und höher) brauchen einen externen PullUp-Widerstand (z.b. 1...10 kOhm) zwischen GPI und 3.3V.

#### Die internen Timer bewirken keine Veränderung bei der Positionsanzeige.
Die internen Timer müssen über TronfernoMCU programmiert worden sein, nicht die originale 2411. Es gibt keine Möglichkeit an die in der 2411 und den Motoren gespeicherten Timer-Daten zu gelangen.  Ebenso ist es unmöglich Positionsänderung aufgrund des internen Zufalls-Timers anzugzeigen.

#### Die Dämmerungsfunktion macht den Rollladen zu spät oder zu früh zu / Was macht Astro-Korrektur.
Die Astro/DÄmmerungs-Funktion verwendet eine Tabelle für ein Halbjahr und der Motor spiegelt diese intern für das andere Halbjahr. Leider passen diese Halbjahre nicht exakt zusammen. Es wird immer Tage geben mit falschen Zeiten. Man kann durch die Korrektur die Halbjahrs-Tabelle modifizieren, dass Zeiten gewählt werden die für beide Halbjahre einigermaßen passen. Entweder eine Zeit die zwischen beiden realen Zeiten liegt, oder eine Zeit die in beiden Halbjahrenn entweder  niemals vor/nach Dämmerung ist. Die korrigierte Tabelle muss dann zu dem betreffenden Rollladen übertragen werden um wirksam zu sein.  Die Vorhersage der Dämmerung in der Weboberfläche benutzt immer die aktuell voreingestellte Korrektur und die daraus erzeugte Tabelle.  Wenn die im Rollladen gespeichte Tabelle eine andere ist, dann werden die Vorhersage-Zeiten nicht stimmen.


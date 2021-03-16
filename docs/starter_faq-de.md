## Tronferno FAQ Liste
  

Frage: Welche GPIOs muss ich in der Konfiguration einstellen?
Antwort: In der Regel können relativ beliebige GPIOs in der Schaltung angeschlossen und entsprechend in der Konfiguration zugewiesen werden, solange diese vom verwendeten ESP32-Board nicht anderweitig beansprucht werden (z.B. von einem Ethernet Baustein auf dem Board).


Frage: Warum können Handsender und Sonnensensoren in TronfernoMCU angemeldet und gespeichert werden=
Antwort: Der Sinn dabei ist, die aktuelle Position eines Rollladens bestmöglich zu verfolgen. Leider gibt es mit Fernotron keine Möglichkeit einfach den Motor zu fragen welche Position er gerade hat. Also muss diese, soweit möglich, aus den Funk- und Timerdaten ermittelt werden.


Frage: Wie melde ich Handsender und Sonnensensoren in TronfernoMCU an?
Antwort: In Weboberfläche den Tab Zahnrad=>Alias öffnen. Den Button "Start Pairing" drücken. Nun auf dem Handsender STOP oder auf dem Sonnensensor: "SO POS INST" drücken. Das Gerät erscheint als ID in der Liste "All". Im darunter befindlichen Häkchen-Feld alle Empfänger auswählen welche dieser Sender bewegt (bei denen er mittels SET angemeldet wurde).
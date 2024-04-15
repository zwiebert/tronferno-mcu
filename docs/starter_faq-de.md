## Tronferno FAQ Liste
<details>
<summary>Das OTA Update schlägt immer fehl</summary>

Dies kam schon einige Male vor. Erklärungen dazu finden sich in den Issues im tronferno-mcu-bin repository auf GitHub.

Lösung ist der Download der neuesten Firmware-Version und das flashen über USB. Hinterher funktioniert OTA wieder.
FHEM User können auch ein Update über das TronfernoMCU Modul versuchen, dazu muss die MCU dort über ein USB Gerät verbunden sein, nicht über eine IP Adresse.

Hinweis: Wenn das manuelle flashen irgendwie klemmnt, dann bitte solange versuchen bis es klappt. Das Löschen des Flash-Speichers hilft nicht bei diesem Problem.
Es kann am Treiber liegen oder man muss die Boot-Taste drücken oder eventuell einen Kondensator nachrüsten.
Manche Boards erlauben das erstmalige Flashen ohne den Kondensator, aber weitere Versuche funktionieren nur noch selten.  Ein Skript was das flashen in einer 
Schleife solange probiert, bis es klappt, kann als Notlösung auch helfen.
</details>

<details>
<summary>Der ESP32 crasht beim Booten und startet mehrfach neu</summary>

Falls es kein Bug in der installierten Firmware ist, könnten Datenmüll im Flash-Speicher der Grund sein.
Hier ließ sich das beheben durch ein komplettes Löschen und Neuflashen des Chips. Ein Backkup fast aller Daten über die Webapp ist möglich.
Eigentlich sollte das nicht passieren. Ursache war hier das rüberflashen unterschiedlicher Firmwareversionen mit unterschiedlich großen Partitionen.
</details>

<details>
<summary>Kann ich zwei oder mehr Tronferno-MCU im selben Haus verwenden</summary>

Das ist möglich, aber sie werden nicht zusammenarbeiten sondern sind unabhängig voneinander.

Gründe für mehrere TronfernoMCUs:

  * Verwalten von mehr als 7 Gruppen / 49 Empfängern (Limit pro Programmierzentrale-ID)
  * Bewusstes Aufteilen von Gruppen auf getrennte Zentralen
  * Verwendung eines TronfernoMCUs als Programmierzentrale und zusätzlich einen/mehrere als reinen Repeater/Funksignalverstärker.
</details>

<details>
<summary>Wie konfiguriere ich den Repeater/Funksignalverstärker?</summary>

Der Repeater benötigt eine Senderliste ("White-List") von Funksender-ID die er verstärken soll. Steht ein Funksender nicht auf der Senderliste wird er ignoriert.
Soll z.B. ein Sonnensensor mit der ID 201234 repeatet werden, muss diese ID in die Senderliste eingetragen werden.
Wird nun ein Kommando empfangen was von diesem Sonnensensor gesendet wurde, dann wird eine exakte Kopie dieses Kommando unmittelbar darauf
vom am Tronferno angeschlossenen Sender erneut ausgesendet.
Gibt es mehrere Repeater, dann darf ein Sender nicht bei mehreren Repeatern auf der Senderliste stehen, sondern nur bei genau einem.

</details>

<details>
<summary>Kann ein TronfernoMCU gleichzeitig Programmierzentrale und Repeater sein?</summary>

Ja. Dieses bietet sich an, wenn der Standort von Programmierzentrale und Repeater sowieso am gleichen Ort wäre.

</details>

<details>
<summary>Welche minimale Konfiguration benötigt ein Tronferno der ausschließlich als Repeater verwendet werden soll?</summary>

Wenn ein eigenständiger Repeater benötigt wird, dann beschränkt sich die Konfiguration auf:

* Funkempfänger und Funksender GPIOs (Zahnrad->MCU->Allgemein->Elekrische_Ein_und_Ausgänge)
* Senderliste mit Funksender-IDs (Zahnrad->Sender->Repeater->Senderliste)
* Netzwerkzugang um die Senderliste per Weboberfläche bearbeiten zu können.
* Ohne Netzwerk wird die Senderliste über folgende undokumentierte (provisorische) Kommandozeilen-Option gesetzt oder abgefragt:
      * <pre>config rf-repeater=(ID-Liste|?)</pre>
      *  Beispiel für Senderliste mit drei IDs 10abcd, 201234 und 20abcd:
       <pre>config rf-repeater="10abcd20123420abcd";</pre>
* Alle anderen Funktionen können soweit möglich deaktiviert werden oder können auf Defaultwerten verbleiben.
* In späteren Versionen lassen sich eventuell noch weitere zum repeaten unnötige Funktionen abschalten


</details>

<details>
<summary>Welche GPIOs muss ich in der Konfiguration einstellen?</summary>

In der Regel können relativ beliebige GPIOs in der Schaltung angeschlossen und entsprechend in der Konfiguration zugewiesen werden, solange diese vom verwendeten ESP32-Board nicht anderweitig beansprucht werden (z.B. von einem Ethernet Baustein auf dem Board).


</details>

<details>
<summary>Wozu können Handsender und Sonnensensoren in TronfernoMCU angemeldet und gespeichert werden?</summary>

Der Sinn dabei ist, die aktuelle Position eines Rollladen bestmöglich zu verfolgen. Leider gibt es mit Fernotron keine Möglichkeit einfach den Motor zu fragen welche Position er gerade hat. Also muss diese, soweit möglich, aus den Funk- und Timer-Daten ermittelt werden.

</details>

<details>
<summary>Ich kann einen Sonnensensor nicht anmelden und er taucht nicht im Empfänger-Log auf wenn ich eine der Tasten drücke.</summary>

Vielleicht reicht die Sendeleistung nicht aus, da der Sensor aufgrund zu wenig Sonne noch nicht ausreichend aufgeladen wurde. Er kann zum Anmelden mit einer Lampe geladen werde, und/oder näher an den Empfänger gebracht werden.

</details>

<details>
<summary>Wie melde ich Handsender und Sonnensensoren in TronfernoMCU an?</summary>

Antwort: In Weboberfläche den Tab Zahnrad=>Alias öffnen. Den Button "Start Pairing" drücken. Nun auf dem Handsender STOP oder auf dem Sonnensensor: "SO POS INST" drücken. Das Gerät erscheint als ID in der Liste "All". Im darunter befindlichen Häkchen-Feld alle Empfänger auswählen welche dieser Sender bewegt (bei denen er mittels SET angemeldet wurde).

</details>

<details>
<summary>Wie stelle ich die Endpunkte ein?</summary>

* Weboberfläche Zahnrad->Empfänger->Endpunkte.
* Ein Taster muss angeschlossen sein und konfiguriert sein:
   * Taster an einem GPIO oder GPI und GND anschließen.
   * Taster konfigurieren: Zahnrad->MCU->Allgemein->Elektrische-Ein-und-Ausgänge->Setz-Knopf
   * Bis GPIO33 des ESP32 wird der interne PullUp-Widerstand verwendet.
   * Reine GPIs (GPI34 und höher) brauchen einen externen PullUp-Widerstand (z.b. 1...10 kOhm) zwischen GPI und 3.3V.

</details>

<details>
<summary>Die internen Timer bewirken keine Veränderung bei der Positionsanzeige. Warum?</summary>

Die internen Timer müssen über TronfernoMCU programmiert worden sein, nicht die originale 2411. Es gibt keine Möglichkeit an die in der 2411 und den Motoren gespeicherten Timer-Daten zu gelangen.  Ebenso ist es unmöglich Positionsänderung aufgrund des internen Zufalls-Timer anzuzeigen.
</details>

<details>
<summary>Warum macht die Dämmerungs-Funktion den Rollladen zu wenn es noch zu hell oder bereits zu dunkel ist? Wie hilft die Option Astro-Korrektur?</summary>

Die Astro/Dämmerungs-Funktion verwendet eine Tabelle für ein Halbjahr und der Motor spiegelt diese intern für das andere Halbjahr.
 Leider passen diese Halbjahre nicht exakt zusammen. Es wird immer Tage geben mit falschen Zeiten.
  Man kann durch die Korrektur die Halbjahrs-Tabelle modifizieren, dass Zeiten gewählt werden die für beide Halbjahre einigermaßen passen.
  Entweder eine Zeit die zwischen beiden realen Zeiten liegt, oder eine Zeit die in beiden Halbjahren entweder  niemals vor/nach Dämmerung ist.
  Die korrigierte Tabelle muss dann zu dem betreffenden Rollladen übertragen werden um wirksam zu sein.
  Die Vorhersage der Dämmerung in der Weboberfläche benutzt immer die aktuell voreingestellte Korrektur und die daraus erzeugte Tabelle.
  Wenn die im Rollladen gespeicherte Tabelle eine andere ist, dann werden die Vorhersage-Zeiten nicht damit übereinstimmen.
</details>



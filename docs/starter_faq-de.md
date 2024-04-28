## Tronferno FAQ Liste

#### Fehlfunktionen, Bugs, Neustarts
<details>
<summary>Wie einen Bug melden</summary>
Wenn der Bug seit mehr als einer Beta-Version vorhanden ist, dann ist er wohl noch unentdeckt.
<ul>
  <li>Bitte Versionsnummer angeben</li>
  <li>Manche Fehler sind offensichtlich, aber ich habe es übersehen. Da reicht dann ein einfacher Hinweis</li>
  <li>Ein Backup der Settings wäre nur hilfreich, wenn der Bug davon getriggert wird. Vielleicht von ungewöhnlich vielen Rollläden oder ungewöhnlich langen Namen, etc.  Oder falls das Backkup-File selbst fehlerhaft ist.</li>
  <li>Wichtig sind die Beobachtungen was genau wann, wo, wie passiert ist. Wenig hilfreich sind Spekulationen/ Vermutungen über das warum. Oder ein "es funktioniert nicht" ohne Details.</li>
  <li>Da die Hardware weitgehend einheitlich ist, tritt der Bug vermutlich nur unter bestimmten Umständen oder sehr sporadisch auf, sonst hätte ich ihn ja bereits selbst entdeckt.</li>
  <li>Bei manchen Crashs erscheint ein Backtrace Log im USB Terminal, was die Fehlersuche extrem erleichtern kann</li>
</ul>
</details>


<details>
<summary>Informationen in dieser Doku sind falsch oder veraltet</summary>
Das aktuell halten erwies sich als schwierig bis unmöglich. Auch aus diesem Grund ist die Doku jetzt haupstsächlich in den Mauszeiger-Tooltips der Webapp zu finden. Überschdriften in Tabs erklären ausführlich die Funktion dieses Tabs.
Der Fokus der Dokumentation hier liegt auf das flashen und die Netzwerkverbindung. Alles weiter führende sind nur noch ergänzende Informationen. Das erkunden der Weboberfläche und lesen der Sprechblasen ist der beste Weg alle Funktionen zu lernen. Manches ist leider noch nicht vollständig dokumentiert oder nur in Englisch.
</details>



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
<summary>Der ESP32 friert ein</summary>
<ul>
   <li>Aktuelle Firmware Version flashen  </li>
   <li> Zurückflashen auf Firmware die noch funktioniert hat  </li>
   <li> USB Kabe austauschen  </li>
   <li> USB Netzteil austauschen  </li>
   <li> Lötstellen am USB Port des ESP32 mit Lupe kontrollieren  </li>
   <li> Im USB Terminal nach Backtrace-Logzeilen schauen und diese, die umgebenden Fehlermeldungen und unbedingt auch die Firmware-Versionsnummer in den Bugreport einfügen  </li>
        <li> Beispiel für Bactrace Zeile: <pre>Backtrace: 0x400814ae:0x3ffbad20 0x4008ae15:0x3ffbad40 0x40092d72:0x3ffbad60 0x400ebc1f:0x3ffbadd0 0x400dea41:0x3ffbae00 0x400e19ad:0x3ffbaee0 0x400e99f9:0x3ffbafb0 0x400d734c:0x3ffbafe0 0x400d7091:0x3ffbb000 0x401981df:0x3ffbb020</pre>  </li>
</ul>
</details>

<details>
<summary>Ich kann einen Sonnensensor nicht anmelden und er taucht nicht im Empfänger-Log auf wenn ich eine der Tasten drücke.</summary>

Vielleicht reicht die Sendeleistung nicht aus, da der Sensor aufgrund zu wenig Sonne noch nicht ausreichend aufgeladen wurde. Er kann zum Anmelden mit einer Lampe geladen werde, und/oder näher an den Empfänger gebracht werden.

</details>

<details>
<summary>Die internen Timer bewirken keine Veränderung bei der Positionsanzeige. Warum?</summary>

Die internen Timer müssen über TronfernoMCU programmiert worden sein, nicht die originale 2411. Es gibt keine Möglichkeit an die in der 2411 und den Motoren gespeicherten Timer-Daten zu gelangen.  Ebenso ist es unmöglich Positionsänderung aufgrund des internen Zufalls-Timer anzuzeigen.
</details>


#### Bedienung, Einstellungen


<details>
<summary>Wie konfiguriere ich den Repeater/Funksignalverstärker?</summary>

Der Repeater benötigt eine Senderliste ("White-List") von Funksender-ID die er verstärken soll. Steht ein Funksender nicht auf der Senderliste wird er ignoriert.
Soll z.B. ein Sonnensensor mit der ID 201234 repeatet werden, muss diese ID in die Senderliste eingetragen werden.
Wird nun ein Kommando empfangen was von diesem Sonnensensor gesendet wurde, dann wird eine exakte Kopie dieses Kommando unmittelbar darauf
vom am Tronferno angeschlossenen Sender erneut ausgesendet.
Gibt es mehrere Repeater, dann darf ein Sender nicht bei mehreren Repeatern auf der Senderliste stehen, sondern nur bei genau einem.

</details>

<details>
<summary>Welche minimale Konfiguration benötigt ein Tronferno der ausschließlich als Repeater verwendet werden soll?</summary>

Wenn ein eigenständiger Repeater benötigt wird, dann beschränkt sich die Konfiguration auf:
<ul>
<li> Funkempfänger und Funksender GPIOs (Zahnrad->MCU->Allgemein->Elekrische_Ein_und_Ausgänge)</li>
<li> Senderliste mit Funksender-IDs (Zahnrad->Sender->Repeater->Senderliste)</li>
<li> Netzwerkzugang um die Senderliste per Weboberfläche bearbeiten zu können.</li>
<li> Ohne Netzwerk wird die Senderliste über folgende undokumentierte (provisorische) Kommandozeilen-Option gesetzt oder abgefragt:</li>
<ul>
      <li> <pre>config rf-repeater=(ID-Liste|?)</pre></li>
      <li>  Beispiel für Senderliste mit drei IDs 10abcd, 201234 und 20abcd:  
       <pre>config rf-repeater="10abcd20123420abcd";</pre></li>
</ul>
<li> Alle anderen Funktionen können soweit möglich deaktiviert werden oder können auf Defaultwerten verbleiben.</li>
<li> In späteren Versionen lassen sich eventuell noch weitere zum repeaten unnötige Funktionen abschalten</li>
</ul>

</details>

<details>
<summary>Welche GPIOs muss ich in der Konfiguration einstellen?</summary>

In der Regel können relativ beliebige GPIOs in der Schaltung angeschlossen und entsprechend in der Konfiguration zugewiesen werden, solange diese vom verwendeten ESP32-Board nicht anderweitig beansprucht werden (z.B. von einem Ethernet Baustein auf dem Board).


</details>


<details>
<summary>Wie melde ich Handsender und Sonnensensoren in TronfernoMCU an?</summary>
Die Sonnensoren kann man unter Zahnrad->Sender->Anmelden   einscannen.  Dort den Anmelden Knopf drücken.  Zahnrad ist sichtbar für Administrator oder höher.

Antwort: In Weboberfläche den Tab Zahnrad->Sender->Anmelden öffnen. Dort den Anmelden Knopf drücken. Nun auf dem Handsender STOP oder auf dem Sonnensensor: "SO POS INST" drücken. Das Gerät erscheint als ID in der Liste "All". Im darunter befindlichen Häkchen-Feld alle Empfänger auswählen welche dieser Sender bewegt (bei denen er mittels SET angemeldet wurde).

</details>

<details>
<summary>Wie stelle ich die Endpunkte ein?</summary>
<ul>
<li> Weboberfläche Zahnrad->Empfänger->Endpunkte.</li>
<li> Ein Taster muss angeschlossen sein und konfiguriert sein:</li>
<ul>
   <li> Taster an einem GPIO oder GPI und GND anschließen.</li>
   <li> Taster konfigurieren: Zahnrad->MCU->Allgemein->Elektrische-Ein-und-Ausgänge->Setz-Knopf</li>
   <li> Bis GPIO33 des ESP32 wird der interne PullUp-Widerstand verwendet.</li>
   <li> Reine GPIs (GPI34 und höher) brauchen einen externen PullUp-Widerstand (z.b. 1...10 kOhm) zwischen GPI und 3.3V.</li>
</ul>
</ul>

</details>


#### Funktionen

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

<details>
<summary>Wozu können Handsender und Sonnensensoren in TronfernoMCU angemeldet und gespeichert werden?</summary>

Der Sinn dabei ist, die aktuelle Position eines Rollladen bestmöglich zu verfolgen. Leider gibt es mit Fernotron keine Möglichkeit einfach den Motor zu fragen welche Position er gerade hat. Also muss diese, soweit möglich, aus den Funk- und Timer-Daten ermittelt werden.

</details>

<details>
<summary>Kann ein TronfernoMCU gleichzeitig Programmierzentrale und Repeater sein?</summary>

Ja. Dieses bietet sich an, wenn der Standort von Programmierzentrale und Repeater sowieso am gleichen Ort wäre.

</details>
<details>
<summary>Kann ich zwei oder mehr Tronferno-MCU im selben Haus verwenden</summary>

Das ist möglich, aber sie werden nicht zusammenarbeiten sondern sind unabhängig voneinander.

Gründe für mehrere TronfernoMCUs:

<ul>
  <li> Verwalten von mehr als 7 Gruppen / 49 Empfängern (Limit pro Programmierzentrale-ID)</li>
  <li> Bewusstes Aufteilen von Gruppen auf getrennte Zentralen</li>
  <li> Verwendung eines TronfernoMCUs als Programmierzentrale und zusätzlich einen/mehrere als reinen Repeater/Funksignalverstärker.</li>
</ul>
</details>
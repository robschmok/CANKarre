MOTORSTEUERGERÄT ---- BENJAMIN ADER & JOBIN HOLZWARTH


Bitte installieren sie das beiliegende Library für den Arduino (AUTO.zip).

Das Library ist für die Fahrsimulation zuständig und bekommt von der Steuereinheit (Arduinoprogramm) Gas-, Brems-, Gang- und Kupplungswerte.

Das Arduino Program empfängt über 4 Adressen die "rohen" Werte des Gas- und Bremspedals (Jeweils ein Byte) sowie die Position des Automatik-Wählhebels. Die Zugehörigen Adressen finden sie in den ersten Zeilen des Arduino-Sketches.

Bei Upload des Sketches wird die Funktion "Zündung" ausgeführt, d.h. das Auto startet.

Jetzt können folgende Befehle an die Steuerung über den CAN gesendet werden:


PORT 200 = GAS INPUT	----> LÄNGE: 1 BYTE	WERTE:	0 - 255 (KEIN GAS - VOLLGAS)

PORT 220 = WAHLHEBEL 	----> LÄNGE: 1 BYTE	WERTE:	0 = PARKEN (P)
							1 = RÜCKWÄRTSGANG (R)
							2 = NEUTRAL (N)
							3 = COMFORT DRIVE (D)
							4 = SPORT DRIVE (S)

PORT 250 = BREMSE INPUT	----> LÄNGE: 1 BYTE	WERTE:	0 - 255 (KEINE - VOLLE BREMSUNG)


Die Werte werden von der Steuerlogik verarbeitet und an die AUTO Klasse weitergegeben.
Die Funktion myAuto.update(100) wird 100 Mal pro Sekunde ausgeführt und berechnet die neuen Fahrdaten des Autos.

Am Ende einer jeden Iteration werden die aktuell simulierten Geschwindigkeits-, Drehzahl-, Gang-, Kupplungs- und Hebelwerte "verpackt" und über den CAN alle 4 Iterationen versendet (5 Bytes auf ein Mal, 25x pro Sekunde = 0,5kbit/s Bandbreite). Dabei gelten folgende Codierungen:

-	DIE GESCHWINDIGKEIT (0 - 320 KM/H) WIRD AUF ZWEI BYTES GEMAPPT, WOBEI DER MAXIMALWERT SICH JEDOCH NICHT AUF 0xffff, SONDERN LEDIGLICH AUF 0xfeff BELÄUFT (AUS VERARBEITUNGSGRÜNDEN DES COCKPITS). --> BYTES  1 & 2

-	DIE DREHZAHL (0 - 8000 UMDREHUNGEN/MINUTE) WIRD AUF GLEICHE WEISE VERARBEITET. --> BYTES 3 & 4

-	DIE ERSTEN 3 BIT DES 5. BYTE GEBEN DEN AKTUELLEN GANG AN (0 = RÜCKWÄRTS), DER 4. BIT DIE KUPPLUNG UND DER 5. BIT DEN MODUS (0 = COMFORT, 1 = SPORT) --> BYTE 5
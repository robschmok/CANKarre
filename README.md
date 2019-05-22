UI für das CAN projekt, motorsteuerung folgt vielleicht wenn Ader auch mal git installiert.

Funtion der UI:
  - Arduino 1 empfängt Gschwindigkeit, Drehzahl, Gang, Kupplung, Sport/Comfort und Blinker über CAN,
    anschließend gibt er diese in 5 byte verpackte Informationen plus ein 0xFF startsignal an das
    animatedUI processing sketch weiter. Davör wird das Blinkersignal von einem konstanten an oder aus
    Wert per timer in oszillierende ("Blinkende") Werte umgewandelt. Ein Problem war, dass die vielen
    Serial.write() befehle den timer verlangsamt haben. Dies wurde erst rausgefunden nachdem das sketch
    zufällig länger offen gelassen wurde und der Blinker nach einer minute endlich anging, obwohl der timer
    auf 1hz eingestellt war.
  - Das Processing sketch wartet auf das 0xFF signal, ließt anschließen fünf byte und übersetzt diese in
    brauchbare datentypen.
  - Mithilfe der Daten werden die Zeiger, Gang, Kupplung, Sport/Comfort und Blinker gezeichnet. Da die
    vielen Striche auf der Skala viel Trigonometrie und damit Rechenleistung brauchen würden wenn man sie 
    jeden Frame neu zeichnet, wird der Hintergrund mit den Skalen vom backgroundGen processing sketch 
    einmalig gezeichnet und anschließend im animatedUI ordner unter background.png gespeichert.
    Das animatedUI sketch nimmt anschließend background.png als hintergrund und zeichnet Zeiger und
    andere Informationen einfach darüber.
  - Um die physische Anzeige der Gruppe Carsten möglich zu machen, muss aufgrung der beschränkten Anzahl
    von IO-Ports der Arduino2 alle Informationen Seriell und nicht über den CAN-Bus bekommen. Daher schickt
    animatedUI bzw. das subprogramm ArduinoInterface anschließend alle Daten an den zweiten Arduino weiter.
    Dies geschieht wie beim ersten Arduino mit einem startbyte von 0xFF um die Daten zuordnen zu können.
  - Alle sketches unter /test wurden für diverse Tests verwendet und können ignoriert werden.

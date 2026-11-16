Inoffizieller Build von GeneralsX 1.0.7 (Zero Hour) mit ein paar Zusatz-Fixes.
Spiellogik unverändert: online und im LAN zusammen mit normalem GeneralsX 1.0.7 spielbar.

**Enthalten**
- Discord-Login bleibt nach dem Beenden gespeichert
- Windows: nutzt 4 GB statt 2 GB Speicher (Abstürze beim Kartenladen in 4K)
- Mac/Linux: Punkt, Komma, Minus, Ziffernblock usw. funktionieren als Tasten
- `TOGGLE_GENERALS_EXPERIENCE`: Generals-Beförderungen per Taste (in CommandMap.ini belegbar)
- Raster-Tastenbelegung und Tastenanzeige auf den Buttons, ab Werk an (TheSuperHackers PR #3215, #3216);
  die Anzeige nennt die Taste der eigenen Tastatur (Z auf deutscher, Y auf englischer)

**Raster-Tasten** sind ab Werk an: die Befehlsleiste liegt auf den zwei Tastenreihen
Q W E R T Z U / A S D F G H J (englische Tastatur: Q W E R T Y U), und jeder Button zeigt seine Taste.
Ausschalten in `Options.ini`
(Windows: Dokumente\Command and Conquer Generals Zero Hour Data, Mac: ~/Library/Application Support/GeneralsX/GeneralsZH):
```
GridHotkeys = No
KeyboardOverlay = No
```

**Installieren**
- Windows: `windows-x86-GeneralsXZH.zip` entpacken und wie das offizielle GeneralsX einrichten.
- Mac: `macos-arm64-GeneralsXZH.zip` entpacken, `GeneralsXZH.app` nach Programme ziehen,
  beim ersten Start Rechtsklick > Öffnen.

Quellcode: Branch `release/1.0.7-bluejanis`. Nicht von den GeneralsX-Entwicklern.

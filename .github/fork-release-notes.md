Inoffizieller Build von GeneralsX 1.0.7 (Zero Hour) mit ein paar Zusatz-Fixes.
Spiellogik unverändert: online und im LAN zusammen mit normalem GeneralsX 1.0.7 spielbar.

**Enthalten**
- Discord-Login bleibt nach dem Beenden gespeichert
- Windows: nutzt 4 GB statt 2 GB Speicher (Abstürze beim Kartenladen in 4K)
- Mac/Linux: Punkt, Komma, Minus, Ziffernblock usw. funktionieren als Tasten
- `TOGGLE_GENERALS_EXPERIENCE`: Generals-Beförderungen per Taste (in CommandMap.ini belegbar)
- Raster-Tastenbelegung und Tastenanzeige auf den Buttons (TheSuperHackers PR #3215, #3216)

**Raster-Tasten einschalten** in `Options.ini`
(Windows: Dokumente\Command and Conquer Generals Zero Hour Data, Mac: ~/Library/Application Support/GeneralsX/GeneralsZH)
```
GridHotkeys = Yes
GridHotkeyLayout = QWERTYUASDFGHJ
GridHotkeyColumns = 7
KeyboardOverlay = Yes
```
Das `Y` ist auf deutschen Tastaturen die Z-Taste (das Spiel liest Tasten nach US-Position).

**Installieren**
- Windows: `windows-x86-GeneralsXZH.zip` entpacken und wie das offizielle GeneralsX einrichten.
- Mac: `macos-arm64-GeneralsXZH.zip` entpacken, `GeneralsXZH.app` nach Programme ziehen,
  beim ersten Start Rechtsklick > Öffnen.

Quellcode: Branch `release/1.0.7-bluejanis`. Nicht von den GeneralsX-Entwicklern.

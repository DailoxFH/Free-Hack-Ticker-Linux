# Free-Hack-Ticker-Linux

## **Features**
##### -Beiträge
##### -ChatBox Nachrichten
##### -Private Nachrichten
##### -Darkmode (xD)
##### -Update-Intervall und Benachrichtungsdauer einstellbar

<br>
Private Nachrichten werden nicht auf dem System gespeichert, sondern lediglich die URL bzw die IDs zu der betroffenen Nachricht. Dies wird gehashed, abgespeichert und bei jedem Check miteinander verglichen. 
Falls man "Speicher meine Sitzung" aktiviert hat wird ein Cookie in den Config Ordner abgelegt. Dieser wird nur sehr simpel obfuscated, sollte aber reichen.
Die Session sollte theoretisch nach 30 Tagen ablaufen.
<br>
Getestet unter KDE, Gnome und Unity.

## *Screenshots*
<img src="https://i.imgur.com/2Oi3sq5.png" width="300" height="250"></img>
<img src="https://i.imgur.com/71WSxm4.png" width="300" height="250"></img>
<br>
<img src="https://i.imgur.com/axpoQvY.png" width="300" height="250"></img>
<img src="https://i.imgur.com/rXAkrCc.png" width="300" height="250"></img>

## *Build from source*
Hierfür wird libcurl bzw openssl benötigt.
```shell
git clone [repo]
cd [repo]
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make

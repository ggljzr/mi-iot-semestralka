# Dálkové ovládání LED

## Arduino
Dálkový ovladač - tlačítka pro ovládání LED.
LED deska - podle příchozích příkazů buď od ovladače nebo od Rpi rozsvěcí/zhasíná LED.

## Raspberry Pi

### rpi_rf24
V c++, pošle zvolenou zprávu na zařízení určené adresou `writing_pipe` a `reading_pipe`. Poskytne
tak rozhraní pro webserver.

### Webserver
Možnost vovládat ledky pomocí webové stránky

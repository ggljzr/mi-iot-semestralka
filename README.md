#MI-IOT semestrálka
#Dálkové ovládání LED

##Arduino
Dálkovej ovladač - tlačítka pro ovládání LED.
LED deska - podle příchozích příkazů buď vod vovladače nebo vod Rpi rozsvěcí/zhasíná LED.

##Raspberry Pi

###rpi_rf24
V c++, pošle zvolenou zprávu na zařízení určené adresou writing_pipe a reading_pipe. Poskytne
tak rozhraní pro webserver.

###Webserver
Možnost vovládat ledky pomocí webový stránky

##ToDo
webserver, stmívání s PWM, timestampy ve zprávách, lepší komunikační protokol, další senzory pro LED desku,
grafy z hodnot vod senzorů ve webovym rozhraní

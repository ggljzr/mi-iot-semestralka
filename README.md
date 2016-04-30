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
grafy z hodnot vod senzorů ve webovym rozhraní, 

udelat prikazy digital read a analog read pro cteni generickejch
senzoru (proste akorat precte pin a vrati hodnotu), 
to vlastne staci akorat napsat do ty dokumentace protoze to stejne nebude nikde implementovany zatim,
akorat s tim bude pocitat ta trida device v tim pythonu

i kdyz aspon na ty led desce by se to udelat mohlo, aby se k ni daly pridavat dalsi senzory a aspon
nak je cist bez zmeny kodu, aspon ten analog read, proste precte analogovej senzor A0-A7 a vrati hodnotu

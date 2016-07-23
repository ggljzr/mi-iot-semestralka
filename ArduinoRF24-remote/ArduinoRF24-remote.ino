#include <SPI.h>
#include "RF24.h"

#define LED_PIN 2
#define BUTTON_PIN 3
#define THIS_DEVICE_NUM 1

#define RPI_READ_PIPE_1 0xA0A0A0A0B2LL
#define RPI_READ_PIPE_2 0xA0A0A0A0C3LL

#define RPI_WRITE_PIPE_1 0xF0F0F0F0B2LL
#define RPI_WRITE_PIPE_2 0xF0F0F0F0C3LL

#define THIS_RPI_READ_PIPE  RPI_READ_PIPE_1
#define THIS_RPI_WRITE_PIPE RPI_WRITE_PIPE_1

#define MESSAGE_LENGTH 10
#define READ_BUFFER_SIZE  MESSAGE_LENGTH
#define WRITE_BUFFER_SIZE MESSAGE_LENGTH

//von si pro ty dalsi kanaly bere jen ten prvni
//byte tzn vsechny vostatni byty musej bejt stejny
//jako byty v pipe 1 coz je THIS_RPI_WRITE_PIPE
#define ARD1_TO_ARD2     0xF0F0F0F0A1LL
#define ARD2_TO_ARD1     0xF0F0F0F0A2LL

#define THIS_ARD_WRITE_PIPE ARD1_TO_ARD2
#define THIS_ARD_READ_PIPE  ARD2_TO_ARD1

RF24 radio(7,8);

const uint8_t toggle_led_cmd[10] = {0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//dalkovej ovladac na vsechny dotazy vodpovida akorat touhle defaultni vodpovedi kde je akorat cislo zarizeni
//vodpovida prikazu not_supported
const uint8_t default_answer[10] = {0xFF, 0x01, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//adresy pouzity v openReadingPipe 1 - 5 by se mely
//lisit jen v prvnim bytu

void setup()
{
    Serial.begin(115200);
    Serial.println("Remote");

    radio.begin();
    
    radio.setPALevel(RF24_PA_LOW);

    //otevirani komunikacnich kanalu
    //tady budeme chtit votevrit jeden
    //read kanal pro RPI a jeden pro druhy arduino
    //a pak zatim jeden write kanal do rpi

    radio.openReadingPipe(1, THIS_RPI_WRITE_PIPE);
    radio.openReadingPipe(2, THIS_ARD_READ_PIPE);
    radio.openWritingPipe(THIS_ARD_WRITE_PIPE);    

    radio.startListening();
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
    led_state = 0;
    digitalWrite(LED_PIN, led_state);

}

void loop()
{
    //ted arduino radia proste jen poslouchaj jestli neprijde neco vod rpi a kdyz jo
    //tak poslou nakou vodezvu + treba rozsvitej 13 pin ledku

    uint8_t read_buffer[READ_BUFFER_SIZE];
    //byte write_buffer[WRITE_BUFFER_SIZE] = THIS_ARD_STR;

    byte button_state = digitalRead(BUTTON_PIN);

    //tady zatim neni zadny cekani na vodezvu nebo neco takovyho
    //proste se musi mackat dokavad se nerozsviti ledka
    if(button_state == HIGH)
    {
        Serial.println("Sending On/Off");
        radio.openWritingPipe(THIS_ARD_WRITE_PIPE);
        
        radio.stopListening();
        bool ok = radio.write(toggle_led_cmd, sizeof(uint8_t) * WRITE_BUFFER_SIZE);
        radio.startListening();    
    
        if(ok)
            Serial.println("OK");
        else
            Serial.println("failed");

        delay(1000);
    }


    //pongback
    if(radio.available())
    {
        while(radio.available())
        {
            radio.read(read_buffer, sizeof(byte) * READ_BUFFER_SIZE);
        }
      
        //votevreme prislusnej kanal do kteryho budem vodpovidat
        if(read_buffer[1] == 0x00)
        {
            radio.openWritingPipe(THIS_RPI_READ_PIPE);
        }
        else if(read_buffer[1] == 0x02)
        {
            radio.openWritingPipe(THIS_ARD_WRITE_PIPE);
        }
 
        //prestanem poslouchat abysme poslali zpravu
        //kdyz prichozi zprava neni 0x00 - OK
        if(read_buffer[0] != 0x00)
        {
            radio.stopListening();
            radio.write(default_answer, sizeof(byte) * WRITE_BUFFER_SIZE);
            radio.startListening();
        }

        Serial.print(F("prislo: "));
        for(int i = 0; i < READ_BUFFER_SIZE; i++)
        {
            Serial.print((int) read_buffer[i]);
            Serial.print(" ");
        }
        Serial.println("");
        
    }

}




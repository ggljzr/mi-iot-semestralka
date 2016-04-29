#include <SPI.h>
#include "RF24.h"

#define LED_PIN 2
#define THIS_DEVICE_NUM 2

#define RPI_READ_PIPE_1 0xA0A0A0A0B2LL
#define RPI_READ_PIPE_2 0xA0A0A0A0C3LL

#define RPI_WRITE_PIPE_1 0xF0F0F0F0B2LL
#define RPI_WRITE_PIPE_2 0xF0F0F0F0C3LL

#define THIS_RPI_READ_PIPE  RPI_READ_PIPE_2
#define THIS_RPI_WRITE_PIPE RPI_WRITE_PIPE_2

#define MESSAGE_LENGTH 10
#define READ_BUFFER_SIZE  MESSAGE_LENGTH
#define WRITE_BUFFER_SIZE MESSAGE_LENGTH

#define ARD1_TO_ARD2 0xF0F0F0F0A1LL
#define ARD2_TO_ARD1 0xF0F0F0F0A2LL

#define THIS_ARD_WRITE_PIPE ARD2_TO_ARD1
#define THIS_ARD_READ_PIPE  ARD1_TO_ARD2

RF24 radio(7,8);

//opdpoved ok
uint8_t default_answer[10] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//adresy pouzity v openReadingPipe 1 - 5 by se mely
//lisit jen v prvnim bytu
bool led_state;
void setup()
{
    Serial.begin(115200);
    Serial.println("LED Board");

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
    led_state = 0;
    digitalWrite(LED_PIN, led_state);

}

void loop()
{
    //ted arduino radia proste jen poslouchaj jestli neprijde neco vod rpi a kdyz jo
    //tak poslou nakou vodezvu + treba rozsvitej 13 pin ledku

    uint8_t read_buffer[READ_BUFFER_SIZE];
    //byte write_buffer[WRITE_BUFFER_SIZE] = THIS_ARD_STR;

    //pongback
    if(radio.available())
    {
        while(radio.available())
        {
            radio.read(read_buffer, sizeof(uint8_t) * READ_BUFFER_SIZE);
        }
      
        //votevreme prislusnej kanal do kteryho budem vodpovidat
        if(read_buffer[1] == 0x00)
        {
            radio.openWritingPipe(THIS_RPI_READ_PIPE);
        }
        else if(read_buffer[1] == 0x01)
        {
            radio.openWritingPipe(THIS_ARD_WRITE_PIPE);
        }

        Serial.print(F("prislo: "));
        for(int i = 0; i < READ_BUFFER_SIZE; i++)
        {
            Serial.print((int) read_buffer[i]);
            Serial.print(" ");
        }
        Serial.println("");
        
        //pokud prisel prikaz pro preputi ledky, prepni ledku
        if(read_buffer[0] = 0x01){
            led_state = !led_state;    
            digitalWrite(LED_PIN, led_state);
            delay(50);
        }
        
        //prestanem poslouchat abysme poslali zpravu
        //zase na zpravy ok neodpovidame
        //i kdyz tady pak asi bude proste nakej 
        //switch az bude vic prikazu
        if(read_buffer[0] != 0x00){
            radio.stopListening();
            default_answer[9] = (uint8_t) led_state;
            radio.write(default_answer, sizeof(uint8_t) * WRITE_BUFFER_SIZE);
            radio.startListening();
        }
        
    }

}




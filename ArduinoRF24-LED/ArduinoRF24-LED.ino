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

#define LED_NUM 4

RF24 radio(7,8);

//opdpoved ok
uint8_t default_answer[10] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t not_supported[10] = {0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t led_pins[LED_NUM] = {2,3,4,5};

//adresy pouzity v openReadingPipe 1 - 5 by se mely
//lisit jen v prvnim bytu
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
    
    for(int i = 0; i < LED_NUM; i++){
        pinMode(led_pins[i], OUTPUT);
        digitalWrite(led_pins[i], LOW);
    }

}

void loop()
{
    //ted arduino radia proste jen poslouchaj jestli neprijde neco vod rpi a kdyz jo
    //tak poslou nakou vodezvu + treba rozsvitej 13 pin ledku

    uint8_t read_buffer[READ_BUFFER_SIZE];
    //byte write_buffer[WRITE_BUFFER_SIZE] = THIS_ARD_STR;
    uint8_t write_buffer[WRITE_BUFFER_SIZE];

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

        for(int i = 0; i < WRITE_BUFFER_SIZE;i++)
            write_buffer[i] = 0x00;
        write_buffer[1] = THIS_DEVICE_NUM;

        uint8_t led_num = 0;
        bool current_led_state = 0;
        bool answer = false;
        switch(read_buffer[0])
        {
            case 0x00:
                answer = true;
                break;
            case 0xFF:
                answer = true;
                break;
            case 0x01:
                led_num = read_buffer[9];
                led_num = led_num % LED_NUM;
                current_led_state = digitalRead(led_pins[led_num]);
                current_led_state = !current_led_state;
                digitalWrite(led_pins[led_num], current_led_state);
                write_buffer[0] = 0x00;
                write_buffer[9] = current_led_state;
                break;
            case 0x02:
                led_num = read_buffer[9];
                led_num = led_num % LED_NUM;
                digitalWrite(led_pins[led_num], HIGH);
                write_buffer[0] = 0x00;
                write_buffer[9] = 0x01;
                break;
            case 0x03:
                led_num = read_buffer[9];
                led_num = led_num % LED_NUM;
                digitalWrite(led_pins[led_num], LOW);
                write_buffer[0] = 0x00;
                write_buffer[9] = 0x01;
                break;
            case 0x04:
                write_buffer[9] = digitalRead(led_pins[3]);
                write_buffer[8] = digitalRead(led_pins[2]);
                write_buffer[7] = digitalRead(led_pins[1]);
                write_buffer[6] = digitalRead(led_pins[0]);
                break;
            default:
                write_buffer[0] = 0xFF;
        }

        //posilani vodpovedi kdyz prichozi zprava nebyla vodpoved
        if(!answer){
            radio.stopListening();
            radio.write(write_buffer, sizeof(uint8_t) * MESSAGE_LENGTH);
            radio.startListening();
        }

    }

}




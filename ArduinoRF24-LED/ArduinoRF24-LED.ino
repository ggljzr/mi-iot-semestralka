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

//tohle by asi uplne nemelo bejt vic nez 4
//protoze pak se to nevejde do ty zpravy
//by se muselo pak delat jako bitovej vektor
#define LED_NUM 4

RF24 radio(7,8);

//opdpoved ok
uint8_t default_answer[10] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t not_supported[10] = {0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t led_pins[LED_NUM] = {3,5,6,9};
uint8_t led_states[LED_NUM] = {0,0,0,0};

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
        led_states[i] = 0;
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
        uint8_t pwm_val = 0;
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
                
                if(led_states[led_num] > 0)
                {
                    led_states[led_num] = 0;
                }
                else
                {
                    led_states[led_num] = 255;
                }
                
                analogWrite(led_pins[led_num], led_states[led_num]);
                write_buffer[0] = 0x00;
                write_buffer[9] = led_states[led_num];
                break;
            case 0x02:
                led_num = read_buffer[9];
                led_num = led_num % LED_NUM;
                digitalWrite(led_pins[led_num], HIGH);
                led_states[led_num] = 255;
                write_buffer[0] = 0x00;
                break;
            case 0x03:
                led_num = read_buffer[9];
                led_num = led_num % LED_NUM;
                digitalWrite(led_pins[led_num], LOW);
                led_states[led_num] = 0;
                write_buffer[0] = 0x00;
                break;
            case 0x04:
                for(int i = 0; i < LED_NUM; i++)
                {
                    write_buffer[(WRITE_BUFFER_SIZE - LED_NUM) + i] = led_states[i];
                }
                break;
            case 0x05:
                led_num = read_buffer[9];
                pwm_val = read_buffer[8];
                led_num = led_num % LED_NUM;
                analogWrite(led_pins[led_num], pwm_val);
                led_states[led_num] = pwm_val;
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




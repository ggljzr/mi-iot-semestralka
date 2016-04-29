#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>
 
#define MESSAGE_SIZE 10
#define MESSAGE_TIMEOUT 600
#define READ_BUFFER_SIZE MESSAGE_SIZE
#define WRITE_BUFFER_SIZE MESSAGE_SIZE

#define ADDRESS_SIZE 5

using namespace std;

//generic setup
RF24 radio(22,0);

const uint8_t toggle_led_cmd[10] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int main(int argc, char * argv[])
{
    cout << "RPI hub" << endl;

    if(argc < 4)
    {
        cout << "usage: ./rpi_rf24 reading_pipe writing_pipe cmd(1 byte) data(4 bytes)" << endl;
    }

    uint64_t reading_pipe = strtoll(argv[1], NULL, 16);
    uint64_t writing_pipe = strtoll(argv[2], NULL, 16);

    uint8_t cmd = strtol(argv[3], NULL, 16);
    uint32_t data = strtol(argv[4], NULL, 16);

    radio.begin();
    radio.setRetries(15,15);
    radio.printDetails();

    radio.openReadingPipe(1,reading_pipe);
    radio.openWritingPipe(writing_pipe);

    //uint8_t write_buffer[WRITE_BUFFER_SIZE] = "0RPI  \n";
    uint8_t read_buffer[READ_BUFFER_SIZE];

    printf("sending to %llx\n", writing_pipe);

    radio.stopListening();
    bool ok = radio.write(toggle_led_cmd, sizeof(uint8_t) * MESSAGE_SIZE);

    if(!ok)
        cout << "failed!" << endl;

    radio.startListening();

    uint64_t started_waiting = millis();
    bool timeout = false;
    while(!radio.available() && !timeout)
    {
        if(millis() - started_waiting > 600)
            timeout = true;
    }

    if(timeout)
        cout << "response timed out" << endl;
    else
    {
        radio.read(read_buffer, sizeof(uint8_t) * READ_BUFFER_SIZE);
        cout << "response" << endl;
        for(int i = 0; i < READ_BUFFER_SIZE; i++)
        {
            printf("%02hhx ", read_buffer[i]);
        }
        cout << endl;
    }

    return 0;
}

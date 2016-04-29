#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>
 
#define MESSAGE_SIZE 10
#define READ_BUFFER_SIZE MESSAGE_SIZE
#define WRITE_BUFFER_SIZE MESSAGE_SIZE

#define ADDRESS_SIZE 5

using namespace std;

//generic setup
RF24 radio(22,0);

const uint64_t writing_pipes[5] = {0xF0F0F0F0A1LL, 0xF0F0F0F0B2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0D4LL, 0xF0F0F0F0E5LL};
const uint64_t reading_pipes[5] = {0xA0A0A0A0A1LL, 0xA0A0A0A0B2LL, 0xA0A0A0A0C3LL, 0xA0A0A0A0D4LL, 0xA0A0A0A0E5LL};

const uint8_t toggle_led_cmd[10] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int main(int argc, char * argv[])
{
    cout << "RPI hub" << endl;

    radio.begin();
    radio.setRetries(15,15);
    radio.printDetails();

    radio.openReadingPipe(1,reading_pipes[1]); //reading pipe for ard1
    radio.openReadingPipe(2,reading_pipes[2]); //reading pipe for ard2

    //uint8_t write_buffer[WRITE_BUFFER_SIZE] = "0RPI  \n";
    uint8_t read_buffer[READ_BUFFER_SIZE];

    radio.startListening();

    while(1)
    {    
        cout << "pick arduino: " << endl;
        int picked_ard = 1;
        cin >> picked_ard;
	if(picked_ard > 2 || picked_ard < 1)
	{
		picked_ard = 2;
	}
        cout << "selected: " << (int) picked_ard << endl;

    	radio.openWritingPipe(writing_pipes[picked_ard]);
    
        radio.stopListening();
        cout << "sending..." << endl;
        bool ok = radio.write(toggle_led_cmd, sizeof(uint8_t) * WRITE_BUFFER_SIZE);

        if(!ok)
            cout << "failed!" << endl;

        radio.startListening();

        unsigned long started_waiting = millis();
        bool timeout = false;
        while(! radio.available() && !timeout)
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
                cout << (int) read_buffer[i] << " ";
            cout << endl;
        }
        sleep(1);

    }

    return 0;
}

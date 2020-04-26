#include <iostream>
#include "Serial.h"

using namespace std;

int main() {
    tstring portName(TEXT("COM1"));
    Serial port(portName, 9600);

    char messages[] = {'m', 'r', 's'};

    cout << "0) Change mode" << endl;
    cout << "1) Turn on or off" << endl;
    cout << "2) Change speed" << endl;

    int userInput;
    while(true) {
        cin >> userInput;
        if (userInput < 0 || userInput > 2) {
            cout << "Incorrect value" << endl;
            continue;
        }

        char* msg = (char*) &messages[userInput];

        int bytesWrote = port.write(msg, 1);
        cout << "Bytes wrote: " << bytesWrote << endl;
        port.flush();
        Sleep(1000);
    }

    return 0;
}

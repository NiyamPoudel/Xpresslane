//This code snippet will help you to read data from arduino

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <csignal>
#include <string>
#include <fstream>
#include <set>
#include <algorithm>

using std::cout;
using std::endl;
using namespace std;

/*Portname must contain these backslashes, and remember to
replace the following com port*/
char *port_name = "COM5";

//String for incoming data
char incomingData[32];

//#define ARDUINO_WAIT_TIME 2000
//#define MAX_DATA_LENGTH 255



class SerialPort
{
private:
    HANDLE handler;
    bool connected;
    COMSTAT status;
    DWORD errors;
public:
    SerialPort(char *portName);
    ~SerialPort();

    int readSerialPort(char *buffer, unsigned int buf_size);
    bool writeSerialPort(char *buffer, unsigned int buf_size);
    bool isConnected();
};

SerialPort::SerialPort(char *portName)
{
    this->connected = false;

    this->handler = CreateFileA(static_cast<LPCSTR>(portName),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if (this->handler == INVALID_HANDLE_VALUE){
        if (GetLastError() == ERROR_FILE_NOT_FOUND){
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        }
    else
        {
            printf("ERROR!!!");
        }
    }
    else {
        DCB dcbSerialParameters = {0};

        if (!GetCommState(this->handler, &dcbSerialParameters)) {
            printf("failed to get current serial parameters");
        }
        else {
            dcbSerialParameters.BaudRate = CBR_115200;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &dcbSerialParameters))
            {
                printf("ALERT: could not set Serial port parameters\n");
            }
            else {
                this->connected = true;
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(2000);
            }
        }
    }
}

SerialPort::~SerialPort()
{
    if (this->connected){
        this->connected = false;
        CloseHandle(this->handler);
    }
}

int SerialPort::readSerialPort(char *buffer, unsigned int buf_size)
{
    DWORD bytesRead;
    unsigned int toRead;

    ClearCommError(this->handler, &this->errors, &this->status);

    if (this->status.cbInQue > 0){
        if (this->status.cbInQue > buf_size){
            toRead = buf_size;
        }
        else toRead = this->status.cbInQue;
    }

    if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

    return 0;
}

bool SerialPort::writeSerialPort(char *buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if (!WriteFile(this->handler, (void*) buffer, buf_size, &bytesSend, 0)){
        ClearCommError(this->handler, &this->errors, &this->status);
        return false;
    }
    else return true;
}

bool SerialPort::isConnected()
{
    return this->connected;
}





int main()
{

    SerialPort arduino(port_name);
    if (arduino.isConnected()) cout << "Connection Established" << endl;
    else cout << "ERROR, check port name";
    int x = 0;
    set <int> s;
//writing serial from arduino to dracula
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ofstream datato("dracula.txt");
    cout << "Scanning..." << endl;
    while (arduino.isConnected() && x < 50){
//Check if data has been read or not
        int read_result = arduino.readSerialPort(incomingData, 32);
        //prints out data
        datato << incomingData << ' ' << endl;
        //puts(incomingData);
        //wait a bit
        x++;
        Sleep(100);
    }
    datato.close();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//reading from dracula to set s
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ifstream datafrom("dracula.txt");
    int number;
    while(datafrom >> number){
        s.insert(number);
    }
    datafrom.close();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//saving set s to a file
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    set <int>::iterator it;
    ofstream uniqueto("uniqueID.txt");
    for(it = s.begin(); it != s.end(); it++)
    {
        uniqueto << *it << endl;
        //cout << *it << endl;
    }
    uniqueto.close();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//reading from read tags and price file to compare and output the name of the products and their cost
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ifstream uniquefrom("uniqueID.txt");//contains the unique read tags
    ifstream productfrom("productPrice.txt");//contains the product details with product tags
    ofstream readTo("readProduct.txt");//save the read products into this file
    int IDfromUnique;
    int IDfromProduct;
    string NamefromProduct;
    int PricefromProduct;
    string userName;
    int total = 0;
    int productNo = 1;
    while(productfrom >> IDfromProduct >> NamefromProduct >> PricefromProduct){
        for(it = s.begin(); it != s.end(); it++)
        {
            if (*it == IDfromProduct){
            readTo << NamefromProduct << " " << PricefromProduct << endl;
            cout << productNo << ". " << NamefromProduct << " " << PricefromProduct << endl;
            total = total + PricefromProduct;
            productNo++;
            }
        }
    }
    cout << "Total is ::" << total << endl;
    readTo << "Total " << total << endl;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Getting the associated User ID, their amount and their tagID/updated remaining amount to temp file called UserIDU
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int U_id;
    string U_name;
    int U_amount;
    string U_pass;
    ifstream userTags("userID.txt");
    ofstream updated("userIDU.txt");
    //ofstream finalbill("bill");
    int a = 0;
//function to make input i.e password invisible
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
    string password;

    while(userTags >> U_id >> U_name >> U_amount >> U_pass){
    for(it = s.begin(); it != s.end(); it++)
    {
        if(*it == U_id){
            cout << "User Id: " << U_name << endl;
            cout << "Please enter password: " << endl;
            getline(cin, password);
            if (password == U_pass){
                cout << "Previous amount on card: " << U_amount << endl;
                int remaining = U_amount-total;
                cout << "Current amount on card: " << remaining << endl;
                U_amount = remaining;
            }else {

                cout << "Sorry, Wrong Password" << endl;
            }
        }
     }
    updated << U_id << ' ' << U_name << ' '<< U_amount << ' ' << U_pass << endl;
  }
//finalbill.close();
    updated.close();
    userTags.close();
    readTo.close();
    productfrom.close();
    uniquefrom.close();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//copying updated file(amount) to userIDU
    ifstream finalread("userIDU.txt");
    ofstream copyto("userID.txt");
    int f_id;
    string f_name;
    int f_amount;
    string f_pass;
    while(finalread >> f_id >> f_name >> f_amount >> f_pass){
        copyto << f_id << ' ' << f_name << ' ' << f_amount << ' ' << f_pass << endl;
    }
    copyto.close();
    finalread.close();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}

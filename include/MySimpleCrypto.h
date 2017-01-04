#ifndef MYSIMPLECRYPTO_H
#define MYSIMPLECRYPTO_H

#include <string>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

class MySimpleCrypto
{
    public:
        MySimpleCrypto();
        string Encrypt(string);
        string Decrypt(string);
        virtual ~MySimpleCrypto();
    protected:
    private:
        string key;
        size_t keyLength;
};

#endif // MYSIMPLECRYPTO_H

#include "MySimpleCrypto.h"

#define KEY "lSA7dD23vd0qTV6bt48b7o121phsrZqh"
#define HEX_CHARS "0123456789ABCDEF"


MySimpleCrypto::MySimpleCrypto()
{
    key = string(KEY);
    keyLength = key.size();
}


string MySimpleCrypto::Encrypt(string s)
{
    string ret("");

    char const *hexChars = (char const*)HEX_CHARS;
    unsigned char chr;
    int p;
    for (int c=0; c<(int)s.size(); c++) {
        chr = (unsigned char)s[c];
        p = c % keyLength;
        chr = chr ^ key[p];
        ret += hexChars[chr >> 4];
        ret += hexChars[chr & 15];
    }

    return ret;
}


string MySimpleCrypto::Decrypt(string s)
{
    string ret("");

    string hexChars(HEX_CHARS);
    unsigned char chr;
    int p, d = 0;
    for (int c=0; c<(int)s.size(); c+=2) {
        chr = ( (unsigned char)hexChars.find(s[c] ) << 4 );
        chr = chr ^ ( (unsigned char)hexChars.find(s[c+1]) );
        p = d % keyLength;
        chr = chr ^ key[p];
        ret += (char)chr;
        d++;
    }

    return ret;
}


MySimpleCrypto::~MySimpleCrypto()
{
}

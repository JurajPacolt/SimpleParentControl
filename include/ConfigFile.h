#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <map>
#include "Utils.h"

using namespace std;

class ConfigFile
{
    public:
        ConfigFile(string);
        void Close();
        void Read();
        string GetValue(string);
        virtual ~ConfigFile();
    protected:
    private:
        ifstream file;
        map<string, string> values;
};

#endif // CONFIGFILE_H

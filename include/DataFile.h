#ifndef DATAFILE_H
#define DATAFILE_H

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include "Utils.h"
#include "ConfigFile.h"
#include "MySimpleCrypto.h"

using namespace std;

class DataFile
{
    public:
        DataFile();
        virtual ~DataFile();
        bool IsDataFileExists();
        const char *GetDataFile();
        struct Record {
            string date;
            int howLongForDay;
            long lastVerify;
        };
        bool GetRecord(Record*);
        void SetRecord(Record*);
    protected:
    private:
};

#endif // DATAFILE_H

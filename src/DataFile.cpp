#include "DataFile.h"


DataFile::DataFile()
{
}


void DataFile::SetRecord(DataFile::Record *record)
{
    ofstream f;
    MySimpleCrypto msc;

    try
    {
        f.open(GetDataFile().c_str());

        ostringstream ss;
        ss << record->date;
        ss << " ";
        ss << record->howLongForDay;
        ss << " ";
        ss << record->lastVerify;

        f << msc.Encrypt(ss.str());

        f.close();
    }
    catch (exception &ex)
    {
        cerr << ex.what() << endl;
        exit(1);
    }
}


bool DataFile::GetRecord(DataFile::Record* rec)
{
    ifstream f;

    f.open(GetDataFile().c_str());

    if (!f.fail() && f.is_open())
    {
        string s("");
        MySimpleCrypto msc;

        getline(f, s);
        s = msc.Decrypt(s);

        try
        {
            char *cDate = new char[11];
            memset(cDate, 0, sizeof(char)*11);
            int iHowLongPerDay;
            long lLastVerify;
            sscanf(s.c_str(), "%s %d %ld", cDate, &iHowLongPerDay, &lLastVerify);

            rec->date = Utils::Trim(string(cDate));
            rec->howLongForDay = iHowLongPerDay;
            rec->lastVerify = lLastVerify;
        }
        catch (exception &ex)
        {
            f.close();
            cerr << "Unknown format. " << ex.what() << endl;
            exit(1);
        }

        f.close();

        return true;
    }

    return false;
}


bool DataFile::IsDataFileExists()
{
    ifstream f;
    f.open(GetDataFile().c_str());
    bool ret = f.good();
    f.close();
    return ret;
}


const string DataFile::GetDataFile()
{
    #if defined(_WIN32)
    TCHAR szPath[MAX_PATH + 1];
    memset(szPath, 0, sizeof(TCHAR)*(MAX_PATH + 1));
    if ( !SUCCEEDED( SHGetFolderPath( NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath ) ) ) {
        cerr << "Could not obtain common application data folder." << endl;
        exit(1);
    }
    string homeDir(szPath);
    #elif defined(linux) or defined(__CYGWIN__)
    string homeDir(getenv("HOME"));
    #endif
    string ret = homeDir + PATH_SEPARATOR + ".parent_control_data";

    return ret;
}


DataFile::~DataFile()
{
}

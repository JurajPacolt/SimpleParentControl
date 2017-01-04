#include "ConfigFile.h"


ConfigFile::ConfigFile(string fileName)
{
    file.open(fileName.c_str());
    if (!file.is_open() || file.fail()) {
        cerr << "Could not open configuration file '" << fileName << "'." << endl;
        exit(1);
    }
}


void ConfigFile::Read()
{
    values.clear();

    string line, key, value;
    int pos;
    while (!file.eof())
    {
        line.clear();

        getline(file, line);

        Utils::Trim(line);
        if (line.size() > 0 && line[0] != '#' && line[0] != '\n' && line[0] != '\r')
        {
            pos = line.find("=", 0);
            if (pos >= 0) {
                key = Utils::Trim(line.substr(0, pos));
                value = Utils::Trim(line.substr(pos+1));
                values[key] = value;
            } else {
                cerr << "Unsupported format of the configuration file." << endl;
                exit(1);
            }
        }
    }
}


string ConfigFile::GetValue(string key)
{
    return values[key];
}


void ConfigFile::Close()
{
    file.close();
}


ConfigFile::~ConfigFile()
{
    Close();
}

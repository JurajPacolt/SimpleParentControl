#include "Utils.h"


// Convert wstring to string.
string Utils::WStrToStr(wstring ws)
{
    string s(ws.begin(), ws.end());
    return s;
}


// Transform name of the parameter.
string Utils::TransformParam(string param)
{
    param = Trim(param);

    if (param.compare("-N")==0 || param.compare("--name")==0) {
        return string("name");
    } if (param.compare("-D")==0 || param.compare("--displayname")==0) {
        return string("displayname");
    } if (param.compare("-I")==0 || param.compare("--install")==0) {
        return string("install");
    } if (param.compare("-U")==0 || param.compare("--uninstall")==0) {
        return string("uninstall");
    } if (param.compare("-P")==0 || param.compare("--print")==0) {
        return string("print");
    } if (param.compare("-C")==0 || param.compare("--config")==0) {
        return string("config");
    }
    return string(param);
}


// Split string with delimiter.
vector<string> Utils::SplitString(string str, string delimiter)
{
    vector<string> ret;

    size_t pos = 0;
    string token;
    while ((pos = str.find(delimiter)) != std::string::npos)
    {
        token = str.substr(0, pos);
        ret.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    ret.push_back(str);

    return ret;
}


// Split string to int array.
map<int,int> Utils::SplitStringToIntMap(string str, string separator)
{
    vector<string> items = SplitString(str, separator);
    map<int,int> ret;
    int c = 0, key, value;
    string s;
    for (vector<string>::iterator i=items.begin(); i!=items.end(); i++) {
        s = Trim(*i);
        if ((c % 2) == 0) {
            key = atoi(s.c_str());
        } else {
            value = atoi(s.c_str());
            ret[key] = value;
        }
        c++;
    }

    return ret;
}

// Trim from left
string Utils::LeftTrim(string s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

// Trim from right
string Utils::RightTrim(string s) {
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// Trim from both ends
string Utils::Trim(string s) {
    return LeftTrim(RightTrim(s));
}

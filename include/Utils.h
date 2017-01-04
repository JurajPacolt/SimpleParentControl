#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>

#if defined(_WIN32)
    #define PATH_SEPARATOR "\\"
#elif defined(linux) or defined(__CYGWIN__)
    #define PATH_SEPARATOR "/"
#endif

using namespace std;

class Utils
{
    public:
        static string WStrToStr(wstring);
        static string TransformParam(string);
        static map<int,int> SplitStringToIntMap(string, string);
        static vector<string> SplitString(string, string);
        static string LeftTrim(string);
        static string RightTrim(string);
        static string Trim(string);
    protected:
    private:
};

#endif // UTILS_H

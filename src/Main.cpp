#ifndef UNICODE
    #define UNICODE
#endif

#if defined(_WIN32)
    #include <windows.h>
    #include <tchar.h>
    #include <direct.h>
    #define GetCurrentDir _getcwd
#elif defined(linux) or defined(__CYGWIN__)
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <stdlib.h>

#include "Utils.h"
#include "Version.h"
#include "ConfigFile.h"
#include "DataFile.h"
#include "MySimpleCrypto.h"

#ifndef MAX_PATH
    #define MAX_PATH 255
#endif

using namespace std;


string myPath;
string executeCommand("");
int checkInterval = 60;
map<int, int> mapConditions;
string configFile("");

enum {
    SUNDAY = 0, MONDAY = 1, TUESDAY = 2, WEDNESDAY = 3,
    THURSDAY = 4, FRIDAY = 5, SATURDAY = 6
};


// Reading configuration file.
void ReadConfigFile()
{
    string fileName;
    if (configFile.empty()) {
        fileName = (myPath + (myPath.size() > 0 ? PATH_SEPARATOR : "") + "SimpleParentControl.cfg");
    } else {
        fileName = configFile;
    }
    ConfigFile cfgFile(fileName);
    cfgFile.Read();
    cfgFile.Close();

    executeCommand = Utils::Trim(cfgFile.GetValue("execute"));

    string strCheckInterval = Utils::Trim(cfgFile.GetValue("checkInterval"));
    checkInterval = atoi(strCheckInterval.c_str());

    string conditions = Utils::Trim(cfgFile.GetValue("conditions"));
    mapConditions = Utils::SplitStringToIntMap(conditions, string(","));
}


// Check interval.
void CheckInterval()
{
    try
    {
        ReadConfigFile();

        time_t t = time(0);
        long lNow = t;
        struct tm *now = localtime(&t);

        int howLongTime = mapConditions[now->tm_wday];

        char date[10];
        sprintf(date, "%02d.%02d.%04d", now->tm_mday, (now->tm_mon + 1), (now->tm_year + 1900) );
        string sNow(date);

        DataFile::Record record;
        DataFile *df = new DataFile();
        bool result;
        if (df->IsDataFileExists())
        {
            result = df->GetRecord(&record);
            if (result)
            {
                if (record.date == sNow)
                {
                    int difference = (int)(lNow - record.lastVerify);
                    record.lastVerify = lNow;
                    df->SetRecord(&record);

                    if ((record.howLongForDay/60) > howLongTime) {
                        system(executeCommand.c_str());
                    } else {
                        record.howLongForDay = record.howLongForDay + difference;
                        df->SetRecord(&record);
                    }
                }
                else
                {
                    record.date = sNow;
                    record.howLongForDay = 0;
                    record.lastVerify = lNow;
                    df->SetRecord(&record);
                }
            }
        }
        else
        {
            record.date = sNow;
            record.howLongForDay = 0;
            record.lastVerify = lNow;
            df->SetRecord(&record);
        }
    }
    catch (exception &ex)
    {
        cerr << ex.what() << endl;
    }
}


#if defined(_WIN32)
LPWSTR defaultServiceName = LPWSTR(L"SimpleParentControl");
LPWSTR defaultDisplayServiceName = LPWSTR(L"Simple Parent Control");
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle = 0;
HANDLE stopServiceEvent = 0;


// Service control handler.
void WINAPI ServiceControlHandler(DWORD controlCode)
{
	switch ( controlCode )
	{
		case SERVICE_CONTROL_INTERROGATE:
			break;

		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
			serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus( serviceStatusHandle, &serviceStatus );

			SetEvent( stopServiceEvent );
			return;

		case SERVICE_CONTROL_PAUSE:
			break;

		case SERVICE_CONTROL_CONTINUE:
			break;

		default:
			if ( controlCode >= 128 && controlCode <= 255 )
				// user defined control code
				break;
			else
				// unrecognised control code
				break;
	}

	SetServiceStatus( serviceStatusHandle, &serviceStatus );
}


// Main service method.
void WINAPI ServiceMain( DWORD /*argc*/, TCHAR* /*argv*/[] )
{
	// initialise service status
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandler(defaultServiceName, ServiceControlHandler);

	if (serviceStatusHandle)
	{
		// service is starting
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		// do initialisation here
		stopServiceEvent = CreateEvent(0, FALSE, FALSE, 0);

		// running
		serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

        do
		{
		    CheckInterval();
		}
		while ( WaitForSingleObject( stopServiceEvent, (checkInterval * 1000) ) == WAIT_TIMEOUT );

		// service was stopped
		serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		// do cleanup here
		CloseHandle( stopServiceEvent );
		stopServiceEvent = 0;

		// service is now stopped
		serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );
	}
}


// Starting up service.
void RunService()
{
	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{ defaultServiceName, ServiceMain },
		{ 0, 0 }
	};

	if (!StartServiceCtrlDispatcher( serviceTable )) {
        cout << "Could not start service. Error No. " << GetLastError() << "." << endl;
        exit(1);
	}

	cout << "Service started up." << endl;
}


// Install service.
void InstallService()
{
    SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);

    if (serviceControlManager)
    {
        TCHAR path[ MAX_PATH + 1 ];
        if ( GetModuleFileName( 0, path, sizeof(path)/sizeof(path[0]) ) > 0 )
        {
            SC_HANDLE service =
                CreateService(
                    serviceControlManager,
                    defaultServiceName, defaultDisplayServiceName,
                    SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                    SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path,
                    0, 0, 0, 0, 0);
            SERVICE_DESCRIPTION description = { LPWSTR(L"Simple parental control.") };
            ChangeServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, &description);
			if (service) {
				CloseServiceHandle(service);
			}

			printf("\nService \"%s\" (%s) was installed.\n", Utils::WStrToStr(defaultDisplayServiceName).c_str(), Utils::WStrToStr(defaultServiceName).c_str());
        }
		else
		{
            cout << "Could not install service. Error No. " << GetLastError() << "." << endl;
            exit(1);
		}

        CloseServiceHandle(serviceControlManager);
    }
    else
    {
        cout << "Could not access to service manager. Error No. " << GetLastError() << "." << endl;
        exit(1);
    }
}


// Uninstall service.
void UninstallService()
{
    SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);

    if (serviceControlManager)
    {
        SC_HANDLE service = OpenService( serviceControlManager,
            defaultServiceName, SERVICE_QUERY_STATUS | DELETE );
        if (service)
        {
            SERVICE_STATUS serviceStatus;
            if (QueryServiceStatus(service, &serviceStatus))
            {
                if ( serviceStatus.dwCurrentState == SERVICE_STOPPED )
                    DeleteService(service);
            }

            CloseServiceHandle(service);

			printf("\nService \"%s\" (%s) was uninstalled.\n", Utils::WStrToStr(defaultDisplayServiceName).c_str(), Utils::WStrToStr(defaultServiceName).c_str());
        }
        else
        {
            cout << "Could not uninstall service. Error No. " << GetLastError() << "." << endl;
            exit(1);
        }

        CloseServiceHandle(serviceControlManager);
    }
    else
    {
        cout << "Could not access to service manager. Error No. " << GetLastError() << "." << endl;
        exit(1);
    }
}
#endif // _WIN32


// Entry point.
int main(int argv, char *args[])
{
    char path[MAX_PATH+1];
    GetCurrentDir(path, MAX_PATH);
    myPath = string(path);
    string p(args[0]);
    int pos = p.find_last_of(PATH_SEPARATOR);
    if (pos > 0) {
        myPath = p.substr(0, pos);
    }

    if (argv == 2 && (strcmp(args[1], "-h")==0 || strcmp(args[1], "--help")==0))
    {
        cout << endl;
        cout << "****************************************" << endl;
        cout << "Simple Parent Control, version " << VERSION << endl;
        cout << "****************************************" << endl;
        cout << "Author: " << AUTHOR << endl;
        cout << "Release: " << RELEASE_DATE << endl;
        cout << "****************************************" << endl;
        cout << endl;

        cout << "Application parameters:" << endl;
        cout << "-----------------------" << endl << endl;
        cout << "-h\t--help" << "\t\t" << "Show this help." << endl;
        #if defined(_WIN32)
        cout << endl;
        cout << "-N\t--name" << "\t\t" << "Windows service name." << endl;
        cout << "-D\t--displayname" << "\t" << "Windows service display name." << endl;
        cout << "-I\t--install" << "\t" << "Install service." << endl;
        cout << "-U\t--uninstall" << "\t" << "Uninstall service." << endl;
        #endif // _WIN32
        cout << "-P\t--print" << "\t\t" << "Print data file to screen." << endl;
        cout << "-C\t--config" << "\t" << "Configuration file." << endl;

        cout << endl;
        exit(0);
    }

    set<string> supportedParams;
    #if defined(_WIN32)
    supportedParams.insert("-N");
    supportedParams.insert("--name");
    supportedParams.insert("-D");
    supportedParams.insert("--displayname");
    supportedParams.insert("-I");
    supportedParams.insert("--install");
    supportedParams.insert("-U");
    supportedParams.insert("--uninstall");
    #endif // _WIN32
    supportedParams.insert("-P");
    supportedParams.insert("--print");
    supportedParams.insert("-C");
    supportedParams.insert("--config");

    vector<string> params;
    string param;
    int c;
    for (c=1; c<argv; c++)
    {
        param = string(args[c]);
        params.push_back(param);
        if (param == "-I" || param == "--install"
            || param == "-U" || param == "--uninstall"
                || param == "-P" || param == "--print")
        {
            params.push_back(string("true"));
        }
    }

    if ((params.size() % 2) != 0) {
        cout << "Wrong number of parameters. Must be pair." << endl;
        exit(1);
    }

    map<string, string> mapParams;
    c = 0;
    string value;
    for (vector<string>::iterator i = params.begin(); i!=params.end(); ++i) {
        value = *i;
        if (c > 0 && (c%2) != 0) {
            if (supportedParams.find(param) == supportedParams.end()) {
                cout << "Unknown parameter name '" << param << "'." << endl;
                exit(1);
            }
            mapParams[Utils::TransformParam(param)] = value;
        }
        param = *i;
        c++;
    }

    #if defined(_WIN32)
    if (mapParams.find("install") != mapParams.end()) {
        if (mapParams.find("name") != mapParams.end()) {
            wstring ws = wstring(mapParams["name"].begin(), mapParams["name"].end());
            defaultServiceName = LPWSTR(ws.c_str());
        }
        if (mapParams.find("displayname") != mapParams.end()) {
            wstring ws = wstring(mapParams["displayname"].begin(), mapParams["displayname"].end());
            defaultDisplayServiceName = LPWSTR(ws.c_str());
        }
        InstallService();
    } else if (mapParams.find("uninstall") != mapParams.end()) {
        if (mapParams.find("name") != mapParams.end()) {
            wstring ws = wstring(mapParams["name"].begin(), mapParams["name"].end());
            defaultServiceName = LPWSTR(ws.c_str());
        }
        UninstallService();
    }
    #endif // _WIN32
    if (mapParams.find("print") != mapParams.end()) {
        DataFile *df = new DataFile();
        cout << endl;
        cout << "File path: " << df->GetDataFile() << endl;
        cout << endl;
        cout << "Content of the data file: " << endl;
        cout << "========================= " << endl;
        DataFile::Record *rec = new DataFile::Record();
        bool result = df->GetRecord(rec);
        if (result) {
            cout << "Date: " << rec->date << endl;
            cout << "How long per day [seconds]: " << rec->howLongForDay << endl;
            cout << "How long per day [minutes]: " << (rec->howLongForDay / 60) << endl;
            cout << "Last verification in seconds: " << rec->lastVerify << endl;
        } else {
            cout << "Could not open data file." << endl;
        }
        cout << endl;
    }
    if (mapParams.find("config") != mapParams.end()) {
        configFile = mapParams["config"];
    }

    // If the condition is met we can run CheckInterval or RunService.
    // It is depends from OS.
    if (argv <= 1 || (argv == 3 && mapParams.find("config") != mapParams.end() && !configFile.empty()))
    {
        #if defined(linux) or defined(__CYGWIN__)
        CheckInterval();
        #elif defined(_WIN32)
        RunService();
        #endif
        return 0;
    }

    return 0;
}

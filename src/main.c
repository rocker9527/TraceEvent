#include "WinInternal.h"
#include "Log.h"
#include "TraceEvent.h"
#include <stdio.h>
#include "wgetopt.h"

int
WINAPI
main(void)
{
    int wargc;
    PWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);

    if (wargc < 2)
    {
        wprintf(L"Provide a valid option and agrument.\n"
                L"Try 'TraceEvent.exe --help' for more information.\n");
        return 0;
    }

    int c = 0;
    NTSTATUS Status;
    GUID ProviderID;
    UNICODE_STRING GuidString;
    RtlZeroMemory(&ProviderID, sizeof ProviderID);

    BOOLEAN Start = FALSE;
    PWSTR LoggerName = NULL;
    PCWSTR OptionString = L"Eeg:hLl:q:S:s:";

    // Option Table
    const struct option OptionTable[] = {
        { L"enumguidinfo",  no_argument,         0,  'E' },
        { L"enumguid",      no_argument,         0,  'e' },
        { L"guid",          required_argument,   0,  'g' },
        { L"help",          no_argument,         0,  'h' },
        { L"list",          no_argument,         0,  'L' },
        { L"log",           required_argument,   0,  'l' },
        { L"query",         required_argument,   0,  'q' },
        { L"start",         required_argument,   0,  'S' },
        { L"stop",          required_argument,   0,  's' },
        { 0,                no_argument,         0,   0  },
    };

    // Option parsing
    while ((c = wgetopt_long(wargc, wargv, OptionString, OptionTable, 0)) != -1)
    {
        switch (c)
        {
        case 0:
            wprintf(L"Try 'TraceEvent.exe --help' for more information.\n");
            Usage();
            break;

        case 'E':
            EnumGuidsInfo();
            break;

        case 'e':
            EnumGuids();
            break;

        case 'g':
            RtlInitUnicodeString(&GuidString, optarg);
            Status = RtlGUIDFromString(&GuidString, &ProviderID);
            if (!NT_SUCCESS(Status))
                wprintf(L"Enter Provider GUID correctly\n");
            break;

        case 'L':
            ListSessions();
            break;

        case 'l':
            ConsumeEvent(optarg);
            break;

        case 'q':
            QuerySession(optarg);
            break;

        case 'S':
            LoggerName = optarg;
            Start = TRUE;
            break;

        case 's':
            StopSession(optarg);
            break;

        case 'h':
            Usage();
            break;

        default:
            wprintf(L"Try 'TraceEvent.exe --help' for more information.\n");
        }
    }

    if (Start)
    {
        if (ProviderID.Data1 && ProviderID.Data4)
            StartSession(LoggerName, &ProviderID);
        else
            wprintf(L"Event Provider GUID is not added, use '--guid' option to add.\n");
    }

    // Cleanup
    RtlFreeUnicodeString(&GuidString);
}

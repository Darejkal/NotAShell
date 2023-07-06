// #include <windows.h>
// #include <tlhelp32.h>
// #include <tchar.h>
// #include <stdio.h>

// //  Forward declarations:
// BOOL GetProcessList( );
// BOOL ListProcessModules( DWORD dwPID );
// BOOL ListProcessThreads( DWORD dwOwnerPID );
// void printError( const TCHAR* msg );

// int wmain( void )
// {
//   GetProcessList( );
//   return 0;
// }

// BOOL GetProcessList( )
// {
//   HANDLE hProcessSnap;
//   HANDLE hProcess;
//   PROCESSENTRY32 pe32;
//   DWORD dwPriorityClass;

//   // Take a snapshot of all processes in the system.
//   hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
//   if( hProcessSnap == INVALID_HANDLE_VALUE )
//   {
//     printError( TEXT("CreateToolhelp32Snapshot (of processes)") );
//     return( FALSE );
//   }

//   // Set the size of the structure before using it.
//   pe32.dwSize = sizeof( PROCESSENTRY32 );

//   // Retrieve information about the first process,
//   // and exit if unsuccessful
//   if( !Process32First( hProcessSnap, &pe32 ) )
//   {
//     printError( TEXT("Process32First") ); // show cause of failure
//     CloseHandle( hProcessSnap );          // clean the snapshot object
//     return( FALSE );
//   }

//   // Now walk the snapshot of processes, and
//   // display information about each process in turn
//   do
//   {
//     wprintf( TEXT("\n\n=====================================================" ));
//     wprintf( TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile );
//     wprintf( TEXT("\n-------------------------------------------------------" ));

//     // Retrieve the priority class.
//     dwPriorityClass = 0;
//     hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
//     if( hProcess == NULL )
//       printError( TEXT("OpenProcess") );
//     else
//     {
//       dwPriorityClass = GetPriorityClass( hProcess );
//       if( !dwPriorityClass )
//         printError( TEXT("GetPriorityClass") );
//       CloseHandle( hProcess );
//     }

//     wprintf( TEXT("\n  Process ID        = 0x%08X"), pe32.th32ProcessID );
//     wprintf( TEXT("\n  Thread count      = %d"),   pe32.cntThreads );
//     wprintf( TEXT("\n  Parent process ID = 0x%08X"), pe32.th32ParentProcessID );
//     wprintf( TEXT("\n  Priority base     = %d"), pe32.pcPriClassBase );
//     if( dwPriorityClass )
//       wprintf( TEXT("\n  Priority class    = %d"), dwPriorityClass );

//     // List the modules and threads associated with this process
//     ListProcessModules( pe32.th32ProcessID );
//     ListProcessThreads( pe32.th32ProcessID );

//   } while( Process32Next( hProcessSnap, &pe32 ) );

//   CloseHandle( hProcessSnap );
//   return( TRUE );
// }


// BOOL ListProcessModules( DWORD dwPID )
// {
//   HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
//   MODULEENTRY32 me32;

//   // Take a snapshot of all modules in the specified process.
//   hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
//   if( hModuleSnap == INVALID_HANDLE_VALUE )
//   {
//     printError( TEXT("CreateToolhelp32Snapshot (of modules)") );
//     return( FALSE );
//   }

//   // Set the size of the structure before using it.
//   me32.dwSize = sizeof( MODULEENTRY32 );

//   // Retrieve information about the first module,
//   // and exit if unsuccessful
//   if( !Module32First( hModuleSnap, &me32 ) )
//   {
//     printError( TEXT("Module32First") );  // show cause of failure
//     CloseHandle( hModuleSnap );           // clean the snapshot object
//     return( FALSE );
//   }

//   // Now walk the module list of the process,
//   // and display information about each module
//   do
//   {
//     wprintf( TEXT("\n\n     MODULE NAME:     %s"),   me32.szModule );
//     wprintf( TEXT("\n     Executable     = %s"),     me32.szExePath );
//     wprintf( TEXT("\n     Process ID     = 0x%08X"),         me32.th32ProcessID );
//     wprintf( TEXT("\n     Ref count (g)  = 0x%04X"),     me32.GlblcntUsage );
//     wprintf( TEXT("\n     Ref count (p)  = 0x%04X"),     me32.ProccntUsage );
//     wprintf( TEXT("\n     Base address   = 0x%08X"), (long long int) me32.modBaseAddr );
//     wprintf( TEXT("\n     Base size      = %d"),             me32.modBaseSize );

//   } while( Module32Next( hModuleSnap, &me32 ) );

//   CloseHandle( hModuleSnap );
//   return( TRUE );
// }

// BOOL ListProcessThreads( DWORD dwOwnerPID ) 
// { 
//   HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
//   THREADENTRY32 te32; 
 
//   // Take a snapshot of all running threads  
//   hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
//   if( hThreadSnap == INVALID_HANDLE_VALUE ) 
//     return( FALSE ); 
 
//   // Fill in the size of the structure before using it. 
//   te32.dwSize = sizeof(THREADENTRY32); 
 
//   // Retrieve information about the first thread,
//   // and exit if unsuccessful
//   if( !Thread32First( hThreadSnap, &te32 ) ) 
//   {
//     printError( TEXT("Thread32First") ); // show cause of failure
//     CloseHandle( hThreadSnap );          // clean the snapshot object
//     return( FALSE );
//   }

//   // Now walk the thread list of the system,
//   // and display information about each thread
//   // associated with the specified process
//   do 
//   { 
//     if( te32.th32OwnerProcessID == dwOwnerPID )
//     {
//       wprintf( TEXT("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID ); 
//       wprintf( TEXT("\n     Base priority  = %d"), te32.tpBasePri ); 
//       wprintf( TEXT("\n     Delta priority = %d"), te32.tpDeltaPri ); 
//       wprintf( TEXT("\n"));
//     }
//   } while( Thread32Next(hThreadSnap, &te32 ) ); 

//   CloseHandle( hThreadSnap );
//   return( TRUE );
// }

// void printError( const TCHAR* msg )
// {
//   DWORD eNum;
//   TCHAR sysMsg[256];
//   TCHAR* p;

//   eNum = GetLastError( );
//   FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//          NULL, eNum,
//          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//          sysMsg, 256, NULL );

//   // Trim the end of the line and terminate it with a null
//   p = sysMsg;
//   while( ( *p > 31 ) || ( *p == 9 ) )
//     ++p;
//   do { *p-- = 0; } while( ( p >= sysMsg ) &&
//                           ( ( *p == '.' ) || ( *p < 33 ) ) );

//   // Display the message
//   wprintf( TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg );
// }

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

void PrintProcessNameAndID(DWORD processID) {
    WCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                      PROCESS_VM_READ,
                                  FALSE, processID);

    // Get the process name.

    if (NULL != hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
                               &cbNeeded)) {
            GetModuleBaseName(hProcess, hMod, szProcessName,
                              sizeof(szProcessName) / sizeof(TCHAR));
        }
    }

    // Print the process name and identifier.

    std::wcout<<szProcessName<<L"  (PID: "<<processID<<")\n";

    // Release the handle to the process.

    CloseHandle(hProcess);
}
void printHelp(){
    std::wcout<<L"list\nINPUT: list\nOUTPUT: List all current processes";
}
int wmain(int argc, wchar_t* argv[]) {
    if (argc > 1) {
        printHelp();
        return 0;
    }

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return 1;
    }

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

    for (i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            PrintProcessNameAndID(aProcesses[i]);
        }
    }

    return 0;
}
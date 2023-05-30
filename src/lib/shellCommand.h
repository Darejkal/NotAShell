#include <string>
#include <vector>
#include <windows.h>
#include "shellSetting.h"
namespace ms{
    bool buildin_cd(const std::vector<std::wstring>& cmdArr, std::wstring& contentOut){
        if(cmdArr.size()<2){
            contentOut.append(L"cd.exe\nINPUT: cd.exe [PATH]\nOUTPUT: Output CHANGE CURRENT WORKING DIRECTORY TO [PATH]");
            return 0;
        }
        std::wstring a(cmdArr[1]);
        for(int i=2;i<cmdArr.size();i++){
            a.append(L" ").append(cmdArr[i]);
        }
        if(ms::pathdir_changeCurrentPath(a)){
            contentOut.append(L"Path changed");
        } else{
            contentOut.append(L"Inserted path specifier is not correct: ").append(a);
        }
        return 1;
    }
    bool scExecuteCommand(const HWND& hwndParentWindow,std::wstring& contentOut,const std::wstring& command,const std::wstring& PathFileDirectory,const std::wstring& PathCurrentDirectory) {
    HANDLE childOutRead, childOutWrite, childInRead, childInWrite;
    std::vector<std::wstring> cmdArr = ms::suSplitCommand(command, L" ");
    std::wstring programName=cmdArr[0];
    if(programName.find(L".exe")== std::string::npos){
        programName.append(L".exe");
    }
    if(programName.compare(L"cd.exe")==0){
        buildin_cd(cmdArr,contentOut);
        contentOut.append(L"\r\n");
        return 1;
    }
    std::wstring temp;
    temp = PathFileDirectory.c_str();
    temp.append(_PathDefaultDirectory).append(programName);
    HANDLE f = CreateFileW(temp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE){
        temp = PathCurrentDirectory.c_str();
        temp.append(programName);
        f = CreateFileW(temp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    for(auto pc: PathConfig){
        if (f != INVALID_HANDLE_VALUE) { break;}
        temp=pc.c_str();
        temp.append(programName);
        CloseHandle(f);
        f = CreateFileW(temp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    if(f==INVALID_HANDLE_VALUE){
        contentOut.append(cmdArr[0]).append(L"(.exe) is not a valid or known command.\r\n");  // NewLine OK
        CloseHandle(f);
        return 0;
    }
    CloseHandle(f);
    SECURITY_ATTRIBUTES saAttr;
    ZeroMemory(&saAttr, sizeof(saAttr));
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    CreatePipe(&childOutRead, &childOutWrite, &saAttr, 0);
    SetHandleInformation(childOutRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(childInWrite, HANDLE_FLAG_INHERIT, 0);
    PROCESS_INFORMATION cmdProcess;
    ZeroMemory(&cmdProcess, sizeof(cmdProcess));
    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(STARTUPINFO);
    si.lpDesktop = L"Winsta0\\default";
    si.hStdOutput = childOutWrite;
    si.hStdError = childOutWrite;
    si.hStdInput = childInRead;
    si.dwFlags |= STARTF_USESTDHANDLES;
    if (!CreateProcessW(temp.c_str(), (LPWSTR)command.c_str(), NULL, NULL, TRUE,
                        CREATE_NEW_CONSOLE, NULL, NULL, &si, &cmdProcess)) {
        MessageBox(hwndParentWindow, "Cannot create child", "Command Error", MB_ICONWARNING | MB_OK);
    }
    CloseHandle(childOutWrite);
    CloseHandle(childInRead);
    int bufferSize = 1024;
    BYTE buffer[bufferSize];
    ZeroMemory(&buffer, bufferSize);
    DWORD dwRead = bufferSize, dwIndex = 0;
    OVERLAPPED overlapped;
    while (!ReadFile(childOutRead, &buffer[dwIndex], bufferSize, &dwRead, NULL)) {
        dwIndex = dwIndex + dwRead;
        SendMessage(hwndParentWindow, WM_SIZE, SIZE_RESTORED, 0);
    }
    std::string a((char*)buffer, dwIndex + dwRead);
    std::wstring b(a.begin(), a.end());
    contentOut.append(b).append(L"\r\n");
    CloseHandle(childOutRead);
    CloseHandle(childInWrite);
    return 1;
}
}
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include <strsafe.h>

#include <sstream>
#include <string>
#include <vector>

#include "shellSetting.h"
namespace ms {
std::vector<PROCESS_INFORMATION> sc_childProcessList;
PROCESS_INFORMATION sc_lastCreatedChild;
bool buildin_cd(const std::vector<std::wstring>& cmdArr, std::wstring& contentOut) {
    if (cmdArr.size() < 2) {
        contentOut.append(L"[Built-in] cd\r\nINPUT: cd [PATH]\r\nOUTPUT: OUTPUT: CHANGE CURRENT WORKING DIRECTORY TO [PATH]");
        return 0;
    }
    std::wstring a(cmdArr[1]);
    for (int i = 2; i < cmdArr.size(); i++) {
        a.append(L" ").append(cmdArr[i]);
    }
    if (ms::pathdir_changeCurrentPath(a)) {
        contentOut.append(L"Path changed");
    } else {
        contentOut.append(L"Inserted path specifier is not correct: ").append(a);
    }
    return 1;
}
bool buildin_exit(const std::vector<std::wstring>& cmdArr, std::wstring& contentOut) {
    if (cmdArr.size() > 1) {
        contentOut.append(L"[Built-in] exit\r\nINPUT: exit [PATH]\r\nOUTPUT: Exit shell");
        return 0;
    }
    PostQuitMessage(0);
    return 1;
}
bool buildin_mode(const std::vector<std::wstring>& cmdArr, std::wstring& contentOut) {
    if (cmdArr.size() == 1) {
        contentOut.append(L"Current shell operating mode: ").append(ModeExecution);
        return 0;
    }
    if (cmdArr.size() == 2) {
        if (cmdArr[1].compare(L"foreground") == 0 || cmdArr[1].compare(L"background") == 0) {
            ModeExecution = cmdArr[1];
            contentOut.append(L"Current shell operating mode: ").append(ModeExecution);
            return 1;
        }
    }
    contentOut.append(L"[Built-in] mode\r\nINPUT: mode [foreground/background]\r\nOUTPUT: Change the shell's operating mode\r\nINPUT: mode\r\nOUTPUT: Print the shell's current operating mode");
    return 0;
}
bool buildin_cls(const std::vector<std::wstring>& cmdArr, std::wstring& contentOut) {
    if (cmdArr.size() == 1) {
        staticContent.clear();
        return 1;
    }
    contentOut.append(L"[Built-in] cls\r\nINPUT: cls\r\nOUTPUT: Clean the shell staticOutput");
    return 0;
}
bool buildin_path(const std::vector<std::wstring>& cmdArr, std::wstring& contentOut) {
    if (cmdArr.size() == 1) {
        contentOut.append(L"path.exe\nINPUT: path.exe\n add [SPACE SEPERATED PARAM(S)]\n del [SPACE SEPERATED PARAM(S)]\n cur\nOUTPUT: Modify user's PATH or Show current PATH config");
        return 0;
    }
    std::wstring action(cmdArr[1]);
    bool (*func)(LPCWSTR, std::wstring) = NULL;
    if (!action.compare(L"cur")) {
        WCHAR temp[ms::settingBuff];
        ms::readConfig(L"PATH_CONFIG", temp);
        std::wstring t(temp);
        contentOut.append(L"(BEGIN OF CONFIG)\n").append(t).append(L"\n(END OF CONFIG)\n");
        return 0;
    }
    // Other options:
    if (cmdArr.size() == 2) {
        contentOut.append(L"path.exe\nINPUT: path.exe\n add [SPACE SEPERATED PARAM(S)]\n del [SPACE SEPERATED PARAM(S)]\n cur\nOUTPUT: Modify user's PATH or Show current PATH config");
        return 0;
    }
    if (!action.compare(L"add")) {
        func = ms::addConfig;
    } else if (!action.compare(L"del")) {
        func = ms::removeConfig;
    } else {
        contentOut.append(L"path.exe\nINPUT: path.exe\n add [SPACE SEPERATED PARAM(S)]\n del [SPACE SEPERATED PARAM(S)]\n cur\nOUTPUT: Modify user's PATH or Show current PATH config");
        return 0;
    }
    for (int i = 2; i < cmdArr.size(); i++) {
        std::wstring temp(cmdArr[i]);
        // std::wcout<<temp<<L"\n";
        (*func)(PATH_CONFIG, temp);
    }
    return 0;
}
bool buildin_dir(const std::vector<std::wstring>& cmdArr, std::wstring& contentOut) {
    std::wstringstream wstream;
    WIN32_FIND_DATAW ffd;
    LARGE_INTEGER filesize;
    WCHAR szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    std::wstring path;
    switch (cmdArr.size()) {
        case 1:
            path = PathCurrentDirectory;
            break;
        case 2:
            path = pathdir_getPathIfNewIsRelative(PathCurrentDirectory, std::wstring(cmdArr[1]));
            if (pathdir_directoryExists(path)) {
                break;
            } else {
                contentOut.append(L"The specified path does not exist!\r\n");
            }
        default:
            contentOut.append(L"[Built-in] dir\r\nINPUT: dir [PATH]\r\nOUTPUT: Print out path's files");
            return 0;
    }

    // Check that the input path plus 3 is not longer than MAX_PATH.
    // Three characters are for the "\*" plus NULL appended below.

    if (path.size() > (MAX_PATH - 3)) {
        wstream << L"Directory path is too long.\r\n";
        return 0;
    }

    wstream << L"\r\nTarget directory is \"" << path << L"\" \r\nInfo:\r\n";

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.

    StringCchCopyW(szDir, MAX_PATH, path.c_str());
    StringCchCatW(szDir, MAX_PATH, L"\\*");

    // Find the first file in the directory.

    hFind = FindFirstFileW(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
        wstream << L"Find First File Error\r\n";
        return dwError;
    }

    // List all the files in the directory with some info about them.

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            wstream << "   " << ffd.cFileName << L"   <DIR>\r\n";
        } else {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            wstream << L"   " << ffd.cFileName << " " << filesize.QuadPart << L" bytes\r\n";
        }
    } while (FindNextFileW(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        wstream << L"Find First File Error\r\n";
    }

    FindClose(hFind);
    contentOut.append(wstream.str());
    return 1;
}
bool buildin_help(const std::vector<std::wstring>& cmdArr, std::wstring& contentOut) {
    if (cmdArr.size() == 1) {
        contentOut.append(L"List of built-in command: cd, exit, mode, cls, help, child, path.\r\nList of plugins available:\r\n");
        //-------------------------------------------------------------------------- Get all plugins
        // code's similar to buildin_dir
        std::wstringstream wstream;
        WIN32_FIND_DATAW ffd;
        WCHAR szDir[MAX_PATH];
        HANDLE hFind = INVALID_HANDLE_VALUE;
        DWORD dwError = 0;
        std::wstring path = PathFileDirectory;
        path.append(_PathDefaultPluginDirectory);
        if (path.size() > (MAX_PATH - 3)) {
            wstream << L"Error: Directory path is too long.\r\n";
            return 0;
        }

        StringCchCopyW(szDir, MAX_PATH, path.c_str());
        StringCchCatW(szDir, MAX_PATH, L"\\*");

        hFind = FindFirstFileW(szDir, &ffd);

        if (INVALID_HANDLE_VALUE == hFind) {
            wstream << L"Error: Find First File Error\r\n";
            return dwError;
        }

        // print all exe file
        do {
            if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 && PathMatchSpecW(ffd.cFileName, L"*.exe")) {
                wstream << L"- " << ffd.cFileName << L"\r\n";
            }
        } while (FindNextFileW(hFind, &ffd) != 0);

        dwError = GetLastError();
        if (dwError != ERROR_NO_MORE_FILES) {
            wstream << L"Error: Find First File Error\r\n";
        }

        FindClose(hFind);
        contentOut.append(wstream.str());
        return 0;
    }
    contentOut.append(L"[Built-in] help\r\nINPUT: help\r\nOUTPUT: Print out list of usable commands");
}
DWORD buildin_child_suspend(HANDLE hThread) {
    return SuspendThread(hThread);
}
DWORD buildin_child_kill(HANDLE hProcess) {
    return TerminateProcess(hProcess, 0);
}
DWORD buildin_child_resume(HANDLE hThread) {
    return ResumeThread(hThread);
}
bool buildin_child(const std::vector<std::wstring>& cmdArr, std::wstring& contentOut) {
    if (ModeExecution.compare(L"foreground") == 0) {
        contentOut.append(L"Warning: Currently using foreground mode.\r\n Any processes created during foreground mode will be ignored by this command\r\n\r\n");
    }
    if (cmdArr.size() == 1) {
        contentOut.append(L"List of active child processes managed by MyShell:\r\n");
        // contentOut.append(std::to_wstring(sc_childProcessList.size()));
        DWORD exitCode;
        for (int i = 0; i < sc_childProcessList.size(); i++) {
            GetExitCodeProcess(sc_childProcessList[i].hProcess, &exitCode);
            if (exitCode == STILL_ACTIVE) {
                contentOut.append(L"-Child Process ").append(std::to_wstring(sc_childProcessList[i].dwProcessId)).append(L"\r\n");
                contentOut.append(L"++Thread ID: ").append(std::to_wstring(sc_childProcessList[i].dwThreadId)).append(L"\r\n");
            } else {
                // this means process terminated, couldn't care less
                contentOut.append(L"-Child Process ").append(std::to_wstring(sc_childProcessList[i].dwProcessId)).append(L" TERMINATED\r\n");
                contentOut.append(L"++Thread ID: ").append(std::to_wstring(sc_childProcessList[i].dwThreadId)).append(L"\r\n");
                sc_childProcessList.erase(sc_childProcessList.begin() + i);
            }
        }
        contentOut.append(L"\r\n");
        return 0;
    } else if (cmdArr.size() == 3) {
        PROCESS_INFORMATION pci;
        for (auto x : sc_childProcessList) {
            if (std::to_wstring(x.dwProcessId).compare(cmdArr[2]) == 0) {
                pci = x;
                break;
            }
        }
        if (cmdArr[1].compare(L"kill") == 0) {
            if (buildin_child_kill(pci.hProcess) == 0) {
                contentOut.append(L"Kill process failed!\r\n");
            } else {
                contentOut.append(L"Kill process successfully!\r\n");
            }
        } else if (cmdArr[1].compare(L"suspend") == 0) {
            if (buildin_child_suspend(pci.hThread) == -1) {
                contentOut.append(L"Suspend process failed!\r\n");
            } else {
                contentOut.append(L"Suspend process successfully!\r\n");
            }
        } else if (cmdArr[1].compare(L"resume") == 0) {
            if (buildin_child_resume(pci.hThread) == -1) {
                contentOut.append(L"Resume process failed!\r\n");
            } else {
                contentOut.append(L"Resume process successfully!\r\n");
            }
        }
        return 0;
    }
    contentOut.append(L"[Built-in] child\r\nINPUT: child\r\nOUTPUT: List out all processes created from MyShell.");
    contentOut.append(L"INPUT: child kill/suspend/resume [Process ID] \r\nOUTPUT: Kill/Suspend/Resume (single-threaded) child process with the id");
}
bool scExecuteCommand(const HWND& hwndParentWindow, std::wstring& contentOut, const std::wstring& command, const std::wstring& PathFileDirectory, const std::wstring& PathCurrentDirectory) {
    std::vector<std::wstring> cmdArr = ms::suSplitCommand(command, L" ");
    std::wstring programName = cmdArr[0];
    if (programName.find(L".") == std::string::npos) {
        programName.append(L".exe");
    }
    if (programName.compare(L"cd.exe") == 0) {
        buildin_cd(cmdArr, contentOut);
        contentOut.append(L"\r\n");
        return 1;
    } else if (programName.compare(L"exit.exe") == 0) {
        buildin_exit(cmdArr, contentOut);
        contentOut.append(L"\r\n");
        return 1;
    } else if (programName.compare(L"dir.exe") == 0) {
        buildin_dir(cmdArr, contentOut);
        contentOut.append(L"\r\n");
        return 1;
    } else if (programName.compare(L"mode.exe") == 0) {
        buildin_mode(cmdArr, contentOut);
        contentOut.append(L"\r\n");
        return 1;
    } else if (programName.compare(L"cls.exe") == 0) {
        buildin_cls(cmdArr, contentOut);
        return 1;
    } else if (programName.compare(L"help.exe") == 0) {
        buildin_help(cmdArr, contentOut);
        return 1;
    } else if (programName.compare(L"child.exe") == 0) {
        buildin_child(cmdArr, contentOut);
        return 1;
    } else if (programName.compare(L"path.exe") == 0) {
        buildin_path(cmdArr, contentOut);
        return 1;
    }
    std::wstring temp;
    temp = PathFileDirectory.c_str();
    temp.append(_PathDefaultPluginDirectory).append(programName);
    HANDLE f = CreateFileW(temp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) {
        temp = PathCurrentDirectory.c_str();
        temp.append(programName);
        f = CreateFileW(temp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    for (auto pc : PathConfig) {
        if (f != INVALID_HANDLE_VALUE) {
            break;
        }
        temp = pc.c_str();
        temp.append(programName);
        CloseHandle(f);
        f = CreateFileW(temp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    if (f == INVALID_HANDLE_VALUE) {
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
    HANDLE childOutRead, childOutWrite, childInRead, childInWrite;
    CreatePipe(&childOutRead, &childOutWrite, &saAttr, 0);
    SetHandleInformation(childOutRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(childInWrite, HANDLE_FLAG_INHERIT, 0);
    PROCESS_INFORMATION cmdProcess;
    ZeroMemory(&cmdProcess, sizeof(cmdProcess));
    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(STARTUPINFO);
    wchar_t siDesktop[] = L"Winsta0\\default";
    si.lpDesktop = siDesktop;
    si.hStdOutput = childOutWrite;
    si.hStdError = childOutWrite;
    si.hStdInput = childInRead;
    si.dwFlags |= STARTF_USESTDHANDLES;
    if (ModeExecution.compare(L"foreground") == 0) {
        if (!CreateProcessW(temp.c_str(), (LPWSTR)command.c_str(), NULL, NULL, TRUE,
                            CREATE_NEW_CONSOLE, NULL, NULL, &si, &cmdProcess)) {
            MessageBox(hwndParentWindow, "Cannot create child", "Command Error", MB_ICONWARNING | MB_OK);
        }
        sc_lastCreatedChild = cmdProcess;
        CloseHandle(childOutWrite);
        CloseHandle(childInRead);
        int bufferSize = 1024;
        BYTE buffer[bufferSize];
        ZeroMemory(&buffer, bufferSize);
        DWORD dwRead = bufferSize, dwIndex = 0;
        OVERLAPPED overlapped;
        DWORD exitCode;
        while (!ReadFile(childOutRead, &buffer[dwIndex], bufferSize, &dwRead, NULL)) {
            GetExitCodeProcess(sc_lastCreatedChild.hProcess, &exitCode);
            if (exitCode != STILL_ACTIVE) {
                break;
            }
            dwIndex = dwIndex + dwRead;
            SendMessage(hwndParentWindow, WM_SIZE, SIZE_RESTORED, 0);
        }
        std::string a((char*)buffer, dwIndex + dwRead);
        std::wstring b(a.begin(), a.end());
        contentOut.append(b);
        contentOut.append(L"\r\n");
        CloseHandle(childOutRead);
        CloseHandle(childInWrite);
        return 1;

    } else if (ModeExecution.compare(L"background") == 0) {
        si.hStdOutput = 0;
        si.hStdError = 0;
        si.hStdInput = 0;
        if (!CreateProcessW(temp.c_str(), (LPWSTR)command.c_str(), NULL, NULL, TRUE,
                            CREATE_NEW_CONSOLE, NULL, NULL, &si, &cmdProcess)) {
            MessageBox(hwndParentWindow, "Cannot create child", "Command Error", MB_ICONWARNING | MB_OK);
        }
        contentOut.append(L"[Background mode] Created process in background.\r\n");
        sc_childProcessList.push_back(cmdProcess);
        sc_lastCreatedChild = cmdProcess;
        return 1;
    }
}
}  // namespace ms
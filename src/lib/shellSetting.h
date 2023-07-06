#include <aclapi.h>
#include <iostream>
#include <windows.h>
#include <sstream>
#include <string>
#include <vector>

#include "shellUtility.h"
#define PATH_CONFIG L"PATH_CONFIG"
namespace ms {
const std::wstring _PathDefaultPluginDirectory = L"src\\plugins\\";
const LPCWSTR _RelativeSettingPathDefault = L".\\src\\settings.ini";
const std::wstring shellName=L"(myShell)";
std::wstring PathCurrentDirectory, PathFileDirectory,currentDirectoryText;
std::vector<std::wstring> PathConfig;
const DWORD settingBuff = 256;
const std::wstring configDelimiter=L",";
std::wstring ModeExecution=L"foreground";
bool readConfig(LPCWSTR key, LPWSTR returnLP) {
    return GetPrivateProfileStringW(L"SETTING", key, NULL, returnLP, settingBuff, _RelativeSettingPathDefault) == TRUE;
}
bool writeConfig(LPCWSTR key, LPCWSTR value) {
    return WritePrivateProfileStringW(L"SETTING", key, value, _RelativeSettingPathDefault) == TRUE;
}
bool initPathConfig() {
    WCHAR a[settingBuff];
    readConfig(L"PATH_CONFIG", a);
    std::wstring b(a);
    if (b.size() == 0) {
        writeConfig(L"PATH_CONFIG", L"");
        return 0;
    }
    PathConfig = ms::suSplitCommand(b, configDelimiter);
    return 1;
}
bool addConfig(LPCWSTR key, std::wstring value) {
    if(value.back()!='\\'){
        value.append(L"\\");
    }
    WCHAR a[settingBuff];
    readConfig(key, a);
    std::wstring b(a);
    b.append(configDelimiter).append(value);
    // PathConfig.push_back(value);
    return writeConfig(key, b.c_str());
}
bool removeConfig(LPCWSTR key, std::wstring value) {
    WCHAR a[settingBuff];
    readConfig(key, a);
    std::wstring b(a);
    std::vector<std::wstring> arr = ms::suSplitCommand(b, configDelimiter);
    std::wstring result = L"";
    for (int i = 0; i < arr.size(); i++) {
        if (value.compare(arr[i])!=0) {
            result.append(arr[i]).append(configDelimiter);
        }
    }
    return writeConfig(key, result.c_str());
}

std::wstring pathdir_getPathIfNewIsRelative(std::wstring OldPath,std::wstring NewPath){
    std::vector<std::wstring> _np = suSplitCommand(NewPath,L"\\");
    std::vector<std::wstring> _op = suSplitCommand(OldPath,L"\\");
    // for(int i=0;i<_np.size();i++){
    //     std::wcout<<_np[i]<<",";
    // }
    // std::wcout<<"\n";
    int i=0;
    if(_np[0].compare(L".")==0||_np[0].compare(L"..")==0){
        for(;i<_np.size();i++){
            std::wcout<<"Loop\n";
            if(_np[i].compare(L".")==0||_np[i].compare(L"")==0){
                continue;
            } else if(_np[i].compare(L"..")==0){
                while(_op.back().size()==0||_op.back()[0]==L' '){
                _op.pop_back();
                }
                _op.pop_back();
            } else{
                _op.push_back(_np[i]);
            }
        }
    } else {
        _op=_np;
    }
    std::wstringstream ss;
    for(auto x:_op){
        if(x.compare(L"")==0) continue;
        ss<<x<<L"\\";
    }
    return ss.str();
}
bool pathdir_directoryExists(std::wstring Path){
    DWORD atr= GetFileAttributesW(Path.c_str());
    return (atr!=INVALID_FILE_ATTRIBUTES)&&(atr&FILE_ATTRIBUTE_DIRECTORY);
}
bool pathdir_changeCurrentPath(std::wstring& Path){
    std::wstring newPath=pathdir_getPathIfNewIsRelative(PathCurrentDirectory,Path);
    std::wcout<<newPath<<"\n";
    if(pathdir_directoryExists(newPath)){
        PathCurrentDirectory=newPath;
        currentDirectoryText=shellName;
        currentDirectoryText.append(PathCurrentDirectory).append(L">");
        return 1;
    }
    return 0;
}
}  // namespace ms
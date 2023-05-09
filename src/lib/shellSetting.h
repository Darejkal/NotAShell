#include <aclapi.h>
#include <stdio.h>
#include <windows.h>

#include <string>
#include <vector>

#include "shellUtility.h"
namespace ms {
const std::wstring PathDefault = L"src\\plugins\\";
const LPCWSTR SettingPathDefault = L".\\src\\settings.ini";
std::vector<std::wstring> PathConfig;
const DWORD settingBuff = 256;
const std::wstring configDelimiter=L",";
bool readConfig(LPCWSTR key, LPWSTR returnLP) {
    return GetPrivateProfileStringW(L"SETTING", key, NULL, returnLP, settingBuff, SettingPathDefault) == TRUE;
}
bool writeConfig(LPCWSTR key, LPCWSTR value) {
    return WritePrivateProfileStringW(L"SETTING", key, value, SettingPathDefault) == TRUE;
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
        if (!value.compare(arr[i])) {
            result.append(arr[i]).append(configDelimiter);
        }
    }
    return writeConfig(key, result.c_str());
}
}  // namespace ms
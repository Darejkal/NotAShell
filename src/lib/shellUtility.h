#include <string>
#include <vector>
namespace ms {
std::vector<std::wstring> _suSplitW(std::wstring s, std::wstring delimiter) {
    size_t pos_start = 0, pos_end, s_len = s.length(), delim_len = delimiter.length();
    std::wstring token;
    std::vector<std::wstring> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::wstring::npos) {
        for (int i = pos_start; i < pos_end; i++) {
            if (s[i] != delimiter[i - pos_start]) {
                goto notADelimiter;
            }
        }
        pos_start = pos_end + delim_len;
        continue;
    notADelimiter:
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}
std::vector<std::wstring> suSplitCommand(std::wstring s, std::wstring delimiter) {
    auto temp=_suSplitW(s,L"\"");
    std::vector<std::wstring> result;
    for(int i=0;i<temp.size();i++){
        if(i%2){
            result.push_back(temp[i]);
        } else {
            auto t = _suSplitW(temp[i],delimiter);
            result.insert(result.end(),t.begin(),t.end());
        }
    }
    return result;
}
}  // namespace ms
#pragma once
#include <string>

// 宣言のみ（末尾はセミコロン）
std::wstring Utf8ToWide(const std::string& str);
std::string UrlDecode(const std::string& str);
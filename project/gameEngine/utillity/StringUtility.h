#pragma once
#include <string>
#include <dxgidebug.h>


// 文字コードユーティリティ
namespace IIEngine::StringUtility
{
	/// <summary>
	/// stringを wstringに変換する
	/// </summary>
	/// <param name="str">変換元のUTF-8文字列</param>
	/// <returns></returns>
	std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// wstringを stringに変換する
	/// </summary>
	/// <param name="str">変換元のUTF-16文字列</param>
	/// <returns></returns>
	std::string ConvertString(const std::wstring& str);

}
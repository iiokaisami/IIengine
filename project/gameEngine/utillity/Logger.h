#pragma once
#include <string>


// ログ出力
namespace IIEngine::Logger
{
	/// <summary>
	/// 指定されたメッセージをログに記録します。
	/// </summary>
	/// <param name="message">記録する文字列メッセージ。</param>
	void Log(const std::string& message);
}
#include "Logger.h"

#include <windows.h>

namespace IIEngine::Logger
{
	void Logger::Log(const std::string& message)
	{
		OutputDebugStringA(message.c_str());
	}
}
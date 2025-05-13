/******************************************************************************
/*!
\file       LoggerManager.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 11, 2024
\brief      Defines the Logger Manager class which handles the logs

 /******************************************************************************/

#include <FrameExtractorPCH.hpp>
#include <Core\LoggerManager.hpp>
#include <spdlog\sinks\stdout_color_sinks.h>
#include <spdlog\sinks\ostream_sink.h>
namespace FrameExtractor
{
	spdlog::logger* LoggerManager::sEngineLogger;
	ConsolePanel* LoggerManager::mCP = nullptr;
	void LoggerManager::Init(spdlog::level::level_enum level)
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		sEngineLogger = spdlog::stdout_color_mt("ENGINE").get();
		sEngineLogger->set_level(level);
	}

	void LoggerManager::Shutdown()
	{
		spdlog::shutdown();
	}

	void LoggerManager::SetEngineLogLevel(spdlog::level::level_enum level)
	{
		sEngineLogger->set_level(level);
	}

}


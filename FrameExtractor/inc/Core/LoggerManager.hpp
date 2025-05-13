/******************************************************************************
/*!
\file       LoggerManager.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 11, 2024
\brief      Declares the Logger Manager class which handles the logs

 /******************************************************************************/

#ifndef LoggerManager_HPP
#define LoggerManager_HPP

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <GUI/ConsolePanel.hpp>

#define ENGINE_LOGLEVEL_INFO spdlog::level::info
#define ENGINE_LOGLEVEL_TRACE spdlog::level::trace
#define ENGINE_LOGLEVEL_WARN spdlog::level::warn
#define ENGINE_LOGLEVEL_ERROR spdlog::level::err
#define ENGINE_LOGLEVEL_CRITICAL spdlog::level::critical


#define FRAMEEX_ENABLE_ASSERTS

#ifdef FRAMEEX_ENABLE_ASSERTS
#include <filesystem>
#define FRAMEEX_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { BOREALIS##type##ERROR(msg, __VA_ARGS__); BOREALIS_DEBUGBREAK(); } }
#define FRAMEEX_INTERNAL_ASSERT_WITH_MSG(type, check, ...) BOREALIS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define FRAMEEX_INTERNAL_ASSERT_NO_MSG(type, check) BOREALIS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", BOREALIS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define FRAMEEX_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define FRAMEEX_INTERNAL_ASSERT_GET_MACRO(...) BOREALIS_EXPAND_MACRO( BOREALIS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, BOREALIS_INTERNAL_ASSERT_WITH_MSG, BOREALIS_INTERNAL_ASSERT_NO_MSG) )

 // Currently accepts at least the condition and one additional parameter (the message) being optional
#define FRAMEEX_ASSERT(...) BOREALIS_EXPAND_MACRO( BOREALIS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define FRAMEEX_CORE_ASSERT(...) 
#else
#define FRAMEEX_ASSERT(...)
#define FRAMEEX_CORE_ASSERT(...)
#endif

namespace FrameExtractor
{
	class ConsolePanel;
	class LoggerManager
	{
	public:
		static void Init(spdlog::level::level_enum = ENGINE_LOGLEVEL_TRACE);
		static void Shutdown();
		inline static spdlog::logger*& GetEngineLogger() { return sEngineLogger; }
		void SetEngineLogLevel(spdlog::level::level_enum level);
		static void SetConsole(ConsolePanel* console) { mCP = console; };
		static ConsolePanel* mCP;

	private:
		static spdlog::logger* sEngineLogger; // The Logger
	};
}

// Define the log macros
#define FRAMEEX_CORE_INFO(...)     ::FrameExtractor::LoggerManager::GetEngineLogger()->info(__VA_ARGS__);  
#define FRAMEEX_CORE_TRACE(...)    ::FrameExtractor::LoggerManager::GetEngineLogger()->trace(__VA_ARGS__); 
#define FRAMEEX_CORE_WARN(...)		::FrameExtractor::LoggerManager::GetEngineLogger()->warn(__VA_ARGS__);  
#define FRAMEEX_CORE_ERROR(...)    ::FrameExtractor::LoggerManager::GetEngineLogger()->error(__VA_ARGS__);
#define FRAMEEX_CORE_CRITICAL(...) ::FrameExtractor::LoggerManager::GetEngineLogger()->critical(__VA_ARGS__); 

#define APP_CORE_INFO(...)     		::FrameExtractor::LoggerManager::mCP->AddLogInfo(__VA_ARGS__);
#define APP_CORE_TRACE(...)     		::FrameExtractor::LoggerManager::mCP->AddLogTrace(__VA_ARGS__);
#define APP_CORE_WARN(...)     		::FrameExtractor::LoggerManager::mCP->AddLogWarn(__VA_ARGS__);
#define APP_CORE_ERROR(...)     		::FrameExtractor::LoggerManager::mCP->AddLogError(__VA_ARGS__);



#endif

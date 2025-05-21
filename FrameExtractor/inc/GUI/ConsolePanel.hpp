/******************************************************************************
/*!
\file       ConsolePanel.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 12, 2024
\brief      Declares the Console Panel class which creates the interface for a
			console panel.

 /******************************************************************************/

#ifndef ConsolePanel_HPP
#define ConsolePanel_HPP
#include <string>
#include <imgui.h>
#include <GUI/IPanel.hpp>
#include <spdlog/spdlog.h>
#include <chrono>

namespace FrameExtractor
{
	class ConsolePanel : public IPanel
	{
	public:
		ConsolePanel();
		~ConsolePanel() override;
		void ClearLog();

		template<typename... Args>
		std::string Log(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{
			std::string formattedMessage = fmt::format(fmt, std::forward<Args>(args)...);
			auto now = std::chrono::system_clock::now();
			std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
			std::tm* tmPtr = std::localtime(&currentTime);
			char timestampBuffer[11];
			std::strftime(timestampBuffer, sizeof(timestampBuffer), "[%H:%M:%S]", tmPtr);
			return std::string(timestampBuffer) + " " + formattedMessage;
		}

		template <typename T>
		std::string Log(const T& value)
		{
			auto now = std::chrono::system_clock::now();
			std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
			std::tm* tmPtr = std::localtime(&currentTime);
			char timestampBuffer[11];
			std::strftime(timestampBuffer, sizeof(timestampBuffer), "[%H:%M:%S]", tmPtr);
			return std::string(timestampBuffer) + " " + fmt::to_string(value);
		}

		template<typename... Args>
		void AddLog(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{
			
			Items.push_back({ Strdup(Log(fmt, std::forward<Args>(args)...).c_str()), 0});
		}

		template <typename T>
		void AddLog(const T& value)
		{
			Items.push_back({ Strdup(Log(value).c_str()), 0});
		}

		template<typename... Args>
		void AddLogInfo(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{
			Items.push_back({ Strdup(Log(fmt, std::forward<Args>(args)...).c_str()), 1 });

		}

		template <typename T>
		void AddLogInfo(const T& value)
		{
			Items.push_back({ Strdup(Log(value).c_str()), 1 });
		}


		template<typename... Args>
		void AddLogTrace(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{
			Items.push_back({ Strdup(Log(fmt, std::forward<Args>(args)...).c_str()), 2 });

		}

		template <typename T>
		void AddLogTrace(const T& value)
		{
			Items.push_back({ Strdup(Log(value).c_str()), 2 });
		}

		template<typename... Args>
		void AddLogWarn(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{
			Items.push_back({ Strdup(Log(fmt, std::forward<Args>(args)...).c_str()), 3 });

		}

		template <typename T>
		void AddLogWarn(const T& value)
		{
			Items.push_back({ Strdup(Log(value).c_str()), 3 });
		}

		template<typename... Args>
		void AddLogError(spdlog::format_string_t<Args...> fmt, Args &&...args)
		{
			Items.push_back({ Strdup(Log(fmt, std::forward<Args>(args)...).c_str()), 4 });
		}

		template <typename T>
		void AddLogError(const T& value)
		{
			Items.push_back({ Strdup(Log(value).c_str()), 4 });
		}


		void ExecCommand(const char* command_line);

        // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
        static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);

        int TextEditCallback(ImGuiInputTextCallbackData* data);

		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override { return "Console"; }
	private:
		static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
		static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
		static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
		static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

		char                  InputBuf[256];
		ImVector<std::pair<char*, int>>       Items;
		ImVector<const char*> Commands;
		ImVector<char*>       History;
		int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
		ImGuiTextFilter       Filter;
		bool                  AutoScroll;
		bool                  ScrollToBottom;
	};

}

#endif

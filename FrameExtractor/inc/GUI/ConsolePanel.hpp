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
namespace FrameExtractor
{
	class ConsolePanel : public IPanel
	{
	public:
		ConsolePanel();
		~ConsolePanel() override;
		void ClearLog();

		void AddLog(const char* fmt, ...) IM_FMTARGS(2);
		void AddLogInfo(const char* fmt, ...) IM_FMTARGS(2);
		void AddLogTrace(const char* fmt, ...) IM_FMTARGS(2);
		void AddLogWarn(const char* fmt, ...) IM_FMTARGS(2);
		void AddLogError(const char* fmt, ...) IM_FMTARGS(2);

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

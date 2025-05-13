/******************************************************************************
/*!
\file       ApplicationManager.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Declares the Application Manager class which handles the application

 /******************************************************************************/

#ifndef ApplicationManager_HPP
#define ApplicationManager_HPP
#include <Core/WindowManager.hpp>
#include <GUI/ImGuiManager.hpp>
namespace FrameExtractor
{

	class ApplicationManager
	{
	public:
		ApplicationManager(const std::string& name);
		~ApplicationManager();
		void Run();
		void Quit();
		void Init(const std::string& name);
		void Free();

		static ApplicationManager* GetInstance()
		{
			if (sInstance == nullptr)
			{
				sInstance = new ApplicationManager("Default");
			}
			return sInstance;
		}

		WindowManager* GetWindowManager()
		{
			return mWindowManager;
		}
	private:
		static ApplicationManager* sInstance;
		WindowManager* mWindowManager = nullptr;
		ImGuiManager* mImGuiManager = nullptr;
		bool mIsRunning = true;
	};
}

#endif

/******************************************************************************
/*!
\file       ApplicationManager.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Defines the Application Manager class which handles the application

 /******************************************************************************/


#include <FrameExtractorPCH.hpp>
#include <Core/LoggerManager.hpp>
#include <Core/ApplicationManager.hpp>

namespace FrameExtractor
{
	ApplicationManager* ApplicationManager::sInstance = nullptr;
	ApplicationManager::ApplicationManager(const std::string& name)
	{
		sInstance = this;
		Init(name);
	}
	ApplicationManager::~ApplicationManager()
	{
		Free();
	}
	void ApplicationManager::Run()
	{
		ULONGLONG prevTickCount = GetTickCount64();

		while (mIsRunning)
		{
			ULONGLONG currentTickCount = GetTickCount64();
			float deltaTime = (float)(currentTickCount - prevTickCount)/1000.0f;
			prevTickCount = currentTickCount;

			mImGuiManager->Update(deltaTime);
			mImGuiManager->Render();
			mWindowManager->Update();
		}
	}
	void ApplicationManager::Quit()
	{
		mIsRunning = false;
	}
	void ApplicationManager::Init(const std::string& name)
	{
		mIsRunning = true;
		LoggerManager::Init();
		mWindowManager = new WindowManager(WindowProperties(name));
		mImGuiManager = new ImGuiManager();
	}
	void ApplicationManager::Free()
	{
		delete mImGuiManager;
		delete mWindowManager;
		LoggerManager::Shutdown();
	}
}


/******************************************************************************/
/*!
\file		ApplicationManager.cpp
\author 	Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date   	May 10, 2025
\brief		Defines the Application Manager class which handles the application
******************************************************************************/
#include <FrameExtractorPCH.hpp>

 // Project includes
#include <Core/LoggerManager.hpp>
#include <Core/ApplicationManager.hpp>
#include <Core/AssetManager.hpp>
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
		float deltaTime = 0.f;
		while (mIsRunning)
		{
			ULONGLONG currentTickCount = GetTickCount64();

			mImGuiManager->Update(deltaTime);
			mImGuiManager->Render();
			mWindowManager->Update();
			AssetManager::Update();


			const float targetFrameTimeMs = 1000.0f / 60.0f;

			ULONGLONG frameEndTickCount = GetTickCount64();
			float frameDurationMs = (float)(frameEndTickCount - currentTickCount);

			if (frameDurationMs < targetFrameTimeMs)
			{
				DWORD sleepTime = (DWORD)(targetFrameTimeMs - frameDurationMs);
				Sleep(sleepTime); // Sleep to cap the frame rate
			}
			prevTickCount = GetTickCount64();
			deltaTime = (float)(prevTickCount - currentTickCount) / 1000.0f;
			prevTickCount = currentTickCount;
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
		mWindowManager = MakeScope<WindowManager>(WindowProperties(name));
		mImGuiManager =  MakeScope<ImGuiManager>();
		AssetManager::Init();
	}
	void ApplicationManager::Free()
	{
		LoggerManager::Shutdown();
	}
}
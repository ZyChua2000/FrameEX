
/******************************************************************************/
/*!
\file       ApplicationManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 10, 2025
\brief      Declares the Application Manager class which handles the application

 ******************************************************************************/

#ifndef ApplicationManager_HPP
#define ApplicationManager_HPP

 // Project includes
#include <Core/WindowManager.hpp>
#include <GUI/ImGuiManager.hpp>
namespace FrameExtractor
{
	/*!***************************************************************
		\brief
			The Application Manager class is responsible for managing the 
			application lifecycle
	*****************************************************************/
	class ApplicationManager
	{
	public:
		/*!***************************************************************
			\brief
				Constructor for the ApplicationManager class.
			\param[in] name
				The name of the application.
		*****************************************************************/
		ApplicationManager(const std::string& name);

		/*!***************************************************************
			\brief
				Destructor for the ApplicationManager class.
		*****************************************************************/
		~ApplicationManager();

		/*!***************************************************************
			\brief
				The main loop of the application. It runs until Quit() 
				is called.
		*****************************************************************/
		void Run();

		/*!***************************************************************
			\brief
				The Quit function stops the application loop
		*****************************************************************/
		void Quit();

		/*!***************************************************************
			\brief
				Initializes the ApplicationManager with the given name.
			\param[in] name
				The name of the application.
		*****************************************************************/
		void Init(const std::string& name);

		/*!***************************************************************
			\brief
				Frees the resources used by the ApplicationManager.
		*****************************************************************/
		void Free();

		/*!***************************************************************
			\brief
				Gets the instance of the ApplicationManager.
		*****************************************************************/
		static ApplicationManager*& GetInstance()
		{
			return sInstance;
		}

		/*!***************************************************************
			\brief
				Gets the Window Manager instance.
		*****************************************************************/
		inline WindowManager* GetWindowManager() { return mWindowManager.get(); }
	private:
		static ApplicationManager* sInstance;			//<- Singleton instance of ApplicationManager
		Scope<WindowManager> mWindowManager = nullptr;	//<- Window manager instance
		Scope<ImGuiManager> mImGuiManager = nullptr;	//<- ImGui manager instance
		bool mIsRunning = true;							//<- Flag to check if the application is running
	};
}

#endif
/******************************************************************************/
/*!
\file		EntryPoint.hpp
\author 	Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date   	May 10, 2025
\brief		Defines the Entry Point of the program.

 ******************************************************************************/

#include <FrameExtractorPCH.hpp>
#include <Core/PlatformUtils.hpp>
#include <Core/ApplicationManager.hpp>
 // Manual flags
 //#define HIDE_CONSOLE
 //#define SHOW_CONSOLE
#include <crtdbg.h>
int main()
{
#if not(defined(_DEB) || defined(SHOW_CONSOLE))|| defined(HIDE_CONSOLE)
	FrameExtractor::HideTerminal();
#endif
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	FrameExtractor::ApplicationManager* app = new FrameExtractor::ApplicationManager("Frame Extractor");
	app->Run();
	delete app;
}
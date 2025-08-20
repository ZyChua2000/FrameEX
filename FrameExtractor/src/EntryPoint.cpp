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

int main()
{
#if not(defined(_DEB) || defined(SHOW_CONSOLE))|| defined(HIDE_CONSOLE)
	FrameExtractor::HideTerminal();
#endif
	FrameExtractor::ApplicationManager* app = new FrameExtractor::ApplicationManager("Frame Extractor");
	app->Run();
	app->Free();
}
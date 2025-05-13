/******************************************************************************/
/*!
\file		EntryPoint.hpp
\author 	Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date   	May 10, 2024
\brief		Defines the Entry Point of the program.

 /******************************************************************************/

#include <FrameExtractorPCH.hpp>

#include <Core/ApplicationManager.hpp>
int main()
{
	FrameExtractor::ApplicationManager* app = new FrameExtractor::ApplicationManager("Frame Extractor");
	app->Run();
}
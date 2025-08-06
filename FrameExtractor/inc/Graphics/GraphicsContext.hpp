/******************************************************************************
/*!
\file       GraphicsContext.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 10, 2025
\brief      Declares the Graphics Context class which handles the context on which
			the graphics API will run on.

 /******************************************************************************/

#ifndef GraphicsContext_HPP
#define GraphicsContext_HPP

 // Forward Declaration
struct GLFWwindow;

namespace FrameExtractor
{
	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* windowHandle);
		void Init();
		void SwapBuffers();
	private:
		GLFWwindow* mWindowHandle;
	};
}

#endif
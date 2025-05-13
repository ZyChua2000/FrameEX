/******************************************************************************
/*!
\file       GraphicsContext.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
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

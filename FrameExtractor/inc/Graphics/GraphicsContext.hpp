/******************************************************************************/
/*!
\file       GraphicsContext.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 10, 2025
\brief      Declares the Graphics Context class which handles the context on which
			the graphics API will run on.

 ******************************************************************************/

#ifndef GraphicsContext_HPP
#define GraphicsContext_HPP

 // Forward Declaration
struct GLFWwindow;

namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Class that represents the graphics context for OpenGL.
	*************************************************************************/
	class GraphicsContext
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor that initializes the graphics context with a window handle.
			\param[in] windowHandle
				The GLFW window handle to initialize the context with.
		*************************************************************************/
		GraphicsContext(GLFWwindow* windowHandle);

		/*!***********************************************************************
			\brief
				Initialize the graphics context.
		*************************************************************************/
		void Init();

		/*!***********************************************************************
			\brief
				Swaps the buffers of the graphics context.
		*************************************************************************/
		void SwapBuffers();
	private:
		GLFWwindow* mWindowHandle; //<- The GLFW window handle for the graphics context
	};
}

#endif
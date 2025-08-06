/******************************************************************************
/*!
\file       TaskManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       June 19, 2025
\brief      Declares the Template Structure which handles all data templates

 /******************************************************************************/
#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP
#include <Template/DynamicTask.hpp>
namespace FrameExtractor
{
	class TaskManager
	{
	public:
	private:
		std::vector<DynamicTask> mTasks;
	};

	// Count tasks must have Categories and Notes
	// Classification tasks must have categories
	// Annotation task must have labels and boxes and the image it is tagged to
	// Data cleaning task must have folder segregations, outputs etc
}
#endif
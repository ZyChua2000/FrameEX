/******************************************************************************
/*!
\file       BoundingBox.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 13, 2024
\brief      Declares the struct that represents a bounding box

 /******************************************************************************/

#ifndef BoundingBox_HPP
#define BoundingBox_HPP
struct BoundingBox
{
	float minX = 0, minY = 0, maxX = 0, maxY = 0;
};
#endif
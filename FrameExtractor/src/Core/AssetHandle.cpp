/******************************************************************************
/*!
\file       AssetHandle.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2024
\brief      Defines the Asset Handle class

 /******************************************************************************/

#include <FrameExtractorPCH.hpp>
#include <Core/AssetHandle.hpp>
#include <random>
#include <limits>
namespace FrameExtractor
{
	static std::random_device sRandomDevice;
	static std::mt19937_64 sGenerator(sRandomDevice());
	static std::uniform_int_distribution<uint64_t> sDistribution;
	AssetHandle::AssetHandle() : mID(sDistribution(sGenerator)) {}
}
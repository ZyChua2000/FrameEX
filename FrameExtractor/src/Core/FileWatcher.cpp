/******************************************************************************/
/*!
\file       FileWatcher.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       August 4, 2025
\brief      Defines the FileWatcher class that keeps track of changes in a directory

******************************************************************************/

#include <FrameExtractorPCH.hpp>
// Project includes
#include <Core/FileWatcher.hpp>
namespace FrameExtractor
{
	FileWatcher::FileWatcher()
	{}
	FileWatcher::~FileWatcher()
	{}
	void FileWatcher::SetCallback(std::function<void(const std::filesystem::path&, filewatch::Event)> callback)
	{
		mCallback = callback;
	}

	void FileWatcher::Start(const std::filesystem::path& path, const std::regex& pattern,
		std::function<void(const std::filesystem::path&, filewatch::Event)> callback)
	{
		mFileWatch = MakeScope<filewatch::FileWatch>(path, pattern, callback);
		mCallback = callback;
	}

	void FileWatcher::Start(const std::filesystem::path& path)
	{
		if (mCallback)
			mFileWatch = MakeScope<filewatch::FileWatch>(path, mCallback);
	}

	void FileWatcher::Start(const std::filesystem::path& path, const std::regex& pattern)
	{
		if (mCallback)
			mFileWatch = MakeScope<filewatch::FileWatch>(path, pattern, mCallback);
	}
}
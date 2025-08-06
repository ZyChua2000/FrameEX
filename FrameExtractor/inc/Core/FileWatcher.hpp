/******************************************************************************
/*!
\file       FileWatcher.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       August 4, 2025
\brief      Declares the FileWatcher class that keeps track of changes in a directory

/******************************************************************************/

#ifndef FileWatcher_HPP
#define FileWatcher_HPP
#include <regex>
#include <chrono>
#include <unordered_map>
#include <Core/Core.hpp>
#include <FileWatch.hpp>

namespace FrameExtractor
{
	class FileWatcher
	{
	public:
		FileWatcher();
		~FileWatcher();
		void SetCallback(std::function<void(const std::filesystem::path&, filewatch::Event)> callback);
		void Start(const std::filesystem::path& path, const std::regex& pattern,
			std::function<void(const std::filesystem::path&, filewatch::Event)> callback);
		void Start(const std::filesystem::path& path);
		void Start(const std::filesystem::path& path, const std::regex& pattern);
	private:
		Scope<filewatch::FileWatch> mFileWatch = nullptr;
		std::function<void(const std::filesystem::path&, filewatch::Event)> mCallback = nullptr;
		std::unordered_map<std::filesystem::path, std::chrono::steady_clock::time_point> mEventTimestamps;
	};
}

#endif
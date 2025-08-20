/******************************************************************************/
/*!
\file       FileWatcher.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       August 4, 2025
\brief      Declares the FileWatcher class that keeps track of changes in a directory

******************************************************************************/

#ifndef FileWatcher_HPP
#define FileWatcher_HPP
// Standard Library includes
#include <regex>
#include <chrono>
#include <unordered_map>

// Third-party includes
#include <FileWatch.hpp>

// Project includes
#include <Core/Core.hpp>

namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Class that watches a directory for file changes and triggers callbacks.
	*************************************************************************/
	class FileWatcher
	{
	public:
		/*!***********************************************************************
			\brief
				Default constructor for FileWatcher.
		*************************************************************************/
		FileWatcher();

		/*!***********************************************************************
			\brief
				Destructor for FileWatcher.
		*************************************************************************/
		~FileWatcher();

		/*!***********************************************************************
			\brief
				Set a callback function to be called on file changes.
			\param[in] callback
				The callback function to set.
		*************************************************************************/
		void SetCallback(std::function<void(const std::filesystem::path&, filewatch::Event)> callback);

		/*!***********************************************************************
			\brief
				Start watching a directory for file changes.
			\param[in] path
				The path to the directory to watch.
			\param[in] pattern
				Optional regex pattern to filter files.
			\param[in] callback
				Optional callback function to set.
		*************************************************************************/
		void Start(const std::filesystem::path& path, const std::regex& pattern,
			std::function<void(const std::filesystem::path&, filewatch::Event)> callback);


		/*!***********************************************************************
			\brief
				Start watching a directory for file changes.
			\param[in] path
				The path to the directory to watch.
		*************************************************************************/
		void Start(const std::filesystem::path& path);

		/*!***********************************************************************
			\brief
				Start watching a directory for file changes with a default regex pattern.
			\param[in] path
				The path to the directory to watch.
			\details
				This function uses a default regex pattern to filter files.
		*************************************************************************/
		void Start(const std::filesystem::path& path, const std::regex& pattern);
	private:
		Scope<filewatch::FileWatch> mFileWatch = nullptr; //<- File watcher instance
		std::function<void(const std::filesystem::path&, filewatch::Event)> mCallback = nullptr; //<- Callback function to be called on file changes
	};
}

#endif
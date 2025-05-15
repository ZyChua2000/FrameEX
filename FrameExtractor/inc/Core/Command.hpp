/******************************************************************************
/*!
\file       Command.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 13, 2024
\brief      Declares the Command Design Pattern
 
 /******************************************************************************/

#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <memory>
#include <stack>
#include <Graphics/Video.hpp>
namespace FrameExtractor
{

	class ICommand
	{
	public:
		virtual ~ICommand() = default;
		virtual void undo() = 0;
		virtual void execute() = 0;
	};

	class CommandHistory
	{
	public:
		static void execute(std::unique_ptr<ICommand> command);
		static void undo();
		static void redo();
	private:
		static constexpr size_t MAX_HISTORY = 150;
		static void TrimStack(std::stack<std::unique_ptr<ICommand>>& stack);
		static std::stack<std::unique_ptr<ICommand>> undoStack;
		static std::stack<std::unique_ptr<ICommand>> redoStack;
	};

	template <typename PropVal>
	class ModifyPropertyCommand : public ICommand {
	private:
		PropVal* originalData;
		PropVal oldValue, newValue;

	public:
		ModifyPropertyCommand(PropVal* instance, PropVal oldVal, PropVal newVal)
			: originalData(instance), oldValue(oldVal), newValue(newVal) {
		}

		void execute() override {
			*originalData = newValue;
		}

		void undo() override {
			*originalData = oldValue;
		}
	};

	class PlayCommand : public ICommand {
	private:
		int32_t frameNum;
		int32_t* frameNumPtr;
		bool* playBtnPtr;
		Video* mVideo;

	public:
		PlayCommand(bool* Pl, int32_t* fnp, int32_t fn, Video* vid)
			: playBtnPtr(Pl), frameNumPtr(fnp), frameNum(fn), mVideo(vid) {
		}

		void execute() override {
			*playBtnPtr = true;
		}

		void undo() override {
			*playBtnPtr = false;
			*frameNumPtr = frameNum;
			mVideo->Decode(frameNum);
		}
	};

	class SetVideoFrameCommand : public ICommand {
	private:
		int32_t* frameNumPtr;
		int32_t newValue, oldValue;
		Video* mVideo;

	public:
		SetVideoFrameCommand(int32_t* ptr, int32_t old, int32_t New, Video* vid)
			: frameNumPtr(ptr), oldValue(old), newValue(New), mVideo(vid) {
		}

		void execute() override {
			*frameNumPtr = newValue;
			mVideo->Decode(newValue);
		}

		void undo() override {
			*frameNumPtr = oldValue;
			mVideo->Decode(oldValue);
		}
	};

	class CallFunctionCommand : public ICommand {
	private:
		std::function<void()> funcExecute;
		std::function<void()> funcDe_Execute;

	public:
		CallFunctionCommand(std::function<void()> exe, std::function<void()> dexe)
			: funcExecute(exe), funcDe_Execute(dexe) {
		}

		void execute() override {
			funcExecute();
		}

		void undo() override {
			funcDe_Execute();
		}
	};
}


#endif


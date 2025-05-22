/******************************************************************************
/*!
\file       Command.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 13, 2024
\brief      Declares the Command Design Pattern

 /******************************************************************************/
#include <FrameExtractorPCH.hpp>
#include "Core/Command.hpp"
namespace FrameExtractor
{
	std::deque<std::shared_ptr<ICommand>> CommandHistory::undoStack;
	std::deque<std::shared_ptr<ICommand>> CommandHistory::redoStack;
	std::weak_ptr<ICommand> CommandHistory::savedCommand;
	void CommandHistory::execute(std::shared_ptr<ICommand> command)
	{
		command->execute();
		undoStack.push_back(std::move(command));  // push to the back (top of stack)
		redoStack.clear();                        // clears redo history

		TrimStack(undoStack);
	}
	void CommandHistory::undo()
	{
		if (!undoStack.empty()) {
			auto command = std::move(undoStack.back());
			undoStack.pop_back();
			command->undo();
			redoStack.push_back(std::move(command));  // also push to back
		}
	}
	void CommandHistory::redo()
	{
		if (!redoStack.empty()) {
			auto command = std::move(redoStack.back());
			redoStack.pop_back();
			command->execute();
			undoStack.push_back(std::move(command));
		}
	}
	void CommandHistory::TrimStack(std::deque<std::shared_ptr<ICommand>>& stack)
	{
		while (stack.size() > MAX_HISTORY) {
			stack.pop_front();  // Remove the oldest command
		}
	}


	void CommandHistory::markSaved()
	{
		savedCommand = undoStack.empty() ? std::weak_ptr<ICommand>() : undoStack.back();
	}

	bool CommandHistory::isDirty()
	{
		if (undoStack.empty() && savedCommand.expired()) return false;
		if (undoStack.empty() || savedCommand.expired()) return true;

		return undoStack.back() != savedCommand.lock();  // Compare saved and current
	}

	bool CommandHistory::CanRedo()
	{
		return !redoStack.empty();
	}

	bool CommandHistory::CanUndo()
	{
		return !undoStack.empty();
	}

}


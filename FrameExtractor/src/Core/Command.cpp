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
	std::stack<std::unique_ptr<ICommand>> CommandHistory::undoStack;
	std::stack<std::unique_ptr<ICommand>> CommandHistory::redoStack;

	void CommandHistory::execute(std::unique_ptr<ICommand> command)
	{
		command->execute();
		undoStack.push(std::move(command));
		while (!redoStack.empty()) redoStack.pop();
	}
	void CommandHistory::undo()
	{
		if (!undoStack.empty()) {
			auto command = std::move(undoStack.top());
			undoStack.pop();
			command->undo();
			redoStack.push(std::move(command));
		}
	}
	void CommandHistory::redo()
	{
		if (!redoStack.empty()) {
			auto command = std::move(redoStack.top());
			redoStack.pop();
			command->execute();
			undoStack.push(std::move(command));
		}
	}
	void CommandHistory::TrimStack(std::stack<std::unique_ptr<ICommand>>& stack)
	{
		while (stack.size() > MAX_HISTORY) {
			stack.pop();  // Remove the oldest command
		}
	}
}


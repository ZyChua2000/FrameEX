/******************************************************************************/
/*!
\file       Command.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 13, 2025
\brief      Declares the Command Design Pattern

 ******************************************************************************/

#ifndef COMMAND_HPP
#define COMMAND_HPP
 // Standard Library includes
#include <memory>
#include <stack>
#include <type_traits>
#include <utility> // for std::declval
#include <vector>
#include <map>

// Third-party includes
#include <rttr/variant.h>

// Project includes
#include <Graphics/Video.hpp>
#include <Core/Project.hpp>
#include <Core/ExcelSerialiser.hpp>
namespace FrameExtractor
{
	template<typename, typename = std::void_t<>>
	struct has_clear : std::false_type {};

	template<typename T>
	struct has_clear<T, std::void_t<decltype(std::declval<T&>().clear())>> : std::true_type {};

	template<typename T>
	inline constexpr bool has_clear_v = has_clear<T>::value;

	/*!***********************************************************************
		\brief
			Interface for the Command Design Pattern.
			Defines the basic structure for commands that can be executed and undone.
			Commands are used to encapsulate actions that can be performed on objects,
			allowing for undo/redo functionality.
	*************************************************************************/
	class ICommand
	{
	public:
		/*!***********************************************************************
			\brief
				Default destructor for ICommand class.
				Ensures proper cleanup of derived command classes.
		*************************************************************************/
		virtual ~ICommand() = default;

		/*!***********************************************************************
			\brief
				Pure virtual function to undo the command.
				Derived classes must implement this function to define how to revert the command's action.
		*************************************************************************/
		virtual void undo() = 0;

		/*!***********************************************************************
			\brief
				Pure virtual function to execute the command.
				Derived classes must implement this function to define the command's action.
		*************************************************************************/
		virtual void execute() = 0;
	};

	class CommandHistory
	{
	public:

		/*!***********************************************************************
			\brief
				Executes a command and manages the command history.
				When a command is executed, it is added to the undo stack.
			\param[in] command
				The command to be executed.
				If the command is already in the undo stack, it is removed before execution.
				If the command is nullptr, this function does nothing.
		*************************************************************************/
		static void execute(std::shared_ptr<ICommand> command);

		/*!***********************************************************************
			\brief
				Undoes the last executed command.
				The command is moved from the undo stack to the redo stack.
				If there are no commands to undo, this function does nothing.
		*************************************************************************/
		static void undo();

		/*!***********************************************************************
			\brief
				Redoes the last undone command.
				The command is moved from the redo stack back to the undo stack.
				If there are no commands to redo, this function does nothing.
		*************************************************************************/
		static void redo();

		/*!***********************************************************************
			\brief
				Marks the current state as saved.
		*************************************************************************/
		static void markSaved();

		/*!***********************************************************************
			\brief
				Checks if the command history is dirty (i.e., if there are unsaved changes).
			\return
				True if there are commands in the undo stack that have not been saved,
		*************************************************************************/
		static bool isDirty();

		/*!***********************************************************************
			\brief
				Checks if there are commands that can be redone
			\return 
				True if there are commands in the redo stack, false otherwise.
		*************************************************************************/
		static bool CanRedo();

		/*!***********************************************************************
			\brief
				Checks if there are commands that can be undone
			\return 
				True if there are commands in the undo stack, false otherwise.
		*************************************************************************/
		static bool CanUndo();
	private:
		static constexpr size_t MAX_HISTORY = 150;

		/*!***********************************************************************
			\brief
				Trims the command stacks to maintain a maximum size.
				This function ensures that the undo and redo stacks do not exceed the maximum history size.
			\param[in] stack
				The stack to be trimmed.
				If the stack exceeds the maximum size, it will remove the oldest commands until it is within limits.
		*************************************************************************/
		static void TrimStack(std::deque<std::shared_ptr<ICommand>>& stack);
		static std::deque<std::shared_ptr<ICommand>> undoStack;
		static std::deque<std::shared_ptr<ICommand>> redoStack;
		static std::weak_ptr<ICommand> savedCommand;
	};


	/*!***********************************************************************
		\brief
			Command to modify a property of an object.
			This command stores the original value, the old value, and the new value,
			and allows for executing and undoing the modification.
		\param PropVal
			The type of the property value being modified.
	*************************************************************************/
	template <typename PropVal>
	class ModifyPropertyCommand : public ICommand
	{
	private:
		PropVal* originalData;
		PropVal oldValue, newValue;

	public:
		ModifyPropertyCommand(PropVal* instance, PropVal oldVal, PropVal newVal)
			: originalData(instance), oldValue(oldVal), newValue(newVal)
		{}

		void execute() override
		{
			*originalData = newValue;
		}

		void undo() override
		{
			*originalData = oldValue;
		}
	};


	/*!***********************************************************************
		\brief
			Command to delete a dynamic task layer.
			This command stores the interface to the dynamic task, the index of the layer,
			and the data that was deleted, allowing for undo functionality.
	*************************************************************************/
	class DeleteDynamicTaskLayerCommand : public ICommand
	{
	private:
		DynamicTaskInterface mDTInterface;
		int mLayer;
		std::pair<std::vector<rttr::variant>, rttr::variant> mDeletedData;
	public:
		DeleteDynamicTaskLayerCommand(DynamicTaskInterface dtInterface, int idx)
			: mDTInterface(dtInterface), mLayer(idx)
		{}

		void execute() override
		{
			mDeletedData = mDTInterface.DeleteLayer(mLayer);
		}

		void undo() override
		{
			mDTInterface.AddLayer(mDeletedData.first, mDeletedData.second);
		}
	};


	/*!***********************************************************************
		\brief
			Command to delete a dynamic task leaf.
			This command stores the interface to the dynamic task, the index of the leaf,
			and the data that was deleted, allowing for undo functionality.
	*************************************************************************/
	class DeleteDynamicTaskLeafCommand : public ICommand
	{
	private:
		DynamicTaskInterface mDTInterface;
		int mIndex;
		std::pair<std::vector<rttr::variant>, std::vector<TaskData>> mDeletedData;
	public:
		DeleteDynamicTaskLeafCommand(DynamicTaskInterface dtInterface, int idx)
			: mDTInterface(dtInterface), mIndex(idx)
		{}

		DeleteDynamicTaskLeafCommand(DynamicTaskInterface dtInterface) :mDTInterface(dtInterface), mIndex(-1)
		{}
		void execute() override
		{
			mIndex == -1 ? mDeletedData = mDTInterface.DeleteCurrentLeaf() : mDeletedData = mDTInterface.DeleteLeaf(mIndex);
		}

		void undo() override
		{
			mDTInterface.AddLeaf(mDeletedData.first);
			mDTInterface.GetData() = mDeletedData.second;
		}
	};


	/*!***********************************************************************
		\brief
			Command to modify a reflected property of an object.
			This command stores the original data, the old value, and the new value,
			and allows for executing and undoing the modification.
		\param PropVal
			The type of the property value being modified.
	*************************************************************************/
	template <typename PropVal>
	class ModifyReflectedPropertyCommand : public ICommand
	{
	private:
		rttr::variant* originalData;
		PropVal oldValue, newValue;

	public:
		ModifyReflectedPropertyCommand(rttr::variant* instance, PropVal oldVal, PropVal newVal)
			: originalData(instance), oldValue(oldVal), newValue(newVal)
		{}

		void execute() override
		{
			*originalData = newValue;
		}

		void undo() override
		{
			*originalData = oldValue;
		}
	};

	/*!***********************************************************************
		\brief
			Command to modify a pair of properties of two objects.
			This command stores the original data for both properties, the old values, and the new values,
			and allows for executing and undoing the modification.
		\param PropVal
			The type of the property value being modified.
	*************************************************************************/
	template <typename PropVal>
	class ModifyPropertyPairCommand : public ICommand
	{
	private:
		PropVal* originalData1, * originalData2;
		PropVal oldValue1, newValue1, oldValue2, newValue2;
	public:
		ModifyPropertyPairCommand(PropVal* instance1, PropVal oldVal1, PropVal newVal1, PropVal* instance2, PropVal oldVal2, PropVal newVal2)
			: originalData1(instance1), oldValue1(oldVal1), newValue1(newVal1), originalData2(instance2), oldValue2(oldVal2), newValue2(newVal2)
		{}

		void execute() override
		{
			*originalData1 = newValue1;
			*originalData2 = newValue2;
		}

		void undo() override
		{
			*originalData1 = oldValue1;
			*originalData2 = oldValue2;
		}
	};

	/*!***********************************************************************
		\brief
			Command to play a video.
			This command stores the frame number, a pointer to the frame number,
			a pointer to the play button state, and a reference to the video object.
			It allows for executing and undoing the play action.
	*************************************************************************/
	class PlayCommand : public ICommand
	{
	private:
		int32_t frameNum;
		int32_t* frameNumPtr;
		bool* playBtnPtr;
		Ref<Video> mVideo;

	public:
		PlayCommand(bool* Pl, int32_t* fnp, int32_t fn, Ref<Video> vid)
			: playBtnPtr(Pl), frameNumPtr(fnp), frameNum(fn), mVideo(vid)
		{}

		void execute() override
		{
			*playBtnPtr = true;
		}

		void undo() override
		{
			*playBtnPtr = false;
			*frameNumPtr = frameNum;
			mVideo->Decode(frameNum);
		}
	};


	/*!***********************************************************************
		\brief
			Command to set the video frame number.
			This command stores a pointer to the frame number, the new value, the old value,
			and a reference to the video object. It allows for executing and undoing the frame change.
	*************************************************************************/
	class SetVideoFrameCommand : public ICommand
	{
	private:
		int32_t* frameNumPtr;
		int32_t newValue, oldValue;
		Ref<Video> mVideo;

	public:
		SetVideoFrameCommand(int32_t* ptr, int32_t old, int32_t New, Ref<Video> vid)
			: frameNumPtr(ptr), oldValue(old), newValue(New), mVideo(vid)
		{}

		void execute() override
		{
			*frameNumPtr = newValue;
			mVideo->Decode(newValue);
		}

		void undo() override
		{
			*frameNumPtr = oldValue;
			mVideo->Decode(oldValue);
		}
	};

	/*!***********************************************************************
		\brief
			Command to call a function.
			This command stores two functions: one for execution and one for undoing the action.
			It allows for executing the function and undoing it if necessary.
	*************************************************************************/
	class CallFunctionCommand : public ICommand
	{
	private:
		std::function<void()> funcExecute;
		std::function<void()> funcDe_Execute;

	public:
		CallFunctionCommand(std::function<void()> exe, std::function<void()> dexe)
			: funcExecute(exe), funcDe_Execute(dexe)
		{}

		void execute() override
		{
			funcExecute();
		}

		void undo() override
		{
			funcDe_Execute();
		}
	};


	/*!***********************************************************************
		\brief
			Command to erase a key from a map-like container.
			This command stores a pointer to the original data, a duplicate of the data,
			and the distance from the beginning of the container to the key to be erased.
			It allows for executing the erase action and undoing it by restoring the original data.
	*************************************************************************/
	template <typename Map>
	class EraseKeyCommand : public ICommand
	{
	private:
		Map* originalData;
		Map duplicateData;
		int distance;

		// Static assertion to ensure Map is a std::map-like container
		static_assert(std::is_same<typename Map::key_type, typename Map::key_type>::value,
			"Map must be a std::map or similar container with valid key_type");
	public:
		EraseKeyCommand(Map* OD, int dist)
			: originalData(OD), duplicateData(*OD), distance(dist)
		{}

		void execute() override
		{
			auto IT = originalData->begin();
			std::advance(IT, distance);
			originalData->erase(IT);
		}

		void undo() override
		{
			*originalData = duplicateData;
		}
	};


	/*!***********************************************************************
		\brief
			Command to add a key-value pair to a map-like container.
			This command stores a pointer to the original data, the new key, and the value to be added.
			It allows for executing the addition and undoing it by removing the key.
	*************************************************************************/
	template <typename Key, typename Val>
	class AddKeyCommand : public ICommand
	{
	private:
		std::map<Key, Val>* originalData;
		Key newKey;
		Val value;

	public:
		AddKeyCommand(std::map<Key, Val>* OD, Key key, Val val)
			: originalData(OD), newKey(key), value(val)
		{}

		void execute() override
		{
			(*originalData)[newKey] = value;
		}

		void undo() override
		{
			originalData->erase(newKey);
		}
	};

	/*!***********************************************************************
		\brief
			Command to push a value onto a vector.
			This command stores a pointer to the original vector and the value to be added.
			It allows for executing the push_back action and undoing it by popping the last element.
	*************************************************************************/
	template <typename ValType>
	class PushBackCommand : public ICommand
	{
	private:
		std::vector<ValType>* original;
		ValType val;

	public:
		PushBackCommand(std::vector<ValType>* OD, ValType data)
			: original(OD), val(data)
		{}

		void execute() override
		{
			original->push_back(val);
		}

		void undo() override
		{
			original->pop_back();
		}
	};


	/*!***********************************************************************
		\brief
			Command to erase a value from a vector at a specific index.
			This command stores a pointer to the original vector, the index of the value to be erased,
			and the value itself. It allows for executing the erase action and undoing it by inserting the value back.
	*************************************************************************/
	template <typename ValType>
	class VectorEraseCommand : public ICommand
	{
	private:
		std::vector<ValType>* original;
		int distance;
		ValType val;

	public:
		VectorEraseCommand(std::vector<ValType>* OD, int data)
			: original(OD), distance(data)
		{
			val = (*original)[distance];
		}

		void execute() override
		{
			original->erase(original->begin() + distance);
		}

		void undo() override
		{
			original->insert(original->begin() + distance, val);
		}
	};

	/*!***********************************************************************
		\brief
			Command to clear a container.
			This command stores a pointer to the container and a copy of its data before clearing.
			It allows for executing the clear action and undoing it by restoring the original data.
		\param Container
			The type of the container being cleared.
	*************************************************************************/
	template <typename Container>
	class ClearContainerCommand : public ICommand
	{
	private:
		Container* container;
		Container oldData;

		static_assert(has_clear_v<Container>, "ClearContainerCommand requires Container to have a clear() method");

	public:
		ClearContainerCommand(Container* ogContainer) : container(ogContainer), oldData(*ogContainer) {}

		void execute() override
		{
			container->clear();
		}

		void undo() override
		{
			*container = oldData;
		}
	};

	/*!***********************************************************************
		\brief
			Command to add an entry to the store aggregate data.
			This command stores a pointer to the original data, the new key, and the entrance and time buffers.
			It allows for executing the addition and undoing it by removing the entry.
	*************************************************************************/
	class AddStoreAggregateEntry : public ICommand
	{
	private:
		std::map<Project::StoreCode, std::map<Project::Hour, AggregateData>>* mOriginalData;
		Project::StoreCode newKey;
		int EntranceBuffer;
		int TimeBuffer;

		// derived
		int oldEntrances;
		bool hadOld = false;
	public:
		AddStoreAggregateEntry(std::map<Project::StoreCode, std::map<Project::Hour, AggregateData>>* countData, Project::StoreCode key, int ent, int time) : mOriginalData(countData), newKey(key), EntranceBuffer(ent), TimeBuffer(time)
		{
			oldEntrances = 0;
		}
		void execute() override
		{
			if (!mOriginalData->contains(newKey))
			{
				(*mOriginalData)[newKey] = {};
				hadOld = true;
			}

			if (!(*mOriginalData)[newKey].empty())
			{
				oldEntrances = (int)(*mOriginalData)[newKey].begin()->second.Entrance.size();
				if (EntranceBuffer > oldEntrances)
				{
					for (auto& [time, counter] : (*mOriginalData)[newKey])
					{
						counter.Entrance.resize(EntranceBuffer, {});
					}
				}
				else
				{
					EntranceBuffer = (int)(*mOriginalData)[newKey].begin()->second.Entrance.size();
				}
			}

			if (!(*mOriginalData)[newKey].contains(TimeBuffer))
			{
				(*mOriginalData)[newKey][TimeBuffer] = {};
				for (int i = 0; i < EntranceBuffer; i++)
					(*mOriginalData)[newKey][TimeBuffer].Entrance.push_back({});
			}
		}

		void undo() override
		{
			(*mOriginalData)[newKey].erase(TimeBuffer);
			if (hadOld)
			{
				mOriginalData->erase(newKey);
			}
			else if (!(*mOriginalData)[newKey].empty())
			{
				if (EntranceBuffer > oldEntrances)
				{
					for (auto& [time, counter] : (*mOriginalData)[newKey])
					{
						counter.Entrance.resize(oldEntrances, {});
					}
				}
			}
		}
	};

	/*!***********************************************************************
		\brief
			Command to add an entry to the store entry counting data.
			This command stores a pointer to the original data, the new key, and the entrance and time buffers.
			It allows for executing the addition and undoing it by removing the entry.
	*************************************************************************/
	class AddStoreEntryCounting : public ICommand
	{
	private:
		std::map<Project::StoreCode, std::map<Project::Hour, CountData>>* mOriginalData;
		Project::StoreCode newKey;
		int EntranceBuffer;
		int TimeBuffer;

		// derived
		int oldEntrances;
		bool hadOld = false;
	public:
		AddStoreEntryCounting(std::map<Project::StoreCode, std::map<Project::Hour, CountData>>* countData, Project::StoreCode key, int ent, int time) : mOriginalData(countData), newKey(key), EntranceBuffer(ent), TimeBuffer(time)
		{
			oldEntrances = 0;
		}
		void execute() override
		{
			if (!mOriginalData->contains(newKey))
			{
				(*mOriginalData)[newKey] = {};
				hadOld = true;
			}

			if (!(*mOriginalData)[newKey].empty())
			{
				oldEntrances = (int)(*mOriginalData)[newKey].begin()->second.Entrance.size();
				if (EntranceBuffer > oldEntrances)
				{
					for (auto& [time, counter] : (*mOriginalData)[newKey])
					{
						counter.Entrance.resize(EntranceBuffer, {});
					}
				}
				else
				{
					EntranceBuffer = (int)(*mOriginalData)[newKey].begin()->second.Entrance.size();
				}
			}

			if (!(*mOriginalData)[newKey].contains(TimeBuffer))
			{
				(*mOriginalData)[newKey][TimeBuffer] = {};
				for (int i = 0; i < EntranceBuffer; i++)
					(*mOriginalData)[newKey][TimeBuffer].Entrance.push_back({});
			}
		}

		void undo() override
		{
			(*mOriginalData)[newKey].erase(TimeBuffer);
			if (hadOld)
			{
				mOriginalData->erase(newKey);
			}
			else if (!(*mOriginalData)[newKey].empty())
			{
				if (EntranceBuffer > oldEntrances)
				{
					for (auto& [time, counter] : (*mOriginalData)[newKey])
					{
						counter.Entrance.resize(oldEntrances, {});
					}
				}
			}
		}
	};
}

#endif
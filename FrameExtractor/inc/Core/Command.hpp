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
#include <type_traits>
#include <utility> // for std::declval
#include <vector>
#include <map>
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
		static void execute(std::shared_ptr<ICommand> command);
		static void undo();
		static void redo();
		static void markSaved();
		static bool isDirty();
		static bool CanRedo();
		static bool CanUndo();
	private:
		static constexpr size_t MAX_HISTORY = 150;
		static void TrimStack(std::deque<std::shared_ptr<ICommand>>& stack);
		static std::deque<std::shared_ptr<ICommand>> undoStack;
		static std::deque<std::shared_ptr<ICommand>> redoStack;
		static std::weak_ptr<ICommand> savedCommand;
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

	template <typename PropVal>
	class ModifyPropertyPairCommand : public ICommand {
	private:
		PropVal* originalData1, *originalData2;
		PropVal oldValue1, newValue1, oldValue2, newValue2;
	public:
		ModifyPropertyPairCommand(PropVal* instance1, PropVal oldVal1, PropVal newVal1, PropVal* instance2, PropVal oldVal2, PropVal newVal2)
			: originalData1(instance1), oldValue1(oldVal1), newValue1(newVal1), originalData2(instance2), oldValue2(oldVal2), newValue2(newVal2) {
		}

		void execute() override {
			*originalData1 = newValue1;
			*originalData2 = newValue2;

		}

		void undo() override {
			*originalData1 = oldValue1;
			*originalData2 = oldValue2;

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
			: originalData(OD), duplicateData(*OD), distance(dist) {
		}

		void execute() override {
			
			auto IT = originalData->begin();
			std::advance(IT, distance);
			originalData->erase(IT);
		}

		void undo() override {
			*originalData = duplicateData;
		}
	};

	template <typename Key, typename Val>
	class AddKeyCommand : public ICommand
	{
	private:
		std::map<Key,Val>* originalData;
		Key newKey;
		Val value;

	public:
		AddKeyCommand(std::map<Key, Val>* OD, Key key, Val val)
			: originalData(OD), newKey(key), value(val) {
		}

		void execute() override {
			(*originalData)[newKey] = value;
		}

		void undo() override {
			originalData->erase(newKey);
		}
	};

	template <typename ValType>
	class PushBackCommand : public ICommand
	{
	private:
		std::vector<ValType>* original;
		ValType val;

	public:
		PushBackCommand(std::vector<ValType>* OD, ValType data)
			: original(OD), val(data) {
		}

		void execute() override {

			original->push_back(val);
		}

		void undo() override {
			original->pop_back();
		}
	};

	template <typename ValType>
	class VectorEraseCommand : public ICommand
	{
	private:
		std::vector<ValType>* original;
		int distance;
		ValType val;

	public:
		VectorEraseCommand(std::vector<ValType>* OD, int data)
			: original(OD), distance(data) {
			val = (*original)[distance];
		}

		void execute() override {

			original->erase(original->begin() + distance);
		}

		void undo() override {
			original->insert(original->begin() + distance, val);
		}
	};

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
		void execute() override {
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
		void execute() override {
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


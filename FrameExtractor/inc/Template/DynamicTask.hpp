/******************************************************************************
/*!
\file       DynamicTask.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       June 23, 2025
\brief      Declares the Dynamic Task struct which represents a task that can
			be a template reference
 /******************************************************************************/
#ifndef Dynamic_Task_HPP
#define Dynamic_Task_HPP
#include <string>
#include <rttr/variant.h>
#include <Template/TemplateDataType.hpp>
#include <Template/DataSpecification.hpp>
#include <Template/TemplateEnums.hpp>
namespace FrameExtractor
{
	struct VariantLess
	{
		bool operator()(const rttr::variant& lhs, const rttr::variant& rhs) const
		{
			// Compare types first
			if (lhs.get_type() != rhs.get_type())
				return lhs.get_type().get_name().to_string() < rhs.get_type().get_name().to_string();

			// Then compare the values based on type
			if (lhs.can_convert<int>())
			{
				return lhs.to_int() < rhs.to_int();
			}
			else if (lhs.can_convert<std::string>())
			{
				return lhs.to_string() < rhs.to_string();
			}
			else if (lhs.can_convert<double>())
			{
				return lhs.to_double() < rhs.to_double();
			}
			else if (lhs.can_convert<float>())
			{
				return lhs.to_float() < rhs.to_float();
			}
			else if (lhs.can_convert<bool>())
			{
				return lhs.to_bool() < rhs.to_bool();
			}
			else if (lhs.can_convert<FrameExtractor::Time>())
			{
				return lhs.get_value<FrameExtractor::Time>().to_string() < rhs.get_value<FrameExtractor::Time>().to_string();
			}
			else if (lhs.can_convert<FrameExtractor::Date>())
			{
				return lhs.get_value<FrameExtractor::Date>().to_string() < rhs.get_value<FrameExtractor::Date>().to_string();
			}

			// Fallback: compare memory address as last resort
			return lhs.get_value<std::uintptr_t>() < rhs.get_value<std::uintptr_t>();
		}
	};

	using ReflectionMap = std::map<rttr::variant, rttr::variant, VariantLess>;

	struct TaskData
	{
		std::string mFieldName;
		rttr::variant mFieldData;
	};

	TaskData GenerateTaskData(DataSpecification& specs);

	enum class PrintSetting
	{
		Loop,
		NoLoop
	};

	struct ExcelExport
	{
		std::map<int, std::string> mColumnHeaders; // Column Index -> Header name
		std::map<int, std::string> mDataColumnMapping; // Field Name -> Column Index
	};

	struct ExcelImport
	{
		std::map<int, std::string> mDataMapping;
		bool mHasHeader = false; // Whether the first row is a header row
	};

	struct DynamicTaskSpecs
	{
		IOType* mOutputType = nullptr;
		IOType* mInputType = nullptr;
		DataSpecification* mTab = nullptr;
		std::vector<DataSpecification*>* mNodeCategories = nullptr;
		std::vector<DataSpecification*>* mFieldSpecs = nullptr;
		rttr::variant* mImportFormat = nullptr;
		rttr::variant* mExportFormat = nullptr;

		inline bool HasTab() { return mTab != nullptr; }
		inline int MaxCategories() { if (mNodeCategories != nullptr) return (int)mNodeCategories->size(); else return 0; }
		inline int MaxDepth() { if (HasTab()) return MaxCategories() + 1; else return MaxCategories(); }
		DynamicTaskSpecs() = default;
		DynamicTaskSpecs(const DynamicTaskSpecs& other);
		DynamicTaskSpecs(DynamicTaskSpecs&& other) noexcept;
		~DynamicTaskSpecs();
		DynamicTaskSpecs& operator=(DynamicTaskSpecs&& other) noexcept;
		DynamicTaskSpecs& operator=(const DynamicTaskSpecs& other);
	};

	struct DynamicTask
	{
		std::string mTaskName;
		ReflectionMap mPages;
		DynamicTaskSpecs* mSpecs = nullptr;

		DynamicTask() = default;
		DynamicTask(DynamicTaskSpecs& specs);
		DynamicTask(const DynamicTask& other);
		DynamicTask(DynamicTask&& other) noexcept;
		DynamicTask& operator=(DynamicTask&& other) noexcept;
		DynamicTask& operator=(const DynamicTask& other);
		~DynamicTask();

		std::vector<TaskData> GenerateFromSpecs();
		void AddPage(const std::vector<rttr::variant>& keys, const rttr::variant& value);
		void AddTab(const std::vector<rttr::variant>& keys, const rttr::variant& value);

		ReflectionMap& GetTabs(std::vector<rttr::variant>& keys);
		std::vector<TaskData>& GetTab(std::vector<int>& indices);
		rttr::variant& GetTab(std::vector<rttr::variant>& keys);
		void RemoveTab(std::vector<int>& indices);
	};

	class DynamicTaskInterface
	{
	public:
		DynamicTaskInterface() = delete; // Prevent default constructor
		DynamicTaskInterface(DynamicTask* task)
			: mTask(task)
		{
			mIndices.resize(task->mSpecs->MaxDepth(), 0); // Reserve space for indices based on max depth
			mLayeredBuffer.resize(task->mSpecs->MaxDepth(), rttr::variant()); // Reserve space for layered buffer
		}

		DynamicTaskInterface(const DynamicTaskInterface& other)
			: mTask(other.mTask), mIndices(other.mIndices), mLayeredBuffer(other.mLayeredBuffer)
		{}

		DynamicTaskInterface(DynamicTaskInterface&& other) noexcept
			: mTask(other.mTask), mIndices(other.mIndices), mLayeredBuffer(other.mLayeredBuffer)
		{
			other.mTask = nullptr; // Transfer ownership
			other.mIndices.clear(); // Clear the indices of the moved-from object
			other.mLayeredBuffer.clear();
		}

		DynamicTaskInterface& operator=(DynamicTaskInterface&& other) noexcept
		{
			if (this != &other)
			{
				mTask = other.mTask; // Transfer ownership
				other.mTask = nullptr; // Nullify the other task
				mIndices = std::move(other.mIndices); // Move indices
				other.mIndices.clear(); // Clear the indices of the moved-from object
				mLayeredBuffer = std::move(other.mLayeredBuffer); // Move layered buffer
				other.mLayeredBuffer.clear(); // Clear the layered buffer of the moved-from object
			}
			return *this;
		}

		DynamicTaskInterface& operator=(const DynamicTaskInterface& other)
		{
			if (this != &other)
			{
				mTask = other.mTask; // Copy the task pointer
				mIndices = other.mIndices; // Copy the indices
				mLayeredBuffer = other.mLayeredBuffer; // Copy the layered buffer
			}
			return *this;
		}

		bool empty() { return mTask->mPages.empty(); }
		std::vector<TaskData>& GetData();
		std::pair<std::vector<rttr::variant>, std::vector<TaskData>> DeleteCurrentLeaf();
		std::pair<std::vector<rttr::variant>, std::vector<TaskData>> DeleteLeaf(int idx);
		std::pair<std::vector<rttr::variant>, rttr::variant> DeleteLayer(int layer);
		void AddLayer(std::vector<rttr::variant>, const rttr::variant& value);
		void AddLeaf(rttr::variant key);
		void AddLeaf(std::vector<rttr::variant>& key);
		ReflectionMap& GetLayer(int layer);
		inline int LayerSize()
		{
			return static_cast<int>(mIndices.size());
		}

		int& operator[](int index)
		{
			if (index < 0 || index >= static_cast<int>(mIndices.size()))
			{
				throw std::out_of_range("Index out of range in DynamicTaskInterface");
			}
			return mIndices[index];
		}

		int operator[](int index) const
		{
			if (index < 0 || index >= static_cast<int>(mIndices.size()))
			{
				throw std::out_of_range("Index out of range in DynamicTaskInterface");
			}
			return mIndices[index];
		}
		std::vector<rttr::variant> mLayeredBuffer;

	private:
		void BackCheckPath(std::vector<rttr::variant>& vars);
		DynamicTask* mTask = nullptr;
		std::vector<int> mIndices; // Indices to access the task data
	};

	DynamicTask GenerateTask(std::filesystem::path path);

	void DumpTask(DynamicTask task, int tab = 0);
}
#endif
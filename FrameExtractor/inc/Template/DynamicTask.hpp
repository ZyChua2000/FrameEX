/******************************************************************************/
/*!
\file       DynamicTask.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       June 23, 2025
\brief      Declares the Dynamic Task struct which represents a task that can
			be a template reference
 ******************************************************************************/
#ifndef Dynamic_Task_HPP
#define Dynamic_Task_HPP
 // Standard Library includes
#include <string>

// Third-party includes
#include <rttr/variant.h>

// Project includes
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

	/*!***********************************************************************
		\brief
			Structure that holds the data for a task, including field name and data.
	*************************************************************************/
	struct TaskData
	{
		std::string mFieldName;
		rttr::variant mFieldData;
	};

	/*!***********************************************************************
		\brief
			Generates a TaskData object based on the provided DataSpecification.
		\param[in] specs
			The DataSpecification object containing the specifications for the task data.
		\return
			A TaskData object populated with the field name and data based on the specifications.
	*************************************************************************/
	TaskData GenerateTaskData(DataSpecification& specs);

	/*!***********************************************************************
		\brief
			Structure that holds the specifications for printing tasks.
	*************************************************************************/
	enum class PrintSetting
	{
		Loop,
		NoLoop
	};

	/*!***********************************************************************
		\brief
			Structure that holds the specifications for Excel export
	*************************************************************************/
	struct ExcelExport
	{
		std::map<int, std::string> mColumnHeaders; // Column Index -> Header name
		std::map<int, std::string> mDataColumnMapping; // Field Name -> Column Index
	};

	/*!***********************************************************************
		\brief
			Structure that holds the specifications for Excel import
	*************************************************************************/
	struct ExcelImport
	{
		std::map<int, std::string> mDataMapping;
		bool mHasHeader = false; // Whether the first row is a header row
	};

	/*!***********************************************************************
		\brief
			Structure that holds the specifications for Dynamic Tasks
	*************************************************************************/
	struct DynamicTaskSpecs
	{
		IOType* mOutputType = nullptr;
		IOType* mInputType = nullptr;
		DataSpecification* mTab = nullptr;
		std::vector<DataSpecification*>* mNodeCategories = nullptr;
		std::vector<DataSpecification*>* mFieldSpecs = nullptr;
		rttr::variant* mImportFormat = nullptr;
		rttr::variant* mExportFormat = nullptr;

		/*!***********************************************************************
			\brief
				Checks if the DynamicTaskSpecs has a tab defined.
			\return
				True if a tab is defined, false otherwise.
		*************************************************************************/
		inline bool HasTab() { return mTab != nullptr; }

		/*!***********************************************************************
			\brief
				Checks how many categories are defined in the DynamicTaskSpecs.
			\return
				The number of categories defined, or 0 if none.
		*************************************************************************/
		inline int MaxCategories() { if (mNodeCategories != nullptr) return (int)mNodeCategories->size(); else return 0; }

		/*!***********************************************************************
			\brief
				Calculates the maximum depth of the DynamicTaskSpecs.
			\return
				The maximum depth, which is one more than the number of categories 
				if a tab is defined, otherwise equal to the number of categories.
		*************************************************************************/
		inline int MaxDepth() { if (HasTab()) return MaxCategories() + 1; else return MaxCategories(); }

		/*!***********************************************************************
			\brief
				Default constructor for DynamicTaskSpecs.
		*************************************************************************/
		DynamicTaskSpecs() = default;

		/*!***********************************************************************
			\brief
				Copy constructor for DynamicTaskSpecs.
			\param[in] other
				The DynamicTaskSpecs object to copy from.
		*************************************************************************/
		DynamicTaskSpecs(const DynamicTaskSpecs& other);

		/*!***********************************************************************
			\brief
				Move constructor for DynamicTaskSpecs.
			\param[in,out] other
				The DynamicTaskSpecs object to move from. After the move, the state of
				the source object is unspecified but valid.
			\details
				Transfers ownership of resources from the source object to the new
				object without performing deep copies. This constructor is noexcept.
		*************************************************************************/
		DynamicTaskSpecs(DynamicTaskSpecs&& other) noexcept;

		/*!***********************************************************************
			\brief
				Destructor for DynamicTaskSpecs.
			\details
				Cleans up resources used by the DynamicTaskSpecs object.
		*************************************************************************/
		~DynamicTaskSpecs();

		/*!***********************************************************************
			\brief
				Assignment operator for DynamicTaskSpecs.
			\param[in,out] other
				The DynamicTaskSpecs object to assign from. After the assignment, the state of
				the source object is unspecified but valid.
			\details
				Transfers ownership of resources from the source object to this object without performing deep copies.
				This operator is noexcept.
		*************************************************************************/
		DynamicTaskSpecs& operator=(DynamicTaskSpecs&& other) noexcept;

		/*!***********************************************************************
			\brief
				Copy assignment operator for DynamicTaskSpecs.
			\param[in] other
				The DynamicTaskSpecs object to copy from.
			\return
				A reference to this object after the assignment.
		*************************************************************************/
		DynamicTaskSpecs& operator=(const DynamicTaskSpecs& other);
	};

	/*!***********************************************************************
		\brief
			Structure that represents a dynamic task, which can be generated from specifications.
	*************************************************************************/
	struct DynamicTask
	{
		std::string mTaskName;
		ReflectionMap mPages;
		DynamicTaskSpecs* mSpecs = nullptr;

		/*!***********************************************************************
			\brief
				Default constructor for DynamicTask.
			\details
				Creates an empty DynamicTask object.
		*************************************************************************/
		DynamicTask() = default;

		/*!***********************************************************************
			\brief
				Constructor that initializes the DynamicTask with specifications.
			\param[in] specs
				The DynamicTaskSpecs object containing the specifications for the task.
			\details
				Creates a DynamicTask object based on the provided specifications.
		*************************************************************************/
		DynamicTask(DynamicTaskSpecs& specs);

		/*!***********************************************************************
			\brief
				Copy constructor for DynamicTask.
			\param[in] other
				The DynamicTask object to copy from.
			\details
				Creates a new DynamicTask object as a copy of the given object.
				Performs a deep copy of the internal data if necessary.
		*************************************************************************/
		DynamicTask(const DynamicTask& other);

		/*!***********************************************************************
			\brief
				Move constructor for DynamicTask.
			\param[in,out] other
				The DynamicTask object to move from. After the move, the state of
				the source object is unspecified but valid.
			\details
				Transfers ownership of resources from the source object to the new
				object without performing deep copies. This constructor is noexcept.
		*************************************************************************/
		DynamicTask(DynamicTask&& other) noexcept;

		/*!***********************************************************************
			\brief
				Move assignment operator for DynamicTask.
			\param[in,out] other
				The DynamicTask object to assign from. After the assignment, the state of
				the source object is unspecified but valid.
			\details
				Transfers ownership of resources from the source object to this object without performing deep copies.
				This operator is noexcept.
			\return
				A reference to this object after the assignment.
		*************************************************************************/
		DynamicTask& operator=(DynamicTask&& other) noexcept;

		/*!***********************************************************************
			\brief
				Copy assignment operator for DynamicTask.
			\param[in] other
				The DynamicTask object to copy from.
			\details
				Copies the internal data from the given object to this object.
			\return
				A reference to this object after the assignment.
		*************************************************************************/
		DynamicTask& operator=(const DynamicTask& other);

		/*!***********************************************************************
			\brief
				Destructor for DynamicTask.
			\details
				Cleans up resources used by the DynamicTask object.
		*************************************************************************/
		~DynamicTask();

		/*!***********************************************************************
			\brief
				Generates a vector of TaskData objects based on the specifications.
			\return
				A vector of TaskData objects generated from the specifications.
		*************************************************************************/
		std::vector<TaskData> GenerateFromSpecs();

		/*!***********************************************************************
			\brief
				Adds a page to the DynamicTask.
			\param[in] keys
				A vector of keys leading up to the page.
			\param[in] value
				The value to associate with the keys.
			\details
				Adds a new page to the DynamicTask's pages map using the provided keys and value.
		*************************************************************************/
		void AddPage(const std::vector<rttr::variant>& keys, const rttr::variant& value);

		/*!***********************************************************************
			\brief
				Adds a tab to the DynamicTask.
			\param[in] keys
				A vector of keys leading up to the tab.
			\param[in] value
				The value to associate with the keys.
			\details
				Adds a new tab to the DynamicTask's pages map using the provided keys and value.
		*************************************************************************/
		void AddTab(const std::vector<rttr::variant>& keys, const rttr::variant& value);

		/*!***********************************************************************
			\brief
				Gets the tabs from the end of the map
			\param[in] keys
				A vector of keys leading up to the tabs.
			\return
				A reference to the ReflectionMap containing the tabs.
		*************************************************************************/
		ReflectionMap& GetTabs(std::vector<rttr::variant>& keys);

		/*!***********************************************************************
			\brief
				Gets the tab data from the DynamicTask.
			\param[in] indices
				A vector of indices to access the specific tab data.
		*************************************************************************/
		std::vector<TaskData>& GetTab(std::vector<int>& indices);

		/*!***********************************************************************
			\brief
				Gets the tab data from the DynamicTask.
			\param[in] keys
				A vector of keys leading up to the specific tab data.
			\return
				A reference to the variant containing the tab data.
			\details
				Retrieves the tab data associated with the provided keys from the DynamicTask's pages map.
		*************************************************************************/
		rttr::variant& GetTab(std::vector<rttr::variant>& keys);

		/*!***********************************************************************
			\brief
				Removes a tab from the DynamicTask.
			\param[in] indices
				A vector of indices to identify the tab to be removed.
			\details
				Removes the specified tab from the DynamicTask's pages map based on the provided indices.
		*************************************************************************/
		void RemoveTab(std::vector<int>& indices);
	};

	/*!***********************************************************************
		\brief
			Class that provides an interface for interacting with a DynamicTask.
	*************************************************************************/
	class DynamicTaskInterface
	{
	public:
		DynamicTaskInterface() = delete; // Prevent default constructor

		/*!***********************************************************************
			\brief
				Constructor that initializes the DynamicTaskInterface with a DynamicTask.
			\param[in] task
				The DynamicTask object to interface with.
			\details
				Creates a DynamicTaskInterface that allows interaction with the specified DynamicTask.
		*************************************************************************/
		DynamicTaskInterface(DynamicTask* task)
			: mTask(task)
		{
			mIndices.resize(task->mSpecs->MaxDepth(), 0); // Reserve space for indices based on max depth
			mLayeredBuffer.resize(task->mSpecs->MaxDepth(), rttr::variant()); // Reserve space for layered buffer
		}


		/*!***********************************************************************
			\brief
				Copy constructor for DynamicTaskInterface.
			\param[in] other
				The DynamicTaskInterface object to copy from.
			\details
				Creates a new DynamicTaskInterface object as a copy of the given object.
				Performs a deep copy of the internal data if necessary.
		*************************************************************************/
		DynamicTaskInterface(const DynamicTaskInterface& other)
			: mTask(other.mTask), mIndices(other.mIndices), mLayeredBuffer(other.mLayeredBuffer)
		{}


		/*!***********************************************************************
			\brief
				Move constructor for DynamicTaskInterface.
			\param[in,out] other
				The DynamicTaskInterface object to move from. After the move, the state of
				the source object is unspecified but valid.
			\details
				Transfers ownership of resources from the source object to the new
				object without performing deep copies. This constructor is noexcept.
		*************************************************************************/
		DynamicTaskInterface(DynamicTaskInterface&& other) noexcept
			: mTask(other.mTask), mIndices(other.mIndices), mLayeredBuffer(other.mLayeredBuffer)
		{
			other.mTask = nullptr; // Transfer ownership
			other.mIndices.clear(); // Clear the indices of the moved-from object
			other.mLayeredBuffer.clear();
		}

		/*!***********************************************************************
			\brief
				Move assignment operator for DynamicTaskInterface.
			\param[in,out] other
				The DynamicTaskInterface object to assign from. After the assignment, the state of
				the source object is unspecified but valid.
			\details
				Transfers ownership of resources from the source object to this object without performing deep copies.
				This operator is noexcept.
			\return
				A reference to this object after the assignment.
		*************************************************************************/
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

		/*!***********************************************************************
			\brief
				Copy assignment operator for DynamicTaskInterface.
			\param[in] other
				The DynamicTaskInterface object to copy from.
			\details
				Copies the internal data from the given object to this object.
			\return
				A reference to this object after the assignment.
		*************************************************************************/
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

		/*!***********************************************************************
			\brief
				Checks if the DynamicTaskInterface is empty.
			\return
				True if the task has no pages, false otherwise.
		*************************************************************************/
		bool empty() { return mTask->mPages.empty(); }

		/*!***********************************************************************
			\brief
				Gets all the data relating to the Task
			\return
				The vector of TaskData associated with the current task.
		*************************************************************************/
		std::vector<TaskData>& GetData();

		/*!***********************************************************************
			\brief
				Deletes the current leaf from the DynamicTaskInterface.
			\return
				The keys and data associated with the current leaf.
		*************************************************************************/
		std::pair<std::vector<rttr::variant>, std::vector<TaskData>> DeleteCurrentLeaf();

		/*!***********************************************************************
			\brief
				Deletes a leaf from the DynamicTaskInterface.
			\param[in] idx
				The index of the leaf to be deleted.
			\return
				A pair containing a vector of rttr::variant representing the keys and a vector of TaskData representing the data associated with the leaf.
			\details
				Removes the specified leaf from the DynamicTaskInterface and returns its keys and data.
		*************************************************************************/
		std::pair<std::vector<rttr::variant>, std::vector<TaskData>> DeleteLeaf(int idx);

		/*!***********************************************************************
			\brief
				Deletes a layer from the DynamicTaskInterface.
			\param[in] layer
				The index of the layer to be deleted.
			\return
				A pair containing a vector of rttr::variant representing the keys and a variant representing the value associated with the layer.
			\details
				Removes the specified layer from the DynamicTaskInterface and returns its keys and value.
		*************************************************************************/
		std::pair<std::vector<rttr::variant>, rttr::variant> DeleteLayer(int layer);

		/*!***********************************************************************
			\brief
				Adds a layer to the DynamicTaskInterface.
			\param[in] keys
				A vector of rttr::variant representing the keys for the layer.
			\param[in] value
				The value to associate with the keys.
			\details
				Adds a new layer to the DynamicTaskInterface using the provided keys and value.
		*************************************************************************/
		void AddLayer(std::vector<rttr::variant>, const rttr::variant& value);

		/*!***********************************************************************
			\brief
				Adds a leaf to the DynamicTaskInterface.
			\param[in] key
				A rttr::variant representing the key for the leaf from the back.
			\details
				Adds a new leaf to the DynamicTaskInterface using the provided key.
		*************************************************************************/
		void AddLeaf(rttr::variant key);

		/*!***********************************************************************
			\brief
				Adds a leaf to the DynamicTaskInterface.
			\param[in] key
				A vector of rttr::variant representing the key for the leaf.
			\details
				Adds a new leaf to the DynamicTaskInterface using the provided key.
		*************************************************************************/
		void AddLeaf(std::vector<rttr::variant>& key);

		/*!***********************************************************************
			\brief
				Gets a specific layer from the DynamicTaskInterface.
			\param[in] layer
				The index of the layer to access.
			\return
				The ReflectionMap associated with the specified layer.
		*************************************************************************/
		ReflectionMap& GetLayer(int layer);

		/*!***********************************************************************
			\brief
				Gets the size of the layer in the DynamicTaskInterface.
			\return
				The size of the layer, which is the number of indices stored.
		*************************************************************************/
		inline int LayerSize()
		{
			return static_cast<int>(mIndices.size());
		}

		/*!***********************************************************************
			\brief
				Accesses the indices of the DynamicTaskInterface.
			\param[in] index
				The index to access.
			\return
				A reference to the value at the specified index.
			\details
				Throws an out_of_range exception if the index is invalid.
		*************************************************************************/
		int& operator[](int index)
		{
			if (index < 0 || index >= static_cast<int>(mIndices.size()))
			{
				throw std::out_of_range("Index out of range in DynamicTaskInterface");
			}
			return mIndices[index];
		}


		/*!***********************************************************************
			\brief
				Accesses the indices of the DynamicTaskInterface.
			\param[in] index
				The index to access.
			\return
				The value at the specified index.
			\details
				Throws an out_of_range exception if the index is invalid.
		*************************************************************************/
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
		/*!***********************************************************************
			\brief
				Checks if the path is valid and contains a valid task.
			\param[in] vars
				A vector of rttr::variant containing the path to check.
			\details
				Validates the path and checks if it contains a valid task.
		*************************************************************************/
		void BackCheckPath(std::vector<rttr::variant>& vars);
		DynamicTask* mTask = nullptr;
		std::vector<int> mIndices; // Indices to access the task data
	};

	/*!***********************************************************************
		\brief
			Generates a DynamicTask from the given path.
		\param[in] path
			The path to the file or directory from which to generate the task.
		\return
			A DynamicTask object generated from the specified path.
	*************************************************************************/
	DynamicTask GenerateTask(std::filesystem::path path);

	/*!***********************************************************************
		\brief
			Dumps the contents of a DynamicTask to the console.
		\param[in] task
			The DynamicTask object to dump.
		\param[in] tab
			The indentation level for formatting the output.
	*************************************************************************/
	void DumpTask(DynamicTask task, int tab = 0);
}
#endif
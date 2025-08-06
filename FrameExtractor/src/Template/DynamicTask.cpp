/******************************************************************************
/*!
\file       DynamicTask.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       June 23, 2025
\brief      Defines the Dynamic Task struct which represents a task that can
			be a template reference
 /******************************************************************************/

#include <FrameExtractorPCH.hpp>
#include <Template/DynamicTask.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <Core/LoggerManager.hpp>
#include <magic_enum/magic_enum.hpp>
#include <Template/TemplateDataType.hpp>

namespace YAML
{
	template<>
	struct convert<FrameExtractor::Date>
	{
		static Node encode(const FrameExtractor::Date& date)
		{
			Node node;
			auto ymd = date.to_chrono();
			node["year"] = int(ymd.year());
			node["month"] = unsigned(ymd.month());
			node["day"] = unsigned(ymd.day());
			return node;
		}

		static bool decode(const Node& node, FrameExtractor::Date& ymd)
		{
			if (!node.IsMap() || !node["year"] || !node["month"] || !node["day"])
				return false;

			int year = node["year"].as<int>();
			unsigned month = node["month"].as<unsigned>();
			unsigned day = node["day"].as<unsigned>();

			ymd = std::chrono::year{ year } / std::chrono::month{ month } / std::chrono::day{ day };
			return true;
		}
	};

	template<>
	struct convert<FrameExtractor::Time>
	{
		static Node encode(const FrameExtractor::Time& time)
		{
			Node node;
			auto t = time.to_chrono();
			node["hours"] = t.hours().count();
			node["minutes"] = t.minutes().count();
			node["seconds"] = t.seconds().count();
			return node;
		}

		static bool decode(const Node& node, FrameExtractor::Time& t)
		{
			if (!node.IsMap() || !node["hours"] || !node["minutes"] || !node["seconds"])
				return false;

			using namespace std::chrono;
			seconds total_seconds =
				hours{ node["hours"].as<int>() } +
				minutes{ node["minutes"].as<int>() } +
				seconds{ node["seconds"].as<int>() };

			t = hh_mm_ss<seconds>{ total_seconds };
			return true;
		}
	};
}

namespace FrameExtractor
{
	DynamicTaskSpecs::DynamicTaskSpecs(const DynamicTaskSpecs& other)
	{
		mOutputType = other.mOutputType ? new IOType(*other.mOutputType) : nullptr;
		mInputType = other.mInputType ? new IOType(*other.mInputType) : nullptr;
		mTab = other.mTab ? new DataSpecification(*other.mTab) : nullptr;

		if (other.mNodeCategories)
		{
			mNodeCategories = new std::vector<DataSpecification*>();
			mNodeCategories->reserve(other.mNodeCategories->size());
			for (auto* cat : *other.mNodeCategories)
			{
				mNodeCategories->push_back(cat ? new DataSpecification(*cat) : nullptr);
			}
		}

		if (other.mFieldSpecs)
		{
			mFieldSpecs = new std::vector<DataSpecification*>();
			mFieldSpecs->reserve(other.mFieldSpecs->size());
			for (auto* field : *other.mFieldSpecs)
			{
				mFieldSpecs->push_back(field ? new DataSpecification(*field) : nullptr);
			}
		}
	}
	DynamicTaskSpecs::DynamicTaskSpecs(DynamicTaskSpecs&& other) noexcept
	{
		mOutputType = other.mOutputType;
		mInputType = other.mInputType;
		mTab = other.mTab;
		mNodeCategories = other.mNodeCategories;
		mFieldSpecs = other.mFieldSpecs;

		other.mOutputType = nullptr;
		other.mInputType = nullptr;
		other.mTab = nullptr;
		other.mNodeCategories = nullptr;
		other.mFieldSpecs = nullptr;
	}
	DynamicTaskSpecs& DynamicTaskSpecs::operator=(DynamicTaskSpecs&& other) noexcept
	{
		if (this == &other) return *this;

		// Clean up current
		if (mOutputType)
			delete mOutputType;
		if (mInputType)
			delete mInputType;
		if (mTab)
			delete mTab;

		if (mNodeCategories)
		{
			for (auto& cat : *mNodeCategories) delete cat;
			delete mNodeCategories;
		}

		if (mFieldSpecs)
		{
			for (auto& field : *mFieldSpecs) delete field;
			delete mFieldSpecs;
		}

		// Transfer ownership
		mOutputType = other.mOutputType;
		mInputType = other.mInputType;
		mTab = other.mTab;
		mNodeCategories = other.mNodeCategories;
		mFieldSpecs = other.mFieldSpecs;

		other.mOutputType = nullptr;
		other.mInputType = nullptr;
		other.mTab = nullptr;
		other.mNodeCategories = nullptr;
		other.mFieldSpecs = nullptr;

		return *this;
	}
	DynamicTaskSpecs& DynamicTaskSpecs::operator=(const DynamicTaskSpecs& other)
	{
		// Prevent self-assignment
		if (this == &other) return *this;

		// Clean up existing resources
		if (mOutputType)
			delete mOutputType;
		if (mInputType)
			delete mInputType;
		if (mTab)
			delete mTab;

		if (mNodeCategories)
		{
			for (auto& cat : *mNodeCategories)
				delete cat;
			delete mNodeCategories;
		}

		if (mFieldSpecs)
		{
			for (auto& field : *mFieldSpecs)
				delete field;
			delete mFieldSpecs;
		}

		// Deep copy individual pointers (check for nullptr)
		mOutputType = other.mOutputType ? new IOType(*other.mOutputType) : nullptr;
		mInputType = other.mInputType ? new IOType(*other.mInputType) : nullptr;
		mTab = other.mTab ? new DataSpecification(*other.mTab) : nullptr;

		// Deep copy vector of pointers: mNodeCategories
		if (other.mNodeCategories)
		{
			mNodeCategories = new std::vector<DataSpecification*>();
			mNodeCategories->reserve(other.mNodeCategories->size());
			for (auto* cat : *other.mNodeCategories)
			{
				mNodeCategories->push_back(cat ? new DataSpecification(*cat) : nullptr);
			}
		}
		else
		{
			mNodeCategories = nullptr;
		}

		// Deep copy vector of pointers: mFieldSpecs
		if (other.mFieldSpecs)
		{
			mFieldSpecs = new std::vector<DataSpecification*>();
			mFieldSpecs->reserve(other.mFieldSpecs->size());
			for (auto* field : *other.mFieldSpecs)
			{
				mFieldSpecs->push_back(field ? new DataSpecification(*field) : nullptr);
			}
		}
		else
		{
			mFieldSpecs = nullptr;
		}

		return *this;
	}
	DynamicTaskSpecs::~DynamicTaskSpecs()
	{
		if (mOutputType) delete mOutputType;
		if (mInputType) delete mInputType;
		if (mTab) delete mTab;
		if (mNodeCategories)
		{
			for (auto& category : *mNodeCategories)
			{
				if (category)
				{
					delete category;
				}
			}
		}
		if (mFieldSpecs)
		{
			for (auto& field : *mFieldSpecs)
			{
				if (field)
				{
					delete field;
				}
			}
		}
	}

	DynamicTask::DynamicTask(DynamicTaskSpecs& specs)
	{
		mSpecs = new DynamicTaskSpecs(specs);
	}
	DynamicTask::DynamicTask(const DynamicTask& other)
	{
		if (other.mSpecs)
			mSpecs = new DynamicTaskSpecs(*other.mSpecs);
		mTaskName = other.mTaskName;
	}
	DynamicTask::DynamicTask(DynamicTask&& other) noexcept
	{
		mSpecs = other.mSpecs;
		mTaskName = other.mTaskName;

		other.mSpecs = nullptr;
		other.mTaskName = "";
	}
	DynamicTask& DynamicTask::operator=(DynamicTask&& other) noexcept
	{
		if (this == &other) return *this;

		// Clean up current
		if (mSpecs)
			delete mSpecs;

		// Transfer ownership
		mSpecs = other.mSpecs;
		mTaskName = other.mTaskName;

		other.mSpecs = nullptr;
		other.mTaskName = "";

		return *this;
	}
	DynamicTask& DynamicTask::operator=(const DynamicTask& other)
	{
		// Prevent self-assignment
		if (this == &other) return *this;
		if (mSpecs)
			delete mSpecs;
		// Deep copy individual pointers (check for nullptr)
		mSpecs = other.mSpecs ? new DynamicTaskSpecs(*other.mSpecs) : nullptr;
		mTaskName = other.mTaskName;

		return *this;
	}
	DynamicTask::~DynamicTask()
	{
		if (mSpecs)
			delete mSpecs;
	}

	std::vector<TaskData> DynamicTask::GenerateFromSpecs()
	{
		std::vector<TaskData> output;
		for (const auto& DataSpecsPtr : *mSpecs->mFieldSpecs)
		{
			output.push_back(GenerateTaskData(*DataSpecsPtr));
		}
		return output;
	}

	void DynamicTask::AddPage(const std::vector<rttr::variant>& keys, const rttr::variant& value)
	{
		if (keys.size() >= mSpecs->MaxCategories())
		{
			return;
		}

		auto* PagePtr = &mPages;
		int count = 0;
		for (const auto& key : keys)
		{
			try
			{
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
				count++;
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to add Page as key not found", mTaskName);
				return;
			}
		}
		if (count == mSpecs->MaxDepth())
			(*PagePtr)[value] = GenerateFromSpecs(); // No tabs
		else if (!(*PagePtr).contains(value))
			(*PagePtr)[value] = ReflectionMap{};
	}

	void DynamicTask::AddTab(const std::vector<rttr::variant>& keys, const rttr::variant& value)
	{
		if (keys.size() != mSpecs->MaxCategories())
		{
			return;
		}

		auto* PagePtr = &mPages;
		for (const auto& key : keys)
		{
			try
			{
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to add Page as key not found", mTaskName);
				return;
			}
		}
		if (!(*PagePtr).contains(value))
			(*PagePtr)[value] = GenerateFromSpecs();
	}

	ReflectionMap& DynamicTask::GetTabs(std::vector<rttr::variant>& keys)
	{
		if (keys.size() != mSpecs->MaxCategories())
		{
			FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as keys size does not match max categories", mTaskName);
			throw std::out_of_range("Keys size does not match max categories");
		}
		auto* PagePtr = &mPages;
		for (const auto& key : keys)
		{
			try
			{
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}
		return *PagePtr;
	}

	rttr::variant& DynamicTask::GetTab(std::vector<rttr::variant>& keys)
	{
		if (keys.size() != mSpecs->MaxDepth())
		{
			FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as keys size does not match max categories", mTaskName);
			throw std::out_of_range("Keys size does not match max categories");
		}
		auto* PagePtr = &mPages;
		for (const auto& key : keys)
		{
			try
			{
				if (key == keys.back()) break;
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}

		return PagePtr->at(keys.back()); // Ensure the last key is a vector of TaskData
	}

	std::vector<TaskData>& DynamicTask::GetTab(std::vector<int>& indices)
	{
		if (indices.size() != mSpecs->MaxDepth())
		{
			FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as keys size does not match max categories", mTaskName);
			throw std::out_of_range("Keys size does not match max categories");
		}
		auto* PagePtr = &mPages;
		for (int i = 0; i < indices.size() - 1; i++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, indices[i]);
				auto key = it->first;
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}

		auto it = PagePtr->begin();
		std::advance(it, indices[indices.size() - 1]);
		return it->second.get_value<std::vector<TaskData>>();
	}

	void DynamicTask::RemoveTab(std::vector<int>& indices)
	{
		if (indices.size() != mSpecs->MaxDepth())
		{
			FRAMEEX_CORE_ERROR("Dynamic Task {} failed to remove Page as keys size does not match max categories", mTaskName);
			throw std::out_of_range("Keys size does not match max categories");
		}
		auto* PagePtr = &mPages;
		for (int i = 0; i < indices.size() - 1; i++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, indices[i]);
				auto key = it->first;
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to remove Page as key not found", mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}
		auto it = PagePtr->begin();
		std::advance(it, indices[indices.size() - 1]);
		PagePtr->erase(it);
	}

	DynamicTask GenerateCountingTask()
	{
		DynamicTask task;
		task.mTaskName = "Counting";

		std::vector<DynamicTask> entranceTasks;
		DynamicTask entranceTask;
		entranceTask.mTaskName = "Entrance";

		DynamicTask Description;
		Description.mTaskName = "Person Description";
		std::vector<DynamicTask> Descriptions;
		Descriptions.push_back(Description);

		DynamicTask frameSkipTask;
		frameSkipTask.mTaskName = "Frame Skips";

		entranceTasks.push_back(entranceTask);

		return task;
	}

	TaskData GenerateTaskData(DataSpecification& specs)
	{
		TaskData output;
		output.mFieldName = *specs.mName;
		switch (*specs.mType)
		{
		case DataType::Bool: output.mFieldData = *reinterpret_cast<bool*>(specs.mDefault);				return output;
		case DataType::Int: output.mFieldData = *reinterpret_cast<int*>(specs.mDefault);				return output;
		case DataType::Float: output.mFieldData = *reinterpret_cast<float*>(specs.mDefault);			return output;
		case DataType::Double: output.mFieldData = *reinterpret_cast<double*>(specs.mDefault);			return output;
		case DataType::String: output.mFieldData = *reinterpret_cast<std::string*>(specs.mDefault);		return output;
		case DataType::Time: output.mFieldData = *reinterpret_cast<Time*>(specs.mDefault);				return output;
		case DataType::Date: output.mFieldData = *reinterpret_cast<Date*>(specs.mDefault);				return output;
		default:																						return output;
		}
	}

	DynamicTask GenerateTask(std::filesystem::path path)
	{
		DynamicTask task;
		task.mSpecs = new DynamicTaskSpecs;
		YAML::Node config = YAML::LoadFile(path.string());

		if (config["Input"])
		{
			task.mSpecs->mInputType = new IOType(magic_enum::enum_cast<IOType>(config["Input"].as<std::string>()).value());
			task.mSpecs->mImportFormat = new rttr::variant(ExcelImport{});
			auto& fmt = task.mSpecs->mImportFormat->get_value<ExcelImport>();
			if (config["ImportList"]["Settings"])
			{
				fmt.mHasHeader = config["ExportList"]["Settings"]["HasHeader"].as<bool>(true);
			}
			if (config["ImportList"]["DataMapping"])
			{
				for (const auto& mapping : config["ImportList"]["DataMapping"])
				{
					fmt.mDataMapping[mapping.first.as<int>()] = mapping.second.as<std::string>();
				}
			}
		}

		if (config["Output"])
		{
			task.mSpecs->mOutputType = new IOType(magic_enum::enum_cast<IOType>(config["Output"].as<std::string>()).value());
		}

		if (*task.mSpecs->mOutputType == IOType::Excel)
		{
			task.mSpecs->mExportFormat = new rttr::variant(ExcelExport{});
			auto& fmt = task.mSpecs->mExportFormat->get_value<ExcelExport>();
			if (config["ExportList"]["ColumnHeaders"])
			{
				for (const auto& header : config["ExportList"]["ColumnHeaders"])
				{
					fmt.mColumnHeaders[header.first.as<int>()] = header.second.as<std::string>();
				}
			}
			if (config["ExportList"]["DataMapping"])
			{
				for (const auto& mapping : config["ExportList"]["DataMapping"])
				{
					fmt.mDataColumnMapping[mapping.first.as<int>()] = mapping.second.as<std::string>();
				}
			}
		}

		if (config["Tab"])
		{
			task.mSpecs->mTab = new DataSpecification();
			if (config["Tab"]["Type"])
			{
				task.mSpecs->mTab->mType = new DataType(magic_enum::enum_cast<DataType>(config["Tab"]["Type"].as<std::string>()).value());
			}
			if (config["Tab"]["Name"])
			{
				task.mSpecs->mTab->mName = new std::string(config["Tab"]["Name"].as<std::string>());
			}
		}

		if (config["NodeCategory"])
		{
			task.mSpecs->mNodeCategories = new std::vector<DataSpecification*>();
			for (const auto& category : config["NodeCategory"])
			{
				DataSpecification* dataspecs = new DataSpecification();
				if (category.second["Type"])
				{
					dataspecs->mType = new DataType(magic_enum::enum_cast<DataType>(category.second["Type"].as<std::string>()).value());
				}
				dataspecs->mName = new std::string(category.first.as<std::string>());
				task.mSpecs->mNodeCategories->push_back(dataspecs);
			}
		}

		if (config["TypeName"])
		{
			task.mTaskName = config["TypeName"].as<std::string>();
		}

		if (config["Fields"])
		{
			task.mSpecs->mFieldSpecs = new std::vector<DataSpecification*>();
			for (const auto& field : config["Fields"])
			{
				DataSpecification* dataspecs = new DataSpecification();
				dataspecs->mName = new std::string(field.first.as<std::string>());

				if (field.second["Type"])
				{
					dataspecs->mType = new DataType(magic_enum::enum_cast<DataType>(field.second["Type"].as<std::string>()).value());
				}

				if (field.second["Default"])
				{
					switch (*dataspecs->mType)
					{
					case DataType::Bool: dataspecs->mDefault = new bool(field.second["Default"].as<bool>()); break;
					case DataType::Int: dataspecs->mDefault = new int(field.second["Default"].as<int>()); break;
					case DataType::Float: dataspecs->mDefault = new float(field.second["Default"].as<float>()); break;
					case DataType::Double: dataspecs->mDefault = new double(field.second["Default"].as<double>()); break;
					case DataType::String: dataspecs->mDefault = new std::string(field.second["Default"].as<std::string>()); break;
					case DataType::Date: dataspecs->mDefault = new Date(field.second["Default"].as<Date>()); break;
					case DataType::Time: dataspecs->mDefault = new Time(field.second["Default"].as<Time>()); break;
					default: break;
					}
				}

				task.mSpecs->mFieldSpecs->push_back(dataspecs);
			}
		}

		task.GenerateFromSpecs();
		return task;
	}
	void DumpTask(DynamicTask tasks, int tab)
	{
		std::cout << "===========Specifications=========" << std::endl;
		if (tasks.mSpecs->mTab)
			std::cout << "Tab Type: " << magic_enum::enum_name(*tasks.mSpecs->mTab->mType) << std::endl;
		if (tasks.mSpecs->mOutputType)
			std::cout << "Output Type: " << magic_enum::enum_name(*tasks.mSpecs->mOutputType) << std::endl;
		if (tasks.mSpecs->mInputType)
			std::cout << "Input Type: " << magic_enum::enum_name(*tasks.mSpecs->mInputType) << std::endl;
		if (tasks.mSpecs->mNodeCategories)
		{
			std::cout << "Node Categories" << std::endl;
			for (auto& categories : *tasks.mSpecs->mNodeCategories)
			{
				std::cout << "\t" << *categories->mName << std::endl;
				std::cout << "\t\t" << "Type: " << magic_enum::enum_name(*categories->mType) << std::endl;
			}
		}
		if (tasks.mSpecs->mFieldSpecs)
		{
			std::cout << "Field Specifications" << std::endl;
			for (auto& categories : *tasks.mSpecs->mFieldSpecs)
			{
				if (categories->mType)
				{
					std::cout << "\t" << *categories->mName << std::endl;

					std::cout << "\t\t" << "Type: " << magic_enum::enum_name(*categories->mType) << std::endl;
					if (categories->mDefault)
					{
						std::cout << "\t\t" << "Default: ";

						switch (*categories->mType)
						{
						case DataType::Bool: std::cout << *reinterpret_cast<bool*>(categories->mDefault); break;
						case DataType::Int: std::cout << *reinterpret_cast<int*>(categories->mDefault); break;
						case DataType::Float: std::cout << *reinterpret_cast<float*>(categories->mDefault); break;
						case DataType::Double: std::cout << *reinterpret_cast<double*>(categories->mDefault); break;
						case DataType::String: std::cout << *reinterpret_cast<std::string*>(categories->mDefault); break;
						case DataType::Date: std::cout << *reinterpret_cast<std::string*>(categories->mDefault); break;
						case DataType::Time: std::cout << *reinterpret_cast<std::string*>(categories->mDefault); break;
						default: break;
						}

						std::cout << std::endl;
					}
				}
			}
		}

		std::cout << std::endl;

		std::cout << "===============Data==============" << std::endl;

		for (int i = 0; i < tab; i++) std::cout << "\t";
		std::cout << tasks.mTaskName << std::endl;
		/*for (auto& task : tasks.mFields)
		{
			for (int i = 0; i < tab + 1; i++) std::cout << "\t";
			std::cout << "Name: " << task.mFieldName << std::endl;
			for (int i = 0; i < tab + 1; i++) std::cout << "\t";
			std::cout << "Type: " << task.mFieldData.get_type().get_name() << std::endl;
			for (int i = 0; i < tab + 1; i++) std::cout << "\t";
			if (task.mFieldData.is_type<int>())
			{
				std::cout << "Data: " << task.mFieldData.get_value<int>() << std::endl << std::endl;
			}

			if (task.mFieldData.is_type<bool>())
			{
				std::cout << "Data: " << task.mFieldData.get_value<bool>() << std::endl << std::endl;
			}

			else if (task.mFieldData.is_type<double>())
			{
				std::cout << "Data: " << task.mFieldData.get_value<double>() << std::endl << std::endl;
			}

			else if (task.mFieldData.is_type<std::string>())
			{
				std::cout << "Data: " << task.mFieldData.get_value<std::string>() << std::endl << std::endl;
			}

			else if (task.mFieldData.is_type<float>())
			{
				std::cout << "Data: " << task.mFieldData.get_value<float>() << std::endl << std::endl;
			}
			else if (task.mFieldData.is_type<DynamicTask>())
			{
				std::cout << std::endl;
				DumpTask(task.mFieldData.get_value<DynamicTask>(), tab + 1);
			}
			else if (task.mFieldData.is_type<std::vector<DynamicTask>>())
			{
				auto vec = task.mFieldData.get_value<std::vector<DynamicTask>>();
				std::cout << std::endl;
				for (auto& element : vec)
				{
					DumpTask(element, tab + 1);
				}
			}
		}*/
	}

	std::vector<TaskData>& DynamicTaskInterface::GetData()
	{
		if (mIndices.size() != mTask->mSpecs->MaxDepth())
		{
			mIndices.resize(mTask->mSpecs->MaxDepth(), 0);
		}
		auto* PagePtr = &mTask->mPages;
		for (int i = 0; i < mIndices.size() - 1; i++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, mIndices[i]);
				auto key = it->first;
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}

		auto it = PagePtr->begin();
		std::advance(it, mIndices[mIndices.size() - 1]);
		return it->second.get_value<std::vector<TaskData>>();
	}
	std::pair<std::vector<rttr::variant>, std::vector<TaskData>> DynamicTaskInterface::DeleteCurrentLeaf()
	{
		if (mIndices.size() != mTask->mSpecs->MaxDepth())
		{
			mIndices.resize(mTask->mSpecs->MaxDepth(), 0);
		}
		auto* PagePtr = &mTask->mPages;
		std::vector<rttr::variant> keyPath;
		for (int i = 0; i < mIndices.size() - 1; i++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, mIndices[i]);
				auto key = it->first;
				keyPath.push_back(key);
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}

		auto it = PagePtr->begin();
		std::advance(it, mIndices[mIndices.size() - 1]);
		auto retData = it->second.get_value<std::vector<TaskData>>();
		auto keyPathRet = keyPath;
		keyPathRet.push_back(it->first); // Add the key of the leaf being deleted to the keyPathRet
		PagePtr->erase(it);
		if (mIndices[mIndices.size() - 1] != 0) mIndices[mIndices.size() - 1]--;

		BackCheckPath(keyPath);
		return { keyPathRet, retData };
	}

	std::pair<std::vector<rttr::variant>, std::vector<TaskData>> DynamicTaskInterface::DeleteLeaf(int idx)
	{
		if (mIndices.size() != mTask->mSpecs->MaxDepth())
		{
			mIndices.resize(mTask->mSpecs->MaxDepth(), 0);
		}
		auto* PagePtr = &mTask->mPages;
		std::vector<rttr::variant> keyPath;
		for (int i = 0; i < mIndices.size() - 1; i++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, mIndices[i]);
				auto key = it->first;
				keyPath.push_back(key);
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}

		auto it = PagePtr->begin();
		std::advance(it, idx);
		auto keyPathRet = keyPath;
		auto retData = it->second.get_value<std::vector<TaskData>>();
		keyPathRet.push_back(it->first); // Add the key of the leaf being deleted to the keyPathRet
		PagePtr->erase(it);
		if (mIndices[mIndices.size() - 1] != 0 && mIndices[mIndices.size() - 1] >= idx) mIndices[mIndices.size() - 1]--;

		BackCheckPath(keyPath);
		return { keyPathRet, retData };
	}

	// 3, 3, 2 ,4 , 1
	std::pair<std::vector<rttr::variant>, rttr::variant> DynamicTaskInterface::DeleteLayer(int layer)
	{
		if (layer < 0 || layer >= mIndices.size())
		{
			FRAMEEX_CORE_ERROR("Dynamic Task {} failed to delete layer as layer index is out of bounds", mTask->mTaskName);
			return {};
		}

		auto* PagePtr = &mTask->mPages;
		std::vector<rttr::variant> keyPath;
		for (int i = 0; i < layer; i++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, mIndices[i]);
				auto key = it->first;
				keyPath.push_back(key);
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}

		auto it = PagePtr->begin();
		std::advance(it, mIndices[layer]);
		auto keyPathRet = keyPath;
		auto retData = it->second;
		keyPathRet.push_back(it->first); // Add the key of the leaf being deleted to the keyPathRet
		PagePtr->erase(it);
		if (mIndices[layer] != 0) mIndices[layer]--;

		BackCheckPath(keyPath);

		return { keyPathRet, retData };
	}
	void DynamicTaskInterface::AddLayer(std::vector<rttr::variant> keys, const rttr::variant& value)
	{
		if (keys.size() >= mTask->mSpecs->MaxDepth())
		{
			FRAMEEX_CORE_ERROR("Dynamic Task {} failed to add layer as keys size does not match max depth", mTask->mTaskName);
			return;
		}
		auto* PagePtr = &mTask->mPages;
		for (int key = 0; key < keys.size() - 1; key++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, mIndices[key]);
				auto key = it->first;
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to add layer as key not found", mTask->mTaskName);
				return;
			}
		}
		if (!(*PagePtr).contains(keys.back()))
			(*PagePtr)[keys.back()] = value;
	}
	void DynamicTaskInterface::AddLeaf(rttr::variant key)
	{
		if (mIndices.size() != mTask->mSpecs->MaxDepth())
		{
			mIndices.resize(mTask->mSpecs->MaxDepth(), 0);
		}
		auto* PagePtr = &mTask->mPages;
		for (int i = 0; i < mIndices.size() - 1; i++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, mIndices[i]);
				auto key = it->first;
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}
		PagePtr->emplace(key, mTask->GenerateFromSpecs());
	}

	// layer is implied, going from backwards
	// e.g. if my vector<int> is {3, 3, 2, 4, 1}, size: 5
	// my vector<rttr::variant> is {"key1", "key2"} -> "key2" is added to index 4, "key 1" is added to index 3
	// Starting index = size(int) - size(key)
	void DynamicTaskInterface::AddLeaf(std::vector<rttr::variant>& keys)
	{
		int startingIndex = static_cast<int>(mIndices.size() - keys.size());
		if (startingIndex < 0 || startingIndex >= mIndices.size())
		{
			FRAMEEX_CORE_ERROR("Dynamic Task {} failed to add leaf as starting index is out of bounds", mTask->mTaskName);
			return;
		}
		auto* PagePtr = &mTask->mPages;
		for (int i = 0; i < startingIndex; i++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, mIndices[i]);
				auto key = it->first;
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}

		for (int i = startingIndex; i < mIndices.size() - 1; i++)
		{
			try
			{
				PagePtr->emplace(keys[i - startingIndex], ReflectionMap());
				PagePtr = &PagePtr->at(keys[i - startingIndex]).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}

		PagePtr->emplace(keys.back(), mTask->GenerateFromSpecs());
	}

	ReflectionMap& DynamicTaskInterface::GetLayer(int layer)
	{
		auto* PagePtr = &mTask->mPages;
		for (int i = 0; i < layer - 1; i++)
		{
			try
			{
				auto it = PagePtr->begin();
				std::advance(it, mIndices[i]);
				auto key = it->first;
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(key).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}
		auto IT = PagePtr->begin();
		std::advance(IT, mIndices[layer - 1]);
		return IT->second.get_value<ReflectionMap>();
	}

	void DynamicTaskInterface::BackCheckPath(std::vector<rttr::variant>& vars)
	{
		auto* PagePtr = &mTask->mPages;
		for (int i = 0; i < vars.size(); i++)
		{
			try
			{
				// Use the key to access the next level of the map
				PagePtr = &PagePtr->at(vars[i]).get_value<ReflectionMap>();
			}
			catch (const std::out_of_range&)
			{
				FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
				throw std::out_of_range("Key not found in Page");
			}
		}
		if (PagePtr->empty())
		{
			PagePtr = &mTask->mPages;
			for (int i = 0; i < vars.size() - 1; i++)
			{
				try
				{
					// Use the key to access the next level of the map
					PagePtr = &PagePtr->at(vars[i]).get_value<ReflectionMap>();
				}
				catch (const std::out_of_range&)
				{
					FRAMEEX_CORE_ERROR("Dynamic Task {} failed to get Page as key not found", mTask->mTaskName);
					throw std::out_of_range("Key not found in Page");
				}
			}
			PagePtr->erase(vars.back());
			if (mIndices[vars.size() - 1] != 0) mIndices[vars.size() - 1]--;
		}
		if (!vars.empty())
			vars.pop_back();

		while (!vars.empty())
		{
			BackCheckPath(vars);
		}
	}
}
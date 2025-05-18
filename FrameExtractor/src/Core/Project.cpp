/******************************************************************************
/*!
\file       Project.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 16, 2024
\brief      Defines the Project class that contains all data and settings
			related to the project.

 /******************************************************************************/
#include <FrameExtractorPCH.hpp>
#include <Core/Project.hpp>
#include <Core/LoggerManager.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace YAML
{
    template<>
    struct convert<FrameExtractor::PersonDesc>
    {
        static bool decode(const Node& node, FrameExtractor::PersonDesc& person)
        {
            if (!node.IsMap()) return false;
            person.Description = node["Description"].as<std::string>();
            person.IsMale = node["IsMale"].as<bool>();
            person.timeStamp = node["Timestamp"].as<std::string>();
            return true;
        }

        static Node encode(const FrameExtractor::PersonDesc& person)
        {
			Node node;
			node["Description"] = person.Description;
			node["IsMale"] = person.IsMale;
			node["Timestamp"] = person.timeStamp;
			return node;
		}
    };

    template<>
    struct convert<FrameExtractor::CountData>
    {
        static bool decode(const Node& node, FrameExtractor::CountData& data)
        {
            data.mCustomer = node["Customer"].as<int>(0);
            data.mReCustomer = node["ReCustomer"].as<int>(0);
            data.mSuspectedStaff = node["SuspectedStaff"].as<int>(0);
            data.mReSuspectedStaff = node["ReSuspectedStaff"].as<int>(0);
            data.mChildren = node["Children"].as<int>(0);
            data.mReChildren = node["ReChildren"].as<int>(0);
            data.mOthers = node["Others"].as<int>(0);
            data.mReOthers = node["ReOthers"].as<int>(0);

            const Node& entranceData = node["EntranceData"];
            if (entranceData)
            {
                for (const auto& entrancePair : entranceData)
                {
                    int entranceIndex = entrancePair.first.as<int>();
                    const auto& typeMap = entrancePair.second;

                    if (entranceIndex >= data.Entrance.size())
                        data.Entrance.resize(entranceIndex + 1);

                    auto& entranceTypes = data.Entrance[entranceIndex];

                    for (const auto& typePair : typeMap)
                    {
                        int typeIndex = typePair.first.as<int>();
                        for (const auto& personNode : typePair.second)
                        {
                            entranceTypes[typeIndex].push_back(personNode.as<FrameExtractor::PersonDesc>());
                        }
                    }
                }
            }

            return true;
        }

        static Node encode(const FrameExtractor::CountData& data)
        {
            Node node;
            node["Customer"] = data.mCustomer;
            node["ReCustomer"] = data.mReCustomer;
            node["SuspectedStaff"] = data.mSuspectedStaff;
            node["ReSuspectedStaff"] = data.mReSuspectedStaff;
            node["Children"] = data.mChildren;
            node["ReChildren"] = data.mReChildren;
            node["Others"] = data.mOthers;
            node["ReOthers"] = data.mReOthers;
            Node entranceDataNode;
            for (size_t entranceIndex = 0; entranceIndex < data.Entrance.size(); ++entranceIndex)
            {
                const auto& entranceTypes = data.Entrance[entranceIndex];
                Node entranceNode;
                for (size_t typeIndex = 0; typeIndex < entranceTypes.size(); ++typeIndex)
                {
                    Node typeNode;
                    for (const auto& person : entranceTypes[typeIndex])
                    {
                        typeNode.push_back(person);
                    }
                    entranceNode[typeIndex] = typeNode;
                }
                entranceDataNode[entranceIndex] = entranceNode;
            }
            node["EntranceData"] = entranceDataNode;
            return node;
        }
    };

    //struct AggregateData
    //{
    //    // Pre-existing Data
    //    std::string ShopperTrack_ID = "DIOR224";
    //    int8_t Enters = 3;
    //    int8_t Exit = 0;

    //    // New Data
    //    int32_t mCustomer = 0;
    //    std::vector<AggregateEntrance> Entrance;
    //    // Entrance Num, Data(time to time)

    //};

    template<>
    struct convert<FrameExtractor::AggregateEntrance>
    {
        static bool decode(const Node& node, FrameExtractor::AggregateEntrance& data)
        {
            if (!node.IsMap()) return false;

            // Decode mCorruptedVideos (as a sequence of strings)
            if (node["CorruptedVideos"])
            {
                data.mCorruptedVideos = node["CorruptedVideos"].as<std::vector<std::string>>();
            }

            // Decode mBlankedVideos (as a sequence of strings)
            if (node["BlankedVideos"])
            {
                data.mBlankedVideos = node["BlankedVideos"].as<std::vector<std::string>>();
            }

            // Decode mFrameSkips (as a sequence of pairs)
            if (node["FrameSkips"])
            {
                for (const auto& frameSkipNode : node["FrameSkips"])
                {
                    // Each item is a pair with two strings
                    data.mFrameSkips.push_back(frameSkipNode.as<std::pair<std::string, std::string>>());
                }
            }

			return true;
        }

        static Node encode(const FrameExtractor::AggregateEntrance& data)
        {
            Node node;

            // Encode mCorruptedVideos as a sequence of strings
            node["CorruptedVideos"] = data.mCorruptedVideos;

            // Encode mBlankedVideos as a sequence of strings
            node["BlankedVideos"] = data.mBlankedVideos;

            // Encode mFrameSkips as a sequence of pairs (time -> time)
            Node frameSkipsNode;
            for (const auto& pair : data.mFrameSkips)
            {
                Node pairNode;
                pairNode.push_back(pair.first);  // first string (time)
                pairNode.push_back(pair.second); // second string (time)
                frameSkipsNode.push_back(pairNode);
            }
            node["FrameSkips"] = frameSkipsNode;

			return node;
        }
    };

    template<>
    struct convert<FrameExtractor::AggregateData>
    {
        static bool decode(const Node& node, FrameExtractor::AggregateData& data)
        {
            // Ensure the node is a map
            if (!node.IsMap()) return false;

            // Decode pre-existing data
            if (node["ShopperTrack_ID"])
            {
                data.ShopperTrack_ID = node["ShopperTrack_ID"].as<std::string>();
            }
            if (node["Enters"])
            {
                data.Enters = node["Enters"].as<int8_t>();
            }
            if (node["Exit"])
            {
                data.Exit = node["Exit"].as<int8_t>();
            }

            // Decode new data (mCustomer)
            if (node["Customer"])
            {
                data.mCustomer = node["Customer"].as<int32_t>();
            }

            // Decode Entrance data (which is a vector of AggregateEntrance objects)
            if (node["Entrance"])
            {
                data.Entrance = node["Entrance"].as<std::vector<FrameExtractor::AggregateEntrance>>();
            }

            return true;
        }

        static Node encode(const FrameExtractor::AggregateData& data)
        {
            Node node;

            // Encode pre-existing data
            node["ShopperTrack_ID"] = data.ShopperTrack_ID;
            node["Enters"] = data.Enters;
            node["Exit"] = data.Exit;

            // Encode new data (mCustomer)
            node["Customer"] = data.mCustomer;

            // Encode Entrance data (which is a vector of AggregateEntrance objects)
            node["Entrance"] = data.Entrance;

            return node;
        }
    };

}



namespace FrameExtractor
{
    std::string EntryTypeToString(EntryType type)
    {
        switch (type)
        {
        case Customer:
            return "Customer";
        case ReCustomer:
            return "Re-entry Customer";
        case SuspectedStaff:
            return "Suspected Staff";
        case ReSuspectedStaff:
            return "Re-entry Suspected Staff";
        case Children:
            return "Children";
        case ReChildren:
            return "Re-entry Children";
        case Others:
            return "Others";
        case ReOthers:
            return "Re-entry Others";
        default:
            return "Unknown type";
        }
    }


    Project::Project()
    {
    }

    Project::~Project()
    {
    }

    void Project::CreateProject(std::string name, std::filesystem::path dir)
    {
		mName = name;
        mProjectDir = dir;
        mProjectDir /= name;
        mAssetDir /= mProjectDir / "Assets";
		std::filesystem::create_directory(mProjectDir);
        std::filesystem::create_directory(mAssetDir);
        mProjectFilePath = mProjectDir / (name + ".FrEX");

        YAML::Node node;

        node["Project Name"] = mName;
        node["Project Directory"] = mProjectDir.string();
        node["Asset Directory"] = mAssetDir.string();

        std::ofstream file(mProjectFilePath);
        if (file.is_open())
        {
			file << node;
			file.close();
		}
        else
        {
			FRAMEEX_CORE_ERROR("Failed to create project file: {}", mProjectFilePath.string());
		}
    }

    void Project::LoadProject(std::filesystem::path path)
    {
        std::ifstream ifs(path);
        if (!ifs.is_open())
        {
			FRAMEEX_CORE_ERROR("Failed to open project file: {}", path.string());
			return;
		}
        YAML::Node node = YAML::Load(ifs);
		mName = node["Project Name"].as<std::string>();
		mProjectDir = node["Project Directory"].as<std::string>();
		mAssetDir = node["Asset Directory"].as<std::string>();
        
        
		ifs.close();
    }

    void Project::SaveProject()
    {
        YAML::Emitter emitter;
        emitter << YAML::Key << "Project Name" << YAML::Value << mName;
        emitter << YAML::Key << "Project Directory" << YAML::Value << mProjectDir.string();
        emitter << YAML::Key << "Asset Directory" << YAML::Value << mAssetDir.string();

        emitter << YAML::BeginMap << YAML::Key << "Counting Data";
        //std::map<StoreCode, std::map<Hour, CountData>> mCountingData;
        for (const auto& [storeCode, hourData] : mCountingData)
        {
            emitter << YAML::Key << storeCode;
			emitter << YAML::BeginMap;
            for (const auto& [hour, data] : hourData)
            {
                emitter << YAML::Key << hour;
                emitter << YAML::Value << YAML::convert<CountData>::encode(data);
			}
			emitter << YAML::EndMap;
		}

        emitter << YAML::BeginMap << YAML::Key << "Aggregate Data";

        for (const auto& [date, storeData] : mAggregateStoreData)
        {
			emitter << YAML::Key << date;
			emitter << YAML::BeginMap;
            for (const auto& [storeCode, hourData] : storeData)
            {
				emitter << YAML::Key << storeCode;
				emitter << YAML::BeginMap;
                for (const auto& [hour, data] : hourData)
                {
					emitter << YAML::Key << hour;
					emitter << YAML::Value << YAML::convert<AggregateData>::encode(data);
                }
            }
        }
    }

}


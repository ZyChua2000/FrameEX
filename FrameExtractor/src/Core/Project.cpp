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
                    int entranceIndex = entrancePair.first.as<int>(); // Get entrance index
                    const auto& typeMap = entrancePair.second;

                    // Resize Entrance vector if necessary
                    if (entranceIndex >= data.Entrance.size())
                        data.Entrance.resize(entranceIndex + 1);

                    auto& entranceTypes = data.Entrance[entranceIndex];

                    for (const auto& typePair : typeMap["DescDetails"])
                    {
                        int typeIndex = typePair.first.as<int>(); // Get type index
                        const Node& typeNode = typePair.second;

                        // Decode mDesc (person descriptions)
                        if (typeNode.IsSequence()) // Ensure that the node is a sequence (array) of person nodes
                        {
                            for (const auto& personNode : typeNode)
                            {
                                entranceTypes.mDesc[typeIndex].push_back(personNode.as<FrameExtractor::PersonDesc>());
                            }
                        }
                       
                    }
                    entranceTypes.mCorruptedVideos = typeMap["FrameDetails"]["CorruptedVideos"].as<std::vector<std::string>>();
                    entranceTypes.mBlankedVideos = typeMap["FrameDetails"]["BlankedVideos"].as<std::vector<std::pair<bool,std::string>>>();
                    const Node& frameSkipsNode = typeMap["FrameDetails"]["FrameSkips"];
                    if (frameSkipsNode.IsSequence()) // Ensure it's a sequence of pairs
                    {
                        for (const auto& frameSkip : frameSkipsNode)
                        {
                            if (frameSkip.IsSequence() && frameSkip.size() == 2)
                            {
                                std::pair<std::string, std::string> frameSkipPair;
                                frameSkipPair.first = frameSkip[0].as<std::string>();
                                frameSkipPair.second = frameSkip[1].as<std::string>();
                                entranceTypes.mFrameSkips.push_back(frameSkipPair);
                            }
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
                for (size_t typeIndex = 0; typeIndex < entranceTypes.mDesc.size(); ++typeIndex)
                {
                    Node typeNode;
                    for (const auto& person : entranceTypes.mDesc[typeIndex])
                    {
                        typeNode.push_back(person);
                    }
                    entranceNode["DescDetails"][std::to_string(typeIndex)] = typeNode;
                }

                Node frameSkipsNode;
                for (const auto& frameSkip : entranceTypes.mFrameSkips)
                {
                    Node pairNode;
                    pairNode.push_back(frameSkip.first);
                    pairNode.push_back(frameSkip.second);
                    frameSkipsNode.push_back(pairNode);
                }
                Node blankNode;
                for (const auto& blankedVideos : entranceTypes.mBlankedVideos)
                {
                    Node pairNode;
                    pairNode.push_back(blankedVideos.first);
                    pairNode.push_back(blankedVideos.second);
                    blankNode.push_back(pairNode);
                }
                entranceNode["FrameDetails"]["CorruptedVideos"] = entranceTypes.mCorruptedVideos;
                entranceNode["FrameDetails"]["BlankedVideos"] = blankNode;
                entranceNode["FrameDetails"]["FrameSkips"] = frameSkipsNode;

                entranceDataNode[std::to_string(entranceIndex)] = entranceNode;
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
            if (node["StoreID"])
            {
                data.StoreID = node["StoreID"].as<std::string>();
            }
            if (node["Enters"])
            {
                data.Enters = node["Enters"].as<int32_t>();
            }
            if (node["Exit"])
            {
                data.Exit = node["Exit"].as<int32_t>();
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
            node["StoreID"] = data.StoreID;
            node["Enters"] = (int32_t)data.Enters;
            node["Exit"] = (int32_t)data.Exit;

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
        mCountingData.clear();
        mAggregateStoreData.clear();
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
        mProjectFilePath = path;
        
        std::map<StoreCode, std::map<Hour, CountData>> tmpCountingData;
        std::map<Date, std::map<StoreCode, std::map<Hour, AggregateData>>> tmpAggregateStoreData;

        if (node["Counting Data"])
        {
            for (const auto& storeNode : node["Counting Data"])
            {
                StoreCode storeCode = storeNode.first.as<StoreCode>(); // Assuming StoreCode is a type that can be converted from YAML.
                for (const auto& hourNode : storeNode.second)
                {
                    Hour hour = hourNode.first.as<Hour>(); // Assuming Hour is a type that can be converted from YAML.
                    CountData countData;
                    YAML::convert<CountData>::decode(node["Counting Data"][storeCode][hour], countData); // Decode CountData from YAML.
                    tmpCountingData[storeCode][hour] = countData;
                }
            }
        }

        // Load Aggregate Data
        if (node["Aggregate Data"])
        {
            for (const auto& dateNode : node["Aggregate Data"])
            {
                Date date = dateNode.first.as<Date>(); // Date key
                const YAML::Node& storesNode = dateNode.second;

                for (const auto& storeNode : storesNode)
                {
                    StoreCode storeCode = storeNode.first.as<StoreCode>(); // StoreCode key
                    const YAML::Node& hoursNode = storeNode.second;

                    for (const auto& hourNode : hoursNode)
                    {
                        Hour hour = hourNode.first.as<Hour>(); // Hour key
                        const YAML::Node& dataNode = hourNode.second;

                        AggregateData aggregateData;
                        YAML::convert<AggregateData>::decode(dataNode, aggregateData); // Correctly decode from already-accessed node
                        tmpAggregateStoreData[date][storeCode][hour] = aggregateData;
                    }
                }
            }
        }

        mCountingData = tmpCountingData;
        mAggregateStoreData = tmpAggregateStoreData;
		ifs.close();
    }

    void Project::SaveProject()
    {
        YAML::Emitter emitter;

        emitter << YAML::BeginMap;

        emitter << YAML::Key << "Project Name" << YAML::Value << mName;
        emitter << YAML::Key << "Project Directory" << YAML::Value << mProjectDir.string();
        emitter << YAML::Key << "Asset Directory" << YAML::Value << mAssetDir.string();

        emitter << YAML::Key << "Counting Data" << YAML::Value << YAML::BeginMap;
        //std::map<StoreCode, std::map<Hour, CountData>> mCountingData;
        for (const auto& [storeCode, hourData] : mCountingData)
        {
            emitter << YAML::Key << storeCode << YAML::Value << YAML::BeginMap;
            for (const auto& [hour, data] : hourData)
            {
                emitter << YAML::Key << hour;
                emitter << YAML::Value << YAML::convert<CountData>::encode(data);
			}
			emitter << YAML::EndMap;
		}

        emitter << YAML::EndMap;

        emitter << YAML::Key << "Aggregate Data" << YAML::Value << YAML::BeginMap;

        for (const auto& [date, storeData] : mAggregateStoreData)
        {
            emitter << YAML::Key << date << YAML::Value << YAML::BeginMap;

            for (const auto& [storeCode, hourData] : storeData)
            {
                emitter << YAML::Key << storeCode << YAML::Value << YAML::BeginMap;

                for (const auto& [hour, data] : hourData)
                {
					emitter << YAML::Key << hour;
					emitter << YAML::Value << YAML::convert<AggregateData>::encode(data);
                }
                emitter << YAML::EndMap;
            }
            emitter << YAML::EndMap;
        }

        emitter << YAML::EndMap;
        emitter << YAML::EndMap;

        std::ofstream file(mProjectFilePath);
        file << emitter.c_str();
		file.close();



    }

}


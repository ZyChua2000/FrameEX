/******************************************************************************
/*!
\file       YAMLSerialiser.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 15, 2024
\brief      Defines the YAML Serialiser class which serialises Data

 /******************************************************************************/

#include "FrameExtractorPCH.hpp"
#include "Core/YAMLSerialiser.hpp"
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
    };
}


namespace FrameExtractor
{
	YAMLSerialiser::YAMLSerialiser(std::filesystem::path filepath) : mPath(filepath)
	{
	}

	YAMLSerialiser::~YAMLSerialiser()
	{
	}
    void YAMLSerialiser::ExportProject(std::map<std::string, std::map<int32_t, CountData>>& countedData)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        for (const auto& [cameraName, hourlyData] : countedData)
        {
            out << YAML::Key << cameraName << YAML::Value << YAML::BeginMap;

            for (const auto& [hour, countData] : hourlyData)
            {
                out << YAML::Key << hour << YAML::Value << YAML::BeginMap;

                out << YAML::Key << "Customer" << YAML::Value << countData.mCustomer;
                out << YAML::Key << "ReCustomer" << YAML::Value << countData.mReCustomer;
                out << YAML::Key << "SuspectedStaff" << YAML::Value << countData.mSuspectedStaff;
                out << YAML::Key << "ReSuspectedStaff" << YAML::Value << countData.mReSuspectedStaff;
                out << YAML::Key << "Children" << YAML::Value << countData.mChildren;
                out << YAML::Key << "ReChildren" << YAML::Value << countData.mReChildren;
                out << YAML::Key << "Others" << YAML::Value << countData.mOthers;
                out << YAML::Key << "ReOthers" << YAML::Value << countData.mReOthers;

                // Entrance data
                out << YAML::Key << "EntranceData" << YAML::Value << YAML::BeginMap;
                for (size_t entranceNum = 0; entranceNum < countData.Entrance.size(); ++entranceNum)
                {
                    const auto& entryVec = countData.Entrance[entranceNum];
                    out << YAML::Key << entranceNum << YAML::Value << YAML::BeginMap;

                    for (size_t entryType = 0; entryType < entryVec.size(); ++entryType)
                    {
                        out << YAML::Key << entryType << YAML::Value << YAML::BeginSeq;
                        for (const auto& person : entryVec[entryType])
                        {
                            out << YAML::BeginMap;
                            out << YAML::Key << "Description" << YAML::Value << person.Description;
                            out << YAML::Key << "IsMale" << YAML::Value << person.IsMale;
                            out << YAML::Key << "Timestamp" << YAML::Value << person.timeStamp;
                            out << YAML::EndMap;
                        }
                        out << YAML::EndSeq;
                    }

                    out << YAML::EndMap;
                }
                out << YAML::EndMap; // End of EntranceData

                out << YAML::EndMap; // End of hour data
            }

            out << YAML::EndMap; // End of cameraName
        }

        out << YAML::EndMap;

        std::ofstream fout(mPath.string());
        fout << out.c_str();
        fout.close();
    }
    void YAMLSerialiser::ImportProject(std::map<std::string, std::map<int32_t, CountData>>& countedData)
    {
        YAML::Node root = YAML::LoadFile(mPath.string());

        for (const auto& cameraNode : root)
        {
            const std::string& cameraName = cameraNode.first.as<std::string>();
            const YAML::Node& cameraData = cameraNode.second;

            for (const auto& hourNode : cameraData)
            {
                int hour = hourNode.first.as<int>();
                const YAML::Node& countDataNode = hourNode.second;

                CountData data = countDataNode.as<CountData>();
                countedData[cameraName][hour] = data;
            }
        }
    }


}
/*
 * Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include "Conversion.hpp"
#include "soss/utilities.hpp"
#include <iostream>

namespace soss {
namespace fiware {

using Json = nlohmann::json;
using nlohmann::detail::value_t;

Json Conversion::soss_to_fiware(const soss::Message& input)
{
    Json output;

    //output["type"] = input.type;

    for (auto attr: input.data)
    {
        Json attr_json;

        if (attr.second.type() == typeid(bool).name())
        {
            attr_json["value"] = Json(*attr.second.cast<bool>());
        }
        else if (attr.second.type() == typeid(int8_t).name())
        {
            attr_json["value"] = Json(*attr.second.cast<int8_t>());
        }
        else if (attr.second.type() == typeid(int16_t).name())
        {
            attr_json["value"] = Json(*attr.second.cast<int16_t>());
        }
        else if (attr.second.type() == typeid(int32_t).name())
        {
            attr_json["value"] = Json(*attr.second.cast<int32_t>());
        }
        else if (attr.second.type() == typeid(int64_t).name())
        {
            attr_json["value"] = Json(*attr.second.cast<int64_t>());
        }
        else if (attr.second.type() == typeid(uint8_t).name())
        {
            attr_json["value"] = Json(*attr.second.cast<uint8_t>());
        }
        else if (attr.second.type() == typeid(uint16_t).name())
        {
            attr_json["value"] = Json(*attr.second.cast<uint16_t>());
        }
        else if (attr.second.type() == typeid(uint32_t).name())
        {
            attr_json["value"] = Json(*attr.second.cast<uint32_t>());
        }
        else if (attr.second.type() == typeid(uint64_t).name())
        {
            attr_json["value"] = Json(*attr.second.cast<uint64_t>());
        }
        else if (attr.second.type() == typeid(float).name())
        {
            attr_json["value"] = Json(*attr.second.cast<float>());
        }
        else if (attr.second.type() == typeid(long).name())
        {
            attr_json["value"] = Json(*attr.second.cast<long>());
        }
        else if (attr.second.type() == typeid(double).name())
        {
            attr_json["value"] = Json(*attr.second.cast<double>());
        }
        else if (attr.second.type() == typeid(std::string).name())
        {
            attr_json["value"] = Json(*attr.second.cast<std::string>());
        }
        else
        {
            std::cerr << "[soss-fiware][conversion]: Error converting from soss message - " <<
                         "Type " << attr.second.type() << " not recognised." << std::endl;
            return Json{};
        }

        try
        {
            attr_json["type"] = m_fiware_types.at(attr.second.type());
        }
        catch (const std::exception& e)
        {
            std::cerr << "[soss-fiware][conversion]: Error converting from soss message - " <<
                         "Type " << attr.second.type() << " threw the following exception:" <<
                         e.what() << std::endl;
            return Json{};
        }

        std::string name = attr.first;
        output[name] = attr_json;
    }

    return output;
}

soss::Message Conversion::fiware_to_soss(const std::string& type, const Json& input)
{
    soss::Message output;

    output.type = type;

    for(Json::const_iterator it = input.begin(); it != input.end(); ++it)
    {
        soss::Field field;

        try
        {
            if (it.value()["type"] == m_fiware_types.at(typeid(bool).name()))
            {
                field.set<bool>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(int8_t).name()))
            {
                field.set<int8_t>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(int16_t).name()))
            {
                field.set<int16_t>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(int32_t).name()))
            {
                field.set<int32_t>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(int64_t).name()))
            {
                field.set<int64_t>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(uint8_t).name()))
            {
                field.set<uint8_t>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(uint16_t).name()))
            {
                field.set<uint16_t>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(uint32_t).name()))
            {
                field.set<uint32_t>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(uint64_t).name()))
            {
                field.set<uint64_t>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(float).name()))
            {
                field.set<float>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(long).name()))
            {
                field.set<long>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(double).name()))
            {
                field.set<double>(it.value()["value"]);
            }
            else if (it.value()["type"] == m_fiware_types.at(typeid(std::string).name()))
            {
                field.set<std::string>(it.value()["value"]);
            }
            else
            {
                std::cerr << "[soss-fiware][conversion]: Error converting from JSON - " <<
                             "Type " << it.value()["type"] << " not recognised." << std::endl;
                std::cout << "TypeID float: " << m_fiware_types.at(typeid(float).name()) << std::endl;
                return soss::Message{};
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "[soss-fiware][conversion]: Error converting from JSON - " <<
                         "Type " << it.value()["type"] << " threw the following exception:" <<
                         e.what() << std::endl;
            return soss::Message{};
        }

        output.data[it.key()] = field;
    }

    return output;
}

} // namespace fiware
} // namespace soss


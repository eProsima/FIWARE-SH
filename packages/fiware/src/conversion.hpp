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

#ifndef SOSS__FIWARE__INTERNAL__CONVERSION_HPP
#define SOSS__FIWARE__INTERNAL__CONVERSION_HPP

#include "json/json.hpp"

#include <soss/Message.hpp>

#include <iostream>

namespace soss {
namespace fiware {

using Json = nlohmann::json;

namespace conversion {


inline bool fiware_type_to_soss_type(
        const Json& fiware_type,
        xtypes::StructType& soss_type)
{
    static const std::map<std::string, xtypes::DynamicType::Ptr> types_mapping =
    {
        { "Boolean", xtypes::primitive_type<bool>() },
        { "int8", xtypes::primitive_type<int8_t>() },
        { "uint8", xtypes::primitive_type<uint8_t>() },
        { "int16", xtypes::primitive_type<int16_t>() },
        { "uint16", xtypes::primitive_type<uint16_t>() },
        { "int32", xtypes::primitive_type<int32_t>() },
        { "uint32", xtypes::primitive_type<uint32_t>() },
        { "int64", xtypes::primitive_type<int64_t>() },
        { "uint64", xtypes::primitive_type<uint64_t>() },
        { "Float", xtypes::primitive_type<float>() },
        { "Double", xtypes::primitive_type<double>() },
        { "Long", xtypes::primitive_type<long>() },
        { "String",  xtypes::StringType() },
        { "Text",  xtypes::StringType() },
    };

    for(const auto& it: fiware_type.items())
    {
        const std::string& member_name = it.key();
        std::string type_name = it.value()["types"][0].get<std::string>();

        const auto& mapping = types_mapping.find(type_name);
        if(mapping == types_mapping.end())
        {
            std::cerr << "[soss-fiware][conversion]: No mapping for fiware type '"
                << type_name << "'" << std::endl;
            return false;
        }
        soss_type.add_member(member_name, *mapping->second);
    }

    return true;
}


template<typename T>
void value_to_soss(
        const Json& fiware_node,
        xtypes::DynamicData::WritableNode& soss_node)
{
    const std::string& member_name = soss_node.from_member()->name();
    soss_node.data().value(fiware_node[member_name]["value"].get<T>());
}

inline bool value_to_soss(
        const xtypes::StructType& struct_type,
        const Json& fiware_node,
        xtypes::WritableDynamicDataRef soss_node)
{
    for (const auto& member : struct_type.members())
    {
        const std::string& member_name = member.name();
        switch (member.type().kind())
        {
            case xtypes::TypeKind::CHAR_8_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<char>();
                break;
            case xtypes::TypeKind::INT_8_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<int8_t>();
                break;
            case xtypes::TypeKind::UINT_8_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<uint8_t>();
                break;
            case xtypes::TypeKind::INT_16_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<int16_t>();
                break;
            case xtypes::TypeKind::UINT_16_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<uint16_t>();
                break;
            case xtypes::TypeKind::INT_32_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<int32_t>();
                break;
            case xtypes::TypeKind::UINT_32_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<uint32_t>();
                break;
            case xtypes::TypeKind::INT_64_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<int64_t>();
                break;
            case xtypes::TypeKind::UINT_64_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<uint64_t>();
                break;
            case xtypes::TypeKind::FLOAT_32_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<float>();
                break;
            case xtypes::TypeKind::FLOAT_64_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<double>();
                break;
            case xtypes::TypeKind::STRING_TYPE:
                soss_node[member_name] = fiware_node[member_name]["value"].get<std::string>();
                break;
            case xtypes::TypeKind::STRUCTURE_TYPE:
                value_to_soss(static_cast<const xtypes::StructType&>(member.type()),
                    fiware_node[member_name]["value"], soss_node[member_name]);
                break;
            default:
                throw false;
        }
    }
    return true;
}


template<>
inline void value_to_soss<xtypes::StructType>(
        const Json& fiware_node,
        xtypes::DynamicData::WritableNode& soss_node)
{
    // const std::string& name = soss_node.from_member()->name();
    xtypes::WritableDynamicDataRef data = soss_node.data();

    data.for_each([&](xtypes::DynamicData::WritableNode& node) -> void
    {
        if (nullptr == node.from_member())
        {
            return;
        }

        const std::string& member_name = node.from_member()->name();
        switch (node.type().kind())
        {
            case xtypes::TypeKind::CHAR_8_TYPE:
                node.data() = fiware_node[member_name]["value"].get<char>();
                break;
            case xtypes::TypeKind::INT_8_TYPE:
                node.data() = fiware_node[member_name]["value"].get<int8_t>();
                break;
            case xtypes::TypeKind::UINT_8_TYPE:
                node.data() = fiware_node[member_name]["value"].get<uint8_t>();
                break;
            case xtypes::TypeKind::INT_16_TYPE:
                node.data() = fiware_node[member_name]["value"].get<int16_t>();
                break;
            case xtypes::TypeKind::UINT_16_TYPE:
                node.data() = fiware_node[member_name]["value"].get<uint16_t>();
                break;
            case xtypes::TypeKind::INT_32_TYPE:
                node.data() = fiware_node[member_name]["value"].get<int32_t>();
                break;
            case xtypes::TypeKind::UINT_32_TYPE:
                node.data() = fiware_node[member_name]["value"].get<uint32_t>();
                break;
            case xtypes::TypeKind::INT_64_TYPE:
                node.data() = fiware_node[member_name]["value"].get<int64_t>();
                break;
            case xtypes::TypeKind::UINT_64_TYPE:
                node.data() = fiware_node[member_name]["value"].get<uint64_t>();
                break;
            case xtypes::TypeKind::FLOAT_32_TYPE:
                node.data() = fiware_node[member_name]["value"].get<float>();
                break;
            case xtypes::TypeKind::FLOAT_64_TYPE:
                node.data() = fiware_node[member_name]["value"].get<double>();
                break;
            case xtypes::TypeKind::STRING_TYPE:
                node.data() = fiware_node[member_name]["value"].get<std::string>();
                break;
            case xtypes::TypeKind::STRUCTURE_TYPE:
                value_to_soss<xtypes::StructType>(fiware_node[member_name], node);
                break;
            default:
                break;
        }
    });
}

inline bool fiware_to_soss(
        const Json& fiware_message,
        xtypes::DynamicData& soss_message)
{
    if (xtypes::TypeKind::STRUCTURE_TYPE == soss_message.type().kind())
    {
        return value_to_soss(static_cast<const xtypes::StructType&>(soss_message.type()),
            fiware_message, soss_message);
    }

    return soss_message.for_each([&](xtypes::DynamicData::WritableNode& node)
    {
        switch(node.type().kind())
        {
            case xtypes::TypeKind::STRING_TYPE:
                value_to_soss<std::string>(fiware_message, node);
                break;
            case xtypes::TypeKind::CHAR_8_TYPE:
                value_to_soss<char>(fiware_message, node);
                break;
            case xtypes::TypeKind::INT_8_TYPE:
                value_to_soss<int8_t>(fiware_message, node);
                break;
            case xtypes::TypeKind::UINT_8_TYPE:
                value_to_soss<uint8_t>(fiware_message, node);
                break;
            case xtypes::TypeKind::INT_16_TYPE:
                value_to_soss<int16_t>(fiware_message, node);
                break;
            case xtypes::TypeKind::UINT_16_TYPE:
                value_to_soss<uint16_t>(fiware_message, node);
                break;
            case xtypes::TypeKind::INT_32_TYPE:
                value_to_soss<int32_t>(fiware_message, node);
                break;
            case xtypes::TypeKind::UINT_32_TYPE:
                value_to_soss<uint32_t>(fiware_message, node);
                break;
            case xtypes::TypeKind::INT_64_TYPE:
                value_to_soss<int64_t>(fiware_message, node);
                break;
            case xtypes::TypeKind::UINT_64_TYPE:
                value_to_soss<uint64_t>(fiware_message, node);
                break;
            case xtypes::TypeKind::FLOAT_32_TYPE:
                value_to_soss<float>(fiware_message, node);
                break;
            case xtypes::TypeKind::FLOAT_64_TYPE:
                value_to_soss<double>(fiware_message, node);
                break;
            default:
                throw false;
        }
    });
}

template<typename T>
void value_to_fiware(
        const xtypes::DynamicData::ReadableNode& soss_node,
        Json& fiware_node)
{
    const std::string& member_name = soss_node.from_member()->name();
    fiware_node[member_name]["value"] = soss_node.data().value<T>();
}

inline bool value_to_fiware(
        const xtypes::StructType& struct_type,
        const xtypes::ReadableDynamicDataRef soss_node,
        Json& fiware_node)
{
    for (const auto& member : struct_type.members())
    {
        const std::string& member_name = member.name();
        switch (member.type().kind())
        {
            case xtypes::TypeKind::CHAR_8_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<char>();
                break;
            case xtypes::TypeKind::INT_8_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<int8_t>();
                break;
            case xtypes::TypeKind::UINT_8_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<uint8_t>();
                break;
            case xtypes::TypeKind::INT_16_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<int16_t>();
                break;
            case xtypes::TypeKind::UINT_16_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<uint16_t>();
                break;
            case xtypes::TypeKind::INT_32_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<int32_t>();
                break;
            case xtypes::TypeKind::UINT_32_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<uint32_t>();
                break;
            case xtypes::TypeKind::INT_64_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<int64_t>();
                break;
            case xtypes::TypeKind::UINT_64_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<uint64_t>();
                break;
            case xtypes::TypeKind::FLOAT_32_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<float>();
                break;
            case xtypes::TypeKind::FLOAT_64_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<double>();
                break;
            case xtypes::TypeKind::STRING_TYPE:
                fiware_node[member_name]["value"] = soss_node[member_name].value<std::string>();
                break;
            case xtypes::TypeKind::STRUCTURE_TYPE:
                value_to_fiware(static_cast<const xtypes::StructType&>(member.type()),
                    soss_node[member_name], fiware_node[member_name]["value"]);
                break;
            default:
                throw false;
        }
    }
    return true;
}

inline bool soss_to_fiware(
        const xtypes::DynamicData& soss_message,
        Json& fiware_message)
{
    if (xtypes::TypeKind::STRUCTURE_TYPE == soss_message.type().kind())
    {
        return value_to_fiware(static_cast<const xtypes::StructType&>(soss_message.type()),
            soss_message, fiware_message);
    }

    return soss_message.for_each([&](const xtypes::DynamicData::ReadableNode& node)
    {
        switch(node.type().kind())
        {
            case xtypes::TypeKind::STRING_TYPE:
                value_to_fiware<std::string>(node, fiware_message);
                break;
            case xtypes::TypeKind::CHAR_8_TYPE:
                value_to_fiware<char>(node, fiware_message);
                break;
            case xtypes::TypeKind::INT_8_TYPE:
                value_to_fiware<int8_t>(node, fiware_message);
                break;
            case xtypes::TypeKind::UINT_8_TYPE:
                value_to_fiware<uint8_t>(node, fiware_message);
                break;
            case xtypes::TypeKind::INT_16_TYPE:
                value_to_fiware<int16_t>(node, fiware_message);
                break;
            case xtypes::TypeKind::UINT_16_TYPE:
                value_to_fiware<uint16_t>(node, fiware_message);
                break;
            case xtypes::TypeKind::INT_32_TYPE:
                value_to_fiware<int32_t>(node, fiware_message);
                break;
            case xtypes::TypeKind::UINT_32_TYPE:
                value_to_fiware<uint32_t>(node, fiware_message);
                break;
            case xtypes::TypeKind::INT_64_TYPE:
                value_to_fiware<int64_t>(node, fiware_message);
                break;
            case xtypes::TypeKind::UINT_64_TYPE:
                value_to_fiware<uint64_t>(node, fiware_message);
                break;
            case xtypes::TypeKind::FLOAT_32_TYPE:
                value_to_fiware<float>(node, fiware_message);
                break;
            case xtypes::TypeKind::FLOAT_64_TYPE:
                value_to_fiware<double>(node, fiware_message);
                break;
            default:
                throw false;
        }
    });
}

} // namespace conversion
} // namespace fiware
} // namespace soss

#endif // SOSS__FIWARE__INTERNAL__CONVERSION_HPP


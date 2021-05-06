/*
 * Copyright 2019 - present Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef _IS_SH_FIWARE__INTERNAL__CONVERSION_HPP_
#define _IS_SH_FIWARE__INTERNAL__CONVERSION_HPP_

#include "is/json-xtypes/json.hpp"

#include <is/core/Message.hpp>

#include <is/utils/Log.hpp>

#include <iostream>

namespace eprosima {
namespace is {
namespace sh {
namespace fiware {

using Json = nlohmann::json;

namespace conversion {

utils::Logger logger("is::sh::FIWARE::Conversion");

/**
 * @brief Declare conversion map from FIWARE's primitive built-in types to xTypes.
 *
 * @param[in] fiware_type The source FIWARE type, in Json format.
 * @param[in] xtype The target type.
 *
 * @returns `true` in case of success, `false` otherwise.
 */
inline bool fiware_type_to_xtypes(
        const Json& fiware_type,
        xtypes::StructType& xtype)
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

    for (const auto& it : fiware_type.items())
    {
        const std::string& member_name = it.key();
        std::string type_name = it.value()["types"][0].get<std::string>();

        const auto& mapping = types_mapping.find(type_name);
        if (mapping == types_mapping.end())
        {
            logger << utils::Logger::Level::ERROR
                   << "No mapping for fiware type '" << type_name << "'" << std::endl;

            return false;
        }
        else
        {
            logger << utils::Logger::Level::DEBUG
                   << "Adding member '" << member_name
                   << "' to type '" << xtype.name() << "'" << std::endl;
        }
        xtype.add_member(member_name, *mapping->second);
    }

    return true;
}

} //  namespace conversion
} //  namespace fiware
} //  namespace sh
} //  namespace is
} //  namespace eprosima

#endif //  _IS_SH_FIWARE__INTERNAL__CONVERSION_HPP_


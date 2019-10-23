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

namespace soss {
namespace fiware {

using Json = nlohmann::json;

namespace conversion {

inline bool fiware_type_to_soss_type(
        const Json& /*fiware_type*/,
        xtypes::StructType& /*soss_type*/)
{
    //TODO
    return true;
}

inline bool fiware_to_soss(
        const Json& /*fiware_message*/,
        xtypes::DynamicData& soss_message)
{
    return soss_message.for_each([&](const xtypes::DynamicData::WritableNode& /*node*/)
    {
        /*
        const std::string& member_name = node.access().struct_member().name();

        switch(node.type().kind())
        {
            case xtypes::TypeKind::STRUCTURE_TYPE:
                break;
            case xtypes::TypeKind::UINT_32_TYPE:
                node.data().value(middleware_message.at(member_name));
                break;
            //... other types
            default:
                throw false;
        }
        */
    });
}

inline bool soss_to_fiware(
        const xtypes::DynamicData& soss_message,
        Json& /*fiware_message*/)
{
    return soss_message.for_each([&](const xtypes::DynamicData::ReadableNode& /*node*/)
    {
        /*
        const std::string& member_name = node.access().struct_member().name();

        switch(node.type().kind())
        {
            case xtypes::TypeKind::STRUCTURE_TYPE:
                break;
            case xtypes::TypeKind::UINT_32_TYPE:
                middleware_message.emplace(member_name, node.data().value<uint32_t>());
                break;
            //... other types
            default:
                throw false;
        }
        */
    });
}

} // namespace conversion
} // namespace fiware
} // namespace soss

#endif // SOSS__FIWARE__INTERNAL__CONVERSION_HPP


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

#ifndef _IS_SH_FIWARE__INTERNAL__PUBLISHER_HPP_
#define _IS_SH_FIWARE__INTERNAL__PUBLISHER_HPP_

#include "conversion.hpp"
#include "NGSIV2Connector.hpp"

#include <is/systemhandle/SystemHandle.hpp>

#include <is/utils/Log.hpp>

#include <is/json-xtypes/conversion.hpp>

#include <iostream>

namespace eprosima {
namespace is {
namespace sh {
namespace fiware {

// TODO Doxygen
class Publisher : public virtual is::TopicPublisher
{
public:

    Publisher(
            NGSIV2Connector& fiware_connector,
            const std::string& topic_name,
            const xtypes::DynamicType& message_type)
        : fiware_connector_(fiware_connector)
        , topic_name_(topic_name)
        , message_type_(message_type)
        , logger_("is::sh::FIWARE::Publisher")
    {
    }

    virtual ~Publisher() override = default;

    bool publish(
            const xtypes::DynamicData& xtypes_message)
    {
        try
        {
            Json fiware_message = json_xtypes::convert(xtypes_message, "value");

            logger_ << utils::Logger::Level::INFO
                    << "Translate message from Integration Service to FIWARE for topic '"
                    << topic_name_ << "' with type '" << message_type_.name()
                    << "', payload: [[ " << xtypes_message << " ]]" << std::endl;

            return fiware_connector_.update_entity(topic_name_, message_type_.name(), fiware_message);
        }
        catch (const json_xtypes::UnsupportedType& e)
        {
            logger_ << utils::Logger::Level::ERROR
                    << "Failed to convert message from Integration Service to FIWARE for topic '"
                    << topic_name_ << "' with type '" << message_type_.name()
                    << "'; reason: " << e.what() << std::endl;

            return false;
        }

    }

private:

    NGSIV2Connector& fiware_connector_;

    std::string topic_name_;
    const xtypes::DynamicType& message_type_;

    utils::Logger logger_;
};

} //  namespace fiware
} //  namespace sh
} //  namespace is
} //  namespace eprosima

#endif //  _IS_SH_FIWARE__INTERNAL__PUBLISHER_HPP_

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

#ifndef SOSS__FIWARE__INTERNAL__PUBLISHER_HPP
#define SOSS__FIWARE__INTERNAL__PUBLISHER_HPP


#include "conversion.hpp"
#include "NGSIV2Connector.hpp"

#include <soss/SystemHandle.hpp>

#include <iostream>

namespace soss {
namespace fiware {

class Publisher : public virtual TopicPublisher
{
public:
    Publisher(
            NGSIV2Connector& fiware_connector,
            const std::string& topic_name,
            const xtypes::DynamicType& message_type)
        : fiware_connector_{fiware_connector}
        , topic_name_{topic_name}
        , message_type_{message_type}
    {}

    virtual ~Publisher() override = default;

    bool publish(
            const xtypes::DynamicData& soss_message)
    {
        std::cout << "[soss-fiware][publisher]: translate message: soss -> fiware "
            "(" << topic_name_ << ") " << std::endl;

        Json fiware_message;
        if(!conversion::soss_to_fiware(soss_message, fiware_message))
        {
            std::cerr << "[soss-fiware][publisher]: conversion error" << std::endl;
            return false;
        }

        return fiware_connector_.update_entity(topic_name_, message_type_.name(), fiware_message);
    }

private:
    NGSIV2Connector& fiware_connector_;

    std::string topic_name_;
    const xtypes::DynamicType& message_type_;
};


} //namespace fiware
} //namespace soss

#endif // SOSS__FIWARE__INTERNAL__PUBLISHER_HPP

/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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
#include <typeindex>

namespace soss {
namespace fiware {


using Json = nlohmann::json;

struct Conversion {

    static Json soss_to_fiware(const soss::Message& input);
    static soss::Message fiware_to_soss(const std::string& type, const Json& input);

private:
    ~Conversion() = default;
};

static std::map<std::string, std::string> m_fiware_types =
{
    {typeid(bool).name(),       "Boolean"   },
    {typeid(int8_t).name(),     "int8"      },
    {typeid(int16_t).name(),    "int16"     },
    {typeid(int32_t).name(),    "int32"     },
    {typeid(int64_t).name(),    "int64"     },
    {typeid(uint8_t).name(),    "uint8"     },
    {typeid(uint16_t).name(),   "uint16"    },
    {typeid(uint32_t).name(),   "uint32"    },
    {typeid(uint64_t).name(),   "uint64"    },
    {typeid(float).name(),      "Float"     },
    {typeid(long).name(),       "Long"      },
    {typeid(double).name(),     "Double"    },
    {typeid(std::string).name(),"String"    }
};

} // namespace fiware
} // namespace soss

#endif // SOSS__FIWARE__INTERNAL__CONVERSION_HPP


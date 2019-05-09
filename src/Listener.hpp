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

#ifndef SOSS__FIWARE__INTERNAL__LISTENER_HPP
#define SOSS__FIWARE__INTERNAL__LISTENER_HPP

#include <asio.hpp>

#include <functional>
#include <thread>

#include <iostream>
#include <mutex>

namespace soss {
namespace fiware {

class Listener {

    using DataReceivedCallback = std::function<void(const std::string& message)>;
    using tcp = asio::ip::tcp;

public:
    Listener(
            uint16_t port,
            DataReceivedCallback callback);

    virtual ~Listener();

    void run();
    void stop();

    uint16_t get_port() const { return port_; }

    bool is_running() const { return running_; }
    bool has_errors() const { return errors_; }

private:
    void listen();
    void start_accept();
    void accept_handler(std::shared_ptr<tcp::socket> socket);
    void read_msg(std::shared_ptr<tcp::socket> socket);

    static const std::size_t BUFFER_SIZE = 8096;

    uint16_t port_;

    std::thread listen_thread_;
    std::vector<std::thread> message_threads_;
    bool running_;
    bool errors_;

    DataReceivedCallback read_callback_;
    asio::io_service service_;
    std::shared_ptr<tcp::acceptor> acceptor_;

    std::mutex mutex_;
};

} // namespace fiware
} // namespace soss

#endif // SOSS__FIWARE__INTERNAL__LISTENER_HPP

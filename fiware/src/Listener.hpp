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

#ifndef SOSS__FIWARE__INTERNAL__LISTENER_HPP
#define SOSS__FIWARE__INTERNAL__LISTENER_HPP

#include <asio.hpp>

#include <functional>
#include <thread>

#include <iostream>


namespace soss {
namespace fiware {

class SyncPort;
class Listener {

    using DataReceivedCallback = std::function<void(const std::string& message)>;
    using tcp = asio::ip::tcp;

public:
    Listener(
            uint16_t desired_port,
            DataReceivedCallback callback);

    virtual ~Listener();

    uint16_t run();
    void stop();

    bool is_running() const { return running_; }
    bool has_errors() const { return errors_; }

private:
    void listen(
            SyncPort& listening_port);

    void start_accept(
            tcp::acceptor& acceptor);

    void accept_handler(
            std::shared_ptr<tcp::socket> socket,
            tcp::acceptor& acceptor);

    void read_msg(
            std::shared_ptr<tcp::socket> socket);


    uint16_t desired_port_;

    std::thread listen_thread_;
    std::vector<std::thread> message_threads_;
    bool running_;
    bool errors_;

    DataReceivedCallback read_callback_;

    asio::io_service service_;

};

} // namespace fiware
} // namespace soss

#endif // SOSS__FIWARE__INTERNAL__LISTENER_HPP

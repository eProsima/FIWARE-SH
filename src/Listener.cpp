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

#include "Listener.hpp"

#include <iostream>

namespace soss {
namespace fiware {


Listener::Listener(
        uint16_t port,
        DataReceivedCallback callback)

    : port_{port}
    , listen_thread_{}
    , running_{false}
    , errors_{false}
    , read_callback_{callback}
{
}

Listener::~Listener()
{
    stop();
}

void Listener::run()
{
    listen_thread_ = std::thread(&Listener::listen, this);
    running_ = true;

    //TODO: should wait until the port was open, use a condition variable?
}

void Listener::stop()
{
    if(running_ && listen_thread_.joinable())
    {
        service_.stop();
        running_ = false;
        listen_thread_.join();

        for (unsigned int i = 0; i < message_threads_.size(); ++i)
        {
            if (message_threads_[i].joinable())
            {
                message_threads_.at(i).join();
            }
        }
    }
}

void Listener::listen()
{
    try
    {
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port_);
        asio::error_code error;

        acceptor_ = std::make_shared<asio::ip::tcp::acceptor>(service_, endpoint);

        std::cout << "[soss-fiware][listener]: listening fiware at port " << port_ << std::endl;

        start_accept();
    }
    catch (std::exception& e)
    {
        std::cerr << "[soss-fiware][listener]: connection error: " << e.what() << std::endl;
        errors_ = true;
    }

    std::cout << "[soss-fiware][listener]: stop listening" << std::endl;
}

void Listener::start_accept()
{
    std::shared_ptr<tcp::socket> socket(new tcp::socket (service_));

    acceptor_->async_accept(*socket, std::bind(&Listener::accept_handler, this, socket));
    service_.run();

    return;
}


void Listener::accept_handler(std::shared_ptr<tcp::socket> socket)
{
    message_threads_.push_back(std::thread(&Listener::read_msg, this, socket));
    start_accept();

    return;
}

void Listener::read_msg(std::shared_ptr<tcp::socket> socket)
{
    asio::error_code error;
    std::array<char, BUFFER_SIZE> buffer;
    std::stringstream ss;
    std::size_t length = socket->read_some(asio::buffer(buffer, BUFFER_SIZE), error);
    ss.write(buffer.data(), static_cast<std::streamsize>(length));

    //Connection problem
    if (error && asio::error::eof != error)
    {
        throw asio::system_error(error);
    }

    //Problem with this socket
    if (0 == length || asio::error::eof == error)
    {
        std::cerr << "[soss-fiware][listener]: connection error: " << error.message() << std::endl;
        return;
    }

    read_callback_(ss.str());

    return;
}

} // namespace fiware
} // namespace soss

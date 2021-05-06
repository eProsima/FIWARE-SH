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

#ifndef _IS_SH_FIWARE__INTERNAL__INTERNAL__LISTENER_HPP_
#define _IS_SH_FIWARE__INTERNAL__INTERNAL__LISTENER_HPP_

#define ASIO_STANDALONE
#include <asio.hpp>

#include <is/utils/Log.hpp>

#include <functional>
#include <thread>
#include <future>

#include <iostream>

namespace eprosima {
namespace is {
namespace sh {
namespace fiware {

// TODO doxygen

class Listener
{
    using DataReceivedCallback = std::function<void(const std::string& message)>;
    using tcp = asio::ip::tcp;

public:

    Listener(
            const uint16_t desired_port,
            DataReceivedCallback callback)
        : desired_port_(desired_port)
        , listen_thread_()
        , running_(false)
        , errors_(false)
        , read_callback_(callback)
        , logger_("is::sh::FIWARE::Listener")
    {
    }

    virtual ~Listener()
    {
        stop();
    }

    uint16_t run()
    {
        std::promise<uint16_t> listening_port_promise;
        std::future<uint16_t> listening_port_future = listening_port_promise.get_future();
        listen_thread_ = std::thread(&Listener::listen, this, std::ref(listening_port_promise));
        running_ = true;

        return listening_port_future.get();
    }

    void stop()
    {
        if (running_ && listen_thread_.joinable())
        {
            service_.stop();
            running_ = false;
            listen_thread_.join();

            for (std::thread& th: message_threads_)
            {
                if (th.joinable())
                {
                    th.join();
                }
            }
        }
    }

    bool is_running() const
    {
        return running_;
    }

    bool has_errors() const
    {
        return errors_;
    }

private:

    void listen(
            std::promise<uint16_t>& listening_port_promise)
    {
        try
        {
            asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), desired_port_);
            asio::error_code error;
            tcp::acceptor acceptor(service_, endpoint);
            uint16_t port = acceptor.local_endpoint().port();
            listening_port_promise.set_value(port);

            logger_ << utils::Logger::Level::INFO
                    << "Listening to FIWARE Context Broker at port " << port << std::endl;

            start_accept(acceptor);
            service_.run();
        }
        catch (std::exception& e)
        {
            logger_ << utils::Logger::Level::ERROR
                    << "Connection error: " << e.what() << std::endl;
            errors_ = true;
        }

        logger_ << utils::Logger::Level::INFO
                << "Stop listening to FIWARE Context Broker" << std::endl;
    }

    void start_accept(
            tcp::acceptor& acceptor)
    {
        std::shared_ptr<tcp::socket> socket(new tcp::socket (service_));

        acceptor.async_accept(
            *socket, std::bind(&Listener::accept_handler, this, socket, std::ref(acceptor)));
    }

    void accept_handler(
            std::shared_ptr<tcp::socket> socket,
            tcp::acceptor& acceptor)
    {
        message_threads_.push_back(std::thread(&Listener::read_msg, this, socket));
        start_accept(acceptor);
    }

    void read_msg(
            std::shared_ptr<tcp::socket> socket)
    {
        try
        {
            //wait for some data
            socket->read_some(asio::null_buffers());

            std::string message;
            message.resize(socket->available());

            socket->read_some(asio::buffer(&message[0], message.size()));

            logger_ << utils::Logger::Level::INFO
                    << "Process incoming message: '" << message << "'" << std::endl;

            read_callback_(message);
        }
        catch (std::exception& e)
        {
            logger_ << utils::Logger::Level::ERROR
                    << "Connection error in reading thread: " << e.what() << std::endl;
        }
    }

    const uint16_t desired_port_;

    std::thread listen_thread_;
    std::vector<std::thread> message_threads_;
    bool running_;
    bool errors_;

    DataReceivedCallback read_callback_;

    asio::io_service service_;

    utils::Logger logger_;
};

} //  namespace fiware
} //  namespace sh
} //  namespace is
} //  namespace eprosima

#endif //  _IS_SH_FIWARE__INTERNAL__INTERNAL__LISTENER_HPP_

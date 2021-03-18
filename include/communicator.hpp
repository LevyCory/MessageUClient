#ifndef __messageu_communicator_hpp__
#define __messageu_communicator_hpp__

#include <boost/asio.hpp>

#include "buffer.hpp"

using tcp = boost::asio::ip::tcp;

class Session final
{
public:
    Session(tcp::socket& socket, const tcp::endpoint& endpoint);
    ~Session();
    Session(const Session&) = delete;
    Session& operator= (const Session&) = delete;
    Session(Session&&) noexcept = default;
    Session& operator= (Session&&) noexcept = default;

    void send(const common::Buffer& buffer);
    common::Buffer receive();

private:
    tcp::socket& _socket;
};

class Communicator final
{
public:
    explicit Communicator(tcp::endpoint&& endpoint) :
        _socket(_ctx),
        _endpoint(std::move(endpoint))
    {}

    ~Communicator() = default;
    Communicator(const Communicator&) = delete;
    Communicator& operator= (const Communicator&) = delete;
    Communicator(Communicator&&) noexcept = default;
    Communicator& operator= (Communicator&&) noexcept = default;

    Session connect() { return Session(_socket, _endpoint); }

private:
    boost::asio::io_context _ctx;
    tcp::socket _socket;
    tcp::endpoint _endpoint;
};

#endif /* __messageu_communicator_hpp__ */

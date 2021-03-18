#include "communicator.hpp"
#include "protocol.hpp"

using namespace protocol;

Session::Session(tcp::socket& socket, const boost::asio::ip::tcp::endpoint& endpoint) :
    _socket(socket)
{
    _socket.connect(endpoint);
}

Session::~Session()
{
    _socket.close();
}

void Session::send(const common::Buffer& buffer)
{
    _socket.send(buffer.to_boost());
}

common::Buffer Session::receive()
{
    // Allocating page size in attempt to avoid realloc for the payload
    common::Buffer buffer(common::PAGE_SIZE);
    _socket.receive(buffer.to_boost(sizeof(response::Header)));

    auto* header = reinterpret_cast<response::Header*>(buffer.data());
    if (header->size == 0)
    {
        return buffer;
    }

    if (header->size > common::PAGE_SIZE - sizeof(response::Header))
    {
        buffer.resize(buffer.size() + header->size);

        // Buffer::resize reallocates memory, which is not guaranteed to
        // return a buffer in the same address
        header = reinterpret_cast<response::Header*>(buffer.data());
    }

    _socket.receive(buffer.to_boost(header->size, sizeof(response::Header)));
    return buffer;
}

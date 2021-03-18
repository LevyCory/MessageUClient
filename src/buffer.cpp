#include "buffer.hpp"

namespace common
{
    std::string Buffer::to_hex_string() const
    {
        std::string converted_buffer;
        boost::algorithm::hex(_buffer.begin(), _buffer.end(),
                              std::back_inserter(converted_buffer));

        return converted_buffer;
    }

    Buffer Buffer::from_hex_string(const std::string& hex_string)
    {
        auto unhex = boost::algorithm::unhex(hex_string);
        return Buffer(unhex.begin(), unhex.end());
    }

    boost::asio::mutable_buffer
    Buffer::to_boost(std::size_t size, std::size_t offset)
    {
        return boost::asio::buffer(_buffer.data() + offset, size ? size : _buffer.size());
    }

    boost::asio::const_buffer
    Buffer::to_boost(std::size_t size, std::size_t offset) const
    {
        return boost::asio::buffer(_buffer.data() + offset, size ? size : _buffer.size());
    }
}

bool operator== (const common::Buffer& lhs, const common::Buffer& rhs)
{
    return lhs.get() == rhs.get();
}

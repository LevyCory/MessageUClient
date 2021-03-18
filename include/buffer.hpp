#ifndef __messageu_buffer_hpp__
#define __messageu_buffer_hpp__

#include <vector>
#include <functional>
#include <cstdint>

#include <boost/asio.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/container_hash/hash.hpp>

#include "common.hpp"
#include "protocol.hpp"

namespace common
{
    class Buffer
    {
    public:
        using value_type = uint8_t;
        using vector_type = std::vector<value_type>;
        using iterator_type = typename vector_type::iterator;
        using const_iterator_type = typename vector_type::const_iterator;

        Buffer() = default;
        explicit Buffer(std::size_t size) :
            _buffer(size)
        {}

        template<typename SourceType>
        Buffer(const SourceType* source, std::size_t size) :
            _buffer(size)
        {
            std::memcpy(_buffer.data(), source, size);
        }

        template<typename IteratorType>
        Buffer(IteratorType begin, IteratorType end) :
            _buffer(begin, end)
        {}
            
        virtual ~Buffer() = default;
        Buffer& operator=(const Buffer&) = delete;
        Buffer(Buffer&& buffer) noexcept = default;
        Buffer& operator=(Buffer&& buffer) noexcept = default;

        uint8_t& operator[] (const std::size_t index) { return _buffer[index]; }
        const uint8_t& operator[] (const std::size_t index) const { return _buffer[index]; }

        vector_type& get() { return _buffer; }
        const vector_type& get() const { return _buffer; }

        // Instead of copy constructor
        Buffer clone() const { return *this; }

        template<typename T>
        [[nodiscard]]
        std::enable_if_t<std::is_pod_v<T>, T&>
        to_pod(std::size_t offset = 0)
        {
            assert(sizeof(T) <= _buffer.size());
            return *reinterpret_cast<T*>(_buffer.data() + offset);
        }

        template<typename T>
        [[nodiscard]]
        std::enable_if_t<std::is_pod_v<T>, const T&>
        to_pod(std::size_t offset = 0) const
        {
            assert(sizeof(T) <= _buffer.size());
            return *reinterpret_cast<const T*>(_buffer.data() + offset);
        }

        uint8_t* data() { return _buffer.data(); }
        const uint8_t* data() const { return _buffer.data(); }

        void resize(std::size_t new_size) { _buffer.resize(new_size); }
        std::size_t size() const { return _buffer.capacity(); }
        bool empty() const { return _buffer.empty(); }

        iterator_type begin() { return _buffer.begin(); }
        iterator_type end() { return _buffer.end(); }
        const_iterator_type begin() const { return _buffer.cbegin(); }
        const_iterator_type end() const { return _buffer.cend(); }

        std::string to_hex_string() const;
        static Buffer from_hex_string(const std::string& hex_string);
        std::string to_string() const { return std::string(_buffer.begin(), _buffer.end()); }

        boost::asio::mutable_buffer to_boost(std::size_t size = 0, std::size_t offset = 0);
        boost::asio::const_buffer to_boost(std::size_t size = 0, std::size_t offset = 0) const;

    private:
        // To copy this object the user must call the 'clone' method
        Buffer(const Buffer&) = default;

        vector_type _buffer;
    };
};

bool operator== (const common::Buffer& lhs, const common::Buffer& rhs);

namespace std
{
    template <>
    struct hash<common::Buffer>
    {
        size_t operator() (const common::Buffer& buffer) const
        {
            return boost::hash<common::Buffer::vector_type>()(buffer.get());
        }
    };

    template<>
    struct equal_to<common::Buffer>
    {
        bool operator() (const common::Buffer& lhs, const common::Buffer& rhs) const
        {
            return lhs.get() == rhs.get();
        }
    };
}

#endif /* __messageu_buffer_hpp__ */

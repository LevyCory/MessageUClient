#ifndef __messageu_message_stream_hpp__
#define __messageu_message_stream_hpp__

#include <string_view>

#include "buffer.hpp"
#include "protocol.hpp"

using protocol::response::MessageEntry;

class MessageStream
{
public:
    explicit MessageStream(const common::Buffer& buffer) :
        _header(reinterpret_cast<const protocol::response::Header*>(buffer.data()))
    {}

    class MessageIterator
    {
    public:
        explicit MessageIterator(const MessageEntry* first) :
            _current(first)
        {}

        friend bool operator== (const MessageIterator& lhs, const MessageIterator& rhs);
        friend bool operator!= (const MessageIterator& lhs, const MessageIterator& rhs);
        
        MessageIterator& operator++ () noexcept;
        MessageIterator operator++(int) noexcept;

        const MessageEntry& operator*() { return *_current; }

    private:
        std::size_t _current_message_size() const;

        const MessageEntry* _current;
    };

    MessageIterator begin();
    MessageIterator end();

private:
    const protocol::response::Header* const _header;
};

#endif /* __messageu_message_stream_hpp__ */

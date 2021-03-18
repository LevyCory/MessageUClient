#include "message_stream.hpp"

std::size_t
MessageStream::MessageIterator::_current_message_size() const
{
    return sizeof(*_current) + _current->message_size;
}

MessageStream::MessageIterator&
MessageStream::MessageIterator::operator++() noexcept
{
    _current = common::advance_by(_current, _current_message_size());
    return *this;
}

MessageStream::MessageIterator
MessageStream::MessageIterator::operator++(int) noexcept
{
    MessageIterator temp = *this;
    ++(*this);
    return temp;
}

MessageStream::MessageIterator
MessageStream::begin()
{
    return MessageIterator(reinterpret_cast<const MessageEntry*>(_header->payload));
}


MessageStream::MessageIterator
MessageStream::end()
{
    const auto* ptr = common::advance_by(_header->payload, _header->size);
    return MessageIterator(reinterpret_cast<const MessageEntry*>(ptr));
}

bool operator== (const MessageStream::MessageIterator& lhs,
                 const MessageStream::MessageIterator& rhs)
{
    return lhs._current == rhs._current;
}

bool operator!= (const MessageStream::MessageIterator& lhs,
                 const MessageStream::MessageIterator& rhs)
{
    return !(lhs == rhs);
}

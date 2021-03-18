#ifndef __messageu_response_handler_hpp__
#define __messageu_response_handler_hpp__

#include <functional>
#include <unordered_map>

#include "buffer.hpp"
#include "protocol.hpp"
#include "state.hpp"

using namespace protocol;

class ResponseHandler
{
    using handler_type = std::function<void(const common::Buffer&)>;
    using handler_table_type = std::unordered_map<response::Type, handler_type>;

public:
    ResponseHandler(ClientState& state) :
        _handler_table(_build_handler_table()),
        _state(state)
    {}

    void handle(const common::Buffer& response_buffer);

private:
    template <typename P>
    using protocol_pair = std::pair<const response::Header&, const P&>;

    template <typename P>
    protocol_pair<P> _parse_buffer(const common::Buffer& buffer)
    {
        return protocol_pair<P>(
            buffer.to_pod<response::Header>(),
            buffer.to_pod<P>(sizeof(response::Header))
        );
    }

    void _error_handler(const common::Buffer&);
    void _message_ack_response(const common::Buffer&);
    void _registration_handler(const common::Buffer& buffer);
    void _user_list_handler(const common::Buffer& buffer);
    void _public_key_handler(const common::Buffer& buffer);
    void _messages_handler(const common::Buffer& buffer);

private:
    handler_table_type _build_handler_table();

    void _message_text_handler(const User& user, const response::MessageEntry& message);
    void _message_sym_key_request_handler(const User& user, const response::MessageEntry& message);
    void _message_sym_key_response_handler(User& user, const response::MessageEntry& message);

    const handler_table_type _handler_table;
    ClientState& _state;
};

#endif /* __messageu_response_handler_hpp__ */

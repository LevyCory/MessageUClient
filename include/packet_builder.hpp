#ifndef __messageu_packet_builder_hpp__
#define __messageu_packet_builder_hpp__

#include <string>

#include <boost/asio.hpp>

#include "state.hpp"
#include "common.hpp"
#include "protocol.hpp"

using namespace protocol;
using common::Buffer;

class PacketBuilder final
{
public:
    PacketBuilder(const ClientState& state) :
        _state(state)
    {}

    Buffer registration(const std::string& user_name) const;
    Buffer list_users() const;
    Buffer get_messages() const;
    Buffer request_public_key(const Buffer& client_uid) const;

    template <typename ContentType>
    common::Buffer _encrypt_message(const ContentType& content,
                                    const User& user,
                                    MessageType type) const
    {
        if (content.empty())
        {
            return {};
        }

        if (type == MessageType::EncryptedText)
        {
            if (!user.aes_cipher)
            {
                throw common::MessageUException("User symmetric key does not exist");
            }

            return user.aes_cipher->encrypt(content);
        }
        else
        {
            if (!user.rsa_encryptor)
            {
                throw common::MessageUException("User public key does not exist");
            }

            return user.rsa_encryptor->encrypt(content);
        }
    }

    template <typename ContentType>
    Buffer send_message(const ContentType& content,
                        const User& recipient,
                        MessageType type) const
    {
        _require_uid();

        auto outgoing_content = _encrypt_message(content, recipient, type);
        auto buffer = _get_request_buffer(request::Type::SendMessage,
                                          static_cast<uint32_t>(outgoing_content.size()));

        auto& message = buffer.to_pod<request::Message>(sizeof(request::Header));
        std::memcpy(message.uid, recipient.id.data(), sizeof(message.uid));

        message.type = type;
        message.size = static_cast<uint32_t>(outgoing_content.size());
        std::copy(outgoing_content.begin(), outgoing_content.end(), message.payload);

        return buffer;
    }

private:
    Buffer _get_request_buffer(request::Type type, uint32_t additional_data = 0) const;
    void _fill_header(Buffer& request_buffer, request::Type code, uint32_t size) const;
    void _require_uid() const;

    const ClientState& _state;
};

#endif /* __messageu_packet_builder_hpp__ */

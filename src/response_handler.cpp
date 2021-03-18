#include "response_handler.hpp"

#include <iostream>
#include <string_view>

#include "common.hpp"
#include "message_stream.hpp"
#include "aes_cipher.hpp"

using namespace response;

ResponseHandler::handler_table_type
ResponseHandler::_build_handler_table()
{
#define BIND_HANDLER(handler) std::bind(&ResponseHandler::handler, this, std::placeholders::_1)

    handler_table_type handler_table;

    handler_table[response::Type::RegistrationSuccess] = BIND_HANDLER(_registration_handler);
    handler_table[response::Type::UserList] = BIND_HANDLER(_user_list_handler);
    handler_table[response::Type::PendingMessages] = BIND_HANDLER(_messages_handler);
    handler_table[response::Type::PublicKeyResponse] = BIND_HANDLER(_public_key_handler);
    handler_table[response::Type::MessageSentAck] = BIND_HANDLER(_message_ack_response);
    handler_table[response::Type::Error] = BIND_HANDLER(_error_handler);

    return handler_table;

#undef BIND_HANDLER
}

void ResponseHandler::_message_ack_response(const common::Buffer&)
{
    std::cout << "Successfully sent message\n";
}

void ResponseHandler::_error_handler(const common::Buffer&)
{
    throw common::MessageUException("Server returned an unknown error code");
}

void ResponseHandler::_registration_handler(const common::Buffer& buffer)
{
    const auto& header = buffer.to_pod<response::Header>();
    common::Buffer uid(header.payload, header.size);

    _state.self().id = std::move(uid);
    _state.store(common::CLIENT_INFO_FILE);
}

void ResponseHandler::_public_key_handler(const common::Buffer& buffer)
{
    const auto& [header, payload] = _parse_buffer<PublicKeyResponse>(buffer);
    common::Buffer id(payload.id, sizeof(payload.id));

    auto& user = _state.db().get(id);
    common::Buffer public_key(payload.public_key, sizeof(payload.public_key));
    user.rsa_encryptor = std::make_unique<RsaEncryptor>(public_key);
}

void ResponseHandler::_user_list_handler(const common::Buffer& buffer)
{
    auto& header = buffer.to_pod<response::Header>();
    uint32_t user_count = header.size / sizeof(UserListEntry);

    const auto* entry = reinterpret_cast<const UserListEntry*>(header.payload);
    for (std::size_t i = 0; i < user_count; i++, entry++)
    {
        std::string name{ entry->name.str };
        std::cout << name << "\n";

        if (!_state.db().exists(name))
        {
            _state.db().emplace(std::move(name),
                                common::Buffer(entry->id, sizeof(entry->id)));
        }
    }
}

void ResponseHandler::_messages_handler(const common::Buffer& buffer)
{
    MessageStream stream(buffer);
    for (const auto& message : stream)
    {
        common::Buffer id(message.sender_id, sizeof(message.sender_id));
        auto& user = _state.db().get(id);

        switch (message.type)
        {
        case MessageType::SymKeyRequest:
            _message_sym_key_request_handler(user, message);
            std::cout << "Request for symmetric key\n";
            break;
        case MessageType::SymKeyResponse:
            _message_sym_key_response_handler(user, message);
            std::cout << "Response for symmetric key\n";
            break;
        case MessageType::EncryptedText:
            return _message_text_handler(user, message);
        default:
            throw common::MessageUException("Unknown message type");
        }
    }
}

void ResponseHandler::_message_text_handler(const User& user,
                                            const response::MessageEntry& message)
{
    common::Buffer content(message.content, message.message_size);
    auto decrypted_message = _state.self().aes_cipher->decrypt(content);
    std::cout << decrypted_message.to_string() << "\n";
}

void ResponseHandler::_message_sym_key_response_handler(User& user,
                                                        const response::MessageEntry& message)
{
    common::Buffer content(message.content, message.message_size);
    common::Buffer sym_key = _state.self().rsa_decryptor->decrypt(content);
    user.aes_cipher = std::make_unique<AesCipher>(std::move(sym_key));
}

void ResponseHandler::_message_sym_key_request_handler(const User& user,
                                                       const response::MessageEntry& message)
{
    std::cout << user.name << " requested your symmetric key\n";
}

void ResponseHandler::handle(const common::Buffer& response_buffer)
{
    auto& header = response_buffer.to_pod<response::Header>();
    auto callback_entry = _handler_table.find(header.opcode);
    if (callback_entry == _handler_table.end())
    {
        throw common::MessageUException("Invalid message code");
    }

    return callback_entry->second(response_buffer);
}


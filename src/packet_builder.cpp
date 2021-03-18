#include "packet_builder.hpp"


Buffer PacketBuilder::_get_request_buffer(request::Type type, uint32_t additional_data) const
{
    uint32_t request_size = sizeof(request::Header);
    switch (type)
    {
    case protocol::request::Type::GetUserList:
    case protocol::request::Type::GetMessages:
        break;
    case protocol::request::Type::Register:
        request_size += sizeof(request::Registration);
        break;
    case protocol::request::Type::SendMessage:
        request_size += (sizeof(request::Message) + additional_data);
        break;
    case protocol::request::Type::GetPublicKey:
        request_size += sizeof(request::PublicKeyRequest);
        break;
    default:
        throw common::MessageUException("Invalid message type");
    }

    Buffer buffer(request_size);
    _fill_header(buffer, type, request_size - sizeof(request::Header));
    return buffer;
}

void PacketBuilder::_require_uid() const
{
    if (!_state.is_registered())
    {
        throw common::MessageUException("User ID is required to create this message. Please register");
    }
}

void PacketBuilder::_fill_header(Buffer& request_buffer, request::Type code, uint32_t size) const
{
    auto& header = request_buffer.to_pod<request::Header>();
    if (_state.is_registered())
    {
        std::memcpy(header.uid, _state.self().id.data(), sizeof(header.uid));
    }

    header.version = protocol::VERSION;
    header.opcode = code;
    header.size = size;
}

Buffer PacketBuilder::registration(const std::string& user_name) const
{
    if (user_name.length() > sizeof(request::Registration::username))
    {
        throw common::MessageUException("User name cannot be longer than 255 characters");
    }

    Buffer buffer =  _get_request_buffer(request::Type::Register);
    auto& payload = buffer.to_pod<request::Registration>(sizeof(request::Header));

    std::memcpy(payload.username, user_name.c_str(), user_name.length());
    payload.username[user_name.length()] = 0;

    auto key = _state.self().rsa_encryptor->key();
    std::memcpy(payload.public_key, key.data(), sizeof(payload.public_key));

    return buffer;
}

Buffer PacketBuilder::request_public_key(const Buffer& client_uid) const
{
    _require_uid();

    Buffer buffer = _get_request_buffer(request::Type::GetPublicKey);
    auto& payload = buffer.to_pod<request::PublicKeyRequest>(sizeof(request::Header));
    std::memcpy(payload.uid, client_uid.data(), sizeof(payload.uid));

    return buffer;
}

Buffer PacketBuilder::list_users() const
{
    _require_uid();
    return _get_request_buffer(request::Type::GetUserList);
}

Buffer PacketBuilder::get_messages() const
{
    _require_uid();
    return _get_request_buffer(request::Type::GetMessages);
}


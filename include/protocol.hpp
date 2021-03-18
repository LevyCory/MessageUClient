#ifndef __messageu_protocol_hpp__
#define __messageu_protocol_hpp__

namespace protocol
{
    static constexpr uint8_t VERSION = 1;

    enum class MessageType : uint8_t
    {
        SymKeyRequest = 1,
        SymKeyResponse = 2,
        EncryptedText = 3
    };
}

// Requests are sent to the server
namespace protocol::request
{
    enum class Type : uint8_t
    {
        Register = 100,
        GetUserList = 101,
        GetPublicKey = 102,
        SendMessage = 103,
        GetMessages = 104
    };


#pragma pack(push, 1)
#pragma warning(disable: 4200)
    struct Header
    {
        uint8_t uid[16];
        uint8_t version;
        Type opcode;
        uint32_t size;
        uint8_t payload[0];
    };

    struct Message
    {
        uint8_t uid[16];
        MessageType type;
        uint32_t size;
        uint8_t payload[0];
    };

    struct Registration
    {
        uint8_t username[255];
        uint8_t public_key[160];
    };

    struct PublicKeyRequest
    {
        uint8_t uid[16];
    };

#pragma warning(default: 4200)
#pragma pack(pop)
}

// Responses are received from the server
namespace protocol::response
{
    enum class Type : uint16_t
    {
        RegistrationSuccess = 1000,
        UserList = 1001,
        PublicKeyResponse = 1002,
        MessageSentAck = 1003,
        PendingMessages = 1004,
        Error = 9000
    };

#pragma pack(push, 1)
#pragma warning(disable: 4200)
    struct Header
    {
        uint8_t version;
        Type opcode;
        uint32_t size;
        uint8_t payload[0];
    };

    struct RegistrationResponse
    {
        uint8_t client_id[16];
    };

    struct UserListEntry
    {
        uint8_t id[16];

        // By my convention, network structs must be defined using exact sizes (uint8_t, uint16_t, etc)
        // This union is syntactic sugar for reinterpret_cast<char*>(entry.name)
        union
        {
            uint8_t data[255];
            char str[255];
        } name;
    };

    struct PublicKeyResponse
    {
        uint8_t id[16];
        uint8_t public_key[160];
    };

    struct MessageEntry
    {
        uint8_t sender_id[16];
        uint32_t message_id;
        MessageType type;
        uint32_t message_size;
        uint8_t content[0];
    };
#pragma warning(default: 4200)
#pragma pack(pop)
}

#endif /* __messageu_protocol_hpp__ */

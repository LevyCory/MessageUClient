#include "client.hpp"

#include <iostream>
#include <fstream>
#include <string>

Client::Client(const fs::path& server_info, const fs::path& client_info) :
    _communicator(_read_server_config(server_info)),
    _packet_builder(_state),
    _response_handler(_state)
{
    if (fs::exists(client_info))
    {
        _state = ClientState(client_info);
    }
}

common::Buffer Client::_get_user_request(const UserInput& choice)
{
    const auto& user = _state.db().get(choice.user_name);
    switch (choice.operation)
    {
    case UserChoice::PublicKeyRequest:
        return _packet_builder.request_public_key(user.id);

    case UserChoice::SendText:
        return _packet_builder.send_message(choice.message, user, MessageType::EncryptedText);

    case UserChoice::SymmetricKeyRequest:
        return _packet_builder.send_message(""s,  user, MessageType::SymKeyRequest);

    case UserChoice::SymmetricKeyResponse:
        return _packet_builder.send_message(_state.self().aes_cipher->key().clone(),
                                            user, MessageType::SymKeyResponse);
    default:
        return {};
    }
}

common::Buffer Client::_get_request_from_choice(const UserInput& choice)
{
    switch (choice.operation)
    {
    case UserChoice::Register:
        if (_state.is_registered())
        {
            throw common::MessageUException("User already registered");
        }

        return _packet_builder.registration(choice.user_name);

    case UserChoice::UserList:
        return _packet_builder.list_users();

    case UserChoice::ReadPendingMessages:
        return _packet_builder.get_messages();

    case UserChoice::SendText:
    case UserChoice::PublicKeyRequest:
    case UserChoice::SymmetricKeyRequest:
    case UserChoice::SymmetricKeyResponse:
        return _get_user_request(choice);

    default:
        return {};
    }
}

void Client::run()
{
    _shell.greet();

    while (true)
    {
        try
        {
            auto input = _shell.interact();
            if (input.operation == UserChoice::Exit)
            {
                break;
            }

            if (input.operation == UserChoice::Register)
            {
                _state.self().name = input.user_name;
            }

            auto session = _communicator.connect();
            session.send(_get_request_from_choice(input));

            auto response = session.receive();
            _response_handler.handle(response);
        }
        catch (std::invalid_argument&)
        {
            std::cout << "Invalid argument: please enter a valid input\n";
        }
        catch (const std::exception& exception)
        {
            std::cout << exception.what() << "\n";
        }
    }
}

endpoint Client::_read_server_config(const fs::path& path)
{
    std::ifstream input(path);
    fs::path cwd = fs::current_path();
    if (!input.is_open())
    {
        throw common::MessageUException("Could not open server info file");
    }

    std::string address;
    std::getline(input, address, ':');

    // Read port as a number
    uint16_t port;
    input >> port;

    return endpoint(boost::asio::ip::address::from_string(std::move(address)), port);
}

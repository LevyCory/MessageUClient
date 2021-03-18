#include "shell.hpp"

#include <iostream>

void Shell::_print_menu()
{
    std::cout <<
        "1) Register\n"
        "2) Request for client list\n"
        "3) Request for public key\n"
        "4) Request for waiting messages\n"
        "5) Send a text message\n"
        "51) Send a request for symmetric key\n"
        "52) Send your symmetric key\n"
        "0) Exit" << std::endl;
}

bool UserInput::is_user_name_required() const
{
    switch (operation)
    {
    case UserChoice::Register:
    case UserChoice::PublicKeyRequest:
    case UserChoice::SendText:
    case UserChoice::SymmetricKeyRequest:
    case UserChoice::SymmetricKeyResponse:
        return true;
    default:
        return false;
    }
}

bool UserInput::is_message_required() const
{
    return operation == UserChoice::SendText;
}

UserChoice UserInput::_long_to_choice(long choice)
{
    UserChoice user_choice = static_cast<UserChoice>(choice);
    switch (user_choice)
    {
    case UserChoice::Exit:
    case UserChoice::Register:
    case UserChoice::UserList:
    case UserChoice::PublicKeyRequest:
    case UserChoice::ReadPendingMessages:
    case UserChoice::SendText:
    case UserChoice::SymmetricKeyRequest:
    case UserChoice::SymmetricKeyResponse:
        return user_choice;
    default:
        return UserChoice::Invalid;
    }
}

void Shell::greet()
{
    std::cout << "MessageU client at your service\n\n";
}

UserInput Shell::interact()
{
    _print_menu();

    std::string operation;
    std::getline(std::cin, operation);

    UserInput input(std::stol(operation));
    if (input.is_user_name_required())
    {
        std::cout << "Please enter user name:" << std::endl;
        std::getline(std::cin, input.user_name);
    }

    if (input.is_message_required())
    {
        std::cout << "Please enter a text message:" << std::endl;
        std::getline(std::cin, input.message);
    }

    return input;
}

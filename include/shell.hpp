#ifndef __messageu_shell_hpp__
#define __messageu_shell_hpp__

#include <string>

enum class UserChoice : uint64_t
{
    Invalid = -1,
    Exit = 0,
    Register = 1,
    UserList = 2,
    PublicKeyRequest = 3,
    ReadPendingMessages = 4,
    SendText = 5,
    SymmetricKeyRequest = 51,
    SymmetricKeyResponse = 52,
};

class UserInput final
{
public:
    UserInput() :
        operation(UserChoice::Invalid)
    {}

    explicit UserInput(long choice) :
        operation(_long_to_choice(choice))
    {}

    UserInput(const UserInput&) = delete;
    UserInput& operator= (const UserInput&) = delete;
    UserInput(UserInput&&) noexcept = default;
    UserInput& operator= (UserInput&&) noexcept = default;

    UserChoice operation;
    std::string user_name;
    std::string message;

    bool is_user_name_required() const;
    bool is_message_required() const;

private:
    static UserChoice _long_to_choice(long choice);
};

class Shell
{
public:
    static void greet();
    UserInput interact();

private:
    static void _print_menu();
};

#endif /* __messageu_shell_hpp__ */

#ifndef __messageu_client_hpp__
#define __messageu_client_hpp__

#include <string>
#include <filesystem>

#include <boost/asio.hpp>

#include "aes_cipher.hpp"
#include "rsa_cipher.hpp"
#include "shell.hpp"
#include "communicator.hpp"
#include "packet_builder.hpp"
#include "response_handler.hpp"
#include "state.hpp"
#include "common.hpp"

namespace fs = std::filesystem;
using endpoint = boost::asio::ip::tcp::endpoint;

class Client final
{
public:
    Client(const fs::path& server_info, const fs::path& client_info);
    ~Client() = default;
    Client(const Client&) = delete;
    Client& operator = (const Client&) = delete;
    Client(Client&&) noexcept = default;
    Client& operator = (Client&&) noexcept = default;

    void run();

private:
    static endpoint _read_server_config(const fs::path& path);
    common::Buffer _get_request_from_choice(const UserInput& choice);
    common::Buffer _get_user_request(const UserInput& choice);

    ClientState _state;
    Communicator _communicator;
    PacketBuilder _packet_builder;
    ResponseHandler _response_handler;
    Shell _shell;
};

#endif __messageu_client_hpp__

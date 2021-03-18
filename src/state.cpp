#include "state.hpp"

#include <fstream>

ClientState::ClientState()
{
    _self.aes_cipher = std::make_unique<AesCipher>();
    _self.rsa_decryptor = std::make_unique<RsaDecryptor>();
    _self.rsa_encryptor =
        std::make_unique<RsaEncryptor>(_self.rsa_decryptor->public_key());
}

void ClientState::load(const fs::path& file)
{
    std::ifstream input_file(file);
    if (!input_file.is_open())
    {
        throw common::MessageUException("Could not open file; State loading failed");
    }

    std::string name;
    if (!std::getline(input_file, name))
    {
        throw common::MessageUException("Could not read name from state file");
    }

    std::string hex_uid;
    if (!std::getline(input_file, hex_uid))
    {
        throw common::MessageUException("Could not read user ID from state file");
    }

    // Inefficient but only happens once
    std::string private_key;
    std::string private_key_component;
    while(std::getline(input_file, private_key_component))
    {
        private_key += private_key_component;
    }

    _self.id = common::Buffer::from_hex_string(hex_uid);
    _self.name = std::move(name);
    _self.rsa_decryptor = RsaDecryptor::from_base64(private_key);
    _self.rsa_encryptor = std::make_unique<RsaEncryptor>(_self.rsa_decryptor->public_key());
}

void ClientState::store(const fs::path& file) const
{
    std::ofstream output_file(file);
    if (!output_file.is_open())
    {
        throw common::MessageUException("Could not open file; State storing failed");
    }

    output_file <<
        _self.name << "\n" <<
        _self.id.to_hex_string() << "\n" <<
        _self.rsa_decryptor->to_base64();
}

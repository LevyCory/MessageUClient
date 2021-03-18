#include "aes_cipher.hpp"

#include <immintrin.h>
#include <cryptocpp/filters.h>

common::Buffer AesCipher::_generate_sym_key()
{
    common::Buffer key(CryptoPP::AES::DEFAULT_KEYLENGTH);
    for (size_t i = 0; i < key.size(); i += sizeof(uint32_t))
    {
        _rdrand32_step(reinterpret_cast<uint32_t*>(&key[i]));
    }

    return key;
}

AesCipher::AesCipher(common::Buffer&& sym_key) :
    _symmetric_key(std::move(sym_key)),
    _init_vector{ 0 },
    _encryption(_symmetric_key.data(), CryptoPP::AES::DEFAULT_KEYLENGTH),
    _cbc_encryption(_encryption, _init_vector.data()),
    _decryption(_symmetric_key.data(), CryptoPP::AES::DEFAULT_KEYLENGTH),
    _cbc_decryption(_decryption, _init_vector.data())
{}

AesCipher::AesCipher() :
    AesCipher(_generate_sym_key())
{}

common::Buffer AesCipher::decrypt(const common::Buffer& buffer)
{
    common::Buffer decrypted_data;
    CryptoPP::StreamTransformationFilter decryptor(
        _cbc_decryption,
        new CryptoPP::VectorSink(decrypted_data.get())
    );

    decryptor.Put(buffer.data(), buffer.size());
    decryptor.MessageEnd();

    return decrypted_data;
}

common::Buffer AesCipher::_encrypt(const uint8_t* ptr, std::size_t size)
{
    common::Buffer encrypted_data;
    CryptoPP::StreamTransformationFilter encryptor(
        _cbc_encryption,
        new CryptoPP::VectorSink(encrypted_data.get())
    );

    encryptor.Put(ptr, size);
    encryptor.MessageEnd();

    return encrypted_data;
}

common::Buffer AesCipher::encrypt(const common::Buffer& buffer)
{
    return _encrypt(buffer.data(), buffer.size());
}

common::Buffer AesCipher::encrypt(const std::string& string)
{
    return _encrypt(reinterpret_cast<const uint8_t*>(string.c_str()), string.length());
}

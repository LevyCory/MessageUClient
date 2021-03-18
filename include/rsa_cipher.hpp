#ifndef __messageu_rsa_cipher_hpp__
#define __messageu_rsa_cipher_hpp__

#include <memory>

#include <cryptocpp/rsa.h>
#include <cryptocpp/osrng.h>

#include "common.hpp"

class RsaEncryptor final
{
public:
    using uptr = std::unique_ptr<RsaEncryptor>;

    RsaEncryptor() = default;
    RsaEncryptor(const common::Buffer& public_key);
    RsaEncryptor(CryptoPP::RSA::PublicKey&& public_key);
    ~RsaEncryptor() = default;
    RsaEncryptor(const RsaEncryptor&) = delete;
    RsaEncryptor& operator= (const RsaEncryptor&) = delete;
    RsaEncryptor(RsaEncryptor&&) noexcept = default;
    RsaEncryptor& operator= (RsaEncryptor&&) noexcept = default;

    common::Buffer encrypt(const common::Buffer& buffer);
    common::Buffer encrypt(const std::string& buffer);

    common::Buffer key() const;

private:
    static CryptoPP::RSA::PublicKey _public_key_from_buffer(const common::Buffer& buffer);

    CryptoPP::RSA::PublicKey _public_key;
    CryptoPP::RSAES_OAEP_SHA_Encryptor _encryptor;
};

class RsaDecryptor
{
public:
    using uptr = std::unique_ptr<RsaDecryptor>;

    RsaDecryptor();
    ~RsaDecryptor() = default;
    RsaDecryptor(const RsaEncryptor&) = delete;
    RsaDecryptor& operator= (const RsaEncryptor&) = delete;
    RsaDecryptor(RsaDecryptor&&) noexcept = default;
    RsaDecryptor& operator= (RsaDecryptor&&) noexcept = default;

    common::Buffer decrypt(const common::Buffer& buffer);
    CryptoPP::RSA::PublicKey public_key() const;

    std::string to_base64() const;
    static uptr from_base64(const std::string& private_key);

private:
    RsaDecryptor(CryptoPP::RSA::PrivateKey&& private_key);
    static uptr _make_unique_decryptor(CryptoPP::RSA::PrivateKey&& private_key);
    static CryptoPP::RSA::PrivateKey _gen_private_key();

    CryptoPP::RSA::PrivateKey _private_key;
    CryptoPP::RSAES_OAEP_SHA_Decryptor _decryptor;
};

#endif /* __messageu_rsa_cipher_hpp__ */

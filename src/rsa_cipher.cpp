#include <cryptocpp/base64.h>

#include "rsa_cipher.hpp"


RsaEncryptor::RsaEncryptor(const common::Buffer& public_key) :
    _public_key(_public_key_from_buffer(public_key)),
    _encryptor(_public_key)
{}

RsaEncryptor::RsaEncryptor(CryptoPP::RSA::PublicKey&& public_key) :
    _public_key(std::move(public_key)),
    _encryptor(_public_key)
{}

CryptoPP::RSA::PublicKey
RsaEncryptor::_public_key_from_buffer(const common::Buffer& buffer)
{
    CryptoPP::VectorSource source(buffer.get(), true);
    CryptoPP::RSA::PublicKey public_key;
    public_key.Load(source);

    return public_key;
}

common::Buffer RsaEncryptor::key() const
{
    common::Buffer buffer;
    CryptoPP::VectorSink sink(buffer.get());
    _public_key.Save(sink);

    return buffer;
}

common::Buffer RsaEncryptor::encrypt(const common::Buffer& buffer)
{
    common::Buffer encrypted_data;
    CryptoPP::AutoSeededRandomPool pool;
    CryptoPP::VectorSource source(
        buffer.get(),
        true,
        new CryptoPP::PK_EncryptorFilter(
            pool,
            _encryptor,
            new CryptoPP::VectorSink(
                encrypted_data.get()
            )
        )
    );

    return encrypted_data;
}

common::Buffer RsaEncryptor::encrypt(const std::string& buffer)
{
    throw common::MessageUException("RSA encryptor does not support strings");
}

RsaDecryptor::RsaDecryptor() :
    _private_key(_gen_private_key()),
    _decryptor(_private_key)
{}

RsaDecryptor::RsaDecryptor(CryptoPP::RSA::PrivateKey&& private_key) :
    _private_key(std::move(private_key)),
    _decryptor(_private_key)
{}

CryptoPP::RSA::PrivateKey RsaDecryptor::_gen_private_key()
{
    CryptoPP::RSA::PrivateKey private_key;
    CryptoPP::AutoSeededRandomPool pool;
    private_key.GenerateRandomWithKeySize(pool, common::RSA_KEY_SIZE);
    return private_key;
}

CryptoPP::RSA::PublicKey RsaDecryptor::public_key() const
{
    return CryptoPP::RSA::PublicKey(_private_key);
}

common::Buffer RsaDecryptor::decrypt(const common::Buffer& buffer)
{
    common::Buffer decrypted_data;
    CryptoPP::AutoSeededRandomPool pool;
    CryptoPP::VectorSource source(
        buffer.get(),
        true,
        new CryptoPP::PK_DecryptorFilter(
            pool,
            _decryptor,
            new CryptoPP::VectorSink(
                decrypted_data.get()
            )
        )
    );

    return decrypted_data;
}

std::string RsaDecryptor::to_base64() const
{
    std::string base64key;
    CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(base64key));
    _private_key.DEREncode(encoder);
    encoder.MessageEnd();

    return base64key;
}

RsaDecryptor::uptr RsaDecryptor::_make_unique_decryptor(CryptoPP::RSA::PrivateKey&& private_key)
{
    auto* decryptor = new RsaDecryptor(std::move(private_key));
    return uptr(decryptor);
}

RsaDecryptor::uptr RsaDecryptor::from_base64(const std::string& private_key)
{
    CryptoPP::ByteQueue bytes;
    CryptoPP::StringSource source(private_key, true, new CryptoPP::Base64Decoder);
    source.TransferTo(bytes);
    bytes.MessageEnd();

    CryptoPP::RSA::PrivateKey private_key_obj;
    private_key_obj.Load(bytes);

    return _make_unique_decryptor(std::move(private_key_obj));
}

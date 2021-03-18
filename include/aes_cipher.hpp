#ifndef __messageu_aes_cipher_hpp__
#define __messageu_aes_cipher_hpp__

#include <array>

#include <cryptocpp/aes.h>
#include <cryptocpp/modes.h>

#include "common.hpp"

class AesCipher final
{
public:
	using uptr = std::unique_ptr<AesCipher>;

	AesCipher();
	AesCipher(common::Buffer&& sym_key);
	~AesCipher() = default;
	AesCipher(const AesCipher&) = delete;
	AesCipher& operator= (const AesCipher&) = delete;
	AesCipher(AesCipher&&) noexcept = default;
	AesCipher& operator= (AesCipher&&) noexcept = default;

	common::Buffer decrypt(const common::Buffer& buffer);
	common::Buffer encrypt(const common::Buffer& buffer);
	common::Buffer encrypt(const std::string& buffer);

	const common::Buffer& key() const { return _symmetric_key; }

private:
	static common::Buffer _generate_sym_key();
	common::Buffer _encrypt(const uint8_t* ptr, std::size_t size);

	common::Buffer _symmetric_key;
	std::array<uint8_t, CryptoPP::AES::BLOCKSIZE> _init_vector;

	CryptoPP::AES::Encryption _encryption;
	CryptoPP::CBC_Mode_ExternalCipher::Encryption _cbc_encryption;
	CryptoPP::AES::Decryption _decryption;
	CryptoPP::CBC_Mode_ExternalCipher::Decryption _cbc_decryption;
};

#endif /* __messageu_aes_cipher_hpp__ */

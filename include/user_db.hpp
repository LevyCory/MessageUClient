#ifndef __messageu_user_db__
#define __messageu_user_db__

#include <memory>

#include "aes_cipher.hpp"
#include "rsa_cipher.hpp"
#include "common.hpp"

struct User
{
    using sptr = std::shared_ptr<User>;

    std::string name;
    common::Buffer id;

    // These contain immovable members, so were allocating them on the heap
    AesCipher::uptr aes_cipher;
    RsaDecryptor::uptr rsa_decryptor;
    RsaEncryptor::uptr rsa_encryptor;
};

// In a real world scenario I'd rather user boost::multi_index or a real DB, 
// but this'll do for this project
class UserDB final
{
    using user_id_cache_type = std::unordered_map<common::Buffer, User::sptr, std::hash<common::Buffer>>;
    using username_cache_type = std::unordered_map<std::string, User::sptr>;

public:
    void emplace(std::string&& name, common::Buffer&& id);

    User& get(const std::string& name) { return _get(name); }
    const User& get(const std::string& name) const { return _get(name); }

    User& get(const common::Buffer& id) { return _get(id); }
    const User& get(const common::Buffer& id) const { return _get(id); }

    bool exists(const std::string& name) const;

private:
    User& _get(const common::Buffer& name) const;
    User& _get(const std::string& name) const;

    user_id_cache_type _user_id_cache;
    username_cache_type _user_name_cache;
};

#endif /* __messageu_user_db__ */

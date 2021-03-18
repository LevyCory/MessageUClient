#include "user_db.hpp"

void UserDB::emplace(std::string&& name, common::Buffer&& id)
{
    auto user = std::make_shared<User>();
    user->name = std::move(name);
    user->id = std::move(id);

    _user_name_cache[user->name] = user;
    _user_id_cache[user->id.clone()] = std::move(user);
}

User& UserDB::_get(const std::string& name) const
{
    auto iter = _user_name_cache.find(name);
    if (iter == _user_name_cache.end() || !iter->second)
    {
        throw common::MessageUException("UserDB::get - User does not exist in DB");
    }

    return *iter->second;
}

User& UserDB::_get(const common::Buffer& name) const
{
    auto iter = _user_id_cache.find(name);
    if (iter == _user_id_cache.end() || !iter->second)
    {
        throw common::MessageUException("UserDB::get - User does not exist in DB");
    }

    return *iter->second;
}


bool UserDB::exists(const std::string& name) const
{
    return _user_name_cache.find(name) != _user_name_cache.end();
}

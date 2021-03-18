#ifndef __messageu_state_hpp__
#define __messageu_state_hpp__

#include <filesystem>

#include "common.hpp"
#include "user_db.hpp"

namespace fs = std::filesystem;


class ClientState final
{
public:
    ClientState();
    explicit ClientState(const fs::path& state_file)
    {
        load(state_file);
    }

    ~ClientState() = default;
    ClientState(const ClientState&) = delete;
    ClientState& operator= (const ClientState&) = delete;
    ClientState(ClientState&&) noexcept = default;
    ClientState& operator= (ClientState&&) noexcept = default;

    void store(const fs::path& file) const;
    void load(const fs::path& file);

    bool is_registered() const { return !_self.id.empty(); }

    User& self() { return _self; }
    const User& self() const { return _self; }

    UserDB& db() { return _db; }
    const UserDB& db() const { return _db; }

private:
    User _self;
    UserDB _db;
};

#endif /* __messageu_state_hpp__ */

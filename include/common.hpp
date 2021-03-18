#ifndef __messageu_common_hpp__
#define __messageu_common_hpp__

#include <string>
#include <exception>

#include "buffer.hpp"

using namespace std::string_literals;

namespace common
{
    static constexpr std::size_t PAGE_SIZE = 4096;
    static constexpr std::size_t RSA_KEY_SIZE = 1024;
    static constexpr std::size_t USER_ID_SIZE = 16;
    static constexpr std::size_t RSA_PUBLIC_KEY_TOTAL_SIZE = 160;
    static constexpr std::size_t USER_NAME_MAX_SIZE = 255;

    static inline std::string CLIENT_INFO_FILE = ".\\me.info"s;
    static inline std::string SERVER_INFO_FILE = ".\\server.info"s;

    class MessageUException : public std::exception
    {
    public:
        MessageUException(const char* message) :
            std::exception(message)
        {}
    };

    template <typename P>
    P* advance_by(const P* ptr, std::size_t size)
    {
        // C style cast to prevent type checking
        return reinterpret_cast<P*>((std::size_t)ptr + size);
    }
}

#endif /* __messageu_common_hpp__ */

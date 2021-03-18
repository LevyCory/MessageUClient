#include "common.hpp"
#include "client.hpp"

int main()
{
    Client client(common::SERVER_INFO_FILE, common::CLIENT_INFO_FILE);
    client.run();
    return 0;
}
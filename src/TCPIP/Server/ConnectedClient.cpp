#include "ConnectedClient.hpp"

TCPIP::ConnectedClient::~ConnectedClient()
{
    auto v = rand();
    v += rand();
}

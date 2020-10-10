#include "PacketFactory.hpp"

std::map<char, flags_t> PacketFactory::cmdToFlag = {
        {'M', MULTICAST},
        {'B', BROADCAST},
        {'L', LIST_HANDLES},
        {'E', CLIENT_EXIT}
};
#pragma once

namespace TCPIP
{
    /// Just an abstraction
    class AbstractKey
    {
    public:
        AbstractKey() = default;
        AbstractKey(AbstractKey const&) = delete;
        AbstractKey(AbstractKey&& ) = default;
    };
}

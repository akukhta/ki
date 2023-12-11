#include <vector>
#include <optional>

class IQueue
{
public:
    virtual void push(std::vector<unsigned char>) = 0;
    virtual std::optional<std::vector<unsigned char>> pop() = 0;

    virtual ~IQueue(){}
};
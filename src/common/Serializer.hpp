#pragma once
#include <vector>

class Serializer
{
public:
    Serializer(std::vector<unsigned char> &buffer)
        : buffer(buffer){}

    template <typename T>
        requires std::is_fundamental_v<T>
    void serialize(T const &data)
    {
        auto dataInBytes = reinterpret_cast<unsigned char const *>(data);

        for (size_t i = 0; i < sizeof(data); i++)
        {
            buffer.push_back(dataInBytes[i]);
        }
    }

    template <typename T>
        requires std::is_fundamental_v<T>
    void deserialize(T &data)
    {
        data = *reinterpret_cast<T*>(buffer.data() + offset);
        offset += sizeof(data);
    }

    template <typename T>
    requires (std::ranges::contiguous_range<T> && std::is_fundamental_v<typename T::value_type>)
    void serialize(T const& container)
    {
        size_t elementsCount = container.size();
        serialize(elementsCount);

        for (auto const &el : container)
        {
            serialize(el);
        }
    }

    template <typename T>
    requires (std::ranges::contiguous_range<T>
              && std::is_fundamental_v<typename T::value_type>)void deserialize(T& container)
    {
        size_t elementsCount;
        deserialize(elementsCount);
        container.reserve(elementsCount);

        for (size_t i = 0; i < elementsCount; i++)
        {
            typename T::value_type element;
            deserialize(element);
            container.push_back(element);
        }
    }

private:
    std::vector<unsigned char> &buffer;
    size_t offset = 0;
};
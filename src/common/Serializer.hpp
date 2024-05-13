#pragma once
#include <span>

class Serializer
{
public:
    explicit Serializer(std::vector<unsigned char> *buffer = nullptr)
        : buffer(buffer){}

    template <typename T>
        requires std::is_fundamental_v<T>
    void serialize(T const &data)
    {
        if (buffer == nullptr)
        {
            throw std::runtime_error("Can`t serialize to the empty buffer");
        }

        auto dataInBytes = reinterpret_cast<unsigned char const *>(data);

        for (size_t i = 0; i < sizeof(data); i++)
        {
            buffer->push_back(dataInBytes[i]);
        }
    }

    template <typename T>
    requires std::is_fundamental_v<T>
    void serialize(unsigned char **externalBuffer, T const &data)
    {
        if (externalBuffer == nullptr)
        {
            throw std::runtime_error("Can`t serialize to the empty buffer");
        }

        auto dataInBytes = reinterpret_cast<unsigned char const *>(data);

        for (size_t i = 0; i < sizeof(data); i++)
        {
            *externalBuffer[i] = dataInBytes[i];
        }

        externalBuffer += sizeof(data);
    }



    template <typename T>
        requires std::is_fundamental_v<T>
    void deserialize(T &data)
    {
        data = *reinterpret_cast<T*>(buffer->data() + offset);
        offset += sizeof(data);
    }

    template <typename T>
        requires std::is_fundamental_v<T>
    void deserialize(unsigned char const *externalBuffer, T &data)
    {
        data = *reinterpret_cast<T const*>(externalBuffer);
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
    requires (std::ranges::contiguous_range<T> && std::is_fundamental_v<typename T::value_type>)
    void serialize(unsigned char *externalBuffer, T const& container)
    {
        if (externalBuffer == nullptr)
        {
            throw std::runtime_error("Buffer is nullptr");
        }

        size_t elementsCount = container.size();
        size_t innerOffset = 0;
        serialize(elementsCount);
        innerOffset += sizeof(size_t);

        for (auto const &el : container)
        {
            serialize(externalBuffer + innerOffset, el);
            innerOffset += sizeof(el);
        }
    }

    template <typename T>
    requires (std::ranges::contiguous_range<T>
              && std::is_fundamental_v<typename T::value_type>)
    void deserialize(T& container)
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

    template <typename T>
    requires (std::ranges::contiguous_range<T>
              && std::is_fundamental_v<typename T::value_type>)
    void deserialize(unsigned char const *externalBuffer, T& container)
    {
        size_t externalBufferOffset = 0;

        size_t elementsCount;
        deserialize(externalBuffer, elementsCount);
        container.reserve(elementsCount);

        externalBufferOffset += sizeof(size_t);

        for (size_t i = 0; i < elementsCount; i++)
        {
            typename T::value_type element;
            deserialize(externalBuffer + externalBufferOffset, element);
            externalBufferOffset += sizeof(element);
            container.push_back(element);
        }
    }

private:
    std::vector<unsigned char> *buffer = nullptr;

    size_t offset = 0;
};
#pragma once
#include <span>

namespace SerializerType
{
    struct InternalBuffer{};
    struct ExternalBuffer {};
    struct NoBuffer {};
}

template<typename T>
class Serializer;

template<>
class Serializer<SerializerType::NoBuffer>
{
public:

    Serializer() = delete; // No need to create an object

    template <typename T>
        requires std::is_fundamental_v<T>
    static void serialize(unsigned char *externalBuffer, T const &data)
    {
        if (externalBuffer == nullptr)
        {
            throw std::runtime_error("Can`t serialize to the empty buffer");
        }

        auto dataInBytes = reinterpret_cast<unsigned char const *>(data);

        for (size_t i = 0; i < sizeof(data); i++)
        {
            externalBuffer[i] = dataInBytes[i];
        }
    }

    template <typename T>
        requires (std::ranges::contiguous_range<T> && std::is_fundamental_v<typename T::value_type>)
    static void serialize(unsigned char *externalBuffer, T const& container)
    {
        if (externalBuffer == nullptr)
        {
            throw std::runtime_error("Buffer is nullptr");
        }
        size_t offset = 0;
        size_t elementsCount = container.size();

        serialize(externalBuffer, elementsCount);
        offset += sizeof(elementsCount);

        for (auto const &el : container)
        {
            serialize(externalBuffer + offset, el);
            offset += sizeof(el);
        }
    }

    template <typename T>
        requires std::is_fundamental_v<T>
    static void deserialize(unsigned char const *externalBuffer, T &data)
    {
        data = *reinterpret_cast<T const*>(externalBuffer);
    }

    template <typename T>
        requires (std::ranges::contiguous_range<T> && std::is_fundamental_v<typename T::value_type>)
    static void deserialize(unsigned char const *externalBuffer, T& container)
    {
        size_t offset = 0;

        size_t elementsCount;
        deserialize(externalBuffer, elementsCount);
        container.reserve(elementsCount);

        offset += sizeof(size_t);

        for (size_t i = 0; i < elementsCount; i++)
        {
            typename T::value_type element;
            deserialize(externalBuffer + offset, element);
            offset += sizeof(element);
            container.push_back(element);
        }
    }

    template <typename T>
        requires std::is_fundamental_v<T>
    static void overwrite(unsigned char *externalBuffer, size_t pos, T const &data)
    {
        if (externalBuffer == nullptr)
        {
            throw std::runtime_error("Buffer is nullptr");
        }

        auto dataInBytes = reinterpret_cast<unsigned char const *>(&data);

        for (size_t i = 0, memoryPos = pos; i < sizeof(data); i++, memoryPos++)
        {
            externalBuffer[memoryPos] = dataInBytes[i];
        }
    }
};

template<>
class Serializer<SerializerType::ExternalBuffer>
{
public:
    Serializer(std::vector<unsigned char> &buffer) : buffer(buffer) {}

    template <typename T>
        requires std::is_fundamental_v<T>
    void serialize(T const &data)
    {
        auto dataInBytes = reinterpret_cast<unsigned char const *>(&data);

        for (size_t i = 0; i < sizeof(data); i++)
        {
            buffer.push_back(dataInBytes[i]);
        }
    }

    template <typename T>
        requires std::is_fundamental_v<T>
    void overwrite(size_t pos, T const &data)
    {
        if (pos > buffer.size()|| pos + sizeof(data) > buffer.size())
        {
            throw std::runtime_error("Not enough memory to overwrite the data at the given position");
        }

        auto dataInBytes = reinterpret_cast<unsigned char const *>(&data);

        for (size_t i = 0, memoryPos = pos; i < sizeof(data); i++, memoryPos++)
        {
            buffer[memoryPos] = dataInBytes[i];
        }
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

    std::vector<unsigned char>& getBuffer()
    {
        buffer.shrink_to_fit();
        return buffer;
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

private:
    std::vector<unsigned char> &buffer;
    size_t offset = 0;
};

template<>
class Serializer<SerializerType::InternalBuffer> : public Serializer<SerializerType::ExternalBuffer>
{
public:
    Serializer(size_t bytesReserved = BUFFER_SIZE)
        :  Serializer<SerializerType::ExternalBuffer>(internalBuffer)
    {
        internalBuffer.reserve(bytesReserved);
    }


private:
    std::vector<unsigned char> internalBuffer;
};
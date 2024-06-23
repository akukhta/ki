cmake_minimum_required(VERSION 3.0)

project(ki)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(Boost 1.82.0 REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME}
		src/main.cpp
		src/common/ICopyTool.hpp
		src/common/ParallelCopyTool.hpp
		src/common/OptionsParser.hpp
		src/queue/IQueue.hpp
		src/queue/SynchronizedQueue.hpp
		src/queue/BufferedQueue.hpp
        src/queue/Buffer.hpp
		src/reader/IFileReader.hpp
		src/reader/MMapFileReader.hpp
		src/writer/IFileWriter.hpp
		src/writer/MMapFileWriter.hpp
		src/common/StopWatch.cpp
		src/common/StopWatch.h
		src/writer/BufferedFileWriter.hpp
		src/IPC/SharedMemoryManager.cpp
		src/IPC/SharedMemoryManager.hpp
		src/common/IPCTool.hpp
		src/queue/Buffer/BufferConfiguration.hpp
		src/IPC/IPCProcInfo.hpp
		src/IPC/IPCToolType.hpp
		src/IPC/SiblingProcessObserver.hpp
		src/IPC/SiblingProcessObserver.cpp
		src/TCPIP/Server/MultiFileWriter.cpp
		src/TCPIP/Server/MultiFileWriter.hpp
		src/TCPIP/Common/FileInfo.hpp
		src/queue/Buffer/BufferType.hpp
		src/common/TCPIPTool.hpp
		src/TCPIP/Common/Utiles.hpp
		src/TCPIP/Client/TCPIPClient.cpp
		src/TCPIP/Client/TCPIPClient.hpp
		src/TCPIP/Server/TCPIPServer.cpp
		src/TCPIP/Server/TCPIPServer.hpp
		src/TCPIP/Common/Buffer.hpp
		src/TCPIP/Server/IServer.hpp
		src/common/Serializer.hpp
		src/TCPIP/Client/IClient.hpp
        src/common/Logger.hpp
		src/TCPIP/Client/BufferedFileReader.h
		src/TCPIP/Request/RequestTypes.h
        src/TCPIP/Server/ConnectedClient.cpp
		src/TCPIP/Server/ConnectedClient.hpp
		src/TCPIP/Server/TCPIPQueue.hpp
		src/TCPIP/Request/RequestHandler.cpp
		src/TCPIP/Request/RequestHandler.hpp
        src/TCPIP/Request/RequestHeader.hpp
        src/TCPIP/Request/ClientRequest.hpp
        src/TCPIP/Request/ClientRequest.cpp
		src/TCPIP/Request/IRequestHandler.hpp
		src/TCPIP/Request/RequestCreator.cpp
		src/TCPIP/Request/RequestCreator.hpp
		src/TCPIP/Common/JsonSettingsParser.cpp
		src/TCPIP/Common/JsonSettingsParser.hpp
		src/common/CLIProgressBar.hpp
		src/TCPIP/Client/IClientCommunication.hpp
		src/TCPIP/Client/TCPClientCommunication.cpp
		src/TCPIP/Client/TCPClientCommunication.hpp
		src/common/FileLogger.hpp
		src/common/CLILoadIndicator.hpp
		src/TCPIP/SecureClient/SecureClient.cpp
		src/TCPIP/SecureClient/SecureClient.hpp
		src/TCPIP/Security/RSAKey.cpp
		src/TCPIP/Security/RSAKey.hpp
		src/TCPIP/Security/IEncryption.hpp
		src/TCPIP/Security/IKeyManager.hpp
		src/TCPIP/Security/AbstractKey.hpp
		src/TCPIP/Security/RSAEncryption.cpp
		src/TCPIP/Security/RSAEncryption.hpp
		src/TCPIP/Security/KeyManager.cpp
		src/TCPIP/Security/KeyManager.hpp
		src/TCPIP/Security/SecureRequestHandler.cpp
		src/TCPIP/Security/SecureRequestHandler.hpp
		src/TCPIP/Security/Chacha20Key.cpp
		src/TCPIP/Security/Chacha20Key.hpp
		src/TCPIP/Security/ChaCha20Encryption.cpp
		src/TCPIP/Security/ChaCha20Encryption.hpp
		src/TCPIP/SecureServer/SecureTCPIPServer.cpp
		src/TCPIP/SecureServer/SecureTCPIPServer.hpp
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=1)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
	target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=0)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/cryptopp/include)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options stdc++fs fmt::fmt  OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/cryptopp/lib/libcryptopp.a)
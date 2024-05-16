cmake_minimum_required(VERSION 3.0)

project(ki)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(Boost 1.82.0 REQUIRED)

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
		src/writer/MultiFileWriter.cpp
		src/writer/MultiFileWriter.hpp
		src/TCPIP/Common/FileInfo.hpp
		src/queue/Buffer/BufferType.hpp
		src/common/TCPIPTool.hpp
		src/TCPIP/Common/TCPUtiles.hpp
		src/TCPIP/Client/TCPIPClient.cpp
		src/TCPIP/Client/TCPIPClient.hpp
		src/TCPIP/Server/TCPIPServer.cpp
		src/TCPIP/Server/TCPIPServer.hpp
		src/queue/TCPIPBuffer.hpp
		src/TCPIP/Server/IServer.hpp
		src/common/Serializer.hpp
		src/TCPIP/Client/IClient.hpp
		src/common/Logger.cpp
		src/common/Logger.hpp
		src/reader/TCPIPBufferedFileReader.h
		src/TCPIP/Request/RequestTypes.h
		src/TCPIP/Request/IRequest.h
		src/TCPIP/Request/ObtainBufferRequest.hpp
		src/TCPIP/Server/ConnectedClient.cpp
		src/TCPIP/Server/ConnectedClient.hpp
		src/TCPIP/Server/TCPIPQueue.hpp
		src/TCPIP/Request/RequestHandler.cpp
		src/TCPIP/Request/RequestHandler.hpp
		src/TCPIP/Request/RequestParser.hpp
		src/TCPIP/Request/RequestHeader.hpp
		src/TCPIP/Request/FileInfoReceived.hpp
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=1)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
	target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=0)
endif()

target_link_libraries(${PROJECT_NAME} Boost::program_options stdc++fs)
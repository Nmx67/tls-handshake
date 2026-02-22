
#include <winsock2.h>
#include <ws2tcpip.h>
// #define WIN_MEAN_AND_LEAN
// #include <windows.h>

#include <iostream>
#include <cstring>

#include "win32_util.h"
#include "tcp.hpp"



TCPConnection::TCPConnection(const std::string& host, int port)
   : host(host)
   , port(port)
{}

void TCPConnection::connect() {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cout << "Error on creating socket (err=" << errno << ", " << strerror(errno) << ")" << std::endl;
        return;
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(host.c_str());
    sa.sin_port = htons(port);
    // InetPton(AF_INET, host.c_str(), &sa.sin_addr.s_addr);

    auto connect_ret = ::connect(sock, (struct sockaddr*)&sa, sizeof(sa));
    if (connect_ret) {
       int wsa_errno = WSAGetLastError();
        std::cout << "Error connecting to server (err=" << wsa_errno << ", " << strerror(wsa_errno) << ")" << " " << host << std::endl;
        return;
    }
}

void TCPConnection::close() {
#ifdef _WIN32
   if( ::closesocket(sock) != 0) {
      int wsa_errno = WSAGetLastError();
      std::cout << "Error closing socket (err=" << wsa_errno << ", " << strerror(wsa_errno) << ")" << " " << host << std::endl;

   }
#else
    ::close(sock);
#endif
}

void TCPConnection::send(const bytes_t& packet)
{
    if (::send(sock, (char *) packet.data(), packet.size(), 0) < 0) {
        std::cout << "TCPConnection(" << host << ")::send: " << "sending failed" << std::endl;
    }
}

bytes_t TCPConnection::recv() {
    const int buf_size = 4096;
    std::uint8_t buf[buf_size];
    bytes_t recv_data;
    while (true) {
        int recv_size;
        if ((recv_size = ::recv(sock, (char *) buf, buf_size, 0)) < 0) {
            perror("reading stream message");
            break;
        }
        if (recv_size == 0) {
            std::cout << "recv_size = 0" << std::endl;
            break;
        }
        recv_data += bytes_t(buf, buf + recv_size);
        if (recv_size < buf_size) {
            break;
        }
    }
    return recv_data;
}

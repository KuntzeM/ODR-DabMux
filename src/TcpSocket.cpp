/*
   Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Her Majesty the Queen in
   Right of Canada (Communications Research Center Canada)

   Copyright (C) 2016
   Matthias P. Braendli, matthias.braendli@mpb.li

    http://www.opendigitalradio.org
   */
/*
   This file is part of ODR-DabMux.

   ODR-DabMux is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   ODR-DabMux is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ODR-DabMux.  If not, see <http://www.gnu.org/licenses/>.
   */

#include "TcpSocket.h"
#include "Log.h"
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>

using namespace std;

TcpSocket::TcpSocket() :
    m_sock(INVALID_SOCKET)
{
    if ((m_sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        throw std::runtime_error("Can't create socket");
    }
}

TcpSocket::TcpSocket(int port, const string& name) :
    m_sock(INVALID_SOCKET)
{
    if ((m_sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        throw std::runtime_error("Can't create socket");
    }

    reuseopt_t reuse = 1;
    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))
            == SOCKET_ERROR) {
        throw std::runtime_error("Can't reuse address");
    }

    m_own_address.setAddress(name);
    m_own_address.setPort(port);

    if (bind(m_sock, m_own_address.getAddress(), sizeof(sockaddr_in)) == SOCKET_ERROR) {
        ::close(m_sock);
        m_sock = INVALID_SOCKET;
        throw std::runtime_error("Can't bind socket");
    }
}

TcpSocket::TcpSocket(SOCKET sock, InetAddress own, InetAddress remote) :
    m_own_address(own),
    m_remote_address(remote),
    m_sock(sock) { }

// The move constructors must ensure the moved-from
// TcpSocket won't destroy our socket handle
TcpSocket::TcpSocket(TcpSocket&& other)
{
    m_sock = other.m_sock;
    other.m_sock = INVALID_SOCKET;

    m_own_address = other.m_own_address;
    m_remote_address = other.m_remote_address;
}

TcpSocket& TcpSocket::operator=(TcpSocket&& other)
{
    m_sock = other.m_sock;
    other.m_sock = INVALID_SOCKET;

    m_own_address = other.m_own_address;
    m_remote_address = other.m_remote_address;
    return *this;
}

/**
 *  Close the underlying socket.
 *  @return 0  if ok
 *          -1 if error
 */
int TcpSocket::close()
{
    if (m_sock != INVALID_SOCKET) {
        int res = ::close(m_sock);
        if (res != 0) {
            setInetError("Can't close socket");
            return -1;
        }
        m_sock = INVALID_SOCKET;
    }
    return 0;
}

TcpSocket::~TcpSocket()
{
    close();
}

ssize_t TcpSocket::recv(void* data, size_t size)
{
    ssize_t ret = ::recv(m_sock, (char*)data, size, 0);
    if (ret == SOCKET_ERROR) {
        stringstream ss;
        ss << "TCP Socket recv error: " << strerror(errno);
        throw std::runtime_error(ss.str());
    }
    return ret;
}


ssize_t TcpSocket::send(const void* data, size_t size)
{
    /* Without MSG_NOSIGNAL the process would receive a SIGPIPE and die */
    ssize_t ret = ::send(m_sock, (const char*)data, size, MSG_NOSIGNAL);

    if (ret == SOCKET_ERROR) {
        stringstream ss;
        ss << "TCP Socket send error: " << strerror(errno);
        throw std::runtime_error(ss.str());
    }
    return ret;
}

void TcpSocket::listen()
{
    if (::listen(m_sock, 1) == SOCKET_ERROR) {
        stringstream ss;
        ss << "TCP Socket listen error: " << strerror(errno);
        throw std::runtime_error(ss.str());
    }
}

TcpSocket TcpSocket::accept()
{
    InetAddress remote_addr;
    socklen_t addrLen = sizeof(sockaddr_in);

    SOCKET socket = ::accept(m_sock, remote_addr.getAddress(), &addrLen);
    if (socket == SOCKET_ERROR) {
        stringstream ss;
        ss << "TCP Socket accept error: " << strerror(errno);
        throw std::runtime_error(ss.str());
    }
    else {
        TcpSocket client(socket, m_own_address, remote_addr);
        return client;
    }
}

boost::optional<TcpSocket> TcpSocket::accept(int timeout_ms)
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(m_sock, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 1000ul * timeout_ms;

    retval = select(1, &rfds, NULL, NULL, &tv);

    if (retval == -1) {
        stringstream ss;
        ss << "TCP Socket accept error: " << strerror(errno);
        throw std::runtime_error(ss.str());
    }
    else if (retval) {
        return accept();
    }
    else {
        return boost::none;
    }
}


InetAddress TcpSocket::getOwnAddress() const
{
    return m_own_address;
}

InetAddress TcpSocket::getRemoteAddress() const
{
    return m_remote_address;
}

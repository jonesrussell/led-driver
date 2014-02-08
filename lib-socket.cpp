// LED Driver
// Copyright (C) 2014 Darryl Sokoloski <darryl@sokoloski.ca>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <stdexcept>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

#include "lib-socket.h"

libSocketAbstract::libSocketAbstract()
    : sd(-1)
{
}

libSocketAbstract::~libSocketAbstract()
{
    if (sd > -1) close(sd);
}

ssize_t libSocketAbstract::Read(unsigned char *buffer, ssize_t length)
{
    ssize_t bytes;
    if ((bytes = read(sd, buffer, length)) != length)
        std::cerr << "error reading from socket: " << strerror(errno) << std::endl;
    return bytes;
}

ssize_t libSocketAbstract::Write(unsigned char *buffer, ssize_t length)
{
    ssize_t bytes;
    if ((bytes = write(sd, buffer, length)) != length)
        std::cerr << "error writing to socket: " << strerror(errno) << std::endl;
    return bytes;
}

int libSocketAbstract::SetReadTimeout(int seconds)
{
    int rc;
    struct timeval tv;

    tv.tv_usec = 0;
    tv.tv_sec = seconds;

    if ((rc = setsockopt(sd,
        SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval))) == -1) {
        std::cerr << __func__ << ": error setting socket read time-out: "
            << strerror(errno) << std::endl;
    }

    return rc;
}

libSocketClient::libSocketClient(const char *node, const char *service)
    : libSocketAbstract()
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_V4MAPPED | AI_ALL;
    hints.ai_protocol = 0;

    int rc = getaddrinfo(node, service, &hints, &result);
    if (rc != 0) {
        std::cerr << __func__ << ": getaddrinfo: " << gai_strerror(rc) << std::endl;
        throw std::runtime_error("getaddrinfo");
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sd == -1) continue;
        if (connect(sd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(sd);
    }

    if (result != NULL) freeaddrinfo(result);

    if (rp == NULL) {
        std::cerr << __func__ << ": unable to connect" << std::endl;
        throw std::runtime_error("unable to connect");
    }
}

libSocketServer::libSocketServer(const char *service)
    : libSocketAbstract()
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_V4MAPPED | AI_ALL;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    int rc = getaddrinfo(NULL, service, &hints, &result);
    if (rc != 0) {
        std::cerr << __func__ << ": getaddrinfo: " << gai_strerror(rc) << std::endl;
        throw std::runtime_error("getaddrinfo");
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sd == -1) continue;
        if (bind(sd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;

        close(sd);
    }

    if (result != NULL) freeaddrinfo(result);

    if (rp == NULL) {
        std::cerr << __func__ << ": unable to bind" << std::endl;
        throw std::runtime_error("unable to bind");
    }

    fcntl(sd, F_SETOWN, getpid());
    fcntl(sd, F_SETFL, FASYNC);
}

ssize_t libSocketServer::Read(unsigned char *buffer, ssize_t length)
{
    ssize_t bytes;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);

    bytes = recvfrom(sd, buffer, length, 0,
        (struct sockaddr *)&peer_addr, &peer_addr_len);

    if (bytes != length)
        std::cerr << __func__ << ": error reading" << std::endl;
    else {
#if 0
        char host[NI_MAXHOST], service[NI_MAXSERV];
        int rc = getnameinfo((struct sockaddr *)&peer_addr,
            peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
        if (rc == 0) {
            std::cerr << __func__ << ": read " << bytes << " bytes from: "
                << host << ":" << service << std::endl;
        }
#endif
    }

    return bytes;
}

ssize_t libSocketServer::Write(unsigned char *buffer, ssize_t length)
{
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);

    ssize_t bytes = sendto(sd, buffer, length, 0,
        (struct sockaddr *)&peer_addr, peer_addr_len);

    if (bytes != length)
        std::cerr << __func__ << ": error writing" << std::endl;
    else {
#if 0
        char host[NI_MAXHOST], service[NI_MAXSERV];
        int rc = getnameinfo((struct sockaddr *)&peer_addr,
            peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
        if (rc == 0) {
            std::cerr << __func__ << ": wrote " << bytes << " bytes to: "
                << host << ":" << service << std::endl;
        }
#endif
    }

    return bytes;
}

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4

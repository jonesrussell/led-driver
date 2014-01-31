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

#ifndef _LIB_SOCKET_H
#define _LIB_SOCKET_H

enum libSocketOpCode
{
    LSOP_SET_LEDS_OFF = 1,
    LSOP_SET_ALERT,
    LSOP_GET_ALERTS,
    LSOP_SET_MOODLIGHT,
    LSOP_GET_MOODLIGHTS,

    LSOP_OK = 0xF0,
    LSOP_EOF = 0xF1,
    LSOP_NOP = 0xF2,
    LSOP_ERROR = 0xF3,
};

#define LSPKT_MAX_LENGTH    1024
#define LSPKT_MAX_DATASIZE  (LSPKT_MAX_LENGTH - 1)

struct libSocketPacket
{
    unsigned char opcode;
    unsigned char data[LSPKT_MAX_DATASIZE];
};

class libSocketAbstract
{
public:
    libSocketAbstract();
    virtual ~libSocketAbstract();

    virtual ssize_t Read(unsigned char *buffer, ssize_t length);
    virtual ssize_t Write(unsigned char *buffer, ssize_t length);

    int SetReadTimeout(int seconds);

protected:

    int sd;
};

class libSocketClient : public libSocketAbstract
{
public:
    libSocketClient(const char *node, const char *service);

protected:
};

class libSocketServer : public libSocketAbstract
{
public:
    libSocketServer(const char *service);

    virtual ssize_t Read(unsigned char *buffer, ssize_t length);
    virtual ssize_t Write(unsigned char *buffer, ssize_t length);

protected:
    struct sockaddr_storage peer_addr;
};

#endif // _LIB_SOCKET_H

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4

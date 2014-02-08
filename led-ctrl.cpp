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

#include <locale>
#include <iostream>
#include <vector>

#include <sys/socket.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "hidapi.h"

#include "lib-socket.h"

#include "ld-color.h"
#include "ld-abstract.h"
#include "ld-lightpack.h"

static void usage(int rc)
{
    std::cerr << "led-ctrl <options>" << std::endl;
    std::cerr << "  -n,--node <node>" << std::endl;
    std::cerr << "    Specify node (host) address." << std::endl;
    std::cerr << "  -s,--service <service>" << std::endl;
    std::cerr << "    Specify service name/number (default: 3500)." << std::endl;
    std::cerr << "  -f,--off" << std::endl;
    std::cerr << "    Turn off all LEDs." << std::endl;
    std::cerr << "  -a,--alert <alert>" << std::endl;
    std::cerr << "    Display alert." << std::endl;
    std::cerr << "  -A,--get-alerts" << std::endl;
    std::cerr << "    List available alert types." << std::endl;
    std::cerr << "  -m,--moodlight [<moodlight>]" << std::endl;
    std::cerr << "    Start moodlight." << std::endl;
    std::cerr << "  -M,--get-moodlights" << std::endl;
    std::cerr << "    List available moodlight types." << std::endl;
    std::cerr << "  -d,--debug" << std::endl;
    std::cerr << "    Enable debug mode." << std::endl;
    std::cerr << "  -v,--version" << std::endl;
    std::cerr << "    Display version and exit." << std::endl;

    exit(rc);
}

int main(int argc, char *argv[])
{
    int rc, debug = 0;
    const char *node = "localhost", *service = "3500";
    struct libSocketPacket pkt;

    pkt.opcode = LSOP_NOP;

    static struct option options[] =
    {
        { "off", no_argument, 0, 'f' },
        { "alert", required_argument, 0, 'a' },
        { "get-alerts", no_argument, 0, 'A' },
        { "moodlight", optional_argument, 0, 'm' },
        { "get-moodlights", no_argument, 0, 'M' },
        { "node", required_argument, 0, 'n' },
        { "service", required_argument, 0, 's' },
        { "debug", no_argument, 0, 'd' },
        { "version", no_argument, 0, 'V' },
        { "help", no_argument, 0, 'h' },

        { NULL, 0, 0, 0 }
    };

    for (optind = 1;; ) {
        int o = 0;
        if ((rc = getopt_long(argc, argv,
            "fa:Am::Mn:s:dVh?", options, &o)) == -1) break;
        switch (rc) {
        case 'f':
            if (pkt.opcode != LSOP_NOP) {
                std::cerr << "Operation already set." << std::endl;
                return 1;
            }
            pkt.opcode = LSOP_SET_LEDS_OFF;
            break;
        case 'a':
            if (pkt.opcode != LSOP_NOP) {
                std::cerr << "Operation already set." << std::endl;
                return 1;
            }
            pkt.opcode = LSOP_SET_ALERT;
            strncpy((char *)pkt.data, optarg, LSPKT_MAX_DATASIZE);
            pkt.data[LSPKT_MAX_DATASIZE - 1] = 0;
            break;
        case 'A':
            if (pkt.opcode != LSOP_NOP) {
                std::cerr << "Operation already set." << std::endl;
                return 1;
            }
            pkt.opcode = LSOP_GET_ALERTS;
            break;
        case 'm':
            if (pkt.opcode != LSOP_NOP) {
                std::cerr << "Operation already set." << std::endl;
                return 1;
            }
            pkt.opcode = LSOP_SET_MOODLIGHT;
            if (optarg == NULL)
                strcpy((char *)pkt.data, "default");
            else {
                strncpy((char *)pkt.data, optarg, LSPKT_MAX_DATASIZE);
                pkt.data[LSPKT_MAX_DATASIZE - 1] = 0;
            }
            break;
        case 'M':
            if (pkt.opcode != LSOP_NOP) {
                std::cerr << "Operation already set." << std::endl;
                return 1;
            }
            pkt.opcode = LSOP_GET_MOODLIGHTS;
            break;
        case 'n':
            node = optarg;
            break;
        case 's':
            service = optarg;
            break;
        case 'd':
            debug = 1;
            break;
        case 'V':
            break;
        case '?':
            std::cerr << "Try --help for more information." << std::endl;
            return 1;
        case 'h':
            usage(0);
        }
    }

    if (pkt.opcode == LSOP_NOP) {
        std::cerr << "No operation set." << std::endl;
        return 1;
    }

    libSocketClient skt_client(node, service);
    skt_client.SetReadTimeout(1);

    ssize_t bytes = skt_client.Write((unsigned char *)&pkt, LSPKT_MAX_LENGTH);

    for ( ; bytes == LSPKT_MAX_LENGTH; ) {
        bytes = skt_client.Read((unsigned char *)&pkt, LSPKT_MAX_LENGTH);
        if (bytes != LSPKT_MAX_LENGTH) return 1;

        switch (pkt.opcode) {
        case LSOP_OK:
            if (debug) std::cerr << "Operation successful." << std::endl;
            return 0;
        case LSOP_ERROR:
            std::cerr << "Operation failed." << std::endl;
            return 1;
        case LSOP_EOF:
            if (debug) std::cerr << "Operation successful, EOF." << std::endl;
            return 0;
        case LSOP_GET_ALERTS:
            std::cerr << (const char *)pkt.data << std::endl;
            break;
        case LSOP_GET_MOODLIGHTS:
            std::cerr << (const char *)pkt.data << std::endl;
            break;
        default:
            std::cerr << "Unexpected opcode." << std::endl;
        }
    }

    return 0;
}

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4

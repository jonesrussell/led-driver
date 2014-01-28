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

#ifndef _LD_LIGHTPACK_H
#define _LD_LIGHTPACK_H

#define LP_LEDS         10

#define LP_USB_VID      0x1D50
#define LP_USB_PID      0x6022

// 0: Report ID, 1 - 65 (64 bytes): Data
#define LP_BUFFER_SIZE  65

enum LightPackBufferIndex
{
    LP_INDEX_RID,
    LP_INDEX_OPCODE,
    LP_INDEX_DATA,
};

enum LightPackOpCode
{
    LPOP_LED_UPDATE = 1,
    LPOP_SET_LEDS_OFF,
    LPOP_SET_TIMER_OPTIONS,
    LPOP_SET_PWM_LEVEL_MAX_VALUE,
    LPOP_SET_SMOOTH_SLOWDOWN,
    LPOP_SET_BRIGHTNESS,

    LPOP_NOP = 0x0F
};

enum LightPackFirmwareVersion
{
    LP_FW_VER_MAJOR = 1,
    LP_FW_VER_MINOR,
};

class LedDriverLightpack : public LedDriverAbstract
{
public:

    LedDriverLightpack();
    virtual ~LedDriverLightpack();

    virtual void Open(void);
    virtual void Close(void);

    virtual void SetLedsOff(void);
    virtual void SetLedColors(LedColors &colors);

    virtual void SetOption(const char *option, int value);

protected:

    bool Read(void);
    bool Write(int opcode);

    hid_device *dh;
    unsigned char *buffer_read;
    unsigned char *buffer_write;
};

#endif // _LD_LIGHTPACK_H

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4

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

#ifndef _LD_COLOR_H
#define _LD_COLOR_H

struct LedColor
{
    unsigned r;
    unsigned g;
    unsigned b;
};

typedef std::vector<struct LedColor> LedColors;

#endif // _LD_COLOR_H

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4

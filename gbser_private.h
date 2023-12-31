/*
    Serial interface - private header for gbser*.c

    Copyright (C) 2006-2014  Robert Lipe, robertlipe+source@gpsbabel.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 */
#ifndef GBSER_PRIVATE_H_
#define GBSER_PRIVATE_H_

#define MYMAGIC 0x91827364
#define BUFSIZE 512

[[gnu::format(printf, 2, 3)]] void gbser_db(int l, const char* msg, ...);
int gbser_fill_buffer(void* handle, unsigned want, unsigned* ms);
unsigned gbser_read_buffer(void* handle, void** buf, unsigned* len);
#endif // GBSER_PRIVATE_H_

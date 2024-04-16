# pyOCD debugger
# Copyright (c) 2023 Himax Technologies, Inc
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

FLASH_ALGO = {
    'load_address' : 0x00000000,

    # Flash algorithm as a hex string
    'instructions': [
    0xe7fdbe00,
    0x2130b086, 0x00bbf242, 0x1101f2c5, 0xf2c0b672, 0x2205208f, 0x0c30f841, 0x3000f640, 0x3007f2c5,
    0x20006002, 0x7280f44f, 0x92009001, 0x9204680a, 0xb3e29a00, 0x5060f04f, 0x3cfff04f, 0xbf00e002,
    0xb39a9a00, 0xcb04f840, 0x3b049b00, 0x9b019300, 0x93013304, 0x061b9b01, 0xbf00d1f2, 0x93056a0b,
    0x0f1b9b05, 0x9b059303, 0x437ff023, 0x9b039302, 0xd0072b01, 0x2b039b03, 0x9b02d004, 0x3a049a01,
    0xd1eb4293, 0x2a019a03, 0x9a03d00c, 0xd0092a03, 0x2c28f851, 0x3c2cf851, 0xd103429a, 0x9204680a,
    0xbf00e7ce, 0xb0062001, 0xbf004770, 0xb0062000, 0x00004770, 0x2000b672, 0x00004770, 0x0048f640,
    0xf2c5b672, 0x22003007, 0x21016801, 0x1201f2c5, 0x68416001, 0x0103f021, 0x21046041, 0x12b8f8c0,
    0x018bf242, 0x218ff2c0, 0x7080f101, 0x20006010, 0x47706011, 0xb084b510, 0x3c00f640, 0x0e30f240,
    0x3c07f2c5, 0xf2c52305, 0xb6721e01, 0x3000f8cc, 0x3000f8de, 0xb3a99302, 0x03fff020, 0x5c60f100,
    0xbf00e006, 0x3000f8de, 0x39049302, 0xd0294603, 0x4b04f852, 0xf0101d18, 0xf84c0ffc, 0xd1f44b04,
    0x4020f8de, 0x9c039403, 0x94010f24, 0xf0249c03, 0x9400447f, 0x2c019c01, 0x9c01d005, 0xd0022c03,
    0x429c9c00, 0x9b01d1ec, 0xd0082b01, 0x2b039b01, 0xf85ed005, 0xf85e3c28, 0x42a34c2c, 0x2001d0d2,
    0xbd10b004, 0xb0042000, 0x0000bd10, 0xf640b672, 0xf2c53100, 0x22003107, 0x600a2000, 0x00004770,
    0x47706800, 0x47706001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
    ],

    # Relative function addresses
    'pc_init': 0x000000c1,
    'pc_unInit': 0x00000191,
    'pc_program_page': 0x000000f9,
    'pc_erase_sector': 0x000000b9,
    'pc_eraseAll': 0x00000005,

    'static_base' : 0x00000000 + 0x00000004 + 0x000001a8,
    'begin_stack' : 0x000031c0,
    'end_stack' : 0x000021c0,
    'begin_data' : 0x00000000 + 0x1000,
    'page_size' : 0x1000,
    'analyzer_supported' : False,
    'analyzer_address' : 0x00000000,
    # Enable double buffering
    'page_buffers' : [
        0x000001c0,
        0x000011c0
    ],
    'min_program_length' : 0x1000,

    # Relative region addresses and sizes
    'ro_start': 0x4,
    'ro_size': 0x1a8,
    'rw_start': 0x1ac,
    'rw_size': 0x4,
    'zi_start': 0x1b0,
    'zi_size': 0x10,

    # Flash information
    'flash_start': 0x0,
    'flash_size': 0x40000,
    'sector_sizes': (
        (0x0, 0x1000),
    )
}

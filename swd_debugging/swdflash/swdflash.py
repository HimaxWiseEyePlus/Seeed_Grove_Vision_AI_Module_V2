#!/usr/bin/env python

# 20230513 Update version 1.0.3
#          Add set core1 state reset after target.reset_and_halt()
#          Rename ISP_AHB_SETTING to ISP_AHB_SETTING_SECTOR_ERASE
#          Fix __wait_halt error read STACK_CANARY
#          Add SAU, ITGU_LUT, DTGU_LUT after target.reset_and_halt()
#          Add RAM_START_ADDRESS

# 20230718 Update version 1.0.4, add FlashAlgo_sck_equ_hclk_div8, FlashAlgo_sck_equ_hclk_div16
# 20230731 Update version 1.0.5, update FlashAlgo_sck_equ_hclk_div8, FlashAlgo_sck_equ_hclk_div16. remove FlashAlgo_sck_equ_hclk
# 20230731 add SCU_ISP_PPC
# 20230803 add FlashAlgo_sck_equ_hclk_div2, FlashAlgo_sck_equ_hclk_div4
# 20230807 fixed __wait_halt for fixed operation=erase_all --method=FlashAlgoDB,FlashAlgo
# 20230807 modified self.progressbar.update for operation=erase_all --method=Direct
# 20230807 Update version 1.0.6

import os
import sys
import timeit
import time
import signal
import argparse
import logging
import _version

from pyocd.core.helpers import ConnectHelper
from pyocd.utility.timeout import Timeout
from pyocd.core import exceptions
from pyocd.core.target import Target
from typing import Sequence
import colorama

SCK_EQU_HCLK_DIV  = 16
RAM_START_ADDRESS = 0x10000000
dump_instructions = False

LOG = logging.getLogger(__name__)
logging.basicConfig(level=logging.CRITICAL)

DEFAULT_FLASH_ADDRESS = 0x000000
DEFAULT_DUMP_SIZE     = 0x1000
SECTOR_ERASE_SIZE     = 4096
PAGE_SIZE             = 256

class Operation:
    AUTO = "auto"
    DUMP = "dump"
    ERASE_ONLY = "erase_only"
    ERASE_ALL = "erase_all"

class Method:
    FLASH_ALGO_DB = "FlashAlgoDB"
    FLASH_ALGO = "FlashAlgo"
    DIRECT = "Direct"

class Device:
    W25Q128JW_4096 = "W25Q128JW_4096"
    W25Q128JW_8192 = "W25Q128JW_8192"

if SCK_EQU_HCLK_DIV <= 2:
    from FlashAlgo_sck_equ_hclk_div2 import W25Q128JW_4096
    from FlashAlgo_sck_equ_hclk_div2 import W25Q128JW_8192
    ISP_AHB_SETTING_SECTOR_ERASE = 0x0288208B | 0 << 16 # [2]sclk_is_hclk 0: reference baud rate, clk-_baud1 = 0, div2
    ISP_AHB_SETTING_ERASE_ALL    = 0x028820BB | 0 << 16 # [2]sclk_is_hclk 0: reference baud rate, clk-_baud1 = 0, div2
elif SCK_EQU_HCLK_DIV <= 4:
    from FlashAlgo_sck_equ_hclk_div4 import W25Q128JW_4096
    from FlashAlgo_sck_equ_hclk_div4 import W25Q128JW_8192
    ISP_AHB_SETTING_SECTOR_ERASE = 0x0288208B | 1 << 16 # [2]sclk_is_hclk 0: reference baud rate, clk-_baud1 = 1, div4
    ISP_AHB_SETTING_ERASE_ALL    = 0x028820BB | 1 << 16 # [2]sclk_is_hclk 0: reference baud rate, clk-_baud1 = 1, div4
elif SCK_EQU_HCLK_DIV <= 8:
    from FlashAlgo_sck_equ_hclk_div8 import W25Q128JW_4096
    from FlashAlgo_sck_equ_hclk_div8 import W25Q128JW_8192
    ISP_AHB_SETTING_SECTOR_ERASE = 0x0288208B | 3 << 16 # [2]sclk_is_hclk 0: reference baud rate, clk-_baud1 = 3, div8
    ISP_AHB_SETTING_ERASE_ALL    = 0x028820BB | 3 << 16 # [2]sclk_is_hclk 0: reference baud rate, clk-_baud1 = 3, div8
else :
    from FlashAlgo_sck_equ_hclk_div16 import W25Q128JW_4096
    from FlashAlgo_sck_equ_hclk_div16 import W25Q128JW_8192
    ISP_AHB_SETTING_SECTOR_ERASE = 0x0288208B | 7 << 16 # [2]sclk_is_hclk 0: reference baud rate, clk-_baud1 = 7, div16
    ISP_AHB_SETTING_ERASE_ALL    = 0x028820BB | 7 << 16 # [2]sclk_is_hclk 0: reference baud rate, clk-_baud1 = 7, div16

# [18:16]clk-_baud1 Setting	0	1	2	3	4	  5	   6	 7
#                   ration  2	4	6	8	10	  12  14	16

TARGET_FLASH_ALGO = {
    Device.W25Q128JW_4096: W25Q128JW_4096.FLASH_ALGO,
    Device.W25Q128JW_8192: W25Q128JW_8192.FLASH_ALGO,
}

# ISP_PPDONE_OFFSET
BIT_POS_REG_CONFLICT_ADDR   = 0
BIT_SIZE_REG_CONFLICT_ADDR  = 24
BIT_POS_REG_INT_STATUS      = 28
BIT_SIZE_REG_INT_STATUS     = 4

SPI_ISP_FLASH_CTRL_ALIAS   =  0x51010000
BASE_ADDR_FLASH1_W_ALIAS   =  0x38000000
BASE_ADDR_FLASH1_R_ALIAS   =  0x3A000000
BASE_ADDR_FLASH2_ALIAS     =  0x3C000000
BASE_ADDR_APB_SCU_HSC_ALIAS = 0x53070000

ISP_CONTROL_ADDR           = SPI_ISP_FLASH_CTRL_ALIAS
ISP_MEM_IN_ADDR            = BASE_ADDR_FLASH1_R_ALIAS
ISP_MEM_OUT_ADDR           = BASE_ADDR_FLASH1_W_ALIAS
SCU_ISP_XIP_SPICACHE_ADDR  = 0x53070B00

# ISP_CONTROL_ADDR
ISP_AHB_SETTING_OFFEST     = 0x00
ISP_CRC_WOUT_OFFSET        = 0x04
ISP_CRC_ROUT_OFFSET        = 0x08
ISP_PGM_EN_OFFSET          = 0x10
ISP_PGA_CTRL_OFFSET        = 0x14
ISP_PGA_DATA_OFFSET        = 0x18
ISP_STATUS_CLR_OFFEST      = 0x30
ISP_PPDONE_OFFSET          = 0x50
ISP_PPDONE_COUNTER_OFFSET  = 0x40

SCU_ISP_XIP_SPICACHE_OFF           = 0x00000000
SCU_ISP_XIP_SPICACHE_ISP_WRITE_EN  = 0x00000001
SCU_ISP_XIP_SPICACHE_XIP_EN        = 0x00000002
SCU_ISP_XIP_SPICACHE_ISP_TEST_MODE = 0x00000004

SCU_HSC_ADDR                = BASE_ADDR_APB_SCU_HSC_ALIAS
SCU_ISP_PPC_CFG_APNS_OFFSET = 0x848
SCU_ISP_PPC_CFG_INT_OFFSET  = 0x84C

options = {
    'option_defaults': {
        'frequency': 7200000,
        'reset_type': 'sw_sysresetreq',
    },
}

# LPCLinkv2
# real SWD CLK : pyOCD options 'frequency'
# 1.23MHz      : 1000000
# 1.56MHz      : 1250000
# 2MHz         : 1800000
# 2.3MHz       : 2000000
# 2.6MHz       : 2300000
# 5.5MHz       : 2500000
# 5.5MHz       : 5000000
# 6.25Mz       : 10000000

# STM32 DAPLink
# real SWD CLK : pyOCD options 'frequency'
# 0.420MHz     : 1000000
# 0.819MHz     : 1800000
# 0.943MHz     : 2000000
# 1.47MHz      : 3600000
# 1.79MHz      : 4000000
# 2MHz         : 5000000
# 2.17MHz      : 6000000
# 3.13MHz      : 7200000

def signal_handler(signum, frame):
    if signum == signal.SIGINT.value:
        global session

        if (session is not None) :
            session.close()

        sys.exit(1)

class ProgressBar:
    bar_string_fmt = "\rProgress: [{}{}] {:.2%} {}/{}"
    cnt = 0

    def __init__(self, total, bar_total=30):
        self.total = total
        self.bar_total = bar_total

    def update(self, step=1, value=None):
        total = self.total
        if (value is None):
            self.cnt += step
        else:
            self.cnt = value

        bar_cnt = (int((self.cnt/total)*self.bar_total))
        space_cnt = self.bar_total - bar_cnt

        progress = self.bar_string_fmt.format(
            "█" * bar_cnt,
            " " * space_cnt,
            self.cnt/total,
            self.cnt,
            total
        )

        print(colorama.Style.NORMAL + colorama.Fore.YELLOW + progress, end="")
        percent = self.cnt/total

        if percent >= 1:
            print(colorama.Style.RESET_ALL + colorama.Fore.RESET + "\n")
            
class FlashProgramming:
    def __init__(self, target, progressbar=None):
        self.target = target
        self.progressbar = progressbar
        self.max_program_length = PAGE_SIZE
        self.min_program_length = PAGE_SIZE

    def init(self) -> bool:
        self.target.write_memory(SCU_HSC_ADDR + SCU_ISP_PPC_CFG_APNS_OFFSET, 0 << 16 | 1 << 0)
        self.target.write_memory(SCU_HSC_ADDR + SCU_ISP_PPC_CFG_INT_OFFSET, 0 << 16 | 0 << 0)
        self.target.write_memory(SCU_ISP_XIP_SPICACHE_ADDR, SCU_ISP_XIP_SPICACHE_ISP_TEST_MODE)
        self.target.write_memory(ISP_CONTROL_ADDR + ISP_AHB_SETTING_OFFEST, ISP_AHB_SETTING_SECTOR_ERASE | 1 << 24)
        self.target.write_memory(ISP_CONTROL_ADDR + ISP_AHB_SETTING_OFFEST, ISP_AHB_SETTING_SECTOR_ERASE)
        return True

    def uninit(self) -> bool:
        self.target.write_memory(SCU_ISP_XIP_SPICACHE_ADDR, SCU_ISP_XIP_SPICACHE_OFF)
        return True

    def __progm(self, addr: int, data: Sequence[int]) -> bool:
        self.target.read_memory(ISP_CONTROL_ADDR + ISP_STATUS_CLR_OFFEST) #read for clear CRC info
        self.target.write_memory_block8(ISP_MEM_OUT_ADDR + addr, data)

        with Timeout(10) as time_out:
            while time_out.check():
                try:
                    pp_stat = self.target.read_memory(ISP_CONTROL_ADDR + ISP_PPDONE_OFFSET)
                    conflict_address = pp_stat & 0x00FFFFFF
                    #pp_count = self.target.read_memory(ISP_CONTROL_ADDR + ISP_PPDONE_COUNTER_OFFSET)
                    #pp_count = pp_count & 0x00FFFFFF

                    if (conflict_address == (0xFC|addr)):
                        break

                    #spi_intp_status = pp_stat >> BIT_POS_REG_INT_STATUS
                    #if (spi_intp_status == 3 or spi_intp_status == 2 or spi_intp_status == 1):
                    #    break

                    #if (pp_count == 1):
                    #    break
                except exceptions.TransferError:
                    pp_stat = 0
                    time.sleep(0.01)
            else:
                print(colorama.Fore.RED + f"__progm timeout! reg 0x{ISP_CONTROL_ADDR + ISP_PPDONE_OFFSET:08X} = 0x{pp_stat:08X}")
                return False
    
        crc_wout = self.target.read_memory(ISP_CONTROL_ADDR + ISP_CRC_WOUT_OFFSET)
        crc_rout = self.target.read_memory(ISP_CONTROL_ADDR + ISP_CRC_ROUT_OFFSET)
    
        if (crc_wout != crc_rout):
            print(colorama.Fore.RED + f"crc_wout 0x{crc_wout:08X} != crc_rout 0x{crc_rout:08X}")
            return False

        return True

    def __call_eraseAll(self) -> bool:
        self.target.read_memory(ISP_CONTROL_ADDR + ISP_STATUS_CLR_OFFEST) #read for clear CRC info
        page_data = [0xFF] * 256
        self.target.write_memory_block8(ISP_MEM_OUT_ADDR, page_data)
        count = 0

        with Timeout(60) as time_out:
            while time_out.check():
                try:
                    pp_stat = self.target.read_memory(ISP_CONTROL_ADDR + ISP_PPDONE_OFFSET)
                    conflict_address = pp_stat & 0x00FFFFFF

                    if (conflict_address == (0xFC)):
                        break

                    #spi_intp_status = pp_stat >> BIT_POS_REG_INT_STATUS
                    #if (spi_intp_status == 3 or spi_intp_status == 2 or spi_intp_status == 1):
                    #    break

                    time.sleep(0.01)
                except exceptions.TransferError:
                    pp_stat = 0
                    time.sleep(0.01)

                #Move to here. due to read ISP_PPDONE_OFFSET show exceptions.TransferError when ISP PP busy
                if (self.progressbar is not None):
                    if (count >= (self.progressbar.total / 2)):
                        count = 0
                    else :
                        count = count + 1

                    self.progressbar.update(value=count)
            else:
                print(colorama.Fore.RED + f"__call_eraseAll timeout! reg 0x{ISP_CONTROL_ADDR + ISP_PPDONE_OFFSET:08X} = 0x{pp_stat:08X}")
                return False

        crc_wout = self.target.read_memory(ISP_CONTROL_ADDR + ISP_CRC_WOUT_OFFSET)
        crc_rout = self.target.read_memory(ISP_CONTROL_ADDR + ISP_CRC_ROUT_OFFSET)

        if (crc_wout != crc_rout):
            print(colorama.Fore.RED + f"crc_wout 0x{crc_wout:08X} != crc_rout 0x{crc_rout:08X}")
            return False

        return True

    def flash_dump(self, addr: int, sz: int) -> bytearray:
        self.target.write_memory(SCU_ISP_XIP_SPICACHE_ADDR, SCU_ISP_XIP_SPICACHE_OFF)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x008, 0x00000000)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x000, 0x80800807)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x004, 0x00000009)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x010, 0x00000001)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x014, 0x00000004)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x0F4, 0x00104218)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x100, 0x0000006B)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x104, 0x0000006B)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x108, 0x08410462)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x10C, 0x00000001)
        self.target.write_memory(ISP_MEM_IN_ADDR + 0x008, 0x00000001)
        self.target.write_memory(SCU_ISP_XIP_SPICACHE_ADDR, SCU_ISP_XIP_SPICACHE_XIP_EN)
        data = []

        partial_read_size = sz // 128

        if (partial_read_size < SECTOR_ERASE_SIZE):
            partial_read_size = SECTOR_ERASE_SIZE

        if (self.progressbar is not None):
            self.progressbar.update(value=0)

        while (sz > 0):
            data = data + self.target.read_memory_block8(ISP_MEM_IN_ADDR + addr, partial_read_size)
            sz -= partial_read_size
            addr += partial_read_size

            if (self.progressbar is not None):
                self.progressbar.update(step=partial_read_size)
                
        if (self.progressbar is not None):
            self.progressbar.update(value=self.progressbar.total)

        return bytearray(data)

    def flash_program(self, addr: int, data: Sequence[int], enable_double_buffering: bool=False) -> bool:
        self.target.write_memory(SCU_ISP_XIP_SPICACHE_ADDR, 
                                 SCU_ISP_XIP_SPICACHE_ISP_WRITE_EN |
                                 SCU_ISP_XIP_SPICACHE_ISP_TEST_MODE)

        sz = len(data)
        offset = 0

        if (self.progressbar is not None):
            self.progressbar.update(value=0)

        while (offset < sz) :
            page_data = data[offset:offset + self.max_program_length]

            if not self.__progm(offset + addr, page_data):
                print(colorama.Fore.RED + f"flash_program failed!! addr = 0x{offset + addr:08X}")
                return False

            offset = offset + self.max_program_length

            if (self.progressbar is not None):
                self.progressbar.update(step=self.max_program_length)

        if (self.progressbar is not None):
            self.progressbar.update(value=self.progressbar.total)

        return True

    def flash_eraseAll(self) -> bool:
        self.target.write_memory(ISP_CONTROL_ADDR + ISP_AHB_SETTING_OFFEST, ISP_AHB_SETTING_ERASE_ALL)
        self.target.write_memory(SCU_ISP_XIP_SPICACHE_ADDR, 
                                 SCU_ISP_XIP_SPICACHE_ISP_WRITE_EN |
                                 SCU_ISP_XIP_SPICACHE_ISP_TEST_MODE)

        if (self.progressbar is not None):
            self.progressbar.update(value=0)

        if not self.__call_eraseAll():
            print(colorama.Fore.RED + f"flash_eraseAll failed!!")
            return False

        if (self.progressbar is not None):
            self.progressbar.update(value=self.progressbar.total)

        return True

class FlashProgrammingFlashAlgo(FlashProgramming):
    STACK_CANARY     = 0xdeadf00d
    HALT_TIMEOUT     = 10

    def __init__(self, target, progressbar=None, device=Device.W25Q128JW_4096):
        super(FlashProgrammingFlashAlgo, self).__init__(target, progressbar)
        self.flash_algo = TARGET_FLASH_ALGO[device]
        self.instructions = self.flash_algo['instructions']
        self.load_address = RAM_START_ADDRESS + self.flash_algo['load_address']
        self.pc_init = RAM_START_ADDRESS + self.flash_algo['pc_init']
        self.pc_unInit = RAM_START_ADDRESS + self.flash_algo['pc_unInit']
        self.pc_program_page = RAM_START_ADDRESS + self.flash_algo['pc_program_page']
        self.pc_erase_sector = RAM_START_ADDRESS + self.flash_algo['pc_erase_sector']
        self.pc_eraseAll = RAM_START_ADDRESS + self.flash_algo['pc_eraseAll']
        self.begin_data = RAM_START_ADDRESS + self.flash_algo['begin_data']
        self.begin_stack = RAM_START_ADDRESS + self.flash_algo['begin_stack']
        self.static_base = RAM_START_ADDRESS + self.flash_algo['static_base']
        self.page_buffers = [RAM_START_ADDRESS + self.flash_algo['page_buffers'][0], RAM_START_ADDRESS + self.flash_algo['page_buffers'][1]]
        self.end_stack = RAM_START_ADDRESS + self.flash_algo['end_stack']
        self.page_buffer = self.page_buffers[0]
        self.max_program_length = self.page_buffers[1] - self.page_buffers[0]
        self.min_program_length = self.flash_algo['min_program_length']

        assert_cond1 = self.end_stack < self.page_buffers[1]

        if (assert_cond1):
            print(colorama.Fore.RED + f"end_stack {self.end_stack:08X} < page_buffers[2] {self.page_buffers[1]:08X}")
            assert False

        assert_cond2 = (self.max_program_length % PAGE_SIZE) != 0

        if (assert_cond2):
            print(colorama.Fore.RED + f"(max_program_length % PAGE_SIZE) != 0. max_program_length = {self.min_program_length:08X}")
            assert False

        assert_cond3 = (self.min_program_length % PAGE_SIZE) != 0

        if (assert_cond3):
            print(colorama.Fore.RED + f"(min_program_length % PAGE_SIZE) != 0. min_program_length = {self.min_program_length:08X}")
            assert False

        assert_cond4 = (self.max_program_length < self.min_program_length)

        if (assert_cond4):
            print(colorama.Fore.RED + f"max_program_length < min_program_length. max,min = {self.min_program_length:08X},{self.max_program_length:08X}")
            assert False

        #assert_cond5 = (self.begin_stack > 0x7FFF)
        #
        #if (assert_cond5):
        #    print(colorama.Fore.RED + f"begin_stack >= 0x7FFF. begin_stack = {self.begin_stack:08X}")
        #    assert False

    def __call_progm(self, addr: int, program_length: int, program_buffer: int) -> None:
        reg_list = []
        data_list = []
        reg_list.append('pc')
        data_list.append(self.pc_program_page)
        reg_list.append('r0')
        data_list.append(addr)
        reg_list.append('r1')
        data_list.append(program_length)
        reg_list.append('r2')
        data_list.append(program_buffer)
        reg_list.append('lr')
        data_list.append(self.load_address + 1)
        self.target.write_core_registers_raw(reg_list, data_list)
        self.target.resume()

    def __call_eraseAll(self) -> None:
        reg_list = []
        data_list = []
        reg_list.append('pc')
        data_list.append(self.pc_eraseAll)
        reg_list.append('lr')
        data_list.append(self.load_address + 1)
        self.target.write_core_registers_raw(reg_list, data_list)
        self.target.resume()
        
    def __wait_halt(self, prefix="", halt_timeout=HALT_TIMEOUT, handle_timeout_error=True) -> bool:
        error = False
        state = Target.State.RUNNING

        with Timeout(halt_timeout) as t_o:
            while t_o.check():
                try:
                    state = self.target.get_state()
                    if state != Target.State.RUNNING:
                        break

                except exceptions.TransferError:
                    print(colorama.Fore.RED + prefix + " __wait_halt t_o.check(): exceptions.TransferError")
                    print(colorama.Fore.RED + "get_state " + str(state))
                    self.target.flush()
                    self.target.halt()
                    error = True
            else:
                if (handle_timeout_error):
                    print(colorama.Fore.RED + prefix + " __wait_halt t_o.check() else:")
                    print(colorama.Fore.RED + "get_state " + str(state))
                    self.target.halt()

                error = True

        if (handle_timeout_error):
            if state != Target.State.HALTED:
                self.target.halt()
                error = True
        else :
            if (error):
                return False
            else:
                return True

        if (error):
            print(colorama.Fore.RED + prefix + " __wait_halt failed!!")
            pc = self.target.read_core_register("pc")
            print(colorama.Fore.RED + f"pc: 0x{pc:08X}")
            lr = self.target.read_core_register("lr")
            print(colorama.Fore.RED + f"lr: 0x{lr:08X}")
            sp = self.target.read_core_register("sp")
            print(colorama.Fore.RED + f"sp: 0x{sp:08X}")
            r9 = self.target.read_core_register("r9")
            print(colorama.Fore.RED + f"r9: 0x{r9:08X}")
            r0 = self.target.read_core_register("r0")
            print(colorama.Fore.RED + f"r0: 0x{r0:08X}")
            r1 = self.target.read_core_register("r1")
            print(colorama.Fore.RED + f"r1: 0x{r1:08X}")
            r2 = self.target.read_core_register("r2")
            print(colorama.Fore.RED + f"r2: 0x{r2:08X}")
            r3 = self.target.read_core_register("r3")
            print(colorama.Fore.RED + f"r3: 0x{r3:08X}")
            canary = self.target.read_memory(self.end_stack)
            print(colorama.Fore.RED + f"end_stack: 0x{canary:08X}")
            ISP_PPDONE = self.target.read_memory(ISP_CONTROL_ADDR + ISP_PPDONE_OFFSET)
            print(colorama.Fore.RED + f"0x{ISP_CONTROL_ADDR + ISP_PPDONE_OFFSET:08X}: 0x{ISP_PPDONE:08X}")
            return False

        r0 = self.target.read_core_register("r0")

        if (r0 != 0):
            print(colorama.Fore.RED + prefix + f" __wait_halt r0: 0x{r0:08X}")
            return False

        return True

    def flash_program(self, addr: int, data: Sequence[int], enable_double_buffering: bool=False) -> bool:
        index = 0
        total_length = len(data)
        self.page_buffer = self.page_buffers[0]

        if ((total_length - index) >= self.max_program_length):
            program_length = self.max_program_length
        elif ((total_length - index) >= SECTOR_ERASE_SIZE):
            program_length = SECTOR_ERASE_SIZE
        else:
            program_length = self.min_program_length

        page_data = data[index:index + program_length]

        self.target.write_memory_block8(self.page_buffer, page_data)

        if (self.progressbar is not None):
            self.progressbar.update(value=0)

        while (index < total_length) :
            self.__call_progm(addr + index, self.max_program_length, self.page_buffer)
            index += self.max_program_length

            if (index < total_length):
                if ((total_length - index) >= self.max_program_length):
                    program_length = self.max_program_length
                elif ((total_length - index) >= SECTOR_ERASE_SIZE):
                    program_length = SECTOR_ERASE_SIZE
                else:
                    program_length = self.min_program_length

                page_data = data[index:index + program_length]

                if (enable_double_buffering):
                    if (self.page_buffer == self.page_buffers[0]):
                        self.page_buffer = self.page_buffers[1]
                    else:
                        self.page_buffer = self.page_buffers[0]

                    self.target.write_memory_block8(self.page_buffer, page_data)

            if not self.__wait_halt("__call_progm" + f" addr + index = 0x{(addr + index):08X}"):
                return False

            if ((index < total_length) and (not enable_double_buffering)):
                self.target.write_memory_block8(self.page_buffer, page_data)

            if (self.progressbar is not None):
                self.progressbar.update(step=self.max_program_length)

        if (self.progressbar is not None):
            self.progressbar.update(value=self.progressbar.total)

        return True

    def flash_eraseAll(self) -> bool:

        if (self.progressbar is not None):
            self.progressbar.update(value=0)
        
        self.__call_eraseAll()
        
        countTimout = 0
        count = 0
        ret_wait_halt = False

        while (countTimout < 6000): #60 sec
            ret_wait_halt = self.__wait_halt("__call_eraseAll", 0.01, False)
            countTimout = countTimout + 1

            if (ret_wait_halt):
                break
                
            if (self.progressbar is not None):
                if (count >= (self.progressbar.total / 2)):
                    count = 0
                else :
                    count = count + 1

                self.progressbar.update(value=count)
        
        if (not ret_wait_halt):
            ret_wait_halt = self.__wait_halt("__call_eraseAll", 0.01, True)

        if (not ret_wait_halt):
            return False

        if (self.progressbar is not None):
            self.progressbar.update(value=self.progressbar.total)

        return True

    def __call_uninit(self) -> bool:
        reg_list = []
        data_list = []
        reg_list.append('pc')
        data_list.append(self.pc_unInit)
        reg_list.append('r0')
        data_list.append(0)
        reg_list.append('lr')
        data_list.append(self.load_address + 1)
        self.target.write_core_registers_raw(reg_list, data_list)
        self.target.resume()

    def uninit(self) -> bool:
        self.__call_uninit()
        return self.__wait_halt("__call_uninit")

    def __call_init(self) -> bool:
        reg_list = []
        data_list = []
        reg_list.append('pc')
        data_list.append(self.pc_init)
        reg_list.append('r0')
        data_list.append(0)
        reg_list.append('r1')
        data_list.append(0)
        reg_list.append('r2')
        data_list.append(0)
        reg_list.append('lr')
        data_list.append(self.load_address + 1)
        self.target.write_core_registers_raw(reg_list, data_list)
        self.target.resume()

    def init(self) -> bool:
        #RamForAlgo = os.path.dirname(os.path.abspath(__file__)) + "/Flash/" + self.device + ".bin"
#
        #if not os.path.exists(RamForAlgo) :
        #    print(colorama.Fore.RED + "not exists: {}".format(RamForAlgo))
        #    return False
#
        #if not os.path.isfile(RamForAlgo) :
        #    print(colorama.Fore.RED + "not file: {}".format(RamForAlgo))
        #    return False
#
        #if not os.access(RamForAlgo, os.R_OK) :
        #    print(colorama.Fore.RED + "not readable: {}".format(RamForAlgo))
        #    return False
#
        #f = open(RamForAlgo, "rb")
        #self.inst = list(bytearray(f.read()))
        #f.close()
#
        ## Download the flash algorithm in RAM
        #ram_addr = self.load_address
        #self.target.write_memory(ram_addr, self.BLOB_HEADER)
        #ram_addr = ram_addr + self.BLOB_HEADER_SIZE
        #self.target.write_memory_block8(ram_addr, self.inst)

        # Download the flash algorithm in RAM
        self.target.write_memory_block32(self.load_address, self.instructions)

        if (dump_instructions is True) :
            rd_instructions = self.target.read_memory_block32(self.load_address, len(self.instructions))
            print(f"rd_instructions: {rd_instructions}")

        # init static base for RWPI
        self.target.write_core_register("r9", self.static_base)

        # init stack
        self.target.write_core_register("sp", self.begin_stack)
        self.target.write_memory(self.end_stack, self.STACK_CANARY)

        self.__call_init()
        return self.__wait_halt("__call_init")

def main():
    global session
    session = None
    instance = None

    print("===== start =====")
    signal.signal(signal.SIGINT, signal_handler)

    try:
        session = ConnectHelper.session_with_chosen_probe(
            blocking=True,
#            target_override="we2",
            target_override="cortex-m",
            **options)

        session.open()
        target = session.board.target
        target.reset_and_halt()

        pc = target.read_core_register("pc")
        print(f"current pc = 0x{pc:08X} after reset_and_halt")

        target.write_memory(0x56100210, 0x00000000)
        target.write_memory(0x56100D08, 0x00000001)
        target.write_memory(0xE000EDD0, 0x00000000)
        target.write_memory(0xE001E510, 0x00000000)
        target.write_memory(0xE001E610, 0x00000000)
        target.write_core_register("pc", RAM_START_ADDRESS)
        target.flush()

        binfile = args.bin
        startaddr = args.addr

        if (args.operation == Operation.DUMP):
            wf = open(binfile, "wb")

            if not os.access(binfile, os.W_OK) :
                print(colorama.Fore.RED + f"not writable: {binfile}")
                return

            size = args.dump_size
        elif (args.operation == Operation.ERASE_ALL):
            dataStream = [0xFF] * 256
            startaddr = 0
            size = len(dataStream)
        else:
            f = open(binfile, "rb")
            dataStream = list(bytearray(f.read()))
            f.close()

            if not os.path.exists(binfile) :
                print(colorama.Fore.RED + f"not exists: {binfile}")
                return

            if not os.path.isfile(binfile) :
                print(colorama.Fore.RED + f"not file: {binfile}")
                return

            if not os.access(binfile, os.R_OK) :
                print(colorama.Fore.RED + f"not readable: {binfile}")
                return

            startaddr_align_sector = (startaddr // SECTOR_ERASE_SIZE) * SECTOR_ERASE_SIZE
            dataStream = ((startaddr - startaddr_align_sector) * [0xFF]) + dataStream

            ednaddr = startaddr + len(dataStream)
            endaddr_align_sector = ((ednaddr + (SECTOR_ERASE_SIZE - 1)) // SECTOR_ERASE_SIZE) * SECTOR_ERASE_SIZE
            dataStream = dataStream + ((endaddr_align_sector - ednaddr) * [0xFF])

            startaddr = startaddr_align_sector
            size = len(dataStream)

        if (args.method == Method.FLASH_ALGO or args.method == Method.FLASH_ALGO_DB):
            instance = FlashProgrammingFlashAlgo(target, ProgressBar(size), args.device)
        else:
            instance = FlashProgramming(target, ProgressBar(size))

        if (instance.init()):
            start = timeit.default_timer()

            if (args.operation == Operation.DUMP):
                print(f"dump flash addr 0x{startaddr:08X} size 0x{size:08X}")
                dataStream = instance.flash_dump(startaddr, size)
                wf.write(dataStream)
                wf.close()
            elif (args.operation == Operation.ERASE_ALL):
                print(f"pyOCD script method select {args.method}")
                print(f"flash erase all")
                instance.flash_eraseAll()
            else :
                print(f"pyOCD script method select {args.method}")

                if (args.operation == Operation.ERASE_ONLY):
                    dataStream = [0xFF] * len(dataStream)

                print(f"program flash addr 0x{startaddr:08X} size 0x{len(dataStream):08X}")

                if (args.method == Method.FLASH_ALGO_DB):
                    instance.flash_program(startaddr, dataStream, True)
                else:
                    instance.flash_program(startaddr, dataStream, False)

            stop = timeit.default_timer()
            print(f"The elapsed time = {stop - start:f} sec")
        else:
            print(colorama.Fore.RED + "init failed!!")

    finally:
        if (instance is not None) :
            instance.uninit()

        if (session is not None) :
            session.close()

    print("===== end =====")

if __name__ == '__main__':
    print("swdflash.py version " +  _version.__version__)

    parser = argparse.ArgumentParser(description="pyOCD Script for EPII ISP Flash Programming")

    parser.add_argument("--bin",
#                        required=True, type=str,
                        type=str,
                        help="bin file path")

    parser.add_argument("--addr",
                        default=DEFAULT_FLASH_ADDRESS, type=lambda x: int(x,0),
                        help="Flash address. Default is " + hex(DEFAULT_FLASH_ADDRESS))

    parser.add_argument("--operation",
                        default=Operation.AUTO, type=str,
                        choices=[Operation.AUTO, Operation.ERASE_ONLY, Operation.ERASE_ALL, Operation.DUMP], 
                        help="Operation selection. Default is " + Operation.AUTO)

    parser.add_argument("--dump_size",
                        default=DEFAULT_DUMP_SIZE, type=lambda x: int(x,0),
                        help="Dump size for 'dump' operation. Default is " + hex(DEFAULT_DUMP_SIZE))

    parser.add_argument("--method",
                        default=Method.FLASH_ALGO_DB, type=str,
                        choices=[Method.FLASH_ALGO_DB, Method.FLASH_ALGO, Method.DIRECT], 
                        help="The pyOCD script method selection for 'auto' and 'erase_only' operation. Default is " + Method.FLASH_ALGO_DB)

    parser.add_argument("--device",
                        default=Device.W25Q128JW_4096, type=str,
                        choices=[Device.W25Q128JW_4096, Device.W25Q128JW_8192], 
                        help="Select flash device. Default is " + Device.W25Q128JW_4096)

    args = parser.parse_args()

    main()

mcudefs = {
    'atmega88': {
        'RAMEND': 0x4FF,
        'FLASHEND': 0x1FFF
    },
    'atmega168': {
        'RAMEND': 0x4FF,
        'FLASHEND': 0x3FFF
    }
}

def bootstartb(mcu, szb):
    """
    Calculates the boot loader's start address, given its size in bytes.
    @param mcu
    @param szb Size of requested bootloader section, in bytes.
    """
    return mcudefs[mcu]['FLASHEND']-szb+1

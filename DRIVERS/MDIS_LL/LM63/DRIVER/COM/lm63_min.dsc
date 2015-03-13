#************************** MDIS4 device descriptor *************************
#
# lm63_min.dsc: Descriptor for LM63
# Automatically generated by mdiswiz 1.5.012_beta-linux-5 from 13y00106.xml
# 2005-11-24
#
#****************************************************************************

lm63_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING LM63

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING smbpci_ich
    DEVICE_SLOT = U_INT32 0x0

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # SMBus bus number
    SMB_BUSNBR = U_INT32 0

    # SMBus address of LM63
    SMB_DEVADDR = U_INT32 0x98
}
# EOF

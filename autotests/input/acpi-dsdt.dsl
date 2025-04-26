/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20250404 (32-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 *
 * Disassembling to symbolic ASL+ operators
 *
 * Disassembly of C:/Users/Ich/Desktop/acpi/dsdt.dat
 *
 * Original Table Header:
 *     Signature        "DSDT"
 *     Length           0x0004F1EE (324078)
 *     Revision         0x02
 *     Checksum         0x21
 *     OEM ID           "LENOVO"
 *     OEM Table ID     "CB-01   "
 *     OEM Revision     0x00000003 (3)
 *     Compiler ID      "MSFT"
 *     Compiler Version 0x05000000 (83886080)
 */
DefinitionBlock ("", "DSDT", 2, "LENOVO", "CB-01   ", 0x00000003)
{
    /*
     * ...
     * The following methods were unresolved and many not compile properly
     * because the disassembler had to guess at the number of arguments
     * required for each:
     */
    External (_SB_.ADP1, UnknownObj)
    External (_SB_.BAT0, UnknownObj)
    External (_SB_.FPID, UnknownObj)
    External (_SB_.STOR, UnknownObj)
    External (_SB_.SPMI, UnknownObj)
    External (_SB_.PMIC, UnknownObj)
    External (_SB_.PSUB, UnknownObj)
    External (_SB_.EMUL, UnknownObj)
    External (_SB_.PEP0, UnknownObj)
	External (_SB_.TZ45._DSM, MethodObj)    // Warning: Unknown method, guessing 3 arguments
	External (BFDT, IntObj)

    Scope (\_SB)
    {
        // Random string with some escape sequences
        Name (STRI, "asdf\n\x1234\1234\\")
        // Note: Not escapes:  ^^    ^

        Name (SOID, 0x0000027B)
        Name (STOR, 0x00000000)
        Name (SIDS, "SCP_PURWA")
        Zero
        Method (PHRV, 0, Serialized)
        {
            Name (HWRN, Zero)
            HWRN = FPID /* \_SB_.FPID */
            Return (HWRN) /* \_SB_.PHRV.HWRN */
        }
		Device (UFS0)
        {
            Alias (\_SB.STOR, STOR)
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                If (((STOR == One) || (STOR == 0x03)))
                {
                    Return (0x0F)
                }
                Else
                {
                    Return (Zero)
                }
            }

            Name (_DEP, Package (One)  // _DEP: Dependencies
            {
                \_SB.PEP0
            })
            Name (_HID, "QCOM24A5")  // _HID: Hardware ID
            Alias (\_SB.PSUB, _SUB)
            Alias (\_SB.EMUL, EMUL)
            Name (_UID, Zero)  // _UID: Unique ID
            Name (_CCA, One)  // _CCA: Cache Coherency Attribute
            Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
            {
                Name (RBUF, ResourceTemplate ()
                {
                    Memory32Fixed (ReadWrite,
                        0x01D84000,         // Address Base
                        0x0001C000,         // Address Length
                        )
                    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                    {
                        0x00000129,
                    }
                })
                Return (RBUF) /* \_SB_.UFS0._CRS.RBUF */
            }

            Device (DEV0)
            {
                Method (_ADR, 0, NotSerialized)  // _ADR: Address
                {
                    Return (0x08)
                }

                Method (_RMV, 0, NotSerialized)  // _RMV: Removal Status
                {
                    Return (Zero)
                }
            }
        }

        Device (ADC1)
        {
            Name (_DEP, Package (0x02)  // _DEP: Dependencies
            {
                \_SB.SPMI,
                \_SB.PMIC
            })
            Name (_HID, "QCOM0C11")  // _HID: Hardware ID
            Name (_UID, Zero)  // _UID: Unique ID
            Alias (\_SB.PSUB, _SUB)
            Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
            {
                Name (INTB, ResourceTemplate ()
                {
                    GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullUp, 0x0000,
                        "\\_SB.PM01", 0x00, ResourceConsumer, ,
                        RawDataBuffer (0x01)  // Vendor Data
                        {
                            0x02
                        })
                        {   // Pin list
                            0x009F
                        }
                    GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullUp, 0x0000,
                        "\\_SB.PM01", 0x00, ResourceConsumer, ,
                        RawDataBuffer (0x01)  // Vendor Data
                        {
                            0x02
                        })
                        {   // Pin list
                            0x00A0
                        }
                })
                Name (NAM, Buffer (0x0A)
                {
                    "\\_SB.SPMI"
                })
                Name (VUSR, Buffer (0x0C)
                {
                    /* 0000 */  0x8E, 0x13, 0x00, 0x01, 0x00, 0xC1, 0x02, 0x00,  // ........
                    /* 0008 */  0x90, 0x01, 0x00, 0x00                           // ....
                })
                Name (VBTM, Buffer (0x0C)
                {
                    /* 0000 */  0x8E, 0x13, 0x00, 0x01, 0x00, 0xC1, 0x02, 0x00,  // ........
                    /* 0008 */  0x91, 0x01, 0x00, 0x00                           // ....
                })
                Concatenate (VUSR, NAM, Local1)
                Concatenate (VBTM, NAM, Local2)
                Concatenate (Local1, Local2, Local3)
                Concatenate (Local3, INTB, Local0)
                Return (Local0)
            }
        }

        Name (SUPP, Zero)
        Name (CTRL, Zero)
        Method (_OSC, 4, NotSerialized)  // _OSC: Operating System Capabilities
        {
            CreateDWordField (Arg3, Zero, CDW1)
            CreateDWordField (Arg3, 0x04, CDW2)
            CreateDWordField (Arg3, 0x08, CDW3)
            If ((Arg0 == ToUUID ("23a0d13a-26ab-486c-9c5f-0ffa525a575a") /* USB4 Capabilities */))
            {
                SUPP = CDW2 /* \_SB_._OSC.CDW2 */
                CTRL = CDW3 /* \_SB_._OSC.CDW3 */
                CTRL &= 0x0F
                If ((Arg1 != One))
                {
                    CDW1 |= 0x08
                }

                If ((CDW3 != CTRL))
                {
                    CDW1 |= 0x10
                }

                CDW3 = CTRL /* \_SB_.CTRL */
                Return (Arg3)
            }
            ElseIf ((Arg0 == ToUUID ("0811b06e-4a27-44f9-8d60-3cbbc22e7b48") /* Platform-wide Capabilities */))
            {
                CTRL = CDW2 /* \_SB_._OSC.CDW2 */
                If (~(CDW1 & One))
                {
                    If ((CTRL & 0x12)){}
                    If ((CTRL & 0x00080000))
                    {
                        Debug = "OS supports battery charge limiting"
                    }
                }

                If ((Arg1 != One))
                {
                    CDW1 |= 0x08
                }

                CDW2 = CTRL /* \_SB_.CTRL */
                Return (Arg3)
            }
            Else
            {
                CDW1 |= 0x04
                Return (Arg3)
            }
        }
	}
}

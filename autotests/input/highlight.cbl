*> These are equivalent.
INVOKE my-class "foo" RETURNING var
MOVE my-class::"foo" TO var *> Inline method invocation

ADD 1 TO x
ADD 1, a, b TO x ROUNDED, y, z ROUNDED

ADD a, b TO c
    ON SIZE ERROR
        DISPLAY "Error"
END-ADD

ADD a TO b
    NOT SIZE ERROR
        DISPLAY "No error"
    ON SIZE ERROR
        DISPLAY "Error"

RD  sales-report
    PAGE LIMITS 60 LINES
    FIRST DETAIL 3
    CONTROLS seller-name.

01  TYPE PAGE HEADING.
    03  COL 1                    VALUE "Sales Report".
    03  COL 74                   VALUE "Page".
    03  COL 79                   PIC Z9 SOURCE PAGE-COUNTER.

01  sales-on-day TYPE DETAIL, LINE + 1.
    03  COL 3                    VALUE "Sales on".
    03  COL 12                   PIC 99/99/9999 SOURCE sales-date.
    03  COL 21                   VALUE "were".
    03  COL 26                   PIC $$$$9.99 SOURCE sales-amount.

01  invalid-sales TYPE DETAIL, LINE + 1.
    03  COL 3                    VALUE "INVALID RECORD:".
    03  COL 19                   PIC X(34) SOURCE sales-record.

01  TYPE CONTROL HEADING seller-name, LINE + 2.
    03  COL 1                    VALUE "Seller:".
    03  COL 9                    PIC X(30) SOURCE seller-name.

PIC 99.
PIC 9(7)V99.
PIC 9(10).  *> sads
PIC 9(5)V9(2).
PIC 9(5)
PIC +++++
PIC 99/99/9(4)
PIC *(4)9.99
PIC X(3)BX(3)BX(3)

OPEN INPUT sales, OUTPUT report-out
INITIATE sales-report

PERFORM UNTIL 1 <> 1
    READ sales
        AT END
            EXIT PERFORM
    END-READ

    VALIDATE sales-record
    IF valid-record
        GENERATE sales-on-day
    ELSE
        GENERATE invalid-sales
    END-IF
END-PERFORM

TERMINATE sales-report
CLOSE sales, report-out

EVALUATE TRUE ALSO desired-speed ALSO current-speed
    WHEN lid-closed ALSO min-speed THRU max-speed ALSO LESS THAN desired-speed
        PERFORM speed-up-machine
    WHEN lid-closed ALSO min-speed THRU max-speed ALSO GREATER THAN desired-speed
        PERFORM slow-down-machine
    WHEN lid-open ALSO ANY ALSO NOT ZERO
        PERFORM emergency-stop
    WHEN OTHER
        CONTINUE
END-EVALUATE

//COBUCLG  JOB (001),'COBOL BASE TEST',                                 00010000
//             CLASS=A,MSGCLASS=A,MSGLEVEL=(1,1)                        00020000
//BASETEST EXEC COBUCLG                                                 00030000
//COB.SYSIN DD *                                                        00040000
 00000* VALIDATION OF BASE COBOL INSTALL                                00050000
 01000 IDENTIFICATION DIVISION.                                         00060000
 01100 PROGRAM-ID. 'HELLO'.                                             00070000
 02000 ENVIRONMENT DIVISION.                                            00080000
 02100 CONFIGURATION SECTION.                                           00090000
 02110 SOURCE-COMPUTER.  GNULINUX.                                      00100000
 02120 OBJECT-COMPUTER.  HERCULES.                                      00110000
 02200 SPECIAL-NAMES.                                                   00120000
 02210     CONSOLE IS CONSL.                                            00130000
 03000 DATA DIVISION.                                                   00140000
 04000 PROCEDURE DIVISION.                                              00150000
 04100 00-MAIN.                                                         00160000
 04110     DISPLAY 'HELLO, WORLD' UPON CONSL.                           00170000
 04900     STOP RUN.                                                    00180000
//LKED.SYSLIB DD DSNAME=SYS1.COBLIB,DISP=SHR                            00190000
//            DD DSNAME=SYS1.LINKLIB,DISP=SHR                           00200000
//GO.SYSPRINT DD SYSOUT=A                                               00210000
//                                                                      00220000

*> COBOL with embedded SQL.
*> Normally for extensions pco, scb and sqb, but currently integrated into COBOL
EXEC SQL
    select count(*) from abc
END-EXEC
ADD 1 TO x

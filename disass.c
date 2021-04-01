/*
** Disassembler.  Derived from Gibbs & Kusche's simulator.
*/

/************************************************************************
**                                                                      *
**      Instruction mnemonic table (used for tracing)                   *
**                                                                      *
************************************************************************/

#include "com.h"

/*
** This table contains the mnemonic strings for the 8080/Z-80 opcodes.
**
** "q" denotes a register number in bits 3 through 5 of the opcode.
**      Values are interpreted as follows:
**              Normal 8080     Normal Z-80     DD prefix       FD prefix
**      000             B               B               B               B
**      001             C               C               C               C
**      010             D               D               D               D
**      011             E               E               E               E
**      100             H               H               XH              YH
**      101             L               L               XL              YL
**      110             M            (HL)           (IX+n)          (IY+n)
**      111             A               A               A               A
**
** "r" denotes a register number in bits 0 through 2 of the opcode.
**      Values are interpreted the same as for "q" above.
**
** "b" denotes a bit number encoded into bits 3-5 of the opcode.
**
** "p" denotes a 2-bit register pair number in bits 4 and 5 of the opcode.
**      Values are interpreted as follows:
**              8080    Z-80
**      00      B       BC
**      01      D       DE
**      10      H       HL      (no DD or FD prefix)
**      10      X       IX      (with DD prefix)
**      10      Y       IY      (with FD prefix)
**      11      S       SP      (if opcode is below F0)
**      11      P       AF      (if opcode is F0 or greater)
**
** "h" is replaced by IX or IY if the opcode prefix is DD or FD respectively.
**  If the instruction is not prefixed, "h" is replaced by HL.
** "x" is replaced by X or Y if the opcode prefix is DD or FD respectively.
**
** "n" denotes an 8-bit number following the opcode.
**
** "a" denotes a 16-bit address following the opcode.
*/

/* Mnemonics for 8080 opcodes 00 through 3F */

char mnop008[] = {
        "NOP$        LXI  p,a$   STAX p$     INX  p$     " /* 00-03 */
        "INR  q$     DCR  q$     MVI  q,n$   RLC$        " /* 04-07 */
        "EXAF$       DADx p$     LDAX p$     DCX  p$     " /* 08-0B */
        "INR  q$     DCR  q$     MVI  q,n$   RRC$        " /* 0C-0F */
        "DJNZ n$     LXI  p,a$   STAX p$     INX  p$     " /* 10-13 */
        "INR  q$     DCR  q$     MVI  q,n$   RAL$        " /* 14-17 */
        "JR   n$     DADx p$     LDAX p$     DCX  p$     " /* 18-1B */
        "INR  q$     DCR  q$     MVI  q,n$   RAR$        " /* 1C-1F */
        "JRNZ n$     LXI  p,a$   ShD a$      INX  p$     " /* 20-23 */
        "INR  q$     DCR  q$     MVI  q,n$   DAA$        " /* 24-27 */
        "JRZ  n$     DADx p$     LhD a$      DCX  p$     " /* 28-2B */
        "INR  q$     DCR  q$     MVI  q,n$   CMA$        " /* 2C-2F */
        "JRNC n$     LXI  p,a$   STA  a$     INX  p$     " /* 30-33 */
        "INR  q$     DCR  q$     MVI  q,n$   STC$        " /* 34-37 */
        "JRC  n$     DADx p$     LDA  a$     DCX  p$     " /* 38-3B */
        "INR  q$     DCR  q$     MVI  q,n$   CMC$        " /* 3C-3F */
};

/* Mnemonics for Z-80 opcodes 00 through 3F */

char mnop00z[] = {
        "NOP$        LD   p,a$   LD   (p),A$ INC  p$     " /* 00-03 */
        "INC  q$     DEC  q$     LD   q,n$   RLCA$       " /* 04-07 */
        "EX   AF,AF$ ADD  h,p$   LD   A,(p)$ DEC  p$     " /* 08-0B */
        "INC  q$     DEC  q$     LD   q,n$   RRCA$       " /* 0C-0F */
        "DJNZ n$     LD   p,a$   LD   (p),A$ INC  p$     " /* 10-13 */
        "INC  q$     DEC  q$     LD   q,n$   RLA$        " /* 14-17 */
        "JR   n$     ADD  h,p$   LD   A,(p)$ DEC  p$     " /* 18-1B */
        "INC  q$     DEC  q$     LD   q,n$   RRA$        " /* 1C-1F */
        "JR   NZ,n$  LD   p,a$   LD   (a),HL$INC  p$     " /* 20-23 */
        "INC  q$     DEC  q$     LD   q,n$   DAA$        " /* 24-27 */
        "JR   Z,n$   ADD  h,p$   LD   HL,(a)$DEC  p$     " /* 28-2B */
        "INC  q$     DEC  q$     LD   q,n$   CPL$        " /* 2C-2F */
        "JR   NC,n$  LD   p,a$   LD   (a),A$ INC  p$     " /* 30-33 */
        "INC  q$     DEC  q$     LD   q,n$   SCF$        " /* 34-37 */
        "JR   C,n$   ADD  h,p$   LD   A,(a)$ DEC  p$     " /* 38-3B */
        "INC  q$     DEC  q$     LD   q,n$   CCF$        " /* 3C-3F */
};

/* Mnemonics for opcodes 40 through 7F are easy - 76 is HLT */
/*  (HALT for Z-80), and all others are MOV (LD for Z-80). */

char mnop408[] = {
        "MOV  q,r$"
};

char mnop40z[] = {
        "LD   q,r$"
};

char mnop768[] = {
        "HLT  (Svc #n)$"
};

char mnop76z[] = {
        "HALT (Svc #n)$"
};

/* Mnemonics for 8080 opcodes 80 through BF */

char mnop808[] = {
        "ADD  r$  ADC  r$  SUB  r$  SBB  r$  " /* 80-9F */
        "ANA  r$  XRA  r$  ORA  r$  CMP  r$  " /* A0-BF */
};

/* Mnemonics for Z-80 opcodes 80 through BF */

char mnop80z[] = {
        "ADD  A,r$ADC  A,r$SUB  r$  SBC  A,r$" /* 80-9F */
        "AND  r$  XOR  r$  OR   r$  CP   r$  " /* A0-BF */
};

/*
** Mnemonics for 8080 opcodes C0 through FF
**  These are interpreted by the same routine as for opcodes 00 through 3F.
*/

char mnopC08[] = {
        "RNZ$        POP  p$     JNZ  a$     JMP  a$     " /* C0-C3 */
        "CNZ  a$     PUSH p$     ADI  n$     RST  0$     " /* C4-C7 */
        "RZ$         RET$        JZ   a$     ILLEGAL$    " /* C8-CB */
        "CZ   a$     CALL a$     ACI  n$     RST  1$     " /* CC-CF */
        "RNC$        POP  p$     JNC  a$     OUT  n$     " /* D0-D3 */
        "CNC  a$     PUSH p$     SUI  n$     RST  2$     " /* D4-D7 */
        "RC$         EXX$        JC   a$     IN   n$     " /* D8-DB */
        "CC   a$     ILLEGAL$    SBI  n$     RST  3$     " /* DC-DF */
        "RPO$        POP  p$     JPO  a$     XTh$        " /* E0-E3 */
        "CPO  a$     PUSH p$     ANI  n$     RST  4$     " /* E4-E7 */
        "RPE$        PCh$        JPE  a$     XCHG$       " /* E8-EB */
        "CPE  a$     ILLEGAL$    XRI  n$     RST  5$     " /* EC-FF */
        "RP$         POP  p$     JP   a$     DI$         " /* F0-F3 */
        "CP   a$     PUSH p$     ORI  n$     RST  6$     " /* F4-F7 */
        "RM$         SPh$        JM   a$     EI$         " /* F8-FB */
        "CM   a$     ILLEGAL$    CPI  n$     RST  7$     " /* FC-FF */
};

/*
** Mnemonics for Z-80 opcodes C0 through FF
**  These are interpreted by the same routine as for opcodes 00 through 3F.
*/

char mnopC0z[] = {
        "RET  NZ$    LD   p,(SP)$JP   NZ,a$  JP   a$     " /* C0-C3 */
        "CALL NZ,a$  LD   (SP),p$ADD  A,n$   RST  0$     " /* C4-C7 */
        "RET  Z$     RET$        JP   Z,a$   ILLEGAL$    " /* C8-CB */
        "CALL Z,a$   CALL a$     ADC  A,n$   RST  8$     " /* CC-CF */
        "RET  NC$    LD   p,(SP)$JP   NC,a$  OUT  n,A$   " /* D0-D3 */
        "CALL NC,a$  LD   (SP),p$SUB  A,n$   RST  10$    " /* D4-D7 */
        "RET  C$     EXX$        JP   C,a$   IN   A,n$   " /* D8-DB */
        "CALL C,a$   ILLEGAL$    SBC  A,n$   RST  18$    " /* DC-DF */
        "RET  PO$    LD   p,(SP)$JP   PO,a$  EX   (SP),p$" /* E0-E3 */
        "CALL PO,a$  LD   (SP),p$AND  A,n$   RST  20$    " /* E4-E7 */
        "RET  PE$    JP   (p)$   JP   PE,a$  EX   DE,p$  " /* E8-EB */
        "CALL PE,a$  ILLEGAL$    XOR  A,n$   RST  28$    " /* EC-FF */
        "RET  P$     LD   p,(SP)$JP   P,a$   DI$         " /* F0-F3 */
        "CALL P,a$   LD   (SP),p$OR   A,n$   RST  30$    " /* F4-F7 */
        "RET  M$     LD   SP,h$  JP   M,a$   EI$         " /* F8-FB */
        "CALL M,a$   ILLEGAL$    CP   A,n$   RST  38$    " /* FC-FF */
};

/*
** Mnemonics for opcodes CB00 through CB3F -
**  these are the same for both the 8080 and the Z-80.
*/

char mnopCB0[] = {
        "RLC  r$ RRC  r$ RL   r$ RR   r$ " /* CB00-CB1F */
        "SLA  r$ SRA  r$ ILLEGAL$SRL  r$ " /* CB20-CB3F */
};

/*
** Mnemonics for opcodes CB40 through CBFF -
**  these are the same for both the 8080 and the Z-80.
*/

char mnopCB4[] = {
        "BIT  br$RES  br$SET  br$"
};

/*
** Mnemonics for 8080 opcodes ED40 through ED7F
**  These are interpreted by the same routine as for opcodes 00 through 3F.
*/

char mnopE48[] = {
        "IN   q,(C)$ OUT  (C),q$ DSBB p$     SBCD a$     " /* ED40-ED43 */
        "NEG$        RETN$       IM0$        MOV  I,A$   " /* ED44-ED47 */
        "IN   q,(C)$ OUT  (C),q$ DADC p$     LBCD a$     " /* ED48-ED4B */
        "ILLEGAL$    RETI$       ILLEGAL$    MOV  R,A$   " /* ED4C-ED4F */
        "IN   q,(C)$ OUT  (C),q$ DSBB p$     SDED a$     " /* ED50-ED53 */
        "ILLEGAL$    ILLEGAL$    IM1$        MOV  A,I$   " /* ED54-ED57 */
        "IN   q,(C)$ OUT  (C),q$ DADC p$     LDED a$     " /* ED58-ED5B */
        "ILLEGAL$    ILLEGAL$    IM2$        MOV  A,R$   " /* ED5C-ED5F */
        "IN   q,(C)$ OUT  (C),q$ DSBB p$     SHLD a$     " /* ED60-ED63 */
        "ILLEGAL$    ILLEGAL$    ILLEGAL$    RRD$        " /* ED64-ED67 */
        "IN   q,(C)$ OUT  (C),q$ DADC p$     LHLD a$     " /* ED68-ED6B */
        "ILLEGAL$    ILLEGAL$    ILLEGAL$    RLD$        " /* ED6C-ED6F */
        "IN   q,(C)$ OUT  (C),q$ DSBB p$     SSPD a$     " /* ED70-ED73 */
        "ILLEGAL$    ILLEGAL$    ILLEGAL$    ILLEGAL$    " /* ED74-ED77 */
        "IN   q,(C)$ OUT  (C),q$ DADC p$     LSPD a$     " /* ED78-ED7B */
        "ILLEGAL$    ILLEGAL$    ILLEGAL$    ILLEGAL$    " /* ED7C-ED7F */
};

/*
** Mnemonics for Z-80 opcodes ED40 through ED7F
**  These are interpreted by the same routine as for opcodes 00 through 3F.
*/

char mnopE4z[] = {
        "IN   q,(C)$ OUT  (C),q$ SBC  HL,p$  LD   (a),p$ " /* ED40-ED43 */
        "NEG$        RETN$       IM   0$     LD   I,A$   " /* ED44-ED47 */
        "IN   q,(C)$ OUT  (C),q$ ADC  HL,p$  LD   p,(a)$ " /* ED48-ED4B */
        "ILLEGAL$    RETI$       ILLEGAL$    LD   R,A$   " /* ED4C-ED4F */
        "IN   q,(C)$ OUT  (C),q$ SBC  HL,p$  LD   (a),p$ " /* ED50-ED53 */
        "ILLEGAL$    ILLEGAL$    IM   1$     LD   A,I$   " /* ED54-ED57 */
        "IN   q,(C)$ OUT  (C),q$ ADC  HL,p$  LD   p,(a)$ " /* ED58-ED5B */
        "ILLEGAL$    ILLEGAL$    IM   2$     LD   A,R$   " /* ED5C-ED5F */
        "IN   q,(C)$ OUT  (C),q$ SBC  HL,p$  LD   (a),p$ " /* ED60-ED63 */
        "ILLEGAL$    ILLEGAL$    ILLEGAL$    RRD$        " /* ED64-ED67 */
        "IN   q,(C)$ OUT  (C),q$ ADC  HL,p$  LD   p,(a)$ " /* ED68-ED6B */
        "ILLEGAL$    ILLEGAL$    ILLEGAL$    RLD$        " /* ED6C-ED6F */
        "IN   q,(C)$ OUT  (C),q$ SBC  HL,p$  LD   (a),p$ " /* ED70-ED73 */
        "ILLEGAL$    ILLEGAL$    ILLEGAL$    ILLEGAL$    " /* ED74-ED77 */
        "IN   q,(C)$ OUT  (C),q$ ADC  HL,p$  LD   p,(a)$ " /* ED78-ED7B */
        "ILLEGAL$    ILLEGAL$    ILLEGAL$    ILLEGAL$    " /* ED7C-ED7F */
};

/*
** Mnemonics for miscellaneous ED-prefix instructions -
**  these are the same for both the 8080 and the Z-80.
*/

char mnopEA[] = {
        "LDI$ CPI$ INI$ OUTI$"  /* EDA0-EDA3 */
        "                    "  /* EDA4-EDA7 (illegal) */
        "LDD$ CPD$ IND$ OUTD$"  /* EDA8-EDAB */
        "                    "  /* EDAC-EDAF (illegal) */
        "LDIR$CPIR$INIR$OTIR$"  /* EDB0-EDB3 */
        "                    "  /* EDB4-EDB7 (illegal) */
        "LDDR$CPDR$INDR$OTDR$"  /* EDB8-EDBB */
};

/* Mnemonic for illegal instructions */
char mnopilg[] = {
        "ILLEGAL$"
};

static char illegal2[] = {"ILLEGAL FOR CP/M"};

char *svcmnemonics[64] = {
        "WARMSTART",            /* BIOS 01 */
        "CONSTATUS",            /* BIOS 02 */
        "CONINPUT",             /* BIOS 03 */
        "CONOUTPUT",            /* BIOS 04 */
        "LISTOUT",              /* BIOS 05 */
        "PUNCH",                /* BIOS 06 */
        "READER",               /* BIOS 07 */
        "HOME",                 /* BIOS 08 */
        "SETDISK",              /* BIOS 09 */
        "SETTRACK",             /* BIOS 0A */
        "SETSECTOR",            /* BIOS 0B */
        "SETDMA",               /* BIOS 0C */
        "READDISK",             /* BIOS 0D */
        "WRITEDISK",            /* BIOS 0E */
        "LISTSTATUS",           /* BIOS 0F */
        "SECTORTRAN",           /* BIOS 10 */

        "SYSTEM RESET",         /* BDOS 00 */
        "CONSOLE INPUT",        /* BDOS 01 */
        "CONSOLE OUTPUT",       /* BDOS 02 */
        "READER INPUT",         /* BDOS 03 */
        "PUNCH OUTPUT",         /* BDOS 04 */
        "LIST OUTPUT",          /* BDOS 05 */
        "DIRECT CONSOLE I/O",   /* BDOS 06 */
        "GET IOBYTE",           /* BDOS 07 */
        "SET IOBYTE",           /* BDOS 08 */
        "PRINT STRING",         /* BDOS 09 */
        "READ CONSOLE BUFFER",  /* BDOS 0A */
        "GET CONSOLE STATUS",   /* BDOS 0B */
        "GET VERSION NUMBER",   /* BDOS 0C */
        "RESET DISK SYSTEM",    /* BDOS 0D */
        "SELECT DISK",          /* BDOS 0E */
        "OPEN FILE",            /* BDOS 0F */
        "CLOSE FILE",           /* BDOS 10 */
        "SEARCH FOR FIRST",     /* BDOS 11 */
        "SEARCH FOR NEXT",      /* BDOS 12 */
        "DELETE FILE",          /* BDOS 13 */
        "READ SEQUENTIAL",      /* BDOS 14 */
        "WRITE SEQUENTIAL",     /* BDOS 15 */
        "MAKE FILE",            /* BDOS 16 */
        "RENAME FILE",          /* BDOS 17 */
        "RETURN LOGIN VECTOR",  /* BDOS 18 */
        "RETURN CURRENT DISK",  /* BDOS 19 */
        "SET DMA ADDRESS",      /* BDOS 1A */
        "GET ALLOC ADDRESS",    /* BDOS 1B */
        "WRITE PROTECT DISK",   /* BDOS 1C */
        "GET R/O VECTOR",       /* BDOS 1D */
        "SET FILE ATTRIBUTES",  /* BDOS 1E */
        "GET DISK PARMS",       /* BDOS 1F */
        "GET/SET USER CODE",    /* BDOS 20 */
        "READ RANDOM",          /* BDOS 21 */
        "WRITE RANDOM",         /* BDOS 22 */
        "COMPUTE FILE SIZE",    /* BDOS 23 */
        "SET RANDOM RECORD",    /* BDOS 24 */
        "RESET DRIVE",          /* BDOS 25 */
        "WRITE RANDOM (ZERO)",  /* BDOS 26 */
        illegal2,
        illegal2,
        illegal2,
        illegal2,
        illegal2,
        illegal2,
        illegal2,
        illegal2,
        illegal2
};

char *
decode(ppc, zilog, operands)
unsigned char *ppc;
int zilog;                      /* TRUE for Zilog Mnemonics. */
int operands;                   /* TRUE to do operands too. */
{
    static char outbuf[40];
    unsigned char opcode, prefix;
    char *table;

    prefix = 0;
    opcode = *ppc;
    if (opcode == 0xDD || opcode == 0xFD) { /* IX/IY prefix? */
        prefix = opcode;
        opcode = ppc[1];
    }
    if (opcode < 0x40) {
        table = zilog ? mnop00z : mnop008;
        table = &table[opcode * 12];
    } else if (opcode >= 0xC0) {
        if (opcode == 0xED) {
            opcode = ppc[1];
            if (opcode < 0x40) { /* ED00-ED3F? */
                table = mnopilg; /* It's illegal. */
            } else if (opcode < 0x80) { /* ED40-ED7F? */
                table = zilog ? mnopE4z : mnopE48;
                table = &table[(opcode & 0x3F) * 12];
            } else if (opcode < 0xA0 || opcode > 0xBB) {
                table = mnopilg; /* It's illegal. */
            } else {            /* ED80-ED9F. */
                table = &mnopEA[(opcode & 0x1F) * 5];
                if (*table == ' ')
                    table = mnopilg; /* It's illegal. */
            }
        } else if (opcode == 0xCB) {
            opcode = ppc[1];
            if (prefix)         /* IX/IY with displacement? */
                opcode = ppc[3];
            if (opcode < 0x40) { /* Opcode CB00-CB3F? */
                table = &mnopCB0[(opcode & -8)];
            } else {            /* Opcode CB40-CBFF. */
                table = &mnopCB4[((opcode >> 3) & 0x18) - 8];
            }
        } else {                /* Opcode C0-FF excluding CB and ED. */
            table = zilog ? mnopC0z : mnopC08;
            table = &table[(opcode - 0xC0) * 12];
        }
    } else if (opcode >= 0x80) {
        table = zilog ? mnop80z : mnop808;
        table = &table[((opcode >> 3) & 7) * 9];
    } else {                    /* It's $40-$7F. */
        if (opcode == 0x76) {   /* Halt is a special case. */
            table = zilog ? mnop76z : mnop768;
        } else {
            table = zilog ? mnop40z : mnop408;
        }
    }
    tdecode(table, outbuf, prefix, opcode, ppc, zilog, operands);
    return outbuf;
}

void
tdecode(table, outbuf, prefix, opcode, ppc, zilog, operands)
char *table, *outbuf;
unsigned char prefix, opcode, *ppc;
int zilog, operands;
{
    char key, *cp;
    int reg;

    while ((key = *table++) != '$') {
        switch (key) {
          case 'b':             /* Bit operand. */
            *outbuf++ = ((opcode & 0x38) >> 3) + '0';
            key = ',';
            break;

          case 'r':             /* Register. */
            reg = opcode & 7;
          areg:
            key = "BCDEHLMA"[reg];
            if (reg == 6) {     /* Memory */
                if (prefix >= 0xDD) {
                    *outbuf++ = '(';
                    *outbuf++ = 'I';
                    *outbuf++ = (prefix == 0xDD) ? 'X' : 'Y';
                    *outbuf++ = '+';
                    if (operands) {
                        outbuf += mysprintf(outbuf, "%02X", ppc[2]);
                    } else {
                        *outbuf++ = 'N';
                        *outbuf++ = 'N';
                    }
                    key = ')';
                } else {
                    if (zilog) {
                        *outbuf++ = '(';
                        *outbuf++ = 'H';
                        *outbuf++ = 'L';
                        key = ')';
                    }
                }
            }
            if (prefix >= 0xDD && reg >= 4 && reg < 6) { /* IX/IY? */
                if (prefix == 0xDD)
                    *outbuf++ = 'X';
                else
                    *outbuf++ = 'Y';
            }
            break;

          case 'q':             /* Register in bits 3-5 */
            reg = (opcode >> 3) & 7;
            goto areg;
            break;

          case 'p':             /* Register pair in bits 4-5 */
            reg = (opcode >> 4) & 3;
            switch (reg) {
              case 0:           /* BC */
                key = 'B';
                if (zilog) {
                    *outbuf++ = key;
                    key = 'C';
                }
                break;

              case 1:           /* DE */
                key = 'D';
                if (zilog) {
                    *outbuf++ = key;
                    key = 'E';
                }
                break;

              case 2:           /* HL (or IX or IY) */
                switch (prefix) {
                  case 0xDD:
                    key = 'X';
                    if (zilog) {
                        *outbuf++ = 'I';
                    }
                    break;

                  case 0xFD:
                    key = 'Y';
                    if (zilog) {
                        *outbuf++ = 'Y';
                    }
                    break;

                  default:
                    key = 'H';
                    if (zilog) {
                        *outbuf++ = key;
                        key = 'L';
                    }
                    break;
                }
                break;

              case 3:           /* P (or SP) */
                if (opcode > 0xF0) {
                    key = 'P';
                    if (zilog) {
                        *outbuf++ = 'A';
                        key = 'F';
                    }
                } else {        /* Must be SP. */
                    key = 'S';
                    if (zilog) {
                        *outbuf++ = key;
                        key = 'P';
                    }
                }
                break;
            }
            break;

          case 'h':             /* HL or IX or IY */
            switch (prefix) {
              case 0xDD:
                *outbuf++ = 'I';
                key = 'X';
                break;

              case 0xFD:
                *outbuf++ = 'I';
                key = 'Y';
                break;

              default:
                *outbuf++ = 'H';
                key = 'L';
                break;
            }
            break;

          case 'x':             /* ' ' or X or Y */
            switch (prefix) {
              case 0xDD:
                key = 'X';
                break;

              case 0xFD:
                key = 'Y';
                break;

              default:
                key = ' ';
                break;
            }
            break;

          case 'n':
            reg = ppc[1];
            if (prefix && opcode >= 0x36)
                reg = ppc[3];
            if (operands) {
                outbuf += mysprintf(outbuf, "%02X", reg);
                key = *--outbuf; /* Tacky! */
            } else {
                *outbuf++ = key = 'N';
            }
            break;

          case 'a':
            if (operands) {
                if (*ppc == 0xED || prefix)
                    outbuf += mysprintf(outbuf, "%02X%02X", ppc[3], ppc[2]);
                else
                    outbuf += mysprintf(outbuf, "%02X%02X", ppc[2], ppc[1]);
                key = *--outbuf; /* Tacky! */
            } else {
                *outbuf++ = 'N';
                *outbuf++ = 'N';
                *outbuf++ = 'N';
                key = 'N';
            }
            break;
        }
        *outbuf++ = key;
    }
    *outbuf++ = 0;
}

#if 0
/*
* Decode the instruction.
*
        moveq   #0,d0
        move.b  (pseudopc),d0   ;Opcode
        clr.b   prefix          ;Assume it's not a prefix.
        cmpi.b  #$DD,d0         ;Is it?
        beq.s   dopfx           ;Yes.
        cmpi.b  #$FD,d0
        bne.s   saveop          ;No.
dopfx   move.b  d0,prefix       ;It's a prefix.
        move.b  1(pseudopc),d0  ;The opcode is really here.
saveop  move.b  d0,opcode       ;Save the opcode.
* Look up the opcode.
        cmpi.b  #$40,d0         ;Is opcode in range 00-3F?
        bcc.s   lookupC         ;No.
* The opcode is in the range 00-3F.
        lea     mnop008,a1      ;Assume we're using 8080 mnemonics.
        tst.b   z80flag         ;Should we use Z-80 mnemonics?
        beq.s   lokup0c         ;No
        lea     mnop00z,a1
lokup0c mulu    #12,d0          ;Offset into opcode table
        add.l   d0,a1           ;A1 points to decoded instruction
        bra     decode          ;Decode remainder of instruction.
* The opcode is in the range C0-FF.
lookupC cmpi.b  #$C0,d0         ;Is opcode in range C0-FF?
        bcs.s   lookup8         ;No.
        cmpi.b  #$ED,d0         ;ED-prefix instructions?
        beq     lookupE         ;Yes.
        cmpi.b  #$CB,d0         ;CB-prefix instructions?
        beq     lookupB         ;Yes.
        lea     mnopC08,a1      ;Assume we're using 8080 mnemonics.
        tst.b   z80flag         ;Should we use Z-80 mnemonics?
        beq.s   lokupCc         ;No.
        lea     mnopC0z,a1
lokupCc subi.w  #$C0,d0
        mulu    #12,d0          ;Offset into opcode table
        add.l   d0,a1           ;A1 points to decoded instruction
        bra     decode          ;Decode remainder of instruction.
* The opcode is in the range 80-BF.
lookup8 cmpi.b  #$80,d0         ;Is opcode in range 80-BF?
        bcs.s   lookup4         ;No - it's in range 40-7F.
        lea     mnop808,a1      ;Assume we're using 8080 mnemonics.
        tst.b   z80flag         ;Are we?
        beq.s   lokup8c         ;Yes.
        lea     mnop80z,a1      ;Use the Z-80 mnemonic table.
lokup8c lsr.b   #3,d0           ;Divide opcode by 8.
        andi.w  #7,d0           ;Instruction type
        mulu    #9,d0           ;Offset into opcode table
        add.l   d0,a1           ;A1 points to decoded instruction
        bra     decode          ;Decode remainder of instruction.
* The opcode is in the range 40-7F.
lookup4 cmpi.b  #$76,d0         ;Is this a HLT (Z-80 HALT) instruction?
        bne.s   lokup4m         ;No.
        lea     mnop768,a1
        tst.b   z80flag         ;Do the Z-80 mnemonic?
        beq     decode          ;No.
        lea     mnop76z,a1
        bra     decode
lokup4m lea     mnop408,a1      ;Assume 8080 MOV instruction.
        tst.b   z80flag         ;Are we doing Z-80 mnemonics?
        beq     decode          ;No.
        lea     mnop40z,a1      ;Make it a Z-80 LD instruction.
        bra     decode
* CB-prefix instruction decoding
lookupB move.b  1(pseudopc),d0  ;Sub-opcode
        tst.b   prefix          ;IX or IY with displacement?
        beq.s   lokupB0         ;No.
        move.b  2(pseudopc),d0  ;Skip over the displacement.
lokupB0 move.b  d0,opcode
        cmpi.b  #$40,d0         ;Is opcode in range CB00-CB3F?
        bcc.s   lokupB4         ;No.
        lea     mnopCB08,a1
        lsr.b   #3,d0
        mulu    #8,d0
        add.l   d0,a1
        bra     decode
lokupB4 move.b  d0,d1           ;Save the sub-opcode.
        andi.w  #$C0,d0         ;Opcode is in range CB40-CBFF.
        lsr.b   #3,d0           ;Displacement into 8-byte entries
        lea     mnopCB48,a1
        add.l   d0,a1
lokupBm move.b  (a1)+,(a0)+     ;Move mnemonic to decoded area.
        cmpi.b  #'$',(a1)
        bne.s   lokupBm
        move.b  d1,d0           ;Get the sub-opcode again.
        andi.w  #$38,d0         ;Isolate the bit number.
        lsr.b   #3,d0
        addi.b  #'0',d0         ;Convert the bit number to ASCII.
        move.b  d0,(a0)+        ;Move bit number to decoded instruction.
        move.b  #',',(a0)+
        move.b  d1,d0
        andi.b  #7,d0           ;Isolate the register specification.
        bsr     dspreg          ;Set up the register number.
        bra     dispop
* ED-prefix instruction decoding
lookupE move.b  1(pseudopc),d0  ;Sub-opcode
        move.b  d0,opcode
        cmpi.b  #$40,d0         ;Is it below ED40?
        bcs     lookupI         ;Yes - it's illegal.
        cmpi.b  #$80,d0         ;Is it in range ED40-ED7F?
        bcc     lokupE8         ;No.
        lea     mnopE48,a1      ;Assume we're using 8080 mnemonics.
        tst.b   z80flag         ;Should we use Z-80 mnemonics?
        beq.s   lokupEc         ;No
        lea     mnopE4z,a1
lokupEc andi.w  #$3F,d0
        mulu    #12,d0          ;Offset into opcode table
        add.l   d0,a1           ;A1 points to decoded instruction
        bra     decode          ;Decode remainder of instruction.
lokupE8 cmpi.b  #$A0,d0         ;Is opcode in range ED80-ED9F?
        bcs     lookupI         ;Yes - it's illegal.
        cmpi.b  #$BC,d0         ;It must be in range EDA0-EDBB.
        bcc     lookupI
        lea     mnopEA8,a1
        andi.w  #$1F,d0
        mulu    #5,d0
        add.l   d0,a1
        cmpi.b  #' ',(a1)       ;Is opcode blank?
        bne.s   decode          ;No - decode the instruction.
* The instruction is illegal.
lookupI lea     mnopilg,a1      ;Point to "ILLEGAL" message.
*
* Decode the instruction according to the string pointed to by A1.
*  The decoded instruction is built where A0 points.
*  See the mnemonic tables for an explanation of operand codes.
*
decode:
* Code "r" - register in bits 0-2 of opcode
        cmpi.b  #'r',(a1)
        bne.s   decodeq
        move.b  opcode,d0
        bsr     dspreg
        bra     decodex
* Code "q" - register in bits 3-5 of opcode
decodeq cmpi.b  #'q',(a1)
        bne.s   decodep
        move.b  opcode,d0
        lsr.b   #3,d0
        bsr     dspreg
        bra     decodex
* Code "p" - register pair in bits 4-5 of opcode
decodep cmpi.b  #'p',(a1)
        bne     decodeh
        move.b  opcode,d0
        andi.b  #$30,d0         ;Isolate the register bits.
        bne.s   decodpd         ;They aren't 00.
        move.b  #'B',(a0)+      ;00 - register B
        tst.b   z80flag         ;Are we using Z-80 mnemonics?
        beq     decodex         ;No.
        move.b  #'C',(a0)+      ;Call it BC for Z-80.
        bra     decodex
decodpd cmpi.b  #$20,d0
        beq.s   decodph
        bhi.s   decodps
        move.b  #'D',(a0)+      ;01 - register D (DE for Z-80)
        tst.b   z80flag
        beq     decodex
        move.b  #'E',(a0)+
        bra     decodex
decodph cmpi.b  #$DD,prefix     ;10 - check for index prefix.
        beq.s   decodpx         ;IX
        bhi.s   decodpy         ;IY
        move.b  #'H',(a0)+      ;Register H (HL for Z-80)
        tst.b   z80flag
        beq     decodex
        move.b  #'L',(a0)+
        bra     decodex
decodpx move.b  #'I',(a0)+      ;IX
        move.b  #'X',(a0)+
        bra     decodex
decodpy move.b  #'I',(a0)+      ;IY
        move.b  #'Y',(a0)+
        bra     decodex
decodps cmpi.b  #$F0,opcode     ;11 - depends on opcode
        bcc.s   decodpp
        move.b  #'S',(a0)+      ;11 is SP if opcode is less than F0.
        move.b  #'P',(a0)+
        bra     decodex
decodpp tst.b   z80flag         ;Otherwise, it's PSW (AF for Z-80).
        bne.s   decodpz
        move.b  #'P',(a0)+
        move.b  #'S',(a0)+
        move.b  #'W',(a0)+
        bra.s   decodex
decodpz move.b  #'A',(a0)+
        move.b  #'F',(a0)+
        bra.s   decodex
* Code "h" - HL, IX, or IY depending on prefix
decodeh cmpi.b  #'h',(a1)
        bne.s   decoden
        cmpi.b  #$DD,prefix
        beq.s   decodpx         ;IX
        bhi.s   decodpy         ;IY
        move.b  #'H',(a0)+      ;HL
        move.b  #'L',(a0)+
        bra     decodex
* Code "n" - 8-bit value following opcode
decoden cmpi.b  #'n',(a1)
        bne.s   decodea
        move.b  1(pseudopc),d1
        tst.b   prefix          ;DD or FD prefix?
        beq.s   1$              ;No - we have the value.
        cmpi.b  #$36,opcode     ;Is opcode below 36?
        bcs.s   1$              ;Yes - it's a move to index register half
        move.b  3(pseudopc),d1  ;The value is actually here.
1$      bsr     ubyte           ;Convert the value to a hex string.
        bra.s   decodex
* Code "a" - 16-bit value following opcode
decodea cmpi.b  #'a',(a1)
        bne.s   decodem
        move.b  2(pseudopc),d1
        lsl.w   #8,d1
        move.b  1(pseudopc),d1
        cmpi.b  #$ED,(pseudopc) ;Is this an ED-prefix instruction?
        beq.s   1$              ;Yes - the value is shifted one byte.
        tst.b   prefix          ;DD or FD prefix?
        beq.s   2$              ;No - we have the value.
1$      move.b  3(pseudopc),d1  ;The value is actually here.
        lsl.w   #8,d1
        move.b  2(pseudopc),d1
2$      bsr     uword           ;Convert the value to a hex string.
        bra.s   decodex
* Not a special code - just move the character as is.
decodem move.b  (a1),(a0)+
* Try for another character to move (and possibly decode).
decodex addq.l  #1,a1
        cmpi.b  #'$',(a1)
        bne     decode
*
* Display the decoded instruction.
*
dispop  move.b  #cr,(a0)+
        move.b  #lf,(a0)+
        move.b  #'$',(a0)+
        move.l  #workbuf,d1
        bsr     pstring         ;Displaying as a single string is much faster.



/**/
#endif

/***********************************************************************|
|                                                                       |
|       This file contains the target processor (8080/Z-80) simulation  |
|       routines.                                                       |
|                                                                       |
|***********************************************************************/

#include "com.h"

extern void service(unsigned char *tpa);
extern void illegal(unsigned char *tpa);
extern void stepper(unsigned char *tpa);

/***********************************************************************|
|                                                                       |
|       Flag register lookup tables.                                    |
|                                                                       |
|***********************************************************************/

/*
** 8080 Flags are in the order: S Z 0 ACY 0 P 1 CY
**       S: 80
**       Z: 40
**       A: 10
**       P: 04
**       C: 01
**
** Z80 Flags are in the order: S Z 5 H 3 P/V N CY
**       S: 80
**       Z: 40
**       H: 10
**     P/V: 04
**       N: 02
**       C: 01
**
*/

#define CF 1
#define NF 2
#define ZPF 4
#define VF 4
/* Z80 add/subtract V flag logic taken from 68K flags register description. */
#define AVF(s, d, r) (((((s) & (d) & ~(r)) | (~(s) & ~(d) & (r))) >> 5) & VF)
#define SVF(s, d, r) ((((~(s) & (d) & ~(r)) | ((s) & ~(d) & (r))) >> 5) & VF)
#define HF 0x10
#define PF 4
#define ZF 0x40
#define SF 0x80

/*
** Lookup table for 8080 flags S,Z,P,C based on 9-bit results.
*/
static unsigned char flags[512] = {
    0x44, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84,

    0x45, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01, 0x01, 0x05, 0x05, 0x01,
    0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
    0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01, 0x01, 0x05, 0x05, 0x01,
    0x05, 0x01, 0x01, 0x05, 0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01,
    0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01, 0x01, 0x05, 0x05, 0x01,
    0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
    0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
    0x01, 0x05, 0x05, 0x01, 0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
    0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01, 0x01, 0x05, 0x05, 0x01,
    0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01, 0x05, 0x01, 0x01, 0x05,
    0x05, 0x01, 0x01, 0x05, 0x01, 0x05, 0x05, 0x01, 0x81, 0x85, 0x85, 0x81,
    0x85, 0x81, 0x81, 0x85, 0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
    0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81, 0x81, 0x85, 0x85, 0x81,
    0x85, 0x81, 0x81, 0x85, 0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
    0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
    0x85, 0x81, 0x81, 0x85, 0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
    0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81, 0x81, 0x85, 0x85, 0x81,
    0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81, 0x85, 0x81, 0x81, 0x85,
    0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81, 0x81, 0x85, 0x85, 0x81,
    0x85, 0x81, 0x81, 0x85, 0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81,
    0x85, 0x81, 0x81, 0x85, 0x81, 0x85, 0x85, 0x81, 0x81, 0x85, 0x85, 0x81,
    0x85, 0x81, 0x81, 0x85};

/*
** Table of cycle counts per 8080 instruction.  For those with two different
** times the times are averaged, unless (like DJNZ) you can assume that one
** time is dominant.
*/

unsigned char cycletabl[256] = {
    4,  10, 7,  6,
    4,  4,  7,  4, /* NOP,LXIB,STAXB,INXB,INRB,DCRB,MVIB,RLCA */
    4,  11, 7,  6,
    4,  4,  7,  4, /* EXAF,DADB,LDAXB,DCXB,INRC,DCRC,MVIC,RRCA */
    13, 10, 7,  6,
    4,  4,  7,  4, /* DJNZ,LXID,STAXD,INXD,INRD,DCRD,MVID,RAL */
    12, 11, 7,  6,
    4,  4,  7,  4, /* JR,DADD,LDAXD,DCXD,INRE,DCRE,MVIE,RAR */
    7,  10, 16, 6,
    4,  4,  7,  4, /* JRNZ,LXIH,SHLD,INXH,INRH,DCRH,MVIH,DAA */
    7,  11, 16, 6,
    4,  4,  7,  4, /* JRZ,DADH,LHLD,DCXH,INRL,DCRL,MVIL,CMA */
    7,  10, 13, 6,
    11, 11, 7,  4, /* JRNC,LXIS,STA,INXS,INRM,DCRM,MVIM,STC */
    7,  11, 13, 6,
    4,  4,  7,  4, /* JRC,DADS,LDA,DCXS,INRA,DCRA,MVIA,CMC */
    4,  4,  4,  4,
    4,  4,  7,  4, /* MOVBB,MOVBC,MOVBD,MOVBE,MOVBH,MOVBL,MOVBM,MOVBA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* MOVCB,MOVCC,MOVCD,MOVCE,MOVCH,MOVCL,MOVCM,MOVCA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* MOVDB,MOVDC,MOVDD,MOVDE,MOVDH,MOVDL,MOVDM,MOVDA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* MOVEB,MOVEC,MOVED,MOVEE,MOVEH,MOVEL,MOVEM,MOVEA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* MOVHB,MOVHC,MOVHD,MOVHE,MOVHH,MOVHL,MOVHM,MOVHA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* MOVLB,MOVLC,MOVLD,MOVLE,MOVLH,MOVLL,MOVLM,MOVLA */
    7,  7,  7,  7,
    7,  7,  4,  7, /* MOVMB,MOVMC,MOVMD,MOVME,MOVMH,MOVML,HLT,MOVMA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* MOVAB,MOVAC,MOVAD,MOVAE,MOVAH,MOVAL,MOVAM,MOVAA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* ADDB,ADDC,ADDD,ADDE,ADDH,ADDL,ADDM,ADDA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* ADCB,ADCC,ADCD,ADCE,ADCH,ADCL,ADCM,ADCA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* SUBB,SUBC,SUBD,SUBE,SUBH,SUBL,SUBM,SUBA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* SBBB,SBBC,SBBD,SBBE,SBBH,SBBL,SBBM,SBBA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* ANDB,ANDC,ANDD,ANDE,ANDH,ANDL,ANDM,ANDA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* XRAB,XRAC,XRAD,XRAE,XRAH,XRAL,XRAM,XRAA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* ORAB,ORAC,ORAD,ORAE,ORAH,ORAL,ORAM,ORAA */
    4,  4,  4,  4,
    4,  4,  7,  4, /* CMPB,CMPC,CMPD,CMPE,CMPH,CMPL,CMPM,CMPA */
    5,  10, 10, 10,
    10, 11, 7,  11, /* RNZ,POPB,JNZ,JMP,CNZ,PUSHB,ADI,RST0 */
    5,  10, 10, 0,
    10, 17, 7,  11, /* RZ,RET,JZ,PRECB,CZ,CALL,ACI,RST8 */
    5,  10, 10, 11,
    10, 11, 7,  11, /* RNC,POPD,JNC,OUT,CNC,PUSHD,SUI,RST10 */
    5,  4,  10, 11,
    10, 0,  7,  11, /* RC,EXX,JC,IN,CC,PREDD,SBI,RST18 */
    5,  10, 10, 19,
    10, 11, 7,  11, /* RPO,POPH,JPO,XTHL,CPO,PUSHH,ANI,RST20 */
    5,  4,  10, 4,
    10, 0,  7,  11, /* RPE,PCHL,JPE,XCHG,CPE,PREED,XRI,RST28 */
    5,  10, 10, 4,
    10, 11, 7,  11, /* RP,POPP,JP,DI,CP,PUSHP,ORI,RST30 */
    5,  6,  10, 4,
    10, 0,  7,  11 /* RM,SPHL,JM,EI,CM,PREFD,CPI,RST38 */
};

/*
** As above, but for the various Z80 instructions.
*/

unsigned char cycletabl1[256] = {
    /* CBxx*/
    8, 8, 8,  8,
    8, 8, 15, 8, /* RLCB,RLCC,RLCD,RLCE,RLCH,RLCL,RLCM,RLCA */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RRCB,RRCC,RRCD,RRCE,RRCH,RRCL,RRCM,RRCA */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RLB,RLC,RLD,RLE,RLH,RLL,RLM,RLA */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RRB,RRC,RRD,RRE,RRH,RRL,RRM,RRA */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SLAB,SLAC,SLAD,SLAE,SLAH,SLAL,SLAM,SLAA */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SRAB,SRAC,SRAD,SRAE,SRAH,SRAL,SRAM,SRAA */
    0, 0, 0,  0,
    0, 0, 0,  0, /* -,-,-,-,-,-,-,- */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SRLB,SRLC,SRLD,SRLE,SRLH,SRLL,SRLM,SRLA */
    8, 8, 8,  8,
    8, 8, 12, 8, /* BIT0B,BIT0C,BIT0D,BIT0E,BIT0H,BIT0L,BIT0M,BIT0A */
    8, 8, 8,  8,
    8, 8, 12, 8, /* BIT1B,BIT1C,BIT1D,BIT1E,BIT1H,BIT1L,BIT1M,BIT1A */
    8, 8, 8,  8,
    8, 8, 12, 8, /* BIT2B,BIT2C,BIT2D,BIT2E,BIT2H,BIT2L,BIT2M,BIT2A */
    8, 8, 8,  8,
    8, 8, 12, 8, /* BIT3B,BIT3C,BIT3D,BIT3E,BIT3H,BIT3L,BIT3M,BIT3A */
    8, 8, 8,  8,
    8, 8, 12, 8, /* BIT4B,BIT4C,BIT4D,BIT4E,BIT4H,BIT4L,BIT4M,BIT4A */
    8, 8, 8,  8,
    8, 8, 12, 8, /* BIT5B,BIT5C,BIT5D,BIT5E,BIT5H,BIT5L,BIT5M,BIT5A */
    8, 8, 8,  8,
    8, 8, 12, 8, /* BIT6B,BIT6C,BIT6D,BIT6E,BIT6H,BIT6L,BIT6M,BIT6A */
    8, 8, 8,  8,
    8, 8, 12, 8, /* BIT7B,BIT7C,BIT7D,BIT7E,BIT7H,BIT7L,BIT7M,BIT7A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RES0B,RES0C,RES0D,RES0E,RES0H,RES0L,RES0M,RES0A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RES1B,RES1C,RES1D,RES1E,RES1H,RES1L,RES1M,RES1A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RES2B,RES2C,RES2D,RES2E,RES2H,RES2L,RES2M,RES2A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RES3B,RES3C,RES3D,RES3E,RES3H,RES3L,RES3M,RES3A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RES4B,RES4C,RES4D,RES4E,RES4H,RES4L,RES4M,RES4A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RES5B,RES5C,RES5D,RES5E,RES5H,RES5L,RES5M,RES5A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RES6B,RES6C,RES6D,RES6E,RES6H,RES6L,RES6M,RES6A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* RES7B,RES7C,RES7D,RES7E,RES7H,RES7L,RES7M,RES7A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SET0B,SET0C,SET0D,SET0E,SET0H,SET0L,SET0M,SET0A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SET1B,SET1C,SET1D,SET1E,SET1H,SET1L,SET1M,SET1A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SET2B,SET2C,SET2D,SET2E,SET2H,SET2L,SET2M,SET2A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SET3B,SET3C,SET3D,SET3E,SET3H,SET3L,SET3M,SET3A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SET4B,SET4C,SET4D,SET4E,SET4H,SET4L,SET4M,SET4A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SET5B,SET5C,SET5D,SET5E,SET5H,SET5L,SET5M,SET5A */
    8, 8, 8,  8,
    8, 8, 15, 8, /* SET6B,SET6C,SET6D,SET6E,SET6H,SET6L,SET6M,SET6A */
    8, 8, 8,  8,
    8, 8, 15, 8 /* SET7B,SET7C,SET7D,SET7E,SET7H,SET7L,SET7M,SET7A */
};

unsigned char cycletabl2[256] = {
    /* DDxx */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  15, 0,  0,  0,
    0,  0,  0, /* -,DADXB,-,-,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  15, 0,  0,  0,
    0,  0,  0, /* -,DADXD,-,-,-,-,-,- */
    0,  14, 20, 10, 9,
    9,  9,  0, /* -,LXIX,SIXD,INXX,INRXH,DCRXH,MVIXH,- */
    0,  15, 20, 10, 9,
    9,  9,  0, /* -,DADXX,LIXD,DCXX,INRXL,DCRXL,MVIXL,- */
    0,  0,  0,  0,  23,
    23, 19, 0, /* -,-,-,-,INRIX,DCRIX,MVIIX,- */
    0,  15, 0,  0,  0,
    0,  0,  0, /* -,DADXS,-,-,-,-,-,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVBXH,MOVBXL,MOVBIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVCXH,MOVCXL,MOVCIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVDXH,MOVDXL,MOVDIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVEXH,MOVEXL,MOVEIX,- */
    9,  9,  9,  9,  9,
    9,  19, 9, /* MOVXHB,MOVXHC,MOVXHD,MOVXHE,MOVXHXH,MOVXHXL,MOVXHIX,MOVXHA */
    9,  9,  9,  9,  9,
    9,  19, 9, /* MOVXLB,MOVXLC,MOVXLD,MOVXLE,MOVXLXH,MOVLHXL,MOLXHIX,MLVXHA */
    19, 19, 19, 19, 19,
    19, 0,  19, /* MOVIXB,MOVIXC,MOVIXD,MOVIXE,MOVIXH,MOVIXL,-,MOVIXA */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVAXH,MOVAXL,MOVAIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,ADDXH,ADDXL,ADDIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,ADCXH,ADCXL,ADCIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,SUBXH,SUBXL,SUBIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,SBCXH,SBCXL,SBCIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,ANAXH,ANAXL,ANAIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,XRAXH,XRAXL,XRAIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,ORAXH,ORAXL,ORAIX,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,CMPXH,CMPXL,CMPIX,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,PREDDCB,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  14, 0,  23, 0,
    15, 0,  0, /* -,POPX,-,XTIX,-,PUSHX,-,- */
    0,  8,  0,  0,  0,
    0,  0,  0, /* -,PCIX,-,-,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  10, 0,  0,  0,
    0,  0,  0 /* -,SPIX,-,-,-,-,-,- */
};

unsigned char cycletabl3[256] = {
    /* EDxx */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    12, 12, 15, 20, 8, 14, 8, 9,  /* INBC,OUTCB,DSBBB,SBCD,NEG,RETN,IM0,MOVIA */
    12, 12, 15, 20, 0, 14, 0, 9,  /* INCC,OUTCC,DADCB,LBCD,-,RETI,-,MOVRA */
    12, 12, 15, 20, 0, 0,  8, 9,  /* INDC,OUTCD,DSBBD,SDED,-,-,IM1,MOVAI */
    12, 12, 15, 20, 0, 0,  8, 9,  /* INEC,OUTCE,DADCD,LDED,-,-,IM2,MOVAR */
    12, 12, 15, 0,  0, 0,  0, 19, /* INHC,OUTCH,DSBBH,-,-,-,-,RRD */
    12, 12, 15, 0,  0, 0,  0, 19, /* INLC,OUTCL,DADCH,-,-,-,-,RLD */
    12, 12, 15, 20, 0, 0,  0, 0,  /* INMC,OUTCM,DSBBS,SSPD,-,-,-,- */
    12, 12, 15, 20, 0, 0,  0, 0,  /* INAC,OUTCA,DADCS,LSPD,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0,  /* -,-,-,-,-,-,-,- */

    16, 16, 16, 16, 0, 0,  0, 0, /* LDI,CPI,INI,OUTI,-,-,-,- */
    16, 16, 16, 16, 0, 0,  0, 0, /* LDD,CPD,IND,OUTD,-,-,-,- */
    16, 16, 16, 16, 0, 0,  0, 0, /* LDIR,CPIR,INIR,OTIR,-,-,-,- */
    16, 16, 16, 16, 0, 0,  0, 0, /* LDDR,CPDR,INDR,OTDR,-,-,-,- */

    0,  0,  0,  0,  0, 0,  0, 0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0, 0,  0, 0  /* -,-,-,-,-,-,-,- */
};

unsigned char cycletabl4[256] = {
    /* FDxx */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  15, 0,  0,  0,
    0,  0,  0, /* -,DADYB,-,-,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  15, 0,  0,  0,
    0,  0,  0, /* -,DADYD,-,-,-,-,-,- */
    0,  14, 20, 10, 9,
    9,  9,  0, /* -,LXIY,SIYD,INXY,INRYH,DCRYH,MVIYH,- */
    0,  15, 20, 10, 9,
    9,  9,  0, /* -,DADYY,LIYD,DCXY,INRYL,DCRYL,MVIYL,- */
    0,  0,  0,  0,  23,
    23, 19, 0, /* -,-,-,-,INRIY,DCRIY,MVIIY,- */
    0,  15, 0,  0,  0,
    0,  0,  0, /* -,DADYS,-,-,-,-,-,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVBYH,MOVBYL,MOVBIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVCYH,MOVCYL,MOVCIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVDYH,MOVDYL,MOVDIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVEYH,MOVEYL,MOVEIY,- */
    9,  9,  9,  9,  9,
    9,  19, 9, /* MOVYHB,MOVYHC,MOVYHD,MOVYHE,MOVYHYH,MOVYHYL,MOVYHIY,MOVYHA */
    9,  9,  9,  9,  9,
    9,  19, 9, /* MOVYLB,MOVYLC,MOVYLD,MOVYLE,MOVYLYH,MOVLHYL,MOLYHIY,MLVYHA */
    19, 19, 19, 19, 19,
    19, 0,  19, /* MOVIYB,MOVIYC,MOVIYD,MOVIYE,MOVIYH,MOVIYL,-,MOVIYA */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,MOVAYH,MOVAYL,MOVAIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,ADDYH,ADDYL,ADDIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,ADCYH,ADCYL,ADCIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,SUBYH,SUBYL,SUBIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,SBCYH,SBCYL,SBCIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,ANAYH,ANAYL,ANAIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,YRAYH,YRAYL,YRAIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,ORAYH,ORAYL,ORAIY,- */
    0,  0,  0,  0,  9,
    9,  19, 0, /* -,-,-,-,CMPYH,CMPYL,CMPIY,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,PREDDCB,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  14, 0,  23, 0,
    15, 0,  0, /* -,POPY,-,XTIY,-,PUSHY,-,- */
    0,  8,  0,  0,  0,
    0,  0,  0, /* -,PCIY,-,-,-,-,-,- */
    0,  0,  0,  0,  0,
    0,  0,  0, /* -,-,-,-,-,-,-,- */
    0,  10, 0,  0,  0,
    0,  0,  0 /* -,SPIY,-,-,-,-,-,- */
};

/*
** The whole following series 00-3F & 80-FF have undocumenteds where
** the destination goes into the standard B,C,D,E,H,L,X,A series instead
** of back to the source (in-place).  Cycle count is the same.  Right now
** we don't implement them.  These are rare, implementation of this
** whole table might be better done as a single subroutine rather than
** a giant switch-case.
*/
unsigned char cycletabl5[256] = {
    /* DDCBddxx */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RLC (IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RRC (IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RL (IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RR (IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SLA (IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SRA (IX+dd),- */
    0, 0, 0, 0, 0, 0, 0,  0, /* -,-,-,-,-,-,-,- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SRL (IX+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 0,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 1,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 2,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 3,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 4,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 5,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 6,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 7,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 0,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 1,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 2,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 3,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 4,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 5,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 6,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 7,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 0,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 1,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 2,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 3,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 4,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 5,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 6,(IX+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0  /* -,-,-,-,-,-,SET 7,(IX+dd),- */
};

unsigned char cycletabl6[256] = {
    /* FDCBddxx */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RLC (IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RRC (IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RL (IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RR (IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SLA (IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SRA (IY+dd),- */
    0, 0, 0, 0, 0, 0, 0,  0, /* -,-,-,-,-,-,-,- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SRL (IY+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 0,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 1,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 2,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 3,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 4,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 5,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 6,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 20, 0, /* -,-,-,-,-,-,BIT 7,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 0,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 1,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 2,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 3,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 4,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 5,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 6,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,RES 7,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 0,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 1,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 2,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 3,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 4,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 5,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0, /* -,-,-,-,-,-,SET 6,(IY+dd),- */
    0, 0, 0, 0, 0, 0, 23, 0  /* -,-,-,-,-,-,SET 7,(IY+dd),- */
};

/*
** Macros for dealing with the 'private' variables used (for speed) by
** the simulation loop.  EXPORTing makes them available outside,
** IMPORT reverses this.  SEXPORT is a service export, since service
** requests and illegals are handled when pointing to the offending
** opcode, rather than past it to the next instruction.
*/

#define EXPORT                                                                 \
  us = (regp->ppc = ppc) - tpa;                                                \
  regp->pclo = us;                                                             \
  regp->pchi = us >> 8;                                                        \
  us = (regp->psp = psp) - tpa;                                                \
  regp->splo = us;                                                             \
  regp->sphi = us >> 8;

#define SEXPORT                                                                \
  --ppc;                                                                       \
  EXPORT;

#define IMPORT                                                                 \
  ppc = regp->ppc;                                                             \
  psp = regp->psp;

/***********************************************************************|
|                                                                       |
|       Opcode simulation.                                              |
|                                                                       |
|       Flag handling stinks, largely because we don't have             |
|       easy/portable access to the native CC flags.                    |
|                                                                       |
|***********************************************************************/

int opcodesimulator(tpa) unsigned char *tpa;
{
  struct regs *regp;
  unsigned char *ppc, *psp; /* Local copies of these, 20% faster. */
  char sc;
  unsigned char uc;
  unsigned short us, us2, us3;
  unsigned long ul;
  int running;

  regp = (struct regs *)(tpa - sizeof(struct regs));
  running = regp->running;
  IMPORT;
  do {
    switch (*ppc++) {
    case 0x00: /* 00 NOP */
      break;

    case 0x01: /* 01 LXI BC,nnnn */
      regp->regc = *ppc++;
      regp->regb = *ppc++;
      break;

    case 0x02: /* 02 STAX B */
      tpa[(regp->regb << 8 | regp->regc)] = regp->rega;
      break;

    case 0x03: /* 03 INX B */
      us = (regp->regb << 8 | regp->regc) + 1;
      regp->regc = us;
      regp->regb = us >> 8;
      break;

    case 0x04: /* 04 INR B */
      us = (regp->operand1 = regp->regb) + (regp->operand2 = 1) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->regb = us;
      regp->regf = (flags[us] & ~(ZPF | CF | NF)) | uc | (regp->regf & CF);
      break;

    case 0x05: /* 05 DCR B */
      us = ((regp->operand1 = regp->regb) - (regp->operand2 = 1) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regb = us;
      regp->regf = (flags[us] & ~(ZPF | CF)) | uc | NF | (regp->regf & CF);
      break;

    case 0x06: /* 06 MVI b,nn */
      regp->regb = *ppc++;
      break;

    case 0x07: /* 07 RLC */
      us = regp->rega << 1;
      if (us & 0x100) {
        us |= 1;
        regp->regf |= CF;
      } else
        regp->regf &= ~CF;
      regp->rega = us;
      break;

    case 0x08: /* 08 EXAF (Z-80) */
      uc = regp->rega;
      regp->rega = regp->rega2;
      regp->rega2 = uc;
      uc = regp->regf;
      regp->regf = regp->regf2;
      regp->regf2 = uc;
      break;

    case 0x09: /* 09 DAD B */
      us = (regp->regb << 8 | regp->regc);
      us2 = (regp->regh << 8 | regp->regl);
      ul = us + us2;
      regp->regl = ul;
      regp->regh = ul >> 8;
      if (ul & 0x10000)
        regp->regf |= CF;
      else
        regp->regf &= ~CF;
      break;

    case 0x0A: /* 0A LDAX B */
      us = (regp->regb << 8 | regp->regc);
      regp->rega = tpa[us];
      break;

    case 0x0B: /* 0B DCX B */
      us = (regp->regb << 8 | regp->regc) - 1;
      regp->regc = us;
      regp->regb = us >> 8;
      break;

    case 0x0C: /* 0C INR C */
      us = (regp->operand1 = regp->regc) + (regp->operand2 = 1) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->regc = us;
      regp->regf = (flags[us] & ~(ZPF | CF | NF)) | uc | (regp->regf & CF);
      break;

    case 0x0D: /* 0D DCR C */
#ifdef RUNOPTS
      /* LBL: DCR C; JRNZ LBL?*/
      if (ppc[0] == 0x20 && ppc[1] == 0xFD && regp->regc > 1)
        regp->regc = 1;

#endif
      us = ((regp->operand1 = regp->regc) - (regp->operand2 = 1) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regc = us;
      regp->regf = (flags[us] & ~(ZPF | CF)) | uc | NF | (regp->regf & CF);
      break;

    case 0x0E: /* 0E MVI C */
      regp->regc = *ppc++;
      break;

    case 0x0F: /* 0F RRC */
      us = regp->rega;
      if (us & 1) {
        us |= 0x100;
        regp->regf |= CF;
      } else
        regp->regf &= ~CF;
      regp->rega = us >> 1;
      break;

    case 0x10: /* 10 DJNZ dd (Z-80) */
      sc = *ppc++;
      if (--regp->regb)
        ppc += sc;
      else {
      }
      break;

    case 0x11: /* 11 LXI DE,nnnn */
      regp->rege = *ppc++;
      regp->regd = *ppc++;
      break;

    case 0x12: /* 12 STAX D */
      tpa[(regp->regd << 8 | regp->rege)] = regp->rega;
      break;

    case 0x13: /* 13 INX D */
      us = (regp->regd << 8 | regp->rege) + 1;
      regp->rege = us;
      regp->regd = us >> 8;
      break;

    case 0x14: /* 14 INR D */
      us = (regp->operand1 = regp->regd) + (regp->operand2 = 1) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->regd = us;
      regp->regf = (flags[us] & ~(ZPF | CF | NF)) | uc | (regp->regf & CF);
      break;

    case 0x15: /* 15 DCR D */
      us = ((regp->operand1 = regp->regd) - (regp->operand2 = 1) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regd = us;
      regp->regf = (flags[us] & ~(ZPF | CF)) | uc | NF | (regp->regf & CF);
      break;

    case 0x16: /* 16 MVI D,nn */
      regp->regd = *ppc++;
      break;

    case 0x17: /* 17 RAL */
      us = regp->rega << 1 | (regp->regf & CF);
      if (us & 0x100)
        regp->regf |= CF;
      else
        regp->regf &= ~CF;
      regp->rega = us;
      break;

    case 0x18: /* 18 JR dd (Z-80) */
      sc = *ppc++;
      ppc += sc;
      break;

    case 0x19: /* 19 DAD D */
      us = (regp->regd << 8 | regp->rege);
      us2 = (regp->regh << 8 | regp->regl);
      ul = us + us2;
      regp->regl = ul;
      regp->regh = ul >> 8;
      if (ul & 0x10000)
        regp->regf |= CF;
      else
        regp->regf &= ~CF;
      break;

    case 0x1A: /* 1A LDAX D */
      us = (regp->regd << 8 | regp->rege);
      regp->rega = tpa[us];
      break;

    case 0x1B: /* 1B DCX D */
      us = (regp->regd << 8 | regp->rege) - 1;
      regp->rege = us;
      regp->regd = us >> 8;
      break;

    case 0x1C: /* 1C INR E */
      us = (regp->operand1 = regp->rege) + (regp->operand2 = 1) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rege = us;
      regp->regf = (flags[us] & ~(ZPF | CF | NF)) | uc | (regp->regf & CF);
      break;

    case 0x1D: /* 1D DCR E */
      us = ((regp->operand1 = regp->rege) - (regp->operand2 = 1) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rege = us;
      regp->regf = (flags[us] & ~(ZPF | CF)) | uc | NF | (regp->regf & CF);
      break;

    case 0x1E: /* 1E MVI E,nn */
      regp->rege = *ppc++;
      break;

    case 0x1F: /* 1F RAR */
      us = regp->rega | (regp->regf << 8);
      if (us & 1)
        regp->regf |= CF;
      else
        regp->regf &= ~CF;
      regp->rega = us >> 1;
      break;

    case 0x20: /* 20 JRNZ dd (Z-80) */
      sc = *ppc++;
      if (!(regp->regf & ZF))
        ppc += sc;
      break;

    case 0x21: /* 21 LXI H,nnnn */
      regp->regl = *ppc++;
      regp->regh = *ppc++;
      break;

    case 0x22: /* 22 SHLD addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      tpa[us++] = regp->regl;
      tpa[us] = regp->regh;
      break;

    case 0x23: /* 23 INX H */
      us = (regp->regh << 8 | regp->regl) + 1;
      regp->regl = us;
      regp->regh = us >> 8;
      break;

    case 0x24: /* 24 INR H */
      us = (regp->operand1 = regp->regh) + (regp->operand2 = 1) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->regh = us;
      regp->regf = (flags[us] & ~(ZPF | CF | NF)) | uc | (regp->regf & CF);
      break;

    case 0x25: /* 25 DCR H */
      us = ((regp->operand1 = regp->regh) - (regp->operand2 = 1) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regh = us;
      regp->regf = (flags[us] & ~(ZPF | CF)) | uc | NF | (regp->regf & CF);
      break;

    case 0x26: /* 26 MVI H,nn */
      regp->regh = *ppc++;
      break;

    case 0x27: /* 27 DAA */
      uc = (((regp->operand1 & 0x0F) + (regp->operand2 & 0x0F) +
             regp->operand3) &
            31) >>
           4; /* Recreate half-carry. */
      us = ((regp->regf & CF) << 8) | regp->rega;
      us2 = 0;
      if (uc || ((us & 0x0F) > 9))
        us2 = 6;
      if ((us & 0x1FF) > 0x99)
        us2 += 0x60;
      us &= 0xFF;         /* Drop the carry-in now. */
      if (regp->regf & 2) /* Subtract? */
        us -= us2;
      else /* Add.*/
        us += us2;
      regp->rega = us;
      regp->regf = flags[us & 0x1FF];
      break;

    case 0x28: /* 28 JRZ dd (Z-80) */
      sc = *ppc++;
      if (regp->regf & ZF)
        ppc += sc;
      break;

    case 0x29: /* 29 DAD H */
      us = (regp->regh << 8 | regp->regl);
      ul = us + us;
      regp->regl = ul;
      regp->regh = ul >> 8;
      if (ul & 0x10000)
        regp->regf |= CF;
      else
        regp->regf &= ~CF;
      break;

    case 0x2A: /* 2A LHLD addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      regp->regl = tpa[us++];
      regp->regh = tpa[us];
      break;

    case 0x2B: /* 2B DCX H */
      us = (regp->regh << 8 | regp->regl) - 1;
      regp->regl = us;
      regp->regh = us >> 8;
      break;

    case 0x2C: /* 2C INR L */
      us = (regp->operand1 = regp->regl) + (regp->operand2 = 1) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->regl = us;
      regp->regf = (flags[us] & ~(ZPF | CF | NF)) | uc | (regp->regf & CF);
      break;

    case 0x2D: /* 2D DCR L */
      us = ((regp->operand1 = regp->regl) - (regp->operand2 = 1) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regl = us;
      regp->regf = (flags[us] & ~(ZPF | CF)) | uc | NF | (regp->regf & CF);
      break;

    case 0x2E: /* 2E MVI L,nn */
      regp->regl = *ppc++;
      break;

    case 0x2F: /* 2F CMA */
      regp->rega = ~regp->rega;
      regp->regf |= HF | NF;
      break;

    case 0x30: /* 30 JRNC dd (Z-80) */
      sc = *ppc++;
      if (!(regp->regf & 0x1))
        ppc += sc;
      break;

    case 0x31: /* 31 LXI S,nnnn */
      us = *ppc++;
      us |= *ppc++ << 8;
      psp = &tpa[us];
      break;

    case 0x32: /* 32 STA addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      tpa[us] = regp->rega;
      break;

    case 0x33: /* 33 INX S */
      psp++;
      break;

    case 0x34: /* 34 INR M */
      us2 = regp->regh << 8 | regp->regl;
      us = (regp->operand1 = tpa[us2]) + (regp->operand2 = 1) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      tpa[us2] = us;
      regp->regf = (flags[us] & ~(ZPF | CF | NF)) | uc | (regp->regf & CF);
      break;

    case 0x35: /* 35 DCR M */
      us2 = regp->regh << 8 | regp->regl;
      us = ((regp->operand1 = tpa[us2]) - (regp->operand2 = 1) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      tpa[us2] = us;
      regp->regf = (flags[us] & ~(ZPF | CF)) | uc | NF | (regp->regf & CF);
      break;

    case 0x36: /* 36 MVI M,nn */
      us = regp->regh << 8 | regp->regl;
      tpa[us] = *ppc++;
      break;

    case 0x37: /* 37 STC */
      regp->regf |= CF;
      regp->regf &= ~(HF | NF);
      break;

    case 0x38: /* 38 JRC dd (Z-80) */
      sc = *ppc++;
      if (regp->regf & 0x1)
        ppc += sc;
      break;

    case 0x39: /* 39 DAD S */
      us = psp - tpa;
      us2 = (regp->regh << 8 | regp->regl);
      ul = us + us2;
      regp->regl = ul;
      regp->regh = ul >> 8;
      if (ul & 0x10000)
        regp->regf |= CF;
      else
        regp->regf &= ~CF;
      break;

    case 0x3A: /* 3A LDA addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      regp->rega = tpa[us];
      break;

    case 0x3B: /* 3B DCX S */
      --psp;
      break;

    case 0x3C: /* 3C INR A */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = 1) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | CF | NF)) | uc | (regp->regf & CF);
      break;

    case 0x3D: /* 3D DCR A */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = 1) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | CF)) | uc | NF | (regp->regf & CF);
      break;

    case 0x3E: /* 3E MVI A,nn */
      regp->rega = *ppc++;
      break;

    case 0x3F: /* 3F CMC */
      regp->regf ^= CF;
      regp->regf &= ~NF;
      break;

    case 0x40: /* 40 MOV B,B */
      break;

    case 0x41: /* 41 MOV B,C */
      regp->regb = regp->regc;
      break;

    case 0x42: /* 42 MOV B,D */
      regp->regb = regp->regd;
      break;

    case 0x43: /* 43 MOV B,E */
      regp->regb = regp->rege;
      break;

    case 0x44: /* 44 MOV B,H */
      regp->regb = regp->regh;
      break;

    case 0x45: /* 45 MOV B,L */
      regp->regb = regp->regl;
      break;

    case 0x46: /* 46 MOV B,M */
      us = regp->regh << 8 | regp->regl;
      regp->regb = tpa[us];
      break;

    case 0x47: /* 47 MOV B,A */
      regp->regb = regp->rega;
      break;

    case 0x48: /* 48 MOV C,B */
      regp->regc = regp->regb;
      break;

    case 0x49: /* 49 MOV C,C */
      break;

    case 0x4A: /* 4A MOV C,D */
      regp->regc = regp->regd;
      break;

    case 0x4B: /* 4B MOV C,E */
      regp->regc = regp->rege;
      break;

    case 0x4C: /* 4C MOV C,H */
      regp->regc = regp->regh;
      break;

    case 0x4D: /* 4D MOV C,L */
      regp->regc = regp->regl;
      break;

    case 0x4E: /* 4E MOV C,M */
      us = regp->regh << 8 | regp->regl;
      regp->regc = tpa[us];
      break;

    case 0x4F: /* 4F MOV C,A */
      regp->regc = regp->rega;
      break;

    case 0x50: /* 50 MOV D,B */
      regp->regd = regp->regb;
      break;

    case 0x51: /* 51 MOV D,C */
      regp->regd = regp->regc;
      break;

    case 0x52: /* 52 MOV D,D */
      break;

    case 0x53: /* 53 MOV D,E */
      regp->regd = regp->rege;
      break;

    case 0x54: /* 54 MOV D,H */
      regp->regd = regp->regh;
      break;

    case 0x55: /* 55 MOV D,L */
      regp->regd = regp->regl;
      break;

    case 0x56: /* 56 MOV D,M */
      us = regp->regh << 8 | regp->regl;
      regp->regd = tpa[us];
      break;

    case 0x57: /* 57 MOV D,A */
      regp->regd = regp->rega;
      break;

    case 0x58: /* 58 MOV E,B */
      regp->rege = regp->regb;
      break;

    case 0x59: /* 59 MOV E,C */
      regp->rege = regp->regc;
      break;

    case 0x5A: /* 5A MOV E,D */
      regp->rege = regp->regd;
      break;

    case 0x5B: /* 5B MOV E,E */
      break;

    case 0x5C: /* 5C MOV E,H */
      regp->rege = regp->regh;
      break;

    case 0x5D: /* 5D MOV E,L */
      regp->rege = regp->regl;
      break;

    case 0x5E: /* 5E MOV E,M */
      us = regp->regh << 8 | regp->regl;
      regp->rege = tpa[us];
      break;

    case 0x5F: /* 5F MOV E,A */
      regp->rege = regp->rega;
      break;

    case 0x60: /* 60 MOV H,B */
      regp->regh = regp->regb;
      break;

    case 0x61: /* 61 MOV H,C */
      regp->regh = regp->regc;
      break;

    case 0x62: /* 62 MOV H,D */
      regp->regh = regp->regd;
      break;

    case 0x63: /* 63 MOV H,E */
      regp->regh = regp->rege;
      break;

    case 0x64: /* 64 MOV H,H */
      break;

    case 0x65: /* 65 MOV H,L */
      regp->regh = regp->regl;
      break;

    case 0x66: /* 66 MOV H,M */
      us = regp->regh << 8 | regp->regl;
      regp->regh = tpa[us];
      break;

    case 0x67: /* 67 MOV H,A */
      regp->regh = regp->rega;
      break;

    case 0x68: /* 68 MOV L,B */
      regp->regl = regp->regb;
      break;

    case 0x69: /* 69 MOV L,C */
      regp->regl = regp->regc;
      break;

    case 0x6A: /* 6A MOV L,D */
      regp->regl = regp->regd;
      break;

    case 0x6B: /* 6B MOV L,E */
      regp->regl = regp->rege;
      break;

    case 0x6C: /* 6C MOV L,H */
      regp->regl = regp->regh;
      break;

    case 0x6D: /* 6D MOV L,L */
      break;

    case 0x6E: /* 6E MOV L,M */
      us = regp->regh << 8 | regp->regl;
      regp->regl = tpa[us];
      break;

    case 0x6F: /* 6F MOV L,A */
      regp->regl = regp->rega;
      break;

    case 0x70: /* 70 MOV M,B */
      us = regp->regh << 8 | regp->regl;
      tpa[us] = regp->regb;
      break;

    case 0x71: /* 71 MOV M,C */
      us = regp->regh << 8 | regp->regl;
      tpa[us] = regp->regc;
      break;

    case 0x72: /* 72 MOV M,D */
      us = regp->regh << 8 | regp->regl;
      tpa[us] = regp->regd;
      break;

    case 0x73: /* 73 MOV M,E */
      us = regp->regh << 8 | regp->regl;
      tpa[us] = regp->rege;
      break;

    case 0x74: /* 74 MOV M,H */
      us = regp->regh << 8 | regp->regl;
      tpa[us] = regp->regh;
      break;

    case 0x75: /* 75 MOV M,L */
      us = regp->regh << 8 | regp->regl;
      tpa[us] = regp->regl;
      break;

    case 0x76: /* 76 HLT */
      SEXPORT; /* Export full register set. */
      service(tpa);
      IMPORT;
      ppc += 2; /* Move past service opcode. */
      regp->regf = flags[regp->rega];
      break;

    case 0x77: /* 77 MOV M,A */
      us = regp->regh << 8 | regp->regl;
      tpa[us] = regp->rega;
      break;

    case 0x78: /* 78 MOV A,B */
      regp->rega = regp->regb;
      break;

    case 0x79: /* 79 MOV A,C */
      regp->rega = regp->regc;
      break;

    case 0x7A: /* 7A MOV A,D */
      regp->rega = regp->regd;
      break;

    case 0x7B: /* 7B MOV A,E */
      regp->rega = regp->rege;
      break;

    case 0x7C: /* 7C MOV A,H */
      regp->rega = regp->regh;
      break;

    case 0x7D: /* 7D MOV A,L */
      regp->rega = regp->regl;
      break;

    case 0x7E: /* 7E MOV A,M */
      us = regp->regh << 8 | regp->regl;
      regp->rega = tpa[us];
      break;

    case 0x7F: /* 7F MOV A,A */
      break;

    case 0x80: /* 80 ADD B */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regb) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x81: /* 81 ADD C */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regc) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x82: /* 82 ADD D */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regd) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x83: /* 83 ADD E */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->rege) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x84: /* 84 ADD H */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regh) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x85: /* 85 ADD L */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regl) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x86: /* 86 ADD M */
      us2 = regp->regh << 8 | regp->regl;
      us = (regp->operand1 = regp->rega) + (regp->operand2 = tpa[us2]) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x87: /* 87 ADD A */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->rega) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x88: /* 88 ADC B */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regb) +
           (regp->operand3 = (regp->regf & CF));
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x89: /* 89 ADC C */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regc) +
           (regp->operand3 = (regp->regf & CF));
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x8A: /* 8A ADC D */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regd) +
           (regp->operand3 = (regp->regf & CF));
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x8B: /* 8B ADC E */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->rege) +
           (regp->operand3 = (regp->regf & CF));
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x8C: /* 8C ADC H */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regh) +
           (regp->operand3 = (regp->regf & CF));
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x8D: /* 8D ADC L */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regl) +
           (regp->operand3 = (regp->regf & CF));
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x8E: /* 8E ADC M */
      us2 = regp->regh << 8 | regp->regl;
      us = (regp->operand1 = regp->rega) + (regp->operand2 = tpa[us2]) +
           (regp->operand3 = (regp->regf & CF));
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x8F: /* 8F ADC A */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->rega) +
           (regp->operand3 = (regp->regf & CF));
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~(ZPF | NF)) | uc;
      break;

    case 0x90: /* 90 SUB B */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regb) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x91: /* 91 SUB C */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regc) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x92: /* 92 SUB D */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regd) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x93: /* 93 SUB E */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->rege) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x94: /* 94 SUB H */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regh) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x95: /* 95 SUB L */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regl) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x96: /* 96 SUB M */
      us2 = regp->regh << 8 | regp->regl;
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2]) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x97: /* 97 SUB A */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->rega) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x98: /* 98 SBB B */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regb) -
            (regp->operand3 = (regp->regf & CF))) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x99: /* 99 SBB C */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regc) -
            (regp->operand3 = (regp->regf & CF))) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x9A: /* 9A SBB D */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regd) -
            (regp->operand3 = (regp->regf & CF))) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x9B: /* 9B SBB E */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->rege) -
            (regp->operand3 = (regp->regf & CF))) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x9C: /* 9C SBB H */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regh) -
            (regp->operand3 = (regp->regf & CF))) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x9D: /* 9D SBB L */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regl) -
            (regp->operand3 = (regp->regf & CF))) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x9E: /* 9E SBB M */
      us2 = regp->regh << 8 | regp->regl;
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2]) -
            (regp->operand3 = (regp->regf & CF))) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0x9F: /* 9F SBB A */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->rega) -
            (regp->operand3 = (regp->regf & CF))) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xA0: /* A0 ANA B */
      regp->rega &= regp->regb;
      regp->regf = flags[regp->rega];
      break;

    case 0xA1: /* A1 ANA C */
      regp->rega &= regp->regc;
      regp->regf = flags[regp->rega];
      break;

    case 0xA2: /* A2 ANA D */
      regp->rega &= regp->regd;
      regp->regf = flags[regp->rega];
      break;

    case 0xA3: /* A3 ANA E */
      regp->rega &= regp->rege;
      regp->regf = flags[regp->rega];
      break;

    case 0xA4: /* A4 ANA H */
      regp->rega &= regp->regh;
      regp->regf = flags[regp->rega];
      break;

    case 0xA5: /* A5 ANA L */
      regp->rega &= regp->regl;
      regp->regf = flags[regp->rega];
      break;

    case 0xA6: /* A6 ANA M */
      us2 = regp->regh << 8 | regp->regl;
      regp->rega &= tpa[us2];
      regp->regf = flags[regp->rega];
      break;

    case 0xA7: /* A7 ANA A */
      regp->rega &= regp->rega;
      regp->regf = flags[regp->rega];
      break;

    case 0xA8: /* A8 XRA B */
      regp->rega ^= regp->regb;
      regp->regf = flags[regp->rega];
      break;

    case 0xA9: /* A9 XRA C */
      regp->rega ^= regp->regc;
      regp->regf = flags[regp->rega];
      break;

    case 0xAA: /* AA XRA D */
      regp->rega ^= regp->regd;
      regp->regf = flags[regp->rega];
      break;

    case 0xAB: /* AB XRA E */
      regp->rega ^= regp->rege;
      regp->regf = flags[regp->rega];
      break;

    case 0xAC: /* AC XRA H */
      regp->rega ^= regp->regh;
      regp->regf = flags[regp->rega];
      break;

    case 0xAD: /* AD XRA L */
      regp->rega ^= regp->regl;
      regp->regf = flags[regp->rega];
      break;

    case 0xAE: /* AE XRA M */
      us2 = regp->regh << 8 | regp->regl;
      regp->rega ^= tpa[us2];
      regp->regf = flags[regp->rega];
      break;

    case 0xAF: /* AF XRA A */
      regp->rega ^= regp->rega;
      regp->regf = flags[regp->rega];
      break;

    case 0xB0: /* B0 ORA B */
      regp->rega |= regp->regb;
      regp->regf = flags[regp->rega];
      break;

    case 0xB1: /* B1 ORA C */
      regp->rega |= regp->regc;
      regp->regf = flags[regp->rega];
      break;

    case 0xB2: /* B2 ORA D */
      regp->rega |= regp->regd;
      regp->regf = flags[regp->rega];
      break;

    case 0xB3: /* B3 ORA E */
      regp->rega |= regp->rege;
      regp->regf = flags[regp->rega];
      break;

    case 0xB4: /* B4 ORA H */
      regp->rega |= regp->regh;
      regp->regf = flags[regp->rega];
      break;

    case 0xB5: /* B5 ORA L */
      regp->rega |= regp->regl;
      regp->regf = flags[regp->rega];
      break;

    case 0xB6: /* B6 ORA M */
      us2 = regp->regh << 8 | regp->regl;
      regp->rega |= tpa[us2];
      regp->regf = flags[regp->rega];
      break;

    case 0xB7: /* B7 ORA A */
      regp->rega |= regp->rega;
      regp->regf = flags[regp->rega];
      break;

    case 0xB8: /* B8 CMP B */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regb) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xB9: /* B9 CMP C */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regc) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xBA: /* BA CMP D */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regd) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xBB: /* BB CMP E */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->rege) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xBC: /* BC CMP H */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regh) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xBD: /* BD CMP L */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regl) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xBE: /* BE CMP M */
      us2 = regp->regh << 8 | regp->regl;
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2]) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xBF: /* BF CMP A */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->rega) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xC0: /* C0 RNZ */
      if (!(regp->regf & ZF)) {
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
      }
      break;

    case 0xC1: /* C1 POP B */
      regp->regc = *psp++;
      regp->regb = *psp++;
      break;

    case 0xC2: /* C2 JNZ addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (!(regp->regf & ZF))
        ppc = &tpa[us];
      break;

    case 0xC3: /* C3 JMP addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      ppc = &tpa[us];
      break;

    case 0xC4: /* C4 CNZ addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (!(regp->regf & ZF)) {
        us2 = ppc - tpa;
        *--psp = us2 >> 8;
        *--psp = us2;
        ppc = &tpa[us];
      }
      break;

    case 0xC5: /* C5 PUSH B */
      *--psp = regp->regb;
      *--psp = regp->regc;
      break;

    case 0xC6: /* C6 ADI nn */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = *ppc++) +
           (regp->operand3 = 0);
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc;
      break;

    case 0xC7: /* C7 RST 0 */
      us = ppc - tpa;
      *--psp = us >> 8;
      *--psp = us;
      ppc = &tpa[0];
      break;

    case 0xC8: /* C8 RZ */
      if (regp->regf & ZF) {
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
      }
      break;

    case 0xC9: /* C9 RET */
      us = *psp++;
      us |= *psp++ << 8;
      ppc = &tpa[us];
      break;

    case 0xCA: /* CA JZ addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (regp->regf & ZF)
        ppc = &tpa[us];
      break;

    case 0xCB: /* CB Prefix for BIT instrs. */
      if (CBop(*ppc++, regp, tpa)) {
        --ppc;
        regp->badflag = 1;
        SEXPORT; /* Export full register set. */
        illegal(tpa);
        running = 0;
      }
      break;

    case 0xCC: /* CC CZ addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (regp->regf & ZF) {
        us2 = ppc - tpa;
        *--psp = us2 >> 8;
        *--psp = us2;
        ppc = &tpa[us];
      }
      break;

    case 0xCD: /* CD CALL addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      us2 = ppc - tpa;
      *--psp = us2 >> 8;
      *--psp = us2;
      ppc = &tpa[us];
      break;

    case 0xCE: /* CE ACI nn */
      us = (regp->operand1 = regp->rega) + (regp->operand2 = *ppc++) +
           (regp->operand3 = (regp->regf & CF));
      uc = AVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc;
      break;

    case 0xCF: /* CF RST 8 */
      us = ppc - tpa;
      *--psp = us >> 8;
      *--psp = us;
      ppc = &tpa[8];
      break;

    case 0xD0: /* D0 RNC */
      if (!(regp->regf & 0x1)) {
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
      }
      break;

    case 0xD1: /* D1 POP D */
      regp->rege = *psp++;
      regp->regd = *psp++;
      break;

    case 0xD2: /* D2 JNC addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (!(regp->regf & 0x1))
        ppc = &tpa[us];
      break;

    case 0xD3: /* D3 OUT nn */
      (void)*ppc++;
      break;

    case 0xD4: /* D4 CNC addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (!(regp->regf & 0x1)) {
        us2 = ppc - tpa;
        *--psp = us2 >> 8;
        *--psp = us2;
        ppc = &tpa[us];
      }
      break;

    case 0xD5: /* D5 PUSH D */
      *--psp = regp->regd;
      *--psp = regp->rege;
      break;

    case 0xD6: /* D6 SUI nn */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = *ppc++) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xD7: /* D7 RST 10 */
      us = ppc - tpa;
      *--psp = us >> 8;
      *--psp = us;
      ppc = &tpa[0x10];
      break;

    case 0xD8: /* D8 RC */
      if (regp->regf & 0x1) {
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
      }
      break;

    case 0xD9: /* D9 EXX (Z-80) */
      uc = regp->regb2;
      regp->regb2 = regp->regb;
      regp->regb = uc;
      uc = regp->regc2;
      regp->regc2 = regp->regc;
      regp->regc = uc;
      uc = regp->regd2;
      regp->regd2 = regp->regd;
      regp->regd = uc;
      uc = regp->rege2;
      regp->rege2 = regp->rege;
      regp->rege = uc;
      uc = regp->regh2;
      regp->regh2 = regp->regh;
      regp->regh = uc;
      uc = regp->regl2;
      regp->regl2 = regp->regl;
      regp->regl = uc;
      break;

    case 0xDA: /* DA JC addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (regp->regf & 0x1)
        ppc = &tpa[us];
      break;

    case 0xDB: /* DB IN nn */
      (void)*ppc++;
      break;

    case 0xDC: /* DC CC addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (regp->regf & 0x1) {
        us2 = ppc - tpa;
        *--psp = us2 >> 8;
        *--psp = us2;
        ppc = &tpa[us];
      }
      break;

    case 0xDD: /* DD Prefix for IX instrs */
      switch (*ppc++) {
      case 0x09: /* DD09 DADX B */
        us = (regp->regb << 8 | regp->regc);
        us2 = (regp->regxh << 8 | regp->regxl);
        ul = us + us2;
        regp->regxl = ul;
        regp->regxh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        regp->regf &= ~NF;
        break;

      case 0x19: /* DD19 DADX D */
        us = (regp->regd << 8 | regp->rege);
        us2 = (regp->regxh << 8 | regp->regxl);
        ul = us + us2;
        regp->regxl = ul;
        regp->regxh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        regp->regf &= ~NF;
        break;

      case 0x21: /* DD21 LXI X,nnnn */
        regp->regxl = *ppc++;
        regp->regxh = *ppc++;
        break;

      case 0x22: /* DD22 SIXD addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        tpa[us++] = regp->regxl;
        tpa[us] = regp->regxh;
        break;

      case 0x23: /* DD23 INX X */
        us = (regp->regxh << 8 | regp->regxl) + 1;
        regp->regxl = us;
        regp->regxh = us >> 8;
        break;

      case 0x24: /* DD24 INR XH (undocumented) */
        us = (regp->operand1 = regp->regxh) + (regp->operand2 = 1) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->regxh = us;
        regp->regf = (flags[us] & ~(PF | CF | NF)) | uc | (regp->regf & CF);
        break;

      case 0x25: /* DD25 DCR XH (undocumented) */
        us = ((regp->operand1 = regp->regxh) - (regp->operand2 = 1) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regxh = us;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0x26: /* DD26 MVI XH,nn (undocumented) */
        regp->regxh = *ppc++;
        break;

      case 0x29: /* DD29 DADX X */
        us = (regp->regxh << 8 | regp->regxl);
        ul = us + us;
        regp->regxl = ul;
        regp->regxh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        regp->regf &= ~NF;
        break;

      case 0x2A: /* DD2A LIXD addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        regp->regxl = tpa[us++];
        regp->regxh = tpa[us];
        break;

      case 0x2B: /* DD2B DCX X */
        us = (regp->regxh << 8 | regp->regxl) - 1;
        regp->regxl = us;
        regp->regxh = us >> 8;
        break;

      case 0x2C: /* DD2C INR XL (undocumented) */
        us = (regp->operand1 = regp->regxl) + (regp->operand2 = 1) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->regxl = us;
        regp->regf = (flags[us] & ~(PF | CF | NF)) | uc | (regp->regf & CF);
        break;

      case 0x2D: /* DD2D DCR XL (undocumented) */
        us = ((regp->operand1 = regp->regxl) - (regp->operand2 = 1) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regxl = us;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0x2E: /* DD2E MVI XL,nn (undocumented) */
        regp->regxl = *ppc++;
        break;

      case 0x34: /* DD34 INR (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        us = (regp->operand1 = tpa[us2]) + (regp->operand2 = 1) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        tpa[us2] = us;
        regp->regf = (flags[us] & ~(PF | CF | NF)) | uc | (regp->regf & CF);
        break;

      case 0x35: /* DD35 DCR (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        us = ((regp->operand1 = tpa[us2]) - (regp->operand2 = 1) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        tpa[us2] = us;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0x36: /* DD36 MVI (IX+dd),nn */
        us = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        tpa[us] = *ppc++;
        break;

      case 0x39: /* DD39 DADX S */
        us = psp - tpa;
        us2 = (regp->regxh << 8 | regp->regxl);
        ul = us + us2;
        regp->regxl = ul;
        regp->regxh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        regp->regf &= ~NF;
        break;

      case 0x44: /* DD44 MOV B,XH (undocumented) */
        regp->regb = regp->regxh;
        break;

      case 0x45: /* DD45 MOV B,XL (undocumented) */
        regp->regb = regp->regxl;
        break;

      case 0x46: /* DD46 MOV B,(IX+dd) */
        regp->regb = tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++];
        break;

      case 0x4C: /* DD4C MOV C,XH (undocumented) */
        regp->regc = regp->regxh;
        break;

      case 0x4D: /* DD4D MOV C,XL (undocumented) */
        regp->regc = regp->regxl;
        break;

      case 0x4E: /* DD4E MOV C,(IX+dd) */
        regp->regc = tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++];
        break;

      case 0x54: /* DD54 MOV D,XH (undocumented) */
        regp->regd = regp->regxh;
        break;

      case 0x55: /* DD55 MOV D,XL (undocumented) */
        regp->regd = regp->regxl;
        break;

      case 0x56: /* DD56 MOV D,(IX+dd) */
        regp->regd = tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++];
        break;

      case 0x5C: /* DD5C MOV E,XH (undocumented) */
        regp->rege = regp->regxh;
        break;

      case 0x5D: /* DD5D MOV E,XL (undocumented) */
        regp->rege = regp->regxl;
        break;

      case 0x5E: /* DD5E MOV E,(IX+dd) */
        regp->rege = tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++];
        break;

      case 0x60: /* DD60 MOV XH,B (undocumented) */
        regp->regxh = regp->regb;
        break;

      case 0x61: /* DD61 MOV XH,C (undocumented) */
        regp->regxh = regp->regc;
        break;

      case 0x62: /* DD62 MOV XH,D (undocumented) */
        regp->regxh = regp->regd;
        break;

      case 0x63: /* DD63 MOV XH,E (undocumented) */
        regp->regxh = regp->rege;
        break;

      case 0x64: /* DD64 MOV XH,XH (undocumented) */
        break;

      case 0x65: /* DD65 MOV XH,XL (undocumented) */
        regp->regxh = regp->regxl;
        break;

      case 0x66: /* DD66 MOV H,(IX+dd) */
        regp->regh = tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++];
        break;

      case 0x67: /* DD67 MOV XH,A */
        regp->regxh = regp->rega;
        break;

      case 0x68: /* DD68 MOV XL,B (undocumented) */
        regp->regxl = regp->regb;
        break;

      case 0x69: /* DD69 MOV XL,C (undocumented) */
        regp->regxl = regp->regc;
        break;

      case 0x6A: /* DD6A MOV XL,D (undocumented) */
        regp->regxl = regp->regd;
        break;

      case 0x6B: /* DD6B MOV XL,E (undocumented) */
        regp->regxl = regp->rege;
        break;

      case 0x6C: /* DD6C MOV XL,XH (undocumented) */
        regp->regxl = regp->regxh;
        break;

      case 0x6D: /* DD6D MOV XL,XL (undocumented) */
        break;

      case 0x6E: /* DD6E MOV L,(IX+dd) */
        regp->regl = tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++];
        break;

      case 0x6F: /* DD6F MOV XL,A */
        regp->regxl = regp->rega;
        break;

      case 0x70: /* DD70 MOV (IX+dd),B */
        tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++] = regp->regb;
        break;

      case 0x71: /* DD71 MOV (IX+dd),C */
        tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++] = regp->regc;
        break;

      case 0x72: /* DD72 MOV (IX+dd),D */
        tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++] = regp->regd;
        break;

      case 0x73: /* DD73 MOV (IX+dd),E */
        tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++] = regp->rege;
        break;

      case 0x74: /* DD74 MOV (IX+dd),H */
        tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++] = regp->regh;
        break;

      case 0x75: /* DD75 MOV (IX+dd),L */
        tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++] = regp->regl;
        break;

      case 0x77: /* DD77 MOV (IX+dd),A */
        tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++] = regp->rega;
        break;

      case 0x7C: /* DD7C MOV A,XH (undocumented) */
        regp->rega = regp->regxh;
        break;

      case 0x7D: /* DD7D MOV A,XL (undocumented) */
        regp->rega = regp->regxl;
        break;

      case 0x7E: /* DD7E MOV A,(IX+dd) */
        regp->rega = tpa[(regp->regxh << 8 | regp->regxl) + (char)*ppc++];
        break;

      case 0x84: /* DD84 ADD XH (undocumented) */
        us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regxh) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x85: /* DD85 ADD XL (undocumented) */
        us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regxl) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x86: /* DD86 ADD (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        us = (regp->operand1 = regp->rega) + (regp->operand2 = tpa[us2]) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x8C: /* DD8C ADC XH (undocumented) */
        us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regxh) +
             (regp->operand3 = (regp->regf & CF));
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x8D: /* DD8D ADC XL (undocumented) */
        us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regxl) +
             (regp->operand3 = (regp->regf & CF));
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x8E: /* DD8E ADC (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        us = (regp->operand1 = regp->rega) + (regp->operand2 = tpa[us2]) +
             (regp->operand3 = (regp->regf & CF));
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x94: /* DD94 SUB XH (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regxh) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x95: /* DD95 SUB XL (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regxl) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x96: /* DD96 SUB (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2]) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x9C: /* DD9C SBB XH (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regxh) -
              (regp->operand3 = (regp->regf & CF))) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x9D: /* DD9D SBB XL (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regxl) -
              (regp->operand3 = (regp->regf & CF))) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x9E: /* DD9E SBB (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2]) -
              (regp->operand3 = (regp->regf & CF))) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0xA4: /* DDA4 ANA XH (undocumented) */
        regp->rega &= regp->regxh;
        regp->regf = flags[regp->rega];
        break;

      case 0xA5: /* DDA5 ANA XL (undocumented) */
        regp->rega &= regp->regxl;
        regp->regf = flags[regp->rega];
        break;

      case 0xA6: /* DDA6 ANA (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        regp->rega &= tpa[us2];
        regp->regf = flags[regp->rega];
        break;

      case 0xAC: /* DDAC XRA XH (undocumented) */
        regp->rega ^= regp->regxh;
        regp->regf = flags[regp->rega];
        break;

      case 0xAD: /* DDAD XRA XL (undocumented) */
        regp->rega ^= regp->regxl;
        regp->regf = flags[regp->rega];
        break;

      case 0xAE: /* DDAE XRA (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        regp->rega ^= tpa[us2];
        regp->regf = flags[regp->rega];
        break;

      case 0xB4: /* DDB4 ORA XH (undocumented) */
        regp->rega |= regp->regxh;
        regp->regf = flags[regp->rega];
        break;

      case 0xB5: /* DDB5 ORA XL (undocumented) */
        regp->rega |= regp->regxl;
        regp->regf = flags[regp->rega];
        break;

      case 0xB6: /* DDB6 ORA (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        regp->rega |= tpa[us2];
        regp->regf = flags[regp->rega];
        break;

      case 0xBC: /* DDBC CMP XH (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regxh) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0xBD: /* DDBD CMP XL (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regxl) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0xBE: /* DDBE CMP (IX+dd) */
        us2 = (regp->regxh << 8 | regp->regxl) + (char)*ppc++;
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2]) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0xCB: /* DDCB Prefix for IX instrs */
        if (DDCBop(ppc[1], ppc[0], regp, tpa)) {
          ppc -= 2;
          regp->badflag = 1;
          SEXPORT; /* Export full register set. */
          illegal(tpa);
          running = 0;
          break;
        }
        ppc += 2;
        break;

      case 0xE1: /* DDE1 POP X */
        regp->regxl = *psp++;
        regp->regxh = *psp++;
        break;

      case 0xE3: /* DDE3 XTIX */
        uc = regp->regxl;
        regp->regxl = psp[0];
        psp[0] = uc;
        uc = regp->regxh;
        regp->regxh = psp[1];
        psp[1] = uc;
        break;

      case 0xE5: /* DDE5 PUSH X */
        *--psp = regp->regxh;
        *--psp = regp->regxl;
        break;

      case 0xE9: /* DDE9 PCIX */
        us = regp->regxh << 8 | regp->regxl;
        ppc = &tpa[us];
        break;

      case 0xF9: /* DDF9 SPIX */
        us = regp->regxh << 8 | regp->regxl;
        psp = &tpa[us];
        break;

      default:
        --ppc;
        regp->badflag = 1;
        SEXPORT; /* Export full register set. */
        illegal(tpa);
        running = 0;
        break;
      }
      break;

    case 0xDE: /* DE SBI nn */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = *ppc++) -
            (regp->operand3 = (regp->regf & CF))) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->rega = us;
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xDF: /* DF RST 18 */
      us = ppc - tpa;
      *--psp = us >> 8;
      *--psp = us;
      ppc = &tpa[0x18];
      break;

    case 0xE0: /* E0 RPO */
      if (!(regp->regf & PF)) {
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
      }
      break;

    case 0xE1: /* E1 POP H */
      regp->regl = *psp++;
      regp->regh = *psp++;
      break;

    case 0xE2: /* E2 JPO addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (!(regp->regf & PF))
        ppc = &tpa[us];
      break;

    case 0xE3: /* E3 XTHL */
      uc = regp->regl;
      regp->regl = psp[0];
      psp[0] = uc;
      uc = regp->regh;
      regp->regh = psp[1];
      psp[1] = uc;
      break;

    case 0xE4: /* E4 CPO addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (!(regp->regf & PF)) {
        us2 = ppc - tpa;
        *--psp = us2 >> 8;
        *--psp = us2;
        ppc = &tpa[us];
      }
      break;

    case 0xE5: /* E5 PUSH H */
      *--psp = regp->regh;
      *--psp = regp->regl;
      break;

    case 0xE6: /* E6 ANI nn */
      regp->rega &= *ppc++;
      regp->regf = flags[regp->rega];
      break;

    case 0xE7: /* E7 RST 20 */
      us = ppc - tpa;
      *--psp = us >> 8;
      *--psp = us;
      ppc = &tpa[0x20];
      break;

    case 0xE8: /* E8 RPE */
      if (regp->regf & PF) {
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
      }
      break;

    case 0xE9: /* E9 PCHL */
      us = regp->regh << 8 | regp->regl;
      ppc = &tpa[us];
      break;

    case 0xEA: /* EA JPE addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (regp->regf & PF)
        ppc = &tpa[us];
      break;

    case 0xEB: /* EB XCHG */
      uc = regp->regd;
      regp->regd = regp->regh;
      regp->regh = uc;
      uc = regp->rege;
      regp->rege = regp->regl;
      regp->regl = uc;
      break;

    case 0xEC: /* EC CPE addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (regp->regf & PF) {
        us2 = ppc - tpa;
        *--psp = us2 >> 8;
        *--psp = us2;
        ppc = &tpa[us];
      }
      break;

      /*        case 0xED:            // ED Illegal for 8080
      **          regp->badflag = 1;
      **          SEXPORT;            // Export full register set.
      **          illegal(tpa);
      **          running = 0;
      **          break;
      */

      /***********************************************************************|
      |                                                                       |
      |       Support for the Z-80 Block move instructions is always provided |
      |       as the flags for this simulation resemble those of the Z-80     |
      |       rather than the 8080.  Certain packages (notably BDS-C) check   |
      |       the flags and mistakenly assume a Z-80, then use LDIR/LDDR.     |
      |       Therefore, minimal Z-80 support is provided for these           |
      |       instructions, even if we're not building the full-blown         |
      |       Z-80 simulator.                                                 |
      |                                                                       |
      |***********************************************************************/

    case 0xED: /* ED Prefix for popular Z-80 instrs. */
      switch (*ppc++) {
      case 0x40: /* ED40 IN B,(C) */
        break;

      case 0x41: /* ED41 OUT (C),B */
        break;

      case 0x42: /* ED42 DSBB B */
        us = (regp->regb << 8 | regp->regc);
        us2 = (regp->regh << 8 | regp->regl);
        ul = ((us2 - us) - (regp->regf & CF)) & 0x1FFFF;
        regp->regl = ul;
        regp->regh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        ul &= 0xFFFF;
        if (ul & 0x8000)
          regp->regf |= SF;
        else
          regp->regf &= ~SF;
        if (ul)
          regp->regf &= ~ZF;
        else
          regp->regf |= ZF;
        if (((~us & us2 & ~ul) | (us & ~us2 & ul)) & 0x8000)
          regp->regf |= VF;
        else
          regp->regf &= ~VF;
        regp->regf |= NF;
        break;

      case 0x43: /* ED43 SBCD addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        tpa[us++] = regp->regc;
        tpa[us] = regp->regb;
        break;

      case 0x44: /* ED44 NEG */
        us = ((regp->operand1 = ~regp->rega) + (regp->operand2 = 1)) & 0x1FF;
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~ZPF) | uc | NF;
        break;

      case 0x45: /* ED45 RETN (treated as RET) */
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
        break;

      case 0x46: /* ED46 IM0 (set 8080 int mode) */
        break;

      case 0x47: /* ED47 MOV I,A */
        regp->regi = regp->rega;
        break;

      case 0x48: /* ED48 IN C,(C) */
        break;

      case 0x49: /* ED49 OUT (C),C */
        break;

      case 0x4A: /* ED4A DADC B */
        us = (regp->regb << 8 | regp->regc);
        us2 = (regp->regh << 8 | regp->regl);
        ul = us2 + us + (regp->regf & CF);
        regp->regl = ul;
        regp->regh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        ul &= 0xFFFF;
        if (ul & 0x8000)
          regp->regf |= SF;
        else
          regp->regf &= ~SF;
        if (ul)
          regp->regf &= ~ZF;
        else
          regp->regf |= ZF;
        if (((us & us2 & ~ul) | (~us & ~us2 & ul)) & 0x8000)
          regp->regf |= VF;
        else
          regp->regf &= ~VF;
        regp->regf &= ~NF;
        break;

      case 0x4B: /* ED4B LBCD addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        regp->regc = tpa[us++];
        regp->regb = tpa[us];
        break;

      case 0x4D: /* ED4D RETI (treated as RET) */
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
        break;

      case 0x4F: /* ED4F MOV R,A */
        regp->regr = regp->rega;
        break;

      case 0x50: /* ED50 IN D,(C) */
        break;

      case 0x51: /* ED51 OUT (C),D */
        break;

      case 0x52: /* ED52 DSBB D */
        us = (regp->regd << 8 | regp->rege);
        us2 = (regp->regh << 8 | regp->regl);
        ul = ((us2 - us) - (regp->regf & CF)) & 0x1FFFF;
        regp->regl = ul;
        regp->regh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        ul &= 0xFFFF;
        if (ul & 0x8000)
          regp->regf |= SF;
        else
          regp->regf &= ~SF;
        if (ul)
          regp->regf &= ~ZF;
        else
          regp->regf |= ZF;
        if (((~us & us2 & ~ul) | (us & ~us2 & ul)) & 0x8000)
          regp->regf |= VF;
        else
          regp->regf &= ~VF;
        regp->regf |= NF;
        break;

      case 0x53: /* ED53 SDED addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        tpa[us++] = regp->rege;
        tpa[us] = regp->regd;
        break;

      case 0x56: /* ED56 IM1 (ignored) */
        break;

      case 0x57: /* ED57 MOV A,I */
        regp->rega = regp->regi;
        regp->regf =
            (flags[regp->rega] & ~(CF | PF)) | (regp->regf & CF) | regp->regiff;
        break;

      case 0x58: /* ED58 IN E,(C) */
        break;

      case 0x59: /* ED59 OUT (C),E */
        break;

      case 0x5A: /* ED5A DADC D */
        us = (regp->regd << 8 | regp->rege);
        us2 = (regp->regh << 8 | regp->regl);
        ul = us2 + us + (regp->regf & CF);
        regp->regl = ul;
        regp->regh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        ul &= 0xFFFF;
        if (ul & 0x8000)
          regp->regf |= SF;
        else
          regp->regf &= ~SF;
        if (ul)
          regp->regf &= ~ZF;
        else
          regp->regf |= ZF;
        if (((us & us2 & ~ul) | (~us & ~us2 & ul)) & 0x8000)
          regp->regf |= VF;
        else
          regp->regf &= ~VF;
        regp->regf &= ~NF;
        break;

      case 0x5B: /* ED5B LDED addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        regp->rege = tpa[us++];
        regp->regd = tpa[us];
        break;

      case 0x5E: /* ED5E IM2 (ignored) */
        break;

      case 0x5F: /* ED5F MOV A,R */
                 /*
                 ** 16-bit Linear Congruential Random # Generator from Knuth V2 3.2.1.1
                 *(2).
                 **
                 ** This generator is (supposedly) the version that repeats more often
                 * than
                 ** 65536, as it is modulo 65537 instead of 65536, but it has better
                 ** distribution of low-order bits (of which we are using 8 as our output
                 ** value).  I was unable to follow his mathematical proofs for deriving
                 * the
                 ** optimal multiplier, so I did a brute-force test starting with a seed
                 * of 1.
                 ** This found 535 to be a good multiplier, which has a period of 40934.
                 ** There may be other seeds that generate longer sequences with other
                 ** multipliers, but there is a limit to how much time I want to put into
                 * this.
                 **
                 ** This kind of generator uses no addition step, and so will get stuck
                 * if
                 ** it ever gets a seed of 0.  It is of the form:
                 **
                 **      X1 = (aX0 + c) MOD (65537), where a is 535 and c is 0.
                 **
                 **              movw seed(regs),d0
                 **              negw d0
                 **      again:  muls #535,d0
                 **              movl d0,d1
                 **              swap d0
                 **              subw d1,d0
                 **              bpls done
                 **              addqw #2,d0
                 **              bgts again
                 **              subqw #1,d0
                 **      done:   movw d0,seed(regs)
                 **
                 */
        {        /* Tacky port of 68K assembler! */
          short d0;
          unsigned long d1;
          d0 = -regp->seed;
        again : {
          d1 = d0 * 535;
          d0 = (d1 >> 16) - (d1 & 0xFFFF);
          if (d0 >= 0) /* bpls done */
            goto done;
          d0 += 2;
          if (d0 > 0) /* bgts again */
            goto again;
          --d0;
        }
        done:
          regp->rega = (d0 & 0x7F) | (regp->regr & 0x80);
          regp->seed = d0;
        }
        regp->regf =
            (flags[regp->rega] & ~(CF | PF)) | (regp->regf & CF) | regp->regiff;
        break;

      case 0x60: /* ED60 IN H,(C) */
        break;

      case 0x61: /* ED61 OUT (C),H */
        break;

      case 0x62: /* ED62 DSBB H */
        us = (regp->regh << 8 | regp->regl);
        us2 = (regp->regh << 8 | regp->regl);
        ul = ((us2 - us) - (regp->regf & CF)) & 0x1FFFF;
        regp->regl = ul;
        regp->regh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        ul &= 0xFFFF;
        if (ul & 0x8000)
          regp->regf |= SF;
        else
          regp->regf &= ~SF;
        if (ul)
          regp->regf &= ~ZF;
        else
          regp->regf |= ZF;
        if (((~us & us2 & ~ul) | (us & ~us2 & ul)) & 0x8000)
          regp->regf |= VF;
        else
          regp->regf &= ~VF;
        regp->regf |= NF;
        break;

      case 0x67: /* ED67 RRD */
        us = regp->regh << 8 | regp->regl;
        us2 = regp->rega << 8 | tpa[us];
        tpa[us] = us2 >> 4;
        regp->rega = (regp->rega & 0xF0) | (us2 & 0x0F);
        regp->regf = (flags[regp->rega] & ~CF) | (regp->regf & CF);
        break;

      case 0x68: /* ED68 IN L,(C) */
        break;

      case 0x69: /* ED69 OUT (C),L */
        break;

      case 0x6A: /* ED6A DADC H */
        us = (regp->regh << 8 | regp->regl);
        us2 = (regp->regh << 8 | regp->regl);
        ul = us2 + us + (regp->regf & CF);
        regp->regl = ul;
        regp->regh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        ul &= 0xFFFF;
        if (ul & 0x8000)
          regp->regf |= SF;
        else
          regp->regf &= ~SF;
        if (ul)
          regp->regf &= ~ZF;
        else
          regp->regf |= ZF;
        if (((us & us2 & ~ul) | (~us & ~us2 & ul)) & 0x8000)
          regp->regf |= VF;
        else
          regp->regf &= ~VF;
        regp->regf &= ~NF;
        break;

      case 0x6F: /* ED6F RLD */
        us = regp->regh << 8 | regp->regl;
        us2 = tpa[us] << 4 | (regp->rega & 0x0F);
        tpa[us] = us2;
        regp->rega = (regp->rega & 0xF0) | (us2 >> 8);
        regp->regf = (flags[regp->rega] & ~CF) | (regp->regf & CF);
        break;

      case 0x70: /* ED70 IN M,(C) */
        break;

      case 0x71: /* ED71 OUT (C),M */
        break;

      case 0x72: /* ED72 DSBB S */
        us = psp - tpa;
        us2 = (regp->regh << 8 | regp->regl);
        ul = ((us2 - us) - (regp->regf & CF)) & 0x1FFFF;
        regp->regl = ul;
        regp->regh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        ul &= 0xFFFF;
        if (ul & 0x8000)
          regp->regf |= SF;
        else
          regp->regf &= ~SF;
        if (ul)
          regp->regf &= ~ZF;
        else
          regp->regf |= ZF;
        if (((~us & us2 & ~ul) | (us & ~us2 & ul)) & 0x8000)
          regp->regf |= VF;
        else
          regp->regf &= ~VF;
        regp->regf |= NF;
        break;

      case 0x73: /* ED73 SSPD addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        us2 = psp - tpa;
        tpa[us++] = us2;
        tpa[us] = us2 >> 8;
        break;

      case 0x78: /* ED78 IN A,(C) */
        break;

      case 0x79: /* ED79 OUT (C),A */
        break;

      case 0x7A: /* ED7A DADC S */
        us = psp - tpa;
        us2 = (regp->regh << 8 | regp->regl);
        ul = us2 + us + (regp->regf & CF);
        regp->regl = ul;
        regp->regh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        ul &= 0xFFFF;
        if (ul & 0x8000)
          regp->regf |= SF;
        else
          regp->regf &= ~SF;
        if (ul)
          regp->regf &= ~ZF;
        else
          regp->regf |= ZF;
        if (((us & us2 & ~ul) | (~us & ~us2 & ul)) & 0x8000)
          regp->regf |= VF;
        else
          regp->regf &= ~VF;
        regp->regf &= ~NF;
        break;

      case 0x7B: /* ED7B LSPD addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        us2 = tpa[us++];
        us2 |= tpa[us] << 8;
        psp = &tpa[us2];
        break;

      case 0xA0:                            /* EDA0 LDI */
        ul = regp->regb << 8 | regp->regc;  /* Count. */
        us = regp->regh << 8 | regp->regl;  /* Source. */
        us2 = regp->regd << 8 | regp->rege; /* Dest. */
        tpa[us++] = tpa[us2++];
        regp->regl = us;
        regp->regh = us >> 8;
        regp->rege = us2;
        regp->regd = us2 >> 8;
        regp->regf &= ~0x3E;
        if (--ul)
          regp->regf |= PF;
        else
          regp->regf &= ~PF;
        regp->regc = ul;
        regp->regb = ul >> 8;
        break;

      case 0xA1: /* EDA1 CMPI (Z-80 mnem. is CPI) */
        us3 = regp->regb << 8 | regp->regc;
        us2 = regp->regh << 8 | regp->regl; /* Source. */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2++]) -
              (regp->operand3 = 0)) &
             0x1FF;
        --us3;
        regp->regl = us2;
        regp->regh = us2 >> 8;
        regp->regc = us3;
        regp->regb = us3 >> 8;
        uc = us3 ? PF : 0;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0xA2:                            /* EDA2 INI */
      case 0xA3:                            /* EDA3 OUTI */
        us = regp->regb << 8 | regp->regc;  /* Count. */
        us2 = regp->regh << 8 | regp->regl; /* Dest. */
        --us;
        us2++;
        regp->regl = us2;
        regp->regh = us2 >> 8;
        regp->regc = us;
        regp->regb = us >> 8;
        break;

      case 0xA8:                            /* EDA8 LDD */
        ul = regp->regb << 8 | regp->regc;  /* Count. */
        us = regp->regh << 8 | regp->regl;  /* Source. */
        us2 = regp->regd << 8 | regp->rege; /* Dest. */
        tpa[us--] = tpa[us2--];
        regp->regl = us;
        regp->regh = us >> 8;
        regp->rege = us2;
        regp->regd = us2 >> 8;
        regp->regf &= ~0x3E;
        if (--ul)
          regp->regf |= PF;
        else
          regp->regf &= ~PF;
        regp->regc = ul;
        regp->regb = ul >> 8;
        break;

      case 0xA9: /* EDA9 CPD */
        ul = regp->regb << 8 | regp->regc;
        us2 = regp->regh << 8 | regp->regl; /* Source. */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2++]) -
              (regp->operand3 = 0)) &
             0x1FF;
        --ul;
        regp->regl = us2;
        regp->regh = us2 >> 8;
        regp->regc = ul;
        regp->regb = ul >> 8;
        uc = ul ? PF : 0;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0xAA:                            /* EDAA IND */
      case 0xAB:                            /* EDAB OUTD */
        us = regp->regb << 8 | regp->regc;  /* Count. */
        us2 = regp->regh << 8 | regp->regl; /* Dest. */
        --us;
        --us2;
        regp->regl = us2;
        regp->regh = us2 >> 8;
        regp->regc = us;
        regp->regb = us >> 8;
        break;

      case 0xB2:                            /* EDB2 INIR */
        us = regp->regb << 8 | regp->regc;  /* Count. */
        us2 = regp->regh << 8 | regp->regl; /* Dest. */
        us2 += us;
        regp->regl = us2;
        regp->regh = us2 >> 8;
        regp->regb = regp->regc = 0;
        break;

      case 0xB3:                            /* EDB3 OTIR */
        us = regp->regb << 8 | regp->regc;  /* Count. */
        us2 = regp->regh << 8 | regp->regl; /* Source. */
        us2 += us;
        regp->regl = us2;
        regp->regh = us2 >> 8;
        regp->regb = regp->regc = 0;
        break;

      case 0xB9: /* EDB9 CPDR */
        ul = regp->regb << 8 | regp->regc;
        us2 = regp->regh << 8 | regp->regl; /* Source. */
        do {
          us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2--]) -
                (regp->operand3 = 0)) &
               0x1FF;
        } while (--ul && us);
        regp->regl = us2;
        regp->regh = us2 >> 8;
        regp->regc = ul;
        regp->regb = ul >> 8;
        uc = ul ? PF : 0;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0xBA:                            /* EDBA INDR */
      case 0xBB:                            /* EDBB OTDR */
        us = regp->regb << 8 | regp->regc;  /* Count. */
        us2 = regp->regh << 8 | regp->regl; /* Count. */
        us2 -= us;
        regp->regl = us2;
        regp->regh = us2 >> 8;
        regp->regb = regp->regc = 0;
        break;

      case 0xB0:                            /* EDB0 LDIR */
        ul = regp->regb << 8 | regp->regc;  /* Count. */
        us2 = regp->regh << 8 | regp->regl; /* Source. */
        regp->regl = (us2 + ul);
        regp->regh = (us2 + ul) >> 8;
        us = regp->regd << 8 | regp->rege; /* Dest. */
        regp->rege = (us + ul);
        regp->regd = (us + ul) >> 8;
        while (ul--)
          tpa[us++] = tpa[us2++];
        regp->regb = regp->regc = 0;
        regp->regf &= (SF | ZF | CF);
        break;

      case 0xB1: /* EDB1 CPIR */
        us3 = regp->regb << 8 | regp->regc;
        us2 = regp->regh << 8 | regp->regl; /* Source. */
        do {
          us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2++]) -
                (regp->operand3 = 0)) &
               0x1FF;
        } while (--us3 && us);
        regp->regl = us2;
        regp->regh = us2 >> 8;
        regp->regc = us3;
        regp->regb = us3 >> 8;
        uc = us3 ? PF : 0;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0xB8:                            /* EDB8 LDDR */
        ul = regp->regb << 8 | regp->regc;  /* Count. */
        us2 = regp->regh << 8 | regp->regl; /* Source. */
        regp->regl = (us2 - ul);
        regp->regh = (us2 - ul) >> 8;
        us = regp->regd << 8 | regp->rege; /* Dest. */
        regp->rege = (us - ul);
        regp->regd = (us - ul) >> 8;
        while (ul--)
          tpa[us--] = tpa[us2--];
        regp->regb = regp->regc = 0;
        regp->regf &= (SF | ZF | CF);
        break;

      default:
        --ppc;
        regp->badflag = 1;
        SEXPORT; /* Export full register set. */
        illegal(tpa);
        running = 0;
      }
      break;

    case 0xEE: /* EE XRI nn */
      regp->rega ^= *ppc++;
      regp->regf = flags[regp->rega];
      break;

    case 0xEF: /* EF RST 28 */
      us = ppc - tpa;
      *--psp = us >> 8;
      *--psp = us;
      ppc = &tpa[0x28];
      break;

    case 0xF0: /* F0 RP */
      if (!(regp->regf & SF)) {
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
      }
      break;

    case 0xF1: /* F1 POP P */
      regp->regf = *psp++;
      regp->rega = *psp++;
      break;

    case 0xF2: /* F2 JP addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (!(regp->regf & SF))
        ppc = &tpa[us];
      break;

    case 0xF3: /* F3 DI */
      regp->regiff = 0;
      break;

    case 0xF4: /* F4 CP addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (!(regp->regf & SF)) {
        us2 = ppc - tpa;
        *--psp = us2 >> 8;
        *--psp = us2;
        ppc = &tpa[us];
      }
      break;

    case 0xF5: /* F5 PUSH P */
      *--psp = regp->rega;
      *--psp = regp->regf;
      break;

    case 0xF6: /* F6 ORI nn */
      regp->rega |= *ppc++;
      regp->regf = flags[regp->rega];
      break;

    case 0xF7: /* F7 RST 30 */
      us = ppc - tpa;
      *--psp = us >> 8;
      *--psp = us;
      ppc = &tpa[0x30];
      break;

    case 0xF8: /* F8 RM */
      if (regp->regf & SF) {
        us = *psp++;
        us |= *psp++ << 8;
        ppc = &tpa[us];
      }
      break;

    case 0xF9: /* F9 SPHL */
      us = regp->regh << 8 | regp->regl;
      psp = &tpa[us];
      break;

    case 0xFA: /* FA JM addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (regp->regf & SF)
        ppc = &tpa[us];
      break;

    case 0xFB: /* FB EI */
      regp->regiff = PF;
      break;

    case 0xFC: /* FC CM addr */
      us = *ppc++;
      us |= *ppc++ << 8;
      if (regp->regf & SF) {
        us2 = ppc - tpa;
        *--psp = us2 >> 8;
        *--psp = us2;
        ppc = &tpa[us];
      }
      break;

    case 0xFD: /* FD Prefix for IY instrs */
      switch (*ppc++) {
      case 0x09: /* FD09 DADY B */
        us = (regp->regb << 8 | regp->regc);
        us2 = (regp->regyh << 8 | regp->regyl);
        ul = us + us2;
        regp->regyl = ul;
        regp->regyh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        regp->regf &= ~NF;
        break;

      case 0x19: /* FD19 DADY D */
        us = (regp->regd << 8 | regp->rege);
        us2 = (regp->regyh << 8 | regp->regyl);
        ul = us + us2;
        regp->regyl = ul;
        regp->regyh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        regp->regf &= ~NF;
        break;

      case 0x21: /* FD21 LXI Y,nnnn */
        regp->regyl = *ppc++;
        regp->regyh = *ppc++;
        break;

      case 0x22: /* FD22 SIYD addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        tpa[us++] = regp->regyl;
        tpa[us] = regp->regyh;
        break;

      case 0x23: /* FD23 INX Y */
        us = (regp->regyh << 8 | regp->regyl) + 1;
        regp->regyl = us;
        regp->regyh = us >> 8;
        break;

      case 0x24: /* FD24 INR YH (undocumented) */
        us = (regp->operand1 = regp->regyh) + (regp->operand2 = 1) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->regyh = us;
        regp->regf = (flags[us] & ~(PF | CF | NF)) | uc | (regp->regf & CF);
        break;

      case 0x25: /* FD25 DCR YH (undocumented) */
        us = ((regp->operand1 = regp->regyh) - (regp->operand2 = 1) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regyh = us;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0x26: /* FD26 MVI YH,nn (undocumented) */
        regp->regyh = *ppc++;
        break;

      case 0x29: /* FD29 DADY Y */
        us = (regp->regyh << 8 | regp->regyl);
        ul = us + us;
        regp->regyl = ul;
        regp->regyh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        regp->regf &= ~NF;
        break;

      case 0x2A: /* FD2A LIYD addr */
        us = *ppc++;
        us |= *ppc++ << 8;
        regp->regyl = tpa[us++];
        regp->regyh = tpa[us];
        break;

      case 0x2B: /* FD2B DCX Y */
        us = (regp->regyh << 8 | regp->regyl) - 1;
        regp->regyl = us;
        regp->regyh = us >> 8;
        break;

      case 0x2C: /* FD2C INR YL (undocumented) */
        us = (regp->operand1 = regp->regyl) + (regp->operand2 = 1) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->regyl = us;
        regp->regf = (flags[us] & ~(PF | CF | NF)) | uc | (regp->regf & CF);
        break;

      case 0x2D: /* FD2D DCR YL (undocumented) */
        us = ((regp->operand1 = regp->regyl) - (regp->operand2 = 1) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regyl = us;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0x2E: /* FD2E MVI YL,nn (undocumented) */
        regp->regyl = *ppc++;
        break;

      case 0x34: /* FD34 INR (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        us = (regp->operand1 = tpa[us2]) + (regp->operand2 = 1) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        tpa[us2] = us;
        regp->regf = (flags[us] & ~(PF | CF | NF)) | uc | (regp->regf & CF);
        break;

      case 0x35: /* FD35 DCR (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        us = ((regp->operand1 = tpa[us2]) - (regp->operand2 = 1) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        tpa[us2] = us;
        regp->regf = (flags[us] & ~(PF | CF)) | uc | NF | (regp->regf & CF);
        break;

      case 0x36: /* FD36 MVI (IY+dd),nn */
        us = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        tpa[us] = *ppc++;
        break;

      case 0x39: /* FD39 DADY S */
        us = psp - tpa;
        us2 = (regp->regyh << 8 | regp->regyl);
        ul = us + us2;
        regp->regyl = ul;
        regp->regyh = ul >> 8;
        if (ul & 0x10000)
          regp->regf |= CF;
        else
          regp->regf &= ~CF;
        regp->regf &= ~NF;
        break;

      case 0x44: /* FD44 MOV B,YH (undocumented) */
        regp->regb = regp->regyh;
        break;

      case 0x45: /* FD45 MOV B,YL (undocumented) */
        regp->regb = regp->regyl;
        break;

      case 0x46: /* FD46 MOV B,(IY+dd) */
        regp->regb = tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++];
        break;

      case 0x4C: /* FD4C MOV C,YH (undocumented) */
        regp->regc = regp->regyh;
        break;

      case 0x4D: /* FD4C MOV C,YL (undocumented) */
        regp->regc = regp->regyl;
        break;

      case 0x4E: /* FD4E MOV C,(IY+dd) */
        regp->regc = tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++];
        break;

      case 0x54: /* FD54 MOV D,YH (undocumented) */
        regp->regd = regp->regyh;
        break;

      case 0x55: /* FD55 MOV D,YL (undocumented) */
        regp->regd = regp->regyl;
        break;

      case 0x56: /* FD56 MOV D,(IY+dd) */
        regp->regd = tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++];
        break;

      case 0x5C: /* FD5C MOV E,YH (undocumented) */
        regp->rege = regp->regyh;
        break;

      case 0x5D: /* FD5D MOV E,YL (undocumented) */
        regp->rege = regp->regyl;
        break;

      case 0x5E: /* FD5E MOV E,(IY+dd) */
        regp->rege = tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++];
        break;

      case 0x60: /* FD60 MOV YH,B (undocumented) */
        regp->regyh = regp->regb;
        break;

      case 0x61: /* FD61 MOV YH,C (undocumented) */
        regp->regyh = regp->regc;
        break;

      case 0x62: /* FD62 MOV YH,D (undocumented) */
        regp->regyh = regp->regd;
        break;

      case 0x63: /* FD63 MOV YH,E (undocumented) */
        regp->regyh = regp->rege;
        break;

      case 0x64: /* FD64 MOV YH,YH (undocumented) */
        break;

      case 0x65: /* FD65 MOV YH,YL (undocumented) */
        regp->regyh = regp->regyl;
        break;

      case 0x66: /* FD66 MOV H,(IY+dd) */
        regp->regh = tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++];
        break;

      case 0x68: /* FD68 MOV YL,B (undocumented) */
        regp->regyl = regp->regb;
        break;

      case 0x69: /* FD69 MOV YL,C (undocumented) */
        regp->regyl = regp->regc;
        break;

      case 0x6A: /* FD6A MOV YL,D (undocumented) */
        regp->regyl = regp->regd;
        break;

      case 0x6B: /* FD6B MOV YL,E (undocumented) */
        regp->regyl = regp->rege;
        break;

      case 0x6C: /* FD6C MOV YL,YH (undocumented) */
        regp->regyl = regp->regyh;
        break;

      case 0x6D: /* FD6D MOV YL,YL (undocumented) */
        break;

      case 0x6E: /* FD6E MOV L,(IY+dd) */
        regp->regl = tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++];
        break;

      case 0x70: /* FD70 MOV (IY+dd),B */
        tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++] = regp->regb;
        break;

      case 0x71: /* FD71 MOV (IY+dd),C */
        tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++] = regp->regc;
        break;

      case 0x72: /* FD72 MOV (IY+dd),D */
        tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++] = regp->regd;
        break;

      case 0x73: /* FD73 MOV (IY+dd),E */
        tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++] = regp->rege;
        break;

      case 0x74: /* FD74 MOV (IY+dd),H */
        tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++] = regp->regh;
        break;

      case 0x75: /* FD75 MOV (IY+dd),L */
        tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++] = regp->regl;
        break;

      case 0x77: /* FD77 MOV (IY+dd),A */
        tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++] = regp->rega;
        break;

      case 0x7C: /* FD7C MOV A,YH (undocumented) */
        regp->rega = regp->regyh;
        break;

      case 0x7D: /* FD7D MOV A,YL (undocumented) */
        regp->rega = regp->regyl;
        break;

      case 0x7E: /* FD7E MOV A,(IY+dd) */
        regp->rega = tpa[(regp->regyh << 8 | regp->regyl) + (char)*ppc++];
        break;

      case 0x84: /* FD84 ADD YH (undocumented) */
        us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regyh) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x85: /* FD85 ADD YL (undocumented) */
        us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regyl) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x86: /* FD86 ADD (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        us = (regp->operand1 = regp->rega) + (regp->operand2 = tpa[us2]) +
             (regp->operand3 = 0);
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x8C: /* FD8C ADC YH (undocumented) */
        us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regyh) +
             (regp->operand3 = (regp->regf & CF));
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x8D: /* FD8D ADC YL (undocumented) */
        us = (regp->operand1 = regp->rega) + (regp->operand2 = regp->regyl) +
             (regp->operand3 = (regp->regf & CF));
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x8E: /* FD8E ADC (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        us = (regp->operand1 = regp->rega) + (regp->operand2 = tpa[us2]) +
             (regp->operand3 = (regp->regf & CF));
        uc = AVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~(PF | NF)) | uc;
        break;

      case 0x94: /* FD94 SUB YH (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regyh) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x95: /* FD95 SUB YL (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regyl) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x96: /* FD96 SUB (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2]) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x9C: /* FD9C SBB YH (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regyh) -
              (regp->operand3 = (regp->regf & CF))) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x9D: /* FD9D SBB YL (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regyl) -
              (regp->operand3 = (regp->regf & CF))) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0x9E: /* FD9E SBB (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2]) -
              (regp->operand3 = (regp->regf & CF))) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->rega = us;
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0xA4: /* FDA4 ANA YH (undocumented) */
        regp->rega &= regp->regyh;
        regp->regf = flags[regp->rega];
        break;

      case 0xA5: /* FDA5 ANA YL (undocumented) */
        regp->rega &= regp->regyl;
        regp->regf = flags[regp->rega];
        break;

      case 0xA6: /* FDA6 ANA (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        regp->rega &= tpa[us2];
        regp->regf = flags[regp->rega];
        break;

      case 0xAC: /* FDAC XRA YH (undocumented) */
        regp->rega ^= regp->regyh;
        regp->regf = flags[regp->rega];
        break;

      case 0xAD: /* FDAD XRA YL (undocumented) */
        regp->rega ^= regp->regyl;
        regp->regf = flags[regp->rega];
        break;

      case 0xAE: /* FDAE XRA (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        regp->rega ^= tpa[us2];
        regp->regf = flags[regp->rega];
        break;

      case 0xB4: /* FDB4 ORA YH (undocumented) */
        regp->rega |= regp->regyh;
        regp->regf = flags[regp->rega];
        break;

      case 0xB5: /* FDB5 ORA YL (undocumented) */
        regp->rega |= regp->regyl;
        regp->regf = flags[regp->rega];
        break;

      case 0xB6: /* FDB6 ORA (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        regp->rega |= tpa[us2];
        regp->regf = flags[regp->rega];
        break;

      case 0xBC: /* FDBC CMP YH (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regyh) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0xBD: /* FDBD CMP YL (undocumented) */
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = regp->regyl) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0xBE: /* FDBE CMP (IY+dd) */
        us2 = (regp->regyh << 8 | regp->regyl) + (char)*ppc++;
        us = ((regp->operand1 = regp->rega) - (regp->operand2 = tpa[us2]) -
              (regp->operand3 = 0)) &
             0x1FF;
        uc = SVF(regp->operand2, regp->operand1, us);
        regp->regf = (flags[us] & ~PF) | uc | NF;
        break;

      case 0xCB: /* FDCB Prefix for IY instrs */
        if (FDCBop(ppc[1], ppc[0], regp, tpa)) {
          ppc -= 2;
          regp->badflag = 1;
          SEXPORT; /* Export full register set. */
          illegal(tpa);
          running = 0;
          break;
        }
        ppc += 2;
        break;

      case 0xE1: /* FDE1 POP Y */
        regp->regyl = *psp++;
        regp->regyh = *psp++;
        break;

      case 0xE3: /* FDE3 XTIY */
        uc = regp->regyl;
        regp->regyl = psp[0];
        psp[0] = uc;
        uc = regp->regyh;
        regp->regyh = psp[1];
        psp[1] = uc;
        break;

      case 0xE5: /* FDE5 PUSH Y */
        *--psp = regp->regyh;
        *--psp = regp->regyl;
        break;

      case 0xE9: /* FDE9 PCIY */
        us = regp->regyh << 8 | regp->regyl;
        ppc = &tpa[us];
        break;

      case 0xF9: /* FDF9 SPIY */
        us = regp->regyh << 8 | regp->regyl;
        psp = &tpa[us];
        break;

      default:
        --ppc;
        regp->badflag = 1;
        SEXPORT; /* Export full register set. */
        illegal(tpa);
        running = 0;
        break;
      }
      break;

    case 0xFE: /* FE CPI nn */
      us = ((regp->operand1 = regp->rega) - (regp->operand2 = *ppc++) -
            (regp->operand3 = 0)) &
           0x1FF;
      uc = SVF(regp->operand2, regp->operand1, us);
      regp->regf = (flags[us] & ~ZPF) | uc | NF;
      break;

    case 0xFF: /* FF RST 38 */
      us = ppc - tpa;
      *--psp = us >> 8;
      *--psp = us;
      ppc = &tpa[0x38];
      break;
    }
  } while (running > 1);
  EXPORT;
}

/*
** The Z-80 CB opcode dispatchers are split out to mollify some C compilers.
** (The single huge nested case doesn't please them.)  These dispatchers
** doesn't need access to much of the main dispatcher's private variables,
** so the separation is fairly painless and efficient.
*/
static int CBop(uc, regp, tpa) unsigned char uc;
struct regs *regp;
unsigned char *tpa;
{
  unsigned short us, us2;
  unsigned long ul;
  int retval;

  retval = 0;
  switch (uc) {
  case 0x00: /* CB00 RLC B */
    us = regp->regb << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    regp->regb = us;
    break;

  case 0x01: /* CB01 RLC C */
    us = regp->regc << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    regp->regc = us;
    break;

  case 0x02: /* CB02 RLC D */
    us = regp->regd << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    regp->regd = us;
    break;

  case 0x03: /* CB03 RLC E */
    us = regp->rege << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    regp->rege = us;
    break;

  case 0x04: /* CB04 RLC H */
    us = regp->regh << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    regp->regh = us;
    break;

  case 0x05: /* CB05 RLC L */
    us = regp->regl << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    regp->regl = us;
    break;

  case 0x06: /* CB06 RLC M */
    us2 = regp->regh << 8 | regp->regl;
    us = tpa[us2] << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x07: /* CB07 RLC A */
    us = regp->rega << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    regp->rega = us;
    break;

  case 0x08: /* CB08 RRC B */
    us = regp->regb;
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    regp->regb = us;
    break;

  case 0x09: /* CB09 RRC C */
    us = regp->regc;
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    regp->regc = us;
    break;

  case 0x0A: /* CB0A RRC D */
    us = regp->regd;
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    regp->regd = us;
    break;

  case 0x0B: /* CB0B RRC E */
    us = regp->rege;
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    regp->rege = us;
    break;

  case 0x0C: /* CB0C RRC H */
    us = regp->regh;
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    regp->regh = us;
    break;

  case 0x0D: /* CB0D RRC L */
    us = regp->regl;
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    regp->regl = us;
    break;

  case 0x0E: /* CB0E RRC M */
    us2 = regp->regh << 8 | regp->regl;
    us = tpa[us2];
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x0F: /* CB0F RRC A */
    us = regp->rega;
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    regp->rega = us;
    break;

  case 0x10: /* CB10 RL B */
    us = regp->regb << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    regp->regb = us;
    break;

  case 0x11: /* CB11 RL C */
    us = regp->regc << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    regp->regc = us;
    break;

  case 0x12: /* CB12 RL D */
    us = regp->regd << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    regp->regd = us;
    break;

  case 0x13: /* CB13 RL E */
    us = regp->rege << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    regp->rege = us;
    break;

  case 0x14: /* CB14 RL H */
    us = regp->regh << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    regp->regh = us;
    break;

  case 0x15: /* CB15 RL L */
    us = regp->regl << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    regp->regl = us;
    break;

  case 0x16: /* CB16 RL M */
    us2 = regp->regh << 8 | regp->regl;
    us = tpa[us2] << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x17: /* CB17 RL A */
    us = regp->rega << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    regp->rega = us;
    break;

  case 0x18: /* CB18 RR B */
    us = regp->regb | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    regp->regb = us;
    break;

  case 0x19: /* CB19 RR C */
    us = regp->regc | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    regp->regc = us;
    break;

  case 0x1A: /* CB1A RR D */
    us = regp->regd | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    regp->regd = us;
    break;

  case 0x1B: /* CB1B RR E */
    us = regp->rege | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    regp->rege = us;
    break;

  case 0x1C: /* CB1C RR H */
    us = regp->regh | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    regp->regh = us;
    break;

  case 0x1D: /* CB1D RR L */
    us = regp->regl | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    regp->regl = us;
    break;

  case 0x1E: /* CB1E RR M */
    us2 = regp->regh << 8 | regp->regl;
    us = tpa[us2] | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x1F: /* CB1F RR A */
    us = regp->rega | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    regp->regb = us;
    break;

  case 0x20: /* CB20 SLA B */
    us = regp->regb << 1;
    regp->regf = flags[us];
    regp->regb = us;
    break;

  case 0x21: /* CB21 SLA C */
    us = regp->regc << 1;
    regp->regf = flags[us];
    regp->regc = us;
    break;

  case 0x22: /* CB22 SLA D */
    us = regp->regd << 1;
    regp->regf = flags[us];
    regp->regd = us;
    break;

  case 0x23: /* CB23 SLA E */
    us = regp->rege << 1;
    regp->regf = flags[us];
    regp->rege = us;
    break;

  case 0x24: /* CB24 SLA H */
    us = regp->regh << 1;
    regp->regf = flags[us];
    regp->regh = us;
    break;

  case 0x25: /* CB25 SLA L */
    us = regp->regl << 1;
    regp->regf = flags[us];
    regp->regl = us;
    break;

  case 0x26: /* CB26 SLA M */
    us2 = regp->regh << 8 | regp->regl;
    us = tpa[us2] << 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x27: /* CB27 SLA A */
    us = regp->rega << 1;
    regp->regf = flags[us];
    regp->rega = us;
    break;

  case 0x28: /* CB28 SRA B */
    us = regp->regb;
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regb = us;
    break;

  case 0x29: /* CB29 SRA C */
    us = regp->regc;
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regc = us;
    break;

  case 0x2A: /* CB2A SRA D */
    us = regp->regd;
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regd = us;
    break;

  case 0x2B: /* CB2B SRA E */
    us = regp->rege;
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->rege = us;
    break;

  case 0x2C: /* CB2C SRA H */
    us = regp->regh;
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regh = us;
    break;

  case 0x2D: /* CB2D SRA L */
    us = regp->regl;
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regl = us;
    break;

  case 0x2E: /* CB2E SRA M */
    us2 = regp->regh << 8 | regp->regl;
    us = tpa[us2];
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x2F: /* CB2F SRA A */
    us = regp->rega;
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->rega = us;
    break;

  case 0x38: /* CB38 SRL B */
    us = regp->regb;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regb = us;
    break;

  case 0x39: /* CB39 SRL C */
    us = regp->regc;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regc = us;
    break;

  case 0x3A: /* CB3A SRL D */
    us = regp->regd;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regd = us;
    break;

  case 0x3B: /* CB3B SRL E */
    us = regp->rege;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->rege = us;
    break;

  case 0x3C: /* CB3C SRL H */
    us = regp->regh;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regh = us;
    break;

  case 0x3D: /* CB3D SRL L */
    us = regp->regl;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->regl = us;
    break;

  case 0x3E: /* CB3E SRL M */
    us2 = regp->regh << 8 | regp->regl;
    us = tpa[us2];
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x3F: /* CB3F SRL A */
    us = regp->rega;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    regp->rega = us;
    break;

  case 0x40: /* CB40 BIT 0,B */
    if (regp->regb & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x41: /* CB41 BIT 0,C */
    if (regp->regc & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x42: /* CB42 BIT 0,D */
    if (regp->regd & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x43: /* CB43 BIT 0,E */
    if (regp->rege & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x44: /* CB44 BIT 0,H */
    if (regp->regh & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x45: /* CB45 BIT 0,L */
    if (regp->regl & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x46: /* CB46 BIT 0,M */
    us2 = regp->regh << 8 | regp->regl;
    if (tpa[us2] & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x47: /* CB47 BIT 0,A */
    if (regp->rega & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x48: /* CB48 BIT 1,B */
    if (regp->regb & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x49: /* CB49 BIT 1,C */
    if (regp->regc & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x4A: /* CB4A BIT 1,D */
    if (regp->regd & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x4B: /* CB4B BIT 1,E */
    if (regp->rege & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x4C: /* CB4C BIT 1,H */
    if (regp->regh & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x4D: /* CB4D BIT 1,L */
    if (regp->regl & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x4E: /* CB4E BIT 1,M */
    us2 = regp->regh << 8 | regp->regl;
    if (tpa[us2] & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x4F: /* CB4F BIT 1,A */
    if (regp->rega & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x50: /* CB50 BIT 2,B */
    if (regp->regb & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x51: /* CB51 BIT 2,C */
    if (regp->regc & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x52: /* CB52 BIT 2,D */
    if (regp->regd & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x53: /* CB53 BIT 2,E */
    if (regp->rege & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x54: /* CB54 BIT 2,H */
    if (regp->regh & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x55: /* CB55 BIT 2,L */
    if (regp->regl & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x56: /* CB56 BIT 2,M */
    us2 = regp->regh << 8 | regp->regl;
    if (tpa[us2] & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x57: /* CB57 BIT 2,A */
    if (regp->rega & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x58: /* CB58 BIT 3,B */
    if (regp->regb & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x59: /* CB59 BIT 3,C */
    if (regp->regc & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x5A: /* CB5A BIT 3,D */
    if (regp->regd & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x5B: /* CB5B BIT 3,E */
    if (regp->rege & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x5C: /* CB5C BIT 3,H */
    if (regp->regh & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x5D: /* CB5D BIT 3,L */
    if (regp->regl & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x5E: /* CB5E BIT 3,M */
    us2 = regp->regh << 8 | regp->regl;
    if (tpa[us2] & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x5F: /* CB5F BIT 3,A */
    if (regp->rega & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x60: /* CB60 BIT 4,B */
    if (regp->regb & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x61: /* CB61 BIT 4,C */
    if (regp->regc & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x62: /* CB62 BIT 4,D */
    if (regp->regd & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x63: /* CB63 BIT 4,E */
    if (regp->rege & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x64: /* CB64 BIT 4,H */
    if (regp->regh & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x65: /* CB65 BIT 4,L */
    if (regp->regl & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x66: /* CB66 BIT 4,M */
    us2 = regp->regh << 8 | regp->regl;
    if (tpa[us2] & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x67: /* CB67 BIT 4,A */
    if (regp->rega & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x68: /* CB68 BIT 5,B */
    if (regp->regb & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x69: /* CB69 BIT 5,C */
    if (regp->regc & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x6A: /* CB6A BIT 5,D */
    if (regp->regd & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x6B: /* CB6B BIT 5,E */
    if (regp->rege & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x6C: /* CB6C BIT 5,H */
    if (regp->regh & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x6D: /* CB6D BIT 5,L */
    if (regp->regl & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x6E: /* CB6E BIT 5,M */
    us2 = regp->regh << 8 | regp->regl;
    if (tpa[us2] & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x6F: /* CB6F BIT 5,A */
    if (regp->rega & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x70: /* CB70 BIT 6,B */
    if (regp->regb & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x71: /* CB71 BIT 6,C */
    if (regp->regc & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x72: /* CB72 BIT 6,D */
    if (regp->regd & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x73: /* CB73 BIT 6,E */
    if (regp->rege & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x74: /* CB74 BIT 6,H */
    if (regp->regh & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x75: /* CB75 BIT 6,L */
    if (regp->regl & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x76: /* CB76 BIT 6,M */
    us2 = regp->regh << 8 | regp->regl;
    if (tpa[us2] & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x77: /* CB77 BIT 6,A */
    if (regp->rega & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x78: /* CB78 BIT 7,B */
    if (regp->regb & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x79: /* CB79 BIT 7,C */
    if (regp->regc & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x7A: /* CB7A BIT 7,D */
    if (regp->regd & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x7B: /* CB7B BIT 7,E */
    if (regp->rege & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x7C: /* CB7C BIT 7,H */
    if (regp->regh & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x7D: /* CB7D BIT 7,L */
    if (regp->regl & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x7E: /* CB7E BIT 7,M */
    us2 = regp->regh << 8 | regp->regl;
    if (tpa[us2] & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x7F: /* CB7F BIT 7,A */
    if (regp->rega & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x80: /* CB80 RES 0,B */
    regp->regb &= ~0x01;
    break;

  case 0x81: /* CB81 RES 0,C */
    regp->regc &= ~0x01;
    break;

  case 0x82: /* CB82 RES 0,D */
    regp->regd &= ~0x01;
    break;

  case 0x83: /* CB83 RES 0,E */
    regp->rege &= ~0x01;
    break;

  case 0x84: /* CB84 RES 0,H */
    regp->regh &= ~0x01;
    break;

  case 0x85: /* CB85 RES 0,L */
    regp->regl &= ~0x01;
    break;

  case 0x86: /* CB86 RES 0,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] &= ~0x01;
    break;

  case 0x87: /* CB87 RES 0,A */
    regp->rega &= ~0x01;
    break;

  case 0x88: /* CB88 RES 1,B */
    regp->regb &= ~0x02;
    break;

  case 0x89: /* CB89 RES 1,C */
    regp->regc &= ~0x02;
    break;

  case 0x8A: /* CB8A RES 1,D */
    regp->regd &= ~0x02;
    break;

  case 0x8B: /* CB8B RES 1,E */
    regp->rege &= ~0x02;
    break;

  case 0x8C: /* CB8C RES 1,H */
    regp->regh &= ~0x02;
    break;

  case 0x8D: /* CB8D RES 1,L */
    regp->regl &= ~0x02;
    break;

  case 0x8E: /* CB8E RES 1,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] &= ~0x02;
    break;

  case 0x8F: /* CB8F RES 1,A */
    regp->rega &= ~0x02;
    break;

  case 0x90: /* CB90 RES 2,B */
    regp->regb &= ~0x04;
    break;

  case 0x91: /* CB91 RES 2,C */
    regp->regc &= ~0x04;
    break;

  case 0x92: /* CB92 RES 2,D */
    regp->regd &= ~0x04;
    break;

  case 0x93: /* CB93 RES 2,E */
    regp->rege &= ~0x04;
    break;

  case 0x94: /* CB94 RES 2,H */
    regp->regh &= ~0x04;
    break;

  case 0x95: /* CB95 RES 2,L */
    regp->regl &= ~0x04;
    break;

  case 0x96: /* CB96 RES 2,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] &= ~0x04;
    break;

  case 0x97: /* CB97 RES 2,A */
    regp->rega &= ~0x04;
    break;

  case 0x98: /* CB98 RES 3,B */
    regp->regb &= ~0x08;
    break;

  case 0x99: /* CB99 RES 3,C */
    regp->regc &= ~0x08;
    break;

  case 0x9A: /* CB9A RES 3,D */
    regp->regd &= ~0x08;
    break;

  case 0x9B: /* CB9B RES 3,E */
    regp->rege &= ~0x08;
    break;

  case 0x9C: /* CB9C RES 3,H */
    regp->regh &= ~0x08;
    break;

  case 0x9D: /* CB9D RES 3,L */
    regp->regl &= ~0x08;
    break;

  case 0x9E: /* CB9E RES 3,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] &= ~0x08;
    break;

  case 0x9F: /* CB9F RES 3,A */
    regp->rega &= ~0x08;
    break;

  case 0xA0: /* CBA0 RES 4,B */
    regp->regb &= ~0x10;
    break;

  case 0xA1: /* CBA1 RES 4,C */
    regp->regc &= ~0x10;
    break;

  case 0xA2: /* CBA2 RES 4,D */
    regp->regd &= ~0x10;
    break;

  case 0xA3: /* CBA3 RES 4,E */
    regp->rege &= ~0x10;
    break;

  case 0xA4: /* CBA4 RES 4,H */
    regp->regh &= ~0x10;
    break;

  case 0xA5: /* CBA5 RES 4,L */
    regp->regl &= ~0x10;
    break;

  case 0xA6: /* CBA6 RES 4,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] &= ~0x10;
    break;

  case 0xA7: /* CBA7 RES 4,A */
    regp->rega &= ~0x10;
    break;

  case 0xA8: /* CBA8 RES 5,B */
    regp->regb &= ~0x20;
    break;

  case 0xA9: /* CBA9 RES 5,C */
    regp->regc &= ~0x20;
    break;

  case 0xAA: /* CBAA RES 5,D */
    regp->regd &= ~0x20;
    break;

  case 0xAB: /* CBAB RES 5,E */
    regp->rege &= ~0x20;
    break;

  case 0xAC: /* CBAC RES 5,H */
    regp->regh &= ~0x20;
    break;

  case 0xAD: /* CBAD RES 5,L */
    regp->regl &= ~0x20;
    break;

  case 0xAE: /* CBAE RES 5,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] &= ~0x20;
    break;

  case 0xAF: /* CBAF RES 5,A */
    regp->rega &= ~0x20;
    break;

  case 0xB0: /* CBB0 RES 6,B */
    regp->regb &= ~0x40;
    break;

  case 0xB1: /* CBB1 RES 6,C */
    regp->regc &= ~0x40;
    break;

  case 0xB2: /* CBB2 RES 6,D */
    regp->regd &= ~0x40;
    break;

  case 0xB3: /* CBB3 RES 6,E */
    regp->rege &= ~0x40;
    break;

  case 0xB4: /* CBB4 RES 6,H */
    regp->regh &= ~0x40;
    break;

  case 0xB5: /* CBB5 RES 6,L */
    regp->regl &= ~0x40;
    break;

  case 0xB6: /* CBB6 RES 6,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] &= ~0x40;
    break;

  case 0xB7: /* CBB7 RES 6,A */
    regp->rega &= ~0x40;
    break;

  case 0xB8: /* CBB8 RES 7,B */
    regp->regb &= ~0x80;
    break;

  case 0xB9: /* CBB9 RES 7,C */
    regp->regc &= ~0x80;
    break;

  case 0xBA: /* CBBA RES 7,D */
    regp->regd &= ~0x80;
    break;

  case 0xBB: /* CBBB RES 7,E */
    regp->rege &= ~0x80;
    break;

  case 0xBC: /* CBBC RES 7,H */
    regp->regh &= ~0x80;
    break;

  case 0xBD: /* CBBD RES 7,L */
    regp->regl &= ~0x80;
    break;

  case 0xBE: /* CBBE RES 7,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] &= ~0x80;
    break;

  case 0xBF: /* CBBF RES 7,A */
    regp->rega &= ~0x80;
    break;

  case 0xC0: /* CBC0 SET 0,B */
    regp->regb |= 0x01;
    break;

  case 0xC1: /* CBC1 SET 0,C */
    regp->regc |= 0x01;
    break;

  case 0xC2: /* CBC2 SET 0,D */
    regp->regd |= 0x01;
    break;

  case 0xC3: /* CBC3 SET 0,E */
    regp->rege |= 0x01;
    break;

  case 0xC4: /* CBC4 SET 0,H */
    regp->regh |= 0x01;
    break;

  case 0xC5: /* CBC5 SET 0,L */
    regp->regl |= 0x01;
    break;

  case 0xC6: /* CBC6 SET 0,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] |= 0x01;
    break;

  case 0xC7: /* CBC7 SET 0,A */
    regp->rega |= 0x01;
    break;

  case 0xC8: /* CBC8 SET 1,B */
    regp->regb |= 0x02;
    break;

  case 0xC9: /* CBC9 SET 1,C */
    regp->regc |= 0x02;
    break;

  case 0xCA: /* CBCA SET 1,D */
    regp->regd |= 0x02;
    break;

  case 0xCB: /* CBCB SET 1,E */
    regp->rege |= 0x02;
    break;

  case 0xCC: /* CBCC SET 1,H */
    regp->regh |= 0x02;
    break;

  case 0xCD: /* CBCD SET 1,L */
    regp->regl |= 0x02;
    break;

  case 0xCE: /* CBCE SET 1,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] |= 0x02;
    break;

  case 0xCF: /* CBCF SET 1,A */
    regp->rega |= 0x02;
    break;

  case 0xD0: /* CBD0 SET 2,B */
    regp->regb |= 0x04;
    break;

  case 0xD1: /* CBD1 SET 2,C */
    regp->regc |= 0x04;
    break;

  case 0xD2: /* CBD2 SET 2,D */
    regp->regd |= 0x04;
    break;

  case 0xD3: /* CBD3 SET 2,E */
    regp->rege |= 0x04;
    break;

  case 0xD4: /* CBD4 SET 2,H */
    regp->regh |= 0x04;
    break;

  case 0xD5: /* CBD5 SET 2,L */
    regp->regl |= 0x04;
    break;

  case 0xD6: /* CBD6 SET 2,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] |= 0x04;
    break;

  case 0xD7: /* CBD7 SET 2,A */
    regp->rega |= 0x04;
    break;

  case 0xD8: /* CBD8 SET 3,B */
    regp->regb |= 0x08;
    break;

  case 0xD9: /* CBD9 SET 3,C */
    regp->regc |= 0x08;
    break;

  case 0xDA: /* CBDA SET 3,D */
    regp->regd |= 0x08;
    break;

  case 0xDB: /* CBDB SET 3,E */
    regp->rege |= 0x08;
    break;

  case 0xDC: /* CBDC SET 3,H */
    regp->regh |= 0x08;
    break;

  case 0xDD: /* CBDD SET 3,L */
    regp->regl |= 0x08;
    break;

  case 0xDE: /* CBDE SET 3,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] |= 0x08;
    break;

  case 0xDF: /* CBDF SET 3,A */
    regp->rega |= 0x08;
    break;

  case 0xE0: /* CBE0 SET 4,B */
    regp->regb |= 0x10;
    break;

  case 0xE1: /* CBE1 SET 4,C */
    regp->regc |= 0x10;
    break;

  case 0xE2: /* CBE2 SET 4,D */
    regp->regd |= 0x10;
    break;

  case 0xE3: /* CBE3 SET 4,E */
    regp->rege |= 0x10;
    break;

  case 0xE4: /* CBE4 SET 4,H */
    regp->regh |= 0x10;
    break;

  case 0xE5: /* CBE5 SET 4,L */
    regp->regl |= 0x10;
    break;

  case 0xE6: /* CBE6 SET 4,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] |= 0x10;
    break;

  case 0xE7: /* CBE7 SET 4,A */
    regp->rega |= 0x10;
    break;

  case 0xE8: /* CBE8 SET 5,B */
    regp->regb |= 0x20;
    break;

  case 0xE9: /* CBE9 SET 5,C */
    regp->regc |= 0x20;
    break;

  case 0xEA: /* CBEA SET 5,D */
    regp->regd |= 0x20;
    break;

  case 0xEB: /* CBEB SET 5,E */
    regp->rege |= 0x20;
    break;

  case 0xEC: /* CBEC SET 5,H */
    regp->regh |= 0x20;
    break;

  case 0xED: /* CBED SET 5,L */
    regp->regl |= 0x20;
    break;

  case 0xEE: /* CBEE SET 5,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] |= 0x20;
    break;

  case 0xEF: /* CBEF SET 5,A */
    regp->rega |= 0x20;
    break;

  case 0xF0: /* CBF0 SET 6,B */
    regp->regb |= 0x40;
    break;

  case 0xF1: /* CBF1 SET 6,C */
    regp->regc |= 0x40;
    break;

  case 0xF2: /* CBF2 SET 6,D */
    regp->regd |= 0x40;
    break;

  case 0xF3: /* CBF3 SET 6,E */
    regp->rege |= 0x40;
    break;

  case 0xF4: /* CBF4 SET 6,H */
    regp->regh |= 0x40;
    break;

  case 0xF5: /* CBF5 SET 6,L */
    regp->regl |= 0x40;
    break;

  case 0xF6: /* CBF6 SET 6,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] |= 0x40;
    break;

  case 0xF7: /* CBF7 SET 6,A */
    regp->rega |= 0x40;
    break;

  case 0xF8: /* CBF8 SET 7,B */
    regp->regb |= 0x80;
    break;

  case 0xF9: /* CBF9 SET 7,C */
    regp->regc |= 0x80;
    break;

  case 0xFA: /* CBFA SET 7,D */
    regp->regd |= 0x80;
    break;

  case 0xFB: /* CBFB SET 7,E */
    regp->rege |= 0x80;
    break;

  case 0xFC: /* CBFC SET 7,H */
    regp->regh |= 0x80;
    break;

  case 0xFD: /* CBFD SET 7,L */
    regp->regl |= 0x80;
    break;

  case 0xFE: /* CBFE SET 7,M */
    us2 = regp->regh << 8 | regp->regl;
    tpa[us2] |= 0x80;
    break;

  case 0xFF: /* CBFF SET 7,A */
    regp->rega |= 0x80;
    break;

  default:
    retval = 1;
    break;
  }
  return retval;
}

static int DDCBop(op, uc, regp, tpa) unsigned char op;
unsigned char uc;
struct regs *regp;
unsigned char *tpa;
{
  unsigned short us, us2;
  unsigned long ul;
  int retval;

  retval = 0; /* Assume success. */
  switch (op) {
  case 0x06: /* DDCBdd06 RLC (IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    us = tpa[us2] << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x0E: /* DDCBdd0E RRC (IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    us = tpa[us2];
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x16: /* DDCBdd16 RL (IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    us = tpa[us2] << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x1E: /* DDCBdd1E RR (IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    us = tpa[us2] | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x26: /* DDCBdd26 SLA (IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    us = tpa[us2] << 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x2E: /* DDCBdd2E SRA (IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    us = tpa[us2];
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x3E: /* DDCBdd3E SRL (IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    us = tpa[us2];
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x46: /* DDCBdd46 BIT 0,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    if (tpa[us2] & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x4E: /* DDCBdd4E BIT 1,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    if (tpa[us2] & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x56: /* DDCBdd56 BIT 2,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    if (tpa[us2] & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x5E: /* DDCBdd5E BIT 3,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    if (tpa[us2] & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x66: /* DDCBdd66 BIT 4,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    if (tpa[us2] & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x6E: /* DDCBdd6E BIT 5,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    if (tpa[us2] & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x76: /* DDCBdd76 BIT 6,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    if (tpa[us2] & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x7E: /* DDCBdd7E BIT 7,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    if (tpa[us2] & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x86: /* DDCBdd86 RES 0,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] &= ~0x01;
    break;

  case 0x8E: /* DDCBdd8E RES 1,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] &= ~0x02;
    break;

  case 0x96: /* DDCBdd96 RES 2,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] &= ~0x04;
    break;

  case 0x9E: /* DDCBdd9E RES 3,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] &= ~0x08;
    break;

  case 0xA6: /* DDCBddA6 RES 4,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] &= ~0x10;
    break;

  case 0xAE: /* DDCBddAE RES 5,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] &= ~0x20;
    break;

  case 0xB6: /* DDCBddB6 RES 6,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] &= ~0x40;
    break;

  case 0xBE: /* DDCBddBE RES 7,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] &= ~0x80;
    break;

  case 0xC6: /* DDCBddC6 SET 0,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] |= 0x01;
    break;

  case 0xCE: /* DDCBddCE SET 1,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] |= 0x02;
    break;

  case 0xD6: /* DDCBddD6 SET 2,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] |= 0x04;
    break;

  case 0xDE: /* DDCBddDE SET 3,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] |= 0x08;
    break;

  case 0xE6: /* DDCBddE6 SET 4,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] |= 0x10;
    break;

  case 0xEE: /* DDCBddEE SET 5,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] |= 0x20;
    break;

  case 0xF6: /* DDCBddF6 SET 6,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] |= 0x40;
    break;

  case 0xFE: /* DDCBddFE SET 7,(IX+dd) */
    us2 = (regp->regxh << 8 | regp->regxl) + uc;
    tpa[us2] |= 0x80;
    break;

  default:
    retval = 1; /* Failure. */
    break;
  }
  return retval;
}

static int FDCBop(op, uc, regp, tpa) unsigned char op;
unsigned char uc;
struct regs *regp;
unsigned char *tpa;
{
  unsigned short us, us2;
  unsigned long ul;
  int retval;

  retval = 0; /* Assume success. */
  switch (op) {
  case 0x06: /* FDCBdd06 RLC (IY+d) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    us = tpa[us2] << 1;
    if (us & 0x100)
      us |= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x0E: /* FDCBdd0E RRC (IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    us = tpa[us2];
    if (us & 1)
      us |= 0x300;
    us >>= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x16: /* FDCBdd16 RL (IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    us = tpa[us2] << 1 | (regp->regf & CF);
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x1E: /* FDCBdd1E RR (IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    us = tpa[us2] | (regp->regf << 8);
    if (us & 0x1)
      us |= 0x200;
    us = (us >> 1) & 0x1FF;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x26: /* FDCBdd26 SLA (IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    us = tpa[us2] << 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x2E: /* FDCBdd2E SRA (IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    us = tpa[us2];
    if (us & 0x80)
      us |= 0x100;
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x3E: /* FDCBdd3E SRL (IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    us = tpa[us2];
    if (us & 0x1)
      us |= 0x200;
    us >>= 1;
    regp->regf = flags[us];
    tpa[us2] = us;
    break;

  case 0x46: /* FDCBdd46 BIT 0,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    if (tpa[us2] & 0x1)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x4E: /* FDCBdd4E BIT 1,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    if (tpa[us2] & 0x2)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x56: /* FDCBdd56 BIT 2,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    if (tpa[us2] & 0x4)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x5E: /* FDCBdd5E BIT 3,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    if (tpa[us2] & 0x8)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x66: /* FDCBdd66 BIT 4,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    if (tpa[us2] & 0x10)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x6E: /* FDCBdd6E BIT 5,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    if (tpa[us2] & 0x20)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x76: /* FDCBdd76 BIT 6,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    if (tpa[us2] & 0x40)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x7E: /* FDCBdd7E BIT 7,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    if (tpa[us2] & 0x80)
      regp->regf &= ~ZF;
    else
      regp->regf |= ZF;
    break;

  case 0x86: /* FDCBdd86 RES 0,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] &= ~0x01;
    break;

  case 0x8E: /* FDCBdd8E RES 1,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] &= ~0x02;
    break;

  case 0x96: /* FDCBdd96 RES 2,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] &= ~0x04;
    break;

  case 0x9E: /* FDCBdd9E RES 3,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] &= ~0x08;
    break;

  case 0xA6: /* FDCBddA6 RES 4,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] &= ~0x10;
    break;

  case 0xAE: /* FDCBddAE RES 5,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] &= ~0x20;
    break;

  case 0xB6: /* FDCBddB6 RES 6,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] &= ~0x40;
    break;

  case 0xBE: /* FDCBddBE RES 7,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] &= ~0x80;
    break;

  case 0xC6: /* FDCBddC6 SET 0,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] |= 0x01;
    break;

  case 0xCE: /* FDCBddCE SET 1,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] |= 0x02;
    break;

  case 0xD6: /* FDCBddD6 SET 2,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] |= 0x04;
    break;

  case 0xDE: /* FDCBddDE SET 3,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] |= 0x08;
    break;

  case 0xE6: /* FDCBddE6 SET 4,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] |= 0x10;
    break;

  case 0xEE: /* FDCBddEE SET 5,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] |= 0x20;
    break;

  case 0xF6: /* FDCBddF6 SET 6,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] |= 0x40;
    break;

  case 0xFE: /* FDCBddFE SET 7,(IY+dd) */
    us2 = (regp->regyh << 8 | regp->regyl) + uc;
    tpa[us2] |= 0x80;
    break;

  default:
    retval = 1; /* Failure. */
    break;
  }
  return retval;
}

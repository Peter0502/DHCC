#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_base_des
* 功能描述：    1、包括模块pub_base_EnDes加密算法；
*              2、包括模块pub_base_DesChk密码检查；
*
* 作    者:    
* 完成日期：    2003年12月16日
*
* 修改记录：    
* 1. 日    期:
*    修 改 人:
*    修改内容:
*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#define PASSWD_LEN 6      /*密码长度*/

#ifndef u_char
#define u_char unsigned char
#endif
#define DESKEY    "tsinghua"  /* 密钥 */    

typedef unsigned long crypt_t;

#define NN    8

#define BIT1    0x80000000l
#define BIT2    0x40000000l
#define BIT3    0x20000000l
#define BIT4    0x10000000l
#define BIT5    0x08000000l
#define BIT6    0x04000000l
#define BIT7    0x02000000l
#define BIT8    0x01000000l
#define BIT9    0x00800000l
#define BIT10   0x00400000l
#define BIT11   0x00200000l
#define BIT12   0x00100000l
#define BIT13   0x00080000l
#define BIT14   0x00040000l
#define BIT15   0x00020000l
#define BIT16   0x00010000l
#define BIT17   0x00008000l
#define BIT18   0x00004000l
#define BIT19   0x00002000l
#define BIT20   0x00001000l
#define BIT21   0x00000800l
#define BIT22   0x00000400l
#define BIT23   0x00000200l
#define BIT24   0x00000100l
#define BIT25   0x00000080l
#define BIT26   0x00000040l
#define BIT27   0x00000020l
#define BIT28   0x00000010l
#define BIT29   0x00000008l
#define BIT30   0x00000004l
#define BIT31   0x00000002l
#define BIT32   0x00000001l

static crypt_t S1[4][16]={14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
                           0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
                           4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
                           15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13};

static crypt_t S2[4][16]={15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
                           3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
                           0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
                           13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9};

static crypt_t S3[4][16]={10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
                           13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
                           13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
                           1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12};

static crypt_t S4[4][16]={7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
                           13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
                           10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
                           3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14};

static crypt_t S5[4][16]={2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
                           14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
                           4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
                           11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3};

static crypt_t S6[4][16]={12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
                           10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
                           9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
                           4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13};

static crypt_t S7[4][16]={4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
                           13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
                           1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
                           6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12};

static crypt_t S8[4][16]={13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
                           1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
                           7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
                           2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11};


#define I8bits  041l<<16                /* The 1st and last bits in S8 block */
#define I7bits  I8bits<<6               /* The 1st and last bits in S7 block */
#define I5bits  0204l                   /* The 1st and last bits in S5 block */
#define I4bits  I5bits<<6               /* The 1st and last bits in S4 block */
#define I3bits  I4bits<<6               /* The 1st and last bits in S3 block */
#define I2bits  I3bits<<6               /* The 1st and last bits in S2 block */
#define I1bits  I2bits<<6               /* The 1st and last bits in S1 block */
#define Jbits   017l                    /* The other bits in S block */

void    descrypt(char*,char*);
void pub_base_EnDes(long txdate, char *acno, char *passwd );

/****  S function ***/
void S(in,pout)
#ifndef M_I286
register
#endif
crypt_t in[2];
crypt_t *pout;
{
#ifndef M_I286
    register
#endif
    crypt_t out;
    crypt_t temp;
    out=0;
    out|=S1[((in[0]&I1bits)+07400000000)>>30][(in[0]>>27)&Jbits]<<28;
    out|=S2[((in[0]&I2bits)+074000000)>>24][(in[0]>>21)&Jbits]<<24;
    out|=S3[((in[0]&I3bits)+0740000)>>18][(in[0]>>15)&Jbits]<<20;
    out|=S4[((in[0]&I4bits)+07400)>>12][(in[0]>>9)&Jbits]<<16;
    out|=S5[((in[0]&I5bits)+074)>>6][(in[0]>>3)&Jbits]<<12;
    temp=((in[0]&03)<<4)+(in[1]>>28);
    out|=S6[((temp&041)+017)>>4][temp>>1]<<8;
    out|=S7[((in[1]&I7bits)+0360000000)>>26][(in[1]>>23)&Jbits]<<4;
    out|=S8[((in[1]&I8bits)+03600000)>>20][(in[1]>>17)&Jbits];
    *pout=out;
}

/****  E function ***/
void E(in,out)
#ifndef M_I286
register
#endif
crypt_t in;
crypt_t out[2];
{
#ifndef M_I286
    register
#endif
    crypt_t out0,out1;

    out0=0;
    out1=0;

    out0|=(in&BIT32)<<31;     /* bit 1 of out[0] */
    out0|=(in&BIT1)>>1;       /* bit 2 of out[0] */
    out0|=(in&BIT2)>>1;       /* bit 3 of out[0] */
    out0|=(in&BIT3)>>1;       /* bit 4 of out[0] */
    out0|=(in&BIT4)>>1;       /* bit 5 of out[0] */
    out0|=(in&BIT5)>>1;       /* bit 6 of out[0] */
    out0|=(in&BIT4)>>3;       /* bit 7 of out[0] */
    out0|=(in&BIT5)>>3;       /* bit 8 of out[0] */
    out0|=(in&BIT6)>>3;       /* bit 9 of out[0] */
    out0|=(in&BIT7)>>3;       /* bit 10 of out[0] */
    out0|=(in&BIT8)>>3;       /* bit 11 of out[0] */
    out0|=(in&BIT9)>>3;       /* bit 12 of out[0] */
    out0|=(in&BIT8)>>5;       /* bit 13 of out[0] */
    out0|=(in&BIT9)>>5;       /* bit 14 of out[0] */
    out0|=(in&BIT10)>>5;      /* bit 15 of out[0] */
    out0|=(in&BIT11)>>5;      /* bit 16 of out[0] */
    out0|=(in&BIT12)>>5;      /* bit 17 of out[0] */
    out0|=(in&BIT13)>>5;      /* bit 18 of out[0] */
    out0|=(in&BIT12)>>7;      /* bit 19 of out[0] */
    out0|=(in&BIT13)>>7;      /* bit 20 of out[0] */
    out0|=(in&BIT14)>>7;      /* bit 21 of out[0] */
    out0|=(in&BIT15)>>7;      /* bit 22 of out[0] */
    out0|=(in&BIT16)>>7;      /* bit 23 of out[0] */
    out0|=(in&BIT17)>>7;      /* bit 24 of out[0] */
    out0|=(in&BIT16)>>9;      /* bit 25 of out[0] */
    out0|=(in&BIT17)>>9;      /* bit 26 of out[0] */
    out0|=(in&BIT18)>>9;      /* bit 27 of out[0] */
    out0|=(in&BIT19)>>9;      /* bit 28 of out[0] */
    out0|=(in&BIT20)>>9;      /* bit 29 of out[0] */
    out0|=(in&BIT21)>>9;      /* bit 30 of out[0] */
    out0|=(in&BIT20)>>11;     /* bit 31 of out[0] */
    out0|=(in&BIT21)>>11;     /* bit 32 of out[0] */
    out1|=(in&BIT22)<<21;     /* bit 1 of out[1] */
    out1|=(in&BIT23)<<21;     /* bit 2 of out[1] */
    out1|=(in&BIT24)<<21;     /* bit 3 of out[1] */
    out1|=(in&BIT25)<<21;     /* bit 4 of out[1] */
    out1|=(in&BIT24)<<19;     /* bit 5 of out[1] */
    out1|=(in&BIT25)<<19;     /* bit 6 of out[1] */
    out1|=(in&BIT26)<<19;     /* bit 7 of out[1] */
    out1|=(in&BIT27)<<19;     /* bit 8 of out[1] */
    out1|=(in&BIT28)<<19;     /* bit 9 of out[1] */
    out1|=(in&BIT29)<<19;     /* bit 10 of out[1] */
    out1|=(in&BIT28)<<17;     /* bit 11 of out[1] */
    out1|=(in&BIT29)<<17;     /* bit 12 of out[1] */
    out1|=(in&BIT30)<<17;     /* bit 13 of out[1] */
    out1|=(in&BIT31)<<17;     /* bit 14 of out[1] */
    out1|=(in&BIT32)<<17;     /* bit 15 of out[1] */
    out1|=(in&BIT1)>>15;      /* bit 16 of out[1] */

    out[0]=out0;
    out[1]=out1;
}

/****  P function ***/
void P(in,pout)
#ifndef M_I286
register
#endif
crypt_t in;
crypt_t *pout;
{
#ifndef M_I286
    register
#endif
    crypt_t out;
    out=0;
    out|=(in&BIT16)<<15;                /* bit 1 of out */
    out|=(in&BIT7)<<5;                  /* bit 2 of out */
    out|=(in&BIT20)<<17;                /* bit 3 of out */
    out|=(in&BIT21)<<17;                /* bit 4 of out */
    out|=(in&BIT29)<<24;                /* bit 5 of out */
    out|=(in&BIT12)<<6;                 /* bit 6 of out */
    out|=(in&BIT28)<<21;                /* bit 7 of out */
    out|=(in&BIT17)<<9;                 /* bit 8 of out */
    out|=(in&BIT1)>>8;                  /* bit 9 of out */
    out|=(in&BIT15)<<5;                 /* bit 10 of out */
    out|=(in&BIT23)<<12;                /* bit 11 of out */
    out|=(in&BIT26)<<14;                /* bit 12 of out */
    out|=(in&BIT5)>>8;                  /* bit 13 of out */
    out|=(in&BIT18)<<4;                 /* bit 14 of out */
    out|=(in&BIT31)<<16;                /* bit 15 of out */
    out|=(in&BIT10)>>6;                 /* bit 16 of out */
    out|=(in&BIT2)>>15;                 /* bit 17 of out */
    out|=(in&BIT8)>>10;                 /* bit 18 of out */
    out|=(in&BIT24)<<5;                 /* bit 19 of out */
    out|=(in&BIT14)>>6;                 /* bit 20 of out */
    out|=(in&BIT32)<<11;                /* bit 21 of out */
    out|=(in&BIT27)<<5;                 /* bit 22 of out */
    out|=(in&BIT3)>>20;                 /* bit 23 of out */
    out|=(in&BIT9)>>15;                 /* bit 24 of out */
    out|=(in&BIT19)>>6;                 /* bit 25 of out */
    out|=(in&BIT13)>>13;                /* bit 26 of out */
    out|=(in&BIT30)<<3;                 /* bit 27 of out */
    out|=(in&BIT6)>>22;                 /* bit 28 of out */
    out|=(in&BIT22)>>7;                 /* bit 29 of out */
    out|=(in&BIT11)>>19;                /* bit 30 of out */
    out|=(in&BIT4)>>27;                 /* bit 31 of out */
    out|=(in&BIT25)>>7;                 /* bit 32 of out */
    *pout=out;
}

/****  IP_1 function ***/
void IP_1(l,r,out)
#ifndef M_I286
register
#endif
crypt_t l,r;
crypt_t out[2];
{
#ifndef M_I286
    register
#endif
    crypt_t out0,out1;

    out0=0;
    out1=0;

    out0|=(l&BIT8)<<7;        /* bit 1 of out[0] */
    out0|=(r&BIT8)<<6;        /* bit 2 of out[0] */
    out0|=(l&BIT16)<<13;      /* bit 3 of out[0] */
    out0|=(r&BIT16)<<12;      /* bit 4 of out[0] */
    out0|=(l&BIT24)<<19;      /* bit 5 of out[0] */
    out0|=(r&BIT24)<<18;      /* bit 6 of out[0] */
    out0|=(l&BIT32)<<25;      /* bit 7 of out[0] */
    out0|=(r&BIT32)<<24;      /* bit 8 of out[0] */
    out0|=(l&BIT7)>>2;        /* bit 9 of out[0] */
    out0|=(r&BIT7)>>3;        /* bit 10 of out[0] */
    out0|=(l&BIT15)<<4;       /* bit 11 of out[0] */
    out0|=(r&BIT15)<<3;       /* bit 12 of out[0] */
    out0|=(l&BIT23)<<10;      /* bit 13 of out[0] */
    out0|=(r&BIT23)<<9;       /* bit 14 of out[0] */
    out0|=(l&BIT31)<<16;      /* bit 15 of out[0] */
    out0|=(r&BIT31)<<15;      /* bit 16 of out[0] */
    out0|=(l&BIT6)>>11;       /* bit 17 of out[0] */
    out0|=(r&BIT6)>>12;       /* bit 18 of out[0] */
    out0|=(l&BIT14)>>5;       /* bit 19 of out[0] */
    out0|=(r&BIT14)>>6;       /* bit 20 of out[0] */
    out0|=(l&BIT22)<<1;       /* bit 21 of out[0] */
    out0|=(r&BIT22);          /* bit 22 of out[0] */
    out0|=(l&BIT30)<<7;       /* bit 23 of out[0] */
    out0|=(r&BIT30)<<6;       /* bit 24 of out[0] */
    out0|=(l&BIT5)>>20;       /* bit 25 of out[0] */
    out0|=(r&BIT5)>>21;       /* bit 26 of out[0] */
    out0|=(l&BIT13)>>14;      /* bit 27 of out[0] */
    out0|=(r&BIT13)>>15;      /* bit 28 of out[0] */
    out0|=(l&BIT21)>>8;       /* bit 29 of out[0] */
    out0|=(r&BIT21)>>9;       /* bit 30 of out[0] */
    out0|=(l&BIT29)>>2;       /* bit 31 of out[0] */
    out0|=(r&BIT29)>>3;       /* bit 32 of out[0] */

    out1|=(l&BIT4)<<3;        /* bit 1 of out[1] */
    out1|=(r&BIT4)<<2;        /* bit 2 of out[1] */
    out1|=(l&BIT12)<<9;       /* bit 3 of out[1] */
    out1|=(r&BIT12)<<8;       /* bit 4 of out[1] */
    out1|=(l&BIT20)<<15;      /* bit 5 of out[1] */
    out1|=(r&BIT20)<<14;      /* bit 6 of out[1] */
    out1|=(l&BIT28)<<21;      /* bit 7 of out[1] */
    out1|=(r&BIT28)<<20;      /* bit 8 of out[1] */
    out1|=(l&BIT3)>>6;        /* bit 9 of out[1] */
    out1|=(r&BIT3)>>7;        /* bit 10 of out[1] */
    out1|=(l&BIT11);          /* bit 11 of out[1] */
    out1|=(r&BIT11)>>1;       /* bit 12 of out[1] */
    out1|=(l&BIT19)<<6;       /* bit 13 of out[1] */
    out1|=(r&BIT19)<<5;       /* bit 14 of out[1] */
    out1|=(l&BIT27)<<12;      /* bit 15 of out[1] */
    out1|=(r&BIT27)<<11;      /* bit 16 of out[1] */
    out1|=(l&BIT2)>>15;       /* bit 17 of out[1] */
    out1|=(r&BIT2)>>16;       /* bit 18 of out[1] */
    out1|=(l&BIT10)>>9;       /* bit 19 of out[1] */
    out1|=(r&BIT10)>>10;      /* bit 20 of out[1] */
    out1|=(l&BIT18)>>3;       /* bit 21 of out[1] */
    out1|=(r&BIT18)>>4;       /* bit 22 of out[1] */
    out1|=(l&BIT26)<<3;       /* bit 23 of out[1] */
    out1|=(r&BIT26)<<2;       /* bit 24 of out[1] */
    out1|=(l&BIT1)>>24;       /* bit 25 of out[1] */
    out1|=(r&BIT1)>>25;       /* bit 26 of out[1] */
    out1|=(l&BIT9)>>18;       /* bit 27 of out[1] */
    out1|=(r&BIT9)>>19;       /* bit 28 of out[1] */
    out1|=(l&BIT17)>>12;      /* bit 29 of out[1] */
    out1|=(r&BIT17)>>13;      /* bit 30 of out[1] */
    out1|=(l&BIT25)>>6;       /* bit 31 of out[1] */
    out1|=(r&BIT25)>>7;       /* bit 32 of out[1] */

    out[0]=out0;
    out[1]=out1;
}

/****  IP function ***/
void IP(in,l,r)
crypt_t in[2];
crypt_t *l,*r;
{
#ifndef M_I286
    register
#endif
    crypt_t in0,in1,out0,out1;

    in0=in[0];
    in1=in[1];
    out0=0;
    out1=0;

    out0|=(in1&BIT26)<<25;  /* bit 1 of out[0] */
    out0|=(in1&BIT18)<<16;  /* bit 2 of out[0] */
    out0|=(in1&BIT10)<<7;   /* bit 3 of out[0] */
    out0|=(in1&BIT2)>>2;    /* bit 4 of out[0] */
    out0|=(in0&BIT26)<<21;  /* bit 5 of out[0] */
    out0|=(in0&BIT18)<<12;  /* bit 6 of out[0] */
    out0|=(in0&BIT10)<<3;   /* bit 7 of out[0] */
    out0|=(in0&BIT2)>>6;    /* bit 8 of out[0] */
    out0|=(in1&BIT28)<<19;  /* bit 9 of out[0] */
    out0|=(in1&BIT20)<<10;  /* bit 10 of out[0] */
    out0|=(in1&BIT12)<<1;   /* bit 11 of out[0] */
    out0|=(in1&BIT4)>>8;    /* bit 12 of out[0] */
    out0|=(in0&BIT28)<<15;  /* bit 13 of out[0] */
    out0|=(in0&BIT20)<<6;   /* bit 14 of out[0] */
    out0|=(in0&BIT12)>>3;   /* bit 15 of out[0] */
    out0|=(in0&BIT4)>>12;   /* bit 16 of out[0] */
    out0|=(in1&BIT30)<<13;  /* bit 17 of out[0] */
    out0|=(in1&BIT22)<<4;   /* bit 18 of out[0] */
    out0|=(in1&BIT14)>>5;   /* bit 19 of out[0] */
    out0|=(in1&BIT6)>>14;   /* bit 20 of out[0] */
    out0|=(in0&BIT30)<<9;   /* bit 21 of out[0] */
    out0|=(in0&BIT22);      /* bit 22 of out[0] */
    out0|=(in0&BIT14)>>9;   /* bit 23 of out[0] */
    out0|=(in0&BIT6)>>18;   /* bit 24 of out[0] */
    out0|=(in1&BIT32)<<7;   /* bit 25 of out[0] */
    out0|=(in1&BIT24)>>2;   /* bit 26 of out[0] */
    out0|=(in1&BIT16)>>11;  /* bit 27 of out[0] */
    out0|=(in1&BIT8)>>20;   /* bit 28 of out[0] */
    out0|=(in0&BIT32)<<3;   /* bit 29 of out[0] */
    out0|=(in0&BIT24)>>6;   /* bit 30 of out[0] */
    out0|=(in0&BIT16)>>15;  /* bit 31 of out[0] */
    out0|=(in0&BIT8)>>24;   /* bit 32 of out[0] */

    out1|=(in1&BIT25)<<24;  /* bit 1 of out[1] */
    out1|=(in1&BIT17)<<15;  /* bit 2 of out[1] */
    out1|=(in1&BIT9)<<6;    /* bit 3 of out[1] */
    out1|=(in1&BIT1)>>3;    /* bit 4 of out[1] */
    out1|=(in0&BIT25)<<20;  /* bit 5 of out[1] */
    out1|=(in0&BIT17)<<11;  /* bit 6 of out[1] */
    out1|=(in0&BIT9)<<2;    /* bit 7 of out[1] */
    out1|=(in0&BIT1)>>7;    /* bit 8 of out[1] */
    out1|=(in1&BIT27)<<18;  /* bit 9 of out[1] */
    out1|=(in1&BIT19)<<9;   /* bit 10 of out[1] */
    out1|=(in1&BIT11);      /* bit 11 of out[1] */
    out1|=(in1&BIT3)>>9;    /* bit 12 of out[1] */
    out1|=(in0&BIT27)<<14;  /* bit 13 of out[1] */
    out1|=(in0&BIT19)<<5;   /* bit 14 of out[1] */
    out1|=(in0&BIT11)>>4;   /* bit 15 of out[1] */
    out1|=(in0&BIT3)>>13;   /* bit 16 of out[1] */
    out1|=(in1&BIT29)<<12;  /* bit 17 of out[1] */
    out1|=(in1&BIT21)<<3;   /* bit 18 of out[1] */
    out1|=(in1&BIT13)>>6;   /* bit 19 of out[1] */
    out1|=(in1&BIT5)>>15;   /* bit 20 of out[1] */
    out1|=(in0&BIT29)<<8;   /* bit 21 of out[1] */
    out1|=(in0&BIT21)>>1;   /* bit 22 of out[1] */
    out1|=(in0&BIT13)>>10;  /* bit 23 of out[1] */
    out1|=(in0&BIT5)>>19;   /* bit 24 of out[1] */
    out1|=(in1&BIT31)<<6;   /* bit 25 of out[1] */
    out1|=(in1&BIT23)>>3;   /* bit 26 of out[1] */
    out1|=(in1&BIT15)>>12;  /* bit 27 of out[1] */
    out1|=(in1&BIT7)>>21;   /* bit 28 of out[1] */
    out1|=(in0&BIT31)<<2;   /* bit 29 of out[1] */
    out1|=(in0&BIT23)>>7;   /* bit 30 of out[1] */
    out1|=(in0&BIT15)>>16;  /* bit 31 of out[1] */
    out1|=(in0&BIT7)>>25;   /* bit 32 of out[1] */

    *l=out0;
    *r=out1;
}

/**********************************************************************
* 函数名：    pub_base_EnDes
* 函数功能：    加密算法
* 作者/时间：    
* 
* 参数：
*     输入：
*            long txdate  交易日期
*            char *acno     帐号
*         	 passwd         加密前密码
* 
*     输出： passwd 加密后密码
*
* 返 回 值: 
*           
* 修改历史：
*
********************************************************************/

void pub_base_EnDes(long txdate, char *acno, char *passwd )
{
    char mm0[17];
    passwd[PASSWD_LEN]='\0';
    strcpy(mm0,passwd);
    descrypt(mm0,DESKEY);
    strncpy( passwd,mm0,PASSWD_LEN );
    passwd[PASSWD_LEN]='\0';
}

/**********************************************************************
* 函数名：    pub_base_DesChk
* 函数功能：    密码检查
* 作者/时间：    
* 
* 参数：
*     输入： char *txdate    交易日期
*           char *acno      账号
*           char *passwd    加密前密码
*           char *ac_passwd 账户密码
* 
*     输出：无
*
* 返 回 值: 0:成功 非0:失败
*           
* 修改历史：
*
********************************************************************/

int    pub_base_DesChk(long txdate, char *acno, char *passwd, char *ac_passwd)
{
    char mm0[17];

    MEMSET_DEBUG(mm0,0x00,sizeof(mm0));
    strcpy( mm0, passwd );
   /* vtcp_log("mm0=[%s],ac_passwd=[%s]",mm0,ac_passwd);*/
	descrypt( mm0, DESKEY );
    mm0[PASSWD_LEN]='\0';

	/*vtcp_log("%s,%d,mm0=[%s],ac_passwd=[%s]",__FILE__,__LINE__,mm0,ac_passwd);*/
    if ( strncmp ( mm0, ac_passwd, PASSWD_LEN ) )
    {
	vtcp_log("%s,%d,密码错误",__FILE__,__LINE__);
        return(1);
    }
	vtcp_log("%s,%d,密码正确",__FILE__,__LINE__);
    return(0);
}

/** 有用 **/
void PC_1(in,c,d)
crypt_t in[2],*c,*d;
{
#ifndef M_I286
    register
#endif
    crypt_t in0,in1,out0,out1;

    in0=in[0];
    in1=in[1];
    out0=out1=0;

    out0|=(in1&BIT25)<<24;      /* bit 1 of out[0] */
    out0|=(in1&BIT17)<<15;      /* bit 2 of out[0] */
    out0|=(in1&BIT9)<<6;        /* bit 3 of out[0] */
    out0|=(in1&BIT1)>>3;        /* bit 4 of out[0] */
    out0|=(in0&BIT25)<<20;      /* bit 5 of out[0] */
    out0|=(in0&BIT17)<<11;      /* bit 6 of out[0] */
    out0|=(in0&BIT9)<<2;        /* bit 7 of out[0] */
    out0|=(in0&BIT1)>>7;        /* bit 8 of out[0] */
    out0|=(in1&BIT26)<<17;      /* bit 9 of out[0] */
    out0|=(in1&BIT18)<<8;       /* bit 10 of out[0] */
    out0|=(in1&BIT10)>>1;       /* bit 11 of out[0] */
    out0|=(in1&BIT2)>>10;       /* bit 12 of out[0] */
    out0|=(in0&BIT26)<<13;      /* bit 13 of out[0] */
    out0|=(in0&BIT18)<<4;       /* bit 14 of out[0] */
    out0|=(in0&BIT10)>>5;       /* bit 15 of out[0] */
    out0|=(in0&BIT2)>>14;       /* bit 16 of out[0] */
    out0|=(in1&BIT27)<<10;      /* bit 17 of out[0] */
    out0|=(in1&BIT19)<<1;       /* bit 18 of out[0] */
    out0|=(in1&BIT11)>>8;       /* bit 19 of out[0] */
    out0|=(in1&BIT3)>>17;       /* bit 20 of out[0] */
    out0|=(in0&BIT27)<<6;       /* bit 21 of out[0] */
    out0|=(in0&BIT19)>>3;       /* bit 22 of out[0] */
    out0|=(in0&BIT11)>>12;      /* bit 23 of out[0] */
    out0|=(in0&BIT3)>>21;       /* bit 24 of out[0] */
    out0|=(in1&BIT28)<<3;       /* bit 25 of out[0] */
    out0|=(in1&BIT20)>>6;       /* bit 26 of out[0] */
    out0|=(in1&BIT12)>>15;      /* bit 27 of out[0] */
    out0|=(in1&BIT4)>>24;       /* bit 28 of out[0] */
    out0|=(in0&BIT31)<<2;       /* bit 29 of out[0] */
    out0|=(in0&BIT23)>>7;       /* bit 30 of out[0] */
    out0|=(in0&BIT15)>>16;      /* bit 31 of out[0] */
    out0|=(in0&BIT7)>>25;       /* bit 32 of out[0] */

    out1|=(in0&BIT31)<<30;      /* bit 1 of out[1] */
    out1|=(in0&BIT23)<<21;      /* bit 2 of out[1] */
    out1|=(in0&BIT15)<<12;      /* bit 3 of out[1] */
    out1|=(in0&BIT7)<<3;        /* bit 4 of out[1] */
    out1|=(in1&BIT30)<<25;      /* bit 5 of out[1] */
    out1|=(in1&BIT22)<<16;      /* bit 6 of out[1] */
    out1|=(in1&BIT14)<<7;       /* bit 7 of out[1] */
    out1|=(in1&BIT6)>>2;        /* bit 8 of out[1] */
    out1|=(in0&BIT30)<<21;      /* bit 9 of out[1] */
    out1|=(in0&BIT22)<<12;      /* bit 10 of out[1] */
    out1|=(in0&BIT14)<<3;       /* bit 11 of out[1] */
    out1|=(in0&BIT6)>>6;        /* bit 12 of out[1] */
    out1|=(in1&BIT29)<<16;      /* bit 13 of out[1] */
    out1|=(in1&BIT21)<<7;       /* bit 14 of out[1] */
    out1|=(in1&BIT13)>>2;       /* bit 15 of out[1] */
    out1|=(in1&BIT5)>>11;       /* bit 16 of out[1] */
    out1|=(in0&BIT29)<<12;      /* bit 17 of out[1] */
    out1|=(in0&BIT21)<<3;       /* bit 18 of out[1] */
    out1|=(in0&BIT13)>>6;       /* bit 19 of out[1] */
    out1|=(in0&BIT5)>>15;       /* bit 20 of out[1] */
    out1|=(in0&BIT28)<<7;       /* bit 21 of out[1] */
    out1|=(in0&BIT20)>>2;       /* bit 22 of out[1] */
    out1|=(in0&BIT12)>>11;      /* bit 23 of out[1] */
    out1|=(in0&BIT4)>>20;       /* bit 24 of out[1] */

    *c=out0>>4;
    *d=((out0&0xf)<<24)|(out1>>8);
}

/** 有用 **/
void PC_2(c,d,out)
crypt_t c,d,out[2];
{
#ifndef M_I286
    register
#endif
    crypt_t in0,in1,out0,out1;

    out0=out1=0;
    in0=(c<<4)|((d>>24)&0xf);
    in1=d<<8;

    out0|=(in0&BIT14)<<13;      /* bit 1 of out[0] */
    out0|=(in0&BIT17)<<15;      /* bit 2 of out[0] */
    out0|=(in0&BIT11)<<8;       /* bit 3 of out[0] */
    out0|=(in0&BIT24)<<20;      /* bit 4 of out[0] */
    out0|=(in0&BIT1)>>4;        /* bit 5 of out[0] */
    out0|=(in0&BIT5)>>1;        /* bit 6 of out[0] */
    out0|=(in0&BIT3)>>4;        /* bit 7 of out[0] */
    out0|=(in0&BIT28)<<20;      /* bit 8 of out[0] */
    out0|=(in0&BIT15)<<6;       /* bit 9 of out[0] */
    out0|=(in0&BIT6)>>4;        /* bit 10 of out[0] */
    out0|=(in0&BIT21)<<10;      /* bit 11 of out[0] */
    out0|=(in0&BIT10)>>2;       /* bit 12 of out[0] */
    out0|=(in0&BIT23)<<10;      /* bit 13 of out[0] */
    out0|=(in0&BIT19)<<5;       /* bit 14 of out[0] */
    out0|=(in0&BIT12)>>3;       /* bit 15 of out[0] */
    out0|=(in0&BIT4)>>12;       /* bit 16 of out[0] */
    out0|=(in0&BIT26)<<9;       /* bit 17 of out[0] */
    out0|=(in0&BIT8)>>10;       /* bit 18 of out[0] */
    out0|=(in0&BIT16)>>3;       /* bit 19 of out[0] */
    out0|=(in0&BIT7)>>13;       /* bit 20 of out[0] */
    out0|=(in0&BIT27)<<6;       /* bit 21 of out[0] */
    out0|=(in0&BIT20)>>2;       /* bit 22 of out[0] */
    out0|=(in0&BIT13)>>10;      /* bit 23 of out[0] */
    out0|=(in0&BIT2)>>22;       /* bit 24 of out[0] */
 
    out0|=(in1&BIT9)>>16;       /* bit 25 of out[0] */
    out0|=(in1&BIT20)>>6;       /* bit 26 of out[0] */
    out0|=(in0&BIT31)<<4;       /* bit 27 of out[0] */
    out0|=(in1&BIT5)>>23;       /* bit 28 of out[0] */
    out0|=(in1&BIT15)>>14;      /* bit 29 of out[0] */
    out0|=(in1&BIT23)>>7;       /* bit 30 of out[0] */
    out0|=(in0&BIT30)>>1;       /* bit 31 of out[0] */
    out0|=(in1&BIT8)>>24;       /* bit 32 of out[0] */
    out1|=(in1&BIT19)<<18;      /* bit 1 of out[1] */
    out1|=(in1&BIT13)<<11;      /* bit 2 of out[1] */
    out1|=(in1&BIT1)>>2;        /* bit 3 of out[1] */
    out1|=(in1&BIT16)<<12;      /* bit 4 of out[1] */
    out1|=(in1&BIT12)<<7;       /* bit 5 of out[1] */
    out1|=(in1&BIT17)<<11;      /* bit 6 of out[1] */
    out1|=(in1&BIT7);           /* bit 7 of out[1] */
    out1|=(in1&BIT24)<<16;      /* bit 8 of out[1] */
    out1|=(in1&BIT2)>>7;        /* bit 9 of out[1] */
    out1|=(in1&BIT21)<<11;      /* bit 10 of out[1] */
    out1|=(in1&BIT14)<<3;       /* bit 11 of out[1] */
    out1|=(in1&BIT10)>>2;       /* bit 12 of out[1] */
    out1|=(in1&BIT18)<<5;       /* bit 13 of out[1] */
    out1|=(in1&BIT4)>>10;       /* bit 14 of out[1] */
    out1|=(in0&BIT29)<<14;      /* bit 15 of out[1] */
    out1|=(in0&BIT32)<<16;      /* bit 16 of out[1] */

    out[0]=out0;
    out[1]=out1;
}

/*** 有用 ***/
void KS( crypt_t ink[2], crypt_t outk[16][2])
{
    crypt_t c,d;

    PC_1(ink,&c,&d);

    c=((c<<1)|(c>>27&01))&0xfffffff;
    d=((d<<1)|(d>>27&01))&0xfffffff;
    PC_2(c,d,outk[0]);

    c=((c<<1)|(c>>27&1))&0xfffffff;
    d=((d<<1)|(d>>27&1))&0xfffffff;
    PC_2(c,d,outk[1]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[2]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[3]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[4]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[5]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[6]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[7]);

    c=((c<<1)|(c>>27&1))&0xfffffff;
    d=((d<<1)|(d>>27&1))&0xfffffff;
    PC_2(c,d,outk[8]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[9]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[10]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[11]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[12]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[13]);

    c=((c<<2)|(c>>26&1))&0xfffffff;
    d=((d<<2)|(d>>26&1))&0xfffffff;
    PC_2(c,d,outk[14]);

    c=((c<<1)|(c>>27&1))&0xfffffff;
    d=((d<<1)|(d>>27&1))&0xfffffff;
    PC_2(c,d,outk[15]);
}

/* 有用 */
void encrypttxt(u_char *intxt,u_char *outtxt, crypt_t k[16][2])
{
    long i;
    crypt_t in[2],out[2];
    crypt_t l,r,eout[2],sout,pout;

    in[0] = (long)*intxt|(long)*(intxt+1)<<8|
            (long)*(intxt+2)<<16|(long)*(intxt+3)<<24;
    in[1] = (long)*(intxt+4)|(long)*(intxt+5)<<8|
            (long)*(intxt+6)<<16|(long)*(intxt+7)<<24;

    IP(in,&l,&r);

    for(i=0;i<NN;i++)
    {
        E(r,eout);
        eout[0]^=k[i][0];
        eout[1]^=k[i][1];
        S(eout,&sout);
        P(sout,&pout);
        pout^=l;
        l=r;
        r=pout;
    }

    IP_1(l,r,out);

    *outtxt=out[0]&0xff;
    *(outtxt+1)=out[0]>>8&0xff;
    *(outtxt+2)=out[0]>>16&0xff;
    *(outtxt+3)=out[0]>>24&0xff;
    *(outtxt+4)=out[1]&0xff;
    *(outtxt+5)=out[1]>>8&0xff;
    *(outtxt+6)=out[1]>>16&0xff;
    *(outtxt+7)=out[1]>>24&0xff;
}

/**********************************************************************
* 函数名：    descrypt
* 函数功能：    加密算法
* 作者/时间：    
* 
* 参数：
*     输入：
* 
*     输出：
*
* 返 回 值: 
*           
* 修改历史：
*
********************************************************************/
/****Modified by SSH,2006.10.11,修改加密算法为我们最习惯的算法****/
void CBexchange(int f,unsigned char s[],unsigned char t[])
{
  switch (f) {
  case 0:
      t[0] = s[2];
      t[1] = s[5];
      t[2] = s[0];
      t[3] = s[4];
      t[4] = s[3];
      t[5] = s[1];
      break;
  case 1:
      t[0] = s[2];
      t[1] = s[5];
      t[2] = s[0];
      t[3] = s[4];
      t[4] = s[3];
      t[5] = s[1];
      break;
  }
}

void Shuff_Data(unsigned char *msg,int len1)
{
/* shuffle string is 19f2c8276ad0839b */

  int offset = 0;

  switch(len1) {
    case 16 :  
	  msg[offset++]^= 0x01;
    case 15 :
	  msg[offset++]^= 0x09;
    case 14 :
	  msg[offset++]^= 0x0f;
    case 13 :
	  msg[offset++]^= 0x02;
    case 12 :
	  msg[offset++]^= 0x0c;
    case 11 :
	  msg[offset++]^= 0x08;
    case 10 :
	  msg[offset++]^= 0x02;
    case 9 :
	  msg[offset++]^= 0x07;
    case 8 :
	  msg[offset++]^= 0x06;
    case 7 :
	  msg[offset++]^= 0x0a;
    case 6 :
	  msg[offset++]^= 0x0d;
    case 5 :
	  msg[offset++]^= 0x00;
    case 4 :
	  msg[offset++]^= 0x08;
    case 3:
	  msg[offset++]^= 0x03;
    case 2 :
	  msg[offset++]^= 0x09;
    case 1 :
	  msg[offset++]^= 0x0b;
  }
}
void descrypt( char *str, char *key){
	char pcMPin[7];
	char buf[32];
	MEMSET_DEBUG(buf,'\0',sizeof(buf));
	MEMCPY_DEBUG(buf,str,6);
	pub_base_strpack(buf);
	sprintf(pcMPin,"%06d",atoi(buf));/****对齐6位左补0****/
	MEMSET_DEBUG(buf,'\0',sizeof(buf));
	MEMCPY_DEBUG(buf,pcMPin,6);  
	CBexchange(0,(unsigned char *)buf,(unsigned char *)pcMPin);  
	Shuff_Data((unsigned char *)pcMPin,6);
	MEMCPY_DEBUG(str,pcMPin,6);
		
}
/****
void descrypt( char *str, char *key)
{
    crypt_t ink[2],outk[16][2];
    char  ch, str0[9],key0[8],cyt_str[9];
    int i;

    MEMSET_DEBUG(ink,'\0',sizeof(ink));
    MEMSET_DEBUG(outk,'\0',sizeof(outk));
    MEMSET_DEBUG(str0,'\0',sizeof(str0));
    MEMSET_DEBUG(key0,'\0',sizeof(key0));
    MEMSET_DEBUG(cyt_str,'\0',sizeof(cyt_str));

    key0[7] = '\0';
    if(strlen(key) >7 )
        key[7] = '\0';
    strcpy(key0,key);
    if(strlen(key) < 7)
        for(i = strlen(key); i < 7; i++)
            key0[i] = 'x';
    strncpy((char *)ink,key0,4);
    strncpy((char *)ink + 4,key0 + 4,3);
    KS(ink,outk);

    str0[8] = '\0';
    if(strlen(str) > 8)
        str[8] = '\0';
    strcpy(str0,str);
    if(strlen(str) < 8)
        for(i = strlen(str); i < 8; i++)
            str0[i] = 'x';
    encrypttxt((unsigned char *)str0,(unsigned char *)cyt_str,outk);
    for(i = 0;i < 8;i++)
    {
        ch = cyt_str[i];
        while(ch < 32 || ch > 126)
            ch = (ch + 95) % 256;
        str[i] = ch;
    }
}
****/

/** 有用 **/
void decrypttxt(intxt,outtxt,k)
u_char *intxt,*outtxt;
crypt_t k[16][2];
{
    long i;
    crypt_t l,r,eout[2],sout,pout;
    crypt_t in[2],out[2];

    in[0] = (long)*intxt|(long)*(intxt+1)<<8|
        (long)*(intxt+2)<<16|(long)*(intxt+3)<<24;
    in[1] = (long)*(intxt+4)|(long)*(intxt+5)<<8|
            (long)*(intxt+6)<<16|(long)*(intxt+7)<<24;

    IP(in,&l,&r);

    for(i=NN-1;i>=0;i--)
    {
        E(r,eout);
        eout[0]^=k[i][0];
        eout[1]^=k[i][1];
        S(eout,&sout);
        P(sout,&pout);
        pout^=l;
        l=r;
        r=pout;
    }

    IP_1(l,r,out);

    *outtxt=out[0]&0xff;
    *(outtxt+1)=out[0]>>8&0xff;
    *(outtxt+2)=out[0]>>16&0xff;
    *(outtxt+3)=out[0]>>24&0xff;
    *(outtxt+4)=out[1]&0xff;
    *(outtxt+5)=out[1]>>8&0xff;
    *(outtxt+6)=out[1]>>16&0xff;
    *(outtxt+7)=out[1]>>24&0xff;
}

void encrpt(in,out,k)
unsigned long in[2],out[2],k[16][2];
{
    long i;
    unsigned long l,r,eout[2],sout,pout;

    IP(in,&l,&r);

    for(i=0;i<NN;i++)
    {
        E(r,eout);
        eout[0]^=k[i][0];
        eout[1]^=k[i][1];
        S(eout,&sout);
        P(sout,&pout);
        pout^=l;
        l=r;
        r=pout;
    }

    IP_1(l,r,out);
}

void decrpt(in,out,k)
unsigned long in[2],out[2],k[16][2];
{
    long i;
    unsigned long l,r,eout[2],sout,pout;

    IP(in,&l,&r);

    for(i=NN-1;i>=0;i--)
    {
        E(r,eout);
        eout[0]^=k[i][0];
        eout[1]^=k[i][1];
        S(eout,&sout);
        P(sout,&pout);
        pout^=l;
        l=r;
        r=pout;
    }

    IP_1(l,r,out);
}

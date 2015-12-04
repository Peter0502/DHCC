#ifndef __sherrmsg_CH__
#define __sherrmsg_CH__
struct sherrmsg_c{
        char      rowid[20];
        char      txday[9];
        char      txtime[7];
        char      kinbr[6];
        char      tlrno[5];
        char      wssrno[7];
        char      tranname[26];
        char      txname[31];
        char      errseq[3];
        char      errmsg[101];
        char      oldinput[41];
        char      newinput[41];
        char      sdtlrno[5];
};
#endif 

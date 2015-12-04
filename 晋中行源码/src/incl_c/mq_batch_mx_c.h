#ifndef __mq_batch_mx_CH__
#define __mq_batch_mx_CH__
struct mq_batch_mx_c{
        long            chk_date;
        char            chk_ord[5];
        long            pack_ord;
        char            pay_acct[25];
        char            taxorg_code[13];
        long            entrust_date;
        char            pack_no[9];
        char            tra_no[9];
        double          tra_amt;
        char            flag[2];
};
#endif 

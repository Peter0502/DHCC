#ifndef __mq_tax_payment_CH__
#define __mq_tax_payment_CH__
struct mq_tax_payment_c{
        char            taxorg_code[13];
        long            entrust_date;
        char            pack_no[9];
        char            tra_no[9];
        char            pay_acct[25];
        char            handorg_name[61];
        double          tra_amt;
        char            taxvou_no[19];
        char            protocol_no[61];
        char            remark[61];
        long            tax_num;
        long            tax_date;
        double          tx_amt;
        long            seq_no;
        char            result[6];
        char            addword[61];
        long            chk_date;
        char            chk_ord[5];
        char            sts[2];
};
#endif 

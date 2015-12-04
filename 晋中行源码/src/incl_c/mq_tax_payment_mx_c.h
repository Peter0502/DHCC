#ifndef __mq_tax_payment_mx_CH__
#define __mq_tax_payment_mx_CH__
struct mq_tax_payment_mx_c{
        char            taxorg_code[13];
        long            entrust_date;
        char            tra_no[9];
        long            tax_num;
        char            tax_name[61];
        long            beg_date;
        long            end_date;
        double          tx_amt;
};
#endif 

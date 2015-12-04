#ifndef __mq_tax_info_CH__
#define __mq_tax_info_CH__
struct mq_tax_info_c{
        char            protocol_no[61];
        char            taxorg_code[13];
        char            taxpay_code[13];
        char            payopbk_code[13];
        char            paybk_code[13];
        char            pay_acct[21];
        char            handorg_name[61];
        long            tx_date;
        char            sts[2];
};
#endif 

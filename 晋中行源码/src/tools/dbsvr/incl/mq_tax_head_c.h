#ifndef __mq_tax_head_CH__
#define __mq_tax_head_CH__
struct mq_tax_head_c{
        char            src[13];
        char            msg_id[21];
        char            taxorg_code[13];
        long            entrust_date;
        char            pack_no[9];
        long            all_num;
        double          all_amt;
        char            handle_type[2];
        char            payee_bankno[13];
        char            payeeorg_code[13];
        char            payee_acct[33];
        char            payee_name[61];
        char            paybk_code[13];
        long            return_term;
        long            succ_num;
        double          succ_amt;
        long            stop_num;
        char            result[6];
        char            addword[61];
        char            sts[2];
        char            send_sts[2];
};
#endif 

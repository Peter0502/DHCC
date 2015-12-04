#ifndef __mq_batch_head_CH__
#define __mq_batch_head_CH__
struct mq_batch_head_c{
        char            msg_id[21];
        char            paybk_code[13];
        char            payee_bankno[13];
        long            chk_date;
        char            chk_ord[5];
        char            chk_type[2];
        long            all_num;
        double          all_amt;
        long            child_num;
        long            pack_ord;
        long            pack_num;
        double          pack_amt;
};
#endif 

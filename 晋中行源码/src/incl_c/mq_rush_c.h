#ifndef __mq_rush_CH__
#define __mq_rush_CH__
struct mq_rush_c{
        char            taxorg_code[13];
        long            entrust_date;
        char            cancel_no[9];
        long            ori_entrust_date;
        char            ori_tra_no[9];
        char            cancel_reson[61];
        char            cancel_answer[2];
        char            addwork_no[5];
};
#endif 

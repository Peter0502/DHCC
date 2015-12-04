#ifndef __redempt_rate_CH__
#define __redempt_rate_CH__
struct redempt_rate_c{
        char      rowid[20];
        char      prdt_code[21];
        char      rate_code[4];
        long      beg_date;
        long      end_date;
        long      min_days;
        long      max_days;
        double    redempt_rate;
        double    profit_rate;
};
#endif 

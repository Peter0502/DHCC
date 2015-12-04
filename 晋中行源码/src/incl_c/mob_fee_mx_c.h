#ifndef __mob_fee_mx_CH__
#define __mob_fee_mx_CH__
struct mob_fee_mx_c{
        char      rowid[20];
        double    tx_date;
        char      tel[7];
        double    trace_no;
        char      tx_code[5];
        char      ct_ind[2];
        char      br_no[6];
        char      ac_no[25];
        char      name[61];
        char      sign_ac_no[31];
        char      server_type[5];
        double    amt_should;
        double    amt_fact;
        double    cnt;
        double    lst_fee_date;
        double    print_cnt;
        char      memo[101];
        char      sts[2];
        char      filler1[21];
        char      filler2[31];
        char      filler3[51];
        char      filler4[101];
};
#endif 

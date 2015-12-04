#ifndef __mob_sendmail_pl_CH__
#define __mob_sendmail_pl_CH__
struct mob_sendmail_pl_c{
        char      rowid[20];
        char      tx_code[5];
        double    ac_id;
        char      ac_no[21];
        char      tel[7];
        double    trace_no;
        double    tx_date;
        double    tx_time;
        char      opt_no[4];
        char      mobile[51];
        char      message[141];
        char      timeflag[2];
        double    t_date;
        double    t_time;
        char      send_level[2];
        double    sene_times;
        double    send_date;
        double    send_time;
        double    sxf_amt;
        char      proc_flag[2];
        double    tt_no;
        char      tx_type[2];
        double    mobseq;
};
#endif 

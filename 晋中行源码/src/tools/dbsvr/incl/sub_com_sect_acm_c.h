#ifndef __sub_com_sect_acm_CH__
#define __sub_com_sect_acm_CH__
struct sub_com_sect_acm_c{
        char      rowid[20];
        char      ac_no[25];
        long      sub_ac_seqn;
        char      sub_ac_no[25];
        long      beg_date;
        long      end_date;
        double    rate;
        double    intst_acm;
        char      intst_type[2];
        char      sts[2];
        long      jz_date;
};
#endif 

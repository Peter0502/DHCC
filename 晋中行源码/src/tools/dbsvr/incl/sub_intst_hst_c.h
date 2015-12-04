#ifndef __sub_intst_hst_CH__
#define __sub_intst_hst_CH__
struct sub_intst_hst_c{
        char      rowid[20];
        char      br_no[6];
        char      ac_no[25];
        long      sub_ac_seqn;
        char      sub_ac_no[25];
        double    bic_bal;
        double    bic_acm;
        double    fct_intst;
        double    intst_tax;
        double    aic_bal;
        double    aic_acm;
        long      ic_date;
        long      intst_date;
        char      ac_ind[2];
        char      pt_ind[2];
        double    rate;
        long      jx_date;
        char      intst_flag[2];
        char      intst_sts[2];
};
#endif 

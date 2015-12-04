#ifndef __cif_shareholder_inf_CH__
#define __cif_shareholder_inf_CH__
struct cif_shareholder_inf_c{
        char      rowid[20];
        long      cif_no;
        long      shareholder_no;
        char      name[21];
        char      sex[2];
        char      type[2];
        long      tx_date;
        double    rate;
        char      id_type[2];
        char      id_no[21];
        char      br_no[21];
        char      ag_cr_no[21];
        long      up_date;
        char      tx_br_no[7];
        char      tel[5];
};
#endif 

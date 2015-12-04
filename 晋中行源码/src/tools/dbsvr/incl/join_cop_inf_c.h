#ifndef __join_cop_inf_CH__
#define __join_cop_inf_CH__
struct join_cop_inf_c{
        char      rowid[20];
        long      cif_no;
        long      cop_no;
        char      name[21];
        char      type[2];
        long      tx_date;
        char      id_type[2];
        char      id_no[21];
        char      br_no[21];
        char      ag_cr_no[21];
        long      up_date;
        char      tx_br_no[7];
        char      tel[5];
};
#endif 

#ifndef __cif_manage_inf_CH__
#define __cif_manage_inf_CH__
struct cif_manage_inf_c{
        char      rowid[20];
        long      cif_no;
        long      manage_no;
        char      name[21];
        char      sex[2];
        long      tx_date;
        char      type[2];
        char      id_type[2];
        char      id_no[21];
        long      up_date;
        char      br_no[7];
        char      tel[5];
};
#endif 

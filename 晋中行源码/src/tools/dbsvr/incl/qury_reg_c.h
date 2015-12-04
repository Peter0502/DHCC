#ifndef __qury_reg_CH__
#define __qury_reg_CH__
struct qury_reg_c{
        char      rowid[20];
        char      tx_br_no[6];
        char      tx_tel[7];
        long      tx_date;
        char      tx_code[5];
        long      tx_time;
        char      wherelist[1025];
        char      filler[61];
};
#endif 

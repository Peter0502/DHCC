#ifndef __idcode_chk_inf_CH__
#define __idcode_chk_inf_CH__
struct idcode_chk_inf_c{
        char      rowid[20];
        char      tx_code[5];
        char      tel[5];
        char      br_no[6];
        long      wk_date;
        char      msgid[21];
        char      msgref[21];
        char      msgno[5];
        char      chkbrno[6];
        long      req_date;
        char      bus_code[3];
        char      oper_user[51];
        char      idno[21];
        char      username[51];
        char      success[2];
        long      end_date;
        char      result[3];
        char      branch[51];
        char      photo[100];
};
#endif


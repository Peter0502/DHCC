#ifndef __auth_remote_inf_CH__
#define __auth_remote_inf_CH__
struct auth_remote_inf_c{
        char      rowid[20];
        long      trace_no;
        char      br_no[6];
        char      tel[5];
        long      tx_date;
        char      tx_code[5];
        char      auth_tel[5];
        char      filename[101];
        char      attachments[201];
        char      title[51];
};
#endif

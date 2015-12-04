#ifndef __tranpara_CH__
#define __tranpara_CH__
struct tranpara_c{
        char      rowid[20];
        char      trancode[5];
        char      subtxcode[5];
        long      recno;
        char      name[21];
        char      length[3];
        char      xdtl_no[7];
        char      xdtl_name[11];
        char      xdtl_flag[6];
        char      text[201];
};
#endif 

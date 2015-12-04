#ifndef __cif_cop_photo_CH__
#define __cif_cop_photo_CH__
struct cif_cop_photo_c{
        char      rowid[20];
        long      cif_no;
        long      photo_seqn;
  char      photo_type[2];
        char      photo[100];
};
#endif

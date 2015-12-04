#ifndef __cif_photo_inf_CH__
#define __cif_photo_inf_CH__
struct cif_photo_inf_c{
        char      rowid[20];
        long      cif_no;
        long      photo_seqn;
        char      photo_type[2];
        char      photo[100];
};
#endif

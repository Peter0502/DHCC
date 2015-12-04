#ifndef __chnl_marchinfo_CH__
#define __chnl_marchinfo_CH__
struct chnl_marchinfo_c{
        char            mrchno[16];
        char            mrchnm[21];
        char            br_no[6];
        char            mrchtype[3];
        char            mrchstat[2];
        char            mrchacno[25];
        char            mrchaddr[21];
        char            mrch_phon[17];
        char            mrchman[11];
        double          chrg;
        long            reg_date;
        long            beg_date;
        long            end_ate;
};
#endif 

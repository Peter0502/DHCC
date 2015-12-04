struct HostGlobal  
{
   short           idx;
   char            region[8];
   char            tbsdy[9] ;
   char            bhdate[9];
   char            chdate[2];
   char            trace[2];
   char            trfile[31];
   char            rptpath[21];
   long            adtno;
   long            recno;
   long            rskey;
} Global; 

struct hostd
{
   char            d_holidy[2];
   char            d_weekdy[2];
   char            d_tbsdy[9] ;
   char            d_nbsdy[9] ;
   char            d_nnbsdy[9];
   char            d_lbsdy[9] ;
   char            d_lmndy[9] ;
   char            d_tmndy[9] ;
   char            d_fnbsdy[9];
   int           d_ndycnt   ;
   int           d_nndycnt  ;
   int           d_ldycnt   ;
   int           d_jday     ;
} TodayDate;

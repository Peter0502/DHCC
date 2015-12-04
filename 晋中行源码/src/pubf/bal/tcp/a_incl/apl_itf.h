/*************************************************************************/
/*  Copyright (c) 1995                                                   */
/*  Unitronics Technology Corporation                                    */
/*  Shanghai Tandem Software Corporation                                 */
/*  QDCUB VERSION 1997.6.21                                              */
/*************************************************************************/


/*************************************************************************/
/*             APCTL/TP  INTERFACE  AREA  FOR  RBS  PROJECT              */
/*-----------------------------------------------------------------------*/
/*                                                                       */
/*    1. TITA    ( Transaction Input Text Area )                         */
/*                                                                       */
/*    2. TOTA    ( Transaction Output Text Area )                        */
/*                                                                       */
/*    3. MACTA   ( Multiple ACcount Text Area )                          */
/*                                                                       */
/*    4. TXLOG   ( Transaction LOG area )                                */
/*                                                                       */
/*    5. TXCOM   ( Transaction COMmon area )                             */
/*                                                                       */
/*************************************************************************/


/*************************************************************************/
/*                 APCTL/TP  INTERFACE  AREA  TITA                       */
/*-----------------------------------------------------------------------*/
/*       +---------------+------------------+------------------+         */
/*       |  APCTL HEADER | TITA BASIC LABEL |    TITA TEXT     |         */
/*       +---------------+------------------+------------------+         */
/*           7  BYTES    +   93  BYTES      +   924  BYTES  = 1024 BYTES */
/*           7  BYTES    +  109  BYTES      +   908  BYTES  = 1024 BYTES */
/*************************************************************************/
#define   TITA_LENGTH         (sizeof (T_TITA_RECD))
#define   TITA_HEADER_LENGTH  (sizeof (T_APCTLHDR))
#define   TITA_LABEL_LENGTH   (sizeof (T_TITA_LABEL))
#define   TITA                it_tita.labtex.label
#define   APLHDR              it_tita.apctlhdr
/* #define   TITA_TEXT_LENGTH    924 */ /* commented out by Hunter */
#define   TITA_TEXT_LENGTH    908       /* modified by Hunter */

typedef   struct   APCTLHDR   T_APCTLHDR;
  struct  APCTLHDR  {
    char   datatype;        /*  data type           */
    char   commit;          /*  commit/rollback     */
    char   rerun;           /*  rerun recovery data */
    char   msglng[4];       /*  message length      */
  }   ;

typedef   struct   TITA_LABEL   T_TITA_LABEL;
  struct  TITA_LABEL  {
    char   htrmseq[2];      /*  correction old trmseq*/
    char   hejfno[6];       /*  correction old ejfno*/
    char   opnbr[5];        /*  open account branch */
    char   kinbr[5];        /*  key in branch       */
    char   trmseq[2];       /*  terminal sequence   */
    char   ejfno[6];        /*  EJF no              */
    char   taskid[2];       /*  task id             */
    char   tmtype;          /*  terminal type       */
    char   aptype;          /*  application type    */
    char   txno[4];         /*  txno                */
    char   ptype;           /*  process type        */
    char   dscpt[3];        /*  description         */
    char   hcode;           /*  cancel transaction  */
    char   actno[14];       /*  account no          */
    char   txtype[2];       /*  tx type             */
    char   multtx;          /*  MATCA tx            */
    char   crdb;            /*  crdb                */
    char   supinit1[1];     /*  supervisor1 code    */
    char   supinit2[1];     /*  supervisor1 code    */
    char   reason[6];       /*  reason code 2*3     */
    char   tlrno[6];        /*  teller no           */ /* modified by Hunter */
	char   sptlrno[6];      /*  supervisor's teller no,   modified by Hunter */
	char   passwd[6];       /*  shuffled password   */ /* modified by Hunter */
    char   nbcd;            /*  nobook code         */
    char   trnmod;          /*  training mode       */
    char   sbtmod;          /*  subtitude mode      */
    char   curcd[3];        /*  currency code       */
    char   xcrt[8];         /*  currency rate       */
    char   txamt[12];       /*  transaction amount  */
  };

typedef   struct   TITA_LABTEX   T_TITA_LABTEX;
  struct  TITA_LABTEX  {
    T_TITA_LABEL   label;
    char           text[TITA_TEXT_LENGTH];
                       };

typedef   struct   TITA_RECD   T_TITA_RECD;
  struct  TITA_RECD  {
    T_APCTLHDR      apctlhdr;
    T_TITA_LABTEX   labtex;
                     };


/*************************************************************************/
/*                 APCTL/TP  INTERFACE  AREA  TOTA                       */
/*-----------------------------------------------------------------------*/
/*              +------------------+------------------+                  */
/*              | TOTA BASIC LABEL |     TOTA TEXT    |                  */
/*              +------------------+------------------+                  */
/*                   42 BYTES      +     858 BYTES  = 900 BYTES          */
/*************************************************************************/

#define   TOTA_LENGTH         (sizeof (T_TOTW_RECD))
#define   TOTA_LABEL_LENGTH   (sizeof (T_TOTW_LABEL))
#define   TOTW                it_totw.labtex.label
#define   TOTA_TEXT_LENGTH    858
#define   TOTA_LIMIT          24

typedef    struct    TOTW_LABEL     T_TOTW_LABEL;
 struct    TOTW_LABEL   {
   char    kinbr[5];        /*  key in branch       */
   char    trmseq[2];       /*  terminal sequence   */
   char    ejfno[6];        /*  EJF no              */
   char    taskid[2];       /*  task id             */
   char    tmtype;          /*  terminal type       */
   char    txdate[8];       /*  transaction date    */
   char    txtime[9];       /*  transaction time    */
   char    msgend;          /*  message end         */
   char    msgtype;         /*  message type        */
   char    msgno[4];        /*  message no          */
   char    msglng[3];       /*  message length      */
};
typedef    struct    TOTW_LABTEX    T_TOTW_LABTEX;
 struct    TOTW_LABTEX   {
           T_TOTW_LABEL    label;
           char            text[TOTA_TEXT_LENGTH];
};

typedef    struct    TOTW_RECD      T_TOTW_RECD;
 struct    TOTW_RECD     {
           T_TOTW_LABTEX   labtex;
           };


/*************************************************************************/
/*                 APCTL/TP  INTERFACE  AREA  MACTA                      */
/*-----------------------------------------------------------------------*/
/*  +-----------+-------------+--------+------------+--------------+     */
/*  | REGION ID | SERVER NAME | LENGTH |MACTA LABEL |  MACTA  TEXT |     */
/*  +-----------+-------------+--------+------------+--------------+     */
/*     8 BYTES  + 30 BYTES    + 4 BYTES + 93  BYTES + 924  BYTES         */
/*     8 BYTES  + 30 BYTES    + 4 BYTES + 109 BYTES + 908  BYTES         */
/*************************************************************************/
#define   MACTA_LENGTH         (sizeof (T_MACTA_RECD))
#define   MACTA_LABEL_LENGTH   (sizeof (T_MACTA_LABEL))
#define   MACTA                it_mactw.labtex.label
/* #define   MACTA_TEXT_LENGTH    924 */ /* commented out by Hunter */
#define   MACTA_TEXT_LENGTH    908       /* modified by Hunter */
#define   MACTA_SERVER_NAME    30
#define   MACTA_REGION_ID      8
#define   MACTA_MSGLNG_LENGTH  4
#define   MACTA_LIMIT          10

typedef   struct   MACTA_LABEL   T_MACTA_LABEL;
  struct  MACTA_LABEL  {
    char   htrmseq[2];      /*  correction old trmseq*/
    char   hejfno[6];       /*  correction old ejfno*/
    char   opnbr[5];        /*  open account branch */
    char   kinbr[5];        /*  key in branch       */
    char   trmseq[2];       /*  terminal sequence   */
    char   ejfno[6];        /*  EJF no              */
    char   taskid[2];       /*  task id             */
    char   tmtype;          /*  terminal type       */
    char   aptype;          /*  application type    */
    char   txno[4];         /*  txno                */
    char   ptype;           /*  process type        */
    char   dscpt[3];        /*  description         */
    char   hcode;           /*  cancel transaction  */
    char   actno[14];       /*  account no          */
    char   txtype[2];       /*  tx type             */
    char   multtx;          /*  MATCA tx            */
    char   crdb;            /*  crdb                */
    char   supinit1[1];     /*  supervisor1 code    */
    char   supinit2[1];     /*  supervisor1 code    */
    char   reason[6];       /*  reason code 2*3     */
    char   tlrno[6];        /*  teller no           */ /* modified by Hunter */
    char   sptlrno[6];      /*  supervisor's teller no,   modified by Hunter */
    char   passwd[6];       /*  shuffled password   */ /* modified by Hunter */
    char   nbcd;            /*  nobook code         */
    char   trnmod;          /*  training mode       */
    char   sbtmod;          /*  subtitude mode      */
    char   curcd[3];        /*  currency code       */
    char   xcrt[8];         /*  currency rate       */
    char   txamt[12];       /*  transaction amount  */
  };

typedef   struct   MACTA_LABTEX   T_MACTA_LABTEX;
  struct  MACTA_LABTEX  {
    T_MACTA_LABEL   label;
    char           text[MACTA_TEXT_LENGTH];
                        };

typedef   struct   MACTA_RECD   T_MACTA_RECD;
  struct  MACTA_RECD    {
    char            region_id[MACTA_REGION_ID];
    char            server_name[MACTA_SERVER_NAME];
    char            msglng[MACTA_MSGLNG_LENGTH];
    T_MACTA_LABTEX  labtex;
                        };


/*************************************************************************/
/*                 APCTL/TP  INTERFACE  AREA  TXLOG                      */
/*-----------------------------------------------------------------------*/
/*               +-------------------+------------------+                */
/*               | TXLOG BASIC LABEL |   TXLOG  TEXT    |                */
/*               +-------------------+------------------+                */
/*                     27 BYTES      +   504 BYTES  =  531 BYTES         */
/*************************************************************************/
#define   LOG_LENGTH          (sizeof (T_LOGW_RECD))
#define   LOG_LABEL_LENGTH    (sizeof (T_LOGW_LABEL))
#define   LOG                 it_logw.labtex.label
#define   LOG_TEXT_LENGTH     504
#define   LOG_LIMIT           6

typedef    struct    LOGW_LABEL     T_LOGW_LABEL;
 struct    LOGW_LABEL   {
   char    txrsut;          /*  transaction result  */
   char    cntid;           /*  count id            */
   char    pcnt[4];         /*  process count       */
   char    tcnt[4];         /*  transaction count   */
   char    afcls;           /*  after close         */
   char    tbsdy[8];        /*  today business date */
   char    msglng[3];       /*  LOG length          */
   char    aptype;          /*  application type    */
   char    txno[4];         /*  tx no               */
                        };
typedef    struct    LOGW_LABTEX    T_LOGW_LABTEX;
 struct    LOGW_LABTEX   {
           T_LOGW_LABEL    label;
           char            text[LOG_TEXT_LENGTH];
                         };

typedef    struct    LOGW_RECD      T_LOGW_RECD;
 struct    LOGW_RECD     {
           T_LOGW_LABTEX   labtex;
                         };


/*************************************************************************/
/*                 APCTL/TP  INTERFACE  AREA  TXCOM                      */
/*************************************************************************/
typedef  struct  TXCOM_AREA  T_TXCOM_AREA;
  struct TXCOM_AREA {
    char      txrsut;           /* txrsut , APCTL initial '0'             */
    char      tbsdy[8];         /* today business date , APCTL update     */
    char      nextdy[8];        /* next calendar date , APCTL update      */
    char      nbsdy[8];         /* next business date , APCTL update      */
    char      nnbsdy[8];        /* next next business date , APCTL update */
    char      lbsdy[8];         /* last business date , APCTL update      */
    char      jday[3];          /* jurian date , APCTL update             */
    char      txtime[9];        /* Transaction time                       */
    short int mttpfg;           /* MACTA tx count , APCTL initial 0       */
    short int totbox;           /* TOTA BOX count , APCTL initial 0       */
    short int logbox;           /* LOG BOX count  , APCTL initial 0       */
    short int ldycnt;           /* last business day count                */
    short int ndycnt;           /* next business day count                */
    short int nndycnt;          /* next next business day count           */
    char      acflg;            /* ACCOUNTING ONLINE , APCTL initial '0'  */
    char      apnofg;           /* int overflow status                    */
    long      rtncd;            /* return code    , APCTL initial 0       */
    char      prgnm[61];        /* program name , APCTL initial '    '    */
    char      msgid[5];         /* message id   , APCTL initial '    '    */
    long      response_id;      /* response destination , APCTL update    */
    char      source_path[100]; /* server to client server file           */
    char      target_path[100]; /* server to client client file           */
    } ;


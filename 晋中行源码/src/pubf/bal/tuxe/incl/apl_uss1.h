struct apctl_work_area {
   short mttp_flag;
   short replce_flag;
   short errmsg_code;
   short split_flag;
   short mactaed_flag;
   short macta_flag;
   short recovr_flag;
   short close_flag;
   short notp_flag;
   short callback_flag;
   short recursive_flag;
   long  response_id;
   char  receive_time[9];
   char  errmsg_note[80];
   char  orignl_labtex[TITA_LENGTH];
  } ;

struct apctl_split_area {
   char  msg1[216];
   char  msg2[234];
  } ;

struct apctl_mg_global  {
   short  idx;
   char   region[8];
   char   tbsdy[9];
   char   bhdate[9];
   char   chdate[2];
   char   trace[2];
   char   trfile[31];
   char   rptpath[21];
   long   adtno;
   long   recno;
   long   rskey;
  } ;

struct apctl_mg_date  {
   char       holidy[2];
   char       weekdy[2];
   char       tbsdy[9];
   char       nbsdy[9];
   char       nnbsdy[9];
   char       lbsdy[9];
   char       lmndy[9];
   char       tmndy[9];
   char       fnbsdy[9];
   short      ndycnt;
   short      nndycnt;
   short      ldycnt;
   short      jday;
   char       nextdy[9];
  } ;

struct apctl_wc_gdate  {
   char        sday[9];
   char        eday[9];
   int         mncnt;
   short       dtcnt;
   int         dycnt;
   short       funcd;
  };

/************************************************************************/
/*    DEFINE   CSTALK   TRANSACTION   PROTOCOL                          */
/************************************************************************/
# define  CS_READ_ONLY                 0
# define  CS_LOCAL_UPDATE              1
# define  CS_BLOB_LOCAL_UPDATE         2
# define  CS_GLOBAL_UPDATE             3
# define  CS_BLOB_GLOBAL_UPDATE        4
# define  CS_GLOBAL_UPDATE_NO_REC      5
# define  CS_BLOB_GLOBAL_UPDATE_NO_REC 6
# define  CS_PREPARE_BEGIN_PHASE       7
# define  CS_PREPARE_PHASE             8
# define  CS_COMMIT_BEGIN_PHASE        9
# define  CS_COMMIT_PHASE              10
# define  CS_ABORT_BEGIN_PHASE         11
# define  CS_ABORT_PHASE               12
# define  CS_BLOB_PREPARE_BEGIN_PHASE  13
# define  CS_BLOB_PREPARE_PHASE        14
# define  CS_BLOB_COMMIT_BEGIN_PHASE   15
# define  CS_BLOB_COMMIT_PHASE         16
# define  CS_BLOB_ABORT_BEGIN_PHASE    17
# define  CS_BLOB_ABORT_PHASE          18

/************************************************************************/
/*    DEFINE   DATA   TYPE                                              */
/************************************************************************/
# define  DATA_FROM_WS_CLIENT         '0'
# define  DATA_FROM_INTER_BANK        '1'
# define  DATA_FROM_BATCH_IO          '2'
# define  DATA_FOR_TWO_PHASE_COMMIT   '3'
# define  DATA_FROM_AP_MACTA          '5'
# define  DATA_FROM_AP_DUMMY_REQUEST  '6'
# define  DATA_FROM_AP_TO_AP          '9'
/************************************************************************/
/*    DEFINE   TERMINAL   TYPE                                          */
/************************************************************************/
# define  CS_TRMTYPE_WS         02
# define  CS_TRMTYPE_CD         11
# define  CS_TRMTYPE_ATM        12
# define  CS_TRMTYPE_POS        13
# define  CS_TRMTYPE_SSPB       14   /* SELF SERVICE PASSBOOK PRN */
# define  CS_TRMTYPE_FT         20   /* INTERBANK FILE TRANSFER   */
# define  CS_TRMTYPE_ARS        21   /* AUTO RECALL SYSTEM        */
# define  CS_TRMTYPE_OBC        22   /* OTHER BANK CHECK          */
# define  CS_TRMTYPE_OBCD       23   /* OTHER BANK CD             */
# define  CS_TRMTYPE_POLICE     24   /* POLICE NETWORK            */
# define  CS_TRMTYPE_KICC       25   /* VISA ORGANIZATION I       */
# define  CS_TRMTYPE_NICE       26   /* VISA ORGANIZATION II      */
# define  CS_TRMTYPE_BATCH      91   /* BATCH  TERMINAL           */
# define  CS_TRMTYPE_MASTER     92   /* MASTER TERMINAL           */

/************************************************************************/
/*    DEFINE   COMMIT  ROLLBACK                                         */
/************************************************************************/
# define  CS_COMMIT_JOB         '1'
# define  CS_ROLLBACK_JOB       '2'

/************************************************************************/
/*    DEFINE   SPLIT  TRANSACTION   TYPE                                */
/************************************************************************/
# define  CS_NORMAL_TX          '0'
# define  CS_RESERVE_TX         '1'
# define  CS_SPLIT_TX           '2'

/************************************************************************/
/*    DEFINE   TRANSACTION    RESULT                                    */
/************************************************************************/
# define  TX_SUCCESS            '0'
# define  TX_REJECT             '1'
# define  TX_DEADLOCK           '2'
# define  TX_MASTER_ERROR       '3'
# define  TX_LOGICAL_ERROR      '4'
# define  TX_RESERVE            '5'
# define  TX_RECURSIVE_LOOP     '9'

/************************************************************************/
/*    DEFINE   OTHER  TRANSACTION   MACRO                               */
/************************************************************************/
# define  CS_TRACE_ON                           '1'
# define  TR_RECV                               '0'
# define  TR_SEND                               '1'
# define  TR_PH1_CLIENT_SEND                    '2'
# define  TR_PH1_CLIENT_RECV                    '3'
# define  TR_PH2_CLIENT_SEND                    '4'
# define  TR_PH2_CLIENT_RECV                    '5'
# define  TR_PH1_SERVER_SEND                    '6'
# define  TR_PH1_SERVER_RECV                    '7'
# define  TR_PH2_SERVER_SEND                    '8'
# define  TR_PH2_SERVER_RECV                    '9'
# define  TR_CLIENT_SEND                        'a'
# define  TR_CLIENT_RECV                        'b'
# define  TR_CLIENT_NOWAIT_SEND                 'c'
# define  TR_CLIENT_NOWAIT_RESPONSE_RECV        'd'

# define  CS_FROM_POLL_CALL                      2
# define  CS_FROM_CALL_BACK                      1
# define  CS_FROM_NORMAL_TX                      0

/************************************************************************/
/*    DEFINE   APCTL  ERROR  MESSAGE                                    */
/************************************************************************/
# define  AP_CHTXDATE_PROCESS         9901
# define  AP_DATA_TYPE_ERROR          9902
# define  AP_NOUN_BUSINESS_DATE       9903
# define  AP_PROCESS_TYPE_ERROR       9904
# define  AP_MACTA_OVER_LIMIT         9905
# define  AP_TXRSUT_ERROR             9906
# define  AP_FAIL_TO_REQUEST          9907
# define  AP_MACTA_TO_PARENT          9908
# define  AP_RDBMS_ERROR              9909
# define  AP_COMTBL_UPD_FAIL          9910
# define  AP_CALL_BACK_UNDEF          9911
# define  AP_MACTA_LENGTH_EXCEED      9912
# define  AP_INVALID_REQUEST_LENGTH   9913
# define  AP_INVALID_TOTA_LENGTH      9914
# define  AP_NO_RESP_BUT_ERR          9920
# define  AP_XACT_NAME_DUPLICATE      9921
# define  AP_SUCCESS_BUT_SQLERR       9922
# define  AP_SUCCESS_BUT_TIMEOUT      9923
# define  AP_COMPRESS_ERROR           9924
# define  AP_UNCOMPRESS_ERROR         9925

/************************************************************************/
/*    DEFINE   APCTL  API   ERROR  MESSAGE                              */
/************************************************************************/
# define  AP_API_REQUEST_SUCCESS      0
# define  AP_API_REQUEST_ITSELF       1
# define  AP_API_REQUEST_FAIL         2

/************************************************************************/
/*    DEFINE   CSTALK   FAULT   MANAGER   XACT   ID                      */
/************************************************************************/
# define  CS_FAULT_PUT_STATUS              1000
# define  CS_FAULT_GET_STATUS_BY_COMB      1001
# define  CS_FAULT_GET_STATUS_BY_XNAME     1002
# define  CS_FAULT_GET_STATUS_BY_SNAME     1003
# define  CS_FAULT_PUT_REPORT              1004

/************************************************************************/
/*    DEFINE   APCTL   FAULT   MESSAGE                                  */
/************************************************************************/
# define  AP_FAULT_DATABASE          " ( RDBMS  SYSTEM  FAULT ) "
# define  AP_FAULT_LOGINID           " ( ORACLE LOGIN USERID NOT DEFINE ) "
# define  AP_FAULT_GLOBAL_TABLE      " ( GLOBAL TABLE NOT FOUND ) "
# define  AP_FAULT_BUSINESS_DATE     " ( BUSINESS DATE NOT FOUND ) "
# define  AP_FAULT_MACTA_ROLLBACK_FR " ( MACTA ROLLBACK FAIL REQUEST ) "
# define  AP_FAULT_MACTA_ROLLBACK_ER " ( MACTA ROLLBACK ERROR RETRUN ) "
# define  AP_FAULT_MACTA_COMMIT_FR   " ( MACTA COMMIT FAIL REQUEST ) "
# define  AP_FAULT_MACTA_COMMIT_ER   " ( MACTA COMMIT ERROR RETURN ) "
# define  AP_FAULT_MACTA_FR          " ( MACTA REQUEST FAIL ) "
# define  AP_XNAME_DUPLI_FR          " ( XACT NAME DUPLICATE )"
# define  AP_FAULT_COMPRESS          " ( COMPRESS ERROR ) "
# define  AP_FAULT_UNCOMPRESS        " ( UNCOMPRESS ERROR ) "

/************************************************************************/
/*    DEFINE   APCTL   TRANSACTION    LIMITATION                        */
/************************************************************************/
# define  CS_FAULT_MAX               500
# define  CS_FAULT_TITA_MAX          64
# define  CS_MACTA_MAX               750
# define  CS_REQUEST_MAX             750

/************************************************************************/
/*    DEFINE   APCTL   COMMAND   LINE   OPTION                          */
/************************************************************************/
# define  CS_AP_DEBUG_MODE          "-DEBUG"
# define  CS_AP_NOAUDIT_MODE        "-NOAUDIT"
# define  CS_AP_PCALL_MODE          "-PCALL"
# define  CS_AP_REPORT_MODE         "-REPORT"
# define  CS_AP_TIMEOUT_MODE        "-TIMEOUT"
# define  CS_AP_USER_MODE           "-USER"
# define  CS_AP_TEST_MODE           "-TEST"
# define  CS_AP_SERVER_MODE         "-S"
# define  CS_AP_ORA_MODE            "-ORA"
# define  CS_AP_COMPRESS_MODE       "-C"
# define  CS_AP_MODE_ON             1
# define  CS_AP_MODE_OFF            0

/************************************************************************/
/*    DEFINE   CSTALK  TRANSACTION    PRIORITY                          */
/************************************************************************/
# define  CS_PRI_LOW                0
# define  CS_PRI_MIDDLE             1
# define  CS_PRI_HIGH               2

/************************************************************************/
/*    DEFINE   CSTALK  SERVICE  TYPE                                    */
/************************************************************************/
# define  CS_NORMAL_SERVER          0
# define  CS_COMMON_SERVER          1
# define  CS_REQUEST_BY_DEMAND      2
# define  CS_NOWAIT_RELEASE         3
/* 1994/01/10 add for interbank , keep agent */
# define  CS_RESIDENT_SERVER        4
# define  CS_NORMAL_REQUEST         0
# define  CS_DYNAMIC_REQUEST        21


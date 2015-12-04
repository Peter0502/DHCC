#ifndef _DEF_H
#define _DEF_H

typedef struct TAG_BalInfo
{
	char			svcname[CSLServiceName + 1];
	char			autotran;  			/** Auto Transaction Flag 	**/
	int				tran_time;			/** Transaction Time 		**/
	char			tx_flag;			/** whether tx begin		**/
	unsigned long 	svcdigit;			/** Service Digit			**/
	char			callback_flg;		/** whether callback needed **/
	char   			callback_svc[CSLCBSName + 1]; /* callback svc name */
	char			pollcall_flg;		/** whether pollcall needed **/
	char   			pollcall_svc[CSLPCSName + 1]; /* pollcall svc name */
	unsigned long	pollcall_interval;	/** Poll Call Interval		**/
	int             reqversion;
	int				debug_mode;
	char			txno[CSLTxNo + 1];
	char			tlrno[CSLTlrNo + 1];
	
	int    			(*user_opendb)();
	int				(*user_closedb)();
	int    			(*user_svrinit)();
	int				(*user_svrdone)();
} T_BalInfo;

typedef struct TAG_TxName             /* Transaction Name */
{
   char     clnm[CSLClientName+1];
   char     svcnm[CSLServiceName+1];
   pid_t    svcpid;
   char     time[CSLTimeStamp+1];
   long     count;
} T_TxName;

typedef struct TAG_BHDROLD
{
   char     clnm[CSLClientName];
   char     svcnm[CSLServiceName];
   char     reqtype;                 /* request or reply type */
   char     cbs[16];                 /* call back service name */
   char     macflg;                  /* set to on if Macced */
   char     mac[5];                  /* message authentication code */
   char     result;                  /* transaction result */
   char     len[4];                  /* transaction data length */
} T_BHDROLD;

typedef struct TAG_BTROLD            /* BAL transaction record */
{
   T_BHDROLD   head;                 /* under construction */ 
   char     data[1];                 /* transaction data text */
} T_BTROLD;     

typedef struct TAG_BHDR
{
   char     clnm[CSLClientName];
   char     svcnm[CSLServiceName]; 
   char     reqtype;				 /* request or reply type */
   char		branch_no[CSLBranchNo];
   char		tty[CSLTTY];
   char     TITA_OR_8583;
   char		cbs[CSLCBSName];		 /* call back service name */
   char     macflg;                  /* set to on if Macced */
   char     mac[CSLMac];             /* message authentication code */
   char     result;                  /* transaction result */
   char     len[CSLData];            /* transaction data length */
} T_BHDR;

typedef struct TAG_BTR               /* BAL transaction record */
{
   T_BHDR   head;                    /* under construction */ 
   char     data[1];                 /* transaction data text */
} T_BTR;     

typedef struct TAG_InfGlobal         /* global information */
{
   short    regid;                   /* region index */
   char     regname[CSLRegName+1];   /* region name */
   char     tbday[CSLDate+1];        /* this business day */
   char     bhday[CSLDate+1];        /* batch processing day */
   short    isonline;                /* system online flag */
   short    tracelevel;              /* system trace level */
} T_infGlobal;

typedef struct TAG_InfDate           /* date information */
{
   char     tbday[CSLDate+1];        /* this business day */
   char     nbday[CSLDate+1];        /* next business day */
   char     nrday[CSLDate+1];        /* next real day */
   char     lbday[CSLDate+1];        /* last business day */
   char     tmeday[CSLDate+1];       /* this month end day */
   char     lmeday[CSLDate+1];       /* last month end day */
   short    nbgap;                   /* next business day gap */
   short    lbgap;                   /* last business day gap */
   char     weekday;                 /* which day of week */
   char     isholiday;                /* holiday flag */
   short    seqno;                   /* day sequence no. */
}  T_infDate;

typedef struct TAG_CommRqstHdr {
    char 		sender[CSLSender + 1];
    char 		rqst[CSLRqst + 1];
    long		len;
} T_CRQHD;

typedef struct TAG_CommRqst {
	T_CRQHD		head;
	char 		data[1];
} T_CommRqst;

typedef struct TAG_CommRplyHdr {
    char 		sender[CSLSender + 1];
    char 		rqst[CSLRqst + 1];
    int			rcode;
	long		len;
} T_CRPHD;

typedef struct TAG_CommRply {
	T_CRPHD		head;
    char 		data[1];
} T_CommRply;

/*******Common Service Data Type ********/
typedef struct TAG_CUnkwnSvc {
	char		mesg[32];
} T_CUnkwnSvc;


typedef struct TAG_CGblInfo {
	T_infGlobal	global;
	T_infDate	date;
} T_CGblInfo;

#endif

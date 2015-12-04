#ifndef _CONST_H
#define _CONST_H

#define CSuccess              0
#define CFailure              (-1)
#define CFlagOn               '1'
#define CFlagOff              '0'
#define CFlagMac              CFlagOn 
#define CFlagCompress         CFlagOn 
#define CBALTxnSucc           '0'
#define CBALTxnFail           '1'
#define CBALTxnError          '2'

#define POLLCALLSVC_PREFIX    "PC"
#define CALLBACKSVC_PREFIX    "CB"

#define BAL_COMMON_SERVICE    "BAL_COMSVC"

/******** Common service rqst type *********/
#define BAL_GINFO_RQST		  "BAL_GLOBAL_INFO"

#define BAL_COMM_PASSWD       "dhccnew"

/** Request Version  **/
#define BAL3_VERSION         0
#define BAL31_VERSION        1

/* Request Type, added by Sean */
#define BAL_OLD_REQ           '0'
#define	BAL_CRYPT_CLIENT_REQ  '4'
#define BAL_SVRREQ_WAIT		  '5'
#define BAL_SVRREQ_NOWAIT	  '6'
#define BAL_SVRREPLY_NOWAIT	  '7'
 
#define CSLTimeStamp          6
#define CSLClientName         5
#define CSLServiceName        15
#define CSLType               10
#define CSLDate               8
#define CSLRegName            7
#define CSLMac                8
#define CSLData               4
#define CSLMAXDATA            1024
#define CSLCBSName			  10
#define CSLPCSName			  10
#define CSLBranchNo			  5
#define CSLTTY                9    /*  10 modified by zhou in 20020408 */
#define CSLTxNo               4
#define CSLTlrNo              3
#define CSLKinBr              5

/** For BAL Debug Level  **/
#define BALDEBUG_LVL1         1
#define BALDEBUG_LVL2         2
#define BALDEBUG_LVL3         3

/*** For Bal Common Service ****/
#define CSLSender			  32
#define CSLRqst				  32

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN		  255
#endif
/*
#ifdef FOR_APCTL
#define CSLRESPONSE           24000 
#else
#define CSLRESPONSE           1024
#endif
*/
#define CSLRESPONSE           3000 
#define BAL_DEFAULT_TXTIME	  30

#define BAL_REGPOLL_EVENT               "BAL.Register.PollCall"
#define BAL_NOTIFYPOLL_EVENT			"Bal.Notify.PollCall"
#define PCAL_GBL_RQST_INTERVAL			60
#define POLLTHRESHOLD		  		5
#define TITA_MODE		  		 'Z'	
#define MODE_8583		  		 '8'    /* only no 'Z' */ 
#define MODE_8583_M		  		 'Y'    /*  */ 


#endif

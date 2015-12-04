#include "bal.h"
#include "global.h"

void _balmain( TPSVCINFO* ptpinfo )
{
	T_BTR		bal_req;
	T_BTR		bal_rply;
	int			rpl_ind_len;
	int			socket;
	int			len;
	char		result;
	char		tmpstr[5];
	unsigned	char	*data;

	struct timeval tvBeginTime;
	struct timeval tvEndTime;

	/** Debug **/
	if( g_balinfo.debug_mode >= BALDEBUG_LVL1 )
	{
		gettimeofday(&tvBeginTime, NULL);
	}

	(void) memcpy(bal_req.head.clnm , ptpinfo->data , CSLClientName);
	(void) memcpy(bal_req.head.svcnm , ptpinfo->data+5 , CSLServiceName);
	bal_req.head.reqtype='0';
	(void) memcpy(bal_req.head.branch_no , ptpinfo->data+21 , CSLBranchNo);
	(void) memcpy(bal_req.head.tty , ptpinfo->data+26 , CSLTTY);
	(void) memcpy(bal_req.head.len , ptpinfo->data+56 , 4);
	bal_req.head.macflg='0';
	bal_req.head.result='0';
	len=atoi(bal_req.head.len);
	(void) memcpy(bal_req.data , ptpinfo->data+60 , len);

	g_balinfo.reqversion = bal_get_req_version(&bal_req);

	/*** DEBUG **/
	if( g_balinfo.debug_mode >= BALDEBUG_LVL2 )
	{
		bal_dumpmesg("Receive Request:", bal_req.data, ptpinfo->len);
	}

	/** set lock wait time **/
	if ( bal_set_lockwaittime() != CSuccess )
	{
		bal_return_error(ptpinfo);
		return ;
	}
	
	/** if necessary, bal begin transaction **/
	if( CSuccess != bal_begin_tx(&g_balinfo) )
	{
		printf("ERROR(%d): bal_begin_tx - %s",
				balerrno,
		    	balstrerror(balerrno));
		bal_return_error(ptpinfo);
		return ;
	}
	
	/** convert request to internal datastructure, and init txcom **/	
	vtcp_log("%s,%d,Gethere\n",__FILE__,__LINE__);
	socket = bal_conv_in(&bal_req) ;
	if ( CSuccess != socket )
	{
		printf("ERROR(%d): convert to internal - %s",
		        balerrno,
		        balstrerror(balerrno));
		bal_return_error(ptpinfo);
		return ;
	}

	/** process request ***/
	vtcp_log("%s,%d,Gethere\n",__FILE__,__LINE__);
	if( CSuccess != bal_process_tx( &bal_req ) )
	{
		vtcp_log("bal_process_tx   error");
		printf("ERROR(%d): process request - %s",
		        balerrno,
		        balstrerror(balerrno));
		bal_return_error(ptpinfo);
		return ;
	}
	vtcp_log("%s,%d,Gethere\n",__FILE__,__LINE__);
	data=(unsigned char *) malloc (sizeof(unsigned char)*CSLRESPONSE);
	memset( data , 0x00 , sizeof(unsigned char)*CSLRESPONSE);
	/** convert app response to rpl_ind **/
	if( CSuccess != bal_conv_out( &bal_req, &rpl_ind_len, &bal_rply , data) )
	{
		vtcp_log("bal_conv_out   error");
		printf("ERROR(%d): put response - %s",
		balerrno,
		balstrerror(balerrno));
		bal_return_error(ptpinfo);
		return ;
	}
	vtcp_log("bal_conv_out   ok");
	result=bal_rply.head.result;
	memset(ptpinfo->data,0x00,rpl_ind_len);
	ptpinfo->len=rpl_ind_len;
	(void) memcpy(ptpinfo->data , bal_rply.head.clnm , CSLClientName);
	(void) memcpy(ptpinfo->data+5 , bal_rply.head.svcnm , CSLServiceName);
	(void) memcpy(ptpinfo->data+20 , "0", 1);
	(void) memcpy(ptpinfo->data+21 , bal_rply.head.branch_no , CSLBranchNo);
	(void) memcpy(ptpinfo->data+26 , bal_rply.head.tty , CSLTTY);
	(void) memcpy(ptpinfo->data+46 , bal_rply.head.result, 1);
	sprintf(tmpstr,"%04d",ptpinfo->len);
	(void) memcpy(ptpinfo->data+56, tmpstr, 4);
	(void) memcpy(ptpinfo->data+60, data , rpl_ind_len-60);

	/** DEBUG 
	g_balinfo.debug_mode = BALDEBUG_LVL2;**/
	if( g_balinfo.debug_mode >= BALDEBUG_LVL2 )
	{
		vtcp_log("===========Begin Log=========" );
		bal_dumpmesg("Send Reply:", bal_rply.data, rpl_ind_len);
	}
	vtcp_log("XXXX" );

	/*** Encryption Data ***/
		
	if ( CBALTxnSucc == result )
	{
		if( CSuccess != bal_commit_tx(&g_balinfo) )
		{
			vtcp_log("bal_commit_tx   error");
			bal_return_error(ptpinfo);
			return ;
		}
		vtcp_log("bal_commit_tx   ok");

		/** Debug  **/
		if( g_balinfo.debug_mode >= BALDEBUG_LVL1 )
		{
			vtcp_log("bal_commit_tx   ok1");
			gettimeofday(&tvEndTime, NULL);
			printf("[%s:%s:%s:%ld]:[%f]:SUCCESS",
					it_txcom.kinbr,
					g_balinfo.tlrno,
					g_balinfo.txno,
					it_txcom.tlsrno,
					(( tvEndTime.tv_sec  - tvBeginTime.tv_sec ) * 1000000.0 +
					(tvEndTime.tv_usec - tvBeginTime.tv_usec )) / 1000000.0 
			       );
		}
		vtcp_log("bal_commit_tx   ok0");

		return;
	}
	else
	{
		vtcp_log("bal_abort_tx   error11");
     	if( bal_abort_tx(&g_balinfo) != CSuccess )
		{
			vtcp_log("bal_abort_tx   error");
			bal_return_error(ptpinfo);
			return ;
		}
		vtcp_log("bal_abort_tx   ok");

		/** Debug  **/
		if( g_balinfo.debug_mode >= BALDEBUG_LVL1 )
		{
			gettimeofday(&tvEndTime, NULL);
			printf("[%s:%s:%s:%ld]:[%s],[%f]:FAIL",
					it_txcom.kinbr,
					g_balinfo.tlrno,
					g_balinfo.txno, it_txcom.tlsrno,RtCode,
					(( tvEndTime.tv_sec  - tvBeginTime.tv_sec ) * 1000000.0 +
					(tvEndTime.tv_usec - tvBeginTime.tv_usec )) / 1000000.0 
			       );
		}
		return ;
	}
}

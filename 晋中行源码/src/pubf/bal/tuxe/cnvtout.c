#include "bal.h"

int bal_conv_out( T_BTR*, int*, T_BTR**);
int bal31_conv_out( T_BTR*, int*, T_BTR**);
int bal3_conv_out( T_BTROLD*, int*, T_BTROLD**);

int bal_conv_out( T_BTR *bal_req, int *preply_len, T_BTR **pbal_rply )
{
	return bal31_conv_out(bal_req, preply_len, pbal_rply);
	if( g_balinfo.reqversion == BAL3_VERSION )
	{
		return bal3_conv_out((T_BTROLD*)bal_req, 
							 preply_len, 
							 (T_BTROLD**)pbal_rply);
	}
	else /** BAL31_VERSION **/
	{
		return bal31_conv_out(bal_req, preply_len, pbal_rply);
	}
}

int bal31_conv_out( T_BTR *bal_req, int *preply_len, T_BTR **pbal_rply )
{
	int   size = 0; 
	char  response_buf[CSLRESPONSE];
	char  result;
	char  str_size[CSLData + 1];

	memset( response_buf, 0, CSLRESPONSE );
   
#ifdef FOR_APCTL
{
	char apctl_result;

	cs18_putmsg(response_buf, &size, &apctl_result );
	if ( TX_SUCCESS == (apctl_result) )
	{
		result = CBALTxnSucc;
	}
	else
	{
		result = CBALTxnFail;
	} 
}
#endif

	*preply_len = size + sizeof(T_BHDR);
	*pbal_rply 	= (T_BTR*)tpalloc("CARRAY", NULL,  *preply_len);
	if ( NULL == *pbal_rply )
	{
        userlog("ERROR(%d): tpalloc pbal_rply - %s", 
                tperrno,
                tpstrerror(tperrno));
        balerrno = EBALTUXERR;
        return CFailure;
	}

	memcpy((*pbal_rply)->head.clnm,  bal_req->head.clnm,  CSLClientName);
	memcpy((*pbal_rply)->head.svcnm, bal_req->head.svcnm, CSLServiceName);
	memcpy((*pbal_rply)->head.cbs, bal_req->head.cbs, CSLCBSName);
	memcpy((*pbal_rply)->head.branch_no, 
		   bal_req->head.branch_no, 
		   CSLBranchNo);
	memcpy((*pbal_rply)->head.tty, 
		   bal_req->head.tty, 
		   CSLTTY);
	(*pbal_rply)->head.reqtype = bal_req->head.reqtype;
	(*pbal_rply)->head.macflg  = bal_req->head.macflg;
	(*pbal_rply)->head.TITA_OR_8583 = bal_req->head.TITA_OR_8583;
	memcpy((*pbal_rply)->data, response_buf, size );
	(*pbal_rply)->head.result = result;
	sprintf( str_size, "%4.0d", size );
	memcpy((*pbal_rply)->head.len, str_size, CSLData);

	return CSuccess;
}

int bal3_conv_out( T_BTROLD *bal_req, int *preply_len, T_BTROLD **pbal_rply )
{
	int   size = 0; 
	char  response_buf[CSLRESPONSE];
	char  result;
	char  str_size[CSLData + 1];

	memset( response_buf, 0, CSLRESPONSE );
   
#ifdef FOR_APCTL
{
	char apctl_result;

	cs18_putmsg(response_buf, &size, &apctl_result );
	if ( TX_SUCCESS == (apctl_result) )
	{
		result = CBALTxnSucc;
	}
	else
	{
		result = CBALTxnFail;
	} 
}
#endif

	*preply_len = size + sizeof(T_BHDROLD);
	*pbal_rply 	= (T_BTROLD*)tpalloc("CARRAY", NULL,  *preply_len);
	if ( NULL == *pbal_rply )
	{
        userlog("ERROR(%d): tpalloc pbal_rply - %s", 
                tperrno,
                tpstrerror(tperrno));
        balerrno = EBALTUXERR;
        return CFailure;
	}

	memcpy((*pbal_rply)->head.clnm,  bal_req->head.clnm,  CSLClientName);
	memcpy((*pbal_rply)->head.svcnm, bal_req->head.svcnm, CSLServiceName);
	(*pbal_rply)->head.reqtype = bal_req->head.reqtype;
	(*pbal_rply)->head.macflg  = bal_req->head.macflg;

	memcpy((*pbal_rply)->data, response_buf, size );
	(*pbal_rply)->head.result = result;
	sprintf( str_size, "%4.0d", size );
	memcpy((*pbal_rply)->head.len, str_size, 4);

	return CSuccess;
}


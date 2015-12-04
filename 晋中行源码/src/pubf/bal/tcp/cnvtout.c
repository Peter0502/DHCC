#include "bal.h"

int bal_conv_out( T_BTR*, int*, T_BTR*,unsigned char*);
int bal31_conv_out( T_BTR*, int*, T_BTR*,unsigned char*);
int bal3_conv_out( T_BTROLD*, int*, T_BTROLD**);

int bal_conv_out( T_BTR *bal_req, int *preply_len, T_BTR *pbal_rply ,unsigned char *data )
{
	return bal31_conv_out(bal_req, preply_len, pbal_rply, data);
}

int bal31_conv_out( T_BTR *bal_req, int *preply_len, T_BTR *pbal_rply , unsigned char *res )
{
	int   size = 0; 
	char  response_buf[CSLRESPONSE];
	/*
	unsigned char  *res;
	*/
	char  result;
	char  str_size[CSLData + 1];

	memset( response_buf, 0, CSLRESPONSE );
   
#ifdef FOR_APCTL
{
	char apctl_result;

	time18_putmsg(response_buf, &size, &apctl_result );
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
	memcpy((pbal_rply)->head.clnm,  bal_req->head.clnm,  CSLClientName);
	/*
	memcpy((pbal_rply)->head.svcnm, bal_req->head.svcnm, CSLServiceName);
	memcpy((pbal_rply)->head.branch_no, 
		   bal_req->head.branch_no, 
		   CSLBranchNo);
	memcpy((pbal_rply)->head.tty, 
		   bal_req->head.tty, 
		   CSLTTY);
	(pbal_rply)->head.reqtype = bal_req->head.reqtype;
	(pbal_rply)->head.macflg  = bal_req->head.macflg;
	*/
	memcpy(res, response_buf, size );
	(pbal_rply)->head.result = result;
	sprintf( str_size, "%4.0d", size );
	memcpy((pbal_rply)->head.len, str_size, CSLData);

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

	time18_putmsg(response_buf, &size, &apctl_result );
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


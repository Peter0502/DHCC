#include "bal.h"

/*** module variables ***/
char		*m_callback_buf;
short int	m_callback_buflen;

void 	bal_callback(TPSVCINFO*);
int 	bal_get_cbsname(char*, unsigned long);
void 	response_nowait(char**, short *, long*);

/***************************************************************************\
 *
 *  FUNCTION:       void bal_callback(TPSVCINFO *rqst);
 *  DESCRIPTION:    The call back service's start point, which receive non-reply
 *					request from server, this request is the reply of the 
 *					server's nowait-request. Function will allocate space for
 *					storing the the nowait-reply. the call user_call_back().
 *  INPUT:          rqst	
 *  OUTPUT:         none.
 *  RETURN:         none.
 *
\***************************************************************************/
void bal_callback(TPSVCINFO *rqst)
{
	T_BTR* cb_btr;
	char   str_len[CSLData + 1];
	int    int_len;

	cb_btr =(T_BTR*)rqst->data;

	userlog("In bal_callback");

	g_balinfo.reqversion = bal_get_req_version(cb_btr);

	/** set lock wait time **/
	if ( bal_set_lockwaittime() != CSuccess )
	{
		bal_return_error(cb_btr);
	}
	
/** if necessary, bal begin transaction **/
	if( CSuccess != bal_begin_tx(&g_balinfo) )
	{
		userlog("ERROR(%d): bal_begin_tx - %s",
				balerrno,
		    	balstrerror(balerrno));
	}
	
/** convert request to internal datastructure, and init txcom **/	
	if ( CSuccess != bal_conv_in(cb_btr) )
	{
		userlog("ERROR(%d): convert to internal - %s",
		        balerrno,
		        balstrerror(balerrno));
	}
	
	memset(str_len, 0, CSLData + 1);
    memcpy(str_len, cb_btr->head.len, CSLData);
	int_len = atoi(str_len);

    m_callback_buf 		= (char*)malloc(int_len);
    m_callback_buflen 	= (short int)int_len;
    memcpy(m_callback_buf, cb_btr->data, int_len);

	userlog("it_txcom.tbsdy [%s]",it_txcom.tbsdy);
    user_call_back();

	free(m_callback_buf);
	m_callback_buf = NULL;
	m_callback_buflen = 0;

	/*** transaction management ***/
#ifdef FOR_APCTL
	if ( TX_SUCCESS == it_txcom.txrsut)
	{
		if( CSuccess != bal_commit_tx(&g_balinfo) )
		{
			userlog("ERROR(%d): callback commit - %s",
			        balerrno,
			        balstrerror(balerrno));
		}
	}
	else
	{
     	if( bal_abort_tx(&g_balinfo) != CSuccess )
		{
			userlog("ERROR(%d): callback abort - %s",
			        balerrno,
			        balstrerror(balerrno));
		}
	}
#endif
	pubf_comm_free_buf();
    tpreturn(TPSUCCESS, 0, NULL, 0, 0L);
}

/***************************************************************************\
 *
 *  FUNCTION:		int bal_get_cbsname(char* cbsname, unsigned long SvcID)
 *  DESCRIPTION:	According to IP addr of host and process id, generate 
 *					call back service name of 10 bytes long. 
 *  INPUT:
 *  OUTPUT:
 *  RETURN:			0, success;
 *					-1, error.
\***************************************************************************/
int bal_get_cbsname(char* str_service, unsigned long long_service)
{
	sprintf(str_service, "%s%08X", CALLBACKSVC_PREFIX, long_service);
	return CSuccess;
}


/***************************************************************************\
 *
 *  FUNCTION:    	void response_nowait(response_buf,response_len,request_id);
 *					char 	**response_buf;
 *					short	*response_len;
 *					long	*request_id;
 *  DESCRIPTION:    This function is called by user-defined call back function.
 *					It get the reply from call back service. 
 *  INPUT:          none.
 *  OUTPUT:         response_buf, the reply of server, format is TOTA.
 *					response_len, the length of response_buf.
 *					request_id,   the id of the request in CD queue.
 *  RETURN:         none.
 *
\***************************************************************************/
void response_nowait(char **response_buf,short *response_len,long* request_id)
{
        *response_len = m_callback_buflen;
        *response_buf = m_callback_buf;
}

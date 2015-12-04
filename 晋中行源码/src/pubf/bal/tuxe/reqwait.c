#include "bal.h"
#include "balcrypt.h"

short int request_wait(char*, char*, short int, short int, char**, short int*);

/***************************************************************************\
 *
 *  FUNCTION:       short request_wait( request_name, request_buf, request_len,
 *                   					time_out,response_buf,response_len);
 *					char    *request_name, *request_buf, **response_buf;
 *					short   request_len, time_out, *response_len;
 *  DESCRIPTION:    Assemble app requst into BAL request, which is a struct of
 *					T_BTR, and send BAL request to service. After receving reply
 *					of service that also is a T_BTR struct, disassemble it to
 *					app's response_buf. 
 *					1. If request_name equal to self, do service.( copied from
 *						cs_sub.c of SCUB)
 *					2. A BAL request and a BAL reply are initialized, including
 *					   space allocation and fields initialization.
 *					3. The app request is assembled into the BAL request.
 *					4. Send BAL request to service, and receive BAL reply
 *					5. The BAL reply is disassembled, and  memory is allocated
 *					   for response_buf. Then the app reply is put into res-
 *					   ponse_buf. 
 *					6. The BAL request and the BAL reply are freed and return.
 *  INPUT:          request_name,	the requested service name.
 *					request_len,	the length of request.
 *					request_buf,	the data of app request.
 *					time_out,		ignored.
 *  OUTPUT:         response_len,   the length of app reply.
 *					response_buf,	the data of app reply.
 *	GLOBAL:			g_balinfo,
 *  RETURN:         0,	if success. 
 *					ERequestWaitError, if failed. Error infomation will be
 *					recorded by BAL Error Handling.
 *
\***************************************************************************/

short int request_wait(char			*request_name, 	
					   char			*request_buf,
					   short int 	request_len,
                       short int	time_out,
					   char			**response_buf,	
					   short int	*response_len)
{
    T_BTR       *pbal_rqst;
    T_BTR       *pbal_reply;
    long        bal_reply_len;
	long		bal_request_len;
	static char	*reply_buf = NULL;
	int         nOldVersion;

	if(reply_buf != NULL) /* free last allocated buffer */
	{
		free(reply_buf);
		reply_buf = NULL;
	}

    /******************* Initiate pbal_rqst **************************/
#ifdef APCTLHDR
	bal_request_len = sizeof(T_BHDR) + request_len;
#else
	bal_request_len = sizeof(T_BHDR) + request_len - 7;
#endif

	pbal_rqst 		= (T_BTR*)tpalloc("CARRAY", NULL, bal_request_len);
    if( pbal_rqst == NULL ) 
    {
        userlog("ERROR(%d): tpalloc pbal_rqst - %s", 
                tperrno,
                tpstrerror(tperrno));
        return ERequestWaitFail;
    }
    
    memcpy(pbal_rqst->head.clnm,  g_balinfo.svcname, CSLClientName);
    memcpy(pbal_rqst->head.svcnm, request_name,       CSLServiceName);
	pbal_rqst->head.reqtype = BAL_SVRREQ_WAIT;
    pbal_rqst->head.macflg  = CRYPT_XORMAC | CRYPT_CBC;
	nOldVersion  = g_balinfo.reqversion;
    g_balinfo.reqversion = BAL31_VERSION;

    /***************** Initiate pbal_reply ***************************/
	bal_reply_len 	= sizeof(T_BHDR);
	pbal_reply 		= (T_BTR*)tpalloc("CARRAY", NULL, bal_reply_len);
    if( pbal_reply == NULL ) 
    {
        userlog("ERROR(%d): tpalloc pbal_reply - %s", 
                tperrno,
                tpstrerror(tperrno));
        tpfree((char*)pbal_rqst);
        return ERequestWaitFail;
    }

#ifdef APCTLHDR
    sprintf(pbal_rqst->head.len, "%04ld", request_len);
    memcpy(pbal_rqst->data, request_buf,  request_len);
#else
    sprintf(pbal_rqst->head.len, "%04ld", request_len - 7);
    memcpy(pbal_rqst->data, &request_buf[7],  request_len - 7);
#endif

	/**********Encryption **********************************************/
	if( CSuccess != bal_encrypt(pbal_rqst) )
	{
		userlog("ERROR: bal_encrypt() failed!");
       	return ERequestWaitFail;
	}

    /****************** Send request to server ***********************/
    if( tpcall(request_name, 
			   (char*)pbal_rqst, 
			   bal_request_len,
               (char**)&pbal_reply, 
			   &bal_reply_len,0) == -1) 
	{
        g_balinfo.reqversion = nOldVersion;
		userlog("ERROR(%d): tpcall - %s", tperrno, tpstrerror(tperrno));
        /********* error handling *******************/
		*response_len = sizeof(T_TOTW_LABEL) + 30;
    	if( (reply_buf = (char*)malloc(*response_len)) == NULL) 
		{
       		userlog("ERROR: malloc() failed!");
       		tpfree((char*)pbal_rqst);
       		tpfree((char*)pbal_reply);
       		return ERequestWaitFail;
    	}
		memset(reply_buf, 0, *response_len);
		strcpy(&reply_buf[sizeof(T_TOTW_LABEL)], "Communication failed!");
		*response_buf = reply_buf;

       	tpfree((char*)pbal_rqst);
       	tpfree((char*)pbal_reply);
       	return ERequestWaitFail;
    }

    g_balinfo.reqversion = nOldVersion;
	
    /****************** allocate response buffer *********************/
    *response_len = bal_reply_len - sizeof(T_BHDR);
    if( (reply_buf = (char*)malloc(*response_len)) == NULL) 
    {
		userlog("ERROR(%d): malloc - %s",
		        errno,
		        strerror(errno)); 
        tpfree((char*)pbal_rqst);
        tpfree((char*)pbal_reply);
        return ERequestWaitFail;
    }
    
    memcpy(reply_buf, pbal_reply->data, *response_len);
	*response_buf = reply_buf;

    tpfree((char*)pbal_rqst);
    tpfree((char*)pbal_reply);
    return CSuccess;
}

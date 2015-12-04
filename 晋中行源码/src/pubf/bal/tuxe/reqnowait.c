#include "bal.h"
#include "balcrypt.h"

short request_nowait(char*, char*,char*, short, long*);

/***************************************************************************\
 *
 *  FUNCTION:       short request_nowait(request_name, request_buf, 
 *										 request_len, request_id);
 *                  char    *request_name, *request_buf;
 *                  short   request_len;
 *					long	request_id; 
 *  DESCRIPTION:    Assemble app requst into BAL request, which is a struct of
 *                  T_BTR, and send BAL request to service. 
 *                  1. A BAL request is initialized, including space allocation
 *                     and fields initialization. In field cbs of T_BHDR, filled
 *					   with call back service name.
 *                  2. The app request is assembled into the BAL request.
 *                  3. Send BAL request to service.
 *                  4. Free the BAL request, and return.
 *  INPUT:          request_name,   the requested service name.
 *                  request_len,    the length of request.
 *                  request_buf,    the data of app request.
 *  OUTPUT:         request_id,		the identifier of this request.
 *	GLOBAL:			g_balinfo
 *  RETURN:         0, if success.
 *                  ERequestNowaitError, if failed. Error infomation will be
 *                  recorded by BAL Error Handling.
 *
\***************************************************************************/
short request_nowait(char 		*request_name,
					 char       *backcall_name,
					 char		*request_buf, 
					 short int	request_len, 
					 long		*request_id)
{
    T_BTR       *pbal_rqst;
    long        bal_request_len;
	int         nOldVersion;

    /****************** Initiate pbal_rqst ***************************/
#ifdef APCTLHDR
    bal_request_len = sizeof(T_BHDR) + request_len;
#else
    bal_request_len = sizeof(T_BHDR) + request_len - 7;
#endif
    pbal_rqst = (T_BTR*)tpalloc("CARRAY", NULL, bal_request_len);
    if( pbal_rqst == NULL ) 
    {
		balerrno = EBALTUXERR;
		userlog("ERROR(%d): tpalloc pbal_rqst - %s",
		        tperrno,
		        tpstrerror(tperrno));
        return ERequestNowaitFail;
    }

    memcpy(pbal_rqst->head.clnm,  g_balinfo.svcname, CSLClientName);
    memcpy(pbal_rqst->head.svcnm, request_name,       CSLServiceName);

    pbal_rqst->head.reqtype = BAL_SVRREQ_NOWAIT; /* reqest_nowait request */
	userlog("back_svc [%s] [%c]",backcall_name,pbal_rqst->head.reqtype);
	/*
    memcpy(pbal_rqst->head.cbs, g_balinfo.callback_svc, CSLCBSName);
	*/
    memcpy(pbal_rqst->head.cbs, backcall_name, CSLCBSName);

	pbal_rqst->head.macflg  = CRYPT_XORMAC | CRYPT_CBC;
	pbal_rqst->head.TITA_OR_8583  = TITA_MODE;

	nOldVersion = g_balinfo.reqversion;
	g_balinfo.reqversion = BAL31_VERSION;

#ifdef APCTLHDR
    sprintf(pbal_rqst->head.len,"%04ld",request_len);
    memcpy(pbal_rqst->data,request_buf,request_len);
#else
    sprintf(pbal_rqst->head.len,"%04ld",request_len - 7);
    memcpy(pbal_rqst->data, &request_buf[7],request_len - 7);
#endif
	/**************** Encryption **********************************/
	if( CSuccess != bal_encrypt(pbal_rqst) )
	{
		userlog("ERROR:bal_encrypt() failed!");
        return ERequestNowaitFail;
	}
	
    /****************** Send request to server **********************/

    if( tpacall(request_name,
                (char*)pbal_rqst,
                bal_request_len,
                TPNOREPLY | TPNOTRAN) == -1) 
    {
		g_balinfo.reqversion = nOldVersion;
		userlog("In Nowait ERROR(%d): tpacall - %s", tperrno, tpstrerror(tperrno));
        tpfree((char*)pbal_rqst);
        return ERequestNowaitFail;
    }
/*
    tpforward(request_name,
                (char*)pbal_rqst,
                bal_request_len,
                0l) ;
*/
	g_balinfo.reqversion = nOldVersion;
    tpfree((char*)pbal_rqst);
	userlog("nowait ok");
    return CSuccess;
}


#include "bal.h"

void 	bal_pollcall(TPSVCINFO *);
int 	bal_get_pcsname(char*, unsigned long);
int 	bal_register_pollcall(unsigned long, unsigned long);
int 	bal_call_auditsvr(T_BTR*, int);
int 	bal_unregister_pollcall(unsigned long);
int 	bal_notify_pollserver(T_BalInfo*);

/***************************************************************************\
 *
 *  FUNCTION:       void bal_pollcall(TPSVCINFO *rqst);
 *  DESCRIPTION:    The poll call service's start point.
 *  INPUT:          rqst	
 *  OUTPUT:         none.
 *  RETURN:         none.
 *
\***************************************************************************/
void bal_pollcall(TPSVCINFO *rqst)
{
	static int poll_threshold = 0;
	static long last_time = 0;
	long  now_time;

	userlog("In bal_pollcall !!");
	poll_threshold++;
	if( poll_threshold > POLLTHRESHOLD )
	{
		if( CFailure == bal_notify_pollserver(&g_balinfo) )
		{
			userlog("ERROR(%d): bal_notify_pollserver failed! - %s",
			        balerrno,
			        balstrerror(balerrno));
			pubf_comm_free_buf();
			tpreturn(TPSUCCESS, 0, NULL, 0, 0L);
		}
		poll_threshold = 0;
	}

	now_time = time(NULL);
	if( now_time - last_time > PCAL_GBL_RQST_INTERVAL )
	{
		last_time = now_time;
	}
	
	bal_init_txcom();

	user_poll_call();

	pubf_comm_free_buf();
	tpreturn(TPSUCCESS, 0, NULL, 0, 0L);
}

/***************************************************************************\
 *
 *  FUNCTION:		int bal_get_pcsname
 *                  (char* szServuce, unsigned long SvcID)
 *  DESCRIPTION:	According to IP addr of host and process id, generate 
 *					polling service name of 10 bytes long. 
 *  INPUT:
 *  OUTPUT:
 *  RETURN:			0, success;
 *					-1, error.
\***************************************************************************/
int bal_get_pcsname(char* str_service, unsigned long long_service)
{
	sprintf(str_service, "%s%08X", POLLCALLSVC_PREFIX, long_service);
	return CSuccess;
}

/***************************************************************************\
 *
 *  FUNCTION:		int bal_register_pollcall(unsigned long svcdigit,
 *                                          unsigned long pc_interval);
 *  DESCRIPTION:	According to IP addr of host and process id, generate 
 *					call back service name of 10 bytes long. 
 *  INPUT:
 *  OUTPUT:
 *  RETURN:			0, success;
 *					-1, error.
\***************************************************************************/
int bal_register_pollcall(unsigned long svcdigit, unsigned long pc_interval)
{
	FBFR32* eventbuf;
	long  	flags;

	if( NULL == ( eventbuf = (FBFR32*)tpalloc("FML32", NULL, 0) ) )
	{
		userlog("ERROR(%d): tpalloc eventbuf failed! -- %s",
				tperrno,
				tpstrerror(tperrno));
		balerrno = EBALTUXERR;
		return CFailure;
	}

	if( -1 == Fadd32(eventbuf, SERVICE, (char*)&svcdigit, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 lSvcDidit ", Ferror32);
		balerrno = EBALTUXERR;
		tpfree((char*)eventbuf);
		return CFailure;
	}

	if( -1 == Fadd32(eventbuf, INTERVAL, (char*)&pc_interval, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 pc_interval", Ferror32);
		balerrno = EBALTUXERR;
		tpfree((char*)eventbuf);
		return CFailure;
	}
	
	flags = 0;
	if( -1 == Fadd32(eventbuf, FLAGS, (char*)&flags, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 lflags", Ferror32);
		balerrno = EBALTUXERR;
		tpfree((char*)eventbuf);
		return CFailure;
	}
userlog("Begin tppost");

	if( -1 == tppost(BAL_REGPOLL_EVENT, (char*)eventbuf, 0, TPSIGRSTRT) )
/*
	if( -1 == tppost(BAL_REGPOLL_EVENT, NULL, 0, TPSIGRSTRT) )
*/
	{
		userlog("ERROR(%d): tppost -- %s", tperrno, tpstrerror(tperrno));
		balerrno = EBALTUXERR;
		tpfree((char*)eventbuf);
		return CFailure;
	}
	
	tpfree((char*)eventbuf);
	return CSuccess;
}

int bal_subscribe_pollcall(unsigned long svcdigit, unsigned long pc_interval)
{
	FBFR32* eventbuf;
	long  	flags;

	if( NULL == ( eventbuf = (FBFR32*)tpalloc("FML32", NULL, 0) ) )
	{
		userlog("ERROR(%d): tpalloc eventbuf failed! -- %s",
				tperrno,
				tpstrerror(tperrno));
		balerrno = EBALTUXERR;
		return CFailure;
	}

	if( -1 == Fadd32(eventbuf, SERVICE, (char*)&svcdigit, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 lSvcDidit ", Ferror32);
		balerrno = EBALTUXERR;
		tpfree((char*)eventbuf);
		return CFailure;
	}

	if( -1 == Fadd32(eventbuf, INTERVAL, (char*)&pc_interval, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 pc_interval", Ferror32);
		balerrno = EBALTUXERR;
		tpfree((char*)eventbuf);
		return CFailure;
	}
	
	flags = 0;
	if( -1 == Fadd32(eventbuf, FLAGS, (char*)&flags, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 lflags", Ferror32);
		balerrno = EBALTUXERR;
		tpfree((char*)eventbuf);
		return CFailure;
	}
userlog("Begin tpsubscribe");

	if( -1 == (sub_serv =tpsubscribe(BAL_REGPOLL_EVENT, NULL,(char*)eventbuf, TPSIGRSTRT)) )
	{
		userlog("ERROR(%d): tpsubscribe -- %s", tperrno, tpstrerror(tperrno));
		balerrno = EBALTUXERR;
		tpfree((char*)eventbuf);
		return CFailure;
	}
	
	tpfree((char*)eventbuf);
	return CSuccess;
}
/***************************************************************************\
 *
 *  FUNCTION:		int bal_unregister_pollcall(unsigned long svcdigit);
 *  DESCRIPTION:	According to IP addr of host and process id, generate 
 *					call back service name of 10 bytes long. 
 *  INPUT:
 *  OUTPUT:
 *  RETURN:			0, success;
 *					-1, error.
\***************************************************************************/
int bal_unregister_pollcall(unsigned long svcdigit)
{
	FBFR32* eventbuf;
	long  flags;
	long  pc_interval;

	if( NULL == ( eventbuf = (FBFR32*)tpalloc("FML32", NULL, 0) ) )
	{
		userlog("ERROR(%d): tpalloc eventbuf failed! -- %s",
				tperrno,
				tpstrerror(tperrno));
		tpfree((char*)eventbuf);
		balerrno = EBALTUXERR;
		return CFailure;
	}

	if( -1 == Fadd32(eventbuf, SERVICE, (char*)&svcdigit, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 lSvcDidit ", Ferror32);
		tpfree((char*)eventbuf);
		balerrno = EBALTUXERR;
		return CFailure;
	}

	pc_interval = 0; /* unused */
	if( -1 == Fadd32(eventbuf, INTERVAL, (char*)&pc_interval, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 pc_interval", Ferror32);
		tpfree((char*)eventbuf);
		balerrno = EBALTUXERR;
		return CFailure;
	}
	
	flags = 1;
	if( -1 == Fadd32(eventbuf, FLAGS, (char*)&flags, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 flags", Ferror32);
		tpfree((char*)eventbuf);
		balerrno = EBALTUXERR;
		return CFailure;
	}

	if( -1 == tppost(BAL_REGPOLL_EVENT, (char*)eventbuf, 0, TPNOTRAN) )
	{
		userlog("ERROR(%d): tppost -- %s", tperrno, tpstrerror(tperrno));
		tpfree((char*)eventbuf);
		balerrno = EBALTUXERR;
		return CFailure;
	}
	
	tpfree((char*)eventbuf);
	return CSuccess;
}

int bal_notify_pollserver(T_BalInfo* balinfo)
{
	FBFR32* eventbuf;
	long	pc_interval;
	long	flags;

	if( NULL == ( eventbuf = (FBFR32*)tpalloc("FML32", NULL, 0) ) )
	{
		userlog("ERROR(%d): tpalloc eventbuf failed! -- %s",
				tperrno,
				tpstrerror(tperrno));
		balerrno = EBALTUXERR;
		return CFailure;
	}

	if( -1 == Fadd32(eventbuf, SERVICE, (char*)&balinfo->svcdigit, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 lSvcDidit ", Ferror32);
		tpfree((char*)eventbuf);
		balerrno = EBALTUXERR;
		return CFailure;
	}

	pc_interval = 0;
	if( -1 == Fadd32(eventbuf, INTERVAL, (char*)&pc_interval, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 pc_interval", Ferror32);
		tpfree((char*)eventbuf);
		balerrno = EBALTUXERR;
		return CFailure;
	}
	
	flags = 2;
	if( -1 == Fadd32(eventbuf, FLAGS, (char*)&flags, (FLDLEN32)0 ) )
	{
		userlog("ERROR(%d): Fadd32 lFlags", Ferror32);
		tpfree((char*)eventbuf);
		balerrno = EBALTUXERR;
		return CFailure;
	}
	if( -1 == tppost(BAL_NOTIFYPOLL_EVENT, (char*)eventbuf, 0, TPNOTRAN) )
	{
		userlog("ERROR(%d): tppost -- %s", tperrno, tpstrerror(tperrno));
		tpfree((char*)eventbuf);
		balerrno = EBALTUXERR;
		return CFailure;
	}

	tpfree((char*)eventbuf);
	return CSuccess;
}

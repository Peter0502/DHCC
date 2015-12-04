#include "bal.h"

void tpsvrdone( void );

void tpsvrdone( void )
{
#ifdef FOR_APCTL
	if ( (-1) == tpunadvertise(g_balinfo.svcname ) )
	{
		userlog("ERROR(%d): tpunadvertise(%s) - %s",
		        tperrno,
		        g_balinfo.svcname,
		        tpstrerror(tperrno));
	}
#endif

	if ( (-1) == bal_close_database() )
	{
		userlog("ERROR  close database err");
	}

/*
	if ( NULL != g_balinfo.user_closedb )
	{
		if ( CSuccess != g_balinfo.user_closedb() )
		{
			userlog("ERROR: user_closedb failed!");
		}
	}
	else
	{
		if ( (-1) == tpclose() )
		{
			userlog("ERROR(%d): tpclose - %s",
			        tperrno,
		    	    tpstrerror(tperrno));
		}
	}

	if ( (-1) == tpclose() )
	{
		userlog("ERROR(%d): tpclose - %s",
		        tperrno,
	    	    tpstrerror(tperrno));
	}
*/
	if( g_balinfo.pollcall_flg )
	{
	
		if (tpunsubscribe(sub_serv,TPNOTRAN) == -1){
			userlog("ERROR(%d): tpunsubscribe - %s",
			        balerrno,
		    	    balstrerror(balerrno));
		}
		if( CFailure == bal_unregister_pollcall(g_balinfo.svcdigit) )
		{
			userlog("ERROR(%d): bal_unregister_pollcall - %s",
			        balerrno,
		    	    balstrerror(balerrno));
		}
	
	}
}

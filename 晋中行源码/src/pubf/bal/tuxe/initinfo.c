#include "bal.h"

int bal_initinfo(T_BalInfo*, int, char**);

int bal_initinfo(T_BalInfo *balinfo, int argc, char** argv)
{
	double		pc_fint;		/** double poll call interval **/
	int			c;
	int         i, is_useropt;
	char   pcFileName[128];

	if( balinfo == NULL )
		return CFailure;
	
	/*** initialize bal info ***/
	balinfo->autotran 			= 1;
	balinfo->tran_time 			= 1;
	balinfo->tx_flag			= 0;
	balinfo->svcdigit			= 0;
	balinfo->callback_flg		= 0;
	balinfo->pollcall_flg		= 0;
	balinfo->pollcall_interval	= 0;
	balinfo->user_opendb		= NULL;
	balinfo->user_closedb		= NULL;
	balinfo->user_svrinit		= NULL;
	balinfo->user_svrdone		= NULL;
	
	memset(balinfo->callback_svc, 0, CSLCBSName + 1);
	memset(balinfo->pollcall_svc, 0, CSLPCSName + 1);
	
	/** Process command line options **/
	for( i = 1; i < argc; i++ )
	{
		if( 0 == memcmp(argv[i], "-p", 2 ) )
		{
			g_balinfo.pollcall_interval = (unsigned long)(atof(&argv[i][2])* 1000000L);
			userlog("%ld",g_balinfo.pollcall_interval);
			break;
		}
	}
	while ( ( c = getopt( argc, argv, "T:p:" ) ) != EOF )
	{
		switch ( (char)c )
		{
		case 'p':
			pc_fint 					= atof( optarg );
			userlog("In -p %s",optarg);
			balinfo->pollcall_interval 	= (unsigned long)(pc_fint * 1000000L);
			userlog("In -p %ld",balinfo->pollcall_interval);
			break; 
		case 'T':
			balinfo->autotran 			= 1;
			balinfo->tran_time			= atoi(optarg);
			if( 0 == balinfo->tran_time )
			{
				balinfo->tran_time		= BAL_DEFAULT_TXTIME;
			}
			break;
			/* options */
		default:
			break;
		}
	}

	balinfo->autotran 			= 1;
	is_useropt = 0;
	for( i = 1; i < argc; i++ )
	{
		if( is_useropt )
		{
			if( 0 == memcmp(argv[i], "-D", 2 ) )
			{
				g_balinfo.debug_mode = atoi(&argv[i][2]);
				break;
			}
		}
		if( 0 == memcmp(argv[i], "ZY", 2 ) )
		{
			is_useropt = 1;
		}
	}

	/***get service digit ***/
	balinfo->svcdigit = bal_get_svcdigit();

	/*** service name 	  ***/
#ifdef FOR_APCTL
	if ( 0 == special_server )
	{
	/*
		strcpy( balinfo->svcname, g_global.regname );
		strcat( balinfo->svcname, tp_name );   
	*/
		strcpy( balinfo->svcname, tp_name );
	}
	else
	{
		strcpy( balinfo->svcname, tp_name );
	} 
#else
	/*** Add code here if not for APCTL ***/
#endif

	/*** cbs name ***/
	if ( CFailure == bal_get_cbsname(balinfo->callback_svc, 
	                                 balinfo->svcdigit) )
	{
		userlog("ERROR: get call back service name failed!");
		return CFailure;
	}
	
	/*** set callback flag **/
	if( NULL != user_call_back )
	{
		balinfo->callback_flg = 1;
	}

	/*** get poll call service name ***/
	if( CFailure == bal_get_pcsname(balinfo->pollcall_svc,
	                                balinfo->svcdigit))
	{
		userlog("ERROR: get pollcall service name failed!");
		return CFailure;
	}
	
	if ((NULL != user_poll_call ) && ( 0 != balinfo->pollcall_interval))
	{
		balinfo->pollcall_flg = 1;
	}
	/****增加大额的初始化函数****/
	sprintf(pcFileName,"%s/etc/tin",getenv("WORKDIR"));
	if(hv_load_tagifno(pcFileName)<0){
		userlog("%s,%d,hv_load_taginfo error",__FILE__,__LINE__);
		return CFailure;
	}

	return CSuccess;
}

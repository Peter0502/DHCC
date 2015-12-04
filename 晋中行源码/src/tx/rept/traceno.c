#include "svrpub_c.h"
#include "public.h"

main( int argc,char *argv[] )
{
	long ldb;

	sql_database();

	pub_base_init_trace_no();

	ldb=get_tlsrno();
	printf(" ldb[%d]\n",ldb );
/******************程序出口************************/
/*
* 正常出口
*/
	sql_close();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before OK return: reply is [%s]",g_pub_tx.reply);
	WRITEMSG
	printf("Before OK return: reply is [%s]\n",g_pub_tx.reply);
	return (0);
/*
* 错误出口
*/
ErrExit:
	sql_close();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before return: reply is [%s]",g_pub_tx.reply);
	WRITEMSG
	printf("Before bad return: reply is [%s]\n",g_pub_tx.reply);
	return (1);
}


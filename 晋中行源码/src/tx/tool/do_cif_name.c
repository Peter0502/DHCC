/*************************************************************
* 按照分户改客户中心户名
**************************************************************/
#define MYSQLERR if( ret ) \
	{ \
		printf( "sql error[%d]!!!\n",ret ); \
		sprintf( acErrMsg, "sql error[%d]!!!",ret ); \
		WRITEMSG \
		goto ErrExit; \
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "public.h"
#include "svrpub_c.h"
#include "cif_basic_inf_c.h"

main(int argc, char *argv[])
{
	int ret = 0;
	struct cif_basic_inf_c cif_basic_inf_c;
	struct dd_mst_c dd_mst_c;
	struct td_mst_c td_mst_c;
	struct ln_mst_c ln_mst_c;
	char str[3000];
	char mystery[17];
	long lAc_id;
	int iAc_seqn;
	char reply[5];
	int i=0;

	ret=sql_database(); /*打开数据库*/
	MYSQLERR

	ret=sql_begin(); /*打开事务*/
	MYSQLERR

	ret= Cif_basic_inf_Dec_Upd( reply , "length(name)=0 or name is null " );
	MYSQLERR

	while(1)
	{
		ret=Cif_basic_inf_Fet_Upd( &cif_basic_inf_c , reply );
		if( ret==100 ) break;
		MYSQLERR

		ret=Dd_mst_Sel( reply,&dd_mst_c,"cif_no=%d",cif_basic_inf_c.cif_no);
		if( !ret ) strcpy( cif_basic_inf_c.name,dd_mst_c.name );
		else if( ret==100 )
		{
			ret=Td_mst_Sel( reply,&td_mst_c,"cif_no=%d",cif_basic_inf_c.cif_no);
			if( !ret ) strcpy( cif_basic_inf_c.name,td_mst_c.name );
			else if( ret==100 )
			{
				ret=Ln_mst_Sel( reply,&ln_mst_c,"cif_no=%d",
										cif_basic_inf_c.cif_no);
				if( !ret ) strcpy( cif_basic_inf_c.name,ln_mst_c.name );
				else if( ret==100 )
				{
					strcpy( cif_basic_inf_c.name,"no");
				}
				else
				{
					MYSQLERR
				}
			}
			else
			{
				MYSQLERR
			}
		}
		else
		{
			MYSQLERR
		}
		i++;
printf("%d,%d,%s\n",i,cif_basic_inf_c.cif_no,cif_basic_inf_c.name);
		ret=Cif_basic_inf_Upd_Upd(cif_basic_inf_c,reply);
		MYSQLERR
	}
	Cif_basic_inf_Clo_Upd();

OkExit:
	sql_commit();
	sql_close();
	printf("手动更改客户名称成功!!!!!\n");
	WRITEMSG
	return 0;
ErrExit:
	sql_rollback();
	sql_close();
	printf("手动更改客户名称失败!!!!!\n");
	WRITEMSG
	return 1;
}

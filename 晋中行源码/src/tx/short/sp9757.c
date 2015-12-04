/*************************************************
* 文 件 名:  sp9757.c
* 功能描述： 1111交易机构信息的快查
* 作    者:  lance
* 完成日期： 2003年05月30日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "cif_magn_inf_c.h"

int sp9757()
{
	int	ret;
	long cif_no;
	char cif_no_str[9];
	char cif_typ[2];
	
	struct 	cif_id_code_rel_c   g_cif_id_code_rel;	/*客户证件号码表*/
	struct  cif_basic_inf_c cif_basic_inf;
	struct  cif_magn_inf_c cif_magn_inf;
	
	/*清空*/
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_magn_inf,0x00,sizeof(struct cif_magn_inf_c));
		
	/*前台取值*/
	get_zd_long("0280",&cif_no);       				/*客户号*/


	/* 检查客户号是否已经存在 */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
						" cif_no=%ld " , cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"无此客户号，请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"读取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;;
	}
	sprintf(acErrMsg,"检查客户号是否已经存在 PASS!");
	WRITEMSG
		
	/* 客户号检查 */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* 检查是否对公客户 */
	if ( cif_typ[0]!='3' )
	{
		sprintf(acErrMsg,"非机构客户号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C101");
		goto ErrExit;
	}
	
	/* 查询机构客户信息表检查是否登记过 */
   	ret=Cif_magn_inf_Sel(g_pub_tx.reply,&cif_magn_inf, "cif_no=%ld",cif_no);
   	if( ret==0 )
   	{
       		sprintf( acErrMsg,"此客户信息已登记!" );
   	  	WRITEMSG
   	  	strcpy(g_pub_tx.reply,"C102");
   		goto ErrExit;
   	}else if( ret&&ret!=100 ){
         	sprintf( acErrMsg,"读取机构客户信息表失败!" );
   	  	WRITEMSG
   		goto ErrExit; 		   		
	}


   	ret=Cif_id_code_rel_Dec_Sel(g_pub_tx.reply, "cif_no=%ld",cif_no);
   	if( ret )
   	{
        sprintf( acErrMsg,"查询客户证件与客户号对照表异常!" );
        WRITEMSG
        goto ErrExit;
	}

	while(1)
	{
		ret=Cif_id_code_rel_Fet_Sel(&g_cif_id_code_rel,g_pub_tx.reply);
		if(ret==100)break;
   		else if( !ret )
   		{
			if(g_cif_id_code_rel.id_type[0]=='8')
				set_zd_data( "0620", g_cif_id_code_rel.id_no );
			if(g_cif_id_code_rel.id_type[0]=='A')
				set_zd_data( "0630", g_cif_id_code_rel.id_no );
			if(g_cif_id_code_rel.id_type[0]=='9')
				set_zd_data( "0300", g_cif_id_code_rel.id_no );	
			if(g_cif_id_code_rel.id_type[0]=='B')
				set_zd_data( "0330", g_cif_id_code_rel.id_no );								
   		}
   		else if( ret!=100 )
   		{
        	sprintf( acErrMsg,"查询客户证件与客户号对照表异常!" );
     	  	WRITEMSG
       		goto ErrExit;
   		}
	}

	Cif_id_code_rel_Clo_Sel();

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


/*************************************************
* 文 件 名:  sp9795.c
* 功能描述： 1102交易的快查
* 作    者:  rob 
* 完成日期： 2003年2月21日
*
* 修改记录： 
*   日   期: 2003年05月30日
*   修 改 人:lance
*   修改内容:增加客户号检查
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_per_inf_c.h"

int sp9795()
{
	int	ret=0,i=0;
	long cif_no;
	long bir_date;
	char sex[3];
	char cif_typ[2];
	struct 	cif_id_code_rel_c   g_cif_id_code_rel;	/*客户证件号码表*/
	struct 	cif_per_inf_c   cif_per_inf;	/*个人客户信息表*/
	
	/*清空*/
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&cif_per_inf,0x00,sizeof(struct cif_per_inf_c));	
	
	/*前台取值*/
	get_zd_long("0280",&cif_no);       				/*客户号*/

	/* 客户号检查 */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* 检查是否个人客户 */
	if ( cif_typ[0]!='1' && cif_typ[0]!='5' )
	{
		sprintf(acErrMsg,"非对私客户号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C011");
		goto ErrExit;
	}
	
	/* 查询个人客户信息表检查是否登记过 */
   	ret=Cif_per_inf_Sel(g_pub_tx.reply,&cif_per_inf, "cif_no=%ld",cif_no);
   	if( ret==0 )
   	{
       		sprintf( acErrMsg,"此客户信息已登记!" );
   	  	WRITEMSG
   	  	strcpy(g_pub_tx.reply,"C088");
   		goto ErrExit;
   	}else if( ret&&ret!=100 ){
         	sprintf( acErrMsg,"读取个人客户信息表失败!" );
   	  	WRITEMSG
   		goto ErrExit; 		   		
	}

   	ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
							"cif_no=%ld  and id_type='1' ",cif_no);
    	if( ret==0 )
   	{
		pub_base_strpack(g_cif_id_code_rel.id_no);
		i=check_bir_sex(g_cif_id_code_rel.id_no,&bir_date,sex);
		if(i)
		{
       			sprintf( acErrMsg,"调用取生日性别函数异常!" );
   	  		WRITEMSG
   			goto ErrExit;
		}
		set_zd_long( "0440", bir_date );
		set_zd_data( "0670", sex );		
   	}else if( ret && ret!=100 ){
       	sprintf( acErrMsg,"查询客户证件与客户号对照表异常!" );
   	  	WRITEMSG
   		goto ErrExit;
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

int check_bir_sex(char *id_no,long *bir_date,char *sex)
{
	char tmplist[20],tmp[20];
	char c;
	memset(tmp,0x00,sizeof(tmp));
	memset(tmplist,0x00,sizeof(tmplist));

	if(strlen(id_no)==18)
	{
		c=id_no[16];
		strcpy(tmplist,id_no+6);
		strncpy(tmp,tmplist,8);
		*bir_date=atol(tmp);
	
		if(c=='1'||c=='3'||c=='5'||c=='7'||c=='9')
			strcpy(sex,"M");
		else if(c=='2'||c=='4'||c=='6'||c=='8'||c=='0')
			strcpy(sex,"F");
	}
	else if(strlen(id_no)==15)
	{
		c=id_no[14];
		strcpy(tmplist,id_no+6);
		strncpy(tmp,tmplist,6);

		memset(tmplist,0x00,sizeof(tmplist));
		strcpy(tmplist,"19");
		strcat(tmplist,tmp);
		*bir_date=atol(tmplist);

		if(c=='1'||c=='3'||c=='5'||c=='7'||c=='9')
			strcpy(sex,"M");
		else if(c=='2'||c=='4'||c=='6'||c=='8'||c=='0')
			strcpy(sex,"F");
	}
	else return 1;

	return 0;
}

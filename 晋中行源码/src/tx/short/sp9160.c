/*************************************************
* 文 件 名: sp9160.c
* 功能描述：回显IC卡对应产品
* 作    者: YW
* 完成日期：2011年12月12日
* 修改记录：   
* 日    期:
* 修 改 人:
* 修改内容:
*insert into tx_sub_def 	values('9160','回显IC卡对应产品','sp9160','0','0');
*insert into tx_def values('9160','回显IC卡对应产品','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
*insert into tx_flow_def values('9160','0','9160','#$')
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "ic_subac_parm_c.h"
#include "mo_ic_ac_rel_c.h"

sp9160()  
{ 		
	/** 数据初始化 **/
	struct ic_subac_parm_c    s_ic_subac_parm;
	struct mo_ic_ac_rel_c     s_mo_ic_ac_rel;
	struct mdm_ac_rel_c       s_mdm_ac_rel;
	int ret=0;
	char wherelist[40];
	char filename[100];
	char cType[2];
	char cAc_no[20];
	FILE *fp;
	int i=0;

	memset (&s_ic_subac_parm, 0x00, sizeof(struct ic_subac_parm_c));
	memset (&s_mo_ic_ac_rel,  0x00, sizeof(struct mo_ic_ac_rel_c));
	memset (&s_mdm_ac_rel,    0x00, sizeof(struct mdm_ac_rel_c));
		
	memset(cType,0,sizeof(cType));
	memset(cAc_no,0,sizeof(cAc_no));
	/** 取值、赋值 **/
	get_zd_data( "0300" , cAc_no);  /* 账    号 */
	get_zd_data( "0670" , cType );	/* 查询类型 0:全部回显 1:回显客户未通子账户类型 2：回显客户开通子账户类型*/
	vtcp_log("cAc_no[%s],cType[%s]",cAc_no,cType );
	pub_base_AllDwnFilName( filename );
	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	ret=Ic_subac_parm_Dec_Sel( g_pub_tx.reply," sts='1' ");
	if( ret ) goto ErrExit;
	while(1)
	{
		memset (&s_ic_subac_parm, 0x00, sizeof(struct ic_subac_parm_c));
		ret = Ic_subac_parm_Fet_Sel(&s_ic_subac_parm,g_pub_tx.reply);
		if( ret==100 ) break;	
		else if( ret ) goto ErrExit;
		
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no='%s' ",cAc_no);
		if( ret )
		{
			sprintf(acErrMsg,"账号不对![%s]",cAc_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P183");
			goto ErrExit;
		}
		
		if(cType[0]=='1'){
			ret=sql_count("mo_ic_ac_rel","main_ac_no='%s' and sts='1' and sub_seqn=%d",s_mdm_ac_rel.ac_no,s_ic_subac_parm.sub_seqn);
			if(!ret){
				continue;
			}
		}if(cType[0]=='2'){
			ret=sql_count("mo_ic_ac_rel","main_ac_no='%s' and sts='1' and sub_seqn=%d",s_mdm_ac_rel.ac_no,s_ic_subac_parm.sub_seqn);
			if(!ret){
				continue;
			}
		}
		fprintf( fp,"0680|%-20s|%04d|\n", s_ic_subac_parm.sub_name, s_ic_subac_parm.sub_seqn );
		
		i++;
	}
	Ic_subac_parm_Clo_Sel();
	vtcp_log("i==[%d]",i );
	vtcp_log("0680|%-20s|%04d|\n", s_ic_subac_parm.sub_name, s_ic_subac_parm.sub_seqn);
	if(i)
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
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

/**************************************************************
* 文 件 名: sp8929.c
* 存折与一户通关联表维护
* 数据库表：ac_yht_rel_c
*
* 作    者: ligl
* 完成日期：2006年11月28日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ac_yht_rel_c.h"

sp8929()
{
	int ret = 0;
	char flag[2];        
	char wherelist[128];    /***查询条件***/
	char cCzac_no    [20];   /***存折  帐号***/
	char cYhtac_no   [20];   /***一户通帐号***/
	char tac_no[20];
	char tyht_ac_no[20];
	char tmpstr[128];
	char cFlag[2];
  int  ttlnum=0;
  double lmt_amt;
	double zz_lmt;
	long tac_id=0;
	long tyht_ac_id=0;
  FILE *fp;
  char filename[100];
  char cStat[7];
  char cCurName[11];
  char cIntsttype[13];
  struct ac_yht_rel_c ac_yht_rel;  /*存折一户通关联表*/
	struct mdm_ac_rel_c vmdm_ac_rel;
	struct dd_mst_c vdd_mst;
	memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&vdd_mst,0x0,sizeof(struct dd_mst_c));
	memset(&ac_yht_rel,0x0,sizeof(ac_yht_rel));
  memset(cFlag,0x0,sizeof(cFlag));
	memset(cCzac_no,0x0,sizeof(cCzac_no));
	memset(cYhtac_no,0x0,sizeof(cYhtac_no));
	memset(tac_no,0x0,sizeof(tac_no));
	memset(tyht_ac_no,0x0,sizeof(tyht_ac_no));
	
	pub_base_sysinit();
  
	/*-------- 取输入变量  ---------*/

	get_zd_data("0310",cCzac_no);
	get_zd_data("0320",cYhtac_no);
	get_zd_data("0670",cFlag);
	get_zd_double("0400",&lmt_amt);
	get_zd_double("0410",&zz_lmt);
	pub_base_old_acno(cCzac_no);
	pub_base_old_acno(cYhtac_no);
if(strlen(cCzac_no))
{
	memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_no='%s'",cCzac_no);
	if(ret==100)
  {
        	sprintf(acErrMsg,"介质帐号关系不存在该帐号!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P102" );
					goto ErrExit;
  }
  if(ret)
  {
        	sprintf(acErrMsg,"取介质帐号关系表出错!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"W011" );
					goto ErrExit;
  }
 	if(vmdm_ac_rel.note_sts[0]=='*')
  {
        	sprintf(acErrMsg,"介质状态不正常!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P172" );
					goto ErrExit;
  } 
  memset(&vdd_mst,0x0,sizeof(struct dd_mst_c));
 	ret=Dd_mst_Sel(g_pub_tx.reply,&vdd_mst,"ac_id=%ld",vmdm_ac_rel.ac_id);
  if(ret==100)
  {
  		       	sprintf(acErrMsg,"活期主文件不存在该帐户ID!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M108" );
							goto ErrExit;
  }
  if(ret)
  {
  						sprintf(acErrMsg,"读活期主表错!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"W015" );
							goto ErrExit;
  }
  if(vdd_mst.ac_sts[0]=='*')
  {
  		      	sprintf(acErrMsg,"该活期帐号状态为销户!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M003" );
							goto ErrExit;       
	}
	tac_id=vdd_mst.ac_id;
}
if(strlen(cYhtac_no))
{
  memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_no='%s'",cYhtac_no);
	if(ret==100)
  {
        	sprintf(acErrMsg,"介质帐号关系不存在该帐号!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P102" );
					goto ErrExit;
  }
  if(ret)
  {
        	sprintf(acErrMsg,"取介质帐号关系表出错!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"W011" );
					goto ErrExit;
  }
 	if(vmdm_ac_rel.note_sts[0]=='*')
  {
        	sprintf(acErrMsg,"介质状态不正常!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P172" );
					goto ErrExit;
  } 
  memset(&vdd_mst,0x0,sizeof(struct dd_mst_c));
 	ret=Dd_mst_Sel(g_pub_tx.reply,&vdd_mst,"ac_id=%ld",vmdm_ac_rel.ac_id);
  if(ret==100)
  {
  		       	sprintf(acErrMsg,"活期主文件不存在该帐户ID!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M108" );
							goto ErrExit;
  }
  if(ret)
  {
  						sprintf(acErrMsg,"读活期主表错!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"W015" );
							goto ErrExit;
  }
  if(vdd_mst.ac_sts[0]=='*')
  {
  		      	sprintf(acErrMsg,"该活期帐号状态为销户!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M003" );
							goto ErrExit;       
	}
	tyht_ac_id=vdd_mst.ac_id;
}
if(cFlag[0]!='1')
{
	
		ret=Ac_yht_rel_Sel( g_pub_tx.reply,&ac_yht_rel,"ac_id=%ld and yht_ac_id=%ld",tac_id,tyht_ac_id);
		ERR_DEAL
		set_zd_double("0410",ac_yht_rel.lmt_amt);
		set_zd_double("0420",ac_yht_rel.zz_lmt);
}
else
{
	if(tac_id)
	{
	 sprintf(tmpstr,"ac_id=%ld and ",tac_id);
	 strcat(wherelist,tmpstr);
	}
	if(tyht_ac_id)
	{
	 sprintf(tmpstr,"yht_ac_id=%ld and ",tyht_ac_id);
	 strcat(wherelist,tmpstr);
	}
	if(lmt_amt)
	{
		sprintf(tmpstr,"lmt_amt=%lf and ",lmt_amt);
	 	strcat(wherelist,tmpstr);
	}
	if(zz_lmt)
	{
		sprintf(tmpstr,"zz_lmt=%lf and ",zz_lmt);
	 	strcat(wherelist,tmpstr);
	}
		strcat(wherelist," 1=1 ");
	vtcp_log( "WHERE [%s]",wherelist );
	ret=Ac_yht_rel_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
		ret=Ac_yht_rel_Fet_Sel( &ac_yht_rel, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			fprintf( fp,"~存折帐号|一户通帐号|控制限额|转帐限额|\n" );
		}
		memset(tac_no,0x0,sizeof(tac_no));
	  memset(tyht_ac_no,0x0,sizeof(tyht_ac_no));
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_id=%ld",ac_yht_rel.ac_id);
		if(ret==100)
  	{
        	sprintf(acErrMsg,"介质帐号关系不存在该帐号!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P102" );
					goto ErrExit;
 	 }
  	if(ret)
  	{
        	sprintf(acErrMsg,"取介质帐号关系表出错!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"W011" );
					goto ErrExit;
  	}
  	
	  memcpy(tac_no,vmdm_ac_rel.ac_no,sizeof(vmdm_ac_rel.ac_no));
	  memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_id=%ld",ac_yht_rel.yht_ac_id);
		if(ret==100)
  	{
        	sprintf(acErrMsg,"介质帐号关系不存在该帐号!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P102" );
					goto ErrExit;
 	 }
  	if(ret)
  	{
        	sprintf(acErrMsg,"取介质帐号关系表出错!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"W011" );
					goto ErrExit;
  	}
	  memcpy(tyht_ac_no,vmdm_ac_rel.ac_no,sizeof(vmdm_ac_rel.ac_no));
    fprintf( fp, "%s|%s|%.2lf|%.2lf|\n",tac_no,tyht_ac_no,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);	
		ttlnum++;
	}

	ret=Ac_yht_rel_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}
}
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

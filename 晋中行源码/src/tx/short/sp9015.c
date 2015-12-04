/*************************************************************
* 文 件 名: sp9015.c
* 功能描述：短信通知扣款信息查询
*
* 作    者: yp
* 完成日期: 2014年12月01日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS, YW_CNT_IND, YW_PERCENT) values ('9015', '短信通知扣款信息查询', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100', '0', '4', '0', '3', null);
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('9015', 0, '9015', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('9015', '短信通知扣款信息查询', 'sp9015', '0', '0');

**************************************************************/

#define ERR_DEAL if(ret) {\
									sprintf(acErrMsg,"sql error"); \
	                WRITEMSG \
	                goto ErrExit;}

#define EXTERN
#include "public.h"
#include "mob_acct_type_c.h"

  
sp9015()
{

	int sday; /**签约时间与扣款时间之间的天数**/
	int flag=0;
  long sys_date=0;
  long date1=0;
  long date2=0;
  int ttlnum=0;			/**读取的总条数**/
  int ret=0;
  int count=0;     /**未收费次数**/
  
  char sac_no[25];
  char temp[512];
  char qflag[8];
  char sflag[8];
  char tmp_date1[9];
  char tmp_date2[9];
  char sname[50];
  char filename[100];
  char wherelist[1024];
  
  FILE *fp;
  
	
 struct mob_acct_type_c sMob_acct_type; 
	
  memset(wherelist,0x00,sizeof(wherelist));
  memset(sac_no,0x00,sizeof(sac_no));
  memset(temp,0x00,sizeof(temp));
	memset(sname,0x00,sizeof(sname));
	memset(tmp_date1,0x00,sizeof(tmp_date1));
	memset(tmp_date2,0x00,sizeof(tmp_date1));
	memset(filename,0x00,sizeof(filename));
  memset(&sMob_acct_type,0x00,sizeof( sMob_acct_type));

	/**-----初始化全局变量-------**/ 	
	pub_base_sysinit();
	
	get_zd_data("0380",sac_no);
	get_zd_data("0250",sname );
	get_zd_long("0290",&date1);
	get_zd_long("0440",&date2);
	get_zd_long("0050",&sys_date);
	
		vtcp_log("%s %d  sname=[%s]",__FILE__,__LINE__,sname);
		vtcp_log("%s %d  sac_no=[%s]",__FILE__,__LINE__,sac_no);
		vtcp_log("%s %d  date1=[%ld]",__FILE__,__LINE__,date1);
		vtcp_log("%s %d  sys_date=[%ld]",__FILE__,__LINE__,sys_date);	

	/**组成查询**/
	memset(wherelist,'\0',sizeof(wherelist));
	
	pub_base_strpack( sac_no );
	if(strlen(sac_no))
	{
		memset(temp,0x00,sizeof(temp));
		sprintf( temp," ac_no='%s' and ",sac_no);
		strcat( wherelist,temp );
	}
	
		if(date1)
	{
		memset(temp,0x00,sizeof(temp));
		sprintf( temp," tx_date>=%ld and ",date1);
		vtcp_log("%s %d  temp=[%s]",__FILE__,__LINE__,temp);
		strcat( wherelist,temp );
	}
	if( date2 )
	{
		memset(temp,0x00,sizeof(temp));
		sprintf( temp," tx_date<=%ld and ",date2);
		strcat( wherelist,temp );
	}


/*
	strcat(wherelist," server_type in ('1001','1002','1003') and sfbz='1' and mob_sts='1' and br_no = '%s' order by ac_no ",g_pub_tx.br_no);
*/
	strcat(wherelist," server_type in ('1001','1002') and sfbz='1' and mob_sts in ('1','2')  order by ac_no ");
	
	vtcp_log("%s %d mob_acct_type where [%s]",__FILE__,__LINE__,wherelist);
	
	ret=Mob_acct_type_Dec_Sel(g_pub_tx.reply,wherelist);
	ERR_DEAL
	
	ttlnum=0;

	while(1)
	{
		 ret=Mob_acct_type_Fet_Sel(&sMob_acct_type,g_pub_tx.reply);
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			vtcp_log("%s %dssss filename[%s]",__FILE__,__LINE__,filename);
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
		/**显示列标题**/
		/*fprintf( fp,"~@账 号|@手机号|@签约日期|@上次收费日期|@系统日期|@收费标志|@签约状态|@未收费次数|\n");*/
		fprintf( fp,"~@账 号|@手机号|@签约日期|@上次收费日期|@系统日期|@收费标志|@签约状态|\n");
	  	}

		/***计算上次收费到当前系统日期之间的月数***/	
		vtcp_log("%s %d 账号: %s",__FILE__,__LINE__,sMob_acct_type.ac_no);
		vtcp_log("%s %d 系统日期[%ld]   上次收费日期[%ld]",__FILE__,__LINE__,sys_date,sMob_acct_type.lst_fee_date);
		/****
		count=pub_base_CaltureMouths(sMob_acct_type.lst_fee_date,sys_date);
		*****/
		count=pub_base_CalMouths(sMob_acct_type.lst_fee_date,sys_date);
		if(count>3)
		count=3;
		vtcp_log("%s %d 月数[%d]",__FILE__,__LINE__,count);
		pub_base_dic_show_str(sflag,"mob_sts",sMob_acct_type.mob_sts);
                strcpy(qflag,"收费");		

		
		
		/*if(count)   如果只查欠费的放开 add by lwb*/ 
		{

			sprintf(tmp_date1,"%ld",sMob_acct_type.tx_date);
			sprintf(tmp_date2,"%ld",sMob_acct_type.lst_fee_date);
		 /* fprintf(fp,"%s|%s|%s|%s|%ld|%s|%s|%d|\n",sMob_acct_type.ac_no,sMob_acct_type.mobile,tmp_date1,tmp_date2,sys_date,sflag,qflag,count);*/
		fprintf(fp,"%s|%s|%s|%s|%ld|%s|%s|%d|\n",sMob_acct_type.ac_no,sMob_acct_type.mobile,tmp_date1,tmp_date2,sys_date,qflag,sflag);

			
			ttlnum++;
		}	 
	}
		if( !ttlnum )
		{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
		}
		else
		{
		fprintf(fp,"共计:|%d|\n",ttlnum);
		ret=Mob_acct_type_Clo_Sel();
		ERR_DEAL
		fclose(fp);
	
		set_zd_data( DC_FILE_SND,"1" );
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


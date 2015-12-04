/***** 以下是收费清单查询(sp5348)的主程序**********/
/*****************************************************
* 文 件 名:  sp5888.c
* 功能描述： 收费清单查询
* 作    者:  wangyh
* 完成日期： 2006年8月30日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_c552r_c.h"
#include "lv_c554r_c.h"
int sp5888()
{
	int ret=0,i;
	char in_date[9];
	char wt_date[9];
	char brno[8];
	char dtlid[4];
	char cTdtlid[4];
	struct  lv_c552r_c       f_lv_c552r;
	struct  lv_c554r_c       f_lv_c554r;
	char cPrtname[5];
	char cPrtname1[5];
	char cFilename[100];
	FILE *fp;      /*文件名*/
/*	memset(tx_date,0,sizeof(tx_date)); */
	memset(in_date,0,sizeof(in_date));
 	memset(wt_date,0,sizeof(wt_date)); 
	memset(brno,0,sizeof(brno)); 
	memset(dtlid,0,sizeof(dtlid)); 
	memset(cTdtlid,0,sizeof(cTdtlid)); 
	memset(&f_lv_c552r,0x00,sizeof(struct lv_c552r_c));
	memset(&f_lv_c554r,0x00,sizeof(struct lv_c554r_c));
	memset(cPrtname,0,sizeof(cPrtname));
	memset(cPrtname1,0,sizeof(cPrtname1));
	vtcp_log("sp5888 开始！[%s][%d]",__FILE__,__LINE__);
	pub_base_sysinit();
	get_zd_data("0240",cTdtlid);
	memcpy(dtlid,cTdtlid,sizeof(dtlid)-1);	
	vtcp_log("[%s][%d]dtlid=[%.3s]",__FILE__,__LINE__,dtlid);		
	vtcp_log("[%s][%d]cTdtlid=[%.3s]",__FILE__,__LINE__,cTdtlid);		
	get_zd_data("0440",in_date);
	get_zd_data("0450",wt_date);
	get_zd_data("0880",brno);

	vtcp_log("[%s][%d]in_date=[%.8s]",__FILE__,__LINE__,in_date);
	vtcp_log("[%s][%d]wt_date=[%.8s]",__FILE__,__LINE__,wt_date);
	vtcp_log("[%s][%d]brno=[%.7s]",__FILE__,__LINE__,brno);
	vtcp_log("[%s][%d]cTdtlid=[%.3s]",__FILE__,__LINE__,cTdtlid);		

	vtcp_log("sp5888 开始！[%s][%d]",__FILE__,__LINE__);

	ret=Lv_c552r_Sel(g_pub_tx.reply,&f_lv_c552r,"in_date='%s'and wt_date='%s' and brno='%s' and dtlid='%s'",in_date,wt_date,brno,cTdtlid);
	vtcp_log("sp5888 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此收费清单");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询收费清单基本信息出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}		
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);

	strcpy(f_lv_c554r.brno, f_lv_c552r.brno);
	f_lv_c554r.in_date = f_lv_c552r.in_date;
	f_lv_c554r.wt_date = f_lv_c552r.wt_date;	
	f_lv_c554r.dtlid = f_lv_c552r.dtlid;
	vtcp_log("查询成功[%d] [%s][%d]",f_lv_c554r.in_date,__FILE__,__LINE__);
	vtcp_log("查询成功[%d] [%s][%d]",f_lv_c554r.wt_date,__FILE__,__LINE__);
	vtcp_log("查询成功[%d] [%s][%d]",f_lv_c554r.dtlid,__FILE__,__LINE__);
	ret=Lv_c554r_Dec_Sel(g_pub_tx.reply,"in_date='%s'and wt_date='%s' and brno='%s' and dtlid='%s'",in_date,wt_date,brno,cTdtlid);
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
	
	ERR_DEAL
	i=0;
	while(1)
	{
	  	memset(f_lv_c552r.sorr,0,sizeof(f_lv_c552r.sorr));
	  	memset(f_lv_c554r.aorm,0,sizeof(f_lv_c554r.aorm));
	  	ret=Lv_c554r_Fet_Sel (&f_lv_c554r,g_pub_tx.reply);
	  	if( ret==100 ) 
        	{
        		break;
        	}
		ERR_DEAL
	 	if( !i )
	  	{
			pub_base_AllDwnFilName(cFilename);
			fp = fopen(cFilename,"w" );
			if( fp==NULL )
			  {
				   sprintf(acErrMsg," open file [%s] error ",cFilename);
				   WRITEMSG
				   strcpy( g_pub_tx.reply,"S047" );
				   goto ErrExit;
			  }
			fprintf( fp,"~CMT编号|计费金额|计费笔数|基础费率|计费单价|收发标志|通知类型|\n" );
		  }
		if(f_lv_c552r.sorr[0]='0')
		  {
			memcpy(cPrtname,"发送", sizeof(cPrtname)-1);
	     }
	        else
	        {
	                memcpy(cPrtname,"接收", sizeof(cPrtname)-1);
	        }
					if(f_lv_c554r.aorm[0]='0')
					{
						memcpy(cPrtname1,"自动", sizeof(cPrtname1)-1);
	        }
	        else
	        {
	                memcpy(cPrtname1,"手工", sizeof(cPrtname1)-1);
	        } 
	        vtcp_log("查询成功！[%s]  [%s][%d]",f_lv_c554r.cmtno,__FILE__,__LINE__);

		fprintf( fp,"%s|%s|%s|%s|%s|%s|%s|\n",f_lv_c554r.cmtno,f_lv_c554r.txamt,f_lv_c554r.rebnum,f_lv_c554r.brate,f_lv_c554r.price,cPrtname,f_lv_c554r.iftype);
		i++;
        }
	ret=Hv_zf_Clo_Sel();
	ERR_DEAL
	if( !i )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}



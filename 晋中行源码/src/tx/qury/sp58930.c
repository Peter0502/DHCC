/***** 以下是支付业务收费清单明细查询(sp5348)的主程序**********/
/*****************************************************
* 文 件 名:  sp5893.c
* 功能描述： 支付业务收费清单明细查询
* 作    者:  wangyh
* 完成日期： 2006年9月1日 
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
#include "hv_zf_c.h"
#include "hv_chargemx_c.h"
int sp5893()
{
	int ret=0,i;
	char in_date[9];
	char tx_date[9];
	char wt_date[9];
	char br_no[8];
	char dtlid[4];
	char orderno[4];
	char cTdtlid[4];
	char cPrtname[5];
	char cPrtname1[5];
	char cFilename[100];
	struct  hv_zf_c       f_hv_zf;
	struct  hv_chargemx_c       f_hv_chargemx;
	HV_FD123_AREA fd123;
	FILE *fp;      /*文件名*/
/*	memset(tx_date,0,sizeof(tx_date)); */
	memset(in_date,0,sizeof(in_date));
	memset(in_date,0,sizeof(tx_date));
 	memset(wt_date,0,sizeof(wt_date)); 
	memset(br_no,0,sizeof(br_no)); 
	memset(dtlid,0,sizeof(dtlid)); 
	memset(orderno,0,sizeof(orderno));
	memset(cTdtlid,0,sizeof(cTdtlid)); 
	memset(&f_hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(&f_hv_chargemx,0x00,sizeof(struct hv_chargemx_c));
	memset(&fd123, ' ',sizeof(fd123));
	memset(cPrtname,0,sizeof(cPrtname));
	memset(cPrtname1,0,sizeof(cPrtname1));
	vtcp_log("sp5893 开始！[%s][%d]",__FILE__,__LINE__);
	pub_base_sysinit();
	get_zd_data("0240",cTdtlid);
	memcpy(orderno,cTdtlid,sizeof(orderno)-1);	
	vtcp_log("[%s][%d]orderno=[%.3s]",__FILE__,__LINE__,orderno);		
	vtcp_log("[%s][%d]cTdtlid=[%.3s]",__FILE__,__LINE__,cTdtlid);		
	get_zd_data("0440",tx_date);
	get_zd_data("0450",wt_date);
	get_zd_data("0880",br_no);
	
	vtcp_log("[%s][%d]tx_date=[%.8s]",__FILE__,__LINE__,tx_date);
	vtcp_log("[%s][%d]wt_date=[%.8s]",__FILE__,__LINE__,wt_date);
	vtcp_log("[%s][%d]br_no=[%.7s]",__FILE__,__LINE__,br_no);
	vtcp_log("[%s][%d]cTdtlid=[%.3s]",__FILE__,__LINE__,cTdtlid);		

	vtcp_log("sp5893 开始！[%s][%d]",__FILE__,__LINE__);

	ret=Hv_zf_Sel(g_pub_tx.reply,&f_hv_zf,"tx_date='%s'and wt_date='%s' and br_no='%s'",tx_date,wt_date,br_no);
	vtcp_log("sp5893 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此支付业务收费清单明细");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询支付业务收费清单明细基本信息出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}		
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);

	strcpy(f_hv_chargemx.br_no, f_hv_zf.br_no);
	f_hv_chargemx.in_date = f_hv_zf.tx_date;
	vtcp_log("查询成功[%d] [%s][%d]",f_hv_zf.tx_date,__FILE__,__LINE__);
	vtcp_log("查询成功[%d] [%s][%d]",f_hv_chargemx.in_date,__FILE__,__LINE__);
	vtcp_log("查询成功[%d] [%s][%d]",f_hv_chargemx.dtlid,__FILE__,__LINE__);
	ret=Hv_chargemx_Dec_Sel(g_pub_tx.reply,"in_date=%d",f_hv_chargemx.in_date);
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
	
	ERR_DEAL
	i=0;
	while(1)
	{ 
	  	memset(f_hv_chargemx.chgtype,0,sizeof(f_hv_chargemx.chgtype));
	  	ret=Hv_chargemx_Fet_Sel (&f_hv_chargemx,g_pub_tx.reply);
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
			fprintf( fp,"~CMT编号|计费金额|计费笔数|基础费率|计费单价|收发标志|打印标志|\n" );
		  }
		  if(f_hv_chargemx.chgtype[0]='0')
		      {
			        memcpy(cPrtname,"发送", sizeof(cPrtname)-1);
	        }
	    else
	        {
	            memcpy(cPrtname,"接收", sizeof(cPrtname)-1);
	        }
		/*			if(f_hv_chargemx.aorm[0]='0')
					{
						memcpy(cPrtname1,"自动", sizeof(cPrtname1)-1);
	        }
	        else
	        {
	                memcpy(cPrtname1,"手工", sizeof(cPrtname1)-1);
	        } 
	        vtcp_log("查询成功！[%s]  [%s][%d]",f_hv_chargemx.cmtno,__FILE__,__LINE__);
    */
		fprintf( fp,"%s|%s|%s|%s|%s|%s|%s|\n",f_hv_chargemx.cmtno,f_hv_chargemx.txamt,f_hv_chargemx.mnum,f_hv_chargemx.brate,f_hv_chargemx.price,cPrtname,f_hv_chargemx.prtflag);
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



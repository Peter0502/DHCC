/***** 以下是支付交易明细查询(sp5351)的主程序**********/
/*****************************************************
* 文 件 名:  sp5891.c
* 功能描述： 支付交易明细查询
* 作    者:  wangyh
* 完成日期： 2006年9月15日 周5
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
#include "lv_define.h"
#include "hv_define.h"
#include "hv_zf_c.h"
int sp5891()
{
	int ret=0,i;
	char flag[1];
	char tx_date[9];
	char lw_ind[2];
	char or_br_no[13];
	char orderno[9];
	char cOrderno[9];
	char cTcmtno[4];
	char cTdtlid[9];
	struct  hv_zf_c       f_hv_zf;
	char cPrtname[5];
	char cPrtname1[5];
	char cFilename[100];
	FILE *fp;      /*文件名*/
/*	memset(tx_date,0,sizeof(tx_date)); */ 
  memset(flag,0,sizeof(flag));
	memset(tx_date,0,sizeof(tx_date));
 	memset(lw_ind,0,sizeof(lw_ind)); 
	memset(or_br_no,0,sizeof(or_br_no));
	memset(cTcmtno,0,sizeof(cTcmtno)); 
	memset(orderno,0,sizeof(orderno));
	memset(cTdtlid,0,sizeof(cTdtlid));  
	memset(&f_hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(cPrtname,0,sizeof(cPrtname));
	memset(cPrtname1,0,sizeof(cPrtname1));
	memcpy(cOrderno,"00000000",sizeof(cOrderno)-1);
	vtcp_log("sp5891 开始！[%s][%d]",__FILE__,__LINE__);
	get_zd_data("0230",cTcmtno);		
	vtcp_log("[%s][%d]cTcmtno=[%s]",__FILE__,__LINE__,cTcmtno);	
	pub_base_sysinit();
	get_zd_data("0230",cTcmtno);		
	vtcp_log("[%s][%d]cTcmtno=[%s]",__FILE__,__LINE__,cTcmtno);	
	memcpy(cTdtlid,cTcmtno,sizeof(cTdtlid)-1);
	get_zd_data("0440",tx_date);
	get_zd_data("0600",or_br_no);
	get_zd_data("0780",orderno);
	get_zd_data("0990",lw_ind);
	vtcp_log("[%s][%d]lw_ind=[%s]",__FILE__,__LINE__,lw_ind);		
	vtcp_log("[%s][%d]tx_date=[%s]",__FILE__,__LINE__,tx_date);
	vtcp_log("[%s][%d]or_br_no=[%s]",__FILE__,__LINE__,or_br_no);
	vtcp_log("[%s][%d]orderno=[%s]",__FILE__,__LINE__,orderno);		
  vtcp_log("[%s][%d]cTdtlid=[%s]",__FILE__,__LINE__,cTdtlid);
	vtcp_log("sp5891 开始！[%s][%d]",__FILE__,__LINE__);
	zip_space(orderno);
	memcpy(cOrderno+8-strlen(orderno),orderno,strlen(orderno));
	vtcp_log("[%s][%d]cOrderno====[%s]",__FILE__,__LINE__,cOrderno);
	ret=Hv_zf_Sel(g_pub_tx.reply,&f_hv_zf,"cmtno='%s' and tx_date=%d and lw_ind='%s' and or_br_no='%s' and orderno='%s'",cTdtlid,apatoi(tx_date,8),lw_ind,or_br_no,cOrderno);
	   vtcp_log("[%s][%d]cTdtlid=[%s]",__FILE__,__LINE__,cTdtlid);	
	   vtcp_log("sp5891 开始！[%s][%d]",__FILE__,__LINE__);
	   vtcp_log("[%s][%d]",__FILE__,__LINE__);
	   if ( ret==100 )
	   {
		    sprintf(acErrMsg,"无此交易");
		     WRITEMSG
		    goto	ErrExit;
	   }
	   else if ( ret )
	   {
		   sprintf(acErrMsg,"查询交易信息出错[%d]",ret);
		   WRITEMSG
   		 goto ErrExit;
	    }		
	    vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
	    vtcp_log("查询成功[%d] [%s][%d]",f_hv_zf.tx_date,__FILE__,__LINE__);

    	ret=Hv_zf_Dec_Sel(g_pub_tx.reply,"cmtno='%s' and tx_date=%d and lw_ind='%s' and or_br_no='%s' and orderno='%s'",cTdtlid,apatoi(tx_date,8),lw_ind,or_br_no,cOrderno);
	    vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
	    vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
	
    	ERR_DEAL
    	i=0;
    	while(1)
       	
    	{
	  /*	memset(f_hv_zf.sorr,0,sizeof(f_hv_zf.sorr));
	  	  memset(f_hv_zf.iorw,0,sizeof(f_hv_zf.iorw));  */
	  	  memset(&f_hv_zf,0,sizeof(struct  hv_zf_c)); 
	  	  ret=Hv_zf_Fet_Sel (&f_hv_zf,g_pub_tx.reply);
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
			      fprintf( fp,"~交易行行号|委托日期|交易序号|申请行行号|交易状态|交易分类|\n" );
		        }
	      vtcp_log("查询成功！[%s]  [%s][%d]",f_hv_zf.orderno,__FILE__,__LINE__);

	      if ( f_hv_zf.lw_ind[0]=='1')
  	        {
  	        	 vtcp_log("查询成功0[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.br_no);
  	        	 vtcp_log("查询成功1[%s] [%d][%d]",__FILE__,__LINE__,f_hv_zf.wt_date);
  	        	 vtcp_log("查询成功3[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.orderno);
  	        	 vtcp_log("查询成功5[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.or_br_no);
  	        	 vtcp_log("查询成功6[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.hv_sts);
  	        	 vtcp_log("查询成功7[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.cmtno);
		           fprintf( fp,"%s|%d|%s|%s|%s|%s|\n", \
		           f_hv_zf.br_no, \
		           f_hv_zf.wt_date, \
		           f_hv_zf.orderno, \
		           f_hv_zf.or_br_no, \
		           f_hv_zf.hv_sts, \
		           f_hv_zf.cmtno);
            }
        else{  
  	        	 vtcp_log("查询成功0[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.br_no);
  	        	 vtcp_log("查询成功1[%s] [%d][%d]",__FILE__,__LINE__,f_hv_zf.wt_date);
  	        	 vtcp_log("查询成功3[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.orderno);
  	        	 vtcp_log("查询成功5[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.or_br_no);
  	        	 vtcp_log("查询成功6[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.hv_sts);
  	        	 vtcp_log("查询成功7[%s] [%d][%s]",__FILE__,__LINE__,f_hv_zf.cmtno);
		           fprintf( fp,"%s|%d|%s|%s|%s|%s|\n", \
		           f_hv_zf.br_no, \
		           f_hv_zf.wt_date, \
		           f_hv_zf.orderno, \
		           f_hv_zf.or_br_no, \
		           f_hv_zf.hv_sts, \
		           f_hv_zf.cmtno);
            }
 
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



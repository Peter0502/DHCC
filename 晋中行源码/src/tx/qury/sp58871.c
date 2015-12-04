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
struct fd1234
{
	  char receco[4];
	  char qsBrno[12];
	  char txamt[15];	  
    HV_802_MAIN_ADD hv_802_main;
}fd124;


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
	char cBuff[2001];
	char cPrtname[5];
	char cPrtname1[5];
	char cFilename[100];
	struct  hv_zf_c       f_hv_zf;
	struct  hv_chargemx_c       f_hv_chargemx;
	
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
	memset(&fd124, ' ',sizeof(fd124));
	memset(cBuff, 0 , sizeof(cBuff));
	
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

	vtcp_log(" f_hv_zf.receco=[%s] f_hv_zf.cash_qs_no=[%s] f_hv_zf.tx_amt=[%f]",f_hv_zf.receco,f_hv_zf.cash_qs_no,f_hv_zf.tx_amt);
	
	memcpy(fd124.receco, f_hv_zf.receco, sizeof(fd124.receco));    
	memcpy(fd124.qsBrno, f_hv_zf.cash_qs_no, sizeof(fd124.qsBrno)); 
  dot_dbl2str(f_hv_zf.tx_amt,sizeof(fd124.txamt) ,2,fd124.txamt);
	
		
	vtcp_log("fd124.receco=[%s] fd124.qsBrnocash_qs_no=[%s] tx_amt=[%.8s]",fd124.receco,fd124.qsBrno,fd124.txamt);
	if(memcmp(f_hv_zf.cmtno,HV_HD,sizeof(f_hv_zf.cmtno)-1) && memcmp(f_hv_zf.cmtno,HV_GKZJHH,sizeof(f_hv_zf.cmtno)-1))
	{
		iHvGetAddit(tx_date,tx_date,f_hv_zf.addid,"000",&fd124.hv_802_main,sizeof(HV_802_MAIN_ADD)-1);
		vtcp_log("%s,%d 得到的附加域",__FILE__,__LINE__);  
	        /**发报中心代码**/
	       /* memcpy(fd124.receco              ,cBuff             ,sizeof(fd124.receco               )-1);*/
	        /**接受清算行号**/
	       /* memcpy(fd124.qsBrno              ,cBuff+4           ,sizeof(fd124.qsBrno               )-1);*/
	       /* set_zd_double("fd124.txamt"      ,f_hv_zf.tx_amt);  */ /* 交易金额 */
	       /* memcpy(fd124.sDate               ,cBuff+31          ,sizeof(fd124.sDate                )-1);
	         memcpy(fd124.eDate               ,cBuff+39          ,sizeof(fd124.eDate                )-1);*/
	        /**月累计业务量收费比率**/
	        /*memcpy(fd124.idrate              ,cBuff+47          ,sizeof(fd124.idrate               )-1);
	        memcpy(fd124.serate              ,cBuff+53          ,sizeof(fd124.serate               )-1);
	        memcpy(fd124.morate              ,cBuff+59          ,sizeof(fd124.morate               )-1);
	        memcpy(fd124.zftxamt             ,cBuff+65          ,sizeof(fd124.zftxamt              )-1);
	        memcpy(fd124.zfnum               ,cBuff+80          ,sizeof(fd124.zfnum                )-1);
	        memcpy(fd124.dbtxamt             ,cBuff+88          ,sizeof(fd124.dbtxamt              )-1);
	        memcpy(fd124.dbnum               ,cBuff+103         ,sizeof(fd124.dbnum                )-1);
	        memcpy(fd124.mestxamt            ,cBuff+111         ,sizeof(fd124.mestxamt             )-1);
	        memcpy(fd124.mesnum              ,cBuff+126         ,sizeof(fd124.mesnum               )-1);
	        	  
	        memcpy(fd124.sDate, hv_802_main.beg_date, sizeof(fd124.sDate)-1);    
	        memcpy(fd124.eDate, hv_802_main.end_date, sizeof(fd124.eDate)-1);          
	        memcpy(fd124.idrate, hv_802_main.zjcy_rate, sizeof(fd124.idrate)-1);      
	        memcpy(fd124.serate, hv_802_main.hb_rate , sizeof(fd124.serate)-1);       
	        memcpy(fd124.morate, hv_802_main.ywl_rate, sizeof(fd124.morate)-1);        
	        memcpy(fd124.zftxamt, hv_802_main.zf_fee  , sizeof(fd124.zftxamt)-1);       
	        memcpy(fd124.zfnum, hv_802_main.zf_cnt  , sizeof(fd124.zfnum)-1);           
	        memcpy(fd124.dbtxamt, hv_802_main.db_fee  , sizeof(fd124.dbtxamt)-1);       
	        memcpy(fd124.dbnum, hv_802_main.db_cnt  , sizeof(fd124.dbnum)-1);       
	        memcpy(fd124.mestxamt, hv_802_main.xx_fee  , sizeof(fd124.mestxamt)-1); 
	        memcpy(fd124.mesnum, hv_802_main.xx_cnt  , sizeof(fd124.mesnum)-1);     
          */
  }

	pub_base_full_space((char*)&fd124,sizeof(fd124));
	vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  

	set_zd_data("1230",(char*)&fd124);
	vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  

	vtcp_log("%s,%d 得到的附加域=[%s]",__FILE__,__LINE__,(char*)&fd124);  

	strcpy(f_hv_chargemx.br_no, f_hv_zf.br_no);

	f_hv_chargemx.in_date = f_hv_zf.tx_date;
	vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  

	vtcp_log("查询成功[%s] [%s][%d]",f_hv_chargemx.br_no,__FILE__,__LINE__);
	vtcp_log("查询成功[%d] [%s][%d]",f_hv_chargemx.in_date,__FILE__,__LINE__);
	
	ret=Hv_chargemx_Dec_Sel(g_pub_tx.reply,"in_date=%d and br_no='%s'",f_hv_chargemx.in_date,f_hv_chargemx.br_no);
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
	i=0;
	while(1)
	{ 
	  	memset(f_hv_chargemx.chgtype,0,sizeof(f_hv_chargemx.chgtype));
	  	ret=Hv_chargemx_Fet_Sel (&f_hv_chargemx,g_pub_tx.reply);
	  	if( ret==100 ) 
        	{
        	  vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  
        		break;
        	}
		ERR_DEAL
	 	if( !i )
	  	{	
      vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  
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
	  vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  

   }
        ret=Hv_chargemx_Clo_Sel();
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
		       vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  

	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
		       vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  

	WRITEMSG
		       vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  

	set_zd_data(DC_REPLY,g_pub_tx.reply);
		       vtcp_log("%s,%d 得到的附加域hahaha",__FILE__,__LINE__);  

	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}



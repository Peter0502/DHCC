/*************************************************
* 文 件 名:  spJ086.c
* 功能描述： 大额来帐交易查询
* 作    者:  ligl
* 完成日期： 2006年8月29日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

spJ086()
{
        int ret=0,i;
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;
	HV_FD123_AREA fd123;
	char cBr_no[6];/*机构号*/
	char cTx_date[9];/*交易日期*/
	char cPrtflag[2];/*打印标志*/
	char cTxstat[2];/*交易状态*/
	char cCmtno[4];/*汇票类型*/
	char cBreakpoint[9];/*断点*/
	char cFilename[100];/*文件名*/
	char cZf_type[2];
	int ttlnum=0;	    /**读取的总条数**/
	FILE *fp;
	char wherelist[1024];  /**查询条件**/
	char cPrtname[7];
	char cStsname[21];
	memset(cBr_no     ,0, sizeof(cBr_no ));
	memset(cTx_date   ,0, sizeof(cTx_date));	
	memset(cPrtflag   ,0, sizeof(cPrtflag));
	memset(cTxstat    ,0, sizeof(cTxstat ));
	memset(cCmtno     ,0, sizeof(cCmtno  ));
	memset(cBreakpoint,0, sizeof(cBreakpoint));
	memset(cFilename  ,0, sizeof(cFilename));
	memset(cPrtname   ,0, sizeof(cPrtname));
	memset(cStsname   ,0, sizeof(cStsname));
	memset(cZf_type,0,sizeof(cZf_type));
	memset(&fd123, ' ',sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(&hv_fzf,0x00,sizeof(struct hv_fzf_c));
	vtcp_log("spJ123 开始！[%s][%d]",__FILE__,__LINE__);
	/**初始变量**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0440",cTx_date);
	get_zd_data("0670",cPrtflag);
	get_zd_data("0690",cTxstat);
	get_zd_data("0230",cCmtno);
	get_zd_data("0720",cZf_type);
	vtcp_log("[%s][%d]cmtno==[%s]",__FILE__,__LINE__,cCmtno);
	/**组成查询条件**/
	if(cZf_type[0]=='0')
	{
	if(memcmp(cCmtno,"000",3)!=0)
	{
		vtcp_log("here！[%s][%d]",__FILE__,__LINE__);		
	 	if(cTxstat[0]=='W')/*全部交易状态*/
	    	{
			if(cPrtflag[0]=='2')/*全部*/
			{
				sprintf(wherelist,"lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date='%s' and cmtno!='802'",cCmtno,cBr_no,cTx_date);
			}
			else if(cPrtflag[0]=='0')/*未打印*/
			{
				sprintf(wherelist,"lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and cmtno!='802'",cCmtno,cBr_no,cTx_date);
			}
			else/*已打印*/
			{
				sprintf(wherelist,"lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date='%s' and hv_prt_ind='1' and cmtno!='802'",cCmtno,cBr_no,cTx_date);
		
			}		
		}
		else
		{
			if(cPrtflag[0]=='2')/*全部*/
			{
				sprintf(wherelist,"lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date=%s and hv_sts='%s' and cmtno!='802'",cCmtno,cBr_no,cTx_date,cTxstat);
			}
			else if(cPrtflag[0]=='0')/*未打印*/
			{
				sprintf(wherelist,"lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date=%s and hv_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and cmtno!='802'",cCmtno,cBr_no,cTx_date,cTxstat);
			}
			else/*已打印*/
			{
				sprintf(wherelist,"lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date=%s and hv_sts='%s' and hv_prt_ind='1' and cmtno!='802'",cCmtno,cBr_no,cTx_date,cTxstat);
			}
		}
	}
	else
	{
		if(cTxstat[0]=='W')/*全部交易状态*/
	    	{
			if(cPrtflag[0]=='2')/*全部*/
			{
				sprintf(wherelist,"lw_ind='2' and br_no='%s' and tx_date='%s'and cmtno!='802' ",cBr_no,cTx_date);
			}
			else if(cPrtflag[0]=='0')/*未打印*/
			{
				sprintf(wherelist,"lw_ind='2' and br_no='%s' and tx_date='%s' and (hv_prt_ind='0' or hv_prt_ind is null)and cmtno!='802' ",cBr_no,cTx_date);
			}
			else/*已打印*/
			{
				sprintf(wherelist,"lw_ind='2' and br_no='%s' and tx_date='%s' and hv_prt_ind='1' and cmtno!='802'",cBr_no,cTx_date);
		
			}		
		}
		else
		{
			if(cPrtflag[0]=='2')/*全部*/
			{
				sprintf(wherelist,"lw_ind='2' and br_no='%s' and tx_date=%s and hv_sts='%s' and cmtno!='802'",cBr_no,cTx_date,cTxstat);
			}
			else if(cPrtflag[0]=='0')/*未打印*/
			{
				sprintf(wherelist,"lw_ind='2' and br_no='%s' and tx_date=%s and hv_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and cmtno!='802'",cBr_no,cTx_date,cTxstat);
			}
			else/*已打印*/
			{
				sprintf(wherelist,"lw_ind='2' and br_no='%s' and tx_date=%s and hv_sts='%s' and hv_prt_ind='1' and cmtno!='802'",cBr_no,cTx_date,cTxstat);
			}
		}
	}
	/*开始查询*/
        ret = Hv_zf_Dec_Sel(g_pub_tx.reply, wherelist);
        ERR_DEAL
        ttlnum=0;
        while(1)
        {
        	memset(cPrtname   ,0, sizeof(cPrtname));
					memset(cStsname   ,0, sizeof(cStsname));
					memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
        	ret=Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
        	if( ret==100 ) 
        	{
        		break;
        	}
		ERR_DEAL
		if( !ttlnum )
		{
			pub_base_AllDwnFilName(cFilename);
			fp = fopen(cFilename,"w");
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",cFilename);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			fprintf( fp,"~交易行|交易日期|顺序号|交易金额|开户行(付)|开户行(收)|状   态|报文编号|打印标志|\n" );
		}
		vtcp_log("spJ123 开始！hv_zf.hv_prt_ind=[%s][%s][%d]",hv_zf.hv_prt_ind,__FILE__,__LINE__);
		if(hv_zf.hv_prt_ind[0]=='1')
		{
			MEMCPY_DEBUG(cPrtname,"已打印", sizeof(cPrtname)-1);
	  }
	  else
	  {
	    MEMCPY_DEBUG(cPrtname,"未打印", sizeof(cPrtname)-1);
	  }
	  
	  switch(hv_zf.hv_sts[0])
	  {
	    case '8':
	    					MEMCPY_DEBUG(cStsname,"来账拒绝",8);
                break;
	    case '9':
	    					MEMCPY_DEBUG(cStsname,"来账记账清算",12);
                break;
	    case 'B':
	    					MEMCPY_DEBUG(cStsname,"来账退回",8);
                break;
	    case 'E':
	    					MEMCPY_DEBUG(cStsname,"来账记账登记",12);
                break;

	    case 'F':
	    					MEMCPY_DEBUG(cStsname,"来账核地方押错",14);
                break;

	    case 'G':
	    					MEMCPY_DEBUG(cStsname,"来账核全国押错",14);
                break;

	    case 'H':
	    					MEMCPY_DEBUG(cStsname,"来账内容不符",12);
                break;

	    case 'I':
	    					MEMCPY_DEBUG(cStsname,"来账户名不符",12);
                break;

	    case 'K':
	    					MEMCPY_DEBUG(cStsname,"核对清算记帐",12);
                break;
      default :
      	       break;
	  }      
		fprintf( fp,"%s|%ld|%s|%.2f|%s|%s|%s|%s|%s|\n",hv_zf.br_no,hv_zf.tx_date,hv_zf.orderno,hv_zf.tx_amt,hv_zf.or_br_no,hv_zf.ac_br_no,cStsname,hv_zf.cmtno,cPrtname);
		ttlnum++;
        }
	ret=Hv_zf_Clo_Sel();
	ERR_DEAL
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}
}
else
{
	if(memcmp(cCmtno,"000",3)!=0)
	{
		vtcp_log("here！[%s][%d]",__FILE__,__LINE__);
		vtcp_log("[%s][%d]cmtno==[%s]",__FILE__,__LINE__,cCmtno);		
	 	if(cTxstat[0]=='W')/*全部交易状态*/
	    	{
			if(cPrtflag[0]=='2')/*全部*/
			{
				if(memcmp(cCmtno,"302",3)==0)
				{
					sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and (cmtno='302' or cmtno='301') and (req_date='%s' or res_date='%s')",cBr_no,cTx_date,HV_QS_BR_NO,cTx_date,cTx_date);
			  }
			  else if(memcmp(cCmtno,"301",3)==0)
			  {
					sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and pay_qs_no!='%s'and (cmtno='302' or cmtno='301') and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTx_date,cTx_date);
			  }
			  else
				{
					sprintf(wherelist,"otype='1' and lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date='%s'",cCmtno,cBr_no,cTx_date);
			  }
			}
			else if(cPrtflag[0]=='0')/*未打印*/
			{
				if(memcmp(cCmtno,"302",3)==0)
				{
					sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and (cmtno='302' or cmtno='301') and (hv_prt_ind='0' or hv_prt_ind is null) and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTx_date,cTx_date);
			  }
			  else if(memcmp(cCmtno,"301",3)==0)
			  {
					sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and (cmtno='302' or cmtno='301') and (hv_prt_ind='0' or hv_prt_ind is null) and (req_date='%s' or res_date='%s')  ",cBr_no,cTx_date,HV_QS_BR_NO,cTx_date,cTx_date);
			  }
				else
				{
					sprintf(wherelist,"otype='1' and lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date='%s' and (hv_prt_ind='0' or hv_prt_ind is null) ",cCmtno,cBr_no,cTx_date);
				}
			}
			else/*已打印*/
			{
				if(memcmp(cCmtno,"302",3)==0)
				{
					sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and (cmtno='302' or cmtno='301') and hv_prt_ind='1' and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTx_date,cTx_date);
			  }
			  else if(memcmp(cCmtno,"301",3)==0)
			  {
					sprintf(wherelist,"otype='1'  and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and (cmtno='302' or cmtno='301') and hv_prt_ind='1' and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTx_date,cTx_date);
			  }
				else
				{
					sprintf(wherelist,"otype='1' and lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date='%s' and hv_prt_ind='1'",cCmtno,cBr_no,cTx_date);
				}
			}		
		}
		else
		{
			if(cPrtflag[0]=='2')/*全部*/
			{
				if(memcmp(cCmtno,"302",3)==0)
				{
					sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and (cmtno='302' or cmtno='301') and hv_fzf_sts='%s' and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTxstat,cTx_date,cTx_date);
			  }
			  else if(memcmp(cCmtno,"301",3)==0)
			  {
					sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and (cmtno='302' or cmtno='301') and hv_fzf_sts='%s' and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTxstat,cTx_date,cTx_date);
			  }
				else
				{
					sprintf(wherelist,"otype='1' and lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date=%s and hv_fzf_sts='%s'",cCmtno,cBr_no,cTx_date,cTxstat);
				}
			}
			else if(cPrtflag[0]=='0')/*未打印*/
			{
				if(memcmp(cCmtno,"302",3)==0)
				{
					sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and hv_fzf_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (cmtno='302' or cmtno='301') and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTxstat,cTx_date,cTx_date);
			  }
			  else if(memcmp(cCmtno,"301",3)==0)
			  {
					sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and hv_fzf_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (cmtno='302' or cmtno='301') and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTxstat,cTx_date,cTx_date);
			  }
				else
				{
					sprintf(wherelist,"otype='1' and lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date=%s and hv_fzf_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) ",cCmtno,cBr_no,cTx_date,cTxstat);
				}
			}
			else/*已打印*/
			{
				if(memcmp(cCmtno,"302",3)==0)
				{
					sprintf(wherelist,"otype='1' and and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and hv_fzf_sts='%s' and hv_prt_ind='1'and (cmtno='302' or cmtno='301') and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTxstat,cTx_date,cTx_date);
			  }
			  else if(memcmp(cCmtno,"301",3)==0)
			  {
					sprintf(wherelist,"otype='1' and and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and hv_fzf_sts='%s' and hv_prt_ind='1'and (cmtno='302' or cmtno='301') and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,HV_QS_BR_NO,cTxstat,cTx_date,cTx_date);
			  }
				else
				{
					sprintf(wherelist,"otype='1' and lw_ind='2' and cmtno='%s'and br_no='%s' and tx_date=%s and hv_fzf_sts='%s' and hv_prt_ind='1' ",cCmtno,cBr_no,cTx_date,cTxstat);
				}			
			}
		}
	}
	else
	{
		if(cTxstat[0]=='W')/*全部交易状态*/
	    	{
			if(cPrtflag[0]=='2')/*全部*/
			{
				
				sprintf(wherelist,"otype='1'  and br_no='%s' and tx_date='%s' and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date);
			}
			else if(cPrtflag[0]=='0')/*未打印*/
			{
				sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (lw_ind='2' or cmtno='301' or cmtno='302')",cBr_no,cTx_date);
			}
			else/*已打印*/
			{
				sprintf(wherelist,"otype='1' and br_no='%s' and tx_date='%s' and hv_prt_ind='1' and (lw_ind='2' or cmtno='301' or cmtno='302')",cBr_no,cTx_date);
		
			}		
		}
		else
		{
			if(cPrtflag[0]=='2')/*全部*/
			{
				sprintf(wherelist,"otype='1' and br_no='%s' and tx_date=%s and hv_fzf_sts='%s' and (lw_ind='2' or cmtno='301' or cmtno='302')",cBr_no,cTx_date,cTxstat);
			}
			else if(cPrtflag[0]=='0')/*未打印*/
			{
				sprintf(wherelist,"otype='1' and br_no='%s' and tx_date=%s and hv_fzf_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (lw_ind='2' or cmtno='301' or cmtno='302')",cBr_no,cTx_date,cTxstat);
			}
			else/*已打印*/
			{
				sprintf(wherelist,"otype='1' and br_no='%s' and tx_date=%s and hv_fzf_sts='%s' and hv_prt_ind='1' and (lw_ind='2' or cmtno='301' or cmtno='302')",cBr_no,cTx_date,cTxstat);
			}
		}
	}
	/*开始查询*/
        ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, wherelist);
        ERR_DEAL
        ttlnum=0;
        while(1)
        {
        	memset(cPrtname   ,0, sizeof(cPrtname));
					memset(&hv_fzf,0x00,sizeof(struct hv_fzf_c));
        	ret=Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
        	if( ret==100 ) 
        	{
        		break;
        	}
		ERR_DEAL
		if( !ttlnum )
		{
			pub_base_AllDwnFilName(cFilename);
			fp = fopen(cFilename,"w");
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",cFilename);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			fprintf( fp,"~交易行|交易日期|顺序号|交易金额|开户行(付)|开户行(收)|状   态|报文编号|打印标志|\n" );
		}
		vtcp_log("spJ123 开始！hv_fzf.hv_prt_ind=[%s][%s][%d]",hv_fzf.hv_prt_ind,__FILE__,__LINE__);
		if(hv_fzf.hv_prt_ind[0]=='1')
		{
					MEMCPY_DEBUG(cPrtname,"已打印", sizeof(cPrtname)-1);
	  }
	  else
	  {
	        MEMCPY_DEBUG(cPrtname,"未打印", sizeof(cPrtname)-1);
	  }
	   		/*pub_base_strpack(hv_fzf.pay_sq_no);
	      if(memcmp(hv_fzf.pay_sq_no,HV_QS_BR_NO,12)==0 && memcmp(hv_fzf.cmtno,"302")==0)*/
				if(memcmp(hv_fzf.cmtno,"301",3)==0 && strlen(hv_fzf.res_orderno)==0 && memcmp(hv_fzf.pay_qs_no,HV_QS_BR_NO,12)==0)
				{
					continue;
				}
				else
				{
				 fprintf( fp,"%s|%ld|%s|%.2f|%s|%s|%s|%s|%s|\n",hv_fzf.br_no,hv_fzf.tx_date,hv_fzf.orderno,hv_fzf.o_tx_amt,hv_fzf.or_br_no,hv_fzf.ac_br_no,hv_fzf.hv_fzf_sts,hv_fzf.cmtno,cPrtname);
				 ttlnum++;
			  }
        }
	ret=Hv_fzf_Clo_Sel();
	ERR_DEAL
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}
}
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
}


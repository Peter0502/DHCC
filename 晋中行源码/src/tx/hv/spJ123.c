/*************************************************
* 文 件 名:  spJ123.c
* 功能描述： 来帐交易查询
* 作    者:  jane
* 完成日期： 2004年6月25日
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

spJ123()
{
        int ret=0,i;
	struct hv_zf_c  hv_zf;
	HV_FD123_AREA fd123;
	char cBr_no[6];/*机构号*/
	char cTx_date[9];/*交易日期*/
	char cPrtflag[2];/*打印标志*/
	char cTxstat[2];/*交易状态*/
	char cCmtno[4];/*汇票类型*/
	char cBreakpoint[9];/*断点*/
	char cFilename[100];/*文件名*/
	int ttlnum=0;	    /**读取的总条数**/
	FILE *fp;
	char wherelist[1024];  /**查询条件**/
	memset(cBr_no     ,0, sizeof(cBr_no ));
	memset(cTx_date   ,0, sizeof(cTx_date));	
	memset(cPrtflag   ,0, sizeof(cPrtflag));
	memset(cTxstat    ,0, sizeof(cTxstat ));
	memset(cCmtno     ,0, sizeof(cCmtno  ));
	memset(cBreakpoint,0, sizeof(cBreakpoint));
	memset(cFilename, 0 , sizeof(cFilename));
	
	memset(&fd123, ' ',sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	
	vtcp_log("spJ123 开始！[%s][%d]",__FILE__,__LINE__);
	/**初始变量**/
	pub_base_sysinit();
	/*get_zd_data("0030",cBr_no);
	get_zd_data("0440",cTx_date);
	get_zd_data("0670",cPrtflag);
	get_zd_data("0690",cTxstat);
	get_zd_data("0230",cCmtno);*/
	MEMCPY_DEBUG(cBr_no,"51801",sizeof(cBr_no)-1);
	MEMCPY_DEBUG(cTx_date,"20080621",sizeof(cTx_date)-1);
	MEMCPY_DEBUG(cPrtflag,"2",sizeof(cPrtflag)-1);
	MEMCPY_DEBUG(cTxstat,"W",sizeof(cTxstat)-1);
	MEMCPY_DEBUG(cCmtno,"100",sizeof(cCmtno)-1);
	/**组成查询条件**/
	if(cTxstat[0]=='W')/*全部交易状态*/
	{
		if(cPrtflag[0]=='2')/*全部*/
		{
			sprintf(wherelist,"cmtno='%s'and br_no='%s' and tx_date='%s'",cCmtno,cBr_no,cTx_date);
		}
		else/*未打印或者打印*/
		{
			sprintf(wherelist,"cmtno='%s'and br_no='%s' and tx_date='%s' and hv_prt_ind='%s'",cCmtno,cBr_no,cTx_date,cPrtflag);
		}		
	}
	else
	{
		if(cPrtflag[0]=='2')/*全部*/
		{
			sprintf(wherelist,"cmtno='%s'and br_no='%s' and tx_date=%s and hv_sts='%s'",cCmtno,cBr_no,cTx_date,cTxstat);
		}
		else/*未打印或者打印*/
		{
			sprintf(wherelist,"cmtno='%s'and br_no='%s' and tx_date=%s and hv_sts='%s' and hv_prt_ind='%s'",cCmtno,cBr_no,cTx_date,cTxstat,cPrtflag);
		}
	
	}
	/*开始查询*/
        ret = Hv_zf_Dec_Sel(g_pub_tx.reply, wherelist);
        ERR_DEAL
        ttlnum=0;
        while(1)
        {
        	ret=Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
        	if( ret==100 ) 
        	{
        		break;
        	}
		ERR_DEAL
		if( !ttlnum )
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
			/*fprintf( fp,"~交易行|交易日期|顺序号|交易金额|开户行(付)|开户行(收)|状   态|报文编号|打印标志|\n" );*/
		}
		fprintf( fp,"%s|%s|%s|%f|%s|%s|%s|%s|%s|\n",hv_zf.br_no,hv_zf.tx_date,hv_zf.orderno,hv_zf.tx_amt,hv_zf.or_br_no,hv_zf.ac_br_no,hv_zf.hv_sts,hv_zf.cmtno,hv_zf.hv_prt_ind);
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


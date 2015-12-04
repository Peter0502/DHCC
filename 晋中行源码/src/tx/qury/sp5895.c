/***** 以下是收费清单查询(sp5348)的主程序**********/
/*****************************************************
* 文 件 名:  sp5893.c
* 功能描述： 大额收费清单查询
* 作    者:  dadary
* 完成日期： 2006年11月2日 
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
#include "hv_chargemx_c.h"
#include "lv_c554r_c.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "hv_zf_c.h"
#include "hv_zf_c.h"
#include "hv_fzf_c.h"
#include "hv_chargemx_c.h"

#define  PRICE_303      5
#define  PRICE_301      5
#define  PRICE_PKG	5

/***对于大小额的支付交易和非支付交易的收费进行分开查询,自由格式可能收费比支付的低****/


struct{
	char sendco[4];	/**中心代码**/
	char qs_brno[12];/**清算行号**/
	char beg_date[8];/*开始日期**/
	char end_date[8];/*结束日期**/
	char txamt[15];	/*总金额**/
	char num[8];	/*总笔数**/
	char rate[6];	/*月累计业务量收费比率*/
}tos88;
char * iNum2TxType(char *ch,char *cTmpList);
int dGet_CMTNO_PRICE( char *cmtno);
int sp5893()
{
	vtcp_log("开始小额收费清单查询[%s][%d]",__FILE__,__LINE__);

	struct lv_c554r_c lv_c554;
	struct hv_chargemx_c hv_charge;
	LV_FD123_AREA     fd123;
	struct hv_zf_c hv_zf;
	struct hv_fzf_c 	hv_fzf;
	
	
	char  otype[2];			/***大额还是小额的标识***/
	
	int   QUERY_DATE;		/***查询日期***/
	
	
	
	int    head_flag = 0 ;/****是否写了文件头的标识   1 已经写了文件头   0  没有写******/
	
	int    NUM_303	= 0;	/**时间段中303的笔数**/
	int    NUM_301  = 0;    /*时间段中301的笔数**/
	int    NUM_PKG  = 0;    /*小额的支付笔数**/
	int    NUM_HV   = 0;	/*大额的支付包的数目**/
	
	double AVERATE_PRICE = 0.0;/**用总收费-303-301的收费除以总的支付的笔数,得出支付交易的单笔平均费用**/
	double totamt        = 0.0;/**某机构该时间段内的收费总量**/
	char  cTotamt[15];		/***某机构收费的总和***/
	
	
	char   br_no[6];	/*机构号*/
	char   beg_mon[7];	/*开始月份*/
	char   end_mon[7];	/*结束月份*/
	FILE 	*fp;            /*文件句柄*/
	char	cFilename[50];	/*文件名称**/	 	
	char 	tx_type[2];	/*查询类型,1-552   2-554****/
	int	iRecordNum = 0; /*记录的条数*/
	char 	c552_iftype[9]; /*552报文的通知类型**/
	char    c552_pkgno [9]; /*552明细报文编号**/
	int 	ret = 0;
	char    cWherelist[500];
	char 	cTmplist [100];


	memset(&tos88,	0x00,	sizeof(tos88));
	memset(&hv_charge,0x00,sizeof(hv_charge));
	memset(&lv_c554,0x00,sizeof(lv_c554));
	memset(&hv_charge,0x00,sizeof(hv_charge));
	
	memset(&hv_zf,0x00,sizeof(hv_zf));
	memset(&hv_fzf,	0x00,	sizeof(hv_fzf));
	memset(&hv_zf,	0x00,	sizeof(hv_zf));
	
	
	memset(&fd123,	0x00,	sizeof(fd123));
	memset(br_no,	0x00,sizeof(br_no));
	memset(beg_mon,	0x00,sizeof(beg_mon));
	memset(end_mon,	0x00,sizeof(end_mon));
	memset(tx_type,	0x00,sizeof(tx_type));
	memset(c552_pkgno,0x00,sizeof(c552_pkgno));
	memset(c552_iftype,0x00,sizeof(c552_iftype));
	memset(cFilename,0x00,sizeof(cFilename));
	memset(cWherelist,0x00,sizeof(cWherelist));
	memset(cTmplist,	0x00,	sizeof(cTmplist));
	memset(cTotamt,0x00,sizeof(cTotamt));

	
	
	pub_base_sysinit();

	get_zd_data("0440",beg_mon);
	get_zd_data("0910",br_no);
	get_zd_data("0680",otype);
	
	
	QUERY_DATE = apatoi(beg_mon,sizeof(beg_mon));
	QUERY_DATE = QUERY_DATE*100;

	
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && memcmp(br_no,g_pub_tx.tx_br_no,5))
	{
		vtcp_log("[%s][%d]非清算中心不能查询他行交易\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	sprintf(cWherelist,"sdate>=%d and edate<=%d  and ",QUERY_DATE+1,QUERY_DATE+31);
	
	
	
	strcat (cWherelist," 1=1 order by paymon ");
	

		
	ret=Hv_chargemx_Dec_Sel(g_pub_tx.reply,cWherelist);
	if(ret)
	{
		vtcp_log("[%s][%d]数据库操作失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	while(1)
	{
		ret = Hv_chargemx_Fet_Sel(&hv_charge,g_pub_tx.reply);
		if(ret &&ret!=100)
		{
			vtcp_log("[%s][%d]数据库操作失败!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}	
		else if(ret==100 &&iRecordNum==0)
		{
			vtcp_log("[%s][%d]找不到对应的记录!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D014");
			goto ErrExit;
		}
		
		if(ret==100)	break;
		
		/***开始分多次从hv_charge中 取记录***/
		/***获取到303的笔数,和301的笔数***/
		NUM_303 = sql_count("hv_fzf"," wt_date<=%d and wt_date>=%d and cmtno='303' and hv_sts ='1' and lw_ind='1'  and otype='1' ",hv_charge.sdate,hv_charge.edate);
		NUM_301 = 0 ; /*sql_count("hv_fzf","wt_date<=%d and wt_date>=%d and cmtno='301' and hv_sts ='1' and lw_ind='1' ",hv_charge.sdate,hv_charge.edate);*/
		NUM_PKG = 0 ;/*sql_count("hv_zf","wt_date<=%d and wt_date>=%d and lv_sts not in('0','1','2','3') and lw_ind='1' ",hv_charge.sdate,hv_charge.edate);*/
		NUM_HV =  sql_count("hv_zf",     "wt_date<=%d and wt_date>=%d and hv_sts not in('C','1') and lw_ind='1' ",hv_charge.sdate,hv_charge.edate);
		
		if( (NUM_PKG+NUM_HV)!=0)
			AVERATE_PRICE = ( apatoi(hv_charge.txamt)-NUM_303*PRICE_303-NUM_301*PRICE_301 ) /(NUM_PKG+NUM_HV);
		else 
			AVERATE_PRICE = 0.0;
			
		/**1.开始处理pkgno报文****/
		
		ret = Hv_zf_Dec_Sel(g_pub_tx.reply," br_no = '%s' and wt_date>=%d and wt_date<=%d and hv_sts not in('C','1') and lw_ind='1'  order by cmtno",br_no,hv_charge.sdate,hv_charge.edate);
		if(ret )
		{
			vtcp_log("[%s][%d]数据库操作错误,请联系中心机房!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		while(1)
		{
			ret = Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]数据库操作错误,请联系中心机房!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			else if(ret==100 )	break;
			
			iRecordNum ++;
			
			if(iRecordNum==1)	/**第一次写文件**/
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
				fprintf( fp,"~CMT编号|机构编号|交易序号|交易日期|计费单价|计费金额|备注|\n" );
				head_flag   =1 ;
			}
			fprintf( fp,"%s|%s|%s|%d|%d|%d|\n",hv_zf.cmtno,br_no,hv_zf.orderno,hv_zf.wt_date,AVERATE_PRICE,AVERATE_PRICE,"大额");
			
		}
		Hv_zf_Clo_Sel();
		totamt = totamt +  iRecordNum *AVERATE_PRICE;
		
		/*2.**开始处理非支付登记***/
		iRecordNum = 0;
		
		ret = Hv_fzf_Dec_Sel(g_pub_tx.reply,"br_no='%s' and wt_date<=%d and wt_date>=%d and cmtno in ('303')  and hv_sts ='1' and lw_ind='1' and otype='1' ",br_no,hv_charge.sdate,hv_charge.edate);
		if(ret )
		{
			vtcp_log("[%s][%d]数据库操作错误,请联系中心机房!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		while(1)
		{
			ret = Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]数据库操作错误,请联系中心机房!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			else if(ret==100 )	break;
			
			iRecordNum ++;
			
			if(head_flag!=1)	/**第一次写文件**/
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
				fprintf( fp,"~CMT编号|机构编号|交易序号|交易日期|计费单价|计费金额|备注|\n" );
				head_flag   =1 ;
			}
			fprintf( fp,"%s|%s|%s|%d|%d|%d|\n",hv_fzf.cmtno,br_no,hv_fzf.orderno,hv_fzf.wt_date,dGet_CMTNO_PRICE(hv_fzf.cmtno),dGet_CMTNO_PRICE(hv_fzf.cmtno),"大额");
			
		}
		Hv_fzf_Clo_Sel();
	   	totamt = totamt +  iRecordNum *PRICE_303;
	
	}
	Lv_c552r_Clo_Sel();
	
	
	if( !iRecordNum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		set_zd_data( DC_FILE_SND,"1");
		fclose(fp);
		
	}

OkExit:
	sprintf(cTotamt,"%15f",totamt);
	set_zd_data("0390",cTotamt);
		
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

char * iNum2TxType(char *ch,char *cTmpList)
{

	vtcp_log("[%s][%d]ch===[%s]\n",__FILE__,__LINE__,ch);
	
	char cName[20];
	memset(cName,0x00,sizeof(cName));

        if(!memcmp(ch,"001",3))
        {
                strcpy(cName,"普通贷记");
        }
        else if(!memcmp(ch,"002",3))
        {
                strcpy(cName,"普通借记");
        }
        else if(!memcmp(ch,"003",3))
        {
                strcpy(cName,"实时贷记");
        }
        else if(!memcmp(ch,"004",3))
        {
                strcpy(cName,"实时借记");

	}
        else if(!memcmp(ch,"005",3))
        {
                strcpy(cName,"定期贷记");
        }
        else if(!memcmp(ch,"006",3))
        {
                strcpy(cName,"定期借记");
        }
        else if(!memcmp(ch,"007",3))
        {
                strcpy(cName,"贷记退汇");
        }
        else if(!memcmp(ch,"008",3))
        {
                strcpy(cName,"普通借记回执");
        }
        else if(!memcmp(ch,"009",3))
        {
                strcpy(cName,"实时贷记回执");
        }
        else if(!memcmp(ch,"010",3))

	 {
                strcpy(cName,"实时借记回执");
        }
        else if(!memcmp(ch,"011",3))
        {
                strcpy(cName,"定期借记回执");
        }
        else if(!memcmp(ch,"012",3))
        {
                strcpy(cName,"通用信息");
        }
        else if(!memcmp(ch,"013",3))
        {
                strcpy(cName,"实时信息");
        }
        else if(!memcmp(ch,"301",3))
        {
                strcpy(cName,"小额查询");
        }
        else if(!memcmp(ch,"302",3))
        {
                strcpy(cName,"小额查复");

	 }
        else if(!memcmp(ch,"303",3))
        {
                strcpy(cName,"自由格式");
        }
        else if(!memcmp(ch,"319",3))
        {
                strcpy(cName,"申请退回");
        }
        else if(!memcmp(ch,"320",3))
        {
                strcpy(cName,"退回应答");
        }
        else if(!memcmp(ch,"321",3))
        {
                strcpy(cName,"申请撤销");
        }
        else if(!memcmp(ch,"322",3))
        {
                strcpy(cName,"撤销应答");
        }
        else if(!memcmp(ch,"323",3))
	{
	        strcpy(cName,"冲正申请");
        }
        else if(!memcmp(ch,"324",3))
        {
                strcpy(cName,"冲正应答");
        }
        else if(!memcmp(ch,"325",3))
        {
                strcpy(cName,"冲正通知");
        }
        else if(!memcmp(ch,"327",3))
        {
                strcpy(cName,"止付申请");
        }
        else if(!memcmp(ch,"328",3))
        {
                strcpy(cName,"止付应答");
        }
        else
                strcpy(cName,"未知交易类型");
        memcpy(cTmpList,cName,strlen(cName));

	vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmpList);
        return  cName;
}

int dGet_CMTNO_PRICE( char *cmtno)
{
	if(!strncmp(cmtno,"301",3))
		return PRICE_301;
	if(!strncmp(cmtno,"303",3))
		return PRICE_303;
	return 0;
}

char * cGet_LV_HV(char otype)
{
	char otype_des[5];
	memset(otype_des,0x00,sizeof(otype_des));
	
	if(otype==1)
		memcpy(otype_des,"大额",4);
	if(otype==2)
		memcpy(otype_des,"小额",4);
	
	return otype_des;
		
}
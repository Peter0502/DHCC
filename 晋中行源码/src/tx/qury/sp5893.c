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
#include "com_branch_c.h"

#define  PRICE_303      100.00
#define  PRICE_301      100.00
#define  PRICE_PKG	550.00
#define  PRICE_121	350.00
#define  PRICE_721	100.00

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
	struct com_branch_c	s_com_branch;
	
	char  otype[2];			/***大额还是小额的标识***/
	
	int   QUERY_DATE;		/***查询日期***/
	
	
	
	int    head_flag = 0 ;/****是否写了文件头的标识   1 已经写了文件头   0  没有写******/
	
	int    NUM_121	= 0;
	int    NUM_721	= 0;
	int    NUM_303	= 0;	/**时间段中303的笔数**/
	int    NUM_301  = 0;    /*时间段中301的笔数**/
	int    NUM_PKG  = 0;    /*小额的支付笔数**/
	int    NUM_HV   = 0;	/*大额的支付包的数目**/
	
	double AVERATE_PRICE = 0.0;/**用总收费-303-301的收费除以总的支付的笔数,得出支付交易的单笔平均费用**/
	double totamt        = 0.0;/**某机构该时间段内的收费总量**/
	char  cTotamt[15];		/***某机构收费的总和***/
	
	double	TMP_PRICE	=	0.0;
	
	int  total_record = 0;     /**总的记录个数**/
	int  charge_record = 0;		/*802记录的个数**/
	
	
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
	
	
	int  	iRecordNum_zf	=	0;
	int  	iRecordNum_ot	=	0;
	int	iRecordNum_fzf	=	0;
	int	iRecordNum_121	=	0;
	int	iRecordNum_721	=	0;
	double  totamt_zf	=	0.0;
	double  totamt_ot	=	0.0;
	double  totamt_fzf	=	0.0;
	double	totamt_121	=	0.0;
	double	totamt_721	=	0.0;
	int	iBrnoNum	=	0;


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

	memset(&s_com_branch,0x00,sizeof(s_com_branch));
	
	pub_base_sysinit();

	get_zd_data("0440",beg_mon);
	get_zd_data("0910",br_no);
	
	
	QUERY_DATE = apatoi(beg_mon,sizeof(beg_mon));
	QUERY_DATE = QUERY_DATE*100;

	
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && memcmp(br_no,g_pub_tx.tx_br_no,5))
	{
		vtcp_log("[%s][%d]非清算中心不能查询他行交易\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	
	/***如果是清算中心查询,可以查询一个汇总的信息***/
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && !memcmp(br_no,"99999",5))
	{
		vtcp_log("[%s][%d]非清算中心不能用99999机构号查询!\n",__FILE__,__LINE__);
		set_zd_data("0130","非清算中心不可用99999机构号查询!");
		goto ErrExit;
	}
	
	
	/****首先从hv_charge表中提取到计费开始结束时间*****/
	sprintf(cWherelist,"edate>=%d and edate<=%d   and ",QUERY_DATE+1,QUERY_DATE+31);
	
	strcat (cWherelist," 1=1  ");
	
	ret=Hv_chargemx_Sel(g_pub_tx.reply,&hv_charge,cWherelist);
	if(ret &&ret!=100)
	{
		vtcp_log("[%s][%d]数据库操作失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	if(ret==100&&charge_record==0)
	{
		vtcp_log("[%s][%d]数据库操作失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	
	
	/*****通过从hv_charge中获取的indate来 从hv_zf中获取记录  cmtno+tx_date,注意  802一天不能发两笔****/
	
	sprintf(cWherelist,"wt_date=%d and  cmtno='802'  and ",hv_charge.in_date);
	
	strcat (cWherelist," 1=1  ");

		
	ret=Hv_zf_Sel(g_pub_tx.reply,&hv_zf,cWherelist);
	if(ret &&ret!=100)
	{
		vtcp_log("[%s][%d]数据库操作失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	if(ret==100&&charge_record==0)
	{
		vtcp_log("[%s][%d]数据库操作失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	totamt = hv_zf.tx_amt * 100;
	memset(&hv_zf,0x00,sizeof(hv_zf));
	
		/***开始分多次从hv_charge中 取记录***/
		/***获取到303的笔数,和301的笔数***/
		NUM_303 = sql_count("hv_fzf",	" wt_date>=%d and wt_date<=%d and cmtno in ('303','301','313','721')  and lw_ind='1'  and otype='1' ",hv_charge.sdate,hv_charge.edate);
		NUM_121 = sql_count("hv_zf",	" wt_date>=%d and wt_date<=%d and cmtno in ('121')  and hv_sts  in('1','2') and lw_ind='1'   ",hv_charge.sdate,hv_charge.edate);
		
		/*********
		NUM_721 = sql_count("hv_fzf",	" wt_date>=%d and wt_date<=%d and cmtno in ('721')   and lw_ind='1'  and otype='1' ",hv_charge.sdate,hv_charge.edate);
		**************/
		NUM_HV =  sql_count("hv_zf",    " wt_date>=%d and wt_date<=%d and hv_sts not in('C','0') and lw_ind='1'  and cmtno not in ('108' ,'121')  ",hv_charge.sdate,hv_charge.edate);
		
		
		 
		vtcp_log("[%s][%d]NUM303==[%d],NUM_HV==[%d]\n",__FILE__,__LINE__,NUM_303,NUM_HV);
		
		vtcp_log("[%s][%d]总金额===[%lf]\n",__FILE__,__LINE__,totamt);
		
		if( (NUM_HV)!=0)
			AVERATE_PRICE = ( totamt-NUM_303*PRICE_303-NUM_121*PRICE_121-NUM_721*PRICE_721 ) /(NUM_HV);
		else 
			AVERATE_PRICE = 0.0;
		
		
		vtcp_log("[%s][%d]平均价格===[%lf]\n",__FILE__,__LINE__,AVERATE_PRICE);
		
		
		/*************提供清算中心的汇总查询*************/
		
		
		if(!memcmp(br_no,"99999",5))
		{
			vtcp_log("[%s][%d]这是清算中心打印汇总信息!\n",__FILE__,__LINE__);
			iBrnoNum=0	;
	
			/*ret = Com_branch_Dec_Sel(g_pub_tx.reply," br_type in ('4','1') order by br_no");20150525 lwb  增加分行   */
			ret = Com_branch_Dec_Sel(g_pub_tx.reply," br_type in ('4','1','3') order by br_no");
			
			if(ret)
			ERR_DEAL
			
			while(1)
			{
				
				ret = Com_branch_Fet_Sel(&s_com_branch,g_pub_tx.reply);
				if(ret && ret!=100)
				ERR_DEAL
				
				if(ret==100)
				{
					vtcp_log("[%s][%d]开始打印结束了!\n",__FILE__,__LINE__);
					fprintf(fp,"%d|%s|%d|%.2f|%s|",iBrnoNum+1,"总         计",NUM_PKG+NUM_303+NUM_HV,totamt,"小额");
					break;
				}
					
				iBrnoNum++;
				vtcp_log("[%s][%d]这是第【%d】次了，br_no=[%s]\n",__FILE__,__LINE__,iBrnoNum,s_com_branch.br_no);
				if(iBrnoNum==1)	/**第一次写文件**/
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
					sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
					vtcp_log("[%s][%d]开始打印了!!!!!!!\n",__FILE__,__LINE__);
					fprintf( fp,"~序号|机构名称|总笔数|总金额|电汇笔数|电汇金额|移存笔数|移存金额|非支付笔数|非支付金额|摘要|\n" );
					
				}
				
				iRecordNum_zf = 0;
				iRecordNum_fzf= 0;
				iRecordNum_ot = 0;
				iRecordNum_121 =0;
				
				totamt_121	= 0.0;
				totamt_ot	= 0.0;
				totamt_zf	= 0.0;
				totamt_fzf	= 0.0;
				
				iRecordNum_ot = sql_count ( "hv_zf" ,  " br_no = '%.5s' and wt_date>=%d and wt_date<=%d and hv_sts not in('0','C' ) and lw_ind='1' and cmtno not in ('108','121')   ",s_com_branch.br_no,hv_charge.sdate,hv_charge.edate);
				iRecordNum_121 = sql_count ( "hv_zf" ,  " br_no = '%.5s' and wt_date>=%d and wt_date<=%d and hv_sts  in('1','2' ) and lw_ind='1' and cmtno  in ('121')   ",s_com_branch.br_no,hv_charge.sdate,hv_charge.edate);
				iRecordNum_fzf = sql_count ( "hv_fzf" ,  "br_no='%s' and wt_date>=%d and wt_date<=%d and cmtno in ('301','303','313','721')   and lw_ind='1' and otype='1'  ",s_com_branch.br_no,hv_charge.sdate,hv_charge.edate);
				
				totamt_ot = iRecordNum_ot * AVERATE_PRICE;
				totamt_121= iRecordNum_121* PRICE_121;
				totamt_fzf= iRecordNum_fzf* PRICE_303;
				totamt_zf = totamt_ot + totamt_121;
				
				
				iRecordNum_zf = iRecordNum_121 + iRecordNum_ot;
				
				vtcp_log("[%s][%d]总金额=[%.2f],iRecordNum_zf=[%d],iRecordNum_fzf==[%d],iRecordNum_121==[%d]\n",__FILE__,__LINE__,totamt,iRecordNum_zf,iRecordNum_fzf,iRecordNum_121);
				fprintf(fp,"%d|%s|%d|%.2f|%d|%.2f|%d|%.2f|%d|%.2f|%s|\n",iBrnoNum,s_com_branch.br_name,iRecordNum_zf+iRecordNum_fzf,totamt_zf+totamt_fzf,
							iRecordNum_ot,totamt_ot,iRecordNum_121,totamt_121,iRecordNum_fzf,totamt_fzf,"小额");
				
			
			}
		
		
		
		
		set_zd_data( DC_FILE_SND,"1");
		
		fclose(fp);
		
		vtcp_log("[%s][%d]跳回去了!\n",__FILE__,__LINE__);
		goto OkExit;
		
		}
		
		/**1.开始处理pkgno报文****/
		iRecordNum = 0;
		ret = Hv_zf_Dec_Sel(g_pub_tx.reply," br_no = '%s' and wt_date>=%d and wt_date<=%d  and lw_ind='1'  and ( ( cmtno not in ('108' ,'121') and hv_sts not in('C','0') )  or ( cmtno='121' and hv_sts in ('1','2')) ) order by cmtno,wt_date ",br_no,hv_charge.sdate,hv_charge.edate);
		if(ret )
		{
			vtcp_log("[%s][%d]数据库操作错误,请联系中心机房!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		while(1)
		{
			TMP_PRICE= 0.0;
			ret = Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]数据库操作错误,请联系中心机房!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			else if(ret==100 )
			{	
				vtcp_log("[%s][%d]如果支付记录已经找完了,开始找非支付!\n",__FILE__,__LINE__);
				totamt_zf = iRecordNum_ot * AVERATE_PRICE + iRecordNum_121 * PRICE_121;
				vtcp_log("[%s][%d]支付交易的总金额===[%.2lf]\n",__FILE__,__LINE__,totamt_zf);
				iRecordNum_fzf  = 0;
		
		
				ret = Hv_fzf_Dec_Sel(g_pub_tx.reply,"br_no='%s' and wt_date>=%d and wt_date<=%d and cmtno in ('303','301','313','721')  and lw_ind='1'  and otype='1' ",br_no,hv_charge.sdate,hv_charge.edate);
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
					else if(ret==100 )
					{	
						vtcp_log("[%s][%d]已经找完了非支付!\n",__FILE__,__LINE__);
						totamt_fzf = iRecordNum_fzf * PRICE_303;
						vtcp_log("[%s][%d]非支付交易的总金额===[%.2lf]\n",__FILE__,__LINE__,totamt_fzf);
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
							vtcp_log("!!!!!!!!!!!!!!![%s]\n",cFilename);
							fprintf( fp,"~序号|CMT编号|机构编号|交易序号|交易日期|计费单价|计费金额|备注||||||\n" );
							head_flag   =1 ;
						}
						/**
						fprintf( fp,"%s|%d|%s|%.2f|%s|%d|%s|%.2f|%s|%d|%s|%.2f|\n","总笔数:",iRecordNum_zf+iRecordNum_fzf,"总金额:",totamt_zf+totamt_fzf,
							"支付笔数:",iRecordNum_zf,"支付总金额:",totamt_zf,"非支付笔数:",iRecordNum_fzf,"非支付总金额:",totamt_fzf);*/
						
						fprintf(fp,"%s|%s|%d|%s|%.2f|\n","         ","总笔数:",iRecordNum_zf+iRecordNum_fzf,"总金额:",totamt_zf+totamt_fzf);
						fprintf(fp,"%s|%s|%d|%s|%.2f|\n","         ","电汇笔数:",iRecordNum_ot,"电汇金额:",iRecordNum_ot * AVERATE_PRICE);
						fprintf(fp,"%s|%s|%d|%s|%.2f|\n","         ","移存笔数:",iRecordNum_121,"移存金额:",iRecordNum_121 * PRICE_121);
						fprintf(fp,"%s|%s|%d|%s|%.2f|\n","         ","非支付笔数:",iRecordNum_fzf,"非支付金额:",totamt_fzf);
						/***
						fprintf(fp,"%s|%d|%s|%.2f|%s|%d|%s|%.2f|%s|%d|%s|%.2f|\n","电汇等笔数:",iRecordNum_ot,"电汇等金额:",iRecordNum_ot * AVERATE_PRICE,
								"资金移存笔数:",iRecordNum_121,"移存金额：",iRecordNum_121 * PRICE_121,"非支付笔数:",iRecordNum_fzf,"非支付金额:",totamt_fzf);
						****/
						break;
					}
					
					iRecordNum_fzf ++;
					vtcp_log("[%s][%d]这是非支付的第[%d]次到!\n",__FILE__,__LINE__,iRecordNum_fzf);
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
						vtcp_log("!!!!!!!!!!!!!!![%s]\n",cFilename);
						fprintf( fp,"~序号|CMT编号|机构编号|交易序号|交易日期|计费单价|计费金额|备注||||||\n" );
						head_flag   =1 ;
					}
					vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
					fprintf( fp,"%d|%s|%s|%s|%d|%.2f|%.2f|%s||||||\n",iRecordNum_fzf+iRecordNum_zf,hv_fzf.cmtno,br_no,hv_fzf.orderno,hv_fzf.wt_date,PRICE_303,PRICE_303,"小额");
					vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
				 }
				 Hv_fzf_Clo_Sel();
				 break;  /***跳初小额支付对应的while循环***/
			}
			
			iRecordNum_zf ++;
			vtcp_log("[%s][%d]hv_zf的第[%d]次循环!\n",__FILE__,__LINE__,iRecordNum_zf);
			
			if(iRecordNum_zf==1)	/**第一次写文件**/
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
				fprintf( fp,"~序号|CMT编号|机构编号|交易序号|交易日期|计费单价|计费金额|备注||||||||\n" );
				head_flag   =1 ;
			}
			if(!memcmp(hv_zf.cmtno,"121",3))
			{
				iRecordNum_121 ++;
				TMP_PRICE = PRICE_121;
			}
			else
			{
				iRecordNum_ot ++;
				TMP_PRICE = AVERATE_PRICE;
			}	
				
			fprintf( fp,"%d|%s|%s|%s|%d|%.2f|%.2f|%s|\n",iRecordNum_zf,hv_zf.cmtno,br_no,hv_zf.orderno,hv_zf.wt_date,TMP_PRICE,TMP_PRICE,"大额");
			
		}
		
		
		Hv_zf_Clo_Sel();
		
		
		set_zd_data( DC_FILE_SND,"1");
		fclose(fp);
		

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

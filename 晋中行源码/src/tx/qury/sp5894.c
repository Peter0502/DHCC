/***** 以下是收费清单查询(sp5348)的主程序**********/
/*****************************************************
* 文 件 名:  sp5894.c
* 功能描述： 小额收费清单查询
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
#include "lv_c552r_c.h"
#include "lv_c554r_c.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
#include "hv_zf_c.h"
#include "hv_fzf_c.h"
#include "com_branch_c.h"


/******单位    元  *****/
#define  PRICE_303     100.00 
#define  PRICE_301     100 
#define  PRICE_PKG	   500 

/***对于大小额的支付交易和非支付交易的收费进行分开查询,自由格式可能收费比支付的低****/

int dGet_CMTNO_PRICE( char *cmtno);
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
int sp5894()
{
	vtcp_log("开始小额收费清单查询[%s][%d]",__FILE__,__LINE__);

	struct lv_c552r_c lv_c552;
	struct lv_c554r_c lv_c554;
	LV_FD123_AREA     fd123;
	struct lv_pkgreg_c lv_pkgreg;
	struct hv_fzf_c 	hv_fzf;
	struct hv_zf_c     hv_zf;
	
	struct com_branch_c s_com_branch;
	char  otype[2];			/***大额还是小额的标识***/
	
	int total_num = 0;		/*总计的记录数,小额的支付加上非支付***/
	
	
	
	int    head_flag = 0 ;/****是否写了文件头的标识   1 已经写了文件头   0  没有写******/
	
	int    NUM_303	= 0;	/**时间段中303的笔数**/
	int    NUM_301  = 0;    /*时间段中301的笔数**/
	int    NUM_PKG  = 0;    /*小额的支付笔数**/
	int    NUM_HV   = 0;	/*大额的支付包的数目**/
	int    c552_num = 0;    /*552记录的个数****/
	int    c552_indate = 0; /*由于受到552要再lv_c552中写入多条记录,但是我只需要一条,需通过indate加以区分***/
	
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

	char    old_br_no[6];	/***旧的机构,用来罗列记录的时候 分开机构***/
	
	int 	iRecordNum_zf	=	0;
	int	iRecordNum_fzf	=	0;
	double	totamt_zf	=	0.0;
	double	totamt_fzf	=	0.0;
	
	int	iBrnoNum 	=	0.0;/***支行的第几个***/
	
	


	memset(&tos88,	0x00,	sizeof(tos88));
	memset(&lv_c552,0x00,sizeof(lv_c552));
	memset(&lv_c554,0x00,sizeof(lv_c554));
	memset(&lv_pkgreg,0x00,sizeof(lv_pkgreg));
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

	memset(old_br_no,0x00,sizeof(old_br_no));
	
	
	memset(&s_com_branch,0x00,sizeof(s_com_branch));
	
	
	pub_base_sysinit();

	get_zd_data("0440",beg_mon);
	get_zd_data("0450",end_mon);
	get_zd_data("0910",br_no);
	get_zd_data("0680",otype);
	

	vtcp_log("[%s][%d]beg_mon=[%s]\n",__FILE__,__LINE__,beg_mon);
	vtcp_log("[%s][%d]end_mon=[%s]\n",__FILE__,__LINE__,end_mon);
	vtcp_log("[%s][%d]br_no=[%s]\n",__FILE__,__LINE__,br_no);
	
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && memcmp(br_no,g_pub_tx.tx_br_no,5))
	{
		vtcp_log("[%s][%d]非清算中心不能查询他行交易\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	sprintf(cWherelist,"paymon>='%.6s' and paymon<='%.6s' and ",beg_mon,end_mon);
	
	
	
	strcat (cWherelist," 1=1 order by paymon ");
	
	
	/***如果是清算中心查询,可以查询一个汇总的信息***/
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && !memcmp(br_no,"99999",5))
	{
		vtcp_log("[%s][%d]非清算中心不能用99999机构号查询!\n",__FILE__,__LINE__);
		set_zd_data("0130","非清算中心不可用99999机构号查询!");
		goto ErrExit;
	}
	
	if(!memcmp(br_no,"99999",5))
	{
		vtcp_log("[%s][%d]清算中心使用了99999机构号查询汇总信息",__FILE__,__LINE__);
		
		ret = Lv_c552r_Sel(g_pub_tx.reply,&lv_c552,cWherelist);
		ERR_DEAL
		
		totamt = atof(lv_c552.totamt);
		vtcp_log("[%s][%d]总金额===[%.2f]\n",__FILE__,__LINE__,totamt);
		/***开始分多次从lv_c552中 取记录***/
		/***获取到303的笔数,和301的笔数***/

		NUM_303 = sql_count("hv_fzf"," wt_date>=%d and wt_date<=%d and cmtno in ('303','301','319','321','323','325','327')  and hv_fzf_sts ='4' and lw_ind='1'  and otype='2' ",lv_c552.sdate,lv_c552.edate);
		NUM_PKG = sql_count("lv_pkgreg","wt_date>=%d and wt_date<=%d and lv_sts not in('0','1','2','3') and lw_ind='1' ",lv_c552.sdate,lv_c552.edate);
		
		if( (NUM_PKG+NUM_303)!=0) /***总的小额收费金额减去 非支付收取的金额 /总的支付笔数  得到单价 ****/
		{
			AVERATE_PRICE = ( totamt - NUM_303*PRICE_303 ) / NUM_PKG;
			vtcp_log("[%.2f]= [%.2f]-[%d * %d ]/[%d] \n",AVERATE_PRICE,totamt,NUM_303,PRICE_303,NUM_PKG);
		}
		else 
			AVERATE_PRICE = 0.0;
			
	
		iBrnoNum=0	;
	
		/*ret = Com_branch_Dec_Sel(g_pub_tx.reply," br_type in ('4','1') order by br_no"); lwb 20150525 加入分行 */
		ret = Com_branch_Dec_Sel(g_pub_tx.reply," br_type in ('4','1','3') order by br_no");
		ERR_DEAL
		
		while(1)
		{
			
			ret = Com_branch_Fet_Sel(&s_com_branch,g_pub_tx.reply);
			if(ret && ret!=100)
			ERR_DEAL
			
			if(ret==100)
			{
				vtcp_log("[%s][%d]开始打印结束了!\n",__FILE__,__LINE__);
				fprintf(fp,"%d|%s|%d|%.2f|%d|%.2f|%d|%.2f|%s|",iBrnoNum+1,"总         计",NUM_PKG+NUM_303,totamt,
						NUM_PKG,NUM_PKG*AVERATE_PRICE,NUM_303,NUM_303 * PRICE_303,"小额");
				
				break;
			}
				
			iBrnoNum++;
			
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
				fprintf( fp,"~序号|机构名称|总笔数|总金额|支付笔数|总支付金额|非支付笔数|非支付金额|摘要|\n" );
				
			}
			
			iRecordNum_zf = 0;
			iRecordNum_fzf= 0;
			totamt_zf	= 0.0;
			totamt_fzf	= 0.0;
			
			iRecordNum_zf = sql_count ( "lv_pkgreg" ,  " br_no = '%.5s' and wt_date>=%d and wt_date<=%d and lv_sts not in('0','1','2','3') and lw_ind='1'   ",s_com_branch.br_no,lv_c552.sdate,lv_c552.edate); 
			iRecordNum_fzf = sql_count ( "hv_fzf" ,  "br_no='%s' and wt_date>=%d and wt_date<=%d and cmtno in ('301','303','319','321','323','325','327')  and hv_fzf_sts ='4' and lw_ind='1' and otype='2'  ",s_com_branch.br_no,lv_c552.sdate,lv_c552.edate);
			totamt_zf = iRecordNum_zf * AVERATE_PRICE;
			totamt_fzf = iRecordNum_fzf * PRICE_303;
			
			
			vtcp_log("[%s][%d]总金额=[%s],iRecordNum_zf=[%d],iRecordNum_fzf==[%d]\n",__FILE__,__LINE__,lv_c552.totamt,iRecordNum_zf,iRecordNum_fzf);
			fprintf(fp,"%d|%s|%d|%.2f|%d|%.2f|%d|%.2f|%s|\n",iBrnoNum,s_com_branch.br_name,iRecordNum_zf+iRecordNum_fzf,totamt_zf+totamt_fzf,
						iRecordNum_zf,totamt_zf,iRecordNum_fzf,totamt_fzf,"小额");
			
			
		}
		
		
		
		
		set_zd_data( DC_FILE_SND,"1");
		
		fclose(fp);
		
		vtcp_log("[%s][%d]跳回去了!\n",__FILE__,__LINE__);
		goto OkExit;
		
		
	}
		
	ret=Lv_c552r_Dec_Sel(g_pub_tx.reply,cWherelist);
	if(ret)
	{
		vtcp_log("[%s][%d]数据库操作失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	while(1)
	{
		ret = Lv_c552r_Fet_Sel(&lv_c552,g_pub_tx.reply);
		if(ret &&ret!=100)
		{
			vtcp_log("[%s][%d]数据库操作失败!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}	
		else if(ret==100 &&c552_num==0)
		{
			vtcp_log("[%s][%d]找不到对应的记录!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		
		
		if(ret==100)	break;
		
		
		if(c552_num==0)
			c552_indate = lv_c552.in_date;
		if(c552_num && (c552_indate==lv_c552.in_date)) /***如果日期没有变化说明了是同一包552里面的记录***/
			continue;
		c552_indate = lv_c552.in_date;
		
		c552_num ++;
		
		
		/*str2dbl(lv_c552.totamt,sizeof(lv_c552.totamt)-1,2,&totamt);*/
		totamt = atof(lv_c552.totamt);
		vtcp_log("[%s][%d]总金额===[%.2f]\n",__FILE__,__LINE__,totamt);
		/***开始分多次从lv_c552中 取记录***/
		/***获取到303的笔数,和301的笔数***/

		NUM_303 = sql_count("hv_fzf"," wt_date>=%d and wt_date<=%d and cmtno in ('303','301','319','321','323','325','327')  and hv_fzf_sts ='4' and lw_ind='1'  and otype='2' ",lv_c552.sdate,lv_c552.edate);
		NUM_PKG = sql_count("lv_pkgreg","wt_date>=%d and wt_date<=%d and lv_sts not in('0','1','2','3') and lw_ind='1' ",lv_c552.sdate,lv_c552.edate);
		
		if( (NUM_PKG+NUM_303)!=0) /***总的小额收费金额减去 非支付收取的金额 /总的支付笔数  得到单价 ****/
		{
			AVERATE_PRICE = ( totamt - NUM_303*PRICE_303 ) / NUM_PKG;
			vtcp_log("[%.2f]= [%.2f]-[%d * %d ]/[%d] \n",AVERATE_PRICE,totamt,NUM_303,PRICE_303,NUM_PKG);
		}
		else 
			AVERATE_PRICE = 0.0;
			
			
		vtcp_log("[%s][%d]总金额=[%s],NUM_303=[%d],NUM_PKG==[%d]\n",__FILE__,__LINE__,lv_c552.totamt,NUM_303,NUM_PKG);
		vtcp_log("[%s][%d]支付交易的单笔价格===[%.2lf]\n",__FILE__,__LINE__,AVERATE_PRICE);	
		/**1.开始处理pkgno报文****/
	
		ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply," br_no = '%s' and wt_date>=%d and wt_date<=%d and lv_sts not in('0','1','2','3') and lw_ind='1'  order by pkgno,wt_date,orderno ",br_no,lv_c552.sdate,lv_c552.edate);
		if(ret )
		{
			vtcp_log("[%s][%d]数据库操作错误,请联系中心机房!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		while(1)
		{
			ret = Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]数据库操作错误,请联系中心机房!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			else if(ret==100 )
			{	
				vtcp_log("[%s][%d]如果支付记录已经找完了,开始找非支付!\n",__FILE__,__LINE__);
				totamt_zf = iRecordNum_zf * AVERATE_PRICE;
				vtcp_log("[%s][%d]支付交易的总金额===[%.2lf]\n",__FILE__,__LINE__,totamt_zf);
				iRecordNum_fzf  = 0;
		
				ret = Hv_fzf_Dec_Sel(g_pub_tx.reply,"br_no='%s' and wt_date>=%d and wt_date<=%d and cmtno in ('301','303','319','321','323','325','327')  and hv_fzf_sts ='4' and lw_ind='1' and otype='2' order by cmtno,wt_date,orderno ",br_no,lv_c552.sdate,lv_c552.edate);
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
							fprintf( fp,"~CMT编号|机构编号|交易序号|交易日期|计费单价|计费金额|备注||||||\n" );
							head_flag   =1 ;
						}
						fprintf( fp,"%s|%d|%s|%.2f|%s|%d|%s|%.2f|%s|%d|%s|%.2f|\n","总笔数:",iRecordNum_zf+iRecordNum_fzf,"总金额:",totamt_zf+totamt_fzf,
								"支付笔数:",iRecordNum_zf,"支付总金额:",totamt_zf,"非支付笔数:",iRecordNum_fzf,"非支付总金额:",totamt_fzf);
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
						fprintf( fp,"~CMT编号|机构编号|交易序号|交易日期|计费单价|计费金额|备注||||||\n" );
						head_flag   =1 ;
					}
					vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
					fprintf( fp,"%s|%s|%s|%d|%.2f|%.2f|%s||||||\n",hv_fzf.cmtno,br_no,hv_fzf.orderno,hv_fzf.wt_date,PRICE_303,PRICE_303,"小额");
					vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
				 }
				 Hv_fzf_Clo_Sel();
				 break;  /***跳初小额支付对应的while循环***/
			}
					
					
			vtcp_log("[%s][%d]这是支付的第[%d]次到!\n",__FILE__,__LINE__,iRecordNum_zf);
			iRecordNum_zf ++;
			
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
				fprintf( fp,"~CMT编号|机构编号|交易序号|交易日期|计费单价|计费金额|备注||||||\n" );
				head_flag   =1 ;
			}
			
			fprintf( fp,"%s|%s|%s|%d|%.2f|%.2f|%s|\n",lv_pkgreg.pkgno,br_no,lv_pkgreg.orderno,lv_pkgreg.wt_date,AVERATE_PRICE,AVERATE_PRICE,"小额");
			
		}
		Lv_pkgreg_Clo_Sel();
		
		
	}
	Lv_c552r_Clo_Sel();
	
	
	/*if( !total_num )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		set_zd_data( DC_FILE_SND,"1");
		fclose(fp);
		
	}**/
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

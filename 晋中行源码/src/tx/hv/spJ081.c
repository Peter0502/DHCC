/***************************************************************
* 文 件 名:     spJ081.c
* 功能描述：    大额历史信息查询
*
* 修改记录：    满足银行的多重需求
* 日    期:     20060921
* 修 改 人:     liuyue
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"


char * iNum2TxType(char *ch,char *cTmpList);
char * iNum2Stat(char ch,char *cTmpList);

static char cCmtno[4];       /*** 20110107 大额查复 ***/

spJ081()
{
	int ret = 0;
	HV_NOTPAY_AREA hv_notpayin;
	HV_FD123_AREA  fd123;
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;

	char    cLw_ind[2];	  /*来往标志 1 往  2 来*/
	char    cBr_no[6];        /*机构号*/
	char    cTx_date[9];      /*交易日期*/
	char    cZf_type[2];      /*支付标志 0 支付类  1 非支付类 */
	char    cTx_type[4];      /*支付交易类型*/
	char    cTx_stat[2];       /*交易状态*/
	int     ttlnum = 0;       /*读取的总条数*/
	FILE    *fp;
	char    cFilename[100];    /*文件名*/
	char    cWherelist[2000];
	char    cTmplist[200];
	int  	iTx_date = 0;
	int	iBr_no	 = 0;
	double Tamt=0.00;
	char    cStat_Desc[20];
	char 	cTx_Desc[20];

	memset(cCmtno , 0 , sizeof(cCmtno));                     /*** 20110107 大额查复 ***/
	memset(cLw_ind,		0x00 ,	sizeof(cLw_ind));
	memset(cBr_no,		0x00 ,	sizeof(cBr_no ));
	memset(cTx_date,	0x00 ,	sizeof(cTx_date));
	memset(cZf_type,	0x00 ,	sizeof(cZf_type));
	memset(cTx_type,	0x00 ,	sizeof(cTx_type));
	memset(cTx_stat,	0x00 ,	sizeof(cTx_stat ));
	memset(cFilename,	0x00 ,	sizeof(cFilename));
	memset(&fd123,		0x00 ,	sizeof(fd123));
	memset(&hv_zf,		0x00 ,	sizeof(struct hv_zf_c));
	memset(&hv_fzf,		0x00 ,	sizeof(struct hv_fzf_c));
	memset(&hv_notpayin,  	0x00 ,	sizeof(hv_notpayin));
	memset(cWherelist,	0x00 ,	sizeof(cWherelist));
	memset(cTmplist,	0x00 ,	sizeof(cTmplist));

	memset(cStat_Desc,0x00,sizeof(cStat_Desc));
	memset(cTx_Desc,0x00,	sizeof(cTx_Desc));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0670",cLw_ind);     
	get_zd_data("0280",cBr_no);
	get_zd_data("0440",cTx_date);
	get_zd_data("0680",cZf_type);
	get_zd_data("0230",cTx_type);
	get_zd_data("0690",cTx_stat);

	vtcp_log("[%s][%d]cBr_no==[%s]\n",__FILE__,__LINE__,cBr_no);

	vtcp_log("[%s][%d]开始大额的查询交易!\n",__FILE__,__LINE__);
	/*********获取附加域**************/
	pub_base_AllDwnFilName(cFilename);
	fp = fopen(cFilename,"rb");
	if(fp==NULL)
	{
		sprintf(acErrMsg,"打开文件[%s]错误!\n",cFilename);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
	}
	fclose(fp);


	/********清算中心可以查询它行的交易*****/

	/**组成查询条件**/
	if(memcmp(g_pub_tx.tx_br_no,HV_CZ_QSBRNO,5))  /****如果不是清算中心**/
	{
		zip_space(cBr_no);
		iBr_no	= atoi(cBr_no);
		if(iBr_no==0)
		{
			vtcp_log("[%s][%d]非清算中心,请输入本机构号!\n",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"非清算中心,请输入本机构号!");
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;	
		}

		if(memcmp(g_pub_tx.tx_br_no,cBr_no,sizeof(cBr_no)-1))
		{
			vtcp_log("[%s][%d]非清算中心不允许查询它行业务!\n",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"非清算中心不能查询它行的业务!");
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
		sprintf(cTmplist,"br_no = '%s' and ",cBr_no);
		strcat(cWherelist,cTmplist);
	}
	else
	{
		zip_space(cBr_no);
		iBr_no = atoi(cBr_no);
		if(iBr_no!=0&&strlen(cBr_no)==BRNO_LEN)
		{
			sprintf(cTmplist,"br_no = '%s' and ",cBr_no);
			strcat(cWherelist,cTmplist);
		}
	}
	/****如果输入的日期为空查询所有的记录****/

	zip_space(cTx_date);
	iTx_date = atoi(cTx_date);

	if(iTx_date!=0 )
	{
		sprintf(cTmplist," wt_date= '%s' and ",cTx_date,cTx_date);
		strcat(cWherelist,cTmplist);
	}

	if(memcmp(cTx_type,"888", sizeof(cTx_type)-1) &&cZf_type[0]=='0')/*****如果选择的不是全部 而且是大额的支付***/
	{
		sprintf(cTmplist,"cmtno = '%s' and ",cTx_type);
		strcat(cWherelist,cTmplist);
	}
	if(memcmp(cTx_type,"888", sizeof(cTx_type)-1) &&cZf_type[0]=='1')/*****如果选择的不是全部 而且是大额的非支付***/
	{ 
		if(apatoi(cTx_type,sizeof(cTx_type)-1)%2!=0)/**大额非支付的申请业务**/
				sprintf(cTmplist,"cmtno = '%s' and ",cTx_type);
		else  if(apatoi(cTx_type,sizeof(cTx_type)-1)%2==0)/**大额非支付的应答业务**/
				sprintf(cTmplist,"(cmtno = '%s' or (cmtno = '%d' and res_orderno is not null) )  and ",cTx_type,apatoi(cTx_type,sizeof(cTx_type)-1)-1);
	
		strcat(cWherelist,cTmplist);
	}
	if(cTx_stat[0]!='Z'&&cZf_type[0]=='1')/***非支付类 而且状态不是选择全部**/
	{
		sprintf(cTmplist,"hv_fzf_sts ='%c'  and  ",cTx_stat[0]);
		strcat(cWherelist,cTmplist);
	}
	else if(cTx_stat[0]!='Z'&&cZf_type[0]=='0') /**支付类*/
	{
		sprintf(cTmplist,"hv_sts ='%c' and ",cTx_stat[0]);
		strcat(cWherelist,cTmplist);
	}
	if(cZf_type[0]=='1')
	{
		sprintf(cTmplist,"otype = '%c' and ",'1');
		strcat(cWherelist,cTmplist);
	}
	/***********根据要求不再用lw_ind作为标识了 ,改用pay_qs_no作为标识***/
	if(cZf_type[0]=='1')/*是非支付**/
	{
		vtcp_log("[%s][%d]lw_ind=[%c]\n",__FILE__,__LINE__,cLw_ind[0]);
		if(apatoi(cTx_type,sizeof(cTx_type)-1)==301 || apatoi(cTx_type,sizeof(cTx_type)-1)==311 || apatoi(cTx_type,sizeof(cTx_type)-1)==313) /**大额非支付的申请业务**/
		{
			if(cLw_ind[0]=='1')/*wangzhang ***/
				sprintf(cWherelist+strlen(cWherelist)," pay_qs_no='%s' and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);
			else if(cLw_ind[0]=='2')/*laizhang***/
				sprintf(cWherelist+strlen(cWherelist),"( pay_qs_no<>'%s' or pay_qs_no is null ) and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);

		}
		else if(apatoi(cTx_type,sizeof(cTx_type)-1)==302||apatoi(cTx_type,sizeof(cTx_type)-1)==312 || apatoi(cTx_type,sizeof(cTx_type)-1)==314)  /**大额支付的应答类业务****/
		{
			if(cLw_ind[0]=='1')/*wangzhang ***/
				sprintf(cWherelist+strlen(cWherelist)," (  pay_qs_no <>'%s' or pay_qs_no is null ) and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);
			else if(cLw_ind[0]=='2')/*laizhang***/
				sprintf(cWherelist+strlen(cWherelist),"  pay_qs_no = '%s'  and 1=1 order by br_no,wt_date,cmtno",HV_QS_BR_NO);
			
		}
		else
		{
			if(cLw_ind[0]=='1')/*wangzhang ***/
				sprintf(cWherelist+strlen(cWherelist)," pay_qs_no='%s' and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);
			else if(cLw_ind[0]=='2')/*laizhang***/
				sprintf(cWherelist+strlen(cWherelist),"( pay_qs_no<>'%s' or pay_qs_no is null ) and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);
		}
	}
	else  if(cZf_type[0]=='0')/*是支付**/
	{
		sprintf(cTmplist," lw_ind ='%c' order by br_no,wt_date,cmtno ",cLw_ind[0]);
		strcat(cWherelist,cTmplist);
	}


	vtcp_log("[%s][%d]cWherelist=[%s]\n",__FILE__,__LINE__,cWherelist);

	vtcp_log("[%s][%d]支付类型===[%c]\n",__FILE__,__LINE__,cZf_type[0]);
	/**开始查询**/
	/***支付交易类的查询***/
	if(cZf_type[0] == '0')
	{
		vtcp_log("[%s][%d]查询支付类交易!\n",__FILE__,__LINE__);
		ret = Hv_zf_Dec_Sel(g_pub_tx.reply, cWherelist);
		if(ret)
		{
			sprintf(acErrMsg,"查询大额支付交易基本信息出错[%d]",ret);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"查询大额支付交易基本信息出错");
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
		Tamt=0.00;
		ttlnum=0;
		while(1)
		{
			ret=Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
			if( ret==100 &&ttlnum>0 )
			{
				break;
			}
			else if(ret==100 &&ttlnum==0)
			{
				vtcp_log("[%s][%d]没有找到对应的记录!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"找不到对应的记录!\n");
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			else if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"数据库操作异常,请与科技科联系!\n");
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
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
				fprintf( fp,"~CMT编号|交易类型|交易行|顺序号|交易日期|交易金额|发起行|接受行|状态|\n" );
			}
			memset(cCmtno , 0 , sizeof(cCmtno));                    /*** 20110107 大额查复 ***/
			memcpy(cCmtno , hv_fzf.cmtno, sizeof(cCmtno)-1);        /*** 20110107 大额查复 ***/
			memset(cTmplist,0x00,sizeof(cTmplist));
			iNum2TxType(hv_zf.cmtno,cTx_Desc);
			vtcp_log("[%s][%d]第【%d】笔,交易类型=[%s]\n",__FILE__,__LINE__,ttlnum,iNum2TxType(hv_zf.cmtno,cTx_Desc));
			fprintf( fp,"%s|%s|%s|%s|%ld|%.2f|%s|%s|%s|\n",hv_zf.cmtno,cTx_Desc,hv_zf.br_no,hv_zf.orderno,hv_zf.wt_date,hv_zf.tx_amt,hv_zf.or_br_no,hv_zf.ac_br_no,iNum2Stat(hv_zf.hv_sts[0],cStat_Desc));
			ttlnum++;
			Tamt += hv_zf.tx_amt;
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));

		}
		fprintf( fp,"共计:|%d笔|金额总计:|%.2f|\n",ttlnum,Tamt);      
		ret=Hv_zf_Clo_Sel();
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

	else if(cZf_type[0]=='1') /***非支付类交易查询***/
	{
		ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, cWherelist);
		if(ret)
		{
			sprintf(acErrMsg,"查询大额非支付交易基本信息出错[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ttlnum=0;
		while(1)
		{
			ret=Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
			if( ret==100 &&ttlnum>0 )
			{
				break;
			}
			else if(ret==100 &&ttlnum==0)
			{
				vtcp_log("[%s][%d]没有找到对应的记录!\n",__FILE__,__LINE__);
				goto ErrExit;
			}
			else if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"数据库操作异常,请与科技科联系!\n");
				goto ErrExit;
			}
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
				fprintf( fp,"~CMT编号|交易类型|交易行|顺序号|交易日期|交易金额|发  起  行|接  受  行|状     态|\n");
			}
			memset(cCmtno , 0 , sizeof(cCmtno));        /*** 20110107 大额查复***/
			memcpy(cCmtno , hv_fzf.cmtno, sizeof(cCmtno)-1);     /*** 20110107 大额查复***/
vtcp_log("[%s][%d]***cCmtno==[%s]",__FILE__,__LINE__,cCmtno);
			iNum2TxType(hv_fzf.cmtno,cTx_Desc);
			vtcp_log("[%s][%d]第【%d】笔,交易类型=[%s]\n",__FILE__,__LINE__,ttlnum,iNum2TxType(hv_fzf.cmtno,cTmplist));
			fprintf( fp,"%s|%s|%s|%s|%ld|%.2f|%s|%s|%s|\n",hv_fzf.cmtno,cTx_Desc,hv_fzf.br_no,hv_fzf.orderno,hv_fzf.wt_date,hv_fzf.o_tx_amt,hv_fzf.or_br_no,hv_fzf.ac_br_no,iNum2Stat(hv_fzf.hv_fzf_sts[0],cStat_Desc));
			ttlnum++;
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));

		}
		ret=Hv_fzf_Clo_Sel();
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



OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);

	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;

}

	/**交易类型**/
char * iNum2TxType(char *ch,char *cTmpList)
{

	vtcp_log("[%s][%d]ch===[%s]\n",__FILE__,__LINE__,ch);

	char cName[20];
	memset(cName,0x00,sizeof(cName));

	if(!memcmp(ch,"100",3))
	{           
		strcpy(cName,"大额汇兑");
	} 
	else if(!memcmp(ch,"101",3))
	{
		strcpy(cName,"委托收款");
	}
	else if(!memcmp(ch,"105",3))
	{
		strcpy(cName,"同业拆借");
	}
	else if(!memcmp(ch,"103",3))
	{
		strcpy(cName,"国库资金贷记划拨");
	}
	else if(!memcmp(ch,"102",3))
	{
		strcpy(cName,"托收承付");
	}
	else if(!memcmp(ch,"109",3))
	{
		strcpy(cName,"电子联行");
	}
	else if(!memcmp(ch,"108",3))
	{
		strcpy(cName,"支付退汇");
	}
	else if(!memcmp(ch,"313",3))
	{
		strcpy(cName,"申请退回");
	}
	else if(!memcmp(ch,"314",3))
	{
		strcpy(cName,"退回应答");
	}
	else if(!memcmp(ch,"301",3))
	{
		strcpy(cName,"大额查询");
	}
	else if(!memcmp(ch,"302",3))
	{
		strcpy(cName,"大额查复");
	}
	else if(!memcmp(ch,"303",3))
	{
		strcpy(cName,"自由格式");
	}
	else if(!memcmp(ch,"311",3))
	{
		strcpy(cName,"申请撤销");
	}
	else if(!memcmp(ch,"407",3))
	{
		strcpy(cName,"质押融资");
	}
	else if(!memcmp(ch,"408",3))
	{
		strcpy(cName,"质押融资扣款");
	}
	else if(!memcmp(ch,"121",3))
	{
		strcpy(cName,"资金移存");
	}
	else if(!memcmp(ch,"721",3))
	{
		strcpy(cName,"申请资金清算");
	}
	else if(!memcmp(ch,"122",3))
	{
		strcpy(cName,"银行资金清算");
	}
	else if(!memcmp(ch,"123",3))
	{
		strcpy(cName,"多余资金退回");
	}
	else if(!memcmp(ch,"725",3))
	{
		strcpy(cName,"全额兑付通知");
	}
	else if(!memcmp(ch,"724",3))
	{
		strcpy(cName,"汇票申请退回");
	}
	else if(!memcmp(ch,"124",3))
	{
		strcpy(cName,"汇票未用退回");
	}
	else
		strcpy(cName,"未知交易类型");



	MEMCPY_DEBUG(cTmpList,cName,strlen(cName));
	vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmpList);
	return  cName;
}

/**交易状态**/
char * iNum2Stat(char ch,char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
	if(ch=='0')
	{
		strcpy(cTmpList,"往账复核");
	}
	else if(ch=='1')
	{
		strcpy(cTmpList,"往账发送");
	}
	else  if(ch=='2')
	{
	vtcp_log("[%s][%d]cCmtno==***===[%s]\n",__FILE__,__LINE__,cCmtno);
		if(memcmp(cCmtno,"301",3)==0)                /*** 20110107 大额查复 ***/
		{
			strcpy(cTmpList,"查询已查复");       /*** 20110107 大额查复 ***/
		}
		else
		{
			strcpy(cTmpList,"往账记账清算");
		}
	}
	else if(ch=='3')
	{
		strcpy(cTmpList,"往账退回记账");
	}
	else if(ch=='4')
	{
		strcpy(cTmpList,"往账退回等待");
	}
	else if(ch=='5')
	{
		strcpy(cTmpList,"往账拒绝");
	}
	else if(ch=='6')
	{
		strcpy(cTmpList,"往账撤销记账");
	}
	else if(ch=='7')
	{
		strcpy(cTmpList,"往账撤销等待");
	}
	else if(ch=='8')
	{
		strcpy(cTmpList,"来账拒绝");
	}
	else if(ch=='9')
	{
		if(cCmtno[0]=='1')                                 /*** 20110107 大额查复 ***/
		{
			strcpy(cTmpList,"来账记账清算");
		}
		else
		{
			if(memcmp(cCmtno,"301",3)==0)              /*** 20110107 大额查复 ***/
			{
				strcpy(cTmpList, "查询待查复");
			}else if(memcmp(cCmtno,"313",3)==0)
			{
				 strcpy(cTmpList, "退回待应答");
			}
			else
			{
				strcpy(cTmpList, "来账正常接收");
			}
		}	
	}
	else if(ch=='A')
	{
		strcpy(cTmpList,"往账登记取消");
	}
	else if(ch=='B')
	if(memcmp(cCmtno,"301",3)==0 || memcmp(cCmtno,"302",3)==0 )              /*** 20110107 大额查复 ***/
	{
		strcpy(cTmpList, "查询已查复");
	}
	else if(memcmp(cCmtno,"313",3)==0 || memcmp(cCmtno,"314",3)==0 )              /*** 20110107 大额查复 ***/
	{
		strcpy(cTmpList, "退回已应答");
	}
	else
	{
		strcpy(cTmpList,"来账退回");
	}
	else if(ch=='C')
	{
		strcpy(cTmpList,"往帐登记(不记账)");
	}
	else if(ch=='D')
	{
		strcpy(cTmpList,"往账复核取消");
	}
	else if(ch=='E')
	{
		strcpy(cTmpList,"来账记账登记");
	}
	else if(ch=='F')
	{
		strcpy(cTmpList,"来账核地方押错");
	}
	else if(ch=='G')
	{
		strcpy(cTmpList,"来账核全国押错");
	}
	else if(ch=='H')
	{
		strcpy(cTmpList,"清算中心挂帐");
	}
	else if(ch=='I')
	{
		strcpy(cTmpList,"支行挂帐");
	}
	else if(ch=='J')
	{
		strcpy(cTmpList,"往账排队");
	}
	else if(ch=='K')
	{
		strcpy(cTmpList,"核对清算记帐");
	}
	else
		strcpy(cTmpList,"未知状态");

	vtcp_log("[%s][%d]cTmpList==[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;

}


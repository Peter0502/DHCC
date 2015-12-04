/***************************************************************
* 文 件 名:     sp9569.c
* 功能描述：    小额据包查询记录明细
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
#include "lv_pkgreg_c.h"
#include "lv_define.h"
#include  "lv_wbctl_c.h"
#include "lv_lbctl_c.h"

char * iNum2TxType(char *ch,char *cTmpList);
char * iNum2Stat(char ch,char *cTmpList);
char * iNum2RcpStat(char *rcpstat,char *cTmpList ,char *pkgno);

sp9569()
{


	vtcp_log("[%s][%d]根据包信息查询记录详细信息\n",__FILE__,__LINE__);
	int ret = 0;
	HV_NOTPAY_AREA hv_notpayin;
	HV_FD123_AREA  fd123;
	struct lv_pkgreg_c  lv_pkgreg;
	struct hv_fzf_c hv_fzf;
	struct lv_wbctl_c lv_wbctl;
	struct lv_lbctl_c lv_lbctl;

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
	char    cStat[21];
	char 	cRcp_stat[21];
	int 	iCmtno;
	char    cPack_date[9];
	char    cPack_id  [9];
	char    cPkgno    [4];
	char    cQs_brno  [13];
	int     iPackid = 0;



	memset(&lv_wbctl,	0x00 ,	sizeof(lv_wbctl));
	memset(&lv_lbctl,	0x00 , 	sizeof(lv_lbctl));
	memset(cLw_ind,		0x00 ,	sizeof(cLw_ind));
	memset(cBr_no,		0x00 ,	sizeof(cBr_no ));
	memset(cTx_date,	0x00 ,	sizeof(cTx_date));
	memset(cZf_type,	0x00 ,	sizeof(cZf_type));
	memset(cTx_type,	0x00 ,	sizeof(cTx_type));
	memset(cTx_stat,	0x00 ,	sizeof(cTx_stat ));
	memset(cFilename,	0x00 ,	sizeof(cFilename));
	memset(&fd123,		0x00 ,	sizeof(fd123));
	memset(&lv_pkgreg,	0x00 ,	sizeof(struct lv_pkgreg_c));
	memset(&hv_fzf,		0x00 ,	sizeof(struct hv_fzf_c));
	memset(&hv_notpayin,0x00 ,	sizeof(hv_notpayin));
	memset(cWherelist,	0x00 ,	sizeof(cWherelist));
	memset(cTmplist,	0x00 ,	sizeof(cTmplist));
	memset(cPack_date,	0x00 ,	sizeof(cPack_date));
	memset(cStat,		0x00 ,	sizeof(cStat));
	memset(cRcp_stat,	0x00 ,	sizeof(cRcp_stat));
	memset(cPack_date,	0x00 ,	sizeof(cPack_date));
	memset(cPack_id,	0x00 ,	sizeof(cPack_id));
	memset(cPkgno,		0x00 ,	sizeof(cPkgno));
	memset(cQs_brno,	0x00 ,	sizeof(cQs_brno));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0440",cPack_id);     
	get_zd_data("0450",cPack_date);
	get_zd_data("0670",cLw_ind);
	get_zd_data("0520",cQs_brno);
	
	
	
	iPackid = atoi(cPack_id);
	sprintf(cPack_id,"%08d",iPackid);
	
	
	vtcp_log("[%s][%d]cPack_id==[%s] cPack_date==[%s] cLw_ind=[%c]\n",__FILE__,__LINE__,cPack_id,cPack_date,cLw_ind[0]);
	vtcp_log("[%s][%d]cQs_brno==[%s]\n",__FILE__,__LINE__,cQs_brno);
	
	if(cLw_ind[0]=='1')/***往包****/
	{
		ret = Lv_wbctl_Sel(g_pub_tx.reply,&lv_wbctl,"packid = '%s' and pack_date = '%s'  ",cPack_id,cPack_date);
		if(ret)
		{
			vtcp_log("[%s][%d]查询lv_wbctl错误[%s][%s]\n",cPack_id,cPack_date);
			goto ErrExit;
		}
		memcpy(cPkgno,lv_wbctl.pkgno,sizeof(lv_wbctl.pkgno)-1);
		memcpy(cBr_no,lv_wbctl.br_no,sizeof(lv_wbctl.br_no)-1);
		
	}
	else if(cLw_ind[0]=='2')/***来帐****/
	{
		ret = Lv_lbctl_Sel(g_pub_tx.reply,&lv_lbctl,"packid = '%s' and pack_date = '%s'  and pay_qs_no = '%s' ",cPack_id,cPack_date,cQs_brno);
		if(ret)
		{
			vtcp_log("[%s][%d]查询lv_wbctl错误[%s][%s]\n",cPack_id,cPack_date);
			goto ErrExit;
		}
		memcpy(cPkgno,lv_lbctl.pkgno,sizeof(lv_lbctl.pkgno)-1);
		memcpy(cBr_no,lv_lbctl.br_no,sizeof(lv_lbctl.br_no)-1);
		
	}
		
	
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
	
	sprintf(cTmplist,"br_no = '%s' and ",cBr_no);
	
	/****如果输入的日期为空查询所有的记录****/
	if(cLw_ind[0]=='1')
	{
		sprintf(cWherelist,"br_no  like  '%%%s%%' and pack_date='%s'  and  packid = '%s' and pkgno='%s' and lw_ind ='%c'",cBr_no,cPack_date,cPack_id,cPkgno,cLw_ind[0]);
	}
	else if(cLw_ind[0]=='2')
	{
		sprintf(cWherelist," pack_date='%%%s%%'  and  packid = '%s' and pkgno='%s' and lw_ind ='%c'",cPack_date,cPack_id,cPkgno,cLw_ind[0]);
	}
	vtcp_log("[%s][%d]cWherelist==[%s]\n",__FILE__,__LINE__,cWherelist);
	
	ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, cWherelist);
	if(ret)
	{
		sprintf(acErrMsg,"查询小额支付交易基本信息出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	ttlnum=0;
	while(1)
	{
		ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
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
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
		/***看lv_pkgreg 中的or_br_no是否是这一清算行对应的机构***/
		if(!iInSameBrno(lv_pkgreg.or_br_no,cQs_brno))
		{
			break;
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
			fprintf( fp,"~PKG编号|交易类型|交易行|顺序号|交易日期|交易金额|发起行|接受行|包序号  |包日期  |交易状态|回执状态|\n" );
		}
		if(lv_pkgreg.pack_date==0)
			strcpy(cPack_date,"        ");
		else
			sprintf(cPack_date,"%08d",lv_pkgreg.pack_date);

		memset(cTmplist,0x00,sizeof(cTmplist));
		vtcp_log("[%s][%d]cTmplist===[%s]\n",__FILE__,__LINE__,cTmplist);
		vtcp_log("[%s][%d]tx_type==[%s]\n",__FILE__,__LINE__,iNum2TxType(lv_pkgreg.pkgno,cTmplist));
		vtcp_log("[%s][%d]第【%d】笔,交易类型=[%s]\n",__FILE__,__LINE__,ttlnum,iNum2TxType(lv_pkgreg.pkgno,cTmplist));
		fprintf( fp,"%s|%s|%s|%s|%ld|%.2f|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.pkgno,cTmplist,lv_pkgreg.br_no,lv_pkgreg.orderno,lv_pkgreg.wt_date,lv_pkgreg.tx_amt,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.packid,cPack_date,iNum2Stat(lv_pkgreg.lv_sts[0],cStat),iNum2RcpStat(lv_pkgreg.rcpstat,cRcp_stat,lv_pkgreg.pkgno));
		ttlnum++;

	}      
	ret=Lv_pkgreg_Clo_Sel();
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
	





OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
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

/**交易状态**/
char * iNum2Stat(char ch,char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
	if(ch=='0')
	{
		strcpy(cTmpList,"无效状态");
	}
	else if(ch=='1')
	{
		strcpy(cTmpList,"往账录入");
	}
	else if(ch=='3')
	{
		strcpy(cTmpList,"往账复核");
	}
	else if(ch=='4')
	{
		strcpy(cTmpList,"往账发送");
	}
	else if(ch=='5')
	{
		strcpy(cTmpList,"往账拒绝");
	}
	else if(ch=='6')
	{
		strcpy(cTmpList,"往账扎差");
	}
	else if(ch=='7')
	{
		strcpy(cTmpList,"往账排队");
	}
	else if(ch=='8')
	{
		strcpy(cTmpList,"往账撤销等待");
	}
	else if(ch=='9')
	{
		strcpy(cTmpList,"往账撤销");
	}
	else if(ch=='A')
	{
		strcpy(cTmpList,"退回冲正止付等待");
	}
	else if(ch=='B')
	{
		strcpy(cTmpList,"往账退回");
	}
	else if(ch=='C')
	{
		strcpy(cTmpList,"往账已冲正");
	}
	else if(ch=='D')
	{
		strcpy(cTmpList,"往账已止付");
	}
	else if(ch=='G')
	{
		strcpy(cTmpList,"往账异常挂帐");
	}
	else if(ch=='H')
	{
		strcpy(cTmpList,"临时接收,不记帐");
	}
	else if(ch=='I')
	{
		strcpy(cTmpList,"正式接收");
	}
	else if(ch=='J')
	{
		strcpy(cTmpList,"全国押错");
	}
	else if(ch=='K')
	{
		strcpy(cTmpList,"地方押错");
	}
	else if(ch=='L')
	{
		strcpy(cTmpList,"全国押和地方押都错");
	}
	else if(ch=='M')
	{
		strcpy(cTmpList,"清算中心挂帐");
	}
	else if(ch=='N')
	{
		strcpy(cTmpList,"支行挂帐");
	}
	else if(ch=='O')
	{
		strcpy(cTmpList,"来账退回等待");
	}
	else if(ch=='P')
	{
		strcpy(cTmpList,"来账退回");
	}
	else if(ch=='Q')
	{
		strcpy(cTmpList,"同意未发送");
	}
	else if(ch=='R')
	{
		strcpy(cTmpList,"同意已发送");
	}
	else if(ch=='S')
	{
		strcpy(cTmpList,"不同意未发送");
	}
	else if(ch=='T')
	{
		strcpy(cTmpList,"不同意已发送");
	}
	else if(ch=='U')
	{
		strcpy(cTmpList,"来账已冲正");
	}
	else if(ch=='V')
	{
		strcpy(cTmpList,"来账已止付");
	}
	else if(ch=='W')
	{
		strcpy(cTmpList,"来账已付款");
	}
	else if(ch=='Z')
	{
		strcpy(cTmpList,"来账异常挂帐");
	}
	else
		strcpy(cTmpList,"未知状态");

	vtcp_log("[%s][%d]cTmpList==[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;

}

/***回执状态*****/
char * iNum2RcpStat(char *rcpstat , char *cTmpList,char *pkgno)
{
	vtcp_log("liuyue[%s][%d]rcpstat==[%s]\n",__FILE__,__LINE__,rcpstat);

	if(!memcmp(pkgno,PKGNO_PTJJ,3) ||!memcmp(pkgno,PKGNO_PTJJHZ,3) ||!memcmp(pkgno,PKGNO_DQJJ,3)||!memcmp(pkgno,PKGNO_DQJJHZ,3)||!memcmp(pkgno,PKGNO_SSJJ,3) ||!memcmp(pkgno,PKGNO_SSJJHZ,3))
	{
			if(!memcmp(rcpstat,"00",2))
			{
				strcpy(cTmpList,"成功");
			}	
			else if(!memcmp(rcpstat,"01",2))
			{
				strcpy(cTmpList,"帐号不符");
			}	
			else if(!memcmp(rcpstat,"02",2))
			{
				strcpy(cTmpList,"帐号户名不符");
			}	
			else if(!memcmp(rcpstat,"03",2))
			{
				strcpy(cTmpList,"账户余额不足支付");
			}	
			else if(!memcmp(rcpstat,"10",2))
			{
				strcpy(cTmpList,"账户密码错误");
			}	
			else if(!memcmp(rcpstat,"11",2))
			{
				strcpy(cTmpList,"帐号状态错误");
			}	
			else if(!memcmp(rcpstat,"20",2))
			{
				strcpy(cTmpList,"业务已撤销");
			}	
			else if(!memcmp(rcpstat,"90",2))
			{
				strcpy(cTmpList,"其他错误");
			}
			else 
				strcpy(cTmpList,"未被回执");	
	}
	else
				strcpy(cTmpList,"       ");

	vtcp_log("[%s][%d]cTmpList===[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;
}


int iInSameBrno(char *or_br_no,char *qs_brno)
{
    char cOr_br_no[13];
    char cQs_brno[13];
    struct hv_uniontab_c hv_uniontab;
    int ret = 0;

    memset(cOr_br_no,0x00,sizeof(cOr_br_no));
    memset(cQs_brno,0x00,   sizeof(cQs_brno));
    memset(&hv_uniontab,0x00,   sizeof(hv_uniontab));


    memcpy(cOr_br_no,or_br_no,sizeof(cOr_br_no)-1);
    memcpy(cQs_brno, qs_brno,sizeof(cQs_brno)-1);


    ret = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no ='%.12s' ",cOr_br_no);
    if(ret)
    {
        vtcp_log("[%s][%d]查找hvuniontabl错误orbrno==[%s]\n",__FILE__,__LINE__,cOr_br_no);
        strcpy(g_pub_tx.reply,"P000");
        return (-1);
    }

    vtcp_log("[%s][%d]qsbrno:table==[%s] other==[%s]\n",__FILE__,__LINE__,hv_uniontab.qs_no,cOr_br_no);

    if(memcmp(cQs_brno,hv_uniontab.qs_no,sizeof(hv_uniontab.qs_no)-1))
    {
        return 0;
    }
    return 1;

}

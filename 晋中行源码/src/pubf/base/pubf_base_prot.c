#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_prot.c
* 功能描述：	理财协议登记主控函数
* 入口参数：
*				prot_type[4]	理财协议编号
*				mode			1-登记；2-取消；3-公积金取款
* 作    者:     xxx
* 完成日期：    2003年1月18日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#define EXTERN
#include "public.h"
#include "prdt_prot_parm_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "mo_infrm_c.h"

	struct  prdt_prot_parm_c prdt_prot_parm_c;
	char	main_prdt_no[4];

pub_base_prot(char prot_type[4],int mode)
{
	struct mo_prdt_prot_c	mo_prdt_prot_c;
	struct mo_prot_ac_rel_c	mo_prot_ac_rel_c;
	struct prdt_ac_id_c	prdt_ac_id_c;
	char   main_ac_no[20];
	double titl_amt,tmp_amt1,tmp_amt2,tmp_amt3;
	double tmp_amt=0.00;
	long   main_ac_id=0;
	int	   main_ac_seqn=0;

	MEMSET_DEBUG(&prdt_prot_parm_c,0x00,sizeof(struct prdt_prot_parm_c));
	MEMSET_DEBUG(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	MEMSET_DEBUG(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));
	MEMSET_DEBUG(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c));
	MEMSET_DEBUG(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	g_reply_int=0;
	titl_amt=0.00;

	if( mode!=1 && mode!=2 && mode!=3 && mode!=4 )
	{
		sprintf(acErrMsg,"操作方式错误![%d]",mode);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P179");
		goto ErrExit;
	}

	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"账号不存在![%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"取介质账号对照错误![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P147");
		goto ErrExit;
	}
	main_ac_id=g_mdm_ac_rel.ac_id;
	main_ac_seqn=g_pub_tx.ac_seqn;

	vtcp_log("xxx -- look prot_type=[%s],ac_no=[%s]",prot_type,g_pub_tx.ac_no);
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	if( strlen(prot_type)==0 )
	{
		if( pub_base_get_prot(g_pub_tx.ac_id,prot_type) )
		{
			sprintf(acErrMsg,"根据帐号取理财协议类型错误![%s]",g_pub_tx.ac_no);
			WRITEMSG
			goto ErrExit;
		}
	}

	vtcp_log("xxx -- look prot_type=[%s],ac_no=[%s]",prot_type,g_pub_tx.ac_no);
	if( strcmp(prot_type,"XD1") && strncmp(prot_type,"TZ",2) )
	{
		WRITEMSG
		g_reply_int=pub_base_check_draw("001");
		if( g_reply_int )
		{
			sprintf(acErrMsg,"校验支取方式错误![%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
	}
	strcpy(g_pub_tx.id_type,g_mdm_ac_rel.id_type);
	strcpy(g_pub_tx.id_no,g_mdm_ac_rel.id_no);

	if( mode==1 )
	{
		g_reply_int=Prdt_prot_parm_Sel(g_pub_tx.reply,&prdt_prot_parm_c,"prot_type='%s' and deal_seq=1 ",prot_type);
		if( g_reply_int==100 )
		{
			sprintf(acErrMsg,"理财协议编号不存在，请于中心机房联系![%s]",prot_type);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P175");
			goto ErrExit;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"取理财协议定义异常，请于中心机房联系![%s][%d]",prot_type,g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P176");
			goto ErrExit;
		}

		g_reply_int=sql_count("mo_prdt_prot","main_ac_id=%ld and sts='1' ",g_pub_tx.ac_id,g_pub_tx.ac_id);
		if( g_reply_int!=0 )
		{
			sprintf(acErrMsg,"该户已经作过理财类登记![%d]",g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P178");
			goto ErrExit;
		}

		/*** 检查结算账户是否合法 ***/
		g_reply_int=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"账号不对![%ld][%d][%s]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,prdt_prot_parm_c.main_prdt_code);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P183");
			goto ErrExit;
		}

		if( pub_chk_prdt_code(prdt_ac_id_c) )
		{
			sprintf(acErrMsg,"检查产品类型出错!");
			WRITEMSG
			goto ErrExit;
		}

		if( pub_chk_ac(prdt_ac_id_c) )
		{
			sprintf(acErrMsg,"检查结算账户状态出错!");
			WRITEMSG
			goto ErrExit;
		}

		tmp_amt=g_pub_tx.tx_amt1;
		g_pub_tx.tx_amt1=0.00;
		strcpy(g_pub_tx.brf,"理财登记");
		if( pub_ins_trace_log() )
		{
			sprintf(acErrMsg,"登记交易流水出错!");
			WRITEMSG
			goto ErrExit;
		}
		g_pub_tx.tx_amt1=tmp_amt;

		if( pub_ins_prot() )
		{
			sprintf(acErrMsg,"登记理财协议出错!");
			WRITEMSG
			goto ErrExit;
		}
	}
	else if( mode==4 )
	{
		g_reply_int=sql_count("mo_prdt_prot",
			"main_ac_id=%ld and main_ac_seqn=%ld and sts[1]=='1' ",
			g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( g_reply_int!=1 )
		{
			sprintf(acErrMsg,"理财协议登记薄错误![%d][%ld][%d]",
				g_reply_int,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P200");
			goto ErrExit;
		}

		g_reply_int=Mo_prot_ac_rel_Dec_Upd(g_pub_tx.reply,
			"main_ac_id=%ld and main_ac_seqn=%d and sts[1]!='*' ",
			g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取理财类账户对照表异常![%s][%d]",
				prot_type,g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P181");
			goto ErrExit;
		}

		while(1)
		{
			g_reply_int=Mo_prot_ac_rel_Fet_Upd(&mo_prot_ac_rel_c,
				g_pub_tx.reply);
			if( g_reply_int==100 )	break;
			else if( g_reply_int )
			{
				sprintf(acErrMsg,"取理财类账户对照表异常![%d]",g_reply_int);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P181");
				goto ErrExit;
			}
			vtcp_log("ac_id=[%ld],seqn=[%d]",
				mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn);
			mo_prot_ac_rel_c.sts[0]='*';

			g_reply_int=Mo_prot_ac_rel_Upd_Upd(mo_prot_ac_rel_c,g_pub_tx.reply);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"取理财类登记簿异常![%d]",g_reply_int);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P181");
				goto ErrExit;
			}
		}
		Mo_prot_ac_rel_Clo_Upd();

			g_reply_int=sql_execute(
				"update mo_prdt_prot set sts='*' where main_ac_id=%ld and main_ac_seqn=%d",
				main_ac_id,main_ac_seqn);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"修改理财协议登记薄错误!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P200");
				goto ErrExit;
   			}
	}
	else 
	{
		g_reply_int=sql_count("mo_prdt_prot","main_ac_id=%ld and main_ac_seqn=%ld and substr(sts,1,1)='1' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( g_reply_int!=1 )
		{
			sprintf(acErrMsg,"理财协议登记薄错误![%d][%ld][%d]",g_reply_int,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P200");
			goto ErrExit;
		}

		g_reply_int=Mo_prot_ac_rel_Dec_Upd(g_pub_tx.reply,"main_ac_id=%ld and main_ac_seqn=%d and sts[1]!='*' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取理财类账户对照表异常，请与中心机房联系![%s][%d]",prot_type,g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P181");
			goto ErrExit;
		}

		tmp_amt1=0.00;
		tmp_amt2=0.00;
		tmp_amt3=0.00;
		while(1)
		{
		g_reply_int=Mo_prot_ac_rel_Fet_Upd(&mo_prot_ac_rel_c,g_pub_tx.reply);
			if( g_reply_int==100 )	break;
			else if( g_reply_int )
			{
				sprintf(acErrMsg,"取理财类账户对照表异常，请于中心机房联系![%d]",g_reply_int);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P181");
				goto ErrExit;
			}
			vtcp_log("ac_id=[%ld],seqn=[%d]",mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn);
			if( strncmp(prot_type,"YY",2) )
			{
				g_pub_tx.ac_id=mo_prot_ac_rel_c.app_ac_id;
				strcpy(g_pub_tx.ac_no,"");
				strcpy(g_pub_tx.crd_no,"");
				g_pub_tx.ac_seqn=mo_prot_ac_rel_c.app_ac_seqn;
				strcpy(g_pub_tx.add_ind,"0");
				g_pub_tx.svc_ind=1004;
				strcpy(g_pub_tx.brf,"协定理财取消");
				strcpy(g_pub_tx.ac_wrk_ind,"000000000");
vtcp_log("ac_id=[%ld],seqn=[%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
				g_pub_tx.hst_ind[0]='1';/* 需要记明细added by liuxuan 20071009 */
				g_reply_int=pub_acct_trance();
				if( g_reply_int )
				{
					sprintf(acErrMsg,"调用存取款主控失败!");
					WRITEMSG
					goto ErrExit;
   				}
   			}
			titl_amt=titl_amt+g_pub_tx.tx_amt1;
			tmp_amt1=tmp_amt1+g_pub_intst.dealval;
			tmp_amt2=tmp_amt2+g_pub_intst.keepval;
			tmp_amt3=tmp_amt3+g_pub_intst.val;
			vtcp_log("HAO -- LOOK dealval=[%lf][%lf] keepval=[%lf][%lf] val=[%lf][%lf]",g_pub_intst.dealval,tmp_amt1,g_pub_intst.keepval,tmp_amt2,g_pub_intst.val,tmp_amt3);
			if( !strncmp(prot_type,"YY",2) )
			{
				titl_amt=0.00;
				tmp_amt1=0.00;
				tmp_amt2=0.00;
				tmp_amt3=0.00;
			}

			strcpy(mo_prot_ac_rel_c.sts,"*");
			g_reply_int=Mo_prot_ac_rel_Upd_Upd(mo_prot_ac_rel_c,g_pub_tx.reply);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"取理财类登记簿异常，请于中心机房联系![%d]",g_reply_int);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P181");
				goto ErrExit;
			}
		}
		Mo_prot_ac_rel_Clo_Upd();

		g_pub_intst.dealval=tmp_amt1;
		g_pub_intst.keepval=tmp_amt2;
		g_pub_intst.val=tmp_amt3;
		g_pub_tx.tmp_amt1=titl_amt;
		vtcp_log("HAO -- LOOK dealval=[%lf][%lf] keepval=[%lf][%lf] val=[%lf][%lf]",g_pub_intst.dealval,tmp_amt1,g_pub_intst.keepval,tmp_amt2,g_pub_intst.val,tmp_amt3);
		if( pub_base_CompDblVal(titl_amt+tmp_amt1+tmp_amt2-tmp_amt3,0.00)==0 )
		{
			sprintf(acErrMsg,"没有能够化回的理财户![%ld][%d]",main_ac_id,main_ac_seqn);
			WRITEMSG
			if( strcmp(mo_prot_ac_rel_c.prot_type,"Z02") || mode!=3 )
			{
				g_reply_int=sql_execute("update mo_prdt_prot set sts='*' where main_ac_id=%ld and main_ac_seqn=%d",main_ac_id,main_ac_seqn);
				if( g_reply_int )
				{
					sprintf(acErrMsg,"修改理财协议登记薄错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P200");
					goto ErrExit;
   				}
   			}
			return 0;
		}
		set_zd_data("102K",g_pub_tx.ct_ind);
		set_zd_double("102I",titl_amt);
		set_zd_double("102P",g_pub_intst.dealval+g_pub_intst.keepval);  /*实付利息*/
		set_zd_double("102Q",g_pub_intst.val);              /*利 息 税*/
		g_reply_int=pubf_acct_proc("LC01");
		if( g_reply_int )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			WRITEMSG
			goto ErrExit;
   		}

		if( strcmp(mo_prot_ac_rel_c.prot_type,"Z02") )
		{
			strcpy(g_pub_tx.ac_no,"");
			strcpy(g_pub_tx.ac_id_type,"");
			g_pub_tx.ac_id=mo_prot_ac_rel_c.main_ac_id;
			g_pub_tx.ac_seqn=mo_prot_ac_rel_c.main_ac_seqn;
			g_pub_tx.tx_amt1=titl_amt+tmp_amt1+tmp_amt2-tmp_amt3;
			strcpy(g_pub_tx.add_ind,"1");
			g_pub_tx.svc_ind=1001;
			strcpy(g_pub_tx.brf,"协定转回");
			strcpy(g_pub_tx.ac_wrk_ind,"000100000");
			g_reply_int=pub_acct_trance();
			if( g_reply_int )
			{
				sprintf(acErrMsg,"调用存取款主控失败!");
				WRITEMSG
				goto ErrExit;
   			}
   				/***修改为使用100域第2子域 modify at 20130517
	set_zd_double("102I",g_pub_tx.tx_amt1);
	****/
			set_zd_double("1002",g_pub_tx.tx_amt1);
			g_reply_int=pubf_acct_proc("LC02");
			if( g_reply_int )
			{
				sprintf(acErrMsg,"登记会计流水错误!");
				WRITEMSG
				goto ErrExit;
   			}
   		}

		if( strcmp(mo_prot_ac_rel_c.prot_type,"Z02") || mode!=3 )
		{
			g_reply_int=sql_execute("update mo_prdt_prot set sts='*' where main_ac_id=%ld and main_ac_seqn=%d",main_ac_id,main_ac_seqn);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"修改理财协议登记薄错误!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P200");
				goto ErrExit;
   			}
   		}
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"理财协议登记主控成功![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"理财协议登记主控失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

int pub_chk_prdt_code(struct prdt_ac_id_c prdt_ac_id_c)
{
	char *k;
	char str[4];

	pub_base_strpack(prdt_prot_parm_c.main_prdt_code);
	pub_base_strpack(prdt_ac_id_c.prdt_no);
	strcpy(main_prdt_no,prdt_ac_id_c.prdt_no);
	strcpy(str,prdt_prot_parm_c.main_prdt_code);

	k=strchr(str,'*');
	if( k!=NULL )
	{
		*k='\0';
		/**** 按财务部要求增加对协定存款的判断 136，146产品也可以做 add by wudw 20130711***/
		if(strcmp(prdt_prot_parm_c.prot_type,"XD1")==0)
		{
			if( strncmp(str,prdt_ac_id_c.prdt_no,strlen(str))!=0 && strcmp(prdt_ac_id_c.prdt_no,"136")!=0 && strcmp(prdt_ac_id_c.prdt_no,"146")!=0)
			{
				sprintf(acErrMsg,"该产品的账号不能购买此项服务![%s],[%s]",str,prdt_ac_id_c.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P284");
				return 1;
			}
		}
		else if( strncmp(str,prdt_ac_id_c.prdt_no,strlen(str))!=0 )
		{
			sprintf(acErrMsg,"该产品的账号不能购买此项服务![%s],[%s]",str,prdt_ac_id_c.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P284");
			return 1;
		}
	}
	else if( strcmp(prdt_prot_parm_c.main_prdt_code,prdt_ac_id_c.prdt_no) )
	{
		sprintf(acErrMsg,"该产品的账号不能购买此项服务![%s],[%s]",prdt_prot_parm_c.main_prdt_code,prdt_ac_id_c.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P284");
		return 1;
	}

	return 0;
ErrExit:
	return 1;
}

int pub_chk_ac(struct prdt_ac_id_c prdt_ac_id_c)
{
	int	ret=0;

	if( prdt_ac_id_c.prdt_no[0]=='1' )
	{
		ret=pub_de_chk_dd_mst("0111");
		if( ret )
		{
			sprintf(acErrMsg,"活期结算户检查错误![%d]",ret);
			WRITEMSG
			return 1;
		}
		strcpy(g_pub_tx.name,g_dd_mst.name);
	}
	else if( prdt_ac_id_c.prdt_no[0]=='2' )
	{
		ret=pub_de_chk_td_mst("0111");
		if( ret )
		{
			sprintf(acErrMsg,"定期结算户检查错误![%d]",ret);
			WRITEMSG
			return 1;
		}
		strcpy(g_pub_tx.name,g_td_mst.name);
	}

	return 0;
ErrExit:
	return 1;
}

int pub_ins_prot()
{
	void 	* handle;
	char	libso[50];
	void	(* myFun)();

	sprintf( libso,"%s/lib/",getenv("HOME") );
	pub_base_strpack ( libso );
	strcat ( libso , "libbase.so");
	libso[strlen(libso)]='\0';

	handle = dlopen(libso, RTLD_LAZY);
	if(handle == NULL)
	{
		vtcp_log("dlerror = [%d] [%s]",errno,dlerror());
		strcpy(g_pub_tx.reply,"S027");
		vtcp_log(">>>>>>>打开错误！");
		goto ErrExit ;
	}

	pub_base_strpack(prdt_prot_parm_c.reg_fun);
	myFun = (void(*)(int))dlsym(handle,prdt_prot_parm_c.reg_fun);
	if( myFun == NULL )
	{
		vtcp_log("dlerror = [%d] [%s]",errno,dlerror());
		strcpy(g_pub_tx.reply,"S027");
		vtcp_log("调用错误！");
		dlerror();
		goto ErrExit ;
	}
	else (*myFun)();

	if( dlclose(handle) )
	{
		strcpy(g_pub_tx.reply,"S027");
		vtcp_log("关闭错误！");
		goto ErrExit ;
	}
	
	if( strcmp(g_pub_tx.reply,"0000") )
	{
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"理财协议登记成功![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"理财协议登记失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

int pub_InsXD1()
{
	struct mo_prdt_prot_c	mo_prdt_prot_c;
	struct mo_prot_ac_rel_c	mo_prot_ac_rel_c;
	MEMSET_DEBUG(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));
	MEMSET_DEBUG(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c));

	strcpy(mo_prdt_prot_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prdt_prot_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prdt_prot_c.main_ac_id=g_pub_tx.ac_id;
	mo_prdt_prot_c.main_ac_seqn=g_pub_tx.ac_seqn;
	mo_prdt_prot_c.leave_amt=g_pub_tx.tx_amt1;
	if(pub_base_CompDblVal(mo_prdt_prot_c.leave_amt,prdt_prot_parm_c.min_bal)<0)
	{
		sprintf(acErrMsg,"留存金额小于规定最小金额![%lf][%lf]",mo_prdt_prot_c.leave_amt,prdt_prot_parm_c.min_bal);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P202");
		return 1;
	}
	mo_prdt_prot_c.turn_amt=0.00;
	mo_prdt_prot_c.tx_date=g_pub_tx.tx_date;
	mo_prdt_prot_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prdt_prot_c.tel,g_pub_tx.tel);
	strcpy(mo_prdt_prot_c.sts,"1");
	mo_prdt_prot_c.m3=0;
	mo_prdt_prot_c.m6=0;
	mo_prdt_prot_c.y1=0;
	mo_prdt_prot_c.y2=0;

	g_reply_int=Mo_prdt_prot_Ins(mo_prdt_prot_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P177");
		return 1;
	}

	strcpy(mo_prot_ac_rel_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prot_ac_rel_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prot_ac_rel_c.main_ac_id=g_pub_tx.ac_id;
	mo_prot_ac_rel_c.main_ac_seqn=g_pub_tx.ac_seqn;
	mo_prot_ac_rel_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prot_ac_rel_c.tel,g_pub_tx.tel);
	strcpy(mo_prot_ac_rel_c.sts,"1");

	/*** 开从产品介质 ***/
	strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
	strcpy(g_pub_tx.crd_no,"");
	strcpy(g_pub_tx.ac_no,"");
	strcpy(g_pub_tx.add_ind,"1");
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	g_pub_tx.tx_amt1=0.00;
	g_reply_int=pub_acct_opn_cif_mdm();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"开介质错误!");
		WRITEMSG
		return 1;
	}
	mo_prot_ac_rel_c.app_ac_id=g_pub_tx.ac_id;

	/*** 开从产品账户 ***/
	/*strcpy(g_pub_tx.prdt_code,prdt_prot_parm_c.app_prdt_code);*/
	/* 按财务要求 产品类型入153单位协定存款 wudawei 20131009*/
	strcpy(g_pub_tx.prdt_code,"153");
	strcpy(g_pub_tx.cur_no,RMB);
	strcpy(g_pub_tx.ac_type,"6");
	/*** 按财务部要求 此处写死按季结息4 以前是利随本清 1***/
	strcpy(g_pub_tx.intst_type,"4");
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.brf,"协定开户");

	g_reply_int=pub_acct_opn_ac();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"开账户错误![%d]",g_reply_int);
		WRITEMSG
		return 1;
	}
	mo_prot_ac_rel_c.app_ac_seqn=g_pub_tx.ac_seqn;
	set_zd_data("0310",g_pub_tx.ac_no);
	set_zd_int("0350",g_pub_tx.ac_seqn);

	g_reply_int=Mo_prot_ac_rel_Ins(mo_prot_ac_rel_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议账户关系表错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P201");
		return 1;
	}

	return 0;
}

int pub_InsTC()
{
	struct mo_prdt_prot_c	mo_prdt_prot_c;
	MEMSET_DEBUG(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));

	if( strncmp(g_pub_tx.ac_no,CRD_HEAD,9) )
	{
		sprintf(acErrMsg,"非卡号不能登记理财协议[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P294");
		return 1;
	}
	strcpy(mo_prdt_prot_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prdt_prot_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prdt_prot_c.main_ac_id=g_pub_tx.ac_id;
	mo_prdt_prot_c.main_ac_seqn=g_pub_tx.ac_seqn;
	mo_prdt_prot_c.leave_amt=g_pub_tx.tx_amt1;
	mo_prdt_prot_c.turn_amt=0.00;
	if(pub_base_CompDblVal(mo_prdt_prot_c.leave_amt,prdt_prot_parm_c.min_bal)<0)
	{
		sprintf(acErrMsg,"留存金额小于规定最小金额![%lf][%lf]",mo_prdt_prot_c.leave_amt,prdt_prot_parm_c.min_bal);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P202");
		return 1;
	}
	mo_prdt_prot_c.tx_date=g_pub_tx.tx_date;
	mo_prdt_prot_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prdt_prot_c.tel,g_pub_tx.tel);
	strcpy(mo_prdt_prot_c.sts,"1");
	mo_prdt_prot_c.m3=0;
	mo_prdt_prot_c.m6=0;
	mo_prdt_prot_c.y1=0;
	mo_prdt_prot_c.y2=0;
	pub_base_strpack(prdt_prot_parm_c.prot_type);
	if( !strcmp(prdt_prot_parm_c.prot_type,"TCA") )
		get_zd_long("0480",&mo_prdt_prot_c.m3);
	else if( !strcmp(prdt_prot_parm_c.prot_type,"TCB") )
		get_zd_long("0490",&mo_prdt_prot_c.m6);
	else if( !strcmp(prdt_prot_parm_c.prot_type,"TCC") )
	{
		get_zd_long("0480",&mo_prdt_prot_c.m3);
		get_zd_long("0500",&mo_prdt_prot_c.y1);
	}
	else if( !strcmp(prdt_prot_parm_c.prot_type,"TCD") )
	{
		get_zd_long("0480",&mo_prdt_prot_c.m3);
		get_zd_long("0490",&mo_prdt_prot_c.m6);
		get_zd_long("0500",&mo_prdt_prot_c.y1);
	}
	else if( !strcmp(prdt_prot_parm_c.prot_type,"TCE") )
	{
		get_zd_long("0480",&mo_prdt_prot_c.m3);
		get_zd_long("0490",&mo_prdt_prot_c.m6);
		get_zd_long("0500",&mo_prdt_prot_c.y1);
		get_zd_long("0510",&mo_prdt_prot_c.y2);
	}
if(mo_prdt_prot_c.m3+mo_prdt_prot_c.m6+mo_prdt_prot_c.y1+mo_prdt_prot_c.y2!=100)
	{
		sprintf(acErrMsg,"登记理财比例错误!");
		WRITEMSG
		vtcp_log("m3=[%ld]",mo_prdt_prot_c.m3);
		vtcp_log("m6=[%ld]",mo_prdt_prot_c.m6);
		vtcp_log("y1=[%ld]",mo_prdt_prot_c.y1);
		vtcp_log("y2=[%ld]",mo_prdt_prot_c.y2);
		strcpy(g_pub_tx.reply,"P206");
		return 1;
	}

	g_reply_int=Mo_prdt_prot_Ins(mo_prdt_prot_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P177");
		return 1;
	}

	return 0;
}

int pub_InsYY1()
{
	struct mo_prdt_prot_c	mo_prdt_prot_c;
	struct mo_prot_ac_rel_c	mo_prot_ac_rel_c;
	struct prdt_ac_id_c		prdt_ac_id_c;
	MEMSET_DEBUG(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));
	MEMSET_DEBUG(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	MEMSET_DEBUG(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c));

	strcpy(mo_prdt_prot_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prdt_prot_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prdt_prot_c.main_ac_id=g_pub_tx.ac_id;
	mo_prdt_prot_c.main_ac_seqn=g_pub_tx.ac_seqn;
	mo_prdt_prot_c.leave_amt=g_pub_tx.tx_amt1;
	if(pub_base_CompDblVal(mo_prdt_prot_c.leave_amt,prdt_prot_parm_c.min_bal)<0)
	{
		sprintf(acErrMsg,"留存金额小于规定最小金额![%lf][%lf]",mo_prdt_prot_c.leave_amt,prdt_prot_parm_c.min_bal);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P202");
		return 1;
	}
	mo_prdt_prot_c.tx_date=g_pub_tx.tx_date;
	mo_prdt_prot_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prdt_prot_c.tel,g_pub_tx.tel);
	strcpy(mo_prdt_prot_c.sts,"1");
	mo_prdt_prot_c.m3=0;
	mo_prdt_prot_c.m6=0;
	mo_prdt_prot_c.y1=0;
	mo_prdt_prot_c.y2=0;

	mo_prot_ac_rel_c.main_ac_id=g_pub_tx.ac_id;
	mo_prot_ac_rel_c.main_ac_seqn=g_pub_tx.ac_seqn;
	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_int("0340",&g_pub_tx.ac_seqn);

	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"账号不存在![%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		return 1;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"取介质账号对照错误![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P147");
		return 1;
	}
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	mo_prdt_prot_c.turn_amt=0.00;
	/**
	if( !strcmp(prdt_prot_parm_c.prot_type,"YY1") || !strcmp(prdt_prot_parm_c.prot_type,"YY5") )
	**/
	{
		g_reply_int=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"账号不对![%ld][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P183");
			return 1;
		}
		mo_prdt_prot_c.turn_amt=g_td_mst.td_amt;

		if( strcmp(g_td_mst.prdt_no,"209") 
			&& strcmp(g_td_mst.prdt_no,"20A")
			&& strcmp(g_td_mst.prdt_no,"20B") )
		{
			sprintf(acErrMsg,"对方账户产品不对![%s]",g_td_mst.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P203");
			return 1;
		}

		if( pub_de_chk_td_mst("1011") )
		{
			sprintf(acErrMsg,"定期结算户检查错误!");
			WRITEMSG
			return 1;
		}
	}

	g_reply_int=Mo_prdt_prot_Ins(mo_prdt_prot_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P177");
		return 1;
	}

	mo_prot_ac_rel_c.app_ac_id=g_pub_tx.ac_id;
	mo_prot_ac_rel_c.app_ac_seqn=g_pub_tx.ac_seqn;
	strcpy(mo_prot_ac_rel_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prot_ac_rel_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prot_ac_rel_c.tx_date=g_pub_tx.tx_date;
	mo_prot_ac_rel_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prot_ac_rel_c.tel,g_pub_tx.tel);
	strcpy(mo_prot_ac_rel_c.sts,"1");

	g_reply_int=Mo_prot_ac_rel_Ins(mo_prot_ac_rel_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P177");
		return 1;
	}

	return 0;
}

int pub_InsYY2()
{
	struct mo_prdt_prot_c	mo_prdt_prot_c;
	MEMSET_DEBUG(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));

	strcpy(mo_prdt_prot_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prdt_prot_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prdt_prot_c.main_ac_id=g_pub_tx.ac_id;
	mo_prdt_prot_c.main_ac_seqn=g_pub_tx.ac_seqn;
	mo_prdt_prot_c.leave_amt=g_pub_tx.tx_amt1;
	mo_prdt_prot_c.turn_amt=0.00;
	mo_prdt_prot_c.tx_date=g_pub_tx.tx_date;
	mo_prdt_prot_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prdt_prot_c.tel,g_pub_tx.tel);
	strcpy(mo_prdt_prot_c.sts,"1");
	mo_prdt_prot_c.m3=0;
	mo_prdt_prot_c.m6=0;
	mo_prdt_prot_c.y1=0;
	mo_prdt_prot_c.y2=0;

	g_reply_int=Mo_prdt_prot_Ins(mo_prdt_prot_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P177");
		return 1;
	}

	return 0;
}
int pub_InsGJJ()
{
	struct mo_prdt_prot_c	mo_prdt_prot_c;
	MEMSET_DEBUG(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));

	strcpy(mo_prdt_prot_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prdt_prot_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prdt_prot_c.main_ac_id=g_pub_tx.ac_id;
	mo_prdt_prot_c.main_ac_seqn=g_pub_tx.ac_seqn;
	mo_prdt_prot_c.leave_amt=0.00;
	mo_prdt_prot_c.turn_amt=0.00;
	mo_prdt_prot_c.tx_date=g_pub_tx.tx_date;
	mo_prdt_prot_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prdt_prot_c.tel,g_pub_tx.tel);
	strcpy(mo_prdt_prot_c.sts,"1");
	mo_prdt_prot_c.m3=0;
	mo_prdt_prot_c.m6=0;
	mo_prdt_prot_c.y1=100;
	mo_prdt_prot_c.y2=0;

	g_reply_int=Mo_prdt_prot_Ins(mo_prdt_prot_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P177");
		return 1;
	}

	return 0;
}
/***通知套餐A的登记控制***/
int pub_InsTZA()
{
	struct mo_infrm_c	mo_infrm_c;
	struct mo_prdt_prot_c	mo_prdt_prot_c;
	int ret=0;
	MEMSET_DEBUG(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));

	ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and ac_seqn=%d", 
		g_pub_tx.ac_id, g_pub_tx.ac_seqn);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询定期主文件错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'",
		g_td_mst.prdt_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询定期产品表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	if( g_td_parm.td_type[0]=='4' )
	{
		ret=Mo_infrm_Sel( g_pub_tx.reply,&mo_infrm_c,
			"ac_id=%d and ac_seqn=%d and sts='0'" ,
			g_pub_tx.ac_id,g_pub_tx.ac_seqn );
		if( ret==100 )
			;
		else if( ret ) goto ErrExit;
		else
		{
			TRACE
			strcpy( g_pub_tx.reply,"D203" );
			goto ErrExit;
		}
	}
	else
	{
			TRACE
			strcpy( g_pub_tx.reply,"P609" );
			goto ErrExit;
	}
vtcp_log("TZA:acid[%d]",g_pub_tx.ac_id);
	g_reply_int=Mo_prdt_prot_Sel(g_pub_tx.reply,&mo_prdt_prot_c,
		"main_ac_id=%d and main_ac_seqn=%d and sts='1'",
		g_pub_tx.ac_id,g_pub_tx.ac_seqn );
	if( g_reply_int==100 )
		;
	else if( g_reply_int==0 )
	{
			TRACE
			strcpy( g_pub_tx.reply,"P178" );
			goto ErrExit;
	}
	else
	 goto ErrExit;

	strcpy(mo_prdt_prot_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prdt_prot_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prdt_prot_c.main_ac_id=g_pub_tx.ac_id;
	mo_prdt_prot_c.main_ac_seqn=g_pub_tx.ac_seqn;
	mo_prdt_prot_c.leave_amt=0.00;
	mo_prdt_prot_c.turn_amt=0.00;
	mo_prdt_prot_c.tx_date=g_pub_tx.tx_date;
	mo_prdt_prot_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prdt_prot_c.tel,g_pub_tx.tel);
	strcpy(mo_prdt_prot_c.sts,"1");
	mo_prdt_prot_c.m3=0;
	mo_prdt_prot_c.m6=0;
	mo_prdt_prot_c.y1=100;
	mo_prdt_prot_c.y2=0;

	g_reply_int=Mo_prdt_prot_Ins(mo_prdt_prot_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P177");
		goto ErrExit;
	}

	/**
	if( g_td_parm.td_type[0]=='4' )
	{
		MEMSET_DEBUG( &mo_infrm_c,0,sizeof(mo_infrm_c) );
		mo_infrm_c.ac_id=g_td_mst.ac_id;
		mo_infrm_c.ac_seqn=g_td_mst.ac_seqn;
		mo_infrm_c.infrm_date=g_pub_tx.tx_date;
		pub_base_deadlineD(mo_infrm_c.infrm_date,
			g_td_parm.term,&mo_infrm_c.prm_draw_date);
		mo_infrm_c.prm_draw_amt=g_td_mst.bal;
		mo_infrm_c.sts[0]='0';
		strcpy( mo_infrm_c.name,"通知套餐 A自动预约" );
		mo_infrm_c.wrk_date=g_pub_tx.tx_date;
		mo_infrm_c.trace_no=g_pub_tx.trace_no;
		ret=Mo_infrm_Ins( mo_infrm_c,g_pub_tx.reply );
		if( ret )
		{
			TRACE
			goto ErrExit;
		}
	}
	**************/

	strcpy( g_pub_tx.reply,"0000" );
	TRACE
	return 0;
ErrExit:
	TRACE
	return 1;
}
/***通知套餐B的登记控制***/
int pub_InsTZB()
{
	struct mo_infrm_c	mo_infrm_c;
	struct mo_prdt_prot_c	mo_prdt_prot_c;
	struct mo_prot_ac_rel_c	mo_prot_ac_rel_c;
	int ret=0;
	MEMSET_DEBUG(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));

	ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%d", 
		g_pub_tx.ac_id, g_pub_tx.ac_seqn);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询定期主文件错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'",
		g_dd_mst.prdt_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询定期产品表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	if( !strcmp(g_dd_parm.prdt_no,"101") )
	{
		;
	}
	else
	{
			vtcp_log("PPPP[%s]",g_dd_parm.prdt_no);
			TRACE
			strcpy( g_pub_tx.reply,"P609" );
			goto ErrExit;
	}
vtcp_log("TZA:acid[%d]",g_pub_tx.ac_id);
	g_reply_int=Mo_prdt_prot_Sel(g_pub_tx.reply,&mo_prdt_prot_c,
		"main_ac_id=%d and main_ac_seqn=%d and sts='1'",
		g_pub_tx.ac_id,g_pub_tx.ac_seqn );
	if( g_reply_int==100 )
		;
	else if( g_reply_int==0 )
	{
			TRACE
			strcpy( g_pub_tx.reply,"P178" );
			goto ErrExit;
	}
	else
	 goto ErrExit;

	strcpy(mo_prdt_prot_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prdt_prot_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prdt_prot_c.main_ac_id=g_pub_tx.ac_id;
	mo_prdt_prot_c.main_ac_seqn=g_pub_tx.ac_seqn;
	mo_prdt_prot_c.leave_amt=0.00;
	mo_prdt_prot_c.turn_amt=0.00;
	mo_prdt_prot_c.tx_date=g_pub_tx.tx_date;
	mo_prdt_prot_c.trace_no=g_pub_tx.trace_no;
	strcpy(mo_prdt_prot_c.tel,g_pub_tx.tel);
	strcpy(mo_prdt_prot_c.sts,"1");
	mo_prdt_prot_c.m3=0;
	mo_prdt_prot_c.m6=0;
	mo_prdt_prot_c.y1=100;
	mo_prdt_prot_c.y2=0;

	g_reply_int=Mo_prdt_prot_Ins(mo_prdt_prot_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P177");
		goto ErrExit;
	}

	MEMSET_DEBUG( &mo_prot_ac_rel_c,0,sizeof(mo_prot_ac_rel_c));
	strcpy(mo_prot_ac_rel_c.tx_br_no,g_pub_tx.tx_br_no);	
	strcpy(mo_prot_ac_rel_c.prot_type,prdt_prot_parm_c.prot_type);	
	mo_prot_ac_rel_c.main_ac_id=g_pub_tx.ac_id;
	mo_prot_ac_rel_c.main_ac_seqn=g_pub_tx.ac_seqn;
	mo_prot_ac_rel_c.trace_no=g_pub_tx.trace_no;
	mo_prot_ac_rel_c.tx_date=g_pub_tx.tx_date;
	pub_base_deadlineD( g_pub_tx.tx_date,-1,&mo_prot_ac_rel_c.tx_date);
	strcpy(mo_prot_ac_rel_c.tel,g_pub_tx.tel);
	strcpy(mo_prot_ac_rel_c.sts,"1");
	mo_prot_ac_rel_c.app_ac_id=g_pub_tx.ac_id;
	mo_prot_ac_rel_c.app_ac_seqn=g_pub_tx.ac_seqn;
	g_reply_int=Mo_prot_ac_rel_Ins(mo_prot_ac_rel_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"登记理财协议账户关系表错误![%d]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P201");
		return 1;
	}

	strcpy( g_pub_tx.reply,"0000" );
	TRACE
	return 0;
ErrExit:
	TRACE
	return 1;
}

int pub_base_get_prot(long Ac_id,char *Prot_type)
{
	int	ret=0;
	struct mo_prdt_prot_c	sMo_prdt_prot;
	MEMSET_DEBUG(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));

	ret=Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot,"main_ac_id=%ld and sts='1' ",Ac_id);
	if( ret )
	{
		sprintf(acErrMsg,"登记理财协议错误![%d][%d]",Ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P180");
		return 1;
	}
	strcpy(Prot_type,sMo_prdt_prot.prot_type);
	return 0;
}

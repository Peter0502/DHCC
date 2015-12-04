/*************************************************************
* 文 件 名: rpt028.c
* 功能描述：银行贷款相关特种传票打印
*
* 作    者: dadary
* 完成日期: 2006年11月18日
*
* 注：格式文件为RPT028.tbl
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define TABLE_RECORD 1
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "trace_log_bk_c.h"
#include "com_branch_c.h"
#include "ln_mst_hst_c.h"
#include "com_sys_parm_c.h"
#include "ln_parm_c.h"
#include "com_item_c.h"
#include "ln_mst_c.h"
#include "dc_acc_rel_c.h"


struct trace_log_bk_c s_trace_log_bk;
struct ln_mst_hst_c s_ln_mst_hst;
static struct com_sys_parm_c sComSysParm;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

static char acc_hrt[8];
static char form_acc_hrt[8];

int get_ratio_mbfesd(int bli,int bll,char str[100]);
char *cGetHvCmnoDes(char *cmtno,char *desc);
char *cGetHvzfStat(char stat,char *desc);
void cDeal_str(char *str,int lenth);
char * cDeal_content(char *buf,char ch,char *str);
void  myfun(char *source,char *target);
char * cTurnMoneyUpper(char *money);
int pub_ln_get_form_acc_hrt( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *data_code , char *acc_hrt );
/***注意了:这里是打印一个个表单,每一页放2个表单*****/
rpt028()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	long old_trace_no=0;
	char acc_hrt_tmp[8];
	char form_acc_hrt_tmp[8];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_trace_log_bk , 0x00 , sizeof(struct trace_log_bk_c));
	memset(acc_hrt,0x00,8);
	memset(fname,0x00,sizeof(fname));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(tmp_str,0x00,sizeof(tmp_str));
	memset(old_br_no,0x00,sizeof(old_br_no));
	
	memset(acc_hrt_tmp,0,sizeof(acc_hrt_tmp));
	memcpy(acc_hrt,"0000000",7);
	memset(form_acc_hrt_tmp,0,sizeof(form_acc_hrt_tmp));
	memcpy(form_acc_hrt,"0000000",7);
	
	strcpy( old_br_no , "*****" );
		

	vtcp_log("[%s][%d]开始生成贷款逾期传票!\n",__FILE__,__LINE__);   
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
	ret  = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "1=1" );
	if( ret  ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", ret  ); 
		WRITEMSG 
		goto ErrExit; 
	}
	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	strcpy( filename , "RPT028");
	strcpy(vpfm, "RPT028");
	/* 条件:交易代码为"G088" */
	strcpy( tmpstr,"tx_code='G088'and substr(prdt_no,1,1)='3' and " );
	strcat( wherelist,tmpstr );	
	/**组成查询**/

	strcat(wherelist," no_show!='1' and 1=1  ORDER BY tx_br_no, prdt_no, trace_no, trace_cnt") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Trace_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	if(ret)
	{
		vtcp_log("[%s][%d]操作Trace_log_bk失败!\n",__FILE__,__LINE__);
		goto ErrExit;
		ttlnum = 0;
	}
	while(1)
	{
		ret=Trace_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		if( s_trace_log_bk.trace_no==old_trace_no )	/*流水号未改变*/
		{
			continue;
		}
		
		old_trace_no= s_trace_log_bk.trace_no;
		
		/*取贷款户信息*/
		ret = pub_ln_lndis_file(s_trace_log_bk.ac_no,s_trace_log_bk.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
		if( ret ) 
		{
			sprintf(acErrMsg,"读取贷款户信息error");
			WRITEMSG
			return ret;
		}
		
		/************20061217  如果 贷款户的余额为0 不出此传票*****/
		if(g_ln_mst.bal==0.0)
			continue;
			
		/***add end  ****/	
		
		
		
			
		/****0152指的是取贷款的内部账的本金科目*********,程序如果需要扩展可以在则例改动**/
		ret = pub_ln_get_acc_hrt(g_ln_mst , g_ln_parm , "0152" , acc_hrt_tmp );
		if(ret)
		{
			vtcp_log("[%s][%d]根据贷款登记簿获取贷款科目号错误!\n",__FILE__,__LINE__);
			return ret;
		}	
		/*****对acc_hrt进行处理，补充为7位的科目号**/
		pub_base_strpack(acc_hrt_tmp);
		memcpy(acc_hrt,acc_hrt_tmp,strlen(acc_hrt_tmp));
		vtcp_log("[%s][%d]获取科目号为:[%s]\n",__FILE__,__LINE__,acc_hrt);
		/***获取转换前的科目字*******************/
		ret = pub_ln_get_form_acc_hrt(g_ln_mst , g_ln_parm , "0152" , form_acc_hrt_tmp );
		if(ret)
		{
			vtcp_log("[%s][%d]根据贷款登记簿获取贷款科目号错误!\n",__FILE__,__LINE__);
			return ret;
		}
		pub_base_strpack(form_acc_hrt_tmp);
		memcpy(form_acc_hrt,form_acc_hrt_tmp,strlen(form_acc_hrt_tmp));
		vtcp_log("[%s][%d]获取科目号为:[%s]\n",__FILE__,__LINE__,form_acc_hrt);	
		
		
		
		
		/* 机构变换时 */
		pub_base_strpack( s_trace_log_bk.tx_br_no );
		if( strcmp( s_trace_log_bk.tx_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/**不是第一个机构 到了另外一个机构关闭文件***/
				fclose(fp);
			}
			
			strcpy( old_br_no , s_trace_log_bk.tx_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_trace_log_bk.tx_br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_trace_log_bk.tx_br_no , filename );
			ERR_DEAL
			
		}
    		/* 打印信息 */
		ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
		ERR_DEAL
		line_file++;
		if( line_file>TABLE_RECORD )	/* 大于换页 */
		{
	/*
			line_file=0;
			page++;
			fprintf(fp,"\f");
			fprintf(fp,"\n");
			fprintf(fp,"\n");
	*/
		}
		ttlnum++;
	}
	Trace_log_bk_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成贷款到期流水清单(正常转逾期)成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成贷款到期流水清单(正常转逾期)失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_mbfesd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	char cDate[9];
	char cTx_Desc[17];
	char cTel[7];
	char amt_tmp[22];
	
	memset(vstr,	0x00,	sizeof(vstr));
	memset(fmt,	0x00,	sizeof(fmt));
	memset(cDate,	0x00,	sizeof(cDate));
	memset(cTel,	0x00,	sizeof(cTel));
	memset(cTx_Desc,0x00,	sizeof(cTx_Desc));
	memset(amt_tmp,0x00,sizeof(amt_tmp));

	struct com_branch_c sComBranch;
	memset(&sComBranch,0x00,sizeof(sComBranch));
	memset(str,	0x00,	sizeof(str));
	
	
	switch( bli ) 
	{
		case 'A': 
			vtcp_log("[%s][%d]开始打印--户名!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf(str,fmt,g_ln_mst.name);
			break;
		case 'B': 
			vtcp_log("[%s][%d]开始打印--帐号!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf(str,fmt,g_mdm_ac_rel.ac_no);
			break;
		case 'D':
			vtcp_log("[%s][%d]开始打印--机构号码!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			ret = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", s_trace_log_bk.tx_br_no );
			if( ret )
			{
				sprintf(acErrMsg, "SELECT com_branch error![%d]", ret );
				WRITEMSG
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s", sComBranch.br_name);
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'E': 
			vtcp_log("[%s][%d]开始打印--年!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'F': 
			vtcp_log("[%s][%d]开始打印--月!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'G': 
			vtcp_log("[%s][%d]开始打印--日!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'I':
			vtcp_log("[%s][%d]打印行号!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.tx_br_no ); 
			break;
		case 'J': 
			vtcp_log("[%s][%d]开始打印--金额!\n",__FILE__,__LINE__);
			vtcp_log("[%s][%d]开始打印--交易金额!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf( amt_tmp,"%.2lf",g_ln_mst.bal);
			pub_rpt_flttomon( amt_tmp,amt_tmp ); 
			vtcp_log("[%s][%d]fmt=[%s]\n",__FILE__,__LINE__,fmt);
			numtomoney( g_ln_mst.bal, vstr);
			vtcp_log("[%s][%d]fmt=[%s]\n",__FILE__,__LINE__,fmt);
			strcat(vstr,"     小写金额: ");
			strcat(vstr,amt_tmp);
			sprintf(str,fmt,vstr);
			vtcp_log("金额1=[%lf],vstr==[%s]str==[%s]\n",g_ln_mst.bal,vstr,str);
			/*strcpy( str, vstr ); */
			break; 
		case 'K':
			vtcp_log("[%s][%d]开始打印--转帐原因1!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
			switch(g_ln_mst.ac_sts[0])
			{
				case '1':
					strcpy(vstr,"贷款到期自动还款");
					break;
				case '2':
					strcpy(vstr,"正常转逾期");
					break;
				case '3':
					strcpy(vstr,"逾期转呆滞");
					break;
				case '4':
					strcpy(vstr,"呆滞转呆帐");
					break;
				case '5':
					strcpy(vstr,"逾期转90天外");
				default:
					strcpy(vstr,"        ");					
			}
			vtcp_log("funk!vstr==[%s]\n",vstr);
			memcpy(str,vstr,bll);
			 break;
		case 'L':
			vtcp_log("[%s][%d]开始打印--逾期内贷款科目字!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
		        memcpy(vstr,acc_hrt,7);
			memcpy(str,vstr,bll);
			 break;
		case 'M':
			vtcp_log("[%s][%d]开始打印--逾期内贷款保证质押贷款科目字!\n",__FILE__,__LINE__);
			memcpy(vstr,form_acc_hrt,7);
			memcpy(str,vstr,bll);
			 break;
		default : 
			memset( str,' ',bll ); 
			
	}
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]值[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}

/****用来将金额转换为大写****/
/***送进来的金额是字符形式的,如12345.67,12345.00 ,12345***/
char * cTurnMoneyUpper(char *money)
{
    char cBuf[100];
    char cZPart[30];
    char cXPart[10];
    int  i_num = 0;
    int  i_unit= 0;
    char *_prt = NULL;
    int i =0;

    char unit[13][5] = {"点","圆","拾","佰","仟","万","拾","佰","仟","亿","拾亿","佰亿"};
    char numb[11][3] = {"","壹","贰","叁","肆","伍","陆","柒","捌","玖","拾"};

    memset(cBuf,0x00,sizeof(cBuf));
    memset(cZPart,0x00,sizeof(cZPart));
    memset(cXPart,0x00,sizeof(cXPart));

    pub_base_strpack( money );
    printf("money===[%s]\n",money);

    _prt = strtok(money,".");
    printf("_prt==[%s]\n",_prt);
    if(_prt!=NULL)
    {
    	memcpy(cZPart,_prt,strlen(_prt));
    	memcpy(cXPart,money+strlen(_prt)+1,2);
    }
    else if(_prt==0x00) /***如果没有.的话,说明补上00*****/
    {
        memcpy(cZPart,money,strlen(money));
        memcpy(cXPart,"00",2);
    }

    printf("整数部分==[%s],小数部分=[%s]\n",cZPart,cXPart);

    /****开始处理整数部分****/
    i = 0;
    printf("[%c] cZPart[i]===\n",cZPart[i]);
    while(cZPart[i]!=0x00)
    {
        i_num = cZPart[i]-'0';
        printf("cZPart[i]==[%c] i_num===[%d]\n",cZPart[i],i_num);
        if(i_num!=0 ||(strlen(cZPart)-i)==5)
        {
        	strcat(cBuf,numb[i_num]);
			strcat(cBuf,unit[strlen(cZPart)-i]);
		}
        i ++;
    }
    /*******开始处理小数部分***/
    if(!memcmp(cXPart,"00",2) && cBuf[0]!=0x00)
        strcat(cBuf,"整");
    else if(cBuf[0]==0x00 &&!memcmp(cXPart,"00",2))
    	memcpy(cBuf,"零圆整",6);
    else
    {
        strcat(cBuf,"点");
        i_num = cXPart[0]-'0';
        if(i_num==0)
            	strcat(cBuf,"零");
        else
       		strcat(cBuf,numb[i_num]);
       		
       		
        i_num = cXPart[1]-'0';
        if(i_num==0)
            	strcat(cBuf,"零");
        else
        	strcat(cBuf,numb[i_num]);
    }

    printf("money==[%s]-->[%s]======\n",money,cBuf);

    return cBuf;


}
/**********************************************************************
* 函 数 名：  pub_ln_get_acc_hrt
* 函数功能：  输入数据编码取贷款科目号
* 作    者：  rob
* 完成时间：  2003年07月29日
*
* 参    数：
*     输  入：   p_ln_mst            贷款主文件结构
*		 p_ln_parm 	     贷款参数结构
*                data_code           数据编码
*      
*     输  出:    acc_hrt	     char(8)	 科目号
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int pub_ln_get_form_acc_hrt( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *data_code , char *acc_hrt )
{
	int ret =0;
	char ln_dc_code[5];
	
	struct dc_acc_rel_c	dc_acc_rel_tmp;
	struct com_item_c com_item_tmp;
	memset( &com_item_tmp  , 0x00, sizeof(struct com_item_c));
	memset( &dc_acc_rel_tmp , 0x00 , sizeof( struct dc_acc_rel_c ) );
	
	if( p_ln_mst.ac_sts[0]=='2' )/* 逾期90天内 */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.dc_code);	
	}
	if( p_ln_mst.ac_sts[0]=='5' )/* 逾期90天外 */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.over_dc_code);	
	}
	if(p_ln_mst.ac_sts[0]=='3')/*呆滞  */
	{
		TRACE
		strcpy(ln_dc_code,p_ln_parm.over90_dc_code);
		
	}
	if( p_ln_mst.ac_sts[0]=='4' )/*呆账  */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.sla_dc_code);	
	}
	
	ret = Dc_acc_rel_Sel(g_pub_tx.reply , &dc_acc_rel_tmp , "dc_code='%s' and data_code='%s' ",ln_dc_code, data_code );
	if ( ret )
	{
		sprintf(acErrMsg,"读取会计代码对照表错误!!RespCode=[%ld][%s]",
			ret,ln_dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S050" );
		return 1;
	}
	ret = Com_item_Sel(g_pub_tx.reply,&com_item_tmp,"acc_hrt='%s'",dc_acc_rel_tmp.acc_hrt);
	if(ret)
	{
		sprintf(acErrMsg,"读取会计代码对照表错误!!RespCode=[%ld][%s]",
			ret,ln_dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S050" );
		return 1;
	}
	strcpy( acc_hrt , com_item_tmp.acc_hrt);
	
	return 0;
}	

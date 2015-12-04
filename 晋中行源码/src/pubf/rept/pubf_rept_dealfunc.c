/*************************************************
* 文 件 名: pubf_rept_dealfunc.c
* 功能描述: 生成报表函数调度程序
*
* 作    者:
* 完成日期：2004年04月14日
* 修改记录：
* 1. 日    期:
*    修 改 人:
*    修改内容:
*************************************************/
#include <stdio.h>
#define EXTERN
#define EXTERNREPT
#include "public.h"
#include "pubf_rept_fun.h"

			char *pub_rept_com_syer();/*出表年*/
			char *pub_rept_com_smon();/*出表月*/
			char *pub_rept_com_sday();/*出表日*/
			char *pub_rept_com_ssea();/*出表季度*/
			char *pub_rept_com_sbrname();/*出表机构*/

			char *pub_rept_cal_kcxjxe(char *_br_no);    /*库存现金限额*/
			char *pub_rept_cal_kcs(char *_br_no);    /*库存数*/
			char *pub_rept_cal_sjs(char *_br_no);    /*上介数*/
			char *pub_rept_cal_sjsj(char *_br_no);    /*上介时间*/
			char *pub_rept_cal_fke(char *_up_br_no);    /*罚款额*/
			char *pub_rept_cal_zyywzkrbb(char *_up_br_no,char *_item);
			char *pub_rept_cal_zyywzkybb(char *_up_br_no,char *_item);
			char *pub_rept_cal_zhkmypj1(char *_up_br_no,char *_main_acc);
			char *pub_rept_cal_zhkmye1(char *_up_br_no,char *_main_acc);
			char *pub_rept_cal_bsrzhkmye1(char *_up_br_no,char *_main_acc);
			char *pub_rept_cal_bnczhkmye1(char *_up_br_no,char *_main_acc);
			char *pub_rept_cal_zhkmye(char *_up_br_no,char *_main_acc);
			char *pub_rept_cal_bsrzhkmye(char *_up_br_no,char *_main_acc);
			char *pub_rept_cal_bnczhkmye(char *_up_br_no,char *_main_acc);
			char *pub_rept_gl_mst_field( char *fld,char *br_no,char *acc_hrt );
			char *pub_rept_gl_rpt_field( char *fld,char *br_no,char *code );
			char *pub_rept_zjgl_mst_field( char *fld,char *br_no,char *acc_hrt );
			char *pub_rept_get_tax(char *parm1,char *parm2,char *br_no,char flag );
            char *pub_rept_TCQS( char *tcqs, char *cash_chg_no );
            char *pub_rept_XJTJ( char *xjtj, char *xjname );

		char *pub_rept_ZQZZ( char *fld,char *br_no,char *acc_hrt);
		char *pub_rept_gl_in_mst_field(char *fld,char *br_no,char *ac_id);
		char *pub_rept_ass_mst_field( char *fld,char *br_no,char *acc_hrt );
/**********************************************************************
* 函 数 名： pub_rept_dealfunc( )
* 函数功能： 生成报表的调度函数,处理函数集合
* 作者/时间：power/2004.1.6
* 参数：
*     输入：
*           char    *prog_name    程序标识号
*     输出：
*
* 返 回 值: 0 成功  -1 失败
* 修改历史：
*
********************************************************************/
char *pub_rept_dealfunc(char *prog_name)
{
	char	*_ret_val;
	char	l_parm[10][256];			/* 函数参数 */
	char	l_str_tmp[CAL_FORM_LEN];	/* 临时保存程序名的变量 */ 

	memset(l_parm, 0x00, sizeof(l_parm));
	memset(l_str_tmp, 0x00, sizeof(l_str_tmp));

	strcpy(l_str_tmp, prog_name);
	get_funcName_parm_1(l_str_tmp, l_parm);
	while(1)
	{
		if( !strncmp(l_parm[0],"ZZ",2) )
		{
			_ret_val = pub_rept_gl_mst_field( l_parm[0],g_brno_tmp.br_no,l_parm[1] );
			break;
		}
		else if( !strncmp(l_parm[0],"GLRPT",5) )
		{
printf("GLRPT xxxxwprog_name[%s][%s][%s][%s]",
			prog_name,l_parm[0],l_parm[1],l_parm[2]);
			_ret_val = pub_rept_gl_rpt_field( l_parm[0],g_brno_tmp.br_no,l_parm[1] );
			break;
		}
		else if( !strncmp(l_parm[0],"NBZ",3) )/**内部帐,传入帐号***/
		{
			_ret_val =(char *) pub_rept_gl_in_mst_field( l_parm[0],g_brno_tmp.br_no,l_parm[1] );
			break;
		}
		else if( !strncmp(l_parm[0],"ZJ",2) )/**进入资金管理登记部***/
		{
			_ret_val =(char *) pub_rept_zjgl_mst_field( l_parm[0],g_brno_tmp.br_no,l_parm[1] );
			break;
		}
		else if( !strncmp(l_parm[0],"ASS",3) )/**进入ass_mst登记部***/
		{
			_ret_val =(char *) pub_rept_ass_mst_field( l_parm[0],g_brno_tmp.br_no,l_parm[1] );
			break;
		}
/**added by Jarod begin用于计算利息税的函数**/
		else if( !strncmp(l_parm[0],"LXRC",4)) /*取利息的函数*/
		{
			_ret_val=(char *)pub_rept_get_tax(l_parm[0],l_parm[1],g_brno_tmp.br_no,g_brno_tmp.ind[0]);
			break; 
		}
		else if( !strncmp(l_parm[0],"LX",2) ) /*取利息的函数*/
		{
			_ret_val=(char *)pub_rept_get_tax(l_parm[0],l_parm[1],g_brno_tmp.br_no,g_brno_tmp.ind[0]);
			break; 
		}
		else if( !strncmp(l_parm[0],"SDS",3)) /*取利息的函数*/
		{
			_ret_val=(char *)pub_rept_get_tax(l_parm[0],l_parm[1],g_brno_tmp.br_no,g_brno_tmp.ind[0]);
			break; 
		}
		/**added by martin begin用于同城清算轧差的函数**/
		else if( !strncmp(l_parm[0],"TCQSC",5) || !strncmp(l_parm[0],"TCQSS",5)) /*取利息的函数*/
		{

			_ret_val=(char *)pub_rept_TCQS(l_parm[0],l_parm[1]);

			break; 
		}
		/***add by martin 用於現金統計（晉中）**/
		else if( !strncmp(l_parm[0],"XJTJ",4)) /*函数*/
		{

			_ret_val=(char *)pub_rept_XJTJ(l_parm[0],l_parm[1]);

			break; 
		}
		/***add by liuyue  损益表增加 同期数 ****/
		else if( !strncmp(l_parm[0],"ZQZZ",4) ) /*从表gl_tsub中取值*/
		{
			vtcp_log("lovexier [%s][%d][%s---%s----%s]\n",__FILE__,__LINE__,l_parm[0],g_brno_tmp.br_no,l_parm[1] );
			_ret_val=(char *)pub_rept_ZQZZ( l_parm[0],g_brno_tmp.br_no,l_parm[1] );

			break; 
		}
		/*----------常用基础函数----------*/
		if( !strcmp(l_parm[0], "NULL") )
		{
			/*_ret_val = f_prt_null();*/
			break;
		}

		if( !strcmp(l_parm[0], "LINE") )
		{
			/*_ret_val = f_prt_line();*/
			break;
		}

		if( !strcmp(l_parm[0], "BCMC") )	/* 根据机构号取得机构名称 */
		{
			_ret_val = pub_rept_com_sbrname();
			break;
		}

		if( !strcmp(l_parm[0], "SYER") )	/* 从系统表中取系统时间年 */
		{
			_ret_val = pub_rept_com_syer();
			break;
		}

		if( !strcmp(l_parm[0], "SMON") )	/* 从系统表中取系统时间月 */
		{
			_ret_val = pub_rept_com_smon();
			break;
		}

		if( !strcmp(l_parm[0], "SDAY") )	/* 从系统表中取系统时间日 */
		{
			_ret_val = pub_rept_com_sday();
			break;
		}


		/*----------计算函数----------*/

		if( !strcmp(l_parm[0], "ZHKMYPJ") )	/**/
		{
			_ret_val = pub_rept_cal_zhkmypj1(l_parm[1],l_parm[2]);
			break;
		}

		if( !strcmp(l_parm[0], "ZHKMYE") )	/**/
		{
			_ret_val = pub_rept_cal_zhkmye1(l_parm[1],l_parm[2]);
			break;
		}

		if( !strcmp(l_parm[0], "BSRZHKMYE") )	/**/
		{
			_ret_val = pub_rept_cal_bsrzhkmye1(l_parm[1],l_parm[2]);
			break;
		}

		if( !strcmp(l_parm[0], "BNCZHKMYE") )	/**/
		{
			_ret_val = pub_rept_cal_bnczhkmye1(l_parm[1],l_parm[2]);
			break;
		}

		if( !strcmp(l_parm[0], "ZYYWZKRBB") )	/**/
		{
			_ret_val = pub_rept_cal_zyywzkrbb(l_parm[1],l_parm[2]);
			break;
		}

		if( !strcmp(l_parm[0], "ZYYWZKYBB") )	/**/
		{
			_ret_val = pub_rept_cal_zyywzkybb(l_parm[1],l_parm[2]);
			break;
		}

		if( !strcmp(l_parm[0], "KCXJXE") )	/**/
		{
			_ret_val = pub_rept_cal_kcxjxe(l_parm[1]);
			break;
		}

		if( !strcmp(l_parm[0], "KCS") )	/**/
		{
			_ret_val = pub_rept_cal_kcs(l_parm[1]);
			break;
		}

		if( !strcmp(l_parm[0], "SJS") )	/**/
		{
			_ret_val = pub_rept_cal_sjs(l_parm[1]);
			break;
		}

		if( !strcmp(l_parm[0], "SJSJ") )	/**/
		{
			_ret_val = pub_rept_cal_sjsj(l_parm[1]);
			break;
		}

		if( !strcmp(l_parm[0], "FKE") )	/**/
		{
			_ret_val = pub_rept_cal_fke(l_parm[1]);
			break;
		}

		/***XXXX***/
		break;
	}
	return _ret_val;	/* 正确出口 */
}


/***下面的部分是liuyue 从bbxxxx.pc文件中移植过来的,换了名字20061022****/
/*********************************************************
	从字符串截取函数标示号和参数
	input	:
				char	*_str
	output	:
				
**********************************************************/
int	get_funcName_parm_1(char *_str, char _re_val[10][256])
{
	int		i = 0;

	while(1)
	{
		if(_str[0]=='\0' || _str[0] == '\n') break;
		get_substr_1(_str, _re_val[i]);	
		del_after_space_1(_re_val[i]);
		i++;
	}
	return 0;
}
/*******************************************************
	根据 '-' , '.' 从字符串中截取子串
	input	:
				char	*_str		
	output	:
				char	*_str		
				char	*_sub_str

********************************************************/
int get_substr_1(char  *_str, char *_sub_str)
{
	int     i, k = 0;
	int     str_len;

	/***  去掉字符串前面的空格  ***/
	del_fore_space_1(_str);

	str_len = strlen(_str);

	for(i = 0; i<=str_len; i++,k++)
	{
		if( _str[i] == '_' || _str[i] == '.' || _str[i] == '\0' || _str[i] == '\n') break;
		_sub_str[k] = _str[i];
	}
	_sub_str[k+1] = '\0';

	if(_str[i] == '\0' || _str[i] == '\n')
		strcpy(_str,_str+i);
	else
		strcpy(_str,_str+i+1);

	return 0;
}


/*******************************************************
	去掉字符串前面的空格
	input	:
				char	*_str;
	output	:
				char	*_str;
********************************************************/
int del_fore_space_1(char *_str)
{
	int     i, k = 0;
	int     str_len;
	char    str[STR_TMP_LEN];

	str_len = strlen(_str);

	for(i = 0; i<str_len; i++,k++)
	{
		if( _str[i] == ' ') continue;
		else    break;
	}
	for(i=0; i<str_len; i++,k++)
	{
		if(_str[k] == '\0' || _str[k] == '\n')
		{
			_str[i] = '\0';
			break;
		}else
		{
			_str[i] = _str[k];
		}
	}
	return 0;
}

/*********************************************************
	去掉字符串后面的空格
	input	:
				char	*_str;
	output	:
				char	*_str;
**********************************************************/
int del_after_space_1(char *_str)
{
	int     i, k = 0;
	int     str_len;

	str_len = strlen(_str);

	for(i = str_len-1; i>0; i-- )
	{
		if( _str[i] == ' ' || _str[i] == '\n') continue;
		else    break;
	}
	_str[i+1] = '\0';

	return 0;
}

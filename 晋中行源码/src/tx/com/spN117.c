/*************************************************
* 文 件 名:  spN117.c
* 功能描述： 
*	在网点内部各柜员间可以进行凭证的调剂；内部调剂采
*	用收付两步处理，调出者先作付出，调入者再作收入。
*	收入的前提是付出。
*
* 作    者:  andy
* 完成日期： 2004年1月10日
*
*insert into tx_def values('5117', 'VTM加清卡', '1000000000000000000000000100000000000000000000000000000000010000000000000000000000000000000100000000000000000000000000000000000', '1', '6', '2');
*insert into tx_flow_def values('5117', '0', 'N117', '#$');
*insert into tx_sub_def values('N117', 'VTM加清卡', 'spN117', '0', '0');
*   修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "vtm_note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_branch_c.h"
#include "com_tel_c.h"
#include "com_item_c.h"
#include "note_parm_c.h"

/******************* 定义全局变量 ********************/
char	g_cOperCode[2];			/* 操作代码 */
char	g_cTwBrno[6];			/* 付出时对方机构 */
char	g_cTwTel[5];			/* 付出时对方柜员 */

#define NOTESECTIONNUM 3/**支持的凭证号段数**/

int spN117()
{
	int  ret;/* 函数返回值 */
	long lNoteCnt = 0;
	long lBegNoteNo = 0;
	long lEndNoteNo = 0;
	char cTmp[1024];
	char cBegNoteNo[17];
	char cEndNoteNo[17];
	struct  com_tel_c com_tel;
	struct  note_parm_c sNoteParm;
	struct  com_item_c  sComItem;
	
	memset( g_cOperCode, 0x00, sizeof( g_cOperCode ) );
	memset( g_cTwTel, 0x00, sizeof( g_cTwTel ) );
	memset( cTmp, 0x00, sizeof( cTmp ) );
	memset( cBegNoteNo, 0x00, sizeof( cBegNoteNo ) );
	memset( cEndNoteNo, 0x00, sizeof( cEndNoteNo ) );
	memset(&com_tel,0x0,sizeof(struct com_tel_c));
	memset(&sNoteParm,0x00,sizeof(sNoteParm));
	memset(&sComItem,0x00,sizeof(sComItem));
	
	/* 初始化g_pub_tx结构 */	
	pub_base_sysinit();
	
	get_zd_data( "0670", g_cOperCode );		/* 操作代码 */
	get_zd_data( "0890", g_pub_tx.note_type );	/* 凭证类型 */
	get_zd_data( "0920", g_cTwTel );	/* 对方柜员 */
	get_zd_data( "1230", cTmp);/*凭证号码*/
	
	vtcp_log("%s,%d 操作代码 g_cOperCode=[%s]\n", __FILE__, __LINE__, g_cOperCode);
	vtcp_log("%s,%d 凭证类型 note_type=[%s]\n", __FILE__, __LINE__, g_pub_tx.note_type);
	vtcp_log("%s,%d 对方柜员 g_cTwTelel=[%s]\n", __FILE__, __LINE__, g_cTwTel);
	vtcp_log("%s,%d 凭证号码 cTmp=[%s]\n", __FILE__, __LINE__, cTmp);
	
	zip_space(cTmp);
	if (0 != vtm_note_sec_num_check(cTmp))
	{
		sprintf(acErrMsg, "领入校验出错");
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Note_parm_Sel(g_pub_tx.reply,&sNoteParm,"note_type='%s'",g_pub_tx.note_type);
	if(ret){
		vtcp_log("%s,%d 查询凭证信息错误 note_type=[%s]\n",__FILE__,__LINE__,g_pub_tx.note_type);
		goto ErrExit;
	}
	set_zd_data("0152",sNoteParm.acc_hrt);
	ret = pub_base_item_inf(sNoteParm.acc_hrt,&sComItem);
	if(ret){
		vtcp_log("%s,%d 根据凭证取对应科目信息错误 note_type=[%s],acc_hrt=[%s],acc_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.note_type,sNoteParm.acc_hrt,sComItem.acc_no);
		goto ErrExit;
	}
	strcpy(g_pub_tx.ac_no,sComItem.acc_no);
	
	/**本方权限检查**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,301,0 ) )
	{
			vtcp_log("本方柜员权限错误!");
			goto ErrExit;
	}

	/**对方权限检查**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_cTwTel,301,1 ) )
	{
			vtcp_log("对方柜员权限错误!");
			goto ErrExit;
	}
	if(strlen(g_cTwTel))
	{	
			ret=Com_tel_Sel(g_pub_tx.reply,&com_tel,"tel='%s'",g_cTwTel);
			if(ret)
			{
					vtcp_log("对方柜员权限错误!");
					goto ErrExit;
			}
			if(!strcmp(com_tel.tx_type,"0000100000"))
			{
						sprintf(acErrMsg, "不允许往该柜员调剂凭证!! ");
						WRITEMSG
						strcpy(g_pub_tx.reply, "T064");
						goto ErrExit;
			}
	}
	
	if (0 != strlen(cTmp))
	{
		
		for (int i = 0; i < NOTESECTIONNUM; i++)
		{
			memcpy(cBegNoteNo, cTmp + 2 * i * 16, 16);
			memcpy(cEndNoteNo, cTmp + (2 * i + 1) * 16, 16);
			
			if (0 != strlen(cBegNoteNo) && 0 != strlen(cEndNoteNo))
			{
				set_zd_data("0580", cBegNoteNo);
				set_zd_data("0590", cEndNoteNo);
				get_zd_long("0580", &lBegNoteNo);
				get_zd_long("0590", &lEndNoteNo);
				lNoteCnt = lEndNoteNo - lBegNoteNo + 1;
				if(1 <= lNoteCnt && 0 < atoi(cBegNoteNo))
				{
					strcpy(g_pub_tx.beg_note_no, cBegNoteNo);
					strcpy(g_pub_tx.end_note_no, cEndNoteNo);
					g_pub_tx.tx_amt1 = lNoteCnt;
					if (-1 == note_chg())
					{
						sprintf(acErrMsg, "调剂凭证出错");
						WRITEMSG
						goto ErrExit;
					}
					vtcp_log("%s,%d lNoteCnt [%d][%ld][%s][%s]\n", __FILE__, __LINE__, i, lNoteCnt, cBegNoteNo, cEndNoteNo);
				}
			}
		}
	}
	else
	{
		if (-1 == no_note_chg())
		{
			sprintf(acErrMsg, "0介质清卡出错");
			WRITEMSG
			goto ErrExit;
		}
	}
	
	if (vtm_note_mst_reg(cTmp))
	{
		sprintf(acErrMsg, "更新VTM加清卡登记簿出错");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


int note_chg()
{
	struct 	note_mst_c	sNoteMst;	/* 柜员凭证台帐 */
	struct 	note_mst_c	sTwNoteMst;	/* 对方柜员凭证台帐 */
	struct	note_mst_hst_c	sNoteHst;	/* 柜员凭证明细 */
	struct  com_tel_c	sComTel;	/* 柜员文件 */
	struct  com_tel_c com_tel;
	struct  note_parm_c sNoteParm;
	struct  com_item_c  sComItem;
	char	cNoteAct[2];		/* 凭证行为 */
	char	cTwNoteAct[2];		/* 对方凭证行为 */
	char	cNoteSts[2];		/* 凭证状态，上缴时候用 */
	char	cInOut[2];
	char	prtime[9];
	long	lNoteCnt;
	int  	hour,min,sec,iCnt;
	
	int	ret=0;			/* 函数返回值 */
	
	memset(&sNoteParm,0x00,sizeof(sNoteParm));
	memset(&sComItem,0x00,sizeof(sComItem));
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sTwNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sNoteHst, 0x00, sizeof( struct note_mst_hst_c ) );
	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( cTwNoteAct, 0x00, sizeof( cTwNoteAct ) );
	memset( cNoteSts, 0x00, sizeof( cNoteSts ) );
	memset( cInOut, 0x00, sizeof( cInOut ) );
	memset( &com_tel,0x0,sizeof(struct com_tel_c));

	strcpy( sNoteMst.tel , g_pub_tx.tel );
	strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
	strcpy( sNoteMst.note_type, g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
	sNoteMst.cnt = (long)g_pub_tx.tx_amt1;
	
	/* 凭证有效性检查:是否存在,是否处于使用状态 */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "检查凭证是否存在，是否启用错" );
		WRITEMSG
		return -1;
	}
		
	/* 凭证起始号码、终止号码冠子头检查 */
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel );
	if ( ret )
	{
		sprintf( acErrMsg, "检查凭证起始号码冠子头错" );
		WRITEMSG
		return -1;
	}
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel );
	if ( ret )
	{
		sprintf( acErrMsg, "检查凭证终止号码冠子头错" );
		WRITEMSG
		return -1;
	}
	
	strcpy( cNoteAct, "2" );	/* 2表示付出 */	
	
	/* 检查操作码, 0 付出 1 收入 */
	switch( g_cOperCode[0] )
	{
		case '0':		/* 付出 */
			strcpy( cInOut, "O" );
			
			strcpy( g_pub_tx.brf,"调出" );
			strcpy( g_pub_tx.add_ind,"0" );
			vtcp_log("[%s][%d]本方权限错-lei--g_pub_tx.note_type=[%s]\n",__FILE__,__LINE__,g_pub_tx.note_type);
			
			/* 检查对方柜员是否是本机构得柜员 */
			ret = pub_base_GetTelInfo( g_cTwTel, &sComTel );
			if ( ret )
			{
				sprintf( acErrMsg, "查找收入方柜员文件错,ret=[%d]", ret );
				WRITEMSG
				return -1;
			}
			
			if (sComTel.csts[0] == '1' || sComTel.csts[0] == '2')
			{
				sprintf(acErrMsg, "该柜员已经签退不能下发!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply, "N096");
				return -1;
			}
			else if (sComTel.csts[0] != '0')
			{
				sprintf(acErrMsg, "该柜员不是签到状态不能下发!!");
				WRITEMSG 
				strcpy(g_pub_tx.reply, "N097");
				return -1;
			}
			
			/* 判断是否和交易柜员属于统一个机构（网点）*/
			if ( strcmp( g_pub_tx.tx_br_no, sComTel.br_no ) )
			{
				sprintf( acErrMsg, "对方柜员不属于本网点" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "S041" );
				return -1;
			}
			strcpy( g_cTwBrno, sComTel.br_no );
			
			/* 付出方柜员凭证检查:是否拥有，状态是否正常 */
			ret = pub_com_ChkNote( cNoteAct, sNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "检查柜员凭证错" );
				WRITEMSG
				return -1;
			}
			
			/* 登记转出柜员凭证台帐（分段处理） */
			ret = pub_com_NoteApart( cNoteAct, sNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "登记转出柜员凭证台帐（分段处理）错" );
				WRITEMSG
				return -1;
			}
			
			/*打印时间 add by zy*/
			pub_base_GetTime2( &hour, &min, &sec );
			memset( prtime, 0x00, sizeof( prtime ) );
			sprintf( prtime, "%02d:%02d:%02d", hour, min, sec );    /*打印时间*/
			set_zd_data( "0600", prtime );
			
			break;
			
		case '1':		/* 收入 */
			strcpy( cInOut, "I" );
			
			strcpy( g_pub_tx.brf,"调入" );
			strcpy( g_pub_tx.add_ind,"1" );
			vtcp_log("[%s][%d]本方权限错-lei--g_pub_tx.note_type=[%s]\n",__FILE__,__LINE__,g_pub_tx.note_type);
			
			/* 查找柜员凭证明细 */
			/* 根据交易日期.凭证种类.起始凭证.终止凭证.对方柜员.凭证行为 */
			vtcp_log("[%s][%d]--[%d][%s][%s][%s][%s]\n",__FILE__,__LINE__,g_pub_tx.tx_date,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.tx_br_no,g_pub_tx.tel);
			ret = pub_com_GetNoteHst( g_pub_tx.tx_date, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, g_pub_tx.tx_br_no, \
				g_pub_tx.tel, cNoteAct, &sNoteHst ); 
			if ( ret ) 
			{
				sprintf( acErrMsg, "查询柜员凭证明细错" );
				WRITEMSG
				ret=Note_mst_hst_Sel(g_pub_tx.reply,&sNoteHst,"tx_date=%d and note_type='%s' and beg_note_no='%s' and end_note_no='%s' and br_no='%s' and sts_ind='%s' ",g_pub_tx.tx_date,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tx_br_no,cNoteAct);
				if ( ret ) 
				{
					sprintf( acErrMsg, "查询柜员凭证明细错" );
					WRITEMSG
					return -1;	
				}
				vtcp_log("[%s][%d] sNotehst_tel[%s] sNoteHst_tw_tel[%s] g_pub_tx.tel[%s]\n",__FILE__,__LINE__,sNoteHst.tel,sNoteHst.tw_tel,g_pub_tx.tel);
				if(memcmp(sNoteHst.tel,g_pub_tx.tel,sizeof(sNoteHst.tel)-1)!=0&&memcmp(sNoteHst.tw_tel,g_pub_tx.tel,sizeof(sNoteHst.tw_tel)-1)!=0)
				{
					sprintf( acErrMsg, "查询柜员凭证明细错" );
					WRITEMSG
					return -1;	
				}
				iCnt=sql_count("note_mst_hst","tx_date=%d and note_type='%s' and beg_note_no='%s' and end_note_no='%s' and br_no='%s' and sts_ind='%s'",g_pub_tx.tx_date,
							g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tx_br_no,cNoteAct);
				if (iCnt==1)
				{
					ret = pub_com_GetNoteHst( g_pub_tx.tx_date, \
					g_pub_tx.note_type, g_pub_tx.beg_note_no, \
					g_pub_tx.end_note_no, g_pub_tx.tx_br_no, \
					sNoteHst.tw_tel, cNoteAct, &sNoteHst ); 
					if ( ret ) 
					{
						sprintf( acErrMsg, "查询柜员凭证明细错" );
						WRITEMSG
						return -1;
					}
				}
				else 
				{
					sprintf( acErrMsg, "查询柜员凭证明细错" );
					WRITEMSG
					return -1;
				}
			}
			vtcp_log("[%s][%d] sNotehst_tel[%s] sNoteHst_tw_tel[%s] g_pub_tx.tel[%s]\n",__FILE__,__LINE__,sNoteHst.tel,sNoteHst.tw_tel,g_pub_tx.tel);
			
			strcpy( g_cTwBrno, sNoteHst.br_no );
			strcpy( g_cTwTel, sNoteHst.tel );
			
			/* 检查对方柜员是否是本机构得柜员 */
			if ( strcmp( g_pub_tx.tx_br_no, sNoteHst.br_no ) )
			{
				sprintf( acErrMsg, "对方柜员不属于本网点" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "S041" );
				return -1;
			}
			
			/* 检查凭证存在，取出状态 */
			strcpy( cNoteAct, "3" );	/* 3表示收入 */
						
			strcpy( sTwNoteMst.tel , sNoteHst.tel );
			strcpy( sTwNoteMst.br_no , sNoteHst.br_no );
			strcpy( sTwNoteMst.note_type , sNoteHst.note_type );
			strcpy( sTwNoteMst.beg_note_no , sNoteHst.beg_note_no );
			strcpy( sTwNoteMst.end_note_no , sNoteHst.end_note_no );
			
			/* 检查对方操作员是否拥有这批凭证 */
			ret = pub_com_ChkNote( cNoteAct, sTwNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "检查柜员凭证错" );
				WRITEMSG
				return -1;
			}
			
			/* 登记柜员凭证台帐（并段处理） */
			ret = pub_com_RegTelNote(g_pub_tx.tel, g_pub_tx.tx_br_no,\
							cNoteAct, g_pub_tx.note_type, \
							g_pub_tx.beg_note_no,g_pub_tx.end_note_no );
			if ( ret )
			{
				sprintf( acErrMsg, "登记柜员凭证台帐（并段处理）错" );
				WRITEMSG
				return -1;
			}
			
			/* 删除/修改下发柜员的凭证 */
			strcpy( cTwNoteAct, "2" );	/* 3表示收入 */
			ret = pub_com_NoteDel ( cTwNoteAct, sTwNoteMst );
			if ( ret )
			{
				sprintf( acErrMsg, "删除/修改下发柜员的凭证错" );
				WRITEMSG
				return -1;
			}
			break;
			
		default:
			sprintf( acErrMsg, "凭证操作码不存在,操作码=[%s]", g_cOperCode );
			WRITEMSG
			return -1;
	}	/* end of switch */
	
	/* 登记柜员凭证明细 */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, g_cTwBrno, g_cTwTel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "登记柜员凭证明细错" );
		WRITEMSG
		return -1;
	}
		
	/* 登记凭证最小号 */
	ret = pub_com_RegNoteMin( g_pub_tx.tx_br_no, \
			g_pub_tx.note_type, g_pub_tx.beg_note_no, \
			g_pub_tx.tel, cInOut );
	if ( ret )
	{
		sprintf( acErrMsg, "登记凭证最小号错" );
		WRITEMSG
		return -1;	
	}

	/* 写交易日志 */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		return -1;
	}
	return 0;
}

/**零介质清卡**/
int no_note_chg()
{
	int iRet = 0;
	
	/**检查操作码, 0 付出 1 收入**/
	switch (g_cOperCode[0])
	{
		case '0': /**付出**/
			strcpy(g_pub_tx.brf, "调出");
			strcpy(g_pub_tx.add_ind, "0");
			if (tel_sts_check())
			{
				sprintf(acErrMsg, "tel_sts_check error");
				WRITEMSG
				return -1;
			}
			break;
			
		case '1': /**收入**/
			strcpy(g_pub_tx.brf, "调入");
			strcpy(g_pub_tx.add_ind, "1");
			break;
			
		default:
			sprintf(acErrMsg, "凭证操作码不存在,操作码=[%s]", g_cOperCode);
			WRITEMSG
			return -1;
	}
	
	/**写交易日志**/
	iRet = pub_ins_trace_log();
	if (iRet)
	{
		sprintf(acErrMsg, "记流水日志错");
		WRITEMSG
		return -1;
	}
	
	return 0;
}

/**对方柜员状态及机构检查**/
int tel_sts_check()
{
	int iRet = 0;
	struct com_tel_c stCom_tel;
	
	memset(&stCom_tel, 0x00, sizeof(struct com_tel_c));
	
	iRet = pub_base_GetTelInfo(g_cTwTel, &stCom_tel);
	if (iRet)
	{
		sprintf(acErrMsg, "查找收入方柜员文件错![%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	if ('1' == stCom_tel.csts[0] || '2' == stCom_tel.csts[0])
	{
		sprintf(acErrMsg, "该柜员已经签退不能下发!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "N096");
		return -1;
	}
	else if ('0' != stCom_tel.csts[0])
	{
		sprintf(acErrMsg, "该柜员不是签到状态不能下发!");
		WRITEMSG 
		strcpy(g_pub_tx.reply, "N097");
		return -1;
	}
	
	if (0 != strcmp(g_pub_tx.tx_br_no, stCom_tel.br_no))
	{
		sprintf(acErrMsg, "对方柜员不属于本网点");
		WRITEMSG
		sprintf(g_pub_tx.reply, "S041");
		return -1;
	}
	
	return 0;
}

/**号段数判断**/
int vtm_note_sec_num_check(char *cTmp)
{
	int iRet = 0;
	char beg_note_no1[17];
	char end_note_no1[17];
	char beg_note_no2[17];
	char end_note_no2[17];
	char beg_note_no3[17];
	char end_note_no3[17];
	struct com_tel_c stCom_tel;
	struct note_mst_c stNote_mst;
	struct vtm_note_mst_c stVtm_note_mst;
	
	memset(beg_note_no1, 0x00, sizeof(beg_note_no1));
	memset(end_note_no1, 0x00, sizeof(end_note_no1));
	memset(beg_note_no2, 0x00, sizeof(beg_note_no2));
	memset(end_note_no2, 0x00, sizeof(end_note_no2));
	memset(beg_note_no3, 0x00, sizeof(beg_note_no3));
	memset(end_note_no3, 0x00, sizeof(end_note_no3));
	memset(&stCom_tel, 0x00, sizeof(struct com_tel_c));
	memset(&stNote_mst, 0x00, sizeof(struct note_mst_c));
	memset(&stVtm_note_mst, 0x00, sizeof(struct vtm_note_mst_c));
	
	memcpy(beg_note_no1, cTmp,16);
	memcpy(end_note_no1, cTmp + 16, 16);
	memcpy(beg_note_no2, cTmp + 32, 16);
	memcpy(end_note_no2, cTmp + 48, 16);
	memcpy(beg_note_no3, cTmp + 64, 16);
	memcpy(end_note_no3, cTmp + 80, 16);
	
	iRet = Com_tel_Sel(g_pub_tx.reply, &stCom_tel, "tel='%s'", g_pub_tx.tel);
	if (iRet)
	{
		sprintf(acErrMsg, "查询柜员表出错");
		WRITEMSG
		return -1;
	}
	
	zip_space(stCom_tel.lvl);
	if (0 != memcmp(stCom_tel.lvl, "9", 1) || 0 == memcmp(g_cOperCode, "0", 1))
	{
		sprintf(acErrMsg, "不是VTM发起的加卡交易，不检查");
		WRITEMSG
		return 0;
	}
	
	iRet = Vtm_note_mst_Sel(g_pub_tx.reply, &stVtm_note_mst, "tw_tel='%s' and ind='2'", g_pub_tx.tel);
	if (iRet)
	{
		sprintf(acErrMsg, "查询VTM加清卡登记簿出错");
		WRITEMSG
		return -1;
	}
	zip_space(stVtm_note_mst.beg_note_no1);
	zip_space(stVtm_note_mst.end_note_no1);
	zip_space(stVtm_note_mst.beg_note_no2);
	zip_space(stVtm_note_mst.end_note_no2);
	zip_space(stVtm_note_mst.beg_note_no3);
	zip_space(stVtm_note_mst.end_note_no3);
	
	
	if (0 != strcmp(stVtm_note_mst.beg_note_no1, beg_note_no1)
		|| 0 != strcmp(stVtm_note_mst.end_note_no1, end_note_no1)
		|| 0 != strcmp(stVtm_note_mst.beg_note_no2, beg_note_no2)
		|| 0 != strcmp(stVtm_note_mst.end_note_no2, end_note_no2)
		|| 0 != strcmp(stVtm_note_mst.beg_note_no3, beg_note_no3)
		|| 0 != strcmp(stVtm_note_mst.end_note_no3, end_note_no3)
		)
	{
		vtcp_log("[%s][%d][%s][%s]\n", __FILE__, __LINE__, stVtm_note_mst.beg_note_no1, beg_note_no1);
		vtcp_log("[%s][%d][%s][%s]\n", __FILE__, __LINE__, stVtm_note_mst.end_note_no1, end_note_no1);
		vtcp_log("[%s][%d][%s][%s]\n", __FILE__, __LINE__, stVtm_note_mst.beg_note_no2, beg_note_no2);
		vtcp_log("[%s][%d][%s][%s]\n", __FILE__, __LINE__, stVtm_note_mst.end_note_no2, end_note_no2);
		vtcp_log("[%s][%d][%s][%s]\n", __FILE__, __LINE__, stVtm_note_mst.beg_note_no3, beg_note_no3);
		vtcp_log("[%s][%d][%s][%s]\n", __FILE__, __LINE__, stVtm_note_mst.end_note_no3, end_note_no3);
		sprintf(acErrMsg, "加卡凭证输入错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "LS78");
		return -1;
	}
	
	return 0;
}

/**更新VTM加清卡登记簿**/
int vtm_note_mst_reg(char *cTmp)
{
	int iRet = 0;
	char beg_note_no1[17];
	char end_note_no1[17];
	char beg_note_no2[17];
	char end_note_no2[17];
	char beg_note_no3[17];
	char end_note_no3[17];
	struct vtm_note_mst_c stVtm_note_mst;
	
	memset(beg_note_no1, 0x00, sizeof(beg_note_no1));
	memset(end_note_no1, 0x00, sizeof(end_note_no1));
	memset(beg_note_no2, 0x00, sizeof(beg_note_no2));
	memset(end_note_no2, 0x00, sizeof(end_note_no2));
	memset(beg_note_no3, 0x00, sizeof(beg_note_no3));
	memset(end_note_no3, 0x00, sizeof(end_note_no3));
	memset(&stVtm_note_mst, 0x00, sizeof(struct vtm_note_mst_c));
	
	memcpy(beg_note_no1, cTmp, 16);
	memcpy(end_note_no1, cTmp + 16, 16);
	memcpy(beg_note_no2, cTmp + 32, 16);
	memcpy(end_note_no2, cTmp + 48, 16);
	memcpy(beg_note_no3, cTmp + 64, 16);
	memcpy(end_note_no3, cTmp + 80, 16);
	
	if (0 == memcmp(g_cOperCode, "0", 1))
	{
		strcpy(stVtm_note_mst.tel, g_pub_tx.tel);
		strcpy(stVtm_note_mst.tw_tel, g_cTwTel);
		strcpy(stVtm_note_mst.note_type, g_pub_tx.note_type);
		strcpy(stVtm_note_mst.beg_note_no1, beg_note_no1);
		strcpy(stVtm_note_mst.end_note_no1, end_note_no1);
		strcpy(stVtm_note_mst.beg_note_no2, beg_note_no2);
		strcpy(stVtm_note_mst.end_note_no2, end_note_no2);
		strcpy(stVtm_note_mst.beg_note_no3, beg_note_no3);
		strcpy(stVtm_note_mst.end_note_no3, end_note_no3);
		stVtm_note_mst.tx_date = g_pub_tx.tx_date;
		stVtm_note_mst.trace_no = g_pub_tx.trace_no;
		strcpy(stVtm_note_mst.ind, "2");
		
		iRet = Vtm_note_mst_Ins(stVtm_note_mst, g_pub_tx.reply);
		if (iRet)
		{
			sprintf(acErrMsg, "登记VTM加清卡登记簿异常出错[%d]!", iRet);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D107");
			return -1;
		}
	}
	else if (0 == memcmp(g_cOperCode, "1", 1))
	{
		iRet = sql_execute("update vtm_note_mst set ind='3' where tel='%s' and tw_tel='%s' and ind='2'", g_cTwTel, g_pub_tx.tel);
		if (iRet)
		{
			sprintf(acErrMsg, "修改登记薄状态失败!", iRet);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D105");
			return -1;
		}
	}
	
	return 0;
}
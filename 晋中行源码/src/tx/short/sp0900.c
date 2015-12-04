/*************************************************
* 文 件 名: sp0900.c
* 功能描述：检查客户凭证
* 作    者: wyw 
* 完成日期：2007年03月6日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "string.h"
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "lv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "note_parm_c.h"
#include "note_cheq_mst_c.h"
#include "lv_define.h"
struct chkpz{
		char pay_ac_no[32];
		char pz_type[3];
		char pz_num[20];
		char buff[930];
};
sp0900()
{
	char payactno[33];
	char pztype[4];
	char pznum[21];
	int count,target;
	int pzno,stnum,endnum;
	char flag[2];
	struct chkpz pz_rec;
	struct note_parm_c     g_note_parm;
	struct mdm_ac_rel_c    g_mdm_ac_rel;
	struct note_cheq_mst_c g_note_cheq_mst;
	memset(flag,0,sizeof(flag));
	memset(payactno,0,sizeof(payactno));
	memset(pztype,0,sizeof(pztype));
	memset(pznum,0,sizeof(pznum));
	memset(&pz_rec,0,sizeof(pz_rec));
	get_fd_data("1230",&pz_rec);
	memcpy(payactno,pz_rec.pay_ac_no,sizeof(payactno)-1);
	memcpy(pztype,pz_rec.pz_type,sizeof(pztype)-1);
	memcpy(pznum,pz_rec.pz_num,sizeof(pznum)-1);
	pub_base_strpack(payactno);
	pub_base_strpack(pznum);
	pub_base_old_acno(payactno);
	vtcp_log("[%s][%d],payactno[%s]pztype[%s]pznum[%s]\n",__FL__,payactno,pztype,pznum);

	pub_base_sysinit();
	g_reply_int=Note_parm_Sel(g_pub_tx.reply,&g_note_parm,"note_type='%s'",pztype);
	if(g_reply_int==100){
		sprintf(acErrMsg,"不存在此类凭证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P132");
		goto ErrExit;
	}else if(g_reply_int){
		sprintf(acErrMsg,"查询凭证参数表错误!");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d],++++++++++payactno[%s]pztype[%s]pznum[%s]\n",__FL__,payactno,pztype,pznum);
	g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",payactno);
	if(g_reply_int){
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!![%s]",payactno);
        WRITEMSG
        strcpy(g_pub_tx.reply,"M003");
        goto ErrExit;
   }
	vtcp_log("[%s][%d],ac_id[%ld]pztype[%s]pznum[%s]\n",__FL__,g_mdm_ac_rel.ac_id,pztype,pznum);
	/* rem by LiuHuafeng order by 王永畏 
	count = sql_count("note_cheq_mst","ac_id=%ld and ac_seq=%ld and ",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	*** replace by next line ******************/
	count = sql_count("note_cheq_mst","ac_id=%ld and ac_seq=%ld and br_no='%s'",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn,g_pub_tx.tx_br_no);
	if(count==0){
		sprintf(acErrMsg,"此帐户不存在此类凭证!");
		WRITEMSG
		flag[0]='0';
		set_zd_data("0720",flag);
		goto OkExit;
	}
	pzno=atoi(pznum);	
	vtcp_log("[%s][%d],ac_id[%ld]count[%d]pzno[%ld]\n",__FL__,g_mdm_ac_rel.ac_id,count,pzno);
	/* rem by LiuHuafeng order by 王永畏 20070504
	g_reply_int = Note_cheq_mst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and  ac_seq=%ld and br_no = '%s' and note_type='%s' ",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn,g_pub_tx.tx_br_no,pztype);
	****** by next line *********************/
	g_reply_int = Note_cheq_mst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and  ac_seq=%ld and note_type='%s' ",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn,pztype);

	while(1){	
		vtcp_log("[%s][%d],ac_id[%ld]count[%d]pzno[%ld]\n",__FL__,g_mdm_ac_rel.ac_id,count,pzno);
		/**查询客户凭证台账,验证支票号码**/
		g_reply_int = Note_cheq_mst_Fet_Sel(&g_note_cheq_mst,g_pub_tx.reply);
		if(g_reply_int==100){
			sprintf(acErrMsg,"客户凭证查找完毕!");
			WRITEMSG
			break;
		}else if(g_reply_int){
			sprintf(acErrMsg,"查找凭证错误!");
			WRITEMSG
			goto ErrExit;
		}
		stnum=atoi(g_note_cheq_mst.beg_note_no);
		endnum=atoi(g_note_cheq_mst.end_note_no);
		if(pzno<stnum || pzno>endnum){
			flag[0]='0';/**不存在此支票号**/
		}else{
	vtcp_log("[%s][%d]flag==[%c]",__FILE__,__LINE__,flag[0]);
			target=pzno-stnum;
			if(g_note_cheq_mst.book_sts[target]=='5'){
				flag[0]='5';/**该支票已经支付过**/
			}else if(g_note_cheq_mst.book_sts[target]=='1'){
				flag[0]='1';/**正常未使用**/
			}else if(g_note_cheq_mst.book_sts[target]=='0'){
				flag[0]='0';/**非本帐户支票**/
			}else{
				flag[0]='2';/***其他状态***/
			}
	vtcp_log("[%s][%d]flag==[%c]",__FILE__,__LINE__,flag[0]);
			goto OkExit;
		}
	}
	vtcp_log("[%s][%d]flag==[%c]",__FILE__,__LINE__,flag[0]);
	Note_cheq_mst_Clo_Sel();
	vtcp_log("[%s][%d]flag==[%s]",__FILE__,__LINE__,flag);
OkExit:
	set_zd_data("0720",flag);
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

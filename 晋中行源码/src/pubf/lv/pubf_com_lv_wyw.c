/*****���֧Ʊ״̬������ƾ֤��***/
/**		sCrid	Ʊ������;     ***/
/**		sCrstno ��ʼ����;     ***/
/**		sCrendno ��ֹ����;    ***/
/**		sActno   �ͻ��˺�;    ***/
/**		hcode    ������־     ***/
/**             	0  �������� */
/**                 1  ȡ������ */
/**    wangyongwei 20070308    **/
/********************************/ 
#include <string.h>
#include <stdio.h>
#include "lv_define.h"
#define  EXTERN
#include "public.h"
#include  "lv_fd123.h"
#include "mdm_ac_rel_c.h"
#include  "note_parm_c.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"

int iLvCheckPzStatus(sCrid,sCrstno,sCrendno,sActno,hcode)
	char *sCrid;
	char *sCrstno;
	char *sCrendno;
	char *sActno;
	char hcode;
{
	int i_Crstno, i_Crendno,i_Pzno,target;
	char s_Crid[4];
	char s_Crstno[21];
	char s_Crendno[21];
	char s_Actno[33];
	char flag;
	struct mdm_ac_rel_c    g_mdm_ac_rel;
	struct note_parm_c     g_note_parm;
	struct note_cheq_mst_c g_note_cheq_mst;
	struct note_cheq_hst_c g_note_cheq_hst;
	memset(s_Crid,0,sizeof(s_Crid));
	memset(s_Crstno,0,sizeof(s_Crstno));
	memset(s_Crendno,0,sizeof(s_Crendno));
	memset(s_Actno,0,sizeof(s_Actno));
	memset(&g_mdm_ac_rel,0,sizeof(g_mdm_ac_rel));
	memset(&g_note_parm,0,sizeof(g_note_parm));
	memset(&g_note_cheq_mst,0,sizeof(g_note_cheq_mst));
	memset(&g_note_cheq_hst,0,sizeof(g_note_cheq_hst));

	memcpy(s_Crid,sCrid,sizeof(s_Crid)-1);
	memcpy(s_Crstno,sCrstno,sizeof(s_Crstno)-1);
	memcpy(s_Crendno,sCrendno,sizeof(s_Crendno)-1);
	memcpy(s_Actno,sActno,sizeof(s_Actno)-1);
	vtcp_log("[%s][%d],��ʼ�������ֹ����:s_Crstno[%s],s_Crendno[%s],s_Actno[%s]\n",__FL__,s_Crstno,s_Crendno,s_Actno);
	pub_base_strpack(s_Actno);
	flag=hcode;
	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%.19s' and \
			opn_br_no='%.5s'",s_Actno,g_pub_tx.tx_br_no); 
	if(g_reply_int){
		sprintf(acErrMsg,"���в����ڴ��˺�!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		return(-1);
	}	
	g_reply_int=Note_parm_Sel(g_pub_tx.reply,&g_note_parm,"note_type='%s'",s_Crid);
	if(g_reply_int){
		sprintf(acErrMsg,"�����ڴ���ƾ֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"N001");
		return(-1);
	}
	if(g_note_parm.sell_ind[0]!='Y' || g_note_parm.use_ind[0]=='1'){
		sprintf(acErrMsg,"�ǿͻ�ƾ֤��ƾ֤ͣ��״̬!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"N018");
		return(-1);
	}
	/***����ƾ֤����ͬ��ƾ֤����Ψһ��,��ͨ��ƾ֤�ŶԿͻ�ƾ֤���в�ѯ***/
	i_Crstno=apatoi(s_Crstno,sizeof(s_Crstno)-1);
	i_Crendno=apatoi(s_Crendno,sizeof(s_Crendno)-1);
	i_Pzno=i_Crstno;
	sprintf(s_Crstno,"%0.16d",i_Crstno);
	sprintf(s_Crendno,"%0.16d",i_Crendno);
	vtcp_log("[%s][%d],��ʼ�������ֹ����:s_Crstno[%s],s_Crendno[%s],s_Actno[%s]\n",__FL__,s_Crstno,s_Crendno,s_Actno);
	
	if(flag=='0'){
		vtcp_log("[%s][%d],����������������ƾ֤����!i_Pzno[%d],i_Crendno[%d]\n",				__FL__,i_Pzno,i_Crendno);
		for(;i_Pzno <= i_Crendno;i_Pzno++){
		/* �°���仰��ԭ�����滻Ϊ���ڵ� by LiuHuafeng 20070504 */
		/* ��Ҫ�ǰ�br_noȥ���� */
			g_reply_int=Note_cheq_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seq=%ld and note_type='%s' and to_number(beg_note_no)<=%ld and to_number(end_note_no)>=%ld",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn,s_Crid,i_Pzno,i_Pzno);
			vtcp_log("[%s][%d]-----------",__FILE__,__LINE__);
			if(g_reply_int){
				sprintf(acErrMsg,"�򿪿ͻ���ϸ̨��ʧ��![%d]",g_reply_int);
				WRITEMSG
				return(-1);
			}
			vtcp_log("[%s][%d]----------------",__FILE__,__LINE__);
			while(1){
				vtcp_log("[%s][%d]-------",__FILE__,__LINE__);
				g_reply_int=Note_cheq_mst_Fet_Upd(&g_note_cheq_mst,g_pub_tx.reply);
				if(g_reply_int==100){
					sprintf(acErrMsg,"�Ǳ��ͻ�ƾ֤![%s][%d][%s]",s_Crid,i_Pzno,s_Actno);
					WRITEMSG
					strcpy(g_pub_tx.reply,"N003");
					return(-1);
				}
				vtcp_log("[%s][%d]------",__FILE__,__LINE__);
				/**target=i_Pzno - apatoi(g_note_cheq_mst.beg_note_no,sizeof(g_note_cheq_mst.beg_note_no)-1);**/
				target=i_Pzno - atoi(g_note_cheq_mst.beg_note_no);
vtcp_log("[%s][%d]--------------------",__FILE__,__LINE__);
				if(g_note_cheq_mst.book_sts[target]=='0'){
					continue;
				}else if(g_note_cheq_mst.book_sts[target]=='1'){
					g_note_cheq_mst.book_sts[target]='5';
					break;
				}else{
					sprintf(acErrMsg,"ƾ֤״̬����!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"N022");
					return(-1);
				}
vtcp_log("[%s][%d]--------------------",__FILE__,__LINE__);
				vtcp_log("[%s][%d]����ִ��whileѭ��!",__FL__);
			}
vtcp_log("[%s][%d]--------------------",__FILE__,__LINE__);
			g_reply_int=Note_cheq_mst_Upd_Upd(g_note_cheq_mst,g_pub_tx.reply);
			if(g_reply_int){
				sprintf(acErrMsg,"���¿ͻ�ƾ̨֤��ʧ��!");
				WRITEMSG
				return(-1);
			}
			vtcp_log("[%s][%d]����ִ��forѭ��!",__FL__);
		}				
		vtcp_log("[%s][%d]����ƾ֤�����!",__FL__);
		Note_cheq_mst_Clo_Upd();
		g_note_cheq_hst.ac_id=g_mdm_ac_rel.ac_id;
		g_note_cheq_hst.ac_seq=g_mdm_ac_rel.ac_seqn;
		memcpy(g_note_cheq_hst.note_type,s_Crid,sizeof(s_Crid)-1);
		memcpy(g_note_cheq_hst.tx_br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.tx_br_no));
		memcpy(g_note_cheq_hst.beg_note_no,s_Crstno,sizeof(g_note_cheq_hst.beg_note_no)-1);
		memcpy(g_note_cheq_hst.end_note_no,s_Crendno,sizeof(g_note_cheq_hst.end_note_no)-1);
		vtcp_log("[%s][%d],beg_no[%s],end_no[%s],note_type[%s]\n",__FL__,g_note_cheq_hst.beg_note_no,g_note_cheq_hst.end_note_no,g_note_cheq_hst.note_type);
		g_note_cheq_hst.use_ind[0]='Y';
		g_note_cheq_hst.cnt=i_Crendno - i_Crstno +1;
		g_note_cheq_hst.tx_date=g_pub_tx.tx_date;
		g_note_cheq_hst.tx_time=g_pub_tx.tx_time;
		g_note_cheq_hst.trace_no=g_pub_tx.trace_no;
		g_note_cheq_hst.trace_cnt=g_pub_tx.trace_cnt;
		memcpy(g_note_cheq_hst.tel,g_pub_tx.tel,sizeof(g_pub_tx.tel));
		memcpy(g_note_cheq_hst.auth,g_pub_tx.auth,sizeof(g_pub_tx.auth));
		memcpy(g_note_cheq_hst.chk,g_pub_tx.chk,sizeof(g_pub_tx.chk));
		vtcp_log("[%s][%d],tx_br_no[%.5s],tel[%.5s],auth[%.5s]ac_id[%d]ac_seq[%d]\n",__FL__,g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.auth,g_note_cheq_hst.ac_id,g_note_cheq_hst.ac_seq);
		g_reply_int=Note_cheq_hst_Ins(g_note_cheq_hst,g_pub_tx.reply);
		if(g_reply_int){

			sprintf(acErrMsg,"����ͻ�̨����ϸʧ��!");
			WRITEMSG
			return(-1);
		}
	}else{
		vtcp_log("[%s][%d]��ʼִ�з�����!",__FL__);
		for(;i_Pzno <= i_Crendno;i_Pzno++){	
			g_reply_int=Note_cheq_mst_Dec_Upd(g_pub_tx.reply,\
				"%ld>=to_number(beg_note_no) and %ld<=to_number(end_note_no) \
				and note_type='%s' and ac_id=%ld and ac_seq=%ld",i_Pzno,i_Pzno,\
				s_Crid,g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
			while(1){
				g_reply_int=Note_cheq_mst_Fet_Upd(&g_note_cheq_mst,g_pub_tx.reply);
				if(g_reply_int==100){
					sprintf(acErrMsg,"�Ǳ��ͻ�ƾ֤![%s][%d][%s]",s_Crid,i_Pzno,s_Actno);
					WRITEMSG
					strcpy(g_pub_tx.reply,"N003");
					return(-1);
				}
				target=i_Pzno - apatoi(g_note_cheq_mst.beg_note_no,sizeof(g_note_cheq_mst.beg_note_no)-1);
				if(g_note_cheq_mst.book_sts[target]=='0'){
                   	continue;
				}else if(g_note_cheq_mst.book_sts[target]=='5'){
					g_note_cheq_mst.book_sts[target]='1';
					break;
				}else{
					sprintf(acErrMsg,"ƾ֤״̬����!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"N022");
					return(-1);
				}
				vtcp_log("[%s][%d]����ִ��whileѭ��!",__FL__);
			}

			g_reply_int=Note_cheq_mst_Upd_Upd(g_note_cheq_mst,g_pub_tx.reply);
			if(g_reply_int){
				sprintf(acErrMsg,"���¿ͻ�ƾ̨֤��ʧ��!");
				WRITEMSG
				return(-1);
			}
			vtcp_log("[%s][%d]����ִ��forѭ��!",__FL__);
		}
		vtcp_log("[%s][%d]����ƾ֤�����!",__FL__);
		Note_cheq_mst_Clo_Upd();
	}							
	return(0);
}

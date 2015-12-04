/*************************************************
* �� �� ��:  spJBBB.c
* ���������� С�����ʴ�ӡ
* ��    ��:  ligl
* ������ڣ� 2006��8��29��
* �޸ļ�¼�� 
*   ��    ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"
#include "lv_fd123.h"
#include "hv_define.h"
#include "find_debug.h"
#include "lv_pldfr_c.h" /*add by chenggx 2011-6-14*/
#include "lv_pldsr_c.h" /*add by chenggx 2011-6-14*/
spJ912()
{
	int ret=0,i=0;
	
	struct lv_pkgreg_c lv_pkgreg;
	struct lv_pldfr_c  lv_pldfr;
	struct lv_pldsr_c  lv_pldsr;
	struct hv_fzf_c hv_fzf;
	struct com_branch_c com_branch;
	LV_FD123_AREA lv_fd123;
	HV_FD123_AREA fd123;
	char cBr_no[6];/*������*/
	char cTel[5];  /*����Ա��*/
	char cTx_date[9];/*��������*/
	char cOrderno[9];/*˳���*/
	char cOrbrno[25];
	char cNextOrderno[9];/*��һ��˳���*/
	char cMinno[9]; /*��С���*/
	char cTemno[17]; /*��ʱ���*/
	char cTxstat[2];/*����״̬*/
	char cPkgno[4];/*��Ʊ����*/
	char cVtype[4];/*ƾ֤����*/
	char cVocnum[17];/*ƾ֤����*/
	char cNextVocnum[17];/*��һ��ƾ֤����*/
	char wherelist[1024];  /**��ѯ����**/
	char cZf_type[2];
	char cNotpay_content[1024];
	char cPayqsno[25];

	memset(cNotpay_content,0x00,sizeof(cNotpay_content));
	memset(&lv_fd123, ' ',sizeof(lv_fd123));
	memset(&fd123, ' ',sizeof(fd123));
	memset(cBr_no     ,0, sizeof(cBr_no));
	memset(cTel     ,0, sizeof(cTel));
	memset(cTx_date   ,0, sizeof(cTx_date));	
	memset(cVtype     ,0, sizeof(cVtype));
	memset(cTxstat    ,0, sizeof(cTxstat));
	memset(cPkgno     ,0, sizeof(cPkgno));
	memset(cVocnum    ,0, sizeof(cVocnum));
	memset(cOrderno   ,0, sizeof(cOrderno));
	memset(cOrbrno    ,0, sizeof(cOrbrno));
	memset(cPayqsno, 0 , sizeof(cPayqsno));
	memset(cTemno     ,0,sizeof(cTemno));
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(&lv_pldfr,0,sizeof(lv_pldfr));
	memset(&lv_pldsr,0,sizeof(lv_pldsr));
	memset(&hv_fzf,0x0,sizeof(struct hv_fzf_c));
	memset(cNextOrderno,0,sizeof(cNextOrderno));
	memset(cNextVocnum    ,0, sizeof(cNextVocnum));
	memset(&com_branch,0x00,sizeof(struct com_branch_c));
	memset(cZf_type,0,sizeof(cZf_type));
	vtcp_log("spJBBB��ʼ��[%s][%d]",__FILE__,__LINE__);
	vtcp_log("spJBBB��ʼ��cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
	
	/**��ʼ����**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0070",cTel);
	get_zd_data("0440",cTx_date);
	/*get_fd_data("0290",cTemno);*/
	get_fd_data("0580",cTemno);
	vtcp_log("0580=[%s][%s][%d]",cTemno,__FILE__,__LINE__);
	get_zd_data("0230",cPkgno);
	get_zd_data("0690",cTxstat);
	get_zd_data("0900",cVtype);
	get_zd_data("0600",cVocnum);
	get_zd_data("0720",cZf_type);
	get_zd_data("0300",cPayqsno);
	get_zd_data("0310",cOrbrno);
	vtcp_log("0310=[%s][%s][%d]",cOrbrno,__FILE__,__LINE__);
	pub_base_strpack(cTemno);
	i=strlen(cTemno);
		vtcp_log("cTemno=[%s][%s][%d]",cTemno,__FILE__,__LINE__);
	/*MEMCPY_DEBUG(cOrderno+8-i,cTemno,sizeof(cTemno)-1);*/
	MEMCPY_DEBUG(cOrderno,cTemno,8);
		vtcp_log("cOrbrno=[%s][%s][%d]",cOrbrno,__FILE__,__LINE__);
	MEMCPY_DEBUG(cNextVocnum,cVocnum,sizeof(cNextVocnum)-1);
	vtcp_log("AP MSG: ��ʼС������ƾ֤��ӡ����![%s][%d]",__FILE__,__LINE__);
	vtcp_log("cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
	vtcp_log("cOrbrno=[%s][%s][%d]",cOrbrno,__FILE__,__LINE__);
	pub_base_strpack(cOrbrno);
	/***********���ƾ֤*************/
	if(cZf_type[0]=='0')
	{
		ret=pub_com_ChkNoteUse(cVtype);/*���ƾ֤�����Ƿ����*/
		if( ret )
		{
			sprintf(acErrMsg,"ƾ֤����ͣ��״̬!");
			WRITEMSG
			goto ErrExit;
		} 
		ret=pub_com_NoteUse(0,0,cVtype,cVocnum,cVocnum,cTel);/*ƾ֤���Ŵ������׹�����ʹ��ƾ֤��*/
		if( ret )
		{
			sprintf(acErrMsg,"��Աʹ��ƾ֤����!");
			WRITEMSG
			goto ErrExit;
		}
		/**�õ���һ��ƾ֤��**/
		ret=pub_com_GetNoteMin(cVtype,cNextVocnum,cTel);
		if (ret)
		{
			MEMCPY_DEBUG(cNextVocnum,"û��ƾ֤��",sizeof(cNextVocnum)-1);
			WRITEMSG
			goto ErrExit;
		}
		/***********������*************/
		vtcp_log("cNextVocnum=[%s][%s][%d]",cNextVocnum,__FILE__,__LINE__);
		vtcp_log("AP MSG: �õ�Ҫ��ӡ�ļ�¼![%s][%d]",__FILE__,__LINE__);
		vtcp_log("cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
		vtcp_log("cOrbrno=[%s][%s][%d]",cOrbrno,__FILE__,__LINE__);
	}
	if(cZf_type[0]=='0')
	{
		/*ret =Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, "br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2'",cBr_no,cTx_date,cOrderno);
		 in_date ��Ϊ wt_date*/
		ret =Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, "br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s'",cBr_no,cTx_date,cOrderno,cOrbrno);
		vtcp_log("spJBBB ��ʼ��[%s][%d]",__FILE__,__LINE__);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"�޴�С��֧������");
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"��ѯС��֧�����׻�����Ϣ����[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}		
		vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
		/*���´��֧������*/
		vtcp_log("����С��֧������[%s][%d]",__FILE__,__LINE__);
		/***�������Ƿ�Ϊ��������****/
		ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",cBr_no);
		if(com_branch.br_type[0]=='1')
		{
			/****Modified by SSH,2007.5.16,sqprtflag->qsprtflag****/
			/**ret = sql_execute("update lv_pkgreg set qsprtflag= '1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2'",cBr_no,cTx_date,cOrderno);
			in_date ��Ϊ wt_date*/
			ret = sql_execute("update lv_pkgreg set qsprtflag= '1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s'",cBr_no,cTx_date,cOrderno,cOrbrno);
			if (ret)
			{
				sprintf(acErrMsg,"����С��֧�����׻�����Ϣ����[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		else
		{
			/**ret = sql_execute("update lv_pkgreg set brprtflag= '1',qsprtflag='1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2'",cBr_no,cTx_date,cOrderno);
			in_date ��Ϊ wt_date**/
			ret = sql_execute("update lv_pkgreg set brprtflag= '1',qsprtflag='1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s'",cBr_no,cTx_date,cOrderno,cOrbrno);
			if (ret)
			{
				sprintf(acErrMsg,"����С��֧�����׻�����Ϣ����[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		/**��lv_fd123������**/
		/*Package���*/
		MEMCPY_DEBUG(lv_fd123.pkgno,lv_pkgreg.pkgno,sizeof(lv_fd123.pkgno));
		vtcp_log("[%s][%d]pkgno=[%s]",__FILE__,__LINE__,lv_pkgreg.pkgno); 	
		/**�����к� ԭ�����к� */
		MEMCPY_DEBUG(lv_fd123.or_br_no        ,lv_pkgreg.or_br_no        ,sizeof(lv_fd123.or_br_no        ));
		vtcp_log("[%s][%d]or_br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.or_br_no);
		
		/**�����˿�����**/
		MEMCPY_DEBUG(lv_fd123.pay_opn_br_no   ,lv_pkgreg.pay_opn_br_no   ,sizeof(lv_fd123.pay_opn_br_no   ));
		vtcp_log("[%s][%d]pay_opn_br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_opn_br_no);
		/**�����������к�**/		
		MEMCPY_DEBUG(lv_fd123.pay_qs_no	,lv_pkgreg.pay_qs_no,sizeof(lv_fd123.pay_qs_no));	
		/**�������ʺ�(ԭ�տ����ʺ�)**/
		MEMCPY_DEBUG(lv_fd123.pay_ac_no       ,lv_pkgreg.pay_ac_no       ,sizeof(lv_fd123.pay_ac_no       ));
		vtcp_log("[%s][%d]pay_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_ac_no);
		MEMCPY_DEBUG(lv_fd123.txnum, lv_pkgreg.txnum,sizeof(lv_fd123.txnum));
		vtcp_log("[%s][%d]pay_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.txnum);
		/**����������(ԭ�տ�������)**/
		MEMCPY_DEBUG(lv_fd123.pay_name        ,lv_pkgreg.pay_name        ,sizeof(lv_fd123.pay_name        ));
		vtcp_log("[%s][%d]pay_name=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_name);
  	
		/**�����˵�ַ(ԭ�տ��˵�ַ)**/
		MEMCPY_DEBUG(lv_fd123.pay_addr        ,lv_pkgreg.pay_addr        ,sizeof(lv_fd123.pay_addr        ));
		vtcp_log("[%s][%d]pay_addr=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_addr);
  	
		/**���������к�**/
		MEMCPY_DEBUG(lv_fd123.cash_qs_no      ,lv_pkgreg.cash_qs_no      ,sizeof(lv_fd123.cash_qs_no      ));
		vtcp_log("[%s][%d]cash_qs_no=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_qs_no);
  	
		/**�����к�(ԭ�����к�)**/
		MEMCPY_DEBUG(lv_fd123.ac_br_no        ,lv_pkgreg.ac_br_no        ,sizeof(lv_fd123.ac_br_no        ));
		vtcp_log("[%s][%d]ac_br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.ac_br_no);
		/**�տ��˿����к�**/
		MEMCPY_DEBUG(lv_fd123.cash_opn_br_no  ,lv_pkgreg.cash_opn_br_no  ,sizeof(lv_fd123.cash_opn_br_no  ));
		vtcp_log("[%s][%d]cash_opn_br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_opn_br_no);
		/**�տ����ʺ�,ԭ�������ʺ�**/
		MEMCPY_DEBUG(lv_fd123.cash_ac_no      ,lv_pkgreg.cash_ac_no      ,sizeof(lv_fd123.cash_ac_no      ));
		vtcp_log("[%s][%d]cash_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_ac_no);
  	
		/**�տ�������,ԭ����������**/
		MEMCPY_DEBUG(lv_fd123.cash_name       ,lv_pkgreg.cash_name       ,sizeof(lv_fd123.cash_name       ));
		vtcp_log("[%s][%d]cash_name=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_name);
		
		/**�տ��˵�ַ**/
		MEMCPY_DEBUG(lv_fd123.cash_addr       ,lv_pkgreg.cash_addr       ,sizeof(lv_fd123.cash_addr       ));
		vtcp_log("[%s][%d]cash_addr=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_addr);
		
		/**ҵ������**/	
		MEMCPY_DEBUG(lv_fd123.yw_type         ,lv_pkgreg.lv_yw_ind         ,sizeof(lv_fd123.yw_type         ));
		vtcp_log("[%s][%d]yw_type=[%s]",__FILE__,__LINE__,lv_pkgreg.lv_yw_ind);
		
		/*������� ����֧���������*/
		MEMCPY_DEBUG(lv_fd123.orderno         ,lv_pkgreg.orderno         ,sizeof(lv_fd123.orderno         ));
		vtcp_log("[%s][%d]orderno=[%s]",__FILE__,__LINE__,lv_pkgreg.orderno);
		
		/**����**/
		MEMCPY_DEBUG(lv_fd123.brf             ,lv_pkgreg.lv_brf          ,sizeof(lv_fd123.brf             ));
		vtcp_log("[%s][%d]brf=[%s]",__FILE__,__LINE__,lv_pkgreg.lv_brf);
		
		/**ԭ֧���������**/
		MEMCPY_DEBUG(lv_fd123.o_orderno       ,lv_pkgreg.o_orderno       ,sizeof(lv_fd123.o_orderno       ));
		/**�����ѽ��*/
		/*dot_dbl2str(&lv_pkgreg.chrg_amt,0,sizeof(lv_fd123.chrg_amt),0,&lv_fd123.chrg_amt);*/
		dot_dbl2str(lv_pkgreg.chrg_amt,sizeof(lv_fd123.chrg_amt),0,lv_fd123.chrg_amt);
		vtcp_log("[%s][%d]chrg_amt=[%s]",__FILE__,__LINE__,lv_pkgreg.chrg_amt);
  	
		/**ί������*/
		sprintf(lv_fd123.wt_date,"%08d",lv_pkgreg.wt_date);
		vtcp_log("[%s][%d] lv_pkgreg.wt_date = [%d] wt_date=[%s]",__FILE__,__LINE__,lv_pkgreg.wt_date,lv_fd123.wt_date);
		/**��ִ״̬**/	
		MEMCPY_DEBUG(lv_fd123.rcp_sts         , lv_pkgreg.rcpstat          ,sizeof( lv_fd123.rcp_sts ));
		/**���ʰ���**/
		MEMCPY_DEBUG(lv_fd123.pack_id         , lv_pkgreg.packid          ,sizeof( lv_fd123.pack_id ));
		vtcp_log("[%s][%d]packid=[%s]",__FILE__,__LINE__,lv_pkgreg.packid);
		
		lv_fd123.operlevel[0]=lv_pkgreg.operlevel[0];
		lv_fd123.lv_sts[0]=lv_pkgreg.lv_sts[0];
		set_zd_double("0390",lv_pkgreg.tx_amt);   /* ���׽�� */
		
		/*beg add by chenggx 2011-6-14 20:25:55*/
		vtcp_log("[%s][%d]pkgno=[%s],txnum=[%s]",__FILE__,__LINE__,lv_pkgreg.pkgno,lv_pkgreg.txnum);
		vtcp_log("[%s][%d]PKGNO_TYXX=[%s],TXNUM_DF=[%s],TXNUM_DS=[%s]",__FILE__,__LINE__,PKGNO_TYXX,TXNUM_DF,TXNUM_DS);
		if(memcmp(lv_pkgreg.pkgno,PKGNO_TYXX,3)==0 && memcmp(lv_pkgreg.txnum,TXNUM_DF,5)==0){/*����*/
			vtcp_log("[%s][%d] ���� �ҿ����� ������ ^_^O(��_��)O����~",__FILE__,__LINE__);
			ret = Lv_pldfr_Sel(g_pub_tx.reply,&lv_pldfr, " packid='%s' and orderno='%s' and wt_date=%ld and tx_mx_no='00000000' ",lv_pkgreg.packid,lv_pkgreg.orderno,lv_pkgreg.wt_date);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"��ѯ����������ʧ��!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "9999" );
				goto	ErrExit;
			}
			memcpy(lv_fd123.cash_ac_no      ,lv_pldfr.pay_ac_no      ,strlen(lv_pldfr.pay_ac_no  ));
			vtcp_log("[%s][%d]lv_fd123.cash_ac_no=[%s] ",__FL__,lv_fd123.cash_ac_no );
			memcpy(lv_fd123.cash_name       ,lv_pldfr.pay_name       ,strlen(lv_pldfr.pay_name   ));
			vtcp_log("[%s][%d]lv_fd123.cash_name=[%s] ",__FL__,lv_fd123.cash_name );
			set_zd_double("0390",lv_pldfr.totamt);   /* ���׽�� */
		}else if(memcmp(lv_pkgreg.pkgno,PKGNO_TYXX,3)==0 && memcmp(lv_pkgreg.txnum,TXNUM_DS,5)==0){/*����*/
			vtcp_log("[%s][%d] ���� �ҿ����� ������ ^_^O(��_��)O����~",__FILE__,__LINE__);
			ret = Lv_pldsr_Sel(g_pub_tx.reply,&lv_pldsr, " packid='%s' and orderno='%s' and wt_date=%ld and tx_mx_no='00000000' ",lv_pkgreg.packid,lv_pkgreg.orderno,lv_pkgreg.wt_date);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"��ѯ�������ձ�ʧ��!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "9999" );
				goto	ErrExit;
			}
			memcpy(lv_fd123.cash_ac_no      ,lv_pldsr.cash_ac_no      ,strlen(lv_pldsr.cash_ac_no  ));
			vtcp_log("[%s][%d]lv_fd123.cash_ac_no=[%s] ",__FL__,lv_fd123.cash_ac_no );
			memcpy(lv_fd123.cash_name       ,lv_pldsr.cash_name       ,strlen(lv_pldsr.cash_name   ));
			vtcp_log("[%s][%d]lv_fd123.cash_name=[%s] ",__FL__,lv_fd123.cash_name );
			set_zd_double("0390",lv_pldsr.totamt);   /* ���׽�� */
		}
		/*end add by chenggx 2011-6-14 20:25:55*/
	
		pub_base_full_space((char*)&lv_fd123,sizeof(lv_fd123));
		set_zd_data("1230",(char*)&lv_fd123);
		vtcp_log("lv_fd123=[%s]",(char*)&lv_fd123);
		set_zd_data("0280",cNextOrderno);
		set_zd_data("0961",cNextVocnum);
	}
	else
	{/* С������ */
		/*��Դ��������ݼ���ĿҪ��ѯ��Ŀ�ı��� */
		vtcp_log("%s,%d ---pay_qs_no==[%s],pkgno=[%s]",__FILE__,__LINE__,cPayqsno,cPkgno);
		if(memcmp(cPkgno,"302",3)==0 || memcmp(cPkgno,"301",3)==0 )
		{
			if(memcmp(cPayqsno,HV_QS_BR_NO,12)==0)
			{
				memcpy(cPkgno,"302",3);
			}
		}
		if(memcmp(cPkgno,"302",3)==0)
		{
/****�޸�Ϊwt_date ���ڴ�ӡ��ʱ������ by  jk 20110426
 ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='2'",cBr_no,cTx_date,cO
rderno,HV_QS_BR_NO);
*****/


			ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and wt_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);
		}
		else if(memcmp(cPkgno,"301",3)==0)
		{
			ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and wt_date='%s' and orderno='%s' and pay_qs_no!='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);
		}
		else
		{
			ret =Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf, "br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and otype='2'",cBr_no,cTx_date,cOrderno);
		}
		vtcp_log("spJBBB ��ʼ��[%s][%d]",__FILE__,__LINE__);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"�޴�С��֧������");
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"��ѯС��֧�����׻�����Ϣ����[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
		/*���´��֧������*/
		vtcp_log("����С��֧������[%s][%d]",__FILE__,__LINE__);
		/***�������Ƿ�Ϊ��������****/
		vtcp_log("[%s][%d]brno=[%s]",__FILE__,__LINE__,cBr_no);
		ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",cBr_no);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		if(com_branch.br_type[0]=='1')
		{
			if(memcmp(cPkgno,"302",3)==0)
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);
			}
			else if(memcmp(cPkgno,"301",3)==0)
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no!='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);
			}
			else
			{
		   	ret = sql_execute("update hv_fzf set hv_prt_ind='1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and otype='2'",cBr_no,cTx_date,cOrderno);
			}
			if (ret)
			{
				sprintf(acErrMsg,"����С��֧�����׻�����Ϣ����[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		else
		{
			if(memcmp(cPkgno,"302",3)==0)
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);	
			}
			else if(memcmp(cPkgno,"301",3)==0)
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no!='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);	
			}
			else
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and otype='2'",cBr_no,cTx_date,cOrderno);
			}
			if (ret)
			{
				sprintf(acErrMsg,"����С��֧�����׻�����Ϣ����[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		
		/**��lv_fd123������**/
		/**cmtno����**/
		MEMCPY_DEBUG(fd123.cmtno	,hv_fzf.cmtno		,sizeof(fd123.cmtno));		
		/**o_cmtno**/		
		MEMCPY_DEBUG(fd123.o_cmtno	,hv_fzf.o_cmtno	,sizeof(fd123.cmtno));		
		/**�����к� ԭ�����к� **/		
		MEMCPY_DEBUG(fd123.or_br_no	,hv_fzf.or_br_no,sizeof(fd123.or_br_no));	
		/**�����������к�**/		
		MEMCPY_DEBUG(fd123.pay_qs_no	,hv_fzf.pay_qs_no,sizeof(fd123.pay_qs_no));	
		/**�������Ĵ���**/		
		MEMCPY_DEBUG(fd123.sendco		,hv_fzf.sendco	,sizeof(fd123.sendco));
		/**���������к�**/				
		MEMCPY_DEBUG(fd123.cash_qs_no	,hv_fzf.cash_qs_no,sizeof(fd123.cash_qs_no));		
		/**�����к�(ԭ�����к�)**/	
		MEMCPY_DEBUG(fd123.ac_br_no	,hv_fzf.ac_br_no,sizeof(fd123.ac_br_no));
		/**������� ����֧���������**/			
		MEMCPY_DEBUG(fd123.orderno	,hv_fzf.orderno	,sizeof(fd123.orderno));	
		/**����**/		
		MEMCPY_DEBUG(fd123.brf		,hv_fzf.hv_brf	,sizeof(fd123.brf));	
		/**ԭ֧���������**/			
		MEMCPY_DEBUG(fd123.o_orderno	,hv_fzf.o_orderno,sizeof(fd123.o_orderno));	
		/**��֧������״̬**/		
		fd123.proc_sts[0] = hv_fzf.hv_fzf_sts[0];			
		/**������ʾ**/						
		fd123.lw_ind[0]	= hv_fzf.lw_ind[0];
		/**Ӧ��״̬**/					
		fd123.resp[0]	= hv_fzf.res_sts[0];
		/**Ӧ�����**/
		MEMCPY_DEBUG(fd123.cr_tx_num,hv_fzf.res_orderno,sizeof(hv_fzf.res_orderno)-1);
		/**Ӧ���к�**/
		MEMCPY_DEBUG(fd123.cr_br_no,hv_fzf.res_br_no,sizeof(hv_fzf.res_br_no)-1);
		vtcp_log("[%s][%d]fd123.cr_br_no=[%s]\n",__FILE__,__LINE__,fd123.cr_br_no);
		/**Ӧ������**/
		sprintf(fd123.cr_date,"%8d",hv_fzf.res_date);
		/**ԭ���׽��**/								
		set_zd_double("0390",hv_fzf.o_tx_amt);
		/**Ӧ������**/
		vtcp_log("%s,%d cmtno==='%s'",__FILE__,__LINE__,fd123.cmtno);
		
		if(memcmp(fd123.cmtno,"314",3)==0 || memcmp(fd123.cmtno,"302",3)==0 ||memcmp(fd123.cmtno,"312",3)==0)
		{
			/* �ж�Ҫ��ӡ���� */
			if(memcmp(fd123.cmtno,"302",3)==0)
			{
				vtcp_log("%s,%d pay_qs_no=[%s]",__FILE__,__LINE__,hv_fzf.pay_qs_no);
				if(memcmp(hv_fzf.pay_qs_no,HV_QS_BR_NO,sizeof(hv_fzf.pay_qs_no)-1)==0)
				{
					/* �����������Ǳ��е������ӡ�鸴���� */
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
				}
				else
				{/* ��ӡ��ѯ���� */
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.req_content,sizeof(hv_fzf.res_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
				}
			}
			else
			{
				MEMCPY_DEBUG(cNotpay_content,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);
				deal_half_charcter(cNotpay_content);					
				MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
			}
		}
		else	/**��������**/
		{
			if(memcmp(fd123.cmtno,"301",3)==0)
			{
				vtcp_log("%s,%d pay_qs_no=[%s]",__FILE__,__LINE__,hv_fzf.pay_qs_no);
				if(memcmp(hv_fzf.pay_qs_no,HV_QS_BR_NO,sizeof(hv_fzf.pay_qs_no)-1)==0)
				{
					/* �����������Ǳ��е������ӡ�鸴���� */
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
				}
				else
				{/* ��ӡ��ѯ���� */
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.req_content,sizeof(hv_fzf.res_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
				}
			}
			else
			{
				MEMCPY_DEBUG(cNotpay_content,hv_fzf.req_content,sizeof(hv_fzf.req_content)-1);
				deal_half_charcter(cNotpay_content);					
				MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
			}
		}
  	
		/* by liuyue 20060922*****�����ѯʱ��ԭ��������ԭ�����е�����*****/
		if(!memcmp(hv_fzf.o_or_br_no,hv_fzf.or_br_no,sizeof(hv_fzf.or_br_no)-1))/**�����о���ԭ������,����ѯ���з����Ľ���**/
		{
			fd123.tx_type[0]	 =  'A';
		}
		else 
			fd123.tx_type[0]	=   'B';
  	vtcp_log("%s,%d content=====",__FILE__,__LINE__);
		vtcp_log("[%s]",fd123.notpay_content);
  	
		/**********add end by liuyue ������hv_fzf.tx_type**************/
			
		MEMCPY_DEBUG(fd123.cr_br_no,hv_fzf.res_br_no,sizeof(hv_fzf.res_br_no)-1);
		set_zd_double("0440",hv_fzf.wt_date);	
		set_zd_double("0450",hv_fzf.o_wt_date);	
		pub_base_full_space((char*)&fd123,sizeof(fd123));
		set_zd_data("1230",(char*)&fd123);
		vtcp_log("lv_fd123=[%s]",(char*)&fd123);
		set_zd_data("0280",cNextOrderno);
		set_zd_data("0961",cNextVocnum);
	}
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
	
}
int deal_half_charcter(char *buf)
{
		int i = 0;
		int j = 0;
		int size = 0;
		int index =0;
		
		
		

		printf("[%s]====\n",buf);
		size = strlen(buf);
		
		char cBuf[1024];
		
		memset(cBuf,0x00,sizeof(cBuf));


		for(i=0;i<size;i++)
		{
				if((int)buf[i]<128 &&(int)buf[i+1]>128 && (i+1+index)%2)
				{
						cBuf[j] = buf[i];
						cBuf[j+1]= ' ';
						j = j+2;
						index++;
				}
				else
				{
						cBuf[j] = buf[i];
						cBuf[j] = buf[i];
						j++;
				}
		}

		MEMCPY_DEBUG(buf,cBuf,sizeof(cBuf));
		return 0;
}



/*************************************************
* �� �� ��:  spJAAA.c
* ���������� ������ʴ�ӡ
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
#include "hv_pub.h"
#include "hv_define.h"
#include "hv_addit_c.h"
#include "find_debug.h"
#include "hv_poinfo_c.h"

spJ911()
{
	int ret=0,i=0;
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;
	struct hv_addit_c hv_addit;
	HV_FD123_AREA fd123;
	char cBr_no[6];/*������*/
	char cTel[5];  /*����Ա��*/
	char cTx_date[9];/*��������*/
	char cOrderno[8];/*˳���*/
	char cNextOrderno[9];/*��һ��˳���*/
	char cMinno[9]; /*��С���*/
	char cTemno[9]; /*��ʱ���*/
	char cTxstat[2];/*����״̬*/
	char cCmtno[4];/*��Ʊ����*/
	char cVtype[4];/*ƾ֤����*/
	char cVocnum[17];/*ƾ֤����*/
	char cNextVocnum[17];/*��һ��ƾ֤����*/
	char cZf_type[2];
	char cPrint[2];  /*�ж��״δ�ӡ��־*/
	char wherelist[1024];  /**��ѯ����**/
	char cBuff[341];
	char cTmpcmtno[4];
	char cTmpday[9];
	char cOrbrno[61];
  char cTmpcmtno1[4];

	char cNotpay_content[1024];
	char cFilename[1000];
	memset(cOrbrno,0 , sizeof(cOrbrno));
	memset(cFilename, 0 , sizeof(cFilename));
	memset(cBuff, 0 , sizeof(cBuff));

	memset(&fd123, ' ',sizeof(fd123));
	memset(&hv_addit,0x0,sizeof(struct hv_addit_c));
	memset(cBr_no     ,0, sizeof(cBr_no));
	memset(cPrint     ,0, sizeof(cPrint));
	memset(cTmpcmtno  ,0, sizeof(cTmpcmtno));
	memset(cTmpcmtno1  ,0, sizeof(cTmpcmtno1));
	memset(cTel       ,0, sizeof(cTel));
	memset(cTmpday    ,0, sizeof(cTmpday));
	memset(cTx_date   ,0, sizeof(cTx_date));	
	memset(cVtype     ,0, sizeof(cVtype));
	memset(cTxstat    ,0, sizeof(cTxstat));
	memset(cCmtno     ,0, sizeof(cCmtno));
	memset(cVocnum    ,0, sizeof(cVocnum));
	memset(cOrderno   ,'0', sizeof(cOrderno));
	memset(cTemno     ,0,sizeof(cTemno));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(cNextOrderno,0,sizeof(cNextOrderno));
	memset(cNextVocnum    ,0, sizeof(cNextVocnum));
	memset(cNotpay_content,0x00,sizeof(cNotpay_content));

  memset(cZf_type,0,sizeof(cZf_type));
	vtcp_log("spJAAA��ʼ��[%s][%d]",__FILE__,__LINE__);
	vtcp_log("spJAAA��ʼ��cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
	
	/**��ʼ����**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0070",cTel);
	get_zd_data("0440",cTx_date);
	get_fd_data("0290",cTemno);
	vtcp_log("cOrderno=[%s][%s][%d]",cTemno,__FILE__,__LINE__);
	get_zd_data("0240",cCmtno);
	get_zd_data("0230",cTmpcmtno1);
	get_zd_data("0670",cPrint);
	get_zd_data("0690",cTxstat);
	get_zd_data("0720",cZf_type);
	get_zd_data("0900",cVtype);
	get_zd_data("0600",cVocnum);
	get_zd_data("0250",cOrbrno);
	zip_space(cOrbrno);
	i=strlen(cTemno);
	MEMCPY_DEBUG(cOrderno+8-i,cTemno,sizeof(cTemno)-1);
	MEMCPY_DEBUG(cNextVocnum,cVocnum,sizeof(cNextVocnum)-1);
	vtcp_log("cOrderno=[%d][%s][%d]zftype=[%c]",i,__FILE__,__LINE__,cZf_type[0]);
	vtcp_log("[%s][%d]cCmtno=====================[%s]",__FILE__,__LINE__,cCmtno);
	vtcp_log("AP MSG: ��ʼ�������ƾ֤��ӡ����![%s][%d]",__FILE__,__LINE__);
	/***********���ƾ֤*************/
	if(cZf_type[0]=='0' && memcmp(cTmpcmtno1,"802",3))
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
	}
	if(cZf_type[0]=='0')
	{
vtcp_log("111111111cmtno[%s]",cTmpcmtno1);
	 if(memcmp(cTmpcmtno1,"802",3)==0)
	 	{
	  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='802'",cBr_no,cTx_date,cOrderno);
	 		
	 	}
	 else if(memcmp(cTmpcmtno1,"812",3)==0)
	 	{
	  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='812'",cBr_no,cTx_date,cOrderno);
	 		
	 	}
	 	else if(memcmp(cTmpcmtno1,"814",3)==0)
	 	{
	  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='814'",cBr_no,cTx_date,cOrderno);
	 		
	 	}
	 	else
	 	{
	  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s'",cBr_no,cTx_date,cOrderno,cOrbrno);
	}
		vtcp_log("sp9561 ��ʼ��[%s][%d]",__FILE__,__LINE__);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"�޴˴��֧������");
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}		
		if(cPrint[0]==hv_zf.hv_prt_ind[0])
			{
				sprintf(acErrMsg,"�ñ������״δ�ӡ[%d]cPrint=[%c]hv_zf.hv_prt_ind[0]=[%c]",ret,cPrint[0],hv_zf.hv_prt_ind[0]);
			  WRITEMSG
			  goto ErrExit;
			}
		vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
		/*���´��֧������*/
		vtcp_log("���´��֧������[%s][%d]",__FILE__,__LINE__);
		if(memcmp(cTmpcmtno1,"812",3)==0)
		{
		ret = sql_execute("update hv_zf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='812' ",cBr_no,cTx_date,cOrderno);
	  }
		else if(memcmp(cTmpcmtno1,"814",3)==0)
		{
		ret = sql_execute("update hv_zf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='814' ",cBr_no,cTx_date,cOrderno);
		}	
		else
		{
		ret = sql_execute("update hv_zf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s' ",cBr_no,cTx_date,cOrderno,cOrbrno);
	  }
		if (ret)
		{
			sprintf(acErrMsg,"���´��֧�����׻�����Ϣ����[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		/**��FD123������**/
	  MEMCPY_DEBUG(fd123.cmtno,hv_zf.cmtno,sizeof(fd123.cmtno));
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		MEMCPY_DEBUG(fd123.o_cmtno,hv_zf.o_cmtno,sizeof(fd123.o_cmtno));
		MEMCPY_DEBUG(fd123.pay_qs_no,hv_zf.pay_qs_no,sizeof(fd123.pay_qs_no));
		/**�����к� ԭ�����к� */
		MEMCPY_DEBUG(fd123.or_br_no        ,hv_zf.or_br_no        ,sizeof(fd123.or_br_no        ));
		/**�������Ĵ���**/
		MEMCPY_DEBUG(fd123.sendco          ,hv_zf.sendco          ,sizeof(fd123.sendco          ));
		/**�����˿�����**/
		MEMCPY_DEBUG(fd123.pay_opn_br_no   ,hv_zf.pay_opn_br_no   ,sizeof(fd123.pay_opn_br_no   ));
		/**�������ʺ�(ԭ�տ����ʺ�)**/
		MEMCPY_DEBUG(fd123.pay_ac_no       ,hv_zf.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));
		/**����������(ԭ�տ�������)**/
		MEMCPY_DEBUG(fd123.pay_name        ,hv_zf.pay_name        ,sizeof(fd123.pay_name        ));
		/**�����˵�ַ(ԭ�տ��˵�ַ)**/
		MEMCPY_DEBUG(fd123.pay_addr        ,hv_zf.pay_addr        ,sizeof(fd123.pay_addr        ));
		/**���������к�**/
		MEMCPY_DEBUG(fd123.cash_qs_no      ,hv_zf.cash_qs_no      ,sizeof(fd123.cash_qs_no      ));
		/**�����к�(ԭ�����к�)**/
		MEMCPY_DEBUG(fd123.ac_br_no        ,hv_zf.ac_br_no        ,sizeof(fd123.ac_br_no        ));
		/**�ձ����Ĵ���**/
		MEMCPY_DEBUG(fd123.receco          ,hv_zf.receco          ,sizeof(fd123.receco          ));
		/**�տ��˿����к�**/
		MEMCPY_DEBUG(fd123.cash_opn_br_no  ,hv_zf.cash_opn_br_no  ,sizeof(fd123.cash_opn_br_no  ));
		/**�տ����ʺ�,ԭ�������ʺ�**/
		MEMCPY_DEBUG(fd123.cash_ac_no      ,hv_zf.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));
		/**�տ�������,ԭ����������**/
		MEMCPY_DEBUG(fd123.cash_name       ,hv_zf.cash_name       ,sizeof(fd123.cash_name       ));
		vtcp_log("[%s][%d]cash_name=[%s]",__FILE__,__LINE__,hv_zf.cash_name);
		/**�տ��˵�ַ**/
		MEMCPY_DEBUG(fd123.cash_addr       ,hv_zf.cash_addr       ,sizeof(fd123.cash_addr       ));
		vtcp_log("[%s][%d]cash_addr=[%s]",__FILE__,__LINE__,hv_zf.cash_addr);
		/**ҵ������**/	
		MEMCPY_DEBUG(fd123.yw_type         ,hv_zf.yw_type         ,sizeof(fd123.yw_type         ));
		vtcp_log("[%s][%d]yw_type=[%s]",__FILE__,__LINE__,hv_zf.yw_type);
		/*������� ����֧���������*/
		MEMCPY_DEBUG(fd123.orderno         ,hv_zf.orderno         ,sizeof(fd123.orderno         ));
		vtcp_log("[%s][%d]orderno=[%s]",__FILE__,__LINE__,hv_zf.orderno);
		/**����**/
		MEMCPY_DEBUG(fd123.brf             ,hv_zf.hv_brf          ,sizeof(fd123.brf             ));
		vtcp_log("[%s][%d]hv_brf=[%s]",__FILE__,__LINE__,hv_zf.hv_brf);
		/**ԭ֧���������**/
		MEMCPY_DEBUG(fd123.o_orderno       ,hv_zf.o_orderno       ,sizeof(fd123.o_orderno       ));
		/** add by wangyongwei 20071012 ȡ��Ʊ���봫��ǰ̨**/
		if(!memcmp(hv_zf.cmtno,"122",3) || !memcmp(hv_zf.cmtno,"123",3)||!memcmp(hv_zf.cmtno,"124",3)){
					char tmp_wtdate[9];
					char tmp_tagdata[1024];
					char sign_brno[13];
					struct hv_poinfo_c cHv_poinfo;
					memset(&cHv_poinfo,0,sizeof(cHv_poinfo));
					memset(tmp_wtdate,0,sizeof(tmp_wtdate));
					memset(tmp_tagdata,0,sizeof(tmp_tagdata));
					memset(sign_brno,0,sizeof(sign_brno));
					sprintf(tmp_wtdate,"%d",hv_zf.wt_date);
					vtcp_log("[%s][%d]******1",__FILE__,__LINE__);
				g_reply_int=iHvGetAddit(tmp_wtdate,tmp_wtdate,hv_zf.addid,"000",tmp_tagdata,0);
					vtcp_log("[%s][%d]******2",__FILE__,__LINE__);
				if(g_reply_int){
						sprintf(acErrMsg,"ȡ��Ʊ����ʧ��!");
						set_zd_data(DC_GET_MSG,acErrMsg);
						strcpy(g_pub_tx.reply,"Z999");
						goto ErrExit;	
				}	
				memcpy(fd123.o_pack_date,tmp_tagdata+12,8);	/*Ʊ������*/
				memcpy(fd123.po_no,tmp_tagdata+20,8);	/*Ʊ�ݺ���*/
				GetOr_br_noByBr_no(g_pub_tx.tx_br_no,sign_brno);	
					vtcp_log("[%s][%d]******3",__FILE__,__LINE__);
				/***g_reply_int=Hv_poinfo_Sel(g_pub_tx.reply,&cHv_poinfo,"pay_br_no='%s' and po_no='%.8s'",sign_brno,fd123.po_no);
				if(g_reply_int=100)
				{
					
				}
				else if(g_reply_int){
						sprintf(acErrMsg,"δ�ҵ���Ʊ��¼!");
						set_zd_data(DC_GET_MSG,acErrMsg);
						strcpy(g_pub_tx.reply,"Z999");
						goto ErrExit;	
				}***/
					vtcp_log("[%s][%d]******3",__FILE__,__LINE__);
				if(!memcmp(hv_zf.cmtno,"124",3) &&(cHv_poinfo.po_sts[0]=='G'||cHv_poinfo.l_po_sts[0]=='G')){
						fd123.actype[0]='1'; /**�жϻ�Ʊ�Ƿ�����**/
						char cAcno[33];
						memset(cAcno,0,sizeof(cAcno));
						pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_HPTH,cAcno);
						memcpy(fd123.pay_ac_no,cAcno,32);
						strcpy(fd123.pay_name,"��Ʊ����ר���˺�");
				}
				vtcp_log("%s,%d,��Ʊ����[%.8s],ǩ������[%.8s],�������˺�[%.32s],����������[%.60s]\n",__FILE__,__LINE__,fd123.po_no,fd123.o_pack_date,fd123.pay_ac_no,fd123.pay_name);
		}
		/** add by wangyongwei 20071012 ȡ��Ʊ���봫��ǰ̨**/
		fd123.operlevel[0]=hv_zf.operlevel[0];
		fd123.proc_sts[0]=hv_zf.hv_sts[0];
		set_zd_double("0390",hv_zf.tx_amt);   /* ���׽�� */
		set_zd_data("0890",hv_zf.note_type);
		set_zd_data("0580",hv_zf.note_no);
		set_zd_long("0450",hv_zf.trace_no);
		/*���´��֧������*/
		pub_base_full_space((char*)&fd123,sizeof(fd123));
		set_zd_data("1230",(char*)&fd123);
		vtcp_log("fd123=[%s]",(char*)&fd123);
		vtcp_log("cNextVocnum=[%s][%s][%d]",cNextVocnum,__FILE__,__LINE__);
		set_zd_data("0961",cNextVocnum);
		}
		else  
		{
			vtcp_log("%s,%d cmtno===[%s]",__FILE__,__LINE__,cCmtno);
			if(memcmp(cCmtno,"302",3)==0)
			{
				ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and tx_date='%s' and orderno='%s' and otype='1' and pay_qs_no='%s' and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO,cTx_date,cTx_date);
			}
			else if(memcmp(cCmtno,"301",3)==0)
			{
			 	ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and tx_date='%s' and orderno='%s' and otype='1' and pay_qs_no!='%s' and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO,cTx_date,cTx_date);
			}
			else
			{ 
				ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf, "br_no='%s' and tx_date='%s' and orderno='%s' and (lw_ind='2' or cmtno='301' or cmtno='302') and otype='1' ",cBr_no,cTx_date,cOrderno);
			}
			vtcp_log("sp9561 ��ʼ��[%s][%d]",__FILE__,__LINE__);
			vtcp_log("[%s][%d]",__FILE__,__LINE__);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�޴˴��֧������");
				WRITEMSG
				goto	ErrExit;
			}
			else if ( ret )
			{
				sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}		
			vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
			/*���´��֧������*/
			vtcp_log("���´��֧������[%s][%d]",__FILE__,__LINE__);
			 if(memcmp(cCmtno,"302",3)==0)
			{
					ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='1'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);	
			}
			else if(memcmp(cCmtno,"301",3)==0)
			{
					ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no!='%s' and otype='1'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);	
			}
			else
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and (lw_ind='2' or cmtno='301' or cmtno='302') and otype='1'",cBr_no,cTx_date,cOrderno);
			}
			if (ret)
			{
				sprintf(acErrMsg,"���´��֧�����׻�����Ϣ����[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			/**��FD123������**/
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
			set_zd_data("0965",hv_fzf.hv_fzf_sts);
			vtcp_log("[%s][%d]proc_sts===[%c]",__FILE__,__LINE__,fd123.proc_sts[0]);	
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
			sprintf(cTmpday,"%8d",hv_fzf.res_date);
			MEMCPY_DEBUG(fd123.cr_date,cTmpday,sizeof(cTmpday)-1);
			/**ԭ���׽��**/								
			set_zd_double("0390",hv_fzf.o_tx_amt);	
			if(memcmp(hv_fzf.cmtno,"725",sizeof(hv_fzf.cmtno)-1)==0)
			{
				FILE *fpw;
				char tmpstr[60];
				memset(tmpstr,0x0,sizeof(tmpstr));
				memset(&hv_addit,0x0,sizeof(struct hv_addit_c));
				iHvGetAddit(cTx_date,cTx_date,hv_fzf.addid,"000",cBuff,sizeof(cBuff)-1);
				vtcp_log("%s,%d �õ��ĸ�����=[%s]",__FILE__,__LINE__,cBuff);
				vtcp_log("%s,%d aaaaaa=[%s]\n",__FILE__,__LINE__,cBuff);
				memset(tmpstr,0x0,sizeof(tmpstr));
				MEMCPY_DEBUG(tmpstr,cBuff+152,8);/*����*/
				vtcp_log("%s,%d,tmpstr====[%s]",__FILE__,__LINE__,tmpstr);
				set_zd_data("0450",tmpstr);
				memset(tmpstr,0x0,sizeof(tmpstr));
				MEMCPY_DEBUG(tmpstr,cBuff+160,8);/*����*/
				set_zd_data("0962",tmpstr);
				memset(tmpstr,0x0,sizeof(tmpstr));
				MEMCPY_DEBUG(tmpstr,cBuff+220,32);/*�ʺ�*/
				set_zd_data("0963",tmpstr);
				memset(tmpstr,0x0,sizeof(tmpstr));
				MEMCPY_DEBUG(tmpstr,cBuff+252,60);/*����*/
				set_zd_data("0964",tmpstr);
				vtcp_log("%s,%d,������ˮ��[%s]**********\n",__FILE__,__LINE__,hv_fzf.trace_no);
				memset(tmpstr,0x0,sizeof(tmpstr));
				sprintf(tmpstr,"%06d", hv_fzf.trace_no);
				set_zd_data("0040",tmpstr);
				/*ret=pub_base_AllDwnFilName(cFilename);
				if(ret)
				{
					vtcp_log("%s,%d �����´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
				}
				fpw=fopen(cFilename,"w");
				if(fpw==NULL)
				{
				vtcp_log("%s,%d ���´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
				}
				HV_725_ADD hv_725;
				memset(&hv_725, 0 , sizeof(hv_725));
				MEMCPY_DEBUG((char *)&hv_725,cBuff, sizeof(hv_725));
				vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FILE__,__LINE__,(char *)&hv_725);
				memset(cBuff, 0 , sizeof(cBuff));
				MEMCPY_DEBUG(cBuff,(char *)&hv_725,sizeof(hv_725));
				fprintf(fpw,"%s",cBuff);
				set_zd_data( DC_FILE_SND,"2");
				close(fpw);*/
			}
			MEMCPY_DEBUG(cTmpcmtno,hv_fzf.cmtno,sizeof(cTmpcmtno)-1);
			vtcp_log("[%s][%d]cTmpcmtno==[%s]",__FILE__,__LINE__,cTmpcmtno);
			vtcp_log("[%s][%d]hv_fzf.pay_qs_no==[%s]",__FILE__,__LINE__,hv_fzf.pay_qs_no);
			/**Ӧ������**/
			if(memcmp(cTmpcmtno,"314",sizeof(cTmpcmtno)-1)==0 || memcmp(cTmpcmtno,"302",sizeof(cTmpcmtno)-1)==0 ||memcmp(cTmpcmtno,"312",3)==0 \
			 || (memcmp(cTmpcmtno,"301",sizeof(cTmpcmtno)-1)==0 && memcmp(hv_fzf.pay_qs_no,HV_QS_BR_NO,12)==0 ))
			{
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);
					vtcp_log("[%s][%d]hv_fzf.res_content==[%s]",__FILE__,__LINE__,hv_fzf.res_content);
  				vtcp_log("[%s][%d]cNotpay_content==[%s]",__FILE__,__LINE__,cNotpay_content);
					deal_half_charcter(cNotpay_content);
					vtcp_log("[%s][%d]cNotpay_content==[%s]",__FILE__,__LINE__,cNotpay_content);
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
					vtcp_log("[%s][%d]cNotpay_content==[%s]",__FILE__,__LINE__,cNotpay_content);
			}
			else	/**��������**/	
			{
  				MEMCPY_DEBUG(cNotpay_content,hv_fzf.req_content,sizeof(hv_fzf.req_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
					vtcp_log("[%s][%d]cNotpay_content==[%s]",__FILE__,__LINE__,cNotpay_content);
  		}
  		
			/* by liuyue 20060922*****�����ѯʱ��ԭ��������ԭ�����е�����*****/
			if(!memcmp(hv_fzf.o_or_br_no,hv_fzf.or_br_no,sizeof(hv_fzf.or_br_no)-1))/**�����о���ԭ������,����ѯ���з����Ľ���**/
			{
				fd123.tx_type[0]	 =  'A';
			}
			else 
			fd123.tx_type[0]	=   'B';
			
			/*���´��֧������*/
			pub_base_full_space((char*)&fd123,sizeof(fd123));
			set_zd_data("1230",(char*)&fd123);
			vtcp_log("fd123=[%s]",(char*)&fd123);
			vtcp_log("cNextVocnum=[%s][%s][%d]",cNextVocnum,__FILE__,__LINE__);
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



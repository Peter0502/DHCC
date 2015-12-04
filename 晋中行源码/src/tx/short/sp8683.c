/*************************************************
* �� �� ��:  sp8683.c
* ���������� ����USBKeyƾ֤ʹ�ô���
* ��    ��:  Han Xichao
* ������ڣ� 2010��10��27��
*
* �޸ļ�¼�� 
* 	��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_attr_c.h"
#include "com_sys_parm_c.h"
#include "netbank_cif_ukey_rel_c.h"
#include "netbank_yqdz_c.h"

int sp8683()
{
	int    ret;
	char   strtmp[17];
	char   cTel[7];
	char   tmpcc[200];
	char   doFlag[1+1];
	char   isFree[1+1];
	struct mdm_attr_c mdm_attr_c;
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct dd_mst_c dd_mst;
	struct com_sys_parm_c com_sys_parm;
	struct netbank_cif_ukey_rel_c netbank_cif_ukey_rel;
	struct netbank_yqdz_c netbank_yqdz;
	
	memset(doFlag,0x00,sizeof(doFlag));
	memset(isFree,0x00,sizeof(isFree));
	memset(tmpcc,0x00,sizeof(tmpcc));
	memset(cTel, 0x00, sizeof(cTel));
	memset(&mdm_attr_c, 0x00, sizeof(struct mdm_attr_c));
	memset(&v_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&dd_mst,0x0,sizeof(struct dd_mst_c));
	memset(&com_sys_parm,0x0,sizeof(struct com_sys_parm_c));
	memset(&netbank_cif_ukey_rel,0x0,sizeof(struct netbank_cif_ukey_rel_c));
	memset(&netbank_yqdz,0x0,sizeof(struct netbank_yqdz_c));
	
	pub_base_sysinit();
	
	/* ǰ̨ȡֵ */
	get_zd_data("0670", doFlag);
	vtcp_log("[%s],[%d],�������Ĳ�����־λΪ[%s]\n",__FILE__,__LINE__,doFlag);
	get_zd_long("0860",&netbank_cif_ukey_rel.cif_no); /* �ͻ��� */
	vtcp_log("[%s],[%d],�������Ŀͻ���Ϊ[%d]\n",__FILE__,__LINE__,netbank_cif_ukey_rel.cif_no);
	get_zd_data("0070", cTel);
	vtcp_log("[%s],[%d],�������Ĺ�Ա����Ϊ[%s]\n",__FILE__,__LINE__,cTel);
  memset(tmpcc,0x00,sizeof(tmpcc));
	get_zd_data("0580",tmpcc);
	vtcp_log("%s,%d tmpcc=[%s]\n",__FILE__,__LINE__,tmpcc);
	get_zd_data("1161", g_pub_tx.mdm_code);
	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s]", g_pub_tx.mdm_code);
	WRITEMSG
	get_zd_data("1162", g_pub_tx.crd_no);
	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s]",g_pub_tx.crd_no);
	WRITEMSG
	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG
	get_zd_data("0580", g_pub_tx.beg_note_no);
	get_zd_data("0580", netbank_cif_ukey_rel.ukey);
	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s]",g_pub_tx.crd_no);
	WRITEMSG
	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG
	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s]",g_pub_tx.crd_no);
	WRITEMSG
	get_zd_data("0580", g_pub_tx.end_note_no);
	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG

	memset(strtmp, 0, sizeof(strtmp));
	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG
	strcpy(strtmp, g_pub_tx.beg_note_no);
	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s][%s]", g_pub_tx.mdm_code,strtmp);
	WRITEMSG
	sprintf(g_pub_tx.beg_note_no, "%016s", strtmp);

	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG
		pub_base_strpack(g_pub_tx.beg_note_no);
	if (!strlen(g_pub_tx.beg_note_no))
		goto OkExit;

	sprintf(acErrMsg, "�������Ľ��ʺͽ�������Ϊ[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG

	ret= Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(ret)
	{
	vtcp_log("[%s][%d]�õ�ϵͳ���ڴ�![%d]\n",__FILE__,__LINE__,ret);
	goto ErrExit;
	}
	vtcp_log("[%s][%d]�������ڣ�[%d]\n",__FILE__,__LINE__,com_sys_parm.sys_date);
	/*** ��ȡ�������� ***/
		ret = Mdm_attr_Sel(&g_pub_tx.reply, &mdm_attr_c, " mdm_code='%s' ", \
				   g_pub_tx.mdm_code);
	if (ret == 100) {
		sprintf(acErrMsg, "�ý������Բ�����![%d][%s]",
			g_reply_int, g_pub_tx.mdm_code);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P132");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "ȡ���������쳣![%d]", g_reply_int);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P133");
		goto ErrExit;
	}
	vtcp_log("heheheehehehehehehehePQAAA[%s]", mdm_attr_c.note_type);
	strcpy(g_pub_tx.note_type, mdm_attr_c.note_type);
	strcpy(g_pub_tx.end_note_no, g_pub_tx.beg_note_no);

	vtcp_log("PQAAA[%s]", g_pub_tx.beg_note_no);
	TRACE
		sprintf(acErrMsg, "mdm_attr_c.note_type=[%s]", mdm_attr_c.note_type);
	WRITEMSG

	/* ��Ҫƾ���ſ������翨�ࣺ����ʱ��Ҫ���뿨�� */
		if (mdm_attr_c.no_ind[0] == 'Y') {
		if (!strlen(g_pub_tx.crd_no)) {
			sprintf(acErrMsg, "��Ҫ���뿨��");
			WRITEMSG
				strcpy(g_pub_tx.reply, "D111");
			goto ErrExit;
		}
		sprintf(acErrMsg, "����Ŀ���Ϊ[%s]", g_pub_tx.crd_no);
		WRITEMSG

		/**��鿨���Ƿ��Ѿ�����**/
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &v_mdm_ac_rel,
					     "ac_no='%s'", g_pub_tx.crd_no);
		vtcp_log("PQAAA[%s]ret[%d]", g_pub_tx.crd_no, ret);
		TRACE
			WRITEMSG
			if (ret && ret != 100)
			goto ErrExit;
		else if (!ret) {
			sprintf(acErrMsg, "��Ҫ���뿨��[%s],ret=[%d]", g_pub_tx.crd_no, ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "D113");
			goto ErrExit;
		}
		strcpy(g_pub_tx.ac_no, g_pub_tx.crd_no);

		vtcp_log("acno[%s]", g_pub_tx.ac_no);
		TRACE
			WRITEMSG

			pub_base_strpack(g_pub_tx.beg_note_no);

		vtcp_log("crd_no[%s]beg_note_no[%s]", g_pub_tx.crd_no, g_pub_tx.beg_note_no);
		TRACE
			WRITEMSG

			vtcp_log("strlen((g_pub_tx.crd_no)=[%d],strlen(g_pub_tx.beg_note_no)=[%d]", strlen(g_pub_tx.crd_no), strlen(g_pub_tx.beg_note_no));
		TRACE
			WRITEMSG
			if (strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.beg_note_no)) {
			/*** ���ݿ���ȡƾ֤�� ***/
			pub_card_CardToNote(g_pub_tx.crd_no, g_pub_tx.beg_note_no);
			strcpy(g_pub_tx.end_note_no, g_pub_tx.beg_note_no);
			vtcp_log("PQAAA[%s]", g_pub_tx.end_note_no);
		}
		vtcp_log("PAAAA[%s]", g_pub_tx.end_note_no);
	}
	/* ��Ҫƾ֤���� */
	if (mdm_attr_c.only_ind[0] == 'Y' && strcmp(doFlag,"2")!=0 ) {
		vtcp_log("PBBBBBBBBBBBBB[%s]", g_pub_tx.note_type);
		vtcp_log("[%s][%d] Beg_no =[%s]", __FILE__,__LINE__,g_pub_tx.beg_note_no);
		ret = pub_com_ChkNoteMin(g_pub_tx.note_type, g_pub_tx.beg_note_no,
					 g_pub_tx.tel);
		if (ret) {
			if(memcmp(g_pub_tx.reply,"D104",4)==0){
				strcpy(g_pub_tx.reply,"N007");
			}
			goto ErrExit;
		}
	}
	
	/* ƾ֤���� */
	if(strcmp(doFlag,"2")!=0 )/**���ǽ�Լ������ʱ�Ȳ���ƾ֤������ֻ��������ǩԼ��**/
	{
		ret=pub_com_ChkNoteUse(mdm_attr_c.note_type);
		if(ret)
		{
			sprintf(acErrMsg,"ƾ֤[%s]����ͣ��״̬",mdm_attr_c.note_type);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N070");
			goto ErrExit;
		}
	}
	if(strcmp(doFlag,"0") == 0)
	{
		goto OkExit;
	}
	else
	{
		ret= Netbank_cif_ukey_rel_Sel(g_pub_tx.reply,&netbank_cif_ukey_rel,"cif_no='%ld' and ukey='%s'",netbank_cif_ukey_rel.cif_no,netbank_cif_ukey_rel.ukey);
		vtcp_log("[%s][%d]Netbank_cif_ukey_rel_Sel ret[%d]\n",__FILE__,__LINE__,ret);
		if(ret==100)
		{
			if(strcmp(doFlag,"2") == 0)
			{
			vtcp_log("[%s][%d]���û���δǩԼ�����ܽ�Լ![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"EB01");
			goto ErrExit;
			}
			else if(strcmp(doFlag,"1") == 0) 
			{
			/**����ǩԼ**/
			strcpy(netbank_cif_ukey_rel.sts,"0");
			netbank_cif_ukey_rel.sign_date=com_sys_parm.sys_date;
			ret = Netbank_cif_ukey_rel_Ins(netbank_cif_ukey_rel, g_pub_tx.reply);
			if (ret != 0)
			{
			sprintf(acErrMsg,"������ǩԼ������¼����!![%d]",ret);
			WRITEMSG
			goto ErrExit;
			}
			vtcp_log("[%s][%d]�Ǽ�����ǩԼ��ɹ�!\n",__FILE__,__LINE__);
			}
			else
			{
			strcpy(g_pub_tx.reply,"H037");
			vtcp_log("[%s][%d]�������ʹ���!\n",__FILE__,__LINE__);
			goto ErrExit;
			}
		}
		else
		{
		/*if(strcmp(netbank_cif_ukey_rel.sts,"0") == 0 && strcmp(doFlag,"1") == 0)
			{
				strcpy(g_pub_tx.reply,"EB02");
				vtcp_log("[%s][%d]�Ѿ�ǩԼ�û�����������ǩԼ����![%d]\n",__FILE__,__LINE__,ret);
				goto ErrExit;
			}
			else*/ if(strcmp(netbank_cif_ukey_rel.sts,"1") == 0 && strcmp(doFlag,"2") == 0)
			{
				strcpy(g_pub_tx.reply,"EB03");
				vtcp_log("[%s][%d]���û��Ѿ���Լ������������Լ![%d]\n",__FILE__,__LINE__,ret);
				goto ErrExit;
			}
			else if(strcmp(netbank_cif_ukey_rel.sts,"0") == 0 && strcmp(doFlag,"2") == 0)
			{
			/**���Ѿ�ǩԼ�û�����Լ����**/
				ret = Netbank_cif_ukey_rel_Dec_Upd(g_pub_tx.reply," cif_no=%ld and ukey='%s'", netbank_cif_ukey_rel.cif_no,netbank_cif_ukey_rel.ukey);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Declare error [%d]",ret);
						WRITEMSG
						goto ErrExit;
				}
				memset(&netbank_cif_ukey_rel,0x0,sizeof(struct netbank_cif_ukey_rel_c));
				ret = Netbank_cif_ukey_rel_Fet_Upd(&netbank_cif_ukey_rel,g_pub_tx.reply);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Fetch error [%d] ",ret);
						WRITEMSG
						Netbank_cif_ukey_rel_Clo_Upd();
						goto ErrExit;
				}
				
				strcpy(netbank_cif_ukey_rel.sts,"1");
				netbank_cif_ukey_rel.cancel_date=com_sys_parm.sys_date;
				ret = Netbank_cif_ukey_rel_Upd_Upd(netbank_cif_ukey_rel, g_pub_tx.reply);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Update error [%d] ",ret);
						WRITEMSG
						Netbank_cif_ukey_rel_Clo_Upd();
						goto ErrExit;
				}
				Netbank_cif_ukey_rel_Clo_Upd();
				/**�ж����ÿͻ�������ʹ�õ�ukey��ĿΪ��ʱ,�Զ���Լ�����������**/
				ret=sql_count("netbank_cif_ukey_rel","cif_no=%ld and sts='0'",netbank_cif_ukey_rel.cif_no);
					if(ret<0)
					{
						sprintf(acErrMsg,"sql_count error!! [%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
					else if(ret==0) 
					{
					/**���û��Ѿ�û������״̬ukey,�ж����Ѿ���Լ������,�Զ���Լ�������������**/
						ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld ",netbank_cif_ukey_rel.cif_no);
						if(ret)
						{
							sprintf( acErrMsg, "sql error" );
							WRITEMSG
							set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
							goto ErrExit;
						}
						while(1)
						{
							ret=Dd_mst_Fet_Sel(&dd_mst,g_pub_tx.reply);
							if(ret)
								{
								if(ret==100)
									{
									break;
									}
									else
									{
									sprintf(acErrMsg,"sql error");
									WRITEMSG
									set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
									Dd_mst_Clo_Sel();
									goto ErrExit;
									}
								}
								vtcp_log("[%s][%d]Ԥ����ǩԼ������˵��˻�IDΪ:[%ld]!\n",__FILE__,__LINE__,dd_mst.ac_id);
							ret= Netbank_yqdz_Sel(g_pub_tx.reply,&netbank_yqdz,"ac_id='%ld' and sts='0'",dd_mst.ac_id);
							if(ret==100)
								{
									/**û��ǩԼ��ֱ�������������Լ**/
									vtcp_log("[%s][%d]�˻�IDΪ:[%ld]���˺Ų�δǩ����������ˣ������Լ\n",__FILE__,__LINE__,dd_mst.ac_id);
									break;
								}
							else
								{
									/**���Ѿ�ǩԼ�û�����Լ����**/
									ret = Netbank_yqdz_Dec_Upd(g_pub_tx.reply," ac_id=%ld ", dd_mst.ac_id);
									if (ret != 0)
									{
										sprintf(acErrMsg,"Declare error [%d]",ret);
										WRITEMSG
										goto ErrExit;
									}
									memset(&netbank_yqdz,0x0,sizeof(struct netbank_yqdz_c));
									ret = Netbank_yqdz_Fet_Upd(&netbank_yqdz,g_pub_tx.reply);
									if (ret != 0)
									{
										sprintf(acErrMsg,"Fetch error [%d] ",ret);
										WRITEMSG
										Netbank_yqdz_Clo_Upd();
										goto ErrExit;
									}
									
									strcpy(netbank_yqdz.sts,"1");
									netbank_yqdz.cancel_date=com_sys_parm.sys_date;
									ret = Netbank_yqdz_Upd_Upd(netbank_yqdz, g_pub_tx.reply);
									if (ret != 0)
									{
										sprintf(acErrMsg,"Update error [%d] ",ret);
										WRITEMSG
										Netbank_yqdz_Clo_Upd();
										goto ErrExit;
									}
									Netbank_yqdz_Clo_Upd();
								}
						}
						
					}
					else
					{
					vtcp_log("[%s][%d]���п���ʹ�õ�ukey:[%d]��,�����Զ���Լ�����������!\n",__FILE__,__LINE__,ret);
					}
			}
			else if(strcmp(netbank_cif_ukey_rel.sts,"1") == 0 && strcmp(doFlag,"1") == 0)
			{
				vtcp_log("[%s][%d]���Ѿ���Լ�û�������ǩԼ����1\n",__FILE__,__LINE__);
				/**���Ѿ���Լ�û�������ǩԼ����**/
				ret = Netbank_cif_ukey_rel_Dec_Upd(g_pub_tx.reply," cif_no=%ld ", netbank_cif_ukey_rel.cif_no);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Declare error [%d]",ret);
						WRITEMSG
						goto ErrExit;
				}
				memset(&netbank_cif_ukey_rel,0x0,sizeof(struct netbank_cif_ukey_rel_c));
				ret = Netbank_cif_ukey_rel_Fet_Upd(&netbank_cif_ukey_rel,g_pub_tx.reply);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Fetch error [%d] ",ret);
						WRITEMSG
						Netbank_cif_ukey_rel_Clo_Upd();
						goto ErrExit;
				}
				strcpy(netbank_cif_ukey_rel.sts,"0");
				netbank_cif_ukey_rel.sign_date=com_sys_parm.sys_date;
				ret = Netbank_cif_ukey_rel_Upd_Upd(netbank_cif_ukey_rel, g_pub_tx.reply);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Update error [%d] ",ret);
						WRITEMSG
						Netbank_cif_ukey_rel_Clo_Upd();
						goto ErrExit;
				}
				Netbank_cif_ukey_rel_Clo_Upd();
			}
			if(strcmp(doFlag,"1") != 0 && strcmp(doFlag,"2") != 0)
			{
			strcpy(g_pub_tx.reply,"H037");
			vtcp_log("[%s][%d]�������ʹ���!\n",__FILE__,__LINE__);
			goto ErrExit;
			}
		}
	}
	if(strcmp(doFlag,"1") == 0)
	{
		ret=pub_com_NoteUse(0,0,mdm_attr_c.note_type,g_pub_tx.beg_note_no,g_pub_tx.beg_note_no,cTel);
		if(ret)
		{
			sprintf(acErrMsg,"��Ա[%s]ʹ��ƾ֤[%s]����",cTel,g_pub_tx.beg_note_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N005");
			goto ErrExit;
		}
	}
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

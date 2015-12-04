/*****************************************
�ļ�����spW005
��  �ܣ����ʺ���ϸ��Ϣ��ѯ

���ʱ�䣺2009��3��25��

����: chengbo

�޸ļ�¼��
 1���޸��� gujingyu
 2���޸�ʱ�� 2009��4��1��
 3���޸�����
******************************************/

#include "string.h"
#include "stdio.h"
#define  EXTERN 
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_parm_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "td_mst_c.h"
#include "td_parm_c.h"

spW005()
{
	
	int      ret;
	char     s_ac_no[21];
	int      s_ac_seqn;
	long     date;
        long     yx_date;
        double   sbal;
	char     bz[5];
	char     ac_std[6];
	char     lxsno[4];  /*��Ϣ˰����*/
	char     filename[300];
	char     sTime[12];
	FILE     *fp;
	
	struct  dd_mst_c      sDd_mst;
	struct  mdm_ac_rel_c  sMdm_ac_rel;
	struct  dd_parm_c     sDd_parm;
	struct  prdt_ac_id_c  sPrdt_ac_id;
	struct  mdm_attr_c    sMdm_attr;
	struct  td_mst_c      sTd_mst;
	struct  td_parm_c     sTd_parm;
	
	memset(sTime,0,sizeof(sTime));
 	 memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
 	 memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
 	 memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
 	 memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
 	 memset(&sMdm_attr,0x00,sizeof(struct mdm_attr_c));
 	 memset(&sTd_parm,0x00,sizeof(struct td_parm_c));
 	 memset(&sTd_mst,0x00,sizeof(struct td_mst_c));
 	 memset(s_ac_no,0,sizeof(s_ac_no));
 	 memset(bz,0,sizeof(bz));
 	 memset(ac_std,0,sizeof(ac_std));
 	 memset(lxsno,0,sizeof(lxsno));
	 memset(filename,0,sizeof(filename));
	  /***ȡֵ����ֵ***/
	  pub_base_sysinit();
  
	  get_zd_data("0310",s_ac_no);
	  get_zd_int("0340",&s_ac_seqn);
  
  
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s' and ac_seqn=9999", s_ac_no);
	if (ret == 100) {
		sprintf(acErrMsg, "�޴˿��ţ�����!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "��ȡ�����˻����ձ��쳣![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P147");
		goto ErrExit;
	}
	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%ld",sMdm_ac_rel.ac_id,s_ac_seqn);
	if (ret == 100) {
		sprintf(acErrMsg, "�ÿ�������������Ʒ������!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
		
	} 
	ret=Mdm_attr_Sel(g_pub_tx.reply,&sMdm_attr,"mdm_code='%s'",sMdm_ac_rel.mdm_code);
	if(ret==100){
		sprintf(acErrMsg,"�����ڴ˽���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg, "��ȡmdm_attr���쳣![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "N056");
		goto ErrExit;
	}
	
	
	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName(filename);
	vtcp_log("%d@%s name[%s] ", __LINE__, __FILE__, filename);
	
	fp = fopen(filename, "w");
	if (fp == NULL) {
		sprintf(acErrMsg, "open file error [%s]", filename);
		WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	fprintf(fp, "~���ʺ�|���˿���|���|������|�������|��Ϣ��|������|������|�˻�����|����|�������|״̬|�������|��ǰ���|������|Ӧ����Ϣ|��Ϣ����|�˻��к�|�����к�|����|����|ȡϢ�������|�´�ȡ������|�����־|�ϴν�������|�ϴν��׹�Ա|��Ϣ˰����|\n");
	
	vtcp_log("%s,%d��ʼ������ѯ�ʺ���Ϣ",__FILE__,__LINE__);
 if(sPrdt_ac_id.ac_id_type[0] == '1'){
	ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn=%ld",sMdm_ac_rel.ac_id,s_ac_seqn);
	if (ret) {
		sprintf(acErrMsg, "��ȡdd_mst���쳣![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "W015");
		goto ErrExit;
	}
	if(sDd_mst.ac_sts[0]=='1')
	{
			strcpy( ac_std,"����");
			date=99999999;
			yx_date=99999999;
	}
			else if(sDd_mst.ac_sts[0]=='3')
			{
				strcpy( ac_std,"��ʧ����");
				yx_date=sDd_mst.lst_date;
				date=99999999;
			}
			else if(sDd_mst.ac_sts[0]=='*' || sDd_mst.ac_sts[0]=='5')
			{
				strcpy( ac_std,"����");
				/*
				date=sDd_mst.lst_date;
				yx_date=99999999;
				*/
				yx_date=sDd_mst.lst_date;
				date=99999999;
			}
	strcpy(bz,"CNY");
	if(sDd_mst.hold_sts[0]=='0'||sDd_mst.hold_sts[0]=='3'){
		sbal=sDd_mst.bal-sDd_mst.ctl_amt-sDd_mst.hold_amt;
        }else if(sDd_mst.hold_sts[0]=='1'||sDd_mst.hold_sts[0]=='2'){
		sbal=sDd_mst.bal-sDd_mst.ctl_amt-0.00;
   	}
	ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm, "prdt_no='%s'", sPrdt_ac_id.prdt_no);
	if (ret) {
		sprintf(acErrMsg, "��ȡdd_parm���쳣![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "W025");
		goto ErrExit;
	}
	strcpy(lxsno,"001");
	fprintf(fp, "%s|%s|%d|%8d|%.2lf|%8d|%8d|%8d|%1s|%s|%.2lf|%s|%.2lf|%.2lf|%.2lf|||%s|%s|%s|%2d||||%8d||%s|\n", s_ac_no,s_ac_no,s_ac_seqn,sDd_mst.opn_date,0.00,sDd_mst.ic_date,date,yx_date,sDd_mst.ac_type,bz,sbal,sDd_mst.ac_sts,sDd_mst.ys_bal,sDd_mst.bal,sDd_parm.min_bal,sDd_mst.opn_br_no,sDd_mst.opn_br_no,sDd_parm.title,0,sDd_mst.lst_date,lxsno);

	}else if(sPrdt_ac_id.ac_id_type[0] == '2'){
	
		ret=Td_mst_Sel(g_pub_tx.reply,&sTd_mst,"ac_id=%ld and ac_seqn=%ld",sMdm_ac_rel.ac_id,s_ac_seqn);
		if(ret){
			  sprintf(acErrMsg,"��ȡtd_mst�����![%d]",ret);
			  WRITEMSG
			  strcpy(g_pub_tx.reply,"W017");
			  goto ErrExit;
			}
			if(sTd_mst.ac_sts[0]=='1')
			{
				strcpy( ac_std,"����");
				/*
				date=99999999;
				yx_date=sTd_mst.mtr_date;
				*/
				yx_date=99999999;
				date=sTd_mst.mtr_date;
			}
			else if(sTd_mst.ac_sts[0]=='*')
			{
				strcpy( ac_std,"����");
				/*
				date=sTd_mst.lst_date;
				yx_date=sTd_mst.mtr_date;
				*/
				yx_date=sTd_mst.lst_date;
				date=sTd_mst.mtr_date;
			}
	strcpy(bz,"CNY");
	if(sTd_mst.hold_sts[0]=='0'||sTd_mst.hold_sts[0]=='3'){
	        sbal=sTd_mst.bal-sTd_mst.ctl_amt-sTd_mst.hold_amt;
          }else if(sTd_mst.hold_sts[0]=='1'||sTd_mst.hold_sts[0]=='2'){
   	      sbal=sTd_mst.bal-sTd_mst.ctl_amt-0.00;
   	    }			 
     ret = Td_parm_Sel(g_pub_tx.reply, &sTd_parm, "prdt_no='%s'", sPrdt_ac_id.prdt_no);
	if (ret) {
		sprintf(acErrMsg, "��ȡtd_parm���쳣![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "A021");
		goto ErrExit;
	}
	sprintf(sTime,"%d%s",sTd_parm.term,sTd_parm.term_type);
	strcpy(lxsno,"001");
	fprintf(fp, "%s|%s|%4d|%d|%.2lf|%d|%d|%d|%s|%s|%.2lf|%s|%.2lf|%.2lf|%.2lf|||%s|%s|%s|%s||||%d||%s|\n", s_ac_no,s_ac_no,s_ac_seqn,sTd_mst.opn_date,0.00,sTd_mst.ic_date,date,yx_date,"2",bz,sbal,sTd_mst.ac_sts,sTd_mst.ys_bal,sTd_mst.bal,sTd_parm.min_bal,sTd_mst.opn_br_no,sMdm_ac_rel.opn_br_no,sTd_parm.title,sTime,sTd_mst.lst_date,lxsno);
	}
        fclose(fp);
	set_zd_data(DC_FILE_SND, "1"); 
	
	   
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



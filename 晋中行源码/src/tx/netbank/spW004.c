/*****************************
*�ļ����ƣ�spW004.c

*���ܣ�������Ϣ��ѯ

*���� chengbo

*ʱ�䣺2009��3��25��

*�޸ļ�¼��
* 1���޸�ʱ��
 
* 2���޸��� Lsy20100928

* 3���޸����� �淶����������ʾ��Ϣ
*
*******************************/
#define  EXTERN 
#include "string.h"
#include "stdio.h"
#include "public.h"
#include "card_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "mdm_attr_c.h"
#include "dd_mst_c.h"
#include "prdt_ac_id_c.h"
#include "td_mst_c.h"
#include "card.h"
 
spW004()
{
    int   ret=0;
    char  ac_no[21];
    char  id_type[2];
    char  id_no[21];
    char  filename[300];
    FILE  *fp;
    long  date;
    long  yx_date;
    char  ac_std[5];
    double sbal;
    
    struct card_reg_c    sCard_reg;
    struct mdm_ac_rel_c  sMdm_ac_rel;
    struct mdm_attr_c    sMdm_attr;
    struct dd_mst_c      sDd_mst;
    struct td_mst_c      sTd_mst;
    struct prdt_ac_id_c  sPrdt_ac_id;
    
    
    memset(&sCard_reg,0x00,sizeof(struct card_reg_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sMdm_attr,0x00,sizeof(struct mdm_attr_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sTd_mst,0x00,sizeof(struct td_mst_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(ac_no,0,sizeof(ac_no));
    memset(id_type,0,sizeof(id_type));
    memset(id_no,0,sizeof(id_no));
    memset(filename,0,sizeof(filename));
    memset(ac_std,0,sizeof(ac_std));
    
    /***ȡֵ����ֵ***/
    pub_base_sysinit();
    
    get_zd_data("0300",ac_no);     /**�ʿ���**/
    get_zd_data("0680",id_type);   /**֤������**/
    get_zd_data("0630",id_no);     /**֤������**/
   	pub_base_old_acno(ac_no); 
	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName(filename);

	fp = fopen(filename, "w");
	if (fp == NULL) {
		sprintf(acErrMsg, "open file error [%s]", filename);
		WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d]�ļ�����----------------------->[%s]",__FILE__,__LINE__,filename);

	 fprintf(fp, "~����|����|����־|��Ƭ����|�����к�|�ֿ��˿ͻ���|������|������|������|��Ч��|״̬|���¿�����|��ѱ�־|�����ȡ��|��Ƭ|���˵�����ʽ|���˵��ʼĵ�ַ����|�ӳ���־|��ϵ|������|���뿨�к�|�˻���|֤������|֤������|ע�����|����������|����ATM����������|����ATM֧ȡ���|������Ա���|E��ͨ�ӿ�����|\n");
	
	vtcp_log("%s,%d��ʼ������ѯ�ʺ���Ϣ",__FILE__,__LINE__);
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", ac_no);
	if (ret == 100) {
		sprintf(acErrMsg, "�޴˿��ţ�����!ac_no=[%s]", ac_no);
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "��ȡ�����˻����ձ��쳣![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P147");
		goto ErrExit;
	}
        
	vtcp_log("[%s=%s],[%s=%s]",id_type,sMdm_ac_rel.id_type,sMdm_ac_rel.id_no,id_no);
	/** Modify By Lsy 20109028 **/
	if ( id_type[0] != sMdm_ac_rel.id_type[0] ) { 
		sprintf(acErrMsg, "�ͻ�֤�����Ͳ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "P444");
		goto ErrExit;
	}
	if ( 0!=strcmp(sMdm_ac_rel.id_no,id_no) ) {
		sprintf(acErrMsg, "�ͻ�֤�����벻��!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "P445");
		goto ErrExit;
	}
	/** End By Lsy 21:46**/
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
	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%d",sMdm_ac_rel.ac_id);
	if(ret)
	{
		sprintf(acErrMsg, "��ȡdd_mst���쳣![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "W015");
		goto ErrExit;
	}
	if(sMdm_ac_rel.ac_seqn == 9999 || sPrdt_ac_id.ac_id_type[0]=='1' )
	{
		ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
	}else if(sPrdt_ac_id.ac_id_type[0]=='2'){
		ret=Td_mst_Sel(g_pub_tx.reply,&sTd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
		sDd_mst.ac_sts[0]=sTd_mst.ac_sts[0];
		sDd_mst.opn_date=sTd_mst.opn_date;
		sDd_mst.lst_date=sTd_mst.lst_date;
		memcpy(sDd_mst.name,sTd_mst.name,sizeof(sDd_mst.name)-1);
		memcpy(sDd_mst.opn_br_no,sTd_mst.opn_br_no,sizeof(sDd_mst.opn_br_no)-1);
		sDd_mst.cif_no=sTd_mst.cif_no;
	}else{
		vtcp_log("%s,%d,��������ڡ������˺�!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"B114");
		goto ErrExit;
	}
	if(ret){
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
		strcpy( ac_std,"������");
	}
	else if(sDd_mst.ac_sts[0]=='*')
	{
		strcpy( ac_std,"����");
		date=sDd_mst.lst_date;
		yx_date=99999999;
	}
	if(memcpy(ac_no,CARDHEADCODE,6)==0)
	{
		ret=Card_reg_Sel(g_pub_tx.reply,&sCard_reg,"ac_no='%s'",ac_no);
		if(ret){
			sprintf(acErrMsg, "��ȡcard_reg���쳣![%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "CU06");
			goto ErrExit;
		}
	}
	sbal=sCard_reg.atm_td_dr_cash_tx_amt+sCard_reg.atm_td_dr_tran_tx_amt;
	fprintf(fp, "%s|%s|||%s|%d|%s|%d|%d|%d|%s|||||||||%d|%s|%s|%s|%s|||%d|%.2lf|||\n", ac_no,sMdm_attr.mdm_type,sMdm_ac_rel.opn_br_no,sDd_mst.cif_no,ac_no,sCard_reg.opn_date,date,yx_date,sMdm_ac_rel.note_sts,sDd_mst.opn_date,sDd_mst.opn_br_no,sDd_mst.name,sMdm_ac_rel.id_type,sMdm_ac_rel.id_no,sCard_reg.add_pswd_cnt,sbal);
				 
	vtcp_log("��ӡ��Ϣ-----------------%s|%s|||%s|%d|%s|%d|%d|%d|%s|||||||||%d|%s|%s|%s|%s|||%d|%.2lf|||\n", ac_no,sMdm_attr.mdm_type,sMdm_ac_rel.opn_br_no,sDd_mst.cif_no,ac_no,sCard_reg.opn_date,date,yx_date,sMdm_ac_rel.note_sts,sDd_mst.opn_date,sDd_mst.opn_br_no,sDd_mst.name,sMdm_ac_rel.id_type,sMdm_ac_rel.id_no,sCard_reg.add_pswd_cnt,sbal);
 
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

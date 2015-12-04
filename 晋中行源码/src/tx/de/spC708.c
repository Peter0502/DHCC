/*************************************************
* �� �� ��:    spC708.c
* ����������   ����������ֽ���塣
* ��    ��:    wzw
* ������ڣ�   2012��6��14��
insert into tx_def      values('3708','��������ֽ�','10100000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000101100000100','1','1','2');
insert into tx_flow_def values('3708',0,'C708','#=0000@1@#$');
insert into tx_flow_def values('3708',1,'A016','#=0000@2@#$');
insert into tx_flow_def values('3708',2,'D005','#=0001@3@#=0002@4@#$');
insert into tx_flow_def values('3708',3,'M002','#$');
insert into tx_flow_def values('3708',4,'D099','#$');
insert into tx_sub_def  values('C708','��������ֽ�','spC708','0','0'); 
*************************************************/
#define  ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include "public.h"
#define  extern
#include "card.h"
#include "mdm_ac_rel_c.h"
#include "mdm_attr_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"

spC708()
{
	int     ret=0;
	double  tx_amt=0.00;      /*���׽��*/
	char    AC_NO[20];        /*ԭ�˺�(�����˺�)*/
	char    ACNO_IN[20];      /*ת���˺�*/
	char    XZ_Flag[2];       /*��������������־*/
	char    cAcc_no[8];       /*����Ǯ���˻����˿�Ŀ*/
	char    cIn_acno[20];     /*IC������Ǯ���ڲ��˻�*/
	
	struct  mdm_ac_rel_c      sMdm_ac_rel;
	struct  mdm_attr_c        sMdm_attr;
	struct  prdt_ac_id_c      sPrdt_ac_id;
	struct  dd_mst_c          sDd_mst;

	memset(AC_NO,             0x00, sizeof(AC_NO));
	memset(ACNO_IN,           0x00, sizeof(ACNO_IN));
	memset(XZ_Flag,           0x00, sizeof(XZ_Flag));
	memset(cAcc_no,           0x00, sizeof(cAcc_no));
	memset(cIn_acno,          0x00, sizeof(cIn_acno));
	
	memset(&sMdm_ac_rel,      0x00, sizeof(struct mdm_ac_rel_c));
	memset(&sMdm_attr,        0x00, sizeof(struct mdm_attr_c));
	memset(&sPrdt_ac_id,      0x00, sizeof(struct prdt_ac_id_c));
	memset(&sDd_mst,          0x00, sizeof(struct dd_mst_c));
 
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/*ȡֵ����ֵ*/
	get_zd_data("0300",AC_NO);
	pub_base_strpack(AC_NO);
	pub_base_old_acno(AC_NO);
	get_zd_data("0310",ACNO_IN);
	pub_base_strpack(ACNO_IN);
	pub_base_old_acno(ACNO_IN);
	get_zd_double("0400", &tx_amt);
	get_zd_data("0670",XZ_Flag);
	get_zd_data("0640",cAcc_no);
	get_zd_data("0320",cIn_acno);
	pub_base_strpack(cIn_acno);
	pub_base_old_acno(cIn_acno);

	if(XZ_Flag[0]=='0') /*�����ֽ����*/
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s' and substr(ac_no,0,6)='%s'",AC_NO,ICCARDHEADCODE);
			ERR_DEAL
		
		ret=Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id=%ld", sMdm_ac_rel.ac_id);
			ERR_DEAL
		if(sDd_mst.ac_sts[0]!='*')
		{
			sprintf(acErrMsg, "�����ֽ���������������");
			WRITEMSG
			strcpy(g_pub_tx.reply, "G009");
			goto ErrExit;
		}
		
		set_zd_data( "1191" , "01" );	 
		set_zd_double( "1194", tx_amt );	
		set_zd_data( "1197", "�ֽ𸶳�" );	
		set_zd_data( "1198",  cIn_acno);
		vtcp_log("%s%d[%s]",__FILE__,__LINE__,cIn_acno);
		
		set_zd_data("1171", "1");
	}
	else if(XZ_Flag[0]=='1') /*����ת�˽���*/
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'and substr(ac_no,0,6)='%s'",ACNO_IN,ICCARDHEADCODE);
			ERR_DEAL

		ret=Mdm_attr_Sel(g_pub_tx.reply, &sMdm_attr, "mdm_code='%s'", sMdm_ac_rel.mdm_code);
			ERR_DEAL
	
		ret=Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld", sMdm_ac_rel.ac_id);
			ERR_DEAL
		
		/*����ӽ���*/
		set_zd_data("1011",ACNO_IN);
		set_zd_long("1012",sPrdt_ac_id.ac_seqn);
		set_zd_data("1014",sMdm_attr.note_type);
		set_zd_data("1015",sMdm_ac_rel.note_no);
		set_zd_double("1013",tx_amt);
		set_zd_data("101A","01");
		set_zd_data("101B","2");
		set_zd_data("1016","���");
		set_zd_data("1017","0");
		
		set_zd_data("1171", "2");
	}
	
	/*(�ڲ��ʽ跽����)*/
	set_zd_data("1201",cIn_acno);
	set_zd_double("1208",tx_amt);
	set_zd_data("120A","�ڲ��ʽ跽����");
	if(XZ_Flag[0]=='0')
	{
		set_zd_data("1205","1");
	}
	else if(XZ_Flag[0]=='1')
	{
		set_zd_data("1205","2");
	}
	set_zd_data("1204","01");

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������ֽ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "��������ֽ�ʧ��![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;	
} 


/*************************************************
* �� �� ��: sp1000.c
* ������������ӡϵͳ��ѯ�ӿ�
* ��    ��: zgf
* ������ڣ�20131113

*�������ã�
insert into tx_def values('1000','��ӡϵͳ��ѯ�ӿ�','10000000000000010000000011111111000010000001110100000000000000000001111100000000000101000011000000000000000000000000000000000100','1','4','0');

insert into tx_sub_def values('1000','��ӡϵͳ��ѯ�ӿ�','sp1000','0','1');

insert into tx_flow_def values ('1000', 0, '1000', '#$');


���ձ�����ֵ
	�ͻ��˺�      0300
	���������		0020

���ر�����ֵ
	�˺�					0310
	����					0260
	��ַ					0250
	��������			0290
	��ϵ��				0270
	��ϵ�绰			0320
	��������			0440
	��������			0450
	��������			0460
	�˻�����    ac_type  0680
	ͨ�ҷ�ʽ	  td_kt_flag(tctd_ktzh)  0690
	���������		0910
������
	��ֻ�п������ڣ����������ڵ��ڿ�������
	�������ſɲ�ѯ�������˺�

*************************************************/

#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"  /**���˺ţ�������ȡ���˻�ID���������ڣ�ֹ�����ڣ�*/
#include "dd_mst_c.h"				/**���˻�ID��ȡ�������������˻����ͣ��������ڣ���Ϣ���ڣ���Ч���ڣ��ͻ��ţ�������**/
#include "td_mst_c.h"
#include "od_mst_c.h"
#include "ln_mst_c.h"
#include "dd_parm_c.h"				/**���˻�ID��ȡ�������������˻����ͣ��������ڣ���Ϣ���ڣ���Ч���ڣ��ͻ��ţ�������**/
#include "td_parm_c.h"
#include "od_parm_c.h"
#include "ln_parm_c.h"
#include "cif_addr_inf_c.h" /**�ɿͻ��Ż�ȡ����ַ���������룩***/
#include "prdt_ac_id_c.h"

sp1000()
{
    struct mdm_ac_rel_c sMdm;
    struct dd_mst_c sDd;
    struct td_mst_c sTd;
    struct od_mst_c sOd;
    struct ln_mst_c sLn;
    struct dd_parm_c sDd_parm;
    struct td_parm_c sTd_parm;
    struct od_parm_c sOd_parm;
    struct ln_parm_c sLn_parm;
    struct cif_addr_inf_c sCif;
    struct prdt_ac_id_c sPrdt;
		char cAcno[20];
		char br_no[6];
		int g_reply_int;
    
    memset(&sMdm,0x00,sizeof(sMdm));
    memset(&sDd,0x00,sizeof(sDd));
    memset(&sTd,0x00,sizeof(sTd));
    memset(&sOd,0x00,sizeof(sOd));
    memset(&sLn,0x00,sizeof(sLn));
    memset(&sDd_parm,0x00,sizeof(sDd_parm));
    memset(&sTd_parm,0x00,sizeof(sTd_parm));
    memset(&sOd_parm,0x00,sizeof(sOd_parm));
    memset(&sLn_parm,0x00,sizeof(sLn_parm));
    memset(&sCif,0x00,sizeof(sCif));
    memset(&sPrdt,0x00,sizeof(sPrdt));
    memset(cAcno,0x00,sizeof(cAcno));
    memset(br_no,0x00,sizeof(br_no));
    
    pub_base_sysinit();
    
 		/*��ȡǰ̨����**/
    get_zd_data("0300",cAcno);   
		if(cAcno[0]>='0' && cAcno[0]<='9'){
			pub_base_old_acno(cAcno);
		}
		
    get_zd_data("0020",br_no); 
vtcp_log("aaaaaaaaaaaaaaaaac_no[%s]--br_no[%s]!!!!!",cAcno,br_no);    
		g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm,"ac_no = '%s' and opn_br_no = '%s'",cAcno,br_no);
		if (g_reply_int)
    {
       vtcp_log("%s,%d,��֤�û���Ϣʧ��!",__FILE__,__LINE__);
       strcpy(g_pub_tx.reply,"CU14");
       goto ErrExit;
    }
              
  	g_reply_int=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt,"ac_id = %ld ",sMdm.ac_id);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]��ѯ��Prdt_ac_id_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"F039");
        goto ErrExit;
    }
		if(sPrdt.ac_id_type[0] == '1')/**�����˻�**/
		{
			g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&sDd,"ac_id = %ld ",sPrdt.ac_id);
	    if(g_reply_int || sDd.ac_sts[0]=='0')
	    {
	        vtcp_log("[%s][%d]��ѯ��Dd_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
	    g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no = '%s' ",sDd.prdt_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Dd_parm_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
			set_zd_data("0260",sDd.name);	/**����**/
			set_zd_data("0270",sDd.name);	/**��ϵ��**/
			set_zd_data("0320","");	/**��ϵ�绰**/
			set_zd_long("0440",sDd.opn_date);	/**��������**/
			set_zd_data("0680",sDd.ac_type);	/**�˻�����ac_type**/
			set_zd_data("0690",sDd_parm.thr_draw_ind);	/**ͨ�ҷ�ʽthr_draw_ind**/
			set_zd_data("0910",sDd.opn_br_no);	/**���������**/
			if(sDd.ac_sts[0] == '*')
			{
				set_zd_long("0450",sDd.lst_date);
			}
			else 
			{
				set_zd_long("0450",99999999);
			}
		}
		else if(sPrdt.ac_id_type[0] == '2')/**�����˻�**/
		{
			g_reply_int=Td_mst_Sel(g_pub_tx.reply,&sTd,"ac_id = %ld ",sPrdt.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Td_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
	    g_reply_int=Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no = '%s' ",sTd.prdt_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Td_parm_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
			set_zd_data("0260",sTd.name);	/**����**/
			set_zd_data("0270",sTd.name);	/**��ϵ��**/
			set_zd_data("0320","");	/**��ϵ�绰**/
			set_zd_long("0440",sTd.opn_date);	/**��������**/
			set_zd_data("0680","d");	/**�˻�����ac_type d ���ڻ�**/
			set_zd_data("0690",sTd_parm.thr_draw_ind);	/**ͨ�ҷ�ʽthr_draw_ind**/
			set_zd_data("0910",sTd.opn_br_no);	/**���������**/
			if(sTd.ac_sts[0] == '*')
			{	
				set_zd_long("0450",sTd.lst_date);
			}	
			else 
			{
				set_zd_long("0450",99999999);
			}
		}
		else if(sPrdt.ac_id_type[0] == '3')/**�����˻�**/
		{
			g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&sLn,"ac_id = %ld ",sPrdt.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Ln_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
	    g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&sLn_parm,"prdt_no = '%s' ",sLn.prdt_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Ln_parm_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
			set_zd_data("0260",sLn.name);	/**����**/
			set_zd_data("0270",sLn.name);	/**��ϵ��**/
			set_zd_data("0320","");	/**��ϵ�绰**/
			set_zd_long("0440",sLn.opn_date);	/**��������**/
			set_zd_data("0680","");	/**�˻�����ac_type**/
			set_zd_data("0690","");	/**ͨ�ҷ�ʽthr_draw_ind**/
			set_zd_data("0910",sLn.opn_br_no);	/**���������**/
			if(sLn.ac_sts[0] == '*')
			{
				set_zd_long("0450",sLn.lst_date);
			}
			else 
			{
				set_zd_long("0450",99999999);
			}
		}
		/*
		else if(sPrdt.ac_id_type[0] == '4')
		{
			g_reply_int=Od_mst_Sel(g_pub_tx.reply,&sOd,"ac_id = %ld ",sPrdt.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Od_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
	    g_reply_int=Od_parm_Sel(g_pub_tx.reply,&sOd_parm,"prdt_no = '%s' ",sOd.prdt_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Od_parm_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
			set_zd_data("0260",sOd.name);	
			set_zd_data("0270",sOd.name);	
			set_zd_data("0320","");	
			set_zd_long("0440",sOd.opn_date);	
			set_zd_data("0680",sOd.ac_type);	
			set_zd_data("0690",sOd_parm.thr_draw_ind);	
			set_zd_data("0910",sOd.opn_br_no);	
		}
		*/
		g_reply_int=Cif_addr_inf_Sel(g_pub_tx.reply,&sCif,"cif_no = %ld ",sPrdt.cif_no);
	if(g_reply_int==100)
	{
	     strcpy(sCif.addr, "");
	     strcpy(sCif.post_code,"030600");
	}
    else if(g_reply_int)
    {
        vtcp_log("[%s][%d]��ѯ��Cif_addr_inf_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"F039");
        goto ErrExit;
    }
		/** set_zd_long("0450",sMdm.end_date);	**//**��������**/
		set_zd_long("0460",sMdm.beg_date);	/**��������**/
		set_zd_data("0310",cAcno); 	/**�˺�**/
		set_zd_data("0250",sCif.addr);	/**��ַ**/
		set_zd_data("0290",sCif.post_code);	/**��������**/	
	        vtcp_log("[%s][%d]��ѯ��Dd_parm_Sel����[%ld]",__FILE__,__LINE__,sMdm.end_date);
	        vtcp_log("[%s][%d]aaaaaaaaa[%ld]",__FILE__,__LINE__,sMdm.beg_date);
    
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

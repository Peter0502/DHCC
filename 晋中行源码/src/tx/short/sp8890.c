/*************************************************
* �� �� ��: sp8890.c
* ��������������֪ͨ�˺ź˶�
* ��    ��: zgf
* ������ڣ�
* �޸ļ�¼��.....................................................
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
* 2. 
*�������ã�
insert into tx_def values('8890','����֪ͨ�˺ź˶�','10000000000000010000000010100000000010000000000100000000000000000000111100000000000001000000000000000000000000000000000000000100','0','6','0');

insert into tx_sub_def values('8890','����֪ͨ�˺ź˶�','sp8890','0','0');

insert into tx_flow_def values ('8890', 0, '8890', '#$');

insert into rsp_code values('F030','���˻�δǩԼ','');

*************************************************/
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "ln_mst_c.h"
#include "mob_acct_type_c.h"
#include "mdm_ac_rel_c.h"
sp8890()
{
		int g_reply_int;
    char cOpt[2];/**��������0��ͨ1�޸�2ע��**/
    char cAcno[21];
    char cActype[2];
    char cOperation[10];
    struct mob_acct_type_c sMob;
    struct ln_mst_c sLn;
    struct dd_mst_c sDd;
    struct mdm_ac_rel_c sMdmm;
    
    memset(&sMdmm,0x00,sizeof(sMdmm));
    memset(&sLn,0x00,sizeof(sLn));
    memset(&sDd,0x00,sizeof(sDd));
    memset(&sMob,0x00,sizeof(sMob));
    memset(cOpt,0x00,sizeof(cOpt));
    memset(cAcno,0x00,sizeof(cAcno));
    memset(cActype,0x00,sizeof(cActype));
    
    pub_base_sysinit();
    get_zd_data("0370",cAcno);
		if(cAcno[0]>='0' && cAcno[0]<='9'){
			pub_base_old_acno(cAcno);
		}

    get_zd_data("0720",cOpt);
    get_zd_data("0710",cActype);
    
    if(cOpt[0]=='0')
        strcpy(cOperation,"ǩԼ");
    else if(cOpt[0]=='1')
        strcpy(cOperation,"�޸�");
    else
    { 
        strcpy(cOperation,"��Լ");
    }
    
    /**�Թ���˽��ǩԼ�����˻���������ʹ��ǩԼҳ�棬ѡ�����ɾ��ʱ���ж��Ƿ��Ѿ�ǩԼ**/
	  g_reply_int=Mob_acct_type_Sel(g_pub_tx.reply,&sMob,"ac_no='%s' ",cAcno);
	  if(cOpt[0] =='0')
		{
	    if(g_reply_int==0)
	    {
			    vtcp_log("[%s][%d]�˺�[%s]�Ѵ��ڶ���ǩԼ��¼!!!",__FILE__,__LINE__,cAcno);
          sprintf(acErrMsg, "���˺���ǩԼ!!");
          strcpy(g_pub_tx.reply,"F038");
          set_zd_data(DC_GET_MSG,acErrMsg);
          goto ErrExit;
	    }
	  }
 		else
		{
			if(g_reply_int==100)
	    {
	        vtcp_log("[%s][%d]�˺�[%s]��δǩԼ!!!",__FILE__,__LINE__,cAcno);
          sprintf(acErrMsg, "���˺�δǩԼ!!");
          strcpy(g_pub_tx.reply,"F030");
          set_zd_data(DC_GET_MSG,acErrMsg);
          goto ErrExit;
	    }
	  }	
     set_zd_data("0580",sMob.mobile);
     vtcp_log("mobile==[%s]",sMob.mobile);
    /**3Ϊ�����1Ϊ���ڻ�***/
		/*if(sMob.ac_type[0]=='3')*/
		vtcp_log("cActype==[%s]",cActype);
		if(!strcmp(cActype,"3"))
		{
			/*****��ѯ�Ƿ��ǻ����˻�*******/
			g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&sLn,"pact_no like '%%%s%%' and  prdt_no not in ('3Y1','3Z1','3Z2','3Z3','3Z4')",cAcno);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Ln_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D104");
	        goto ErrExit;
	    }/**��ѯ�˻��Ƿ�����***/
	    if(sLn.ac_sts[0]=='*')
      {
          sprintf(acErrMsg,"���˺�������!! [%d]",g_reply_int);
          WRITEMSG
          strcpy(g_pub_tx.reply,"P245");
          goto ErrExit;
      }
	    g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdmm,"ac_id= %ld ",sLn.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Mdm_ac_rel_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D104");
	        goto ErrExit;
	    }
		}
		else
		{
			g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdmm,"ac_no='%s'",cAcno);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Mdm_ac_rel_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D104");
	        goto ErrExit;
	    }
	    /*****��ѯ�Ƿ��ǻ����˻�*******/ 
	    /** 20151127 wudawei ��104��̫������Ʒ  **/
	  	g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&sDd,"ac_id=%ld and prdt_no in('101','104','121','122','123','124','125','126','127','128','12A','12B','12C','135','136','146')",sMdmm.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Dd_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }/**��ѯ�˻��Ƿ�����***/
      if(sDd.ac_sts[0]=='*')
      {
          sprintf(acErrMsg,"���˺�������!! [%d]",g_reply_int);
          WRITEMSG
          strcpy(g_pub_tx.reply,"P245");
          goto ErrExit;
      }
      

	  }
	 
    vtcp_log("[%s][%d] ����֪ͨ�˺ź˶Խ��׳ɹ�!!!!!",__FILE__,__LINE__);
    
GoodExit:
		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg,"����֪ͨ�˺ź˶Գɹ�: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"����֪ͨ�˺ź˶Դ���: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
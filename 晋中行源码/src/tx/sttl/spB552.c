/**************************************************************
* �� �� ��:  spB552.c �ж�֪ͨ(����)
* ��������:  
*           ��ҵ�������Ŵ�ϵͳ�����󣬽��жһ�Ʊ������Ϣ
*       ��д������ϵͳ�ĳжһ�Ʊ������Ϣ�Ǽǲ��У�ũ
*       ����ϵͳ�õ������������ᷢ�ͱ��ĵ�ƽ̨��ƽ
*       ̨�����ƽ̨����ת��������ϵͳ,������ʵ�ִ�
*       ƽ̨���ձ���ת�������ĵĹ��ܡ�
* ��    ��:  chenchao
* ������ڣ� 2010-4-6 9:37:33
*
* �޸ļ�¼��
*   ��    ��:
*   �� �� ��:
*   �޸�����:
insert into tx_def values ('4952','�ж�֪ͨ(����)','10000000000000000000000000000000000000000100000000000000010000000000000000000000111000000000000000000000000000000000000000000000','1','3','2','3',null)
insert into tx_flow_def  values ('4952',0,'B552','#=0001@1@#$')
insert into tx_flow_def  values ('4952',1,'B333','#=0000@2@#$')
insert into tx_flow_def  values ('4952',2,'D003','#=0000@3@#$')
insert into tx_flow_def  values ('4952',3,'A017','#$')
insert into tx_sub_def values ('B552','�ж�֪ͨ(����)','spB552','0','0')
*****************************************************************/
#define ERR_DEAL if( ret ) \
{\
    sprintf( acErrMsg, "sql error" ); \
    WRITEMSG \
    goto ErrExit; \
}
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "ecds_recv_reg_c.h"
#include "mo_bank_po_c.h"
#include "mdm_ac_rel_c.h"
#include "hv_orno_brno_c.h"

int spB552()
{
    int    ttlnum = 0;    /**��ȡ��������**/
    int    cnt=0;
    int    i = 0;
    int    ret = 0,pub_ret=0,ins_flag=0;
    long   l_date=0;
    
    char   cTmpBuff[101];
    char   ctype[2];
    char   cPn_po[31];
    char   tmpstr[512];
    char   tmpmo[512];
    char   cPo_type[2];
    char   cDrfttyp[5];
    char   cmtname[20];
    char   procstate[10];
    char   cTrace_no[31];
    char   cOut_br_no[33];
    char   wherelist[1024];    /**��ѯ����**/

    double        dSign_amt=0.00;
    
    struct mo_bank_po_c       sMo_bank_po;
    struct mdm_ac_rel_c       sMdm_ac_rel;
    struct hv_orno_brno_c     sHv_orno_brno;
    struct ecds_recv_reg_c    sEcds_recv_reg,sEcdsrecvreg;
    struct mo_bank_acc_po_c   sMo_bank_acc_po;
    
    
    vtcp_log("[%s][%d]", __FILE__,__LINE__);
    
    memset(tmpmo,0x00,sizeof(tmpmo));
    memset(ctype, 0x00, sizeof(ctype));
    memset(cmtname,0x00,sizeof(cmtname));
    memset(wherelist, 0, sizeof(wherelist));
    memset(procstate,0x00,sizeof(procstate));
    memset(cTrace_no,0x00,sizeof(cTrace_no));
    memset(cOut_br_no,0x00,sizeof(cOut_br_no));
    
    memset(&sMo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
    memset(&sEcdsrecvreg,0x00,sizeof(struct ecds_recv_reg_c));
    memset(&sMo_bank_po,0x00,sizeof(struct mo_bank_po_c));
    
    vtcp_log("[%s][%d]", __FILE__,__LINE__);

    pub_base_sysinit();
    get_zd_long("0440",&l_date);                      /*����*/
    get_zd_double("0410",&sMo_bank_acc_po.sign_amt);  /*Ʊ�ݽ��*/
    get_zd_double("0420",&sMo_bank_acc_po.bail_amt);  /*��֤��    */
    get_zd_double("0410",&dSign_amt);                 /*Ʊ�ݽ��*/
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("101F",cTmpBuff);                    /*��ˮ��*/
    zip_space(cTmpBuff);
    memcpy(cTrace_no , cTmpBuff , sizeof(cTrace_no)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0670",cTmpBuff);                     /*Ʊ�ݷ���*/
    zip_space(cTmpBuff);
    memcpy(cPo_type , cTmpBuff , sizeof(cPo_type)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0180",cTmpBuff);                     /*Ʊ�ݷ���*/
    zip_space(cTmpBuff);
    memcpy(cDrfttyp , cTmpBuff , sizeof(cDrfttyp)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("1243",cTmpBuff);                       /*Ʊ�ݺ�*/
    zip_space(cTmpBuff);
    memcpy(cPn_po , cTmpBuff , sizeof(cPn_po)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0810",cTmpBuff);  /*��Ʊ���˺�*/
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.payer_ac_no , cTmpBuff , sizeof(sMo_bank_acc_po.payer_ac_no)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0580",cTmpBuff);                   /*��Ʊ�˿�����*/
    zip_space(cTmpBuff);
    memcpy(cOut_br_no , cTmpBuff , sizeof(cOut_br_no)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0250",cTmpBuff);   /*�տ�������*/
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.payee_name , cTmpBuff , sizeof(sMo_bank_acc_po.payee_name)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    /**
    get_zd_data("0830",cTmpBuff);   *��֤���˺�*
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.bail_ac_no , cTmpBuff , sizeof(sMo_bank_acc_po.bail_ac_no)-1);
    ***/
    get_zd_data("0830",cTmpBuff);   
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.pact_no, cTmpBuff , sizeof(sMo_bank_acc_po.pact_no)-1);
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0930",cTmpBuff);
    zip_space(cTmpBuff);
    memcpy(ctype , cTmpBuff , sizeof(ctype)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0820",cTmpBuff);
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.payee_ac_no , cTmpBuff , sizeof(sMo_bank_acc_po.payee_ac_no)-1);

    vtcp_log("[%s][%d]----%s,dSign_amt[%f]dSign_amt[%f]bail_amt[%s]",__FILE__,__LINE__,sMo_bank_acc_po.payee_ac_no,dSign_amt,sMo_bank_acc_po.bail_amt);
    vtcp_log("[%s][%d]   -------ctype=[%s]",__FILE__,__LINE__,ctype);
    vtcp_log("[%s][%d]",__FILE__,__LINE__);

    /**��ɲ�ѯ����**/
    pub_base_strpack(cOut_br_no);

    if (cPo_type[0] != 'E')
    {
        sprintf(acErrMsg,"�ǵ�Ʊ�˳�cPo_type=[%s]",cPo_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "DP01");
        goto ErrExit;
    }
    ret = Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsrecvreg,"ecds_tx_date = %ld and ecds_trace_no ='%s'",l_date,cTrace_no);
    if(ret == 0)
    {
        if(sEcdsrecvreg.bf_flag[0] == '0' || sEcdsrecvreg.bf_flag[0] == '2')
        {
        	sprintf(acErrMsg,"�ñ�Ʊ���Ѿ�����!! sEcdsrecvreg.bf_flag=[%s]",sEcdsrecvreg.bf_flag);
        	WRITEMSG
        	strcpy ( g_pub_tx.reply , "NXDP" );
        	goto ErrExit;
        }
        else if(sEcdsrecvreg.bf_flag[0] == '1')
        {
        	pub_ret=sql_execute("update ecds_recv_reg  set bf_flag='2' where ecds_tx_date = %ld and ecds_trace_no ='%s' and bf_flag='1' ",l_date,cTrace_no);
        	if(pub_ret)
        	{
        		sprintf(acErrMsg, "update ecds_recv_reg error ,pub_ret [%d]", pub_ret);
              		WRITEMSG
              		strcpy(g_pub_tx.reply,"D104");
              		goto ErrExit;
              	}   
              	ins_flag=1;
              	
        }
        else
        {
        	sprintf(acErrMsg,"������־����,���[%s]",sEcdsrecvreg.bf_flag);
        	WRITEMSG
        	strcpy ( g_pub_tx.reply , "D999" );
        	goto ErrExit;
        }
    }
    else if (ret && ret !=100)
    {
    	sprintf(acErrMsg,"��ѯEcds_recv_reg�����!! [%s]",cOut_br_no);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT03" );
        goto ErrExit;
    }

    ret = Hv_orno_brno_Sel(g_pub_tx.reply,&sHv_orno_brno," or_br_no=%s ",cOut_br_no);
    if (ret)
    {
        sprintf(acErrMsg,"��ѯHv_orno_brno�����!! [%s]",cOut_br_no);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "S029" );
        goto ErrExit;
    }
    vtcp_log("[%s][%d]----%lf", __FILE__,__LINE__,dSign_amt);
    /**��ɲ�ѯ**
    sprintf(wherelist, "payer_ac_no='%s' and payer_br_no='%s' and  payee_ac_no = '%s' and  bail_ac_no='%s' and bail_amt=%lf  and acc_po_sts='1' ",    
        sMo_bank_acc_po.payer_ac_no, sHv_orno_brno.br_no, sMo_bank_acc_po.payee_ac_no, sMo_bank_acc_po.bail_ac_no, sMo_bank_acc_po.bail_amt);  
    ***/
    sprintf(wherelist, "pact_no='%s' and acc_po_sts='1' ",    
        sMo_bank_acc_po.pact_no);  
    vtcp_log("WHERE [%s]", wherelist); 

    ret = Mo_bank_acc_po_Dec_Sel(g_pub_tx.reply, wherelist);
    ERR_DEAL
    ttlnum = 0;

    while (1) 
    {
        ret = Mo_bank_acc_po_Fet_Sel(&sMo_bank_acc_po, g_pub_tx.reply);
        if (ret == 100 )
            break;
        vtcp_log("[%s][%d]", __FILE__,__LINE__);    
        if (ret)
        {
            sprintf(acErrMsg, "��ѯmo_bank_acc_po��!!ret=[%d]", ret);
            WRITEMSG
            strcpy(g_pub_tx.reply, "AT06");
            goto ErrExit;
        }
        ttlnum++;
    }
    ret = Mo_bank_acc_po_Clo_Sel();
    ERR_DEAL

    if (!ttlnum) 
    {
        strcpy(g_pub_tx.reply, "S049");
        goto ErrExit;
    } 
    vtcp_log("[%s][%d]----%lf", __FILE__,__LINE__,dSign_amt);
    if(ins_flag == 0)
    {	
        /*�����Ecds_recv_reg������ֵ,״̬�ֶ�*/
    	sEcds_recv_reg.ecds_tx_date  =l_date;
    	sEcds_recv_reg.trace_no   = 0;
    	sEcds_recv_reg.tx_date    = 0;
    	sEcds_recv_reg.sts[0]     ='1';
    	sEcds_recv_reg.check_flag[0]='1';
    	sEcds_recv_reg.bf_flag[0]='0';
    	sEcds_recv_reg.drftcls[0] =cPo_type[0];
    	sEcds_recv_reg.issedt     = sMo_bank_acc_po.req_date ;
    	sEcds_recv_reg.duedt      = sMo_bank_acc_po.matr_date ;
    	memcpy(sEcds_recv_reg.ecds_trace_no,  cTrace_no , sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
    	memcpy(sEcds_recv_reg.drfttyp      ,  cDrfttyp  , sizeof(sEcds_recv_reg.drfttyp)-1);
    	memcpy(sEcds_recv_reg.drftnb       ,  cPn_po    , sizeof(sEcds_recv_reg.drftnb)-1);
    	memcpy(sEcds_recv_reg.tag_name     , "CBS1000040"     , sizeof( sEcds_recv_reg.tag_name)-1);
    	Ecds_recv_reg_Debug(&sEcds_recv_reg);
    	ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
    	if (ret )
    	{
    	    sprintf(acErrMsg, "����Ecds_recv_reg��!!ret=[%d]", ret);
    	    WRITEMSG
    	    strcpy(g_pub_tx.reply, "NXC8");
    	    goto ErrExit;
    	}
    }
    cnt = sql_max_int("mo_bank_po","pact_no_num","pact_no='%s'",sMo_bank_acc_po.pact_no);
    vtcp_log("[%s][%d] cnt=[%d] ",__FILE__,__LINE__,cnt);
    ret = Mdm_ac_rel_Sel (g_pub_tx.reply,&sMdm_ac_rel," ac_no='%s' ",sMo_bank_acc_po.payer_ac_no);
    if (ret)
    {
        sprintf(acErrMsg,"��ѯMdm_ac_rel�����!! [%s]",cOut_br_no);  
        WRITEMSG
        strcpy ( g_pub_tx.reply , "S029" );
        goto ErrExit;
    }
    /**********************************ΪB333����׼��������***********************************/
    vtcp_log("[%s][%d] ΪB333׼������[%s][%s][%f][%s]",__FILE__,__LINE__,sMo_bank_acc_po.pact_no,cPn_po,dSign_amt,sMo_bank_acc_po.payee_ac_no);
    set_zd_data("0330", sMo_bank_acc_po.pact_no);      /* �ж�Э���� */
    set_zd_int("0500", (cnt+1));                       /* ��� */
    set_zd_data("0580", cPn_po);                       /* �жһ�Ʊ���� */
    set_zd_double("100E",dSign_amt);                   /* Ʊ���� */
    set_zd_data("0270",sMo_bank_acc_po.payee_name);    /* �տ���ȫ��*/
    set_zd_data("0830",sMo_bank_acc_po.payee_ac_no);   /* �տ����˺�*/
    set_zd_data("0810",sMo_bank_acc_po.payee_br_name); /* �տ��˿���������*/
    set_zd_data("0930","E");                              /*��ƱֽƱ��־**/
    /**********************************ΪD003����׼��������***********************************/
    vtcp_log("[%s][%d] ΪD099׼������ [%s][%f][%s]",__FILE__,__LINE__,sMdm_ac_rel.id_type,sMo_bank_po.po_amt,sMo_bank_acc_po.payer_ac_no);
    set_zd_data("1023","111");                /*��Ʒ���*/
    set_zd_data("102G", sMdm_ac_rel.id_type);
    set_zd_data("102A", sMdm_ac_rel.id_no );
    set_zd_long("102E",sMo_bank_acc_po.matr_date);
    /*set_zd_double("102F",1);                       ������????????????????*/
    set_zd_double("102F",0);                       /*�����Ѹĳ���1--0 by 20150123 xmh*/
    set_zd_data("102J","01" );
    set_zd_data("102K","2" );
    set_zd_data("102Y", "���л��");
    set_zd_data("102O","0" );                /*�Ƿ���۱�־*/
    set_zd_data("1021",  sMo_bank_acc_po.payer_ac_no);
    set_zd_long("1022", sMdm_ac_rel.ac_seqn);
    vtcp_log("[%s][%d]----sMdm_ac_rel.ac_seqn=%d", __FILE__,__LINE__,sMdm_ac_rel.ac_seqn);
    set_zd_data("0910",sMo_bank_acc_po.tx_br_no );
    set_zd_data("0810", "���л��");
    set_zd_int("0193",9001);
    /**********************************ΪA017����׼��������***********************************/
    vtcp_log("[%s][%d] ΪA017׼������sMo_bank_acc_po.payee_ac_no[%s],sMo_bank_po.po_no[%s]",__FILE__,__LINE__,sMo_bank_acc_po.payee_ac_no,cPn_po);
    set_zd_data("0650", "51110");/*�����������Ŀ*/

    

GoodExit:
    strcpy(g_pub_tx.reply, "0001");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
    {
    	memcpy(g_pub_tx.reply,"B552",4);
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}

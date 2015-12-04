/*************************************************
* �� �� ��:  spB557.c
*             ת�������֪ͨ
*             ��Ϊ�����У��յ�ת�������֪ͨ��Ҫ������������
*             ��Ϊ�����У��յ�ת�������֪ͨ�������к���ϵͳ����������
* ��    ��:  chenchao
* ������ڣ� 2010-4-10 10:47:25
*
* �޸ļ�¼��
*   ��    ��:
*   �� �� ��:  xyy
*   �޸�����:
*   ע    �⣺ ũ�������ĵ�ͬ־˵���������ֻ������������ģ���ô��˵�����ܴ���ͬ�������ĵ�ҵ��xyy
insert into tx_def values ('4957','ת�������֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null);
insert into tx_sub_def values ('B557','ת�������֪ͨ','spB557','0','0');

insert into tx_flow_def  values ('4957','0','B557','#=0001@1@#=0002@9@#$')  ;
insert into tx_flow_def  values ('4957','1','B914','#=0001@2@#$')  ;
insert into tx_flow_def  values ('4957','2','b016','#=0000@5@#$')  ;        
insert into tx_flow_def  values ('4957','5','L201','#=0000@6@#$')           ;
insert into tx_flow_def  values ('4957','6','L320','#=0001@7@#$')           ;
insert into tx_flow_def  values ('4957','7','A016','#=0000@8@#$')           ;
insert into tx_flow_def  values ('4957','8','A017','#$')                    ;
insert into tx_flow_def  values ('4957','9','B925','#=0002@10@#$');
insert into tx_flow_def  values ('4957','10','a017','#$')                   ;



insert into  tx_sub_rel  values ('4957','D252','1','01','1204','null'                 );
insert into  tx_sub_rel  values ('4957','D252','1','2','1205','null'                  );
insert into  tx_sub_rel  values ('4957','D252','0','1002','1208','null'               );
insert into  tx_sub_rel  values ('4957','D252','1','ת����ת�뵽�ڻع�','120A','null' );
insert into  tx_sub_rel  values ('4957','a016','1','2620402','1201','null'            );
insert into  tx_sub_rel  values ('4957','a016','1','2','1205','null'                  );
insert into  tx_sub_rel  values ('4957','a016','0','1002','1208','null'               );
insert into  tx_sub_rel  values ('4957','a016','1','ת����ת�뵽�ڻع��������','120A','null');
                                                                                       

insert into  tx_sub_rel  values ('4957','L201','1','01','0210','null'                 );
insert into  tx_sub_rel  values ('4957','L201','1','9','0660','null'                  );
insert into  tx_sub_rel  values ('4957','L201','0','0330','1071','null'               );
insert into  tx_sub_rel  values ('4957','L201','1','0','1072','null'                  );
insert into  tx_sub_rel  values ('4957','L201','1','1','107A','null'                  );
insert into  tx_sub_rel  values ('4957','b016','0','0300','1201','null'               );
insert into  tx_sub_rel  values ('4957','b016','1','2','1205','null'                  );
insert into  tx_sub_rel  values ('4957','b016','0','0890','1206','ƾ֤����'           );
insert into  tx_sub_rel  values ('4957','b016','0','0580','1207','ƾ֤��'             );
insert into  tx_sub_rel  values ('4957','b016','0','1002','1208','null'               );
insert into  tx_sub_rel  values ('4957','b016','1','���ֻ������','120A','null');

insert into  tx_sub_rel  values ('4957','a017','0','0310','1211','�˺�'               );
insert into  tx_sub_rel  values ('4957','a017','1','01','1214','����'                 );
insert into  tx_sub_rel  values ('4957','a017','1','2','1215','��ת��־'              );
insert into  tx_sub_rel  values ('4957','a017','0','1003','1218','���'               );
insert into  tx_sub_rel  values ('4957','a017','1','ת����ת�����ڻع�','121A','ժҪ' );

insert into  tx_sub_rel  values ('4957','A017','0','1211','1211','�˺�'               );



insert into tx_dc_rel  values ('B557', '0000', 'B55700');
insert into dc_entry  values ('B55700', 1, '0210', '23301', '1', '1', '1', '0660', '0400', 'ת���ָ�ծ����ֵ');
insert into dc_entry  values ('B55700', 2, '0210', '23302', '1', '1', '1', '0660', '0410', 'ת���ָ�ծ����Ϣ����');
insert into dc_entry  values ('B55700', 3, '0210', '52207', '1', '1', '1', '0660', '1001', 'ת������Ϣ֧��');
insert into dc_entry  values ('B55700', 4, '0210', '23302', '2', '1', '1', '0660', '1001', 'ת���ָ�ծ����Ϣ����');
insert into dc_entry  values ('B55700', 5, '0210', '12922', '2', '1', '1', '0660', '1004', 'ת���֡���Ϣ����');

*************************************************/
#define ERR_DEAL if( ret ) \
{\
    sprintf( acErrMsg, "sql error" ); \
    WRITEMSG \
    goto ErrExit; \
}
#define EXTERN
#include "public.h" 
#include "hv_define.h" 
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "ln_mst_c.h"
#include "hv_uniontab_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "hv_orno_brno_c.h"

struct mo_discnt_c    sMo_discnt; 
double      amt=0.00;                               /* ʵ�����            */ 
double      drftAmt=0.00;                           /* Ʊ����            */
int 
spB557()
{ 
    struct ecds_recv_reg_c    sEcds_recv_reg,sEcdsrecvreg;   
    struct hv_uniontab_c    sHv_uniontab;
    struct hv_uniontab_c    s_Hv_uniontab;
    struct hv_orno_brno_c    sHv_orno_brno;
    struct hv_orno_brno_c    s_Hv_orno_brno;
    struct com_parm_c    sCom_parm;
    struct mdm_ac_rel_c    sMdm_ac_rel;

    /*���յı�������*/
    int         ret = 0,pub_ret=0,ins_flag=0;

    long        dt=0;                                   /* ����                */ 
    long        l_date=0;
    long        dueDt=0;                                /* ������              */ 
    
    char        cKinbr[7];                              /*�������             */
    char        stlFlg[4+1];                            /* �����־            */ 
    char        drftCls[1+1];                           /* Ʊ�ݷ���            */ 
    char        drftTyp[4+1];                           /* Ʊ������            */ 
    char        drftNb[30+1];                           /* Ʊ�ݺ�              */ 
    char        intrstTyp[1+1];                         /* ��������            */ 
    char        tranSeqNb[28+1];                        /* ������ˮ��          */ 
    char        cTmpBuff[101];
    char        origDscntInNm[60+1];                    /* ԭ����������        */ 
    char        agtAcctSvcr_1[12+1];                    /* (ԭ����)�н����к�*/ 
    char        origDscntOutNm[60+1];                   /* ԭ����������        */ 
    char        agtAcctSvcr_2[12+1];                    /* (ԭ����)�н����к�*/ 
    char        origDscntInAcct[32+1];                  /* ԭ�������˺�        */ 
    char        origDscntOutAcct[32+1];                 /* ԭ�������˺�        */ 
    char        origDscntOutAcctSvcr[12+1];             /* ԭ�������к�        */ 
    char        origDscntInAcctSvcr[12+1];              /* ԭ�������к�        */ 
		char        lw_ind[2];

    double      intrstRate=0.00;                        /* ����                */

    /*end*/

    vtcp_log("[%s][%d]", __FILE__,__LINE__);  
    memset(&sHv_uniontab,0x00,sizeof(struct hv_uniontab_c));
    memset(&s_Hv_orno_brno,0x00,sizeof(struct hv_uniontab_c));
    memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
    memset(&sEcdsrecvreg,0x00,sizeof(struct ecds_recv_reg_c));
    memset(&sMo_discnt,0x00,sizeof(struct mo_discnt_c));
    memset(&sCom_parm,0x00,sizeof(struct com_parm_c));

    memset(cKinbr                ,0x00,sizeof(cKinbr              ));
    memset(stlFlg                ,0x00,sizeof(stlFlg              ));
    memset(drftCls               ,0x00,sizeof(drftCls             ));
    memset(drftTyp               ,0x00,sizeof(drftTyp             ));
    memset(drftNb                ,0x00,sizeof(drftNb              ));
    memset(cTmpBuff              ,0x00,sizeof(cTmpBuff            ));
    memset(intrstTyp             ,0x00,sizeof(intrstTyp           ));
    memset(tranSeqNb             ,0x00,sizeof(tranSeqNb           ));
    memset(agtAcctSvcr_2         ,0x00,sizeof(agtAcctSvcr_2       ));
    memset(origDscntInNm         ,0x00,sizeof(origDscntInNm       ));
    memset(agtAcctSvcr_1         ,0x00,sizeof(agtAcctSvcr_1       ));
    memset(origDscntOutNm        ,0x00,sizeof(origDscntOutNm      ));
    memset(origDscntInAcct       ,0x00,sizeof(origDscntInAcct     ));
    memset(origDscntOutAcct      ,0x00,sizeof(origDscntOutAcct    ));
    memset(origDscntOutAcctSvcr  ,0x00,sizeof(origDscntOutAcctSvcr));
    memset(origDscntInAcctSvcr   ,0x00,sizeof(origDscntInAcctSvcr ));
    memset(lw_ind   ,0x00,sizeof(lw_ind ));

    vtcp_log("[%s][%d]", __FILE__,__LINE__);
    pub_base_sysinit();

    get_zd_long("0440",&dt                  );                /* ����                */
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("101F",cTmpBuff           );                 /* ������ˮ��          */   
    zip_space(cTmpBuff);
    memcpy(tranSeqNb , cTmpBuff , sizeof(tranSeqNb)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0580", cTmpBuff);                           /* ԭ�������к�        */
    zip_space(cTmpBuff);
    memcpy(origDscntInAcctSvcr ,cTmpBuff , sizeof(origDscntInAcctSvcr)-1); 
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0810",cTmpBuff     );                       /* ԭ�������˺�        */     
    zip_space(cTmpBuff);
    memcpy(origDscntInAcct, cTmpBuff , sizeof(origDscntInAcct)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0980",cTmpBuff     );                       /* ԭ�������˺�        */     
    zip_space(cTmpBuff);
    memcpy(lw_ind, cTmpBuff , sizeof(lw_ind)-1);
   
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0961",cTmpBuff       );                     /* ԭ����������        */
    zip_space(cTmpBuff);
    memcpy(origDscntInNm , cTmpBuff , sizeof(origDscntInNm)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0590",cTmpBuff       );                     /* (ԭ����)�н����к�  */
    zip_space(cTmpBuff);
    memcpy(agtAcctSvcr_1 , cTmpBuff , sizeof(agtAcctSvcr_1)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0600",cTmpBuff);                            /* ԭ�������к�        */  
    zip_space(cTmpBuff);
    memcpy(origDscntOutAcctSvcr , cTmpBuff , sizeof(origDscntOutAcctSvcr)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0820",cTmpBuff    );                         /* ԭ�������˺�        */    
    zip_space(cTmpBuff);
    memcpy(origDscntOutAcct , cTmpBuff , sizeof(origDscntOutAcct)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff)); 
    get_zd_data("0962",cTmpBuff      );                        /* ԭ����������        */     
    zip_space(cTmpBuff);
    memcpy(origDscntOutNm , cTmpBuff , sizeof(origDscntOutNm)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0610",cTmpBuff       );                 /* (ԭ����)�н����к�  */     
    zip_space(cTmpBuff);
    memcpy(agtAcctSvcr_2 , cTmpBuff , sizeof(agtAcctSvcr_2)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("101G",cTmpBuff              );                 /* �����־            */     
    zip_space(cTmpBuff);
    memcpy(stlFlg , cTmpBuff , sizeof(stlFlg)-1);
    
    get_zd_data("0670",drftCls             );                 /* Ʊ�ݷ���            */     
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0910",cTmpBuff             );                 /* Ʊ������            */     
    zip_space(cTmpBuff);
    memcpy(drftTyp , cTmpBuff , sizeof(drftTyp)-1);
    
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("1032",cTmpBuff              );                 /* Ʊ�ݺ�              */     
    zip_space(cTmpBuff);
    memcpy(drftNb , cTmpBuff , sizeof(drftNb)-1);

    get_zd_double("0390",&drftAmt          );                 /* Ʊ����            */        
    get_zd_long("0450",&dueDt              );                 /* ������              */     
    get_zd_data("0690",intrstTyp           );                 /* ��������            */     
    get_zd_double("0840",&intrstRate       );                 /* ����                */         
    get_zd_double("0420",&amt              );                 /* ʵ�����            */        


    vtcp_log("[%s][%d]����=[%d]",__FILE__,__LINE__,dt);
    vtcp_log("[%s][%d]������ˮ��=[%s]",__FILE__,__LINE__,tranSeqNb);
    vtcp_log("[%s][%d]ԭ�������к�=[%s]",__FILE__,__LINE__,origDscntInAcctSvcr);
    vtcp_log("[%s][%d]ԭ�������˺�=[%s]",__FILE__,__LINE__,origDscntInAcct);
    vtcp_log("[%s][%d]ԭ����������=[%s]",__FILE__,__LINE__,origDscntInNm);
    vtcp_log("[%s][%d](ԭ����)�н����к�=[%s]",__FILE__,__LINE__,agtAcctSvcr_1);
    vtcp_log("[%s][%d]ԭ�������к�=[%s]",__FILE__,__LINE__,origDscntOutAcctSvcr);
    vtcp_log("[%s][%d]ԭ�������˺�=[%s]",__FILE__,__LINE__,origDscntOutAcct);
    vtcp_log("[%s][%d]ԭ����������=[%s]",__FILE__,__LINE__,origDscntOutNm);
    vtcp_log("[%s][%d](ԭ����)�н����к�=[%s]",__FILE__,__LINE__,agtAcctSvcr_2);
    vtcp_log("[%s][%d]�����־=[%s]",__FILE__,__LINE__,stlFlg);
    vtcp_log("[%s][%d]Ʊ�ݷ���=[%s]",__FILE__,__LINE__,drftCls);
    vtcp_log("[%s][%d]Ʊ������=[%s]",__FILE__,__LINE__,drftTyp);
    vtcp_log("[%s][%d]Ʊ�ݺ�=[%s]",__FILE__,__LINE__,drftNb);
    vtcp_log("[%s][%d]Ʊ����=[%lf]",__FILE__,__LINE__,drftAmt);
    vtcp_log("[%s][%d]������=[%d]",__FILE__,__LINE__,dueDt);
    vtcp_log("[%s][%d]��������=[%s]",__FILE__,__LINE__,intrstTyp);
    vtcp_log("[%s][%d]����=[%lf]",__FILE__,__LINE__,intrstRate);
    vtcp_log("[%s][%d]ʵ�����=[%lf]",__FILE__,__LINE__,amt);
    
    
    ret = Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsrecvreg,"ecds_tx_date = %ld and ecds_trace_no ='%s'",dt , tranSeqNb);
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
        	pub_ret=sql_execute("update ecds_recv_reg  set bf_flag='2' where ecds_tx_date = %ld and ecds_trace_no ='%s' and bf_flag='1' ",dt , tranSeqNb);
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
    	sprintf(acErrMsg,"��ѯEcds_recv_reg�����!! [%s]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT03" );
        goto ErrExit;
    }
    vtcp_log("[%s][%d]drftNb=%sdrftTyp=%s origDscntOutAcctSvcr=%s",__FILE__,__LINE__,drftNb,drftTyp,origDscntOutAcctSvcr);
    if(!memcmp(drftTyp,"AC01",sizeof(drftTyp)-1))
    {
        memset(drftTyp,0x00,sizeof(drftTyp));
        memcpy(drftTyp,"111",3);
    }
    else if (!memcmp(drftTyp,"AC02",sizeof(drftTyp)-1))
    {
        memset(drftTyp,0x00,sizeof(drftTyp));
        memcpy(drftTyp,"112",3);
    }
    else {
        sprintf(acErrMsg, "���ʹ���  ����[%s]",drftTyp);
        WRITEMSG
        goto ErrExit;
    }
    		ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt," pnote_no= '%s' and  note_type ='%s' and sts ='3'", drftNb,drftTyp);     
    		if (ret)
    		{
        		sprintf(acErrMsg,"��ѯmo_discnt�����!!ret= [%d]",ret);
        		WRITEMSG
        		strcpy ( g_pub_tx.reply , "B112" );
        		goto ErrExit;
    		}
    

    ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab," or_br_no='%s' ",origDscntInAcctSvcr);
    if (ret)
    {
        sprintf(acErrMsg,"��ѯHv_uniontab����!! ret=[%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    ret = Hv_uniontab_Sel(g_pub_tx.reply,&s_Hv_uniontab," or_br_no= '%s' ",origDscntOutAcctSvcr);
    if (ret)
    {
        sprintf(acErrMsg,"��ѯs_Hv_uniontab�����!! ret [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    
    vtcp_log("[%s][%d]----", __FILE__,__LINE__);
    if(ins_flag == 0) 
    {
    	sEcds_recv_reg.sts[0]='1';
    	sEcds_recv_reg.check_flag[0]='1'; 
    	sEcds_recv_reg.bf_flag[0]='0';   
    	sEcds_recv_reg.ecds_tx_date           =dt;
    	sEcds_recv_reg.issedt                 = 0 ;
    	sEcds_recv_reg.duedt                  = dueDt ;
    	sEcds_recv_reg.trace_no               = 0;
    	sEcds_recv_reg.tx_date                = g_pub_tx.tx_date; 
    	memcpy(sEcds_recv_reg.tag_name,       "CBS1000090",sizeof( sEcds_recv_reg.tag_name)-1);       
    	memcpy(sEcds_recv_reg.drftcls,        drftCls,sizeof(sEcds_recv_reg.drfttyp)-1);     
    	memcpy(sEcds_recv_reg.drfttyp ,       drftTyp,sizeof(sEcds_recv_reg.drftnb)-1);     
    	memcpy(sEcds_recv_reg.drftnb ,        drftNb,    sizeof(sEcds_recv_reg.drftnb)-1); 
    	memcpy(sEcds_recv_reg.ecds_trace_no,  tranSeqNb,sizeof(sEcds_recv_reg.ecds_trace_no)-1);   
    	
    	ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
    	if (ret )
    	{
    	    Ecds_recv_reg_Debug(&sEcds_recv_reg);
    	    sprintf(acErrMsg, "����Ecds_recv_reg��!!ret=[%d]", ret);
    	    WRITEMSG
    	        strcpy(g_pub_tx.reply, "NXC8");
    	    goto ErrExit;
    	}
    }
    
    
    if (memcmp(sHv_uniontab.qs_no ,s_Hv_uniontab.qs_no ,sizeof(sHv_uniontab.qs_no)-1) !=0 )/*��ͬ������*/
    {
        vtcp_log("[%s][%d]qs_no1=[%s]qs_no2[%s]",__FILE__,__LINE__,sHv_uniontab.qs_no,s_Hv_uniontab.qs_no);
        if ( memcmp(HV_QS_BR_NO,sHv_uniontab.qs_no,12) ==0   )/*��������*/
        {
            vtcp_log("[%s][%d]���������д����֧,sHv_uniontab.qs_no=[%s]",__FILE__,__LINE__,sHv_uniontab.qs_no);
            GetBr_noByOr_br_no(origDscntInAcctSvcr,cKinbr);
            ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",\
            PARM_ECDS_TR_PAY_AC,cKinbr);
            if (ret)
            {
                sprintf(acErrMsg,"��ѯCom_parm�����!!ret  [%d]",ret);
                WRITEMSG
                strcpy ( g_pub_tx.reply , "AT06" );
                goto ErrExit;
            }
             vtcp_log("[%s][%d]----������sCom_parm.val=[%s]",__FILE__,__LINE__,sCom_parm.val);
        
#if 0       
            memcpy(g_pub_tx.ac_get_ind,"000",3);
            g_pub_tx.tx_amt1=drftAmt-amt;
            strcpy(g_pub_tx.ac_wrk_ind, "000000001");
            
            strcpy(g_pub_tx.intst_type, "1");/* ��Ϣ���� */
            strcpy(g_pub_tx.add_ind, "0");  /* ����:0��1��2���� */
            strcpy(g_pub_tx.prt_ind, "0");  /* �Ƿ��ӡ����(��������:1��0�� */
            strcpy(g_pub_tx.hst_ind, "1");  /* ����ϸ:1�ռ���2���յ� ���ջ����� */
            strcpy(g_pub_tx.brf, "ת���֡���Ϣ����");
            sprintf(acErrMsg, "Pub_tx�ṹ��ֵ PASS!");
            WRITEMSG
            
            /*** ���׼��ʴ��� ***/
            if (pub_acct_trance())
            {
            	sprintf(acErrMsg, "���׼��ʴ������!");
            	WRITEMSG
            	goto ErrExit;
            }
            sprintf(acErrMsg, "���׼��ʴ��� PASS!");
            WRITEMSG
#endif
            set_zd_data("0210", "01");
            memcpy(g_pub_tx.ac_no, "50107",sizeof(g_pub_tx.ac_no));/* �˻� */
            set_zd_double("1004",amt-drftAmt);
            strcpy(g_pub_tx.sub_tx_code, "B557");
            ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
            if (ret != 0)
            {
            	vtcp_log("��Ƽ��ʲ��ɹ�[%s][%ld]",__FILE__,__LINE__);
            	sprintf(acErrMsg, "��Ƽ��ʲ��ɹ�!!");
            	WRITEMSG
            	goto ErrExit;
            }
            
  ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_id=%d ",sMo_discnt.ac_id);
    if (ret)
    {
               sprintf(acErrMsg, "ִ��sMdm_ac_rel��[%d]sMo_discnt.ac_id=%d", ret,sMo_discnt.ac_id);
               WRITEMSG
               strcpy(g_pub_tx.reply, "M003");
               goto ErrExit;
    }          
            /************************ΪspB914׼����********************************/
            set_zd_data("0620", sMo_discnt.pact_no);               /* ����Э����*/
            set_zd_data("0670", "0");                              /* �����ʻ�����*/
            set_zd_data("0680","2");                               /*��ת��־     */
            set_zd_data("0330", sMdm_ac_rel.ac_no);                /* �����˺�    */
            
            /*******************ΪA016׼������**************************************/
            set_zd_data("0300",sCom_parm.val);
            set_zd_double("1002",drftAmt);
            set_zd_double("1001",amt);
            /*******************ΪL201׼������**************************************/
            set_zd_double("107B",drftAmt);
            
            /************************ΪspA017׼����*********************************/
            set_zd_data("1211",sCom_parm.val);
            set_zd_double("1218",amt);
            set_zd_data("121A","ת�������֪ͨ");
            set_zd_data("1215","2");
            set_zd_data("1214","01");

            strcpy ( g_pub_tx.reply , "0001" );
            goto GoodExit1;  
        }
        else if ( memcmp(HV_QS_BR_NO,s_Hv_uniontab.qs_no,12) == 0  )    /*�������� */
        {
            vtcp_log("[%s][%d]���������д����֧,s_Hv_uniontab.qs_no=[%s]",__FILE__,__LINE__,s_Hv_uniontab.qs_no);
            GetBr_noByOr_br_no(origDscntOutAcctSvcr,cKinbr);
            ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",\
            PARM_ECDS_TC_AC,cKinbr);/**PARM_ECDS_TC_AC**/
            if (ret)
            {
                sprintf(acErrMsg,"��ѯCom_parm�����!!ret  [%d]",ret);
                WRITEMSG
                strcpy ( g_pub_tx.reply , "AT06" );
                goto ErrExit;
            }
             vtcp_log("[%s][%d]----������sCom_parm.val=[%s]",__FILE__,__LINE__,sCom_parm.val);
            /********************************ΪB935׼������******************************/
            set_zd_data("0630", sMo_discnt.tpact_no);                   /* ת����Э����*/
            set_zd_data("0690",  "2");                                 /* �����ʻ�����  */
            set_zd_double("1002",drftAmt);                                /*Ʊ����       */
            /*****************************ΪA017׼������*********************************/
            set_zd_data("0310",sCom_parm.val);
            set_zd_double("1003",amt);
            /*********************A016**********************/
            set_zd_data("0370",sMo_discnt.tin_ac_no);
            /*****************************�������˲���***********************************/
            set_zd_data("0210", "01");
            set_zd_double("0400",drftAmt);  
            set_zd_double("0410",amt-drftAmt);
            set_zd_double("1001",amt-drftAmt);
            vtcp_log("[%s][%d]0400[%lf]0410[%lf]1001[%lf]1003[%lf]",__FILE__,__LINE__,drftAmt,drftAmt-amt,sMo_discnt.tpn_int-sMo_discnt.tot_int,amt);
            set_zd_data("0660","2");
            strcpy(g_pub_tx.sub_tx_code, "B557");
            ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
            if (ret != 0)
            { 
                  vtcp_log("��Ƽ��ʲ��ɹ�[%s][%ld]",__FILE__,__LINE__);
                  sprintf(acErrMsg, "��Ƽ��ʲ��ɹ�!!");
                  WRITEMSG
                  return 1;
            }
#if 0            
            memcpy(g_pub_tx.ac_get_ind,"000",3);
            g_pub_tx.tx_amt1=drftAmt;
            strcpy(g_pub_tx.ac_wrk_ind, "000000001");
            memcpy(g_pub_tx.ac_no, "23301",sizeof(g_pub_tx.ac_no));/* �˻� */
            strcpy(g_pub_tx.intst_type, "1");/* ��Ϣ���� */
            strcpy(g_pub_tx.add_ind, "0");  /* ����:0��1��2���� */
            strcpy(g_pub_tx.prt_ind, "0");  /* �Ƿ��ӡ����(��������:1��0�� */
            strcpy(g_pub_tx.hst_ind, "1");  /* ����ϸ:1�ռ���2���յ� ���ջ����� */
            strcpy(g_pub_tx.brf, "ת���ָ�ծ����ֵ");
            sprintf(acErrMsg, "Pub_tx�ṹ��ֵ PASS!");
            WRITEMSG
            
            /*** ���׼��ʴ��� ***/
            if (pub_acct_trance())
            {
            	sprintf(acErrMsg, "���׼��ʴ������!");
            	WRITEMSG
            	goto ErrExit;
            }
#endif
            strcpy ( g_pub_tx.reply , "0002" );
            goto GoodExit1;
        }
        else 
        {
            sprintf(acErrMsg, "������������[%s][%s]",sHv_uniontab.qs_no,s_Hv_uniontab.qs_no);
            WRITEMSG
            goto ErrExit;
        }
    } 
    else
    {
        vtcp_log("[%s][%d]��ͬ������sHv_uniontab.qs_no =[%s]s_Hv_uniontab.qs_no=[%s]",__FILE__,__LINE__,sHv_uniontab.qs_no ,s_Hv_uniontab.qs_no);
        if(memcmp(stlFlg,"SM00",sizeof(stlFlg)-1) == 0)
        {
            sprintf(acErrMsg,"ͬһ�����в�������������,����");
            WRITEMSG
            strcpy ( g_pub_tx.reply , "P068" );
            goto ErrExit;
        }
        sprintf(acErrMsg,"ͬһ�����в�����������!!!error");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "P068" );
        goto ErrExit;
    }    
GoodExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
GoodExit1:
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
     if(memcmp(g_pub_tx.reply,"0000",4)==0)
    {
    	memcpy(g_pub_tx.reply,"B557",4);
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;

}

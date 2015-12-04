/*************************************************
* �� �� ��:  spB561.c
*	��ʾ����֪ͨ
*	����Ϊ�ж��У��������У�ʱ�����ڳжһ�Ʊ���ڴ��������Ѿ�����֤��Ȼ���Ӧ�����У�
*	���Ա��ĵ�����ƽ̨Ϊֹ������Ҫ�����ҵ��ϵͳ���ͣ���������Ѿ��ڳжҵ���ʱ�ͳжҸ���ʱ����
*	��Ϊ�տ��˿����У�����ƽ̨ת�����ĵ����ģ�����Ҫ����������
*
* ��    ��:  chenchao
* ������ڣ� 2010-4-11 16:34:04
*
* �޸ļ�¼��
*   ��    ��:
*   �� �� ��:
*   �޸�����:
insert into tx_def values ('4961','��ʾ����֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null)
insert into tx_flow_def  values ('4961','0','B561','#=0001@1@#$')
insert into tx_flow_def  values ('4961','1','A016','#=0000@2@#$');
insert into tx_flow_def  values ('4961','2','A017','#$');
insert into tx_sub_def values ('B561','��ʾ����֪ͨ','spB561','0','0')


*************************************************/
#define ERR_DEAL if( ret ) \
{\
	sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
}
#define EXTERN
#include "public.h" 
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "ecds_recv_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"
#include "mo_discnt_c.h"
#include "hv_uniontab_c.h"
int 
spB561()
{ 
	struct ecds_recv_reg_c	sEcds_recv_reg,sEcdsrecvreg;    
	struct mo_bank_acc_po_c	sMo_bank_acc_po;
	struct mo_bank_po_c	sMo_bank_po;
	struct com_parm_c	sCom_parm;
	struct mdm_ac_rel_c	sMdm_ac_rel;
	/*���ܵı�������*/
	long	dt                =0    ;              /* ����          */
	char	tranSeqNb         [28+1];              /* ������ˮ��    */
	char	accptrAcctSvcr    [12+1];              /* �ж����к�    */
	char	accptrNm          [60+1];              /* �ж�������    */
	char	accptrAcct        [32+1];              /* �ж����˺�    */
	char	agtAcctSvcr_1     [12+1];              /* (�ж�)�н����к�   */
	char	pyeeAcctSvcr      [12+1];              /* �տ����к�    */
	char	pyeeAcct          [32+1];              /* �տ����˺�    */
	char	pyeeNm            [60+1];              /* �տ�������    */
	char	agtAcctSvcr_2     [12+1];              /* (�տ���)�н����к� */
	char	payTyp            [1+1] ;              /* ��ʾ��������  */
	char	stlFlg            [4+1] ;              /* �����־      */
	char	drftCls           [1+1] ;              /* Ʊ�ݷ���      */
	char	drftTyp           [4+1] ;              /* Ʊ������      */
	char	drftNb            [30+1];              /* Ʊ�ݺ�        */
	double	drftAmt           =0.00 ;              /* Ʊ����      */
	long	dueDt             =0    ;              /* ������        */
	/*end*/
	
	char		cKinbr[7];
	char		lw_ind[2];
	char		cTmpBuff[101];
	long		l_date=0;
	int             ttlnum = 0;	/**��ȡ��������**/
	char		wherelist[1024];
	int		cnt=0;
	int             i = 0;
	int         ret = 0,pub_ret=0,ins_flag=0;
	vtcp_log("%s.%d", __FILE__,__LINE__);  
	memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
	memset(&sEcdsrecvreg,0x00,sizeof(struct ecds_recv_reg_c)); 
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));

	memset(cKinbr        ,0x00,sizeof(cKinbr         ));
	memset(tranSeqNb     ,0x00,sizeof(tranSeqNb      ));       /* ������ˮ��    */
	memset(accptrAcctSvcr,0x00,sizeof(accptrAcctSvcr ));       /* �ж����к�    */
	memset(accptrNm      ,0x00,sizeof(accptrNm       ));       /* �ж�������    */
	memset(accptrAcct    ,0x00,sizeof(accptrAcct     ));       /* �ж����˺�    */
	memset(agtAcctSvcr_1 ,0x00,sizeof(agtAcctSvcr_1  ));       /* (�ж�)�н����к�   */
	memset(pyeeAcctSvcr  ,0x00,sizeof(pyeeAcctSvcr   ));       /* �տ����к�    */
	memset(pyeeAcct      ,0x00,sizeof(pyeeAcct       ));       /* �տ����˺�    */
	memset(pyeeNm        ,0x00,sizeof(pyeeNm         ));       /* �տ�������    */
	memset(agtAcctSvcr_2 ,0x00,sizeof(agtAcctSvcr_2  ));       /* (�տ���)�н����к� */
	memset(payTyp        ,0x00,sizeof(payTyp         ));       /* ��ʾ��������  */
	memset(stlFlg        ,0x00,sizeof(stlFlg         ));       /* �����־      */
	memset(drftCls       ,0x00,sizeof(drftCls        ));       /* Ʊ�ݷ���      */
	memset(drftTyp       ,0x00,sizeof(drftTyp        ));       /* Ʊ������      */
	memset(drftNb        ,0x00,sizeof(drftNb         ));       /* Ʊ�ݺ�        */
	memset(lw_ind        ,0x00,sizeof(lw_ind        ));       /* ������ʾ*/

	vtcp_log("%s.%d", __FILE__,__LINE__);
	pub_base_sysinit();

	get_zd_long("0440",&dt             );                        /* ����          */ 
	get_zd_double("0390",&drftAmt      );                        /* Ʊ����      */     
	get_zd_long("0450",&dueDt          );                        /* ������        */
	    
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("101F",cTmpBuff      );                        /* ������ˮ��    */
	zip_space(cTmpBuff);
	memcpy(tranSeqNb , cTmpBuff , sizeof(tranSeqNb)-1);
	
	get_zd_data("0980",lw_ind);	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0580",cTmpBuff );                        /* �ж����к�    */
	zip_space(cTmpBuff);
	memcpy(accptrAcctSvcr , cTmpBuff , sizeof(accptrAcctSvcr)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0961",cTmpBuff       );                        /* �ж�������    */
	zip_space(cTmpBuff);
	memcpy(accptrNm , cTmpBuff , sizeof(accptrNm)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0810",cTmpBuff     );                        /* �ж����˺�    */
	zip_space(cTmpBuff);
	memcpy(accptrAcct , cTmpBuff , sizeof(accptrAcct)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0590",cTmpBuff  );                        /* (�ж�)�н����к�   */
	zip_space(cTmpBuff);
	memcpy(agtAcctSvcr_1 , cTmpBuff , sizeof(agtAcctSvcr_1)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0600",cTmpBuff   );                        /* �տ����к�    */
	zip_space(cTmpBuff);
	memcpy(pyeeAcctSvcr , cTmpBuff , sizeof(pyeeAcctSvcr)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0820",cTmpBuff       );                        /* �տ����˺�    */
	zip_space(cTmpBuff);
	memcpy(pyeeAcct , cTmpBuff , sizeof(pyeeAcct)-1);
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0962",cTmpBuff         );                        /* �տ�������    */
	zip_space(cTmpBuff);
	memcpy(pyeeNm , cTmpBuff , sizeof(pyeeNm)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0610",cTmpBuff  );                        /* (�տ���)�н����к� */ 
	zip_space(cTmpBuff);
	memcpy(agtAcctSvcr_2 , cTmpBuff , sizeof(agtAcctSvcr_2)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0680",cTmpBuff         );                        /* ��ʾ��������  */
	zip_space(cTmpBuff);
	memcpy(payTyp , cTmpBuff , sizeof(payTyp)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("101G",cTmpBuff         );                        /* �����־      */ 
	zip_space(cTmpBuff);
	memcpy(stlFlg , cTmpBuff , sizeof(stlFlg)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0670",cTmpBuff        );                        /* Ʊ�ݷ���      */
	zip_space(cTmpBuff);
	memcpy(drftCls , cTmpBuff , sizeof(drftCls)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0910",cTmpBuff        );                        /* Ʊ������      */ 
	zip_space(cTmpBuff);
	memcpy(drftTyp , cTmpBuff , sizeof(drftTyp)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("1032",cTmpBuff         );                        /* Ʊ�ݺ�        */
	zip_space(cTmpBuff);
	memcpy(drftNb , cTmpBuff , sizeof(drftNb)-1);     
	    

	vtcp_log("[%s][%d]����=[%d]",__FILE__,__LINE__,dt);
	vtcp_log("[%s][%d]������ˮ��=[%s]",__FILE__,__LINE__,tranSeqNb);
	vtcp_log("[%s][%d]�ж����к�=[%s]",__FILE__,__LINE__,accptrAcctSvcr);
	vtcp_log("[%s][%d]�ж�������=[%s]",__FILE__,__LINE__,accptrNm);
	vtcp_log("[%s][%d]�ж����˺�=[%s]",__FILE__,__LINE__,accptrAcct);
	vtcp_log("[%s][%d](�ж�)�н����к�=[%s]",__FILE__,__LINE__,agtAcctSvcr_1);
	vtcp_log("[%s][%d]�տ����к�=[%s]",__FILE__,__LINE__,pyeeAcctSvcr);
	vtcp_log("[%s][%d]�տ����˺�=[%s]",__FILE__,__LINE__,pyeeAcct);
	vtcp_log("[%s][%d]�տ�������=[%s]",__FILE__,__LINE__,pyeeNm);
	vtcp_log("[%s][%d](�տ���)�н����к�=[%s]",__FILE__,__LINE__,agtAcctSvcr_2);
	vtcp_log("[%s][%d]��ʾ��������=[%s]",__FILE__,__LINE__,payTyp);
	vtcp_log("[%s][%d]�����־=[%s]",__FILE__,__LINE__,stlFlg);
	vtcp_log("[%s][%d]Ʊ�ݷ���=[%s]",__FILE__,__LINE__,drftCls);
	vtcp_log("[%s][%d]Ʊ������=[%s]",__FILE__,__LINE__,drftTyp);
	vtcp_log("[%s][%d]Ʊ�ݺ�=[%s]",__FILE__,__LINE__,drftNb);
	vtcp_log("[%s][%d]Ʊ����=[%lf]",__FILE__,__LINE__,drftAmt);
	vtcp_log("[%s][%d]������=[%d]",__FILE__,__LINE__,dueDt);
	 
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
	vtcp_log("%s.%d   -------drftNb=%s  ...drftTyp=%s  ", __FILE__,__LINE__,drftNb,drftTyp );

	if ( '0' == payTyp[0])
	{
		sprintf(acErrMsg,"��Ʊδ����payTyp==[%s]",payTyp);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B359");
		goto ErrExit;
	}
	else if ('1' != payTyp[0] && '2' != payTyp[0])
	{
		sprintf(acErrMsg,"״̬����payTyp==[%s]",payTyp);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P016");
		goto ErrExit;
	}
	ret = Mo_bank_po_Sel( g_pub_tx.reply,&sMo_bank_po,"po_no = '%s' ",drftNb);
	if (ret)
	{
		sprintf(acErrMsg, "��ѯmo_bank_po��!!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B012");
		goto ErrExit;
	}
	sprintf(wherelist, " pact_no='%s' " ,sMo_bank_po.pact_no );  

	vtcp_log("WHERE [%s]", wherelist);

	ret = Mo_bank_acc_po_Dec_Sel(g_pub_tx.reply, wherelist);
	ERR_DEAL
	ttlnum = 0;

	while (1) 
	{
		ret = Mo_bank_acc_po_Fet_Sel(&sMo_bank_acc_po, g_pub_tx.reply);
		if (ret == 100 )
			break;
		vtcp_log("%s.%d", __FILE__,__LINE__);	
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
	vtcp_log("%s.%d---- ", __FILE__,__LINE__);
	
	GetBr_noByOr_br_no(accptrAcctSvcr,cKinbr);
	ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",\
	PARM_ECDS_TR_PAY_AC,cKinbr);
	if (ret)
	{
		sprintf(acErrMsg,"��ѯCom_parm�����!!ret  [%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "AT06" );
		goto ErrExit;
	}

	if(ins_flag == 0) 
	{
		sEcds_recv_reg.ecds_tx_date    		=dt; 
		sEcds_recv_reg.trace_no 		= 0;
		sEcds_recv_reg.tx_date  		= g_pub_tx.tx_date;        
		sEcds_recv_reg.issedt 			= 0;     						/*��̫���*/
		sEcds_recv_reg.duedt	 		= dueDt ;  
		memcpy(sEcds_recv_reg.tag_name,		"CBS1000130",sizeof( sEcds_recv_reg.tag_name)-1);      /*�������������������*/
		memcpy(sEcds_recv_reg.ecds_trace_no,	tranSeqNb,sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
		memcpy(sEcds_recv_reg.drftcls,		drftCls,sizeof(sEcds_recv_reg.drfttyp)-1);     
		memcpy(sEcds_recv_reg.drfttyp ,		drftTyp,sizeof(sEcds_recv_reg.drftnb)-1);     
		memcpy(sEcds_recv_reg.drftnb ,		drftNb,	sizeof(sEcds_recv_reg.drftnb)-1);   
		sEcds_recv_reg.sts[0]='1';
		sEcds_recv_reg.check_flag[0]='1';
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
	if(lw_ind[0]=='1')
	{
		memset(&sCom_parm,0x00,sizeof(sCom_parm));
		GetBr_noByOr_br_no(accptrAcctSvcr,cKinbr);
		ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",10,cKinbr);
	if (ret)
	{
		sprintf(acErrMsg,"��ѯCom_parm�����!!ret  [%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "AT06" );
		goto ErrExit;
	}
	/********************************************/
	set_zd_double("1004",drftAmt);
	set_zd_data("0310",sCom_parm.val);
	/********************************************/
		memset(&sCom_parm,0x00,sizeof(sCom_parm));
		GetBr_noByOr_br_no(accptrAcctSvcr,cKinbr);
		ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",PARM_ECDS_TC_AC,cKinbr);
	if (ret)
	{
		sprintf(acErrMsg,"��ѯCom_parm�����!!ret  [%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "AT06" );
		goto ErrExit;
	}
	
	/*****************ΪA017׼������**************************/
	set_zd_data("1211",sCom_parm.val);
	set_zd_double("1218",drftAmt);
	set_zd_data("121A","��Ʊ���ڸ���");
	set_zd_data("1215","2");
	set_zd_data("1214","01");
	/********************************************************/
		goto GoodExit1;	
	}
	else
	{		
				struct hv_uniontab_c 	sHv_uniontab_in;
				struct hv_uniontab_c  sHv_uniontab_out;	
				memset(&sHv_uniontab_in,0x00,sizeof(sHv_uniontab_in));
				memset(&sHv_uniontab_out,0x00,sizeof(sHv_uniontab_out));
        ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_in," or_br_no=%s ",accptrAcctSvcr);/***�ж����к�**/
        if (ret)
        {
            sprintf(acErrMsg, "[����]��ѯHv_uniontab�����!! ret [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_out," or_br_no=%s ",pyeeAcctSvcr);/**TODO �տ����к�**/
        if (ret)
        {
            sprintf(acErrMsg, "[����]��ѯHv_uniontab�����!! ret [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        if(memcmp(sHv_uniontab_in.qs_no,sHv_uniontab_out.qs_no,12)!=0)
        {
        		GetBr_noByOr_br_no(pyeeAcctSvcr,cKinbr);
						ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",
						PARM_ECDS_TR_PAY_AC,cKinbr);
						if (ret)
						{
							sprintf(acErrMsg,"��ѯCom_parm�����!!ret  [%d]",ret);
							WRITEMSG
							strcpy ( g_pub_tx.reply , "AT06" );
							goto ErrExit;
						}
        }
        else if(memcmp(sHv_uniontab_in.qs_no,sHv_uniontab_out.qs_no,12)==0)
        {
        		GetBr_noByOr_br_no(pyeeAcctSvcr,cKinbr);
						ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",
						PARM_ECDS_TC_AC,cKinbr);
						if (ret)
						{
							sprintf(acErrMsg,"��ѯCom_parm�����!!ret  [%d]",ret);
							WRITEMSG
							strcpy ( g_pub_tx.reply , "AT06" );
							goto ErrExit;
						}
        }
	/****************************ΪA016׼������***************/
	set_zd_data("1201",sCom_parm.val);
	set_zd_double("1208",drftAmt);
	set_zd_data("120A","��Ʊ���ڸ���");
	set_zd_data("1205","2");
	set_zd_data("1204","01");
	if(strlen(pyeeAcct)==1)
	{
   	/********************************ΪL201׼������************/
            set_zd_data("0670","2");
		struct mo_discnt_c sMoDiscnt;
		memset(&sMoDiscnt,0x00,sizeof(sMoDiscnt));
    ret = Mo_discnt_Sel(g_pub_tx.reply, &sMoDiscnt, "pnote_no='%s' and note_type='111' and sts ='3'",drftNb);
    if (ret ==100) {
        sprintf(acErrMsg, "���ֵǼǱ��в����ڴ�Ʊ�����ͺ�Ʊ�ݺ�!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B174");
        goto ErrExit;
    } else if (ret) {
        sprintf(acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    struct mdm_ac_rel_c sMdm_ac_rel;
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
 		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, " ac_id=%ld ", sMoDiscnt.ac_id);
            if (ret) {
               sprintf(acErrMsg, "��ѯmdm_ac_rel����![%s]", sMoDiscnt.ac_id);
               WRITEMSG
               strcpy(g_pub_tx.reply, "L015");
               goto ErrExit;
            }
            set_zd_data("0330",sMdm_ac_rel.ac_no);
  					set_zd_double("1002",drftAmt);
          
   	}
	else
	{
	/********************************ΪD099׼������************/
  set_zd_data("0670","1");
	set_zd_data("1211",pyeeAcct);
	set_zd_double("1218",drftAmt);
	set_zd_data("121A","��Ʊ���ڸ���");
	set_zd_data("1215","2");
	set_zd_data("1214","01");
	
  }
	}

GoodExit:
	strcpy(g_pub_tx.reply, "0001");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
GoodExit1:
	strcpy(g_pub_tx.reply, "0002");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		memcpy(g_pub_tx.reply,"B561",4);
	}
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

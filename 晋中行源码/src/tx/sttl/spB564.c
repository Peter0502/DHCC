/*************************************************
* �� �� ��:  spB564.c
*	��Ʊ֪ͨ
*	Ʊ�ݳжҺ���ʾ��Ʊǰ�������Ʊ������Ʊ����Ʊ�ɹ���
*	��Ϊ�ж��л��յ���֪ͨ���յ���֪ͨ�󽫽���������
*
* ��	��:  chenchao
* ������ڣ� 2010-4-6 9:37:33
*
* �޸ļ�¼��
* ��	��:
* �� �� ��:
* �޸�����:
insert into tx_def values ('4964','��Ʊ֪ͨ','10000000000000000000000000000000000000000100000000000000010000000000000000000000111000000000000000000000000000000000000000000000','1','3','2','3',null);
insert into tx_flow_def  values ('4964',0,'B564','#=0001@1@#$');
insert into tx_flow_def  values ('4964',1,'D003','#=0000@2@#$');
insert into tx_flow_def  values ('4964',2,'D099','#$');
insert into tx_sub_def values ('B564','��Ʊ֪ͨ','spB564','0','0');

insert into tx_sub_rel (tx_code, in_sub_tx, out_ind, out_data, in_data, memo) values ('4964', 'D003', '1', '1', '0191', null);

insert into dc_entry  values ('B56400', 1, '0210', '52107', '1', '1', '1', '0660', '0440', '��Ϣ֧��');

insert into tx_dc_rel (sub_tx_code, dc_code, entry_code) values ('B564', '0000', 'B56400');

*************************************************/
#define MYSQLERR if( ret ) \
{\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit11;\
}
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
#include "hv_uniontab_c.h"
#include "ecds_recv_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"
#include "intst_hst_c.h"
#include "com_sys_parm_c.h"
#include "com_sect_acm_c.h"
#include "com_item_c.h"
#include "mo_bail_rate_c.h"

struct mo_bank_acc_po_c	sMo_bank_acc_po;
int 
spB564()
{ 
	struct ecds_recv_reg_c	sEcds_recv_reg,sEcdsrecvreg;	
	struct mo_bank_po_c	sMo_bank_po;
	struct com_parm_c	sCom_parm;
	struct mdm_ac_rel_c	sMdm_ac_rel;

	/*���ܵı�������*/
	long	Dt=0;				/*����*/
	char	tranSeqNb[28+1];		/*������ˮ��*/
	char	drftCls[1+1] ;			/*Ʊ�ݷ���*/
	char	drftTyp[4+1] ;			/*Ʊ������*/
	char	drftNb[30+1];			/*Ʊ�ݺ�*/
	long	isseDt=0 ;			/*��Ʊ��*/
	long	dueDt=0 ;			/*������*/
	double	drftAmt=0.00;			/*Ʊ����*/
	char	bailAcct[32+1];		 	/*��֤���˺�*/
	double	bailAmt=0.00;			/*��֤��*/
	/*end*/
	
	char		cTmpBuff[101];
	char		cAcc_hrt[9];
	long		l_date=0;
	int		 ttlnum = 0;	/**��ȡ��������**/
	char		wherelist[1024];
	int		cnt=0;
	int		 i = 0;
	double		intst_amt=0.00;
	int         ret = 0,pub_ret=0,ins_flag=0;
	vtcp_log("%s.%d", __FILE__,__LINE__);  
	memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
	memset(&sEcdsrecvreg,0x00,sizeof(struct ecds_recv_reg_c)); 
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));

	memset(tranSeqNb , 0x00,sizeof( tranSeqNb));	 /*������ˮ��*/ 
	memset(drftCls   , 0x00,sizeof( drftCls  ));	 /*Ʊ�ݷ���  */ 
	memset(drftTyp   , 0x00,sizeof( drftTyp  ));	 /*Ʊ������  */ 
	memset(drftNb	, 0x00,sizeof( drftNb   ));	 /*Ʊ�ݺ�	*/ 
	memset(bailAcct  , 0x00,sizeof( bailAcct ));	 /*��֤���˺�*/ 

	vtcp_log("%s.%d", __FILE__,__LINE__);
	pub_base_sysinit();

	get_zd_long("0440",  &Dt	);	/*����*/
	get_zd_long("0460",  &isseDt	);	/*��Ʊ��*/  
	get_zd_long("0450",  &dueDt	 );	/*������*/  
	get_zd_double("0390",  &drftAmt   );	  /*Ʊ����  */
	get_zd_double("0420",  &bailAmt   );	  /*��֤��	*/	  
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("101F",  cTmpBuff );	/*������ˮ��*/ 
	zip_space(cTmpBuff);
	memcpy(tranSeqNb , cTmpBuff , sizeof(tranSeqNb)-1);
	
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0670",  drftCls   );	/*Ʊ�ݷ���*/
	zip_space(cTmpBuff);
	memcpy(drftCls , cTmpBuff , sizeof(drftCls)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0910",  cTmpBuff   );	/*Ʊ������*/
	zip_space(cTmpBuff);
	memcpy(drftTyp , cTmpBuff , sizeof(drftTyp)-1);
	
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("1032",  cTmpBuff	);	/*Ʊ�ݺ�*/
	zip_space(cTmpBuff);
	memcpy(drftNb , cTmpBuff , sizeof(drftNb)-1);
	
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	get_zd_data("0820",  cTmpBuff  );	/*��֤���˺�*/
	zip_space(cTmpBuff);
	memcpy(bailAcct , cTmpBuff , sizeof(bailAcct)-1);  
	  
	/*ret=sysinit();
	if (ret<0)
	{
		vtcp_log("ϵͳ��ʼ������[%s][%d]\n",__FILE__,__LINE__);
		vtcp_log("rc=[%d]",ret);
		goto ErrExit;
	}*/
	vtcp_log("[%s][%d]����=[%d]",__FILE__,__LINE__,Dt);
	vtcp_log("[%s][%d]������ˮ��=[%s]",__FILE__,__LINE__,tranSeqNb);
	vtcp_log("[%s][%d]Ʊ�ݷ���=[%s]",__FILE__,__LINE__,drftCls);
	vtcp_log("[%s][%d]Ʊ������=[%s]",__FILE__,__LINE__,drftTyp);
	vtcp_log("[%s][%d]Ʊ�ݺ�=[%s]",__FILE__,__LINE__,drftNb);
	vtcp_log("[%s][%d]��Ʊ��=[%d]",__FILE__,__LINE__,isseDt);
	vtcp_log("[%s][%d]������=[%d]",__FILE__,__LINE__,dueDt);
	vtcp_log("[%s][%d]Ʊ����=[%lf]",__FILE__,__LINE__,drftAmt);
	vtcp_log("[%s][%d]��֤���˺�=[%s]",__FILE__,__LINE__,bailAcct);
	vtcp_log("[%s][%d]��֤��=[%lf]",__FILE__,__LINE__,bailAmt);

	vtcp_log("%s.%d   -------drftNb=%s  ...drftTyp=%s  ", __FILE__,__LINE__,drftNb,drftTyp );
	
	ret = Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsrecvreg,"ecds_tx_date = %ld and ecds_trace_no ='%s'",Dt , tranSeqNb);
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
			pub_ret=sql_execute("update ecds_recv_reg  set bf_flag='2' where ecds_tx_date = %ld and ecds_trace_no ='%s' and bf_flag='1' ",Dt , tranSeqNb);
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
	ret = Mo_bank_po_Sel( g_pub_tx.reply,&sMo_bank_po,"po_no = '%s' ",drftNb);  /*�и��������û��ȷ��*/
	if (ret)
	{
		sprintf(acErrMsg, "��ѯmo_bank_po��!!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B012");
		goto ErrExit;
	}
	if( sMo_bank_po.po_sts[0] == '3' )
       {
               sprintf( acErrMsg, "�˳жһ�Ʊ������Ʊ!" );
               WRITEMSG
               strcpy( g_pub_tx.reply, "B212" );
               goto ErrExit;
       }else if( sMo_bank_po.po_sts[0] == '2' )
       {
               sprintf( acErrMsg, "�˳жһ�Ʊ�Ѻ�������������Ʊ!" );
               WRITEMSG
               strcpy( g_pub_tx.reply, "B228" );
               goto ErrExit;
       }else if( sMo_bank_po.po_sts[0] == '4' )
       {
               sprintf( acErrMsg, "�˳жһ�Ʊ�����գ���������Ʊ!" );
               WRITEMSG
               strcpy( g_pub_tx.reply, "B229" );
               goto ErrExit;
       }
       if(sMo_bank_po.po_ind[0]=='0')
       {
               sprintf( acErrMsg, "�˳жһ�Ʊ�ѹ�ʧ����������Ʊ!" );
               WRITEMSG
               strcpy( g_pub_tx.reply, "B031" );
               goto ErrExit;
       }
	sprintf(wherelist, " pact_no='%s' " ,sMo_bank_po.pact_no );  

	vtcp_log("WHERE [%s]", wherelist);

	ret = Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply, wherelist);
	ERR_DEAL
	ttlnum = 0;

	while (1) 
	{
		ret = Mo_bank_acc_po_Fet_Upd(&sMo_bank_acc_po, g_pub_tx.reply);
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
		/*****add by ligl 20100508 ��֤���˺Ŵ�Э�������****/
		memcpy(bailAcct,sMo_bank_acc_po.bail_ac_no,sizeof(sMo_bank_acc_po.bail_ac_no)-1);
		pub_base_strpack(bailAcct);
		if(sMo_bank_acc_po.acc_type[0]=='3')
		{
			bailAmt=sMo_bank_po.po_amt;
		}
		else if(sMo_bank_acc_po.acc_type[0]=='1')
		{
			bailAmt=sMo_bank_po.po_amt*sMo_bank_acc_po.bail_amt_ratio/100;
		}
		else if(sMo_bank_acc_po.acc_type[0]=='2')
		{
			bailAmt=0.00;	
		}
		/*****/
		sMo_bank_acc_po.bail_amt=sMo_bank_acc_po.bail_amt-bailAmt;
		strcpy(sMo_bank_acc_po.acc_po_ind, "3");
		ret = Mo_bank_acc_po_Upd_Upd(sMo_bank_acc_po, g_pub_tx.reply);
		if (ret) 
		{
			sprintf(acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		
	}
	ret= Mo_bank_acc_po_Clo_Upd();
	ERR_DEAL

	if (!ttlnum) 
	{
		strcpy(g_pub_tx.reply, "S049");
		goto ErrExit;
	} 
	vtcp_log("%s.%d---- ", __FILE__,__LINE__);
	
	if(ins_flag == 0) 
	{
		sEcds_recv_reg.ecds_tx_date		=Dt;
		memcpy(sEcds_recv_reg.ecds_trace_no,	tranSeqNb,sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
		sEcds_recv_reg.trace_no 		= 0;
		sEcds_recv_reg.tx_date  		= g_pub_tx.tx_date;	
		memcpy(sEcds_recv_reg.drftcls,		drftCls,sizeof(sEcds_recv_reg.drfttyp)-1);	 
		memcpy(sEcds_recv_reg.drfttyp ,		drftTyp,sizeof(sEcds_recv_reg.drftnb)-1);	 
		memcpy(sEcds_recv_reg.drftnb ,		drftNb,	sizeof(sEcds_recv_reg.drftnb)-1);   
		sEcds_recv_reg.issedt 			= Dt ;	 					
		sEcds_recv_reg.duedt	 		= dueDt ;  
		memcpy(sEcds_recv_reg.tag_name,		"CBS1000160",sizeof( sEcds_recv_reg.tag_name)-1);	  /*��������*/
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
	ret = sql_execute("update mo_bank_po set po_sts='3' where po_no = '%s' ",drftNb ); /*�ó���Ʊ״̬*/
	if (ret)
	{
		sprintf(acErrMsg, "update mo_bank_po����!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B017");
		goto ErrExit;
	}
	
	ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",\
	PARM_ECDS_TR_PAY_AC,sMo_bank_acc_po.payer_br_no);
	if (ret)
	{
		sprintf(acErrMsg,"��ѯCom_parm�����!!ret  [%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "AT06" );
		goto ErrExit;
	}
 
	/*��Ϣ����*/
	set_zd_data("1021",bailAcct);
	ret = pubf_intst(&intst_amt);
	if (ret)
	{
		sprintf(acErrMsg, "��Ϣʧ��!!!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "G010");
		goto ErrExit;
	} 
	vtcp_log("[%s][%d]bailAcct=%s",__FILE__,__LINE__,bailAcct);
	/*************************************��Ϣ���˲���******************************/
	memcpy(g_pub_tx.ac_get_ind,"000",3);
	g_pub_tx.tx_amt1=intst_amt;
	strcpy(g_pub_tx.ac_wrk_ind, "000000001");
	memcpy(g_pub_tx.ac_no, "52207",sizeof(g_pub_tx.ac_no));/* �˻� */
	strcpy(g_pub_tx.intst_type, "1");/* ��Ϣ���� */
	strcpy(g_pub_tx.add_ind, "0");  /* ����:0��1��2���� */
	strcpy(g_pub_tx.prt_ind, "0");  /* �Ƿ��ӡ����(��������:1��0�� */
	strcpy(g_pub_tx.hst_ind, "1");  /* ����ϸ:1�ռ���2���յ� ���ջ����� */
	strcpy(g_pub_tx.brf, "��֤����Ϣ");
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
	set_zd_data("0210", "01");
	set_zd_double("0440",intst_amt);
	strcpy(g_pub_tx.sub_tx_code, "B564");
	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (ret != 0)
	{
		vtcp_log("��Ƽ��ʲ��ɹ�[%s][%ld]",__FILE__,__LINE__);
		sprintf(acErrMsg, "��Ƽ��ʲ��ɹ�!!");
		WRITEMSG
		goto ErrExit;
	}
	
	/***********************************ΪD099׼����*******��֤��+��Ϣ******************/
	set_zd_data("1011", sMo_bank_acc_po.payer_ac_no);
	/*set_zd_int("1012", &g_pub_tx.ac_seqn);*/
	set_zd_data("101A", "01");
	set_zd_data("101B", "2");
	set_zd_data("1016", "��֤��");      /***ժҪ***/
	set_zd_data("0630","��֤��");
	set_zd_double("1013", bailAmt+intst_amt);
	set_zd_data("1017", "0");
	set_zd_data("0192", "000000001");
	set_zd_int("0193", 1001);
	/*********************************ΪD003׼������*********��֤��****************/
	set_zd_data("1021",bailAcct);
	set_zd_double("102F",bailAmt);
	set_zd_data("102J","01");
	set_zd_data("102K","2");
	set_zd_data("102Y","��֤��");
	set_zd_data("102O","0");			/*�Ƿ���۱�־*/
	set_zd_data("0810","��֤��");
	set_zd_data("0192","000000001");
	set_zd_int("0193",1001);

GoodExit:
	strcpy(g_pub_tx.reply, "0001");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
GoodExit1:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		memcpy(g_pub_tx.reply,"B564",4);
	}
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

int pubf_intst( double *HQdealval,char *cAcc_hrt)
{
	long		HQday=0;
	int		ret =0;
	double		HQ_rate=0.00;
	struct		mo_bail_rate_c		sMoBailRate1,sMoBailRate;
	memset(&sMoBailRate1,0,sizeof(struct mo_bail_rate_c));
	memset(&sMoBailRate,0,sizeof(struct mo_bail_rate_c));
	memset(cAcc_hrt,0,sizeof(cAcc_hrt));
	
	ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply, " bail_type='1' and pact_no='%s' and sts='1' ",sMo_bank_acc_po.pact_no);
	if (ret) {
		sprintf(acErrMsg, "ִ��Mo_bail_rate_Dec_Upd��![%d]sMo_bank_acc_po.pact_no[%s]", ret, sMo_bank_acc_po.pact_no);
		WRITEMSG
		sprintf(g_pub_tx.reply, "B334");
		return ret;
	}
	ret = Mo_bail_rate_Fet_Upd(&sMoBailRate, g_pub_tx.reply);
	if (ret && ret!=100) {
		sprintf(acErrMsg, "ִ��Mo_bail_rate_Fet_Upd��![%d]sMo_bank_acc_po.pact_no[%s]", ret, sMo_bank_acc_po.pact_no);
		WRITEMSG
		sprintf(g_pub_tx.reply, "B334");
		return ret;
	}
	else if(ret==100)/**û�н�������ά��***/
	{
		*HQdealval = 0.00;
		Mo_bail_rate_Clo_Upd();
		return 0;
	}
	else
	{
	memcpy(&sMoBailRate1, &sMoBailRate, sizeof(struct mo_bail_rate_c));

	pub_base_strpack(sMoBailRate.ac_no);
	
	ret = pub_base_getllz(HQLLBH, "01", g_pub_tx.tx_date, &HQ_rate);
	if (ret) {
		sprintf(acErrMsg, "ȡ���ʴ���!");
		WRITEMSG
		sprintf(g_pub_tx.reply, "O195");
		return ret;
	}
	
	/** �������� **/
	if (pub_base_CalDays(sMoBailRate.beg_date, g_pub_tx.tx_date, "0",&HQday)) {
		sprintf(acErrMsg, "������������!");
		WRITEMSG
		sprintf(g_pub_tx.reply, "O108");
		return ret;
	}
	vtcp_log("[%s][%d] HQday =[%ld]",__FILE__,__LINE__,HQday);
	 
	sprintf(acErrMsg, "HQday=[%ld]HQ_rate=[%lf],ic_rate=[%lf]", HQday, HQ_rate,sMoBailRate.ic_rate);
	WRITEMSG
	/** ������Ϣ **/
	
	*HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
	vtcp_log("[%s][%d] HQdealval=[%f]",__FILE__,__LINE__,*HQdealval);
	*HQdealval = pub_base_PubDround_1(*HQdealval);
	if (pub_base_CompDblVal(HQdealval,0.00) < 0)
	{
		sprintf(acErrMsg, "������Ϣ�������!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D158");
		return ret;
	}

	/** ���±�֤���Ϣ��� **/
	sMoBailRate.intst_date = g_pub_tx.tx_date;
	sMoBailRate.end_trace_no = g_pub_tx.trace_no;
	sMoBailRate.intst_amt = *HQdealval;
	
	/**���±�֤��״̬**/
	strcpy(sMoBailRate.sts, "*");
	ret = Mo_bail_rate_Upd_Upd(sMoBailRate, g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg, "ִ��Mo_bail_rate_Upd_Upd��![%d]Mo_bank_acc_po.pact_no[%s]", ret, sMo_bank_acc_po.pact_no);
		WRITEMSG
		sprintf(g_pub_tx.reply, "B334");
		return ret;
	}
	Mo_bail_rate_Clo_Upd();
	}	
	vtcp_log("[%s][%d] �ñ�֤���ܹ�������[%f]��Ϣ",__FILE__,__LINE__,*HQdealval);
	
	/**ȫ����Ϣת��ԭ��Ϣ�ʻ�**/
	
	/**----------------��鱣֤����Ϣ��Ҫ���ĸ���Ŀȡֵ---------------------------**/
	vtcp_log("[%s][%d] bail_ac_no =[%s]",__FILE__,__LINE__,sMoBailRate.bail_ac_no);
	return 0;
}


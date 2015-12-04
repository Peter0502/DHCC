/*************************************************
* �� �� ��:  spF090.c
* ���������� ATM�Զ����  
*
* ��    ��:  lifei
* ������ڣ� 
*
* �޸ļ�¼�� 
* ��   ��:   2009-7-30 9:08:53
* �� �� ��:  
* �޸�����:

insert into tx_def values ('8620','ATM�Զ����','10000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','3','2','3',null);

insert into tx_flow_def values('8620',0,'F090','#=0001@1@#=0002@3@#$');
insert into tx_flow_def values('8620',1,'M001','#=0000@2@#$');
insert into tx_flow_def values('8620',2,'A017','#$');
insert into tx_flow_def values('8620',3,'A016','#=0000@4@#$');
insert into tx_flow_def values('8620',4,'M002','#$');

insert into tx_sub_def values('F090','ATM�Զ����','spF090','0','0');



insert into tx_sub_rel values('8620','A016','1','190104','1201','�˺�');
insert into tx_sub_rel values('8620','A016','1','01','1204','����');
insert into tx_sub_rel values('8620','A016','1','1','1205','��ת��ʶ');
insert into tx_sub_rel values('8620','A016','0','0400','1208','���');

insert into tx_sub_rel values('8620','A017','1','290201','1211','�˺�');
insert into tx_sub_rel values('8620','A017','1','01','1214','����');
insert into tx_sub_rel values('8620','A017','1','1','1215','��ת��ʶ');
insert into tx_sub_rel values('8620','A017','0','0400','1218','���׽��');

insert into tx_sub_rel values('8620','M001','1','01','1181','����');
insert into tx_sub_rel values('8620','M001','0','0400','1184','���׽��');
insert into tx_sub_rel values('8620','M001','1','ATM����','1187','ժҪ');

insert into tx_sub_rel values('8620','M002','1','01','1191','����');
insert into tx_sub_rel values('8620','M002','0','0400','1194','���׽��');
insert into tx_sub_rel values('8620','M002','1','ATM�̿�','1197','ժҪ');


*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"
#include "mo_rmny_smny_c.h"
#include "seqctl_c.h"
#include "com_parm_c.h"

spF090()
{
	struct com_tel_c com_tel;
	struct cash_mst_c cash_mst;
	struct cash_mst_c Vcash_mst;
	struct cash_mst_c Scash_mst;
	struct cash_mst_hst_c VCashHst;
	struct cash_mst_hst_c SCashHst;
	struct mo_rmny_smny_c Mormnysmny;
	struct seqctl_c       Seqctl;
	char cTel[TEL_LENGTH+1];
	
	double txamt=0.00;
	double txamt1=0.00;
  char cTmp[7];
	char type[2];
	char type1[2];
	int ret=0;
	int ret1=0;
	int ret2=0;
	
	memset(cTel,0x00,sizeof(cTel));
	memset(cTmp,0x00,sizeof(cTmp));
	memset(&com_tel,0x00,sizeof(struct com_tel_c));
	memset(&cash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&Vcash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&Scash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&VCashHst,0x00,sizeof(struct cash_mst_hst_c));
	memset(&SCashHst,0x00,sizeof(struct cash_mst_hst_c));
	memset(&Mormnysmny,0x00,sizeof(struct mo_rmny_smny_c));
	memset(&Seqctl,0x00,sizeof(struct seqctl_c));

	memset(type,0x00,sizeof(type));
	memset(type1,0x00,sizeof(type1)); 
	
	pub_base_sysinit();
	memset(cTmp,0x00,sizeof(cTmp));
	get_zd_data("0580",cTmp);
	memcpy(com_tel.tel,cTmp,4);
	vtcp_log("%s %d cTmp=[%s],com_tel.tel=[%s]",__FILE__,__LINE__,cTmp,com_tel.tel);
	/*memcpy(com_tel.tel,com_tel.name,4);*/
	get_zd_double("0390",&txamt1);
	get_zd_data("0920",cTel);
	vtcp_log("%s %d cTel=[%s],amt=[%f]",__FILE__,__LINE__,cTel,txamt1);

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	
	ret = Com_tel_Sel( g_pub_tx.reply,&com_tel," tel='%s' ",com_tel.tel );
	if( ret )
	{
		sprintf( acErrMsg,"��Ա������[%s]",com_tel.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O120" );
		goto ErrExit;
	}
	/*****add by chengbo ��ͬһ�����Ĺ�Ա������˲���  20090515 ����*****
	if(strcmp(g_pub_tx.tx_br_no,com_tel.br_no)!=0){
		
		sprintf(acErrMsg,"�Ǳ�����ATM��Ա");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L255");
		goto ErrExit;
	}**/
  /*****end 20090515**************/
	if( com_tel.csts[0]=='4' )
	{
		sprintf( acErrMsg,"��Ա״̬��[%s]O[%s]",com_tel.tel,com_tel.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O062" );
		goto ErrExit;
	}
	if( com_tel.csts[0]=='3' )
	{
		sprintf( acErrMsg,"��Ա״̬��[%s]i[%s]",com_tel.tel,com_tel.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O063" );
		goto ErrExit;
	}
	ret = Cash_mst_Sel(g_pub_tx.reply,&cash_mst,"tel='%s'",com_tel.tel);
	if( ret )
	{
		sprintf( acErrMsg,"��Ա������[%s]",com_tel.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"M008" );
		goto ErrExit;
	}
	
	vtcp_log("%s %d qtxamt1=%f,kamt=%f",__FILE__,__LINE__,txamt1,cash_mst.bal);
	
	if (pub_base_CompDblVal(txamt1,cash_mst.bal)==0)
	{
		type[0]='0';	
	}
	else if (pub_base_CompDblVal(txamt1,cash_mst.bal)<0)
	{
		txamt = cash_mst.bal - txamt1;
		type[0]='1';
		type1[0]='0';		
	}
	else
	{
		txamt = txamt1 - cash_mst.bal;
		type[0]='1';
		type1[0]='1';			
	}
	
	ret = pub_base_GetParm_Str(com_tel.br_no,9800,cTmp);
	vtcp_log("%s %d tel=%s",__FILE__,__LINE__,cTmp);
	strcpy(g_pub_tx.tel,cTel);
	vtcp_log("%s %d tel=%s",__FILE__,__LINE__,g_pub_tx.tel);
	
	/***�޸Ĺ�Ա�ֽ�̨��***/
	ret = Cash_mst_Dec_Upd(g_pub_tx.reply,"tel='%s'",g_pub_tx.tel);
	if( ret )
	{
		sprintf( acErrMsg,"��Ա������[%s]",g_pub_tx.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );
		goto ErrExit;
	}
	ret = Cash_mst_Fet_Upd(&Vcash_mst,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˹�Ա��Ǯ��[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M008");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}
	
	Vcash_mst.bal=Vcash_mst.bal+cash_mst.bal;	
	ret = Cash_mst_Upd_Upd(Vcash_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf( acErrMsg,"�޸��ֽ�̨�ʴ���[%s]",g_pub_tx.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"P153" );
		goto ErrExit;
	} 
	Cash_mst_Clo_Upd();
	
	/***�޸�AIM�ֽ�̨��***/
	ret = Cash_mst_Dec_Upd(g_pub_tx.reply,"tel='%s'",com_tel.tel);
	if( ret )
	{
		sprintf( acErrMsg,"��Ա������[%s]",com_tel.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );
		goto ErrExit;
	}
	ret = Cash_mst_Fet_Upd(&Scash_mst,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˹�Ա��Ǯ��[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M008");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}
	Scash_mst.bal=Scash_mst.bal-cash_mst.bal;	
	ret = Cash_mst_Upd_Upd(Scash_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf( acErrMsg,"�޸��ֽ�̨�ʴ���[%s]",g_pub_tx.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"P153" );
		goto ErrExit;
	} 
	Cash_mst_Clo_Upd();
	
	/**�����ֽ�̨����ϸ***/
	strcpy( g_pub_tx.brf,"�����ֽ�");
	strcpy( g_pub_tx.ct_ind,"1" );
	strcpy( g_pub_tx.ac_no,Vcash_mst.ac_no  );
	
	strcpy( g_pub_tx.cur_no,"01" );
	g_pub_tx.tx_amt1=cash_mst.bal;
     
     
  VCashHst.trace_no = g_pub_tx.trace_no;	/*������ˮ��*/
  VCashHst.trace_cnt = g_pub_tx.trace_cnt;	/*������ˮ���*/
	strcpy( VCashHst.br_no , g_pub_tx.tx_br_no );	/*����*/
	strcpy( VCashHst.ac_no , Vcash_mst.ac_no );	/*�˺�*/
	strcpy( VCashHst.tel, g_pub_tx.tel );		/*��Ա*/
	strcpy( VCashHst.cur_no , g_pub_tx.cur_no );	/*����*/
	sprintf( VCashHst.add_ind , g_pub_tx.ct_ind ); 	/*�ո���־*/
	VCashHst.amt =	cash_mst.bal;		        /*������*/
	VCashHst.tx_date = g_pub_tx.tx_date;		/*��ˮ����*/
	VCashHst.tx_time = g_pub_tx.tx_time;		/*����ʱ��*/
	strcpy( VCashHst.brf ,	g_pub_tx.brf );		/*ժҪ*/
	strcpy( VCashHst.cash_code , g_pub_tx.cash_code );	/*�ֽ����*/
	/*get_zd_long("1261",&VCashHst.tt_no);add by cgx 20090818 nxhhyh*/
  ret=Cash_mst_hst_Ins(VCashHst,g_pub_tx.reply);         
  if( ret )
	{
		sprintf(acErrMsg,"�Ǽ�Ǯ����ϸ����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P155");
		goto ErrExit;
	}
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	strcpy( g_pub_tx.brf,"�����ֽ�");
  strcpy( g_pub_tx.ct_ind,"1" );
  strcpy( g_pub_tx.ac_no,Scash_mst.ac_no  );
  
  SCashHst.trace_no = g_pub_tx.trace_no;	/*������ˮ��*/
  SCashHst.trace_cnt = g_pub_tx.trace_cnt;	/*������ˮ���*/
	strcpy( SCashHst.br_no , Scash_mst.br_no );	/*����*/
	strcpy( SCashHst.ac_no , Scash_mst.ac_no );	/*�˺�*/
	strcpy( SCashHst.tel, Scash_mst.tel );		/*��Ա*/
	strcpy( SCashHst.cur_no , g_pub_tx.cur_no );	/*����*/
	sprintf( SCashHst.add_ind , g_pub_tx.ct_ind ); 	/*�ո���־*/
	SCashHst.amt =	cash_mst.bal;		        /*������*/
	SCashHst.tx_date = g_pub_tx.tx_date;		/*��ˮ����*/
	SCashHst.tx_time = g_pub_tx.tx_time;		/*����ʱ��*/
	strcpy( SCashHst.brf ,	g_pub_tx.brf );		/*ժҪ*/
	strcpy( SCashHst.cash_code , g_pub_tx.cash_code );	/*�ֽ����*/         
	/*get_zd_long("1261",&SCashHst.tt_no);add by cgx 20090818 nxhhyh*/
  ret=Cash_mst_hst_Ins(SCashHst,g_pub_tx.reply);         
  if( ret )
	{
		sprintf(acErrMsg,"�Ǽ�Ǯ����ϸ����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P155");
		goto ErrExit;
	}
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data(DC_CODE,g_pub_tx.dc_code);
	if(type[0]=='0') 
	{
	goto OkExit;	
	}
	else
	{
		if(type1[0]=='0')  /***����****/
		{
			/*** ���볤�̿�Ǽǲ� ***/
			set_zd_double("0400",txamt);
			strcpy( Mormnysmny.br_no, com_tel.br_no );
			Mormnysmny.occ_date=g_pub_tx.tx_date;
			strcpy( Mormnysmny.tx_opmd, "0" );
			strcpy( Mormnysmny.cur_no, "01" );
			Mormnysmny.amt=txamt;
			Mormnysmny.tx_date=g_pub_tx.tx_date;
			Mormnysmny.trace_no=g_pub_tx.trace_no;
			strcpy( Mormnysmny.occ_tel, cash_mst.tel);
			strcpy( Mormnysmny.brf, "����Ǽ�");
			strcpy( Mormnysmny.tel, g_pub_tx.tel);
			strcpy( Mormnysmny.occ_br_no,g_pub_tx.tx_br_no);
			
			ret = Seqctl_Dec_Upd(g_pub_tx.reply,"ac_ind='%s' and br_no='00000'",CDK_IND);
			if(ret)
			{
				vtcp_log("%s,%d�����α����!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			ret = Seqctl_Fet_Upd(&Seqctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d�ü�¼������!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			Mormnysmny.seqn=Seqctl.seqno;
			Seqctl.seqno++;
			ret = Seqctl_Upd_Upd(Seqctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d������ű����!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D105");
				goto ErrExit;
			}
			
			Seqctl_Clo_Upd();
			/*get_zd_long("1261",&Mormnysmny.tt_no);add by cgx 20090818 nxhhyh*/
			ret=Mo_rmny_smny_Ins(Mormnysmny,g_pub_tx.reply);         
			if( ret )
			{
				sprintf(acErrMsg,"�Ǽǳ��̿�Ǽǲ�����![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"CD29");
				goto ErrExit;
			}
			
			set_zd_data("121A","ATM�������");
			set_zd_int("0280",Mormnysmny.seqn);
			goto OkExit1;
	        }
		else if (type1[0]=='1') /**�̿�***/
		{
			/*** ���볤�̿�Ǽǲ� ***/
			set_zd_double("0400",txamt);
			strcpy( Mormnysmny.br_no, com_tel.br_no );
			Mormnysmny.occ_date=g_pub_tx.tx_date;
			strcpy( Mormnysmny.tx_opmd, "3" );
			strcpy( Mormnysmny.cur_no, "01" );
			Mormnysmny.amt=txamt;
			Mormnysmny.tx_date=g_pub_tx.tx_date;
			Mormnysmny.trace_no=g_pub_tx.trace_no;
			strcpy( Mormnysmny.occ_tel, cash_mst.tel);
			strcpy( Mormnysmny.brf, "�̿�Ǽ�");
			strcpy( Mormnysmny.tel, g_pub_tx.tel);
			strcpy( Mormnysmny.occ_br_no,g_pub_tx.tx_br_no);
			ret = Seqctl_Dec_Upd(g_pub_tx.reply,"ac_ind='%s' and br_no='00000'",CDK_IND);
			if(ret)
			{
				vtcp_log("%s,%d�����α����!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			ret = Seqctl_Fet_Upd(&Seqctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d�ü�¼������!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			Mormnysmny.seqn=Seqctl.seqno;
			Seqctl.seqno++;
			ret = Seqctl_Upd_Upd(Seqctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d������ű����!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D105");
				goto ErrExit;
			}
			
			Seqctl_Clo_Upd();
			/*get_zd_long("1261",&Mormnysmny.tt_no);add by cgx 20090818 nxhhyh*/
			ret=Mo_rmny_smny_Ins(Mormnysmny,g_pub_tx.reply);         
	                if( ret )
		        {
		          	sprintf(acErrMsg,"�Ǽ�Ǯ����ϸ����![%d]",ret);
		           	WRITEMSG
		           	strcpy(g_pub_tx.reply,"P155");
		           	goto ErrExit;
		        }
			set_zd_data("121A","ATM����̿�");
			set_zd_int("0280",Mormnysmny.seqn);
			goto OkExit2;
		}	
   }
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return(����): reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return(�̿�): reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}

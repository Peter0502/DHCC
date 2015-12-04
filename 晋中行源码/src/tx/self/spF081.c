/*************************************************
* �� �� ��:  spF081.c
* ���������� ��Ա���̿�� 
*
* ��    ��:  robbit
* ������ڣ� 
*
* �޸ļ�¼�� 
* ��   ��:   20080416
* �� �� ��:  
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"
#include "mo_rmny_smny_c.h"
#include "seqctl_c.h"

#define CKREG_KM "26204"  /*����������Ŀ*/ 
#define CKRZ_KM  "51503"  /*��������*/
#define DKREG_KM "13904"  /*�̿�������Ŀ*/
#define DKRZ_KM  "53602"  /*�̿�֧��*/
spF081()
{
	struct com_tel_c com_tel;
	struct cash_mst_c cash_mst;
	struct cash_mst_c Vcash_mst;
	struct cash_mst_c Scash_mst;
	struct cash_mst_hst_c VCashHst;
	struct cash_mst_hst_c SCashHst;
	struct mo_rmny_smny_c Mormnysmny;
	struct seqctl_c       Seqctl;
 
	double txamt=0.00;
	double txamt1=0.00;
	char type[2];
    
	int ret=0;
	int ret1=0;
	int ret2=0;
	long txdate=0;
	long traceno=0;
	
	memset(&com_tel,0x00,sizeof(struct com_tel_c));
	memset(&cash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&Vcash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&Scash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&VCashHst,0x00,sizeof(struct cash_mst_hst_c));
	memset(&SCashHst,0x00,sizeof(struct cash_mst_hst_c));
	memset(&Mormnysmny,0x00,sizeof(struct mo_rmny_smny_c));
	memset(&Seqctl,0x00,sizeof(struct seqctl_c));
	
	memset(type,0x00,sizeof(type));
	   
	pub_base_sysinit();

	get_zd_data("0680",type);
	get_zd_double("0390",&txamt);
	get_zd_long("0440",&txdate);
	get_zd_long("0780",&traceno); /***���***/
	vtcp_log("%s,%d,��������++++++++[%s]",__FILE__,__LINE__,type);
     
	/**------- ��ʼ��ȫ�ֱ��� --------**/
	

	if(type[0]=='0')  /***����Ǽ�****/
	{
		/*** ���볤�̿�Ǽǲ� ***/
		set_zd_double("0400",txamt);
		set_zd_data("0640",CKREG_KM);
		set_zd_data("121A","����Ǽ�");
		set_zd_data("0670","1");
		set_zd_data("1187","����Ǽ�");
		strcpy( Mormnysmny.br_no, g_pub_tx.tx_br_no);
		Mormnysmny.occ_date=g_pub_tx.tx_date;
		strcpy( Mormnysmny.tx_opmd, "0" );
		strcpy( Mormnysmny.cur_no, "01" );
		Mormnysmny.amt=txamt;
		Mormnysmny.tx_date=g_pub_tx.tx_date;
		Mormnysmny.trace_no=g_pub_tx.trace_no;
		strcpy( Mormnysmny.occ_tel, g_pub_tx.tel);
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
		ret=Mo_rmny_smny_Ins(Mormnysmny,g_pub_tx.reply);         
		if( ret )
		{
			sprintf(acErrMsg,"���볤�̿�Ǽǲ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD29");
			goto ErrExit;
		}
		set_zd_int("0280",Mormnysmny.seqn);
		goto OkExit1;
	}
	else if (type[0]=='2') /**�̿�Ǽ�***/
	{
		/*** ���볤�̿�Ǽǲ� ***/
		set_zd_double("0400",txamt);
		set_zd_data("0650",DKREG_KM);
		set_zd_data("121A","�̿�Ǽ�");
		set_zd_data("0670","1");
		set_zd_data("1197","�̿�Ǽ�");
		strcpy( Mormnysmny.br_no, g_pub_tx.tx_br_no );
		Mormnysmny.occ_date=g_pub_tx.tx_date;
		strcpy( Mormnysmny.tx_opmd, "3" );
		strcpy( Mormnysmny.cur_no, "01" );
		Mormnysmny.amt=txamt;
		Mormnysmny.tx_date=g_pub_tx.tx_date;
		Mormnysmny.trace_no=g_pub_tx.trace_no;
		strcpy( Mormnysmny.occ_tel, g_pub_tx.tel);
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
		ret=Mo_rmny_smny_Ins(Mormnysmny,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�Ǽǳ��̿�Ǽǲ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD29");
			goto ErrExit;
		}
	
		set_zd_int("0280",Mormnysmny.seqn);
		goto OkExit2;
	}	
	else if(type[0]=='1')/**��������***/
	{
	  /**�Ҿ���Ӧ�ü��Ϲ�Ա���ж� BY ������ 2008-9-11
		ret=Mo_rmny_smny_Dec_Upd(g_pub_tx.reply,"tx_date=%d and seqn=%d ",txdate,traceno);
		***/
		ret=Mo_rmny_smny_Dec_Upd(g_pub_tx.reply,"tx_date=%d and seqn=%d and occ_tel='%s'",txdate,traceno,g_pub_tx.tel);
		if(ret)
		{
			sprintf(acErrMsg,"�����ڴ˱ʳ����¼[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		ret=Mo_rmny_smny_Fet_Upd(&Mormnysmny,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�����ڴ˱ʳ����¼[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD30");
			goto ErrExit;
		}
		if(Mormnysmny.tx_opmd[0]!='0')
		{
			sprintf(acErrMsg,"�ǳ���Ǽ�״̬��������");
			set_zd_data("DC_GET_MSG",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD31");
			goto ErrExit;
		}
			
		Mormnysmny.tx_date=g_pub_tx.tx_date;
		Mormnysmny.occ_date=g_pub_tx.tx_date;
		strcpy(Mormnysmny.tx_opmd,"1");
		strcpy(Mormnysmny.tel,g_pub_tx.tel);
		strcpy(Mormnysmny.brf,"��������");
		ret=Mo_rmny_smny_Upd_Upd(Mormnysmny,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�޸ĳ��̿�Ǽǲ�����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D32");
			goto ErrExit;
		}
		Mo_rmny_smny_Clo_Upd();
	
		set_zd_double("0400",Mormnysmny.amt);
		set_zd_data("0670","2");
		set_zd_data("0650",CKREG_KM);/***��26204***/
		set_zd_data("0640",CKRZ_KM);/***��51503***/
		set_zd_data("121A","��������");
	
		goto OkExit3;
	
	}
	else if (type[0]=='3') /**�̿�����***/
	{
	  /**�Ҿ���Ӧ�ü��Ϲ�Ա���ж� BY ������ 2008-9-11
		ret=Mo_rmny_smny_Dec_Upd(g_pub_tx.reply,"tx_date=%d and seqn=%d",txdate,traceno);
		***/
		ret=Mo_rmny_smny_Dec_Upd(g_pub_tx.reply,"tx_date=%d and seqn=%d and occ_tel='%s'",txdate,traceno,g_pub_tx.tel);
		if(ret)
		{
			sprintf(acErrMsg,"�����ڴ˱ʼ�¼[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		ret=Mo_rmny_smny_Fet_Upd(&Mormnysmny,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�����ڴ˱ʼ�¼[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD30");
			goto ErrExit;
		}
		if(Mormnysmny.tx_opmd[0]!='3')
		{
			sprintf(acErrMsg,"�Ƕ̿�Ǽ�״̬��������");
			set_zd_data("DC_GET_MSG",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD31");
			goto ErrExit;
		}
		Mormnysmny.tx_date=g_pub_tx.tx_date;
		Mormnysmny.occ_date=g_pub_tx.tx_date;
		strcpy(Mormnysmny.tx_opmd,"4");
		strcpy(Mormnysmny.tel,g_pub_tx.tel);
		strcpy(Mormnysmny.brf,"�̿�����");
		ret=Mo_rmny_smny_Upd_Upd(Mormnysmny,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�޸ĳ��̿�Ǽǲ�����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD32");
			goto ErrExit;
		}
		Mo_rmny_smny_Clo_Upd();
		
		set_zd_double("0400",Mormnysmny.amt);
		set_zd_data("0670","2");
		set_zd_data("0650",DKRZ_KM);/***��53602***/
		set_zd_data("0640",DKREG_KM);/***��13904***/
		set_zd_data("121A","�̿�����");
		goto OkExit3;
		
	}
	else if(type[0]=='4')/**������***/
	{
		/**�Ҿ���Ӧ�ü��Ϲ�Ա���ж� BY ������ 2008-9-11
		ret=Mo_rmny_smny_Dec_Upd(g_pub_tx.reply,"tx_date=%d and seqn=%d",txdate,traceno);
		**/
		ret=Mo_rmny_smny_Dec_Upd(g_pub_tx.reply,"tx_date=%d and seqn=%d and occ_tel='%s'",txdate,traceno,g_pub_tx.tel);
    if(ret)
		{
			sprintf(acErrMsg,"�����ڴ˱ʼ�¼[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		
		ret=Mo_rmny_smny_Fet_Upd(&Mormnysmny,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�����ڴ˱ʼ�¼[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD30");
			goto ErrExit;
		}
		if(Mormnysmny.tx_opmd[0]!='0')
		{
			vtcp_log("++++++++[%s]",Mormnysmny.tx_opmd);
			sprintf(acErrMsg,"�ǳ���Ǽ�״̬���ܳ��");
			set_zd_data("DC_GET_MSG",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD31");
			goto ErrExit;
		}
		Mormnysmny.tx_date=g_pub_tx.tx_date;
		Mormnysmny.occ_date=g_pub_tx.tx_date;
		strcpy(Mormnysmny.tx_opmd,"2");
		strcpy(Mormnysmny.tel,g_pub_tx.tel);
		strcpy(Mormnysmny.brf,"������");
		ret=Mo_rmny_smny_Upd_Upd(Mormnysmny,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�޸ĳ��̿�Ǽǲ�����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD32");
			goto ErrExit;
		}
		Mo_rmny_smny_Clo_Upd();
	
		txamt1=0.00-Mormnysmny.amt;
		set_zd_double("0400",txamt1);
		set_zd_data("0670","1");
		set_zd_data("0640",CKREG_KM);/***��26204***/
		set_zd_data("121A","������");
		set_zd_data("1187","������");
		goto OkExit1;
	
	}
	else if (type[0]=='5') /**�̿���***/
	{
	  /**�Ҿ���Ӧ�ü��Ϲ�Ա���ж� BY ������ 2008-9-11
		ret=Mo_rmny_smny_Dec_Upd(g_pub_tx.reply,"tx_date=%d and seqn=%d",txdate,traceno);
		**/
		ret=Mo_rmny_smny_Dec_Upd(g_pub_tx.reply,"tx_date=%d and seqn=%d and occ_tel='%s'",txdate,traceno,g_pub_tx.tel);
		if(ret)
		{
			sprintf(acErrMsg,"�����ڴ˱ʼ�¼[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		
		ret=Mo_rmny_smny_Fet_Upd(&Mormnysmny,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�����ڴ˱ʼ�¼[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD30");
			goto ErrExit;
		}
		
		if(Mormnysmny.tx_opmd[0]!='3')
		{
			sprintf(acErrMsg,"�Ƕ̿�Ǽ�״̬���ܳ��");
			set_zd_data("DC_GET_MSG",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD31");
			goto ErrExit;
		}
		Mormnysmny.tx_date=g_pub_tx.tx_date;
		Mormnysmny.occ_date=g_pub_tx.tx_date;
		strcpy(Mormnysmny.tx_opmd,"5");
		strcpy(Mormnysmny.tel,g_pub_tx.tel);
		strcpy(Mormnysmny.brf,"�̿���");
		ret=Mo_rmny_smny_Upd_Upd(Mormnysmny,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�޸ĳ��̿�Ǽǲ�����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"CD32");
			goto ErrExit;
		}
		Mo_rmny_smny_Clo_Upd();
	
		txamt1=0.00-Mormnysmny.amt;
		set_zd_double("0400",txamt1);
		set_zd_data("0670","1");
		set_zd_data("0650",DKREG_KM);/***��13904***/
		set_zd_data("121A","�̿���");
		set_zd_data("1197","�̿���");
			
		goto OkExit2;
	
	}else{
		vtcp_log("%s,%d,�����ڸ���ҵ������!type[%s]",__FILE__,__LINE__,type);
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
  
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return(����Ǽǳ��): reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return(�̿�Ǽǳ��): reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

OkExit3:
	strcpy( g_pub_tx.reply, "0003" );
	sprintf(acErrMsg,"Before OK return(���̿�����): reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}

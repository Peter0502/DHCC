/*************************************************
* �� �� ��: spC118.c
* �����������ͻ����յȼ�����
* ��    ��: shangyongchao
* ������ڣ�2013/5/16 9:47:30
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.

insert into tx_def
values('8543','�ͻ����յǼ�����','10000000000000010000001000000000000000000001100100000000000001100011100000000001100000000000000000000000000000000000000000000000','1','4','2','3','');
insert into tx_flow_def values('8543','0','8543','#$');
insert into tx_sub_def values('8543','�ͻ����յǼ�����','spC118','0','0');

*************************************************/
#include "public.h"
#define EXTERN
#include "cif_ass_reg_c.h"
#include "cif_basic_inf_c.h"
static  struct cif_ass_reg_c sCif_ass_reg;
int spC118()
{
	int iRet  = 0;
	int iScore = 0;
	long lCif_no = 0;
	long lEnd_date = 0;
	char cAss_lvl[10];  /*ÿ���ȼ��ĺ��ֽ���*/
	char cFlag[2];   /*ͬ��������־   0��  1��  add by lwb 20141024 */

	struct cif_basic_inf_c sCif_basic_inf;

	char cId_no[21];/*֤������*/

	struct cif_ass_reg_c sCif_ass_reg;
	struct cif_ass_reg_c s_cif_ass_reg;

	pub_base_sysinit();
	memset(cFlag,0x00,sizeof(cFlag));
	memset(&sCif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&sCif_ass_reg,0x00,sizeof(struct cif_ass_reg_c));
	memset(&s_cif_ass_reg,0x00,sizeof(struct cif_ass_reg_c));

	/*ȡֵ*/
	get_zd_long("0480",&s_cif_ass_reg.score);
	get_zd_long("0280",&s_cif_ass_reg.cif_no);
	get_zd_data("0690",cFlag);
	
    /*�������봫���*/
	vtcp_log("%d%s,iScore[%d]lCif_no[%ld]", __LINE__, __FILE__, s_cif_ass_reg.score, s_cif_ass_reg.cif_no);

	iRet = Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no= %ld",s_cif_ass_reg.cif_no);
	if(iRet == 100)
	{
		sprintf(acErrMsg,"�޴˿ͻ��ţ�");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C052");
		goto ErrExit;
	}	  
	else if(iRet)
	{
		sprintf(acErrMsg,"��ѯ�ͻ�������Ϣ�����");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C113");
		goto ErrExit;

	}	

	/*����cOper_flag���в�ͬ����*/
	if(s_cif_ass_reg.score>=0 && s_cif_ass_reg.score<=18)
	{
		strcpy(s_cif_ass_reg.ass_lvl,"1");
		
	}
	else	if(s_cif_ass_reg.score>=19 && s_cif_ass_reg.score<=25)
	{
		strcpy(s_cif_ass_reg.ass_lvl,"2");
		
	}
	else	if(s_cif_ass_reg.score>=26 && s_cif_ass_reg.score<=33)
	{
		strcpy(s_cif_ass_reg.ass_lvl,"3");
		
	}
	else	if(s_cif_ass_reg.score>=34 && s_cif_ass_reg.score<=41)
	{
		strcpy(s_cif_ass_reg.ass_lvl,"4");
		
	}
	else 
	{
		strcpy(s_cif_ass_reg.ass_lvl,"5");
		
	}
	
	
		/*�Ǽǻ��߸��¿ͻ����յǼǱ�*/
		iRet = Cif_ass_reg_Dec_Upd(g_pub_tx.reply,"cif_no =%ld",s_cif_ass_reg.cif_no);
		if(iRet)
		{sprintf(acErrMsg,"���¿ͻ����յǼ������Ǽǲ�����![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C149");
			goto ErrExit;
		}
		iRet = Cif_ass_reg_Fet_Upd(&sCif_ass_reg,g_pub_tx.reply);
		if(iRet  == 100)/*û�о͵Ǽ�*/
		{
			sCif_ass_reg.cif_no = s_cif_ass_reg.cif_no;
			sCif_ass_reg.ass_date  = g_pub_tx.tx_date;
         	   sCif_ass_reg.end_date = g_pub_tx.tx_date+10000;/*1��*/
			strcpy(sCif_ass_reg.ass_br_no,g_pub_tx.tx_br_no);
			strcpy(sCif_ass_reg.ass_tel,g_pub_tx.tel);
			strcpy(sCif_ass_reg.qt_code,"");
			strcpy(sCif_ass_reg.answer,"");
			sCif_ass_reg.score = s_cif_ass_reg.score;
			strcpy(sCif_ass_reg.ass_lvl,s_cif_ass_reg.ass_lvl);
			iRet = Cif_ass_reg_Ins(sCif_ass_reg,g_pub_tx.reply);
			if(iRet)
			{
				Cif_ass_reg_Debug(&sCif_ass_reg);
				sprintf(acErrMsg,"�Ǽǿͻ����յǼ������Ǽǲ�����![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C150");
				goto ErrExit;
			}
		}
		else if(iRet)
		{
			sprintf(acErrMsg,"��ѯ�ͻ����յǼ������Ǽǲ�����![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C150");
			goto ErrExit;
		}
		else/*�о͸���*/
		{
			sCif_ass_reg.ass_date  = g_pub_tx.tx_date;
        	    sCif_ass_reg.end_date = g_pub_tx.tx_date +10000;/*1��*/
			strcpy(sCif_ass_reg.ass_br_no,g_pub_tx.tx_br_no);
			strcpy(sCif_ass_reg.ass_tel,g_pub_tx.tel);
			strcpy(sCif_ass_reg.qt_code,"");
			strcpy(sCif_ass_reg.answer,"");
			
			if(cFlag[0] == '1'){    /*ͬ��������־Ϊ1ʱ  ���·��յȼ�  add by lwb 20141024*/
			strcpy(sCif_ass_reg.ass_lvl,s_cif_ass_reg.ass_lvl);
			sCif_ass_reg.score = s_cif_ass_reg.score;
			}
		
			iRet = Cif_ass_reg_Upd_Upd(sCif_ass_reg,g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg,"���¿ͻ����յǼ������Ǽǲ�����![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C149");
				goto ErrExit;
			}
		}
		Cif_ass_reg_Clo_Upd();
		
		pub_base_dic_show_str(cAss_lvl,"ass_lvl",sCif_ass_reg.ass_lvl);
		set_zd_data("0810",cAss_lvl);
		
		sprintf(g_pub_tx.ac_no,"%d",s_cif_ass_reg.cif_no);
		strcpy(g_pub_tx.brf,"�ͻ����յȼ�����");
		
		if( pub_ins_trace_log() )
		{
			sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
			WRITEMSG
			goto ErrExit;
    }
		
	
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg,"�ͻ����յȼ������ɹ�![%s]",g_pub_tx.reply);
	 WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ͻ����յȼ�����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

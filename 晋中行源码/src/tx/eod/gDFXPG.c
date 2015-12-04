/*************************************************************
* �� �� ��: gDFXPG.c
* ��������: ��ƿͻ����������������ڶ���֪ͨ
*
* ��    ��: liwenbo
* �������: 20141029 
*
* �޸ļ�¼:
* ��    ��:
* �� �� ��:
* ��    ע:
**************************************************************/


#include "public.h"
#define EXTERN
#include "cif_ass_reg_c.h"
#include "mob_acct_type_c.h"

int gDFXPG()
{
	int iRet = 0;
	int iYear = 0;
	int iMouth = 0;
	int iDay = 0;
	char Ac_id_str[500];
	char ac_id_str[20];
	struct cif_ass_reg_c sCif_ass_reg;
	struct dd_mst_c sDd_mst;
	struct mob_acct_type_c sMob_acct_type;
	
	memset(&sCif_ass_reg,0x00,sizeof(sCif_ass_reg));
	memset(&sDd_mst,0x00,sizeof(sDd_mst));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(ac_id_str,0x00,sizeof(ac_id_str));
  memset(Ac_id_str,0x00,sizeof(Ac_id_str));
	
	iRet=sql_begin(); /*������*/
	if( iRet )	{	
	sprintf( acErrMsg, "������ʧ��!!!" );	
	WRITEMSG	
	goto ErrExit;
	}
	pub_base_sysinit();
	
	iYear = (g_pub_tx.tx_date+5)/10000;
	iMouth = (g_pub_tx.tx_date+5)%10000/100;
	iDay = (g_pub_tx.tx_date+5)%10000%100;

	sprintf(g_pub_tx.brf,"����%d��%d��%d�յ���",iYear,iMouth,iDay);
	vtcp_log("%s",g_pub_tx.brf);
	strcpy(g_pub_tx.tx_code ,"FXPG");
	strcpy(g_pub_tx.sub_tx_code ,"FXPG");
	
	iRet = Cif_ass_reg_Dec_Sel(g_pub_tx.reply,"end_date = %ld",g_pub_tx.tx_date+5);
	if(iRet)
	{
		sprintf(acErrMsg,"��ѯ�ͻ����յȼ���ʧ�ܣ�");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C150");
		goto ErrExit;	
	}
	
	while(1)
	{
		iRet = Cif_ass_reg_Fet_Sel(&sCif_ass_reg,g_pub_tx.reply);
		if(iRet == 100)
		{
			break;
		}
		else if(iRet)
		{
			sprintf(acErrMsg,"��ѯ�ͻ����յȼ���ʧ�ܣ�");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C150");
			goto ErrExit;		
		}
		
		iRet = Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no = %d and ac_sts = '1'",sCif_ass_reg.cif_no);
 		if(iRet)
 		{
			vtcp_log("%s,%d ��ѯdd_mst����!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"W015");
			goto ErrExit;	
 		}
 		while(1)
 		{
 			iRet = Dd_mst_Fet_Sel(&sDd_mst,g_pub_tx.reply);	
 			if(iRet == 100)
 			{
 				break;
 			}
 			sprintf(ac_id_str,"%d,",sDd_mst.ac_id);
 			strcat(Ac_id_str,ac_id_str);
 		}
 		Ac_id_str[strlen(Ac_id_str)-1]=' ';     /*�����Ķ���ȥ��*/
 		vtcp_log("Ac_id_str  = %s",Ac_id_str);
 		Dd_mst_Clo_Sel();
 		
 		iRet = Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_id in (%s) and mob_sts <> '3' ",Ac_id_str);
 		if(iRet == 100)
 		{
 		  break;
 		}
 		else if(iRet&&iRet!=100)
 		{	
			vtcp_log("[%s][%d]��ѯ��Mob_acct_type_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
      strcpy(g_pub_tx.reply,"D104");
      goto ErrExit;   
 		}
			
			pub_mob_sendmail(  g_pub_tx.brf, "FXPG",sMob_acct_type.ac_no,"2",0, 0.00 );
			break;
	}
		
			
	
	Cif_ass_reg_Clo_Sel();

OkExit:
        sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ƿͻ������������ڶ���֪ͨ������ϣ�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"%s,%d,��ƿͻ������������ڶ���֪ͨ����ʧ��",__FILE__,__LINE__);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
	
	
		
	
}



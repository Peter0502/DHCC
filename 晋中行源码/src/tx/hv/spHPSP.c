/***************************************************************
* �� �� ��:     spHPSP.c
* ����������    �����˻����л�Ʊ�⸶  
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-27 21:08
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"


spHPSP()
{
	/** ׼���������¼,�Ӳ������ñ��еõ���Ӧ���ʺ� */
	/* add by LiuHuafeng 2006-10-3 16:05 */
		int iRet=0;
		char cKinbr[BRNO_LEN+1];
		char cAc_no[25];
		char cStep[2];
		memset(cStep, 0 , sizeof(cStep));
		memset(cAc_no, 0 , sizeof(cAc_no));
		memset(cKinbr, 0 , sizeof(cKinbr));
		/* �õ���ǰִ�еĲ��� */
		get_zd_data("0990",cStep);
		get_zd_data(DC_TX_BR_NO,cKinbr);
		switch(cStep[0]){
		case '0':
		/* �õ���һ�Է�¼ dr 261   cr �ͻ�  */
			memset(cAc_no,0,sizeof(cAc_no));
			/* 2 �ͻ���*/
			get_zd_data("1011",cAc_no);
			set_zd_data("0660","2");
			set_zd_data("0990","1");
		vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			break;
		case '1':
		/* 1,261�ʺ� */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HPGZ,cAc_no);
			if(iRet)
			{
				sprintf(acErrMsg,"�õ���������Ʊ�����ʺŴ���!\n");
				WRITEMSG
				vtcp_log("%s,%d �õ���������Ʊ�����ʺŴ���\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P005");
				goto ErrExit;
			}
			set_zd_data("0660","1");
			set_zd_data("0990","2");
			set_zd_data("0680","0");
		vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			break;
		case '2':
			/* �õ��ڶ��Է�¼ dr 2440403   cr 2440401  */
			/* 1,2440403֧���˻�ר���ʺ� */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HPTH,cAc_no);
			if(iRet)
			{
				sprintf(acErrMsg,"�õ���������Ʊ�����ʺŴ���!\n");
				WRITEMSG
				vtcp_log("%s,%d �õ���������Ʊ�����ʺŴ���\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P005");
				goto ErrExit;
			}
			set_zd_data("0660","1");
			set_zd_data("0990","3");
			set_zd_data("0680","0");
		vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			break;
		case '3':
			memset(cAc_no,0,sizeof(cAc_no));
			/* 2 2440401֧��ǩ��ר���ʻ� */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
			if(iRet)
			{
				sprintf(acErrMsg,"�õ���������Ʊ�����ʺŴ���!\n");
				WRITEMSG
				vtcp_log("%s,%d �õ���������Ʊ�����ʺŴ���\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P005");
				goto ErrExit;
			}
			set_zd_data("0660","2");
			set_zd_data("0990","4");
			set_zd_data("0680","0");
		vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			break;
		default:
			vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			goto FinishExit1;
			break;
		}
		set_zd_data("0300",cAc_no);
		vtcp_log("%s,%d ac_no====[%s]",__FILE__,__LINE__,cAc_no);
OkExit:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
FinishExit1:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ȫ���ɹ�����![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

/*************************************************
* �� �� ��:  spJ099.c
* ���������� ���ݹ���״̬  ��������/���˿�Ŀ��8583���������
*			 
* ��    ��:  ChenMing
* ������ڣ� 2006��9��12��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"
#include "hv_define.h"

int spJtst()
{ 
	char cLvStat[2];
	char cAc_no [20];
	char cAcno110[20];
	char cKinbr[BRNO_LEN+1];
	int iRet=0;
	char cFinishFlag[2];
	char cCmtno[4];
	char cRespCode[9];
	char cTemp[25];
	
	char cRcpstat[4];
	memset(cRcpstat,0,sizeof(cRcpstat));
	memset(cRespCode,0,sizeof(cRespCode));
	memset(cCmtno, 0 , sizeof(cCmtno));
	memset(cFinishFlag, 0 , sizeof(cFinishFlag));
	memset(cKinbr, 0 , sizeof(cKinbr));
	memset(cAcno110, 0 , sizeof(cAcno110));
	memset(cLvStat, 0 , sizeof(cLvStat));
	memset(cAc_no , 0 , sizeof(cAc_no ));
	memset(cTemp , 0 , sizeof(cTemp ));
		
	get_zd_data("0980",cLvStat);
	get_zd_data("0300",cAc_no );
	get_zd_data("0500",cFinishFlag);
	get_zd_data(DC_TX_BR_NO,cKinbr);
	get_zd_data("0510",cCmtno);
	get_zd_data("0290",cRespCode); /* Ϊʲô29�򴫲����� ��*/
	get_zd_data("0270",cTemp);	
	get_zd_data("0890",cRcpstat);

	vtcp_log("[%s][%d] 1111 cRespCode=[%s]\n",__FILE__,__LINE__,cRespCode);		
	vtcp_log("[%s][%d] 1111 cTemp=[%s]\n",__FILE__,__LINE__,cTemp);				
	vtcp_log("[%s][%d] cmtno=[%s]\n",__FILE__,__LINE__,cCmtno);	
	vtcp_log("[%s][%d] cAc_no=[%s]\n",__FILE__,__LINE__,cAc_no);	
	
	vtcp_log(" %s,%d--- С������ ���������ӽ��� --- ",__FL__);
	/* begin NEWTCTD1 */
	/* begin add by LiuHuafeng 2007-9-2 */
	sprintf(cCmtno,"%03d",atoi(cCmtno));
	vtcp_log("[%s][%d] cmtno=[%s]\n",__FILE__,__LINE__,cCmtno);	
	/* end add by LiuHuafeng 2007-9-2 */
	/* end NEWTCTD1 */
	
	if(!memcmp(cCmtno,"254",3))
	{
		/* ���������254���Ľ����д��� */
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAcno110);
		if(iRet)
		{
			vtcp_log("%s,%d û�����û���%s��110�ʺ�",__FL__,cKinbr);
			strncpy(g_pub_tx.reply,"P076",4);
			goto ErrExit;
		}
		set_zd_data("0300",cAcno110);  /* 110  �ʺ� */
		set_zd_data("0330",LV_WAIT_QS);/* 414�ȴ������Ŀ */
	  
		goto OkExit;
	}
	else if(!memcmp(cCmtno,"920",3))
	{
		memcpy(cRespCode,cTemp+16,8);
		vtcp_log("[%s][%d] 2222 cRespCode=[%s]\n",__FILE__,__LINE__,cRespCode);		
		pub_base_strpack(cRespCode);
		vtcp_log("%s,%d �ܵ�920,Ӧ������[%s] ",__FL__,cRespCode);

		if(cRespCode[3]!='I' && strlen(cRespCode)!=0)
		{
			/* Ӧ�����Ǿܾ�  ����920����Ҫ���������� */
			strncpy(g_pub_tx.reply,"0002",4);  /* ���� JJJ1 */
			vtcp_log("%s,%d �ܵ�920,Ӧ������[%s],�Ǿܾ����͵ı���",__FL__,cRespCode);
			/****NEWYX2 BEGIN:�������н��ױ��ܾ���Ҫ���ʵ�****/
			if(memcmp(cTemp+24,PKGNO_TYXX,3)==0||
				memcmp(cTemp+24,PKGNO_SSXX,3)==0){
				vtcp_log("%s,%d,�յ��ܾ�920��ԭ��[%.3s]����Ҫ����",__FILE__,__LINE__,cTemp+24);
		    	strncpy(g_pub_tx.reply,"0001",4);  /* ���Ǿܾ������ʣ�ֱ�ӱ�J099�Ǽ� */
		    	set_zd_long("0530",-1);      /* iStep== -1  ��ʾ���ʴ������ */			
			}else{
			}
			/****NEWYX2 BEGIN:�������н��ױ��ܾ���Ҫ���ʵ�****/

			goto ErrExit;
		}
		/* begin NEWTCTD1 */
		/* add by LiuHuafeng 20070822 */
		else if(!memcmp(cCmtno,"003",3)
		&& (cLvStat[0]==STAT_LZNMAC_ERR
			||cLvStat[0]==STAT_LZLMAC_ERR
			||cLvStat[0]==STAT_LZAMAC_ERR
			||cLvStat[0]==STAT_LZGZ_QS
			||cLvStat[0]==STAT_LZGZ_BR)
		)
		{
			/* ����������ʵʱ���Ǿܾ�  ����Ҫ������ */
			set_zd_long("0530",-1);            /*iStep== -1  ��ʾ���ʴ������*/
			strncpy(g_pub_tx.reply,"0001",4);  /*���Ǿܾ�������ֱ�ӱ�J099�Ǽ� */
			vtcp_log("%s,%d ʵʱ���Ǿܾ�����Ҫ������",__FILE__,__LINE__);
			goto ErrExit;
		}
		/* end by LiuHuafeng 20070822 */
		/* end NEWTCTD1 */
		else
		{
		    set_zd_long("0530",-1);            /* iStep== -1  ��ʾ���ʴ������ */			
		    strncpy(g_pub_tx.reply,"0001",4);  /* ���Ǿܾ������ʣ�ֱ�ӱ�J099�Ǽ� */
  	    goto ErrExit;	
  	}
	}
	else if(cFinishFlag[0]=='1')
	{
		strncpy(g_pub_tx.reply,"0001",4);
		vtcp_log("%s,%d ����ʶ�����",__FL__);
		goto ErrExit;
	}
	/* begin NEWTCTD1 */
	/* begin add by LiuHuafeng 2007-9-2 */	
	if(!memcmp(cCmtno,"004",3))
	{
		/* ʵʱ���ҵ�����Ӧ��ɹ��߱���֧������������ֱ�Ӽǻ�ִ�� */
		set_zd_data("0660","1");/* �ͻ����ʷ��� */
		set_zd_data("0720","2");/* 110,414 ���ʷ��� */
		set_zd_data("0330",LV_WAIT_QS);/* 414�ȴ������Ŀ */
		set_zd_data("0320",LV_SUB02_SEND);/* ���ù��ɿ�Ŀ */
	}
	/* begin add by LiuHuafeng 2007-11-6 21:48:40 NEWTCTD5 */
	else if(!memcmp(cCmtno,PKGNO_SSDJHZ,3))
	{
		double dTxamt009=0.0;
		/* �����009�����ܾ�����ô�ŵ��������ʣ��ߺ��� */
		set_zd_data("0660","1");/* �ͻ����ʷ��� */
		set_zd_data("0720","2");/* 110,414 ���ʷ��� */
		set_zd_data("0330",LV_WAIT_QS);/* 414�ȴ������Ŀ */
		set_zd_data("0320",LV_SUB02_SEND);/* ���ù��ɿ�Ŀ */
		get_zd_double("0400",&dTxamt009);
		vtcp_log("%s,%d ���׽����%f",__FILE__,__LINE__,dTxamt009);
		set_zd_double("0400",dTxamt009 * -1);
		set_zd_double("0390",dTxamt009 * -1);
	}
	/* end by LiuHaufeng 2007-11-6 21:48:51 NEWTCTD5*/
	else
	{
	/* end add by LiuHuafeng 2007-9-2 */	
	/* end NEWTCTD1 */
		set_zd_data("0660","2");/* �ͻ����ʷ��� */
		set_zd_data("0720","1");/* 110,414 ���ʷ��� */
		set_zd_data("0330",LV_WAIT_QS);/* 414�ȴ������Ŀ */
		set_zd_data("0320",LVLZGD);/* ���ù��ɿ�Ŀ */
		vtcp_log("%s,%d,��ʼ���ÿ�Ŀ�Ƿ�ܾ���009Ҳ������",__FILE__,__LINE__);
	}
	if(cLvStat[0]==STAT_LZRECV2)
	{
		vtcp_log("%s,%d ����״̬Ϊ��������,�ʺ�=[%s]",__FL__,cAc_no);
	/**add by wangyongwei 20070913 �������� �ֽ��տ�Ĺ��˿�Ŀ*/
	#ifdef CZ_XQ
		if(memcmp(cCmtno,PKGNO_SSJJHZ,3)==0 && apatoi(cRcpstat,3)==0){
			char cCashacno[20];
			memset(cCashacno,0,sizeof(cCashacno));
			iRet=pub_base_GetParm_Str(cKinbr,PARM_LVGZ,cCashacno);
			if(iRet)
			{
				vtcp_log("%s,%d û�����û���%s��С������ʺ�",__FL__,cKinbr);
				strncpy(g_pub_tx.reply,"P076",4);
				goto ErrExit;
			}
			if(memcmp(cCashacno,cAc_no,sizeof(cCashacno)-1)==0){
				iRet=pub_base_GetParm_Str(cKinbr,PARM_CZ_LVLZGZ,cAc_no);
				if(iRet)
				{
					vtcp_log("%s,%d û�����û���%s��С������ʺ�",__FL__,cKinbr);
					strncpy(g_pub_tx.reply,"P076",4);
					goto ErrExit;
				}
				set_zd_data("0300",cAc_no);
				set_zd_data("0680","1");
			}
		}
	#endif	
	/**add by wangyongwei 20070913 �������� �ֽ��տ�Ĺ��˿�Ŀ*/
	}
	else if(cLvStat[0]==STAT_LZNMAC_ERR ||cLvStat[0]==STAT_LZLMAC_ERR
	 ||cLvStat[0]==STAT_LZAMAC_ERR ||cLvStat[0]==STAT_LZGZ_QS 
	 ||cLvStat[0]==STAT_LZGZ_BR
	)
	{
	/**add by wangyongwei NEWTCTD 20070913*���ݳ��������ֽ�⸶�Ĺ��˿�Ŀ�ŵ�Ӧ����*/
	#ifdef CZ_XQ
		if((memcmp(cCmtno,PKGNO_SSDJHZ,3)==0 || memcmp(cCmtno,PKGNO_SSJJHZ,3)==0)&& apatoi(cRcpstat,3)!=0){
			vtcp_log("%s,%d �Ƿ��ߵ�����********����״̬Ϊ��������,�ʺ�=[%s]",__FL__,cAc_no);
			iRet=pub_base_GetParm_Str(cKinbr,PARM_CZ_LVLZGZ,cAc_no);
			if(iRet)
			{
				vtcp_log("%s,%d û�����û���%s��С������ʺ�",__FL__,cKinbr);
				strncpy(g_pub_tx.reply,"P076",4);
				goto ErrExit;
			}
			set_zd_data("0300",cAc_no);
			set_zd_data("0680","1");
		}else{
			/* ��Ҫ�������ù��˿�Ŀ */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_LVGZ,cAc_no);
			if(iRet)
			{
				vtcp_log("%s,%d û�����û���%s��С������ʺ�",__FL__,cKinbr);
				strncpy(g_pub_tx.reply,"P076",4);
				goto ErrExit;
			}
			set_zd_data("0300",cAc_no);
			set_zd_data("0680","6");
		}
	#else
	/**add by wangyongwei NEWTCTD 20070913*���ݳ��������ֽ�⸶�Ĺ��˿�Ŀ�ŵ�Ӧ����*/
		/* ��Ҫ�������ù��˿�Ŀ */
		iRet=pub_base_GetParm_Str(cKinbr,PARM_LVGZ,cAc_no);
		if(iRet)
		{
			vtcp_log("%s,%d û�����û���%s��С������ʺ�",__FL__,cKinbr);
			strncpy(g_pub_tx.reply,"P076",4);
			goto ErrExit;
		}
		set_zd_data("0300",cAc_no);
		set_zd_data("0680","6");
	#endif
	}
	
OkExit:
	memcpy(g_pub_tx.reply,"0000",4);
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


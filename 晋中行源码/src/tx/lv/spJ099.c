/*************************************************
* �� �� ��:  spJ099.c
* ���������� С������ �Ǽ����� �ӽ���
*			       ���� �Ǽ����ʱ�
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

int spJ099()
{
	char chkcmtno[4];
	char txtime[7];	
	char cErrmsg[9];
	long iSubpackIndex=0;
	int iStep=0;    
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("spJ098() С������ �Ǽ����� ��ʼ  started! [%ld]",iSubpackIndex);

	vtcp_log(" --- spJ099 С������ �Ǽ����� ��ʼ --- ");	 	
	pub_base_sysinit();
	memset(cErrmsg,0,sizeof(cErrmsg));
	memset(chkcmtno,'\0',sizeof(chkcmtno));
	memset(txtime,'\0',sizeof(txtime));

	get_zd_data("0510",chkcmtno);	
	vtcp_log("[%s][%d]--- chkcmtno =[%s] ",__FILE__,__LINE__,chkcmtno);	
	get_zd_long("0530",&iStep);	
	vtcp_log(" [%s][%d]--- iStep=[%d] ",__FILE__,__LINE__,iStep);
	
	if(iStep != -1) /* ������ţ�iStep==-1��ʾ���ʴ������ */
	{
    	/* ========= 322,324,325,920 ������������δ����    ========= 
    	   ========= ��Щ����ֻ����������������ʱ�ŵǼ�  =========*/
    	if( (memcmp(chkcmtno,"322",3)==0 )||
    		  (memcmp(chkcmtno,"324",3)==0 )||
    		  (memcmp(chkcmtno,"325",3)==0 )||
    		  (memcmp(chkcmtno,"920",3)==0 ) )
    	{
    	    char cFinishflg[2];
    			memset(cFinishflg, 0 , sizeof(cFinishflg));
    			vtcp_log("[%s][%d]--- 322,324,325,920 ������������δ���� ���Ǽ� iStep=[%d] ",__FL__,iStep);
    		  	strcpy(g_pub_tx.reply,"0010" ); /* ����J098���� */
    		  	/* begin add by LiuHuafeng 2009-5-15 11:10:21 for �������� */
    			get_zd_data("0500",cFinishflg);
    			if(cFinishflg[0]=='1' && memcmp(chkcmtno,"920",3)==0)
    			{
    				strcpy(g_pub_tx.reply,"0000" ); /* �����Ǽ� */
    			}
    			/* end add by LiuHuafeng 2009-5-15 11:10:21 for �������� */
    			goto OkExit;
    	}
	}

	vLvBrnoDebug(__FILE__,__LINE__);
	/* ���� С�����ʵǼ����� ������ */
	if (pubf_lv_recv_reg_main(chkcmtno))
	{
		get_zd_data("0130",cErrmsg);
        if(memcmp(cErrmsg,"Ӧ���ظ�",8)==0)
        { 
           strcpy(g_pub_tx.reply,"P022" );
        }
        else
        {
           strcpy(g_pub_tx.reply,"0044" );
        }  
		vLvBrnoDebug(__FILE__,__LINE__);
		goto ErrExit;  
	}
	vLvBrnoDebug(__FILE__,__LINE__);
OkExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ099() С������ ���� �ӽ��׽��� end! [%ld]",__FL__,iSubpackIndex);

	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ099() С������ ���� �ӽ��׽��� end! [%ld]",__FL__,iSubpackIndex);

	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

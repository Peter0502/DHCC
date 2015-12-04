/******************************************************************
ģ������: ���׻�����Ϣ��ѯ
��������: spJ028.c
����ԭ��: spJ028()
�������:
�������:
ʹ��˵��:
�� д ��: dxy
*******************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "hv_fd123.h"
#include "lv_pkgreg_c.h"
#include "hv_fzf_c.h"
#include "hv_zf_c.h"
#include "find_debug.h"
   


static struct {
	char brno[5];	/**���׻���**/
	char wtday[8];	/**ί������**/
}tis19;



  
/*****************************************
*����˵��:��ȡһ�����������еĽ��׵ı��� *
******************************************/
int spJ028()
{
	vtcp_log("[%s][%d]���ܽ��׽��ײ�ѯ\n",__FILE__,__LINE__);
	
	int	iLHD = 0;	/*���*/
	int iLWT = 0;	/*ί��*/
	int iLTS = 0; 	/*����*/
	int iLGK = 0;	/*����*/
	int iLCJ = 0;	/*���*/
	int iLTH = 0;	/*�˻�*/
	int iLCX = 0;	/*��ѯ*/
	int iLCF = 0;	/*�鸴*/

	int	iWHD = 0;	/*���*/
	int iWWT = 0;	/*ί��*/
	int iWTS = 0; 	/*����*/
	int iWGK = 0;	/*����*/
	int iWCJ = 0;	/*���*/
	int iWTH = 0;	/*�˻�*/
	int iWCX = 0;	/*��ѯ*/
	int iWCF = 0;	/*�鸴*/


	int iRecordNum = 0;

	char cWherelist[500];
	memset(cWherelist,0x00,sizeof(cWherelist));
	
	get_zd_data("0910",tis19.brno);
	get_zd_data("0440",tis19.wtday);
	
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) 
		&& ((tis19.brno[0]==0x00 ||tis19.brno[0]==' ')|| memcmp(g_pub_tx.tx_br_no,tis19.brno,5)))
	{
		vtcp_log("%s,%d,���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	if(!memcmp(g_pub_tx.tx_br_no, QS_BR_NO ,5))
	{
		memset(tis19.br_no,0x00,sizeof(tis19.br_no));
		if(tis19.brno[0]==0x00 ||tis19.brno[0]==' ')
		{
			MEMCPY_DEBUG(tis19.br_no,"%",1);	
		}
	}
	/*ret = Com_branch_Dec_Sel(g_pub_tx.reply,"br_no like '%%%s'  and wrk_sts!='*' and br_type not in('0','6') ) ",tis19.br_no);20150525 lwb ���ӷ���    */
	ret = Com_branch_Dec_Sel(g_pub_tx.reply,"br_no like '%%%s'  and wrk_sts!='*' and br_type not in('0','6','7') ) ",tis19.br_no);
	if(ret)
	{
		vtcp_log("[%s][%d]��ѯ�кŴ���!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","��ѯ�кŴ���");
		goto ErrExit;
	}
	while(1)
	{
		ret = Com_branch_Fet_Sel(&s_com_branch,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			strcpy(g_pub_tx.reply,"HV01");
			set_zd_data("0130","��ȡ�����Ŵ���!");
			goto ErrExit;
		}
		if(ret==100)
			break;
		if(iRecordNum==0)
		{
			pub_base_AllDwnFilName( filename);
		    fp = fopen(filename,"w");
		    if(fp==NULL)
		   {
				sprintf(acErrMsg,"open file [%s] failed!\n",filename);
				WRITEMSG
				strcpy(g_pub_tx.reply,"S047");
				goto ErrExit;
			}
			/****��ʾ����****/
			
			fprintf(fp,"~@ǩ������  |@ǩ���к�      |@��Ʊ����      |$��Ʊ���        |@��Ʊ����        |@��Ʊ״̬       |@�����ʺ�                        |@��������          |@����|\n" );

		}
		iQuery_Tx_Cnt(char *tablename,char *cmtno,char lw_flag,char *wt_date,char *br_no)
			
	}
	
			
	
	vtcp_log("[%s][%d]tis19===[%s]\n",__FILE__,__LINE__,(char *)&tis19);

	vtcp_log("[%s][%d]��ʼִ�л��ܲ�ѯ������!\n",__FILE__,__LINE__);
	

	
	
	
	
	
	set_zd_data("1230",(char *)&fd123);
	
	vtcp_log("[%s][%d]fd123====[%s]\n",__FILE__,__LINE__,(char *)&fd123);
	
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"��ѯ���ܽ��׳ɹ�[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;

}

int iQuery_Tx_Cnt(char *tablename,char *cmtno,char lw_flag,char *wt_date,char *br_no)
{
	vtcp_log("[%s][%d]��ʼ��ѯ��¼����,table_name=[%s],cmtno=[%s],lw_flag=[%c],date=[%s],br_no=[%s]\n",
						__FILE__,__LINE__,tablename,cmtno,lw_flag,wt_date,br_no);

	int iCnt = 0;
	iCnt = sql_count(tablename,"wt_date='%.8s and br_no='%.5s' and cmtno='%.3s' and lw_ind='%c'");

	vtcp_log("[%s][%d]��ѯ���ļ�¼����=[%d]\n",__FILE__,__LINE__,iCnt);

	return iCnt;
} 
	
	


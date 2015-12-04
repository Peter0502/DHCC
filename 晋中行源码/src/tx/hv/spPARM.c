/*************************************************************
* �� �� ��: testForParm
* �������������Ը��²�����
*
* ��    ��: LiuHuafeng
* �������: 2006-10-27 17:41
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "find_debug.h"

#include "com_branch_c.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"



struct com_parm_c    com_parm;
struct mdm_ac_rel_c  mdm_ac_rel;
struct com_branch_c  com_branch;
struct in_mst_c      in_mst;
FILE 	*fp;
PARM()
{
	int ret = 0;
	char fullname[100];
	int flag=0;
	int opncnt;
	char vpfm[21];
	char cLines[1024];
	memset(&com_parm, 0 , sizeof(com_parm));
	
	fp=fopen("/home/newsys/etc/mbfe_parm.txt","rb");
	if(fp==NULL)
	{
		vtcp_log("%s,%d �򿪲��������ļ�����",__FILE__,__LINE__);
		goto ErrExit;		
	}
		
	memset(&com_parm  , 0 , sizeof(com_parm  ));
	memset(&mdm_ac_rel, 0 , sizeof(mdm_ac_rel));
	memset(&com_branch, 0 , sizeof(com_branch));
	memset(&in_mst    , 0 , sizeof(in_mst    )); 


	while(1)
	{
		int i=0;
		char cPrdt_no [4];
		char cParm_seq[5];
		memset(cLines, 0 ,sizeof(cLines));
		memset(cPrdt_no , 0 , sizeof(cPrdt_no ));
		memset(cParm_seq, 0 , sizeof(cParm_seq));
		fgets(cLines,1024,fp);
		zip_space(cLines);
		if(strlen(cLines)<2) break;
		for(i=0;i<strlen(cLines);i++)
		{
			if(cLines[i]!='|')
			{
				cPrdt_no[i]=cLines[i];
			}
			else break;
		}
		MEMCPY_DEBUG(cParm_seq,cLines+i+1,sizeof(cParm_seq)-1);
		vtcp_log("%s,%d ��Ʒ���� =[%s] ��� =[%s]",__FILE__,__LINE__,cPrdt_no,cParm_seq);
		memset(&com_branch, 0 , sizeof(com_branch));
		ret=Com_branch_Dec_Sel(g_pub_tx.reply,"br_type !='0' and br_type !='6'");
		while(1)
		{
			ret=Com_branch_Fet_Sel( &com_branch , g_pub_tx.reply );
			if(ret==100)
			{
				Com_branch_Clo_Sel();
				break;
			}
			if(ret)
			{
				vtcp_log("%s,%d ��ȡ����%s��",__FILE__,__LINE__,com_branch.br_no);
				goto ErrExit;
			}
			/* �����˻��Ƿ���ڣ�������ھͲ������� */
			memset(&com_parm, 0 , sizeof(com_parm));
			MEMCPY_DEBUG(com_parm.parm_code,com_branch.br_no,sizeof(com_parm.parm_code)-1);
			com_parm.parm_seqn=atoi(cParm_seq);
			ret=Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=%d",com_branch.br_no,com_parm.parm_seqn);
			if(ret==0)
			{
				MEMCPY_DEBUG(mdm_ac_rel.ac_no,com_parm.val,sizeof(mdm_ac_rel.ac_no));
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",mdm_ac_rel.ac_no);
				if(ret==0)
				{
					/* ���in_mst �����Ƿ�������� */
					ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%d",mdm_ac_rel.ac_id);
					if(ret==0)
					{
						if(in_mst.sts[0]=='1'){
							vtcp_log("%s %d �˺�����",__FILE__,__LINE__);
							continue;
						}
					}
				}
			}
			vtcp_log("%s %d ��Ҫ�滻�˺�",__FILE__,__LINE__);
			ret=In_mst_Dec_Sel(g_pub_tx.reply,"prdt_no='%s' and bal >=0 and opn_br_no='%s' and sts='1' ",cPrdt_no,com_branch.br_no);
			ret=In_mst_Fet_Sel( &in_mst, g_pub_tx.reply );
			if(ret)
			{
				vtcp_log("%s,%d ����%s�Ĳ�Ʒ%s ��Ӧ�ڲ��˺Ų�������Ҫ�����ֹ�����",__FILE__,__LINE__,com_branch.br_no,cPrdt_no);
				continue;
			}
			ret=Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id=%d ",in_mst.ac_id);
			ret=Mdm_ac_rel_Fet_Sel(&mdm_ac_rel,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d ����%s�Ĳ�Ʒ%s ��Ӧ�ڲ��˺Ų�����",__FILE__,__LINE__,com_branch.br_no,in_mst.prdt_no);
				goto ErrExit;
			}
			memset(&com_parm, 0 , sizeof(com_parm));
			MEMCPY_DEBUG(com_parm.parm_code,com_branch.br_no,sizeof(com_parm.parm_code)-1);
			com_parm.parm_seqn=atoi(cParm_seq);
			ret=Com_parm_Dec_Upd( g_pub_tx.reply , "parm_code='%s' and parm_seqn=%d ",com_branch.br_no,com_parm.parm_seqn );
			ret=Com_parm_Fet_Upd(&com_parm,g_pub_tx.reply);
			if(ret)
			{
				com_parm.val_type[0]='5';
				MEMCPY_DEBUG(com_parm.val,mdm_ac_rel.ac_no,strlen(mdm_ac_rel.ac_no));
				ret=Com_parm_Ins(com_parm,g_pub_tx.reply);
				if(ret)
				{
					vtcp_log("%s,%d ����%s�Ĳ�Ʒ%s д����������",__FILE__,__LINE__,com_branch.br_no,in_mst.prdt_no);
					goto ErrExit;
				}
				continue;
			}
			MEMCPY_DEBUG(com_parm.val,mdm_ac_rel.ac_no,strlen(mdm_ac_rel.ac_no));
			ret=Com_parm_Upd_Upd(com_parm,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d ����%s�Ĳ�Ʒ%s ���£��������",__FILE__,__LINE__,com_branch.br_no,in_mst.prdt_no);
				goto ErrExit;
			}
		}
		Com_branch_Clo_Sel();
	}
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���²�����ɹ�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"���²�����ʧ��[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

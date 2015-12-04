/*************************************************************
* 文 件 名: testForParm
* 功能描述：测试更新参数表
*
* 作    者: LiuHuafeng
* 完成日期: 2006-10-27 17:41
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

#include "com_branch_c.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "find_debug.h"



struct com_parm_c    com_parm;
struct mdm_ac_rel_c  mdm_ac_rel;
struct com_branch_c  com_branch;
struct in_mst_c      in_mst;
FILE 	*fp;
MbfeUpdParm()
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
		vtcp_log("%s,%d 打开参数配置文件错误",__FILE__,__LINE__);
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
		memset(cLine, 0 ,sizeof(cLine));
		memset(cPrdt_no , 0 , sizeof(cPrdt_no ));
		memset(cParm_seq, 0 , sizeof(cParm_seq));
		fgets(cLine,1024,fp);
		zip_space(cLine);
		if(strlen(cLine)<2) break;
		for(i=0,i<strlen(cLine);i++)
		{
			if(cLine[i]!='|')
			{
				cPrdt_no[i]=cLine[i];
			}
			else break;
		}
		MEMCPY_DEBUG(cParm_seq,cLine+i,sizeof(cParm_seq)-1);
		vtcp_log("%s,%d 产品号码 =[%s] 序号 =[%s]",__FILE__,__LINE__,cPrdt_no,cParm_seq);
		memset(&com_branch, 0 , sizeof(com_branch));
		ret=Com_branch_Dec_Sel(g_pub_tx.reply,"1=1");
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
				vtcp_log("%s,%d 读取机构%s错",__FILE__,__LINE__,com_branch.br_no);
				goto ErrExit;
			}
			ret=In_mst_Dec_Sel(g_pub_tx.reply,"prdt_no='%s' and bal >=0 and opn_br_no='%s'",cPrdt_no,com_branch.br_no);
			ret=In_mst_Fet_Sel( &in_mst, g_pub_tx.reply );
			if(ret)
			{
				vtcp_log("%s,%d 机构%s的产品%s 对应内部账号不存在",__FILE__,__LINE__,com_branch.br_no,in_mst.prdt_no);
				continue;
			}
			ret=Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id=%d ",in_mst.ac_id);
			ret=Mdm_ac_rel_Fet_Sel(&mdm_ac_rel,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d 机构%s的产品%s 对应内部账号不存在",__FILE__,__LINE__,com_branch.br_no,in_mst.prdt_no);
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
					vtcp_log("%s,%d 机构%s的产品%s 写入ｃｏｍｐｒａｍ错",__FILE__,__LINE__,com_branch.br_no,in_mst.prdt_no);
					goto ErrExit;
				}
				continue;
			}
			MEMCPY_DEBUG(com_parm.val,mdm_ac_rel.ac_no,strlen(mdm_ac_rel.ac_no));
			ret=Com_pram_Upd_upd(&com_parm,g_pub_reply);
			if(ret)
			{
				vtcp_log("%s,%d 机构%s的产品%s 更新ｃｏｍｐｒａｍ错",__FILE__,__LINE__,com_branch.br_no,in_mst.prdt_no);
				goto ErrExit;
			}
		}
		Com_branch_Clo_Sel();
	}
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"更新参数表成功[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"更新参数表失败[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

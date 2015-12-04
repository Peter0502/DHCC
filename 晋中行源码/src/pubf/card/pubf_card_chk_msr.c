/***卡号校验***/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "card_reg_c.h"

/**********************************************
 检查卡磁条信息是否正确
 0---- 不检查
 1---- 检查2磁道
 2---- 检查3磁到
 3---- 仅仅核对2磁道cvn所在位置值
 4---- 检查所有磁道
**********************************************/
int pubf_card_chk_msr(int checkflag,char * card_no)
{
	char cMsr2[38];
	char cMsr3[142];
	struct card_reg_c sCard_reg;
	memset(&sCard_reg , 0 , sizeof(sCard_reg));
	memset(cMsr2, 0 ,sizeof(cMsr2));
	memset(cMsr3, 0 ,sizeof(cMsr3));
	if(checkflag==0) return 0;
	get_zd_data("0750",cMsr2);
	get_zd_data("0760",cMsr3);
	/*vtcp_log("%s,%d msr2==[%s]",__FILE__,__LINE__,cMsr2);
	vtcp_log("%s,%d msr3==[%s]",__FILE__,__LINE__,cMsr3);*/
	pub_base_strpack(cMsr2);
	pub_base_strpack(cMsr3);
	
	memcpy(sCard_reg.ac_no,card_no,sizeof(sCard_reg.ac_no)-1);
	pub_base_strpack(sCard_reg.ac_no);
	vtcp_log("%s,%d 查询card_reg ",__FILE__,__LINE__);
	g_reply_int=Card_reg_Sel(g_pub_tx.reply,&sCard_reg,"ac_no='%s'",sCard_reg.ac_no);
	if(g_reply_int==100)
	{
		vtcp_log("%s,%d 卡登记簿中不存在数据,ac_no=[%s]",__FILE__,__LINE__,sCard_reg.ac_no);
		sprintf(acErrMsg,"卡登记簿中不存在数据,ac_no=[%s]",sCard_reg.ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "D111" );
		return (g_reply_int);
	}
	else if(g_reply_int)
	{
		vtcp_log("%s,%d 数据库操作错误,ac_no=[%s],ret=[%d]",__FILE__,__LINE__,sCard_reg.ac_no,g_reply_int);
		sprintf(acErrMsg,"卡登记簿中不存在数据,ac_no=[%s]",sCard_reg.ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "D111" );
		return (g_reply_int);
	}
	vtcp_log("%s,%d 查询card_reg 结束，找到卡号",__FILE__,__LINE__);
	/*vtcp_log("%s,%d table.msr2=[%s]",__FILE__,__LINE__,sCard_reg.msr2);*/
	if(checkflag==1)
	{
	/*	vtcp_log("%s,%d table.msr2=[%s]",__FILE__,__LINE__,sCard_reg.msr2);*/
		/* 检查2磁道 */
		if(memcmp(sCard_reg.msr2,cMsr2,strlen(sCard_reg.msr2)))
		{
		vtcp_log("%s,%d ac_no=[%s],2磁道信息错误input=[%s]table[%s]",__FILE__,__LINE__,sCard_reg.ac_no,cMsr2,sCard_reg.msr2);
		sprintf(acErrMsg,"2磁道信息错误,ac_no=[%s]",sCard_reg.ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "AT01" );
		return (-1);
		}
	}
	else if(checkflag==2)
	{
		/*vtcp_log("%s,%d table.msr3=[%s]",__FILE__,__LINE__,sCard_reg.msr3);*
		/* 检查3磁道 */
		if(memcmp(sCard_reg.msr3,cMsr3,strlen(sCard_reg.msr3)))
		{
		vtcp_log("%s,%d ac_no=[%s],2磁道信息错误input=[%s]table[%s]",__FILE__,__LINE__,sCard_reg.ac_no,cMsr3,sCard_reg.msr3);
		sprintf(acErrMsg,"3磁道信息错误,ac_no=[%s]",sCard_reg.ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "AT01" );
		return (-1);
		}
	}
	else if(checkflag==3)
	{
		/*vtcp_log("%s,%d table.cvn1=[%s]",__FILE__,__LINE__,sCard_reg.cvn1);*/
		/* 检查2磁道cvn */
		if(memcmp(sCard_reg.cvn1,cMsr2+30,sizeof(sCard_reg.cvn1)-1))
		{
		vtcp_log("%s,%d ac_no=[%s],2磁道信息错误input=[%.3s]table[%s]",__FILE__,__LINE__,sCard_reg.ac_no,cMsr2+30,sCard_reg.cvn1);
		sprintf(acErrMsg,"磁道信息cvn1错误,ac_no=[%s]",sCard_reg.ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "AT01" );
		return (-1);
		}
	}
	else if(checkflag==4)
	{
		/*vtcp_log("%s,%d table.msr2=[%s]",__FILE__,__LINE__,sCard_reg.msr2);*/
		/* 检查2,3磁道 */
		if(memcmp(sCard_reg.msr2,cMsr2,strlen(sCard_reg.msr2)))
		{
		vtcp_log("%s,%d ac_no=[%s],2磁道信息错误input=[%s]table[%s]",__FILE__,__LINE__,sCard_reg.ac_no,cMsr2,sCard_reg.msr2);
		sprintf(acErrMsg,"2磁道信息错误,ac_no=[%s]",sCard_reg.ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "AT01" );
		return (-1);
		}
		if(memcmp(sCard_reg.msr3,cMsr3,strlen(sCard_reg.msr3)))
		{
		vtcp_log("%s,%d ac_no=[%s],2磁道信息错误input=[%s]table[%s]",__FILE__,__LINE__,sCard_reg.ac_no,cMsr3,sCard_reg.msr3);
		sprintf(acErrMsg,"3磁道信息错误,ac_no=[%s]",sCard_reg.ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "AT01" );
		return (-1);
		}
	}
	else
	{
		vtcp_log("%s,%d 参数错误,ac_no=[%s],checkflag=[%d]",__FILE__,__LINE__,sCard_reg.ac_no,checkflag);
		sprintf(acErrMsg,"参数错误,ac_no=[%s]",sCard_reg.ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "AT01" );
		return (-1);
	}
	return 0;
}

/*************************************************
* 文 件 名:  sp6680.c
* 功能描述： 信贷用单笔授权信息查询
*
* 作    者:  李吉祥
* 完成日期： 20110721
*
* 修改记录：
* 日    期:
* 修 改 人: 
* 修改内容:
* 说明：tx_code='6680' sub_tx_code='6680'
        insert into tx_def values ('6680', '信贷用授权信息查询', '10000000000000000000000000000100000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
        insert into tx_flow_def values('6680','0','6680','#$');
        insert into tx_sub_def values ('6680', '信贷用授权信息查询', 'sp6680', '0', '0');

        输入：0300域 最长24位 char 授权编号
        输出：0300域 最长24位 char 授权编号(不变)
	            0670域 1位 char 当前状态
              0－登记，1－成功，2－撤销
*************************************************/
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
void getstr(char * a,char * b,int m,int n){
	memset(a,'\0',m);
	zip_space(b);
	memcpy(a,b,m-1);	
	memset(b, '\0', n);
}
int sp6680(){
	int ret=0;
	int count=0;
	int flag_sum=0;
	char strtmp[201];
	struct cms_rollback_c m_cms_rollback;
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&m_cms_rollback,0,sizeof(m_cms_rollback));
	memset(strtmp,'\0',sizeof(strtmp));
	get_zd_data( "0300", strtmp );		/* 转贴现协议编号 */
	getstr(m_cms_rollback.cms_sq_no,strtmp,sizeof(m_cms_rollback.cms_sq_no),sizeof(strtmp));
	
	ret=Cms_rollback_Dec_Sel(g_pub_tx.reply,"cms_sq_no ='%s' and (substr(filler,0,1)<>'#' or filler is null )",m_cms_rollback.cms_sq_no);
	if(ret)
	{
		sprintf( acErrMsg, "sql error" );
		WRITEMSG
		set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
		goto ErrExit;
	}
	count=0;
	flag_sum=0;
	while(1){
		ret=Cms_rollback_Fet_Sel(&m_cms_rollback,g_pub_tx.reply);
		if(ret){
			if(ret==100)
			{
				if(!count)
				{
					strcpy( g_pub_tx.reply, "XD00" );
					set_zd_data("0130","XD00");/*表示没有查到结果*/
					goto ErrExit;
				}
			break;
			}
			else{
				sprintf(acErrMsg,"sql error");
				WRITEMSG
				set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
				Cms_rollback_Clo_Sel();
				goto ErrExit;
			}
		}
		if(!strcmp(m_cms_rollback.sts,"2")){/*有一个2则返回2*/
			set_zd_data("0670","2");
			set_zd_data("0130","0000");
			Cms_rollback_Clo_Sel();
			goto OKExit;
		}
		if(sql_count(m_cms_rollback.beg_table,m_cms_rollback.beg_sql)){
			flag_sum+=1;
		}
		count+=1;		
	}
	if(flag_sum){
		set_zd_data("0670","0");/**登记 **/
		set_zd_data("0130","0000");
	}else{
		set_zd_data("0670","1");
		set_zd_data("0130","0000");/*都为0则返回1  成功*/
	}
	Cms_rollback_Clo_Sel();
		
OKExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s] ret[%d] ",g_pub_tx.reply,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s] ret[%d] ",g_pub_tx.reply,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
	
	
	

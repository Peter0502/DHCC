/*************************************************
* 文 件 名:  sp6670.c
* 功能描述： 查找客户号与客户姓名
*
* 作    者:  李吉祥
* 完成日期： 20110720
*
* 修改记录：
* 日    期:
* 修 改 人: 
* 修改内容:
* 说明：tx_code='6670' sub_tx_code='6670' 
        insert into tx_def values('6670','信贷查询客户号','00000000000000000000000000111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
        insert into tx_flow_def values('6670','0','6670','#$');
        insert into tx_sub_def  values ('6670', '信贷查询客户号', 'sp6670', '0', '0');
        输入：0670域获得证件类型，0620域获得证件号码
        输出：0130域结果回馈（ "0000"成功，"XD00"无结果， "ERROR"运行失败）
              0270返回姓名，0280返回客户号（long型）
*************************************************/
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
int sp6670(){
	struct cif_id_code_rel_c cif_id_code_rel;
	struct cif_basic_inf_c cif_basic_inf;
	int ret=0;
	char dmz[21];/*代码证*/
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&cif_id_code_rel,0,sizeof(cif_id_code_rel));
	memset(&cif_basic_inf,0,sizeof(cif_basic_inf));
	memset(dmz,'\0',sizeof(dmz));
	
	get_zd_data("0670",cif_id_code_rel.id_type);/* 获得证件类型 */
	get_zd_data("0620",cif_id_code_rel.id_no);/* 获得证件号码 */
	sprintf(acErrMsg,"at line36: reply [%s] id_type[%s]  id_no[%s]",g_pub_tx.reply,cif_id_code_rel.id_type,cif_id_code_rel.id_no);
	WRITEMSG
	if(!strcmp(cif_id_code_rel.id_type,"8")){
		if(cif_id_code_rel.id_no[8]=='-'){	
		  cif_id_code_rel.id_no[8]='%';
	  }
	}/*考虑到信贷发来的数据都是10位，如11277369-X 而之前的核心数据为9位 是11277369X 因此做一个对应*/
	sprintf(acErrMsg,"at line43: reply [%s] id_type[%s]  id_no[%s]",g_pub_tx.reply,cif_id_code_rel.id_type,cif_id_code_rel.id_no);
	WRITEMSG
				
	
	ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,"id_type='%s' and id_no like '%s' ",cif_id_code_rel.id_type,cif_id_code_rel.id_no);
	if(ret){
		if(ret==100){
			strcpy( g_pub_tx.reply, "XD00" );
			set_zd_data("0130","XD00");/*表示没有查到结果*/			
			goto ErrExit;
		}
		else{
			set_zd_data("0130","ERROR");/*表示程序执行失败*/
			goto ErrExit;
		}
	}
	sprintf(acErrMsg,"at line55: reply [%s] ret[%d]  cif_no[%ld]",g_pub_tx.reply,ret,cif_id_code_rel.cif_no);
	WRITEMSG
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld ",cif_id_code_rel.cif_no);
	if(ret){
		if(ret==100){
			strcpy( g_pub_tx.reply, "XD00" );
			set_zd_data("0130","XD00");	/*表示没有查到结果*/
			goto ErrExit;
		}
		else{
			set_zd_data("0130","ERROR");/*表示程序执行失败*/
			goto ErrExit;
		}
	}
	set_zd_long("0280",cif_basic_inf.cif_no);
	set_zd_data("0270",cif_basic_inf.name);
OKExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s] ret[%d] ",g_pub_tx.reply,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	set_zd_data("0130","0000");
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s] ret[%d] ",g_pub_tx.reply,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
		

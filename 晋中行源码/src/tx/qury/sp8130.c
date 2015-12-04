/*****************************************************
*                                                    *
*    程序名称:sp8130.c                               *
*    程序功能:查询账户子账号信息                     *
*    作    者: GUJINGYU                              *
*    完成日期:2010年10月20日                         *
*    修改记录:chenchao                               *
*             修改输入日期时打印不正确问题           *
*                                                    *
*****************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"
#include "public.h"

#define ERR_DEAL if( ret ) {\
       sprintf( acErrMsg, "sql error" ); \
       WRITEMSG \
       goto ErrExit; \
       }

sp8130()
{
	char zAc_no[25];
	long fAc_no=0;
	char name[61];
	char tmplist[100];
	char wherelist[256];
	char filename[100];
	char cQs[6];
	char ac_sts[10];
	int  ret=0;
	int  ttlnum=0;
	long tx_date=0;
	struct sub_dd_mst_c sub_dd_mst;
	struct sub_dd_mst_hst_c sub_dd_mst_hst;
	double sub_sum_amt=0.00;
	FILE *fp=NULL;
	
	memset(zAc_no,0x00,sizeof(zAc_no));
	memset(filename,0x00,sizeof(filename));
	memset(name  ,0x00,sizeof(name  ));
	memset(ac_sts,0x00,sizeof(ac_sts));
	memset(wherelist  ,0x00,sizeof(wherelist));
	memset(tmplist  ,0x00,sizeof(tmplist));
	memset(&sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c)-1);
	memset(&sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c)-1);
	memset(cQs, 0 , sizeof(cQs));
	
	pub_base_sysinit();

	get_zd_data("0300",zAc_no);
	get_zd_long("0780",&fAc_no);
	get_zd_data("0250",name);
	get_zd_long("0440",&tx_date);
	pub_base_old_acno(zAc_no);   /** 对旧帐号的处理 **/
	iFind_qs_br_no2(g_pub_tx.tx_br_no,cQs);

	/*只有总行清算中心和科技中心可以查所有存款明细*/
	if(memcmp(g_pub_tx.tx_br_no,QS_BR_NO,BRNO_LEN) != 0 && memcmp(g_pub_tx.tx_br_no,cQs,BRNO_LEN) != 0 && memcmp(g_pub_tx.tx_br_no,KJ_BR_NO,BRNO_LEN) != 0)
	{
		memset(tmplist,0x00,sizeof(tmplist));
		sprintf( tmplist," br_no='%s' and ",g_pub_tx.tx_br_no );
		strcat( wherelist,tmplist );
	}
	if(strlen(zAc_no))
	{
		sprintf(tmplist,"ac_no = '%s' and ",zAc_no);
		strcat(wherelist,tmplist);
	}
	if(fAc_no!=0)
	{
		sprintf(tmplist," sub_ac_seqn=%d and ",fAc_no); 
		strcat(wherelist,tmplist);
	}
	if(strlen(name))
	{
		sprintf(tmplist," name like '%%%%%s%%%%' and ",name);
		strcat(wherelist,tmplist);
	}
	if(tx_date>0)/** add by chenchao 2011-5-19 20:37:23**/
	{
		sprintf(tmplist," opn_date <= %ld and ",tx_date);
		strcat(wherelist,tmplist);
	}

	strcat(wherelist," 1=1 order by sub_ac_seqn");
	
	ret=Sub_dd_mst_Dec_Sel(g_pub_tx.reply,wherelist);
	ERR_DEAL
	

	while(1)
	{
		ret=Sub_dd_mst_Fet_Sel( &sub_dd_mst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			break;
		}
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			fprintf(fp,"~@子账号|@户名|$余额|@积数|@利率|@状态|\n");
		}
		switch(sub_dd_mst.ac_sts[0]){
		case '1':
				sprintf(ac_sts,"正常");
				break;
		case '*':
				sprintf(ac_sts,"销户");
				break;
		}
		/**if(sub_dd_mst.ac_sts[0]=='*' )**/
		if( tx_date > sub_dd_mst.lst_date && sub_dd_mst.ac_sts[0] == '*') /** modify by chenchao 2011-5-19 18:48:34 **/
		{	 /*不显示销户的记录zyhli0 */
			continue;
		}
		
		if(tx_date!=0 && tx_date<sub_dd_mst.lst_date)
		{
			ret=Sub_dd_mst_hst_Sel(g_pub_tx.reply,&sub_dd_mst_hst,"sub_ac_no='%s' and tx_date<=%ld order by tx_date desc ,trace_no desc",sub_dd_mst.sub_ac_no,tx_date);
			if(ret==100)
			{
				fprintf(fp,"%d|%-50s|%.2lf|%.2lf|%.2lf|%s|\n",sub_dd_mst.sub_ac_seqn,sub_dd_mst.name,sub_dd_mst.bal,sub_dd_mst.intst_acm,sub_dd_mst.rate,ac_sts);
				sub_sum_amt+=sub_dd_mst.bal;
			}
			else
			{
				fprintf(fp,"%d|%-50s|%.2lf||%.2lf|%s|\n",sub_dd_mst.sub_ac_seqn,sub_dd_mst.name,sub_dd_mst_hst.bal,sub_dd_mst.rate,ac_sts);
				sub_sum_amt+=sub_dd_mst_hst.bal;
			}
		} else {
			fprintf(fp,"%d|%-50s|%.2lf|%.2lf|%.2lf|%s|\n",sub_dd_mst.sub_ac_seqn,sub_dd_mst.name,sub_dd_mst.bal,sub_dd_mst.intst_acm,sub_dd_mst.rate,ac_sts);
			sub_sum_amt+=sub_dd_mst.bal;
		}
		ttlnum++;
	}
	Sub_dd_mst_Clo_Sel();
	if(!ttlnum)
	{
		set_zd_data(DC_GET_MSG,"该账号不在本机构存在,请检查开户行!");
		strcpy( g_pub_tx.reply,"S089" ); 
		goto ErrExit;
	}else{
		fprintf(fp,"|总金额|%.2lf|\n", sub_sum_amt);
		fclose(fp);
		set_zd_data(DC_FILE_SND,"1");
	}
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

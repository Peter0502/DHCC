/**********************************************
*�������ƣ�gD110.c
*���ܣ�1,����Ч���ڴ���ϵͳ���ڵļ�¼��Ϊ��Ч
			 2,�޸������з������Ĵ���Ϊ��ȷֵ
*���ڣ�2008��12��26��
*���ߣ�wangzhe
*�޸ļ�¼��
*�޸��ˣ�
*�޸�ʱ�䣺
insert into com_eod_exec values('136','D110','gD110','ͬ��MBFE��������','1111111111','0','0','00','00','0','Y','Y','N','3','Y');
*********************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "hv_uniontab_c.h"

int gD110()
{
	struct hv_uniontab_c	sHv_uniontab_c;
	struct hv_uniontab_c	sHv_uniontab_c1;
	struct hv_uniontab_c	sHv_uniontab_c2;
	struct hv_uniontab_c	sHv_uniontab_c3;
	int ret=0;
	int ttlnum=0;
	char filename[100];
	char pcfilename[100];
	char today_date[9];
	long next_date = 0;
	FILE *fp;
	
	memset(&sHv_uniontab_c,0x00,sizeof(sHv_uniontab_c));
	memset(&sHv_uniontab_c1,0x00,sizeof(sHv_uniontab_c1));
	memset(&sHv_uniontab_c2,0x00,sizeof(sHv_uniontab_c2));
	memset(&sHv_uniontab_c2,0x00,sizeof(sHv_uniontab_c3));
	memset(filename, 0x00, sizeof(filename));
	memset(pcfilename, 0x00, sizeof(pcfilename));
	memset(today_date, 0x00, sizeof(today_date));
	
	pub_base_sysinit();
	/* 1,����Ч���ڴ���ϵͳ���ڵļ�¼�޸�Ϊ��Ч,��br_sts=1 */
	/*ret=sql_execute2("update hv_uniontab set br_sts='1' where br_sts='0' and start_date <='%ld' and end_date >'%ld'",g_pub_tx.tx_date,g_pub_tx.tx_date);*/
	/**����ȫ��֧��ϵͳ�кű�ʱ������ʱ��Ǽ��� hzh 20131119**/
	ret=sql_execute2("update hv_uniontab set br_sts='1',chg_date='%ld',upd_type='2' where br_sts='0' and start_date <='%ld' and end_date >'%ld'",g_pub_tx.tx_date,g_pub_tx.tx_date,g_pub_tx.tx_date);
	if(ret!=0 && ret!=100 ){
		sprintf(acErrMsg,"����ȫ��֧��ϵͳ�кű����!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	
	/**** 3,��ʧЧ����С��ϵͳ���ڵļ�¼��״̬�޸�Ϊ"ע��",��br_sts=2  add at 20131105 ****/
	/*ret=sql_execute2("update hv_uniontab set br_sts='2' where br_sts='1' and end_date <='%ld'",g_pub_tx.tx_date);*/
	/**����ȫ��֧��ϵͳ�кű�ʱ������ʱ��Ǽ��� hzh 20131119**/
	ret=sql_execute2("update hv_uniontab set br_sts='2',chg_date='%ld',upd_type='3' where br_sts='1' and end_date <='%ld'",g_pub_tx.tx_date,g_pub_tx.tx_date);
	if(ret!=0 && ret!=100 ){
		sprintf(acErrMsg,"����ȫ��֧��ϵͳ�кű����!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	/****end at 20131105****/
	
	/* 2,�޸������з������Ĵ���Ϊ��ȷֵ,��sendco������ȷ��ֵ */
	ret = distinct_dec_sel( "hv_uniontab", "qs_no", "sendco='NULL' and start_date <'%ld' and end_date >'%ld'", g_pub_tx.tx_date,g_pub_tx.tx_date );
	if(ret && ret!=1403){
		sprintf(acErrMsg,"����ȫ��֧��ϵͳ�кű����!");
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		char cQs_no[13];
		memset(cQs_no, 0 , sizeof(cQs_no));
		ret=distinct_fet_sel(cQs_no);
		if(ret == 1403)
		{
			break;
		}else if(ret)
		{
			sprintf(acErrMsg,"ȡ��ȫ��֧��ϵͳ�кű����,ret=%d",ret);
			WRITEMSG
			goto ErrExit;
		}
		memset(&sHv_uniontab_c1, 0 , sizeof(sHv_uniontab_c1));
		ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_c1,"or_br_no='%s'",cQs_no);
		if(ret == 100)
		{
			break;
		}
		else if(ret && ret != 100)
		{
			sprintf(acErrMsg,"ȡ��ȫ��֧��ϵͳ�кű����");
			WRITEMSG
			goto ErrExit;
		}
		ret=sql_execute2("update hv_uniontab set sendco='%s' where start_date <'%ld' and end_date >'%ld' and sendco='NULL' and qs_no='%s'",sHv_uniontab_c1.sendco,g_pub_tx.tx_date,g_pub_tx.tx_date,cQs_no);
		if(ret){
			sprintf(acErrMsg,"����ȫ��֧��ϵͳ�кű����!");
			WRITEMSG
			goto ErrExit;
		}
		
	}
	Hv_uniontab_Clo_Sel();
	/**������������к���Ϣ�ļ�¼д���ļ�,�ѱ������˸��������Ϣʱʹ�� hzh 20131119 begin**/
		sprintf( today_date,"%ld",g_pub_tx.tx_date);
		/**����Ҫ�����ɵ��ļ�����׺Ϊ����**/
			vtcp_log("[%s],[%d],aaaaaaaaaaaaaa[%ld]",__LINE__,__FILE__,today_date);
		next_date = pub_base_daynumLAT(today_date,1);
			vtcp_log("[%s],[%d],bbbbbbbbbbbbbbbbb[%ld]",__LINE__,__FILE__,next_date);
		sprintf( filename,"ebank_unionN_%ld.txt",next_date);
		sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
		fp = fopen(pcfilename, "w");
		if (fp == NULL) {
			sprintf(acErrMsg, " open file [%s] error ", pcfilename);
			WRITEMSG
				strcpy(g_pub_tx.reply, "S047");
			vtcp_log("[%s],[%d]",__LINE__,__FILE__);
			goto ErrExit;
		}
		vtcp_log("[%s],[%d],Ϊ���Ÿ��������к���Ϣ���ɵ�ȫ·���ļ���Ϊ[%s],�ļ���Ϊ[%s]", __FILE__, __LINE__, pcfilename,filename);
		ret = Hv_uniontab_Dec_Sel(g_pub_tx.reply, "chg_date='%ld'", g_pub_tx.tx_date);
			if(ret)
			{
				vtcp_log("[%s],[%d],[%d]",__LINE__,__FILE__,ret);
				goto ErrExit;
			}
		ttlnum = 0;
			while (1) 
			{
				ret = Hv_uniontab_Fet_Sel(&sHv_uniontab_c2, g_pub_tx.reply);
				if(ret == 100)
				{
					break;
				}
				else if(ret && ret != 100)
				{
					sprintf(acErrMsg,"ȡ��ȫ��֧��ϵͳ�кű����");
					WRITEMSG
					goto ErrExit;
				}
				fprintf(fp, "%s,,,%s,%s\n", sHv_uniontab_c2.or_br_no, sHv_uniontab_c2.br_name_f, sHv_uniontab_c2.upd_type);
				vtcp_log("%s,,,%s,%s\n", sHv_uniontab_c2.or_br_no, sHv_uniontab_c2.br_name_f, sHv_uniontab_c2.upd_type);
				ttlnum++;
			}
		fclose(fp);
		Hv_uniontab_Clo_Sel();
	/**������������к���Ϣ�ļ�¼д���ļ�,�ѱ������˸��������Ϣʱʹ�� hzh 20131119 end**/

OkExit:		 
       strcpy(g_pub_tx.reply, "0000");
       sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
       WRITEMSG
       sql_commit();
               set_zd_data(DC_REPLY, g_pub_tx.reply);
       return 0;
ErrExit:
       sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
       WRITEMSG
       sql_rollback();
       fclose(fp);
               set_zd_data(DC_REPLY, g_pub_tx.reply);
       return 1;
}

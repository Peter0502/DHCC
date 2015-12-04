/*****************************************************
* �� �� ��:  sp9402.c
* ���������� С���ִ�������Ϣ��ѯ
* ��    ��:  xyy
* ������ڣ� 2006-9-6 13:41 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
int sp9402()
{
	int ret=0,i;
	int iRecordNum = 0; /***��¼��***/
	char cKinbr[BRNO_LEN+1];
	char cDate[9];
	char cBuff[2001];
	char cOptype[2];
	char cFilename[1000];
	LV_FD123_AREA fd123;
	
	struct lv_pkgreg_c  lv_pkgreg;
	memset(&fd123, ' ',sizeof(fd123));
	memset(cKinbr,0,sizeof(cKinbr));
	memset(cDate,0,sizeof(cDate));
	memset(cBuff,0,sizeof(cBuff));
	memset(cFilename,0,sizeof(cFilename));
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(cOptype,0x00,sizeof(cOptype));
	
	vtcp_log("sp9402 ��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_fd_data("1230",(char*)&fd123);
	get_fd_data("0030",cKinbr);
	get_fd_data("0980",cOptype);
	vtcp_log("[%s,%d]1230=[%s]",__FILE__,__LINE__,(char*)&fd123);

	lv_pkgreg.wt_date = apatoi(fd123.wt_date,8);
	memcpy(lv_pkgreg.orderno,fd123.orderno,sizeof(lv_pkgreg.orderno)-1);
	memcpy(lv_pkgreg.or_br_no,fd123.or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
	memcpy(lv_pkgreg.pkgno,	  fd123.pkgno,	 sizeof(lv_pkgreg.pkgno)-1);

	vtcp_log("[%s][%d]lv_pkgreg.pkgno==[%.3s]\n",__FILE__,__LINE__,lv_pkgreg.pkgno);
	zip_space(lv_pkgreg.or_br_no);
	if(strlen(lv_pkgreg.or_br_no)==0)
	{
		memcpy(lv_pkgreg.or_br_no,"%%",sizeof(lv_pkgreg.or_br_no)-1);
	}
	/****ֻ�ܻ�ִ���е���ͨ���****/
	vtcp_log("[%s][%d] orderno>='%s' and or_br_no like '%s' and wt_date>='%ld' and br_no='%s' and pkgno='%s' \
	",__FILE__,__LINE__,lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date,cKinbr,lv_pkgreg.pkgno);


	
	g_reply_int =  Lv_pkgreg_Dec_Sel ( g_pub_tx.reply ,"orderno>='%s' and or_br_no like '%s' \
			and wt_date='%ld' and br_no='%s' and lw_ind='2' and pkgno='%s'  order by orderno",\ 
			lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date,cKinbr,lv_pkgreg.pkgno);
	vtcp_log("[%s][%d]g_reply_int=====[%d]",__FILE__,__LINE__,g_reply_int);
	if( g_reply_int){
		vtcp_log("[%s][%d]�����α� Lv_pkgreg_Dec_Sel OVER",__FILE__,__LINE__);
		sprintf(acErrMsg,"�����α����![%s][%d]",__FILE__,__LINE__);
		set_zd_data(DC_GET_MSG,"�����α����");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] ",__FILE__,__LINE__);
	while(1)
	{	
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
		vtcp_log("[%s][%d]g_reply_int===[%d]",__FILE__,__LINE__,g_reply_int);
		if(g_reply_int==100)
		{
			sprintf(acErrMsg,"û�пɻ�ִ��ҵ��![%s][%d]",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"û�пɻ�ִ��ҵ��");
			WRITEMSG
			goto ErrExit;
		}
		else if(g_reply_int)
		{
			sprintf(acErrMsg,"Lv_pkgreg_Fet_Sel�α����![%s][%d]",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"Lv_pkgreg_Fet_Sel�α����");
			WRITEMSG
			goto ErrExit;
		}
		else
		{
			zip_space(lv_pkgreg.o_packid);
			vtcp_log("[%s][%d]stat==[%c]\n",__FILE__,__LINE__,lv_pkgreg.lv_sts[0]);
			if((cOptype[0]=='1' &&(lv_pkgreg.lv_sts[0]==STAT_YES_NOTSEND||lv_pkgreg.lv_sts[0]==STAT_NO_NOTSEND))||(lv_pkgreg.lv_sts[0]==STAT_LZRECV1 && strlen(lv_pkgreg.o_packid)==0))
			{
				vtcp_log("[%s][%d]��ʼ׼���������!\n",__FILE__,__LINE__);
				/**PKGNO**/
				memcpy(fd123.pkgno	,lv_pkgreg.pkgno,sizeof(fd123.pkgno));
				/**�������ʺ�**/
				memcpy(fd123.pay_ac_no,lv_pkgreg.pay_ac_no,sizeof(fd123.pay_ac_no));
				vtcp_log("[%s][%d]lv_pkgreg.pay_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_ac_no);
				/**����������**/
				memcpy(fd123.pay_name,lv_pkgreg.pay_name,sizeof(fd123.pay_name));
				vtcp_log("[%s][%d]lv_pkgreg.pay_name=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_name);
				/**�����˵�ַ**/
				memcpy(fd123.pay_addr,lv_pkgreg.pay_addr,sizeof(fd123.pay_addr));
				/**�����˿�����**/
				memcpy(fd123.pay_opn_br_no,lv_pkgreg.pay_opn_br_no,\
					sizeof(fd123.pay_opn_br_no));
				/**�տ����ʺ�**/
				memcpy(fd123.cash_ac_no	,lv_pkgreg.cash_ac_no,\
					sizeof(fd123.cash_ac_no));
				vtcp_log("[%s][%d]lv_pkgreg.cash_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_ac_no);	
				/**�տ�������**/
				memcpy(fd123.cash_name,lv_pkgreg.cash_name,sizeof(fd123.cash_name));
				vtcp_log("[%s][%d]lv_pkgreg.cash_name=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_name);
				/**�տ��˵�ַ**/
				memcpy(fd123.cash_addr,lv_pkgreg.cash_addr,sizeof(fd123.cash_addr));
				/**�տ��˿�����**/
				memcpy(fd123.cash_opn_br_no,lv_pkgreg.cash_opn_br_no,\
					sizeof(fd123.cash_opn_br_no));
				/**�����к� **/
				memcpy(fd123.or_br_no,lv_pkgreg.or_br_no,sizeof(fd123.or_br_no));
				/**�����������к�**/		
				memcpy(fd123.pay_qs_no,lv_pkgreg.pay_qs_no,sizeof(fd123.pay_qs_no));
				/**���������к�**/				
				memcpy(fd123.cash_qs_no,lv_pkgreg.cash_qs_no,\
					sizeof(fd123.cash_qs_no));
				/**�����к�**/
				memcpy(fd123.ac_br_no	,lv_pkgreg.ac_br_no,sizeof(fd123.ac_br_no));
				vtcp_log("[%s][%d]lv_pkgreg.ac_br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.ac_br_no);
				vtcp_log("[%s][%d]fd123.ac_br_no=[%s]",__FILE__,__LINE__,fd123.ac_br_no);
				/**ί������**/
				apitoa(lv_pkgreg.wt_date,sizeof(cDate)-1,cDate);
				/**�������**/
				memcpy(fd123.orderno	,lv_pkgreg.orderno,sizeof(fd123.orderno));
				/**ҵ�����ͱ��**/
				memcpy(fd123.txnum	,lv_pkgreg.txnum,sizeof(fd123.txnum));
				/**ҵ������**/
				memcpy(fd123.yw_type	,lv_pkgreg.lv_yw_ind,sizeof(fd123.yw_type));
				/**Ӧ��״̬**/
				memcpy(fd123.rcp_sts	,lv_pkgreg.rcpstat,sizeof(fd123.rcp_sts));
				/**����**/
				memcpy(fd123.brf	,lv_pkgreg.lv_brf,sizeof(fd123.brf));	
				/**ԭ�������**/
				memcpy(fd123.o_orderno,lv_pkgreg.o_orderno,sizeof(fd123.o_orderno));
				/**ԭ���׽��**/		
				/****				
				dbl2str(&lv_pkgreg.tx_amt,0,sizeof(fd123.tx_amt),0,fd123.tx_amt);
				****/
				dot_dbl2str(lv_pkgreg.tx_amt*100,sizeof(fd123.tx_amt),0,fd123.tx_amt);
				
				vtcp_log("[%s][%d]lv_pkgreg.tx_amt=====[%f]",__FILE__,__LINE__,lv_pkgreg.tx_amt);
				vtcp_log("[%s][%d]fd123.tx_amt=====[%s]",__FILE__,__LINE__,fd123.tx_amt);
				/**�����**/
				memcpy(fd123.pack_id	,lv_pkgreg.packid,sizeof(fd123.pack_id));
				vtcp_log("lyly;ly[%s][%d]packid=[%s]\n",__FILE__,__LINE__,fd123.pack_id);
				/**ҵ�����ͱ��**/
				apitoa(lv_pkgreg.pack_date,sizeof(fd123.pack_date),fd123.pack_date);
				vtcp_log("lylyly[%s][%d]pack_date===[%s]\n",__FILE__,__LINE__,fd123.pack_date);
				/**֧������״̬**/		
				fd123.lv_sts[0] = lv_pkgreg.lv_sts[0];
				/** �෽Э��� **/
				memcpy(fd123.dfxyno,lv_pkgreg.protno,sizeof(fd123.dfxyno));
				/****�Ѿ�ȡ������������ֵ��***/
				vtcp_log("[%s][%d]lv_pkgreg.cash_ac_no=====[%s]",__FILE__,__LINE__,lv_pkgreg.cash_ac_no);

				iRecordNum++;

				break;
			}
			else
				continue;
		}
		vtcp_log("[%s][%d]lv_pkgreg.pay_qs_no=====[%s]",__FILE__,__LINE__,lv_pkgreg.pay_qs_no);
	}
	Lv_pkgreg_Clo_Sel( );
	vtcp_log("[%s][%d]lv_pkgreg.pay_qs_no=====[%s]",__FILE__,__LINE__,lv_pkgreg.pay_qs_no);
	if(memcmp(lv_pkgreg.txnum,TXNUM_QTJJ,5)!=0 && memcmp(lv_pkgreg.txnum,TXNUM_DQJJ,5)!=0)
	{
	  	FILE *fpw;
		vtcp_log("%s,%d �õ��ĸ�����=[%s]",__FILE__,__LINE__,cBuff);
		iHvGetAddit(cDate,cDate,lv_pkgreg.addid,"000",cBuff,sizeof(cBuff)-1);
		vtcp_log("%s,%d �õ��ĸ�����=[%s]",__FILE__,__LINE__,cBuff);
		ret=pub_base_AllDwnFilName(cFilename);
		if(ret)
		{
			vtcp_log("%s,%d �����´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
		}
		fpw=fopen(cFilename,"w");
		if(fpw==NULL)
		{
			vtcp_log("%s,%d ���´��ļ����� ret=%d ",__FILE__,__LINE__,ret);
		}
		if(memcmp(lv_pkgreg.txnum,TXNUM_GKJJ,sizeof(lv_pkgreg.txnum)-1)==0)
		{
			GKDJ_ADD_AREA lv_gkdj;
			memset(&lv_gkdj, 0 , sizeof(lv_gkdj));
			memcpy((char *)&lv_gkdj,cBuff, sizeof(lv_gkdj));
			vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FL__,(char *)&lv_gkdj);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_gkdj,sizeof(lv_gkdj));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		if(memcmp(lv_pkgreg.txnum,TXNUM_ZJZF,sizeof(lv_pkgreg.txnum)-1)==0 || memcmp(lv_pkgreg.txnum,TXNUM_SQZF,sizeof(lv_pkgreg.txnum)-1)==0 )
		{
			ZJZF_ADD_AREA lv_zjzf;
			memset(&lv_zjzf, 0 , sizeof(lv_zjzf));
			memcpy((char *)&lv_zjzf,cBuff, sizeof(lv_zjzf));
			vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FL__,(char *)&lv_zjzf);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_zjzf,sizeof(lv_zjzf));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		if(memcmp(lv_pkgreg.txnum,TXNUM_GZJJ,sizeof(lv_pkgreg.txnum)-1)==0)
		{
			GZDJ_ADD_AREA lv_gzdj;
			memset(&lv_gzdj, 0 , sizeof(lv_gzdj));
			memcpy((char *)&lv_gzdj,cBuff, sizeof(lv_gzdj));
			vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FL__,(char *)&lv_gzdj);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_gzdj,sizeof(lv_gzdj));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		if(memcmp(lv_pkgreg.txnum,TXNUM_PLKS,sizeof(lv_pkgreg.txnum)-1)==0)
		{
			SSKS_ADD_AREA lv_ssks;
			memset(&lv_ssks, 0 , sizeof(lv_ssks));
			memcpy((char *)&lv_ssks,cBuff, sizeof(lv_ssks));
			vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FL__,(char *)&lv_ssks);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_ssks,sizeof(lv_ssks));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		if(memcmp(lv_pkgreg.txnum,TXNUM_ZPJL,sizeof(lv_pkgreg.txnum)-1)==0)
		{
			ZPJL_ADD_AREA lv_zpjl;
			memset(&lv_zpjl, 0 , sizeof(lv_zpjl));
			memcpy((char *)&lv_zpjl,cBuff, sizeof(lv_zpjl));
			vtcp_log("%s,%d д�븽�����ļ�=[%s]",__FL__,(char *)&lv_zpjl);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&lv_zpjl,sizeof(lv_zpjl));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
	}
	vtcp_log("[%s][%d]�������ȡ���!\n",__FILE__,__LINE__);



	
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

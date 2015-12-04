/*****************************************************
* �� �� ��:  spJ422.c
* ���������� ֧ƱӰ��ҵ��¼��
* ��    ��:  wangyongwei 
* ������ڣ� 2007-03-13 9:24 
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
#include "lv_addit_c.h"
#include "seqctl_c.h"
#include "hv_brno_orno_c.h"
#include "hv_define.h"
static PAY_IN_AREA payin;
int spJ422()
{
	int iRc=0;
	FILE *fpr;
	struct lv_pkgreg_c lv_pkgreg;
	LV_FD123_AREA  fd123;
	TYJL6_ADD_AREA ty_add6_area;
	TYJL1_ADD_AREA ty_add1_area;
	TYJL4_ADD_AREA ty_add4_area;
	char cFilename[100];
	memset(cFilename,0,sizeof(cFilename));
	memset((char*)&fd123,0,sizeof(fd123));
	memset(&ty_add6_area,0,sizeof(ty_add6_area));
	memset(&ty_add1_area,0,sizeof(ty_add1_area));
	memset(&ty_add4_area,0,sizeof(ty_add4_area));
	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	vtcp_log("[%s][%d] ����֧ƱӰ��ҵ��¼�뽻��!spJ422.c\n",__FILE__,__LINE__);

	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}
	/***ȡ¼��ҵ��123�����Ϣ***/
	get_fd_data("1230",(char*)&fd123);

	/****���payin****/
	g_reply_int = iLvTisToPayin(&payin);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iHvTistoNotpay����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	if(memcmp(payin.F_txnum,TXNUM_TYJL,sizeof(payin.F_txnum))==0){
		pub_base_AllDwnFilName(cFilename);	
		vtcp_log("[%s][%d],�������ļ���cFilename[%s]\n",__FL__,cFilename);
		fpr=fopen(cFilename,"rb");
		if(fpr==NULL){
			vtcp_log("[%s][%d],open add_area file[%s] failed!\n",__FL__,cFilename);	
			sprintf(acErrMsg,"open add_area file[%s] failed!\n",cFilename);
			WRITEMSG
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
		fgets((char*)&ty_add6_area,sizeof(ty_add6_area),fpr);
		vtcp_log("[%s][%d],ty_add6_area[%s]\n",__FL__,&ty_add6_area);	
		fclose(fpr);
		memcpy((char*)&ty_add1_area,(char*)&ty_add6_area,sizeof(ty_add1_area));
		memcpy((char*)&ty_add4_area,(char*)&ty_add6_area+sizeof(ty_add1_area),sizeof(ty_add4_area));
		vtcp_log("[%s][%d]*******************addid[%.8s]\n",__FL__,payin.T_addid);
		g_reply_int=nCifSetAddit(payin.F_wtday,payin.T_addid,"001",(char*)&ty_add1_area,sizeof(ty_add1_area));
		if(g_reply_int){
			vtcp_log("[%s][%d]����ͨ�ø�����1ʧ��!\n",__FL__);
			sprintf(acErrMsg,"����ͨ�ø�����1ʧ��![%d]",g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
		g_reply_int=nCifSetAddit(payin.F_wtday,payin.T_addid,"004",(char*)&ty_add4_area,sizeof(ty_add4_area));	
		if(g_reply_int){
			vtcp_log("[%s][%d],����ͨ�ø�����ʧ��!",__FL__);
			sprintf(acErrMsg,"����ͨ�ø�����ʧ��[%d]\n",g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
	}		
	vtcp_log("[%s][%d],*******************addid[%.8s]\n",__FL__,payin.T_addid);
		
	vtcp_log("[%s][%d] payin.F_feeflag[0]==[%c]",__FILE__,__LINE__,payin.F_feeflag[0]);
	/****�������****/
	g_reply_int = iLvPayinCheck0(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����������,[%s][%d]", __FILE__,__LINE__);
		if(memcmp(g_pub_tx.reply,"0000",4) ==0 || memcmp(g_pub_tx.reply,"",4)==0 )
		{
			strcpy( g_pub_tx.reply, "HV01" );
		}
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] payin.F_feeflag[0]==[%c]",__FILE__,__LINE__,payin.F_feeflag[0]);
	/****Ҫ�ؼ��****/
	g_reply_int = iLvPayinCheck4(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"Ҫ�ؼ�����,[%s][%d]", __FILE__,__LINE__);
		if(memcmp(g_pub_tx.reply,"0000",4) ==0 || memcmp(g_pub_tx.reply,"",4)==0 )
		{
			strcpy( g_pub_tx.reply, "HV01" );
		}
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] payin.F_feeflag[0]==[%c]",__FILE__,__LINE__,payin.F_feeflag[0]);
	/****�кż��****/
	g_reply_int = iLvPayinCheck1(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�кż�����,[%s][%d]", __FILE__,__LINE__);
		if(memcmp(g_pub_tx.reply,"0000",4) ==0 || memcmp(g_pub_tx.reply,"",4)==0 )
		{
			strcpy( g_pub_tx.reply, "HV01" );
		}
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] payin.F_feeflag[0]==[%c]",__FILE__,__LINE__,payin.F_feeflag[0]);
		/* �����Ѽ�� */
	/**if(payin.F_feeflag[0]!='4')
	g_reply_int = iLvCountFeeamt(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,���������Ѵ���,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"���������Ѵ���,[%s][%d]", __FILE__,__LINE__);
		if(memcmp(g_pub_tx.reply,"0000",4) ==0 || memcmp(g_pub_tx.reply,"",4)==0 )
		{
			strcpy( g_pub_tx.reply, "HV01" );
		}
		WRITEMSG
		goto ErrExit;
	}****/

	/****�տ��ʺż��****/
	g_reply_int=iLvPayinCheck3(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�տ��ʺż�����,[%s][%d]", __FILE__,__LINE__);
		if(memcmp(g_pub_tx.reply,"0000",4) ==0 || memcmp(g_pub_tx.reply,"",4)==0 )
		{
			strcpy( g_pub_tx.reply, "HV01" );
		}
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] payactno[%.32s]\n",__FILE__,__LINE__,payin.F_payactno);
	vtcp_log("[%s][%d] cashactno[%.32s]\n",__FILE__,__LINE__,payin.F_cashactno);
	int iOrderno;
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0){
		sprintf( acErrMsg,"�õ�֧��������ų���,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;

	}
	apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
	/***����������������һ���µļ�¼****/
	g_reply_int = iLvNewRecFromPayin(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����һ���µļ�¼����,[%s][%d]", __FILE__,__LINE__);
		if(memcmp(g_pub_tx.reply,"0000",4) ==0 || memcmp(g_pub_tx.reply,"",4)==0 )
		{
			strcpy( g_pub_tx.reply, "HV01" );
		}
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]===========wd_payin->F_txamt=[%.15s]\n",__FL__,payin.F_txamt);
	vtcp_log("[%s][%d]===========wd_payin->T_txamt=[%.15s]\n",__FL__,payin.T_txamt);
	/***�����¼�¼****/
	g_reply_int = iLvUpdRecFromPayin(&payin,1);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�����¼�¼,[%s][%d]", __FILE__,__LINE__);
		if(memcmp(g_pub_tx.reply,"0000",4) ==0 || memcmp(g_pub_tx.reply,"",4)==0 )
		{
			strcpy( g_pub_tx.reply, "HV01" );
		}
		WRITEMSG
		goto ErrExit;
	}
	/**�����ѵ���ȡ�ǿ����ñ�����ȡ�����ٽ����������������***/	
	
        /* ����ˮ��־ */
        g_reply_int = pub_ins_trace_log();
        if ( g_reply_int )
        {
                sprintf( acErrMsg, "����ˮ��־��" );
                strncpy(g_pub_tx.reply,"S023",4);
                WRITEMSG
                goto ErrExit;
        }
        /*****д������ϸ�� ******/
	iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
ilv10_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
/********************���ø�����**********************/         
int nCifSetAddit(char *txday,char *saddid,char *name,char *buf,int length){
	struct lv_addit_c wd_lv_addit;
	char cTxday[9];
	char addid[9];
	memset(addid,0,sizeof(addid));
	memset(cTxday, 0 , sizeof(cTxday));
	memcpy(addid,saddid,sizeof(addid)-1);
	memcpy(cTxday,txday,sizeof(cTxday)-1);
	zip_space(cTxday);
	memset(&wd_lv_addit,'\0',sizeof(wd_lv_addit));
	if(addid[0]=='\0'){
		int nAddid=nCifGetAddid();
		if(nAddid<=0){
			return(-1);
		}
		sprintf(wd_lv_addit.addid,"%08d",nAddid);
		memcpy(wd_lv_addit.tagname,name, sizeof(wd_lv_addit.tagname)-1);
		memcpy(wd_lv_addit.tagdata,buf,length);
		g_reply_int=Lv_addit_Ins(wd_lv_addit,g_pub_tx.reply);
		if(g_reply_int){
			vtcp_log("[%s][%d], insert into lv_addit error=%d\n",__FL__,g_reply_int);
			return(-1);
		}
	}else{
		memcpy(wd_lv_addit.addid,addid,sizeof(wd_lv_addit.addid)-1);
		wd_lv_addit.tx_date = atoi(cTxday);
		memcpy(wd_lv_addit.tagname,name, sizeof(wd_lv_addit.tagname)-1);
		vtcp_log("[%s][%d], find addid[%s] \n",__FL__,wd_lv_addit.addid);
		g_reply_int=Lv_addit_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and tagname='%s' and addid='%s'",wd_lv_addit.tx_date,wd_lv_addit.tagname,wd_lv_addit.addid);
		if(g_reply_int){
			vtcp_log("[%s][%d], select lv_addit error[%d]\n",__FL__,g_reply_int);
			return(-1);
		}
		g_reply_int=Lv_addit_Fet_Upd(&wd_lv_addit,g_pub_tx.reply);
		if(g_reply_int){
			if(g_reply_int==100)
			{
				vtcp_log("[%s][%d], insert into buf=[%s]\n",__FL__,buf);
				vtcp_log("[%s][%d], insert into tagname=[%s],addid[%s]\n",__FL__,wd_lv_addit.tagname,wd_lv_addit.addid);
				memcpy(wd_lv_addit.tagdata,buf,length);
				g_reply_int=Lv_addit_Ins(wd_lv_addit,g_pub_tx.reply);
				if(g_reply_int){
					vtcp_log("[%s][%d], insert into lv_addit error=%d\n",__FL__,g_reply_int);
					return(-1);
				}
				return 0;
			}
			else
			{
				vtcp_log("[%s][%d], insert into lv_addit error=%d\n",__FL__,g_reply_int);
				return(-1);
			}
		}
		vtcp_log("[%s][%d], insert into wd.tagdata=[%s]\n",__FL__,wd_lv_addit.tagdata);
		memcpy(wd_lv_addit.tagdata,buf,length);
		g_reply_int=Lv_addit_Upd_Upd(wd_lv_addit,g_pub_tx.reply);
		if(g_reply_int){
			vtcp_log("[%s][%d], update lv_addit error[%d]\n",__FL__,g_reply_int);
		}
		Lv_addit_Clo_Upd();
	}
	memcpy(saddid,wd_lv_addit.addid,sizeof(wd_lv_addit.addid)-1);
	return(0);
}
/***���ɸ��������****/
int nCifGetAddid(){
	char   Seqno[7];
	struct seqctl_c wd_seqctl;
	memset(Seqno,'\0',sizeof(Seqno));
	memset(&wd_seqctl,0,sizeof(wd_seqctl));
	/**memset(&wd_brno_orno,0,sizeof(wd_brno_orno));
	struct hv_brno_orno_c wd_brno_orno;
	memcpy(wd_brno_orno.br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.tx_br_no));
	g_reply_int=Hv_brno_orno_Sel(g_pub_tx.reply,&wd_brno_orno,"br_no='%s'",wd_brno_orno.br_no);***/
	memcpy(wd_seqctl.br_no,HV_BR_NO,5);
	memcpy(wd_seqctl.ac_ind,LV_ADDID_IND,sizeof(wd_seqctl.ac_ind)-1);
	vtcp_log("[%s][%d],���������Ϊ[%s]!\n",__FL__,wd_seqctl.br_no);
	vtcp_log("[%s][%d],�����������Ϊ[%s]!\n",__FL__,wd_seqctl.ac_ind);
	if((g_reply_int=Seqctl_Dec_Upd(g_pub_tx.reply,"br_no='%s' and ac_ind='%s'",wd_seqctl.br_no,wd_seqctl.ac_ind ))!=0){
		vtcp_log("[%s][%d],������ſ��Ʊ����![%d]!\n",__FL__,g_reply_int);
		return -1;
	}
	vtcp_log("[%s][%d],�����������Ϊ[%s]!\n",__FL__,wd_seqctl.ac_ind);
	if((g_reply_int=Seqctl_Fet_Upd((char*)&wd_seqctl, g_pub_tx.reply))!=0) {
		vtcp_log("[%s][%d],������ſ��Ʊ����[%d]!\n",__FL__,g_reply_int);
		return -1;
	} 
	vtcp_log("[%s][%d],����[%s]��ǰ�����Ϊ:[%f]!\n",__FL__,wd_seqctl.br_no,wd_seqctl.seqno);
	/**sprintf(Seqno,"%08.0f",wd_seqctl.seqno++);**/
	wd_seqctl.seqno++;
	if((g_reply_int=Seqctl_Upd_Upd(wd_seqctl, g_pub_tx.reply))!=0) {
		vtcp_log("[%s][%d],������ſ��Ʊ����[%d]!\n",__FL__,g_reply_int);
		return -1;
	}	
	Seqctl_Clo_Upd();

	vtcp_log("[%s][%d],����[%s]�����ɵ����Ϊ:[%f],wd_seqctl.ac_ind[%s]!\n",__FL__,wd_seqctl.br_no,wd_seqctl.seqno,wd_seqctl.ac_ind);
	vtcp_log("[%s][%d],����[%s]���ص����Ϊ:[%f]!\n",__FL__,wd_seqctl.br_no,wd_seqctl.seqno);
	return (wd_seqctl.seqno);
}

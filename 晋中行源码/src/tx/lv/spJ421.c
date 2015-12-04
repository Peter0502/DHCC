/***************************************************************************/
/* �ļ�����: dh652030.pc                                                   */
/* ��������: ֧Ʊ/ͨ�ý���ҵ�������嵥ά��                               */
/* ����    : ʯ����,2006.10.15						   */
/***************************************************************************/
#define EXTERN
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "cifpack.h"
#include "cifserver.h"
#include "public.h"
#include "lv_pub.h"
#include "lvpack.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"

static struct  lv_pkgreg_c	lv_pkgreg;
static PAY_IN_AREA payin;

static ZPJL2_ADD_AREA wd_zp_add2;
static TYJL2_ADD_AREA wd_ty_add2;

static ZPJL1_ADD_AREA wd_zp_add1;
static ZPJL3_ADD_AREA wd_zp_add3;
static TYJL1_ADD_AREA wd_ty_add1;
static TYJL3_ADD_AREA wd_ty_add3;
static TYJL4_ADD_AREA wd_ty_add4;
static TYJL5_ADD_AREA wd_ty_add5;
	
struct zpjl1_add_area {
        char cpday[9];          /**��Ʊ����****/
        char cpno[13];          /**��Ʊ����****/
        char payno[13];         /**�����к�****/
        char cpactno[33];       /**��Ʊ���ʺ�**/
        char cashactno[33];     /**�տ����ʺ�**/
        char cpamt[13];         /**���***/
        char use[61];           /**��;***/
        char tsfkday[9];        /**��ʾ��������**/
}wc_zp_add1;
struct zpjl2_add_area {
        char bscnt[3];          /**��������,�̶�Ϊ0 ****/
        char bslist[11][60];    /**�������б�**/
}wc_zp_add2;

struct zpjl3_add_area {
        char bscheckpswd[13];/**У������ ****/
        char bsbitmaplen[9];/**ͼ�����ݳ���,Ϊ0 ****/
        char bsbitmaplen2[9];/**ͼ�����ݳ���,Ϊ0 ****/
}wc_zp_add3;

struct tyjl1_add_area{
        char cpday[9];          /**��Ʊ����****/
        char cpno[21];          /**��Ʊ����****/
        char payno[13];         /**�����к�****/
        char dlpayno[13];       /**�������к�****/
        char cpamt[16];         /**��Ʊ���****/
        char use[61];           /**��;***/
}wc_ty_add1;
struct tyjl3_add_area{
        char bscheckpswd[13];/**У������ ****/
}wc_ty_add3;
struct tyjl4_add_area{
        char ptype[3];          /**Ʊ������**/
        char tsday[9];          /**��ʾ��������**/
        char mac[21];           /**��Ѻ**/
        char stopday[9];        /**������**/
        char xyno[21];          /**�ж�Э����**/
        char htno[21];          /**���׺�ͬ����**/
        char cdday[9];          /**�ж�����**/
        char cdname[61];        /**�ж���**/
        char reqname[61];       /**������**/
        char reqactno[33];      /**�������ʺ�**/
        char paybrname[61];     /**�����˿�������**/
        char cashbrname[61];    /**�տ��˿�������**/
        char cpname[61];        /**��Ʊ��ȫ��    **/
        char cpactno[33];       /**��Ʊ���ʺ�    **/
        char paybrname2[61];    /**����������    **/
        char content[61];       /**����**/
}wc_ty_add4;
struct tyjl5_add_area{
        char bsbitmaplen[9];/**ͼ�����ݳ���,Ϊ0 ****/
        char bsbitmaplen2[9];/**ͼ�����ݳ���,Ϊ0 ****/
}wc_ty_add5;


int spJ421()
{
	int iRc=0, cnt=0, sIndex=0,i=0;
	char	cOptype[2];
	char	cSendyn[2];	/* �Ƿ��͸���ӡ�������ı�־1,��;0,���� **/
	char	Name[751];
	char	cTmp[61];
    char 	qsx[2];
    char cFilename[100];
	FILE *fpr;
Gl_file_first=0;
    memset(cFilename,0,sizeof(cFilename));


	memset(cOptype,'\0',sizeof(cOptype));
	memset(cSendyn,'\0',sizeof(cSendyn));
	memset(Name,'\0',sizeof(Name));
	memset(cTmp,'\0',sizeof(cTmp));
    memset(qsx,0,2);

	memset(&lv_pkgreg,'\0',sizeof(lv_pkgreg));
	memset(&payin,'\0',sizeof(payin));
	memset(&payin,'\0',sizeof(payin));
	memset(&wd_zp_add2,'\0',sizeof(wd_zp_add2));
	memset(&wd_ty_add2,'\0',sizeof(wd_ty_add2));

    memset(&wc_zp_add1,'\0',sizeof(wd_zp_add1));
    memset(&wc_zp_add2,'\0',sizeof(wc_zp_add2));
    memset(&wc_zp_add3,'\0',sizeof(wc_zp_add3));
    memset(&wc_ty_add1,'\0',sizeof(wc_ty_add1));
    memset(&wc_ty_add3,'\0',sizeof(wc_ty_add3));
    memset(&wc_ty_add4,'\0',sizeof(wc_ty_add4));
    memset(&wc_ty_add5,'\0',sizeof(wc_ty_add5));

    memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
    memset(&wd_zp_add3,'\0',sizeof(wd_zp_add3));
    memset(&wd_ty_add1,'\0',sizeof(wd_ty_add1));
    memset(&wd_ty_add3,'\0',sizeof(wd_ty_add3));
    memset(&wd_ty_add4,'\0',sizeof(wd_ty_add4));
    memset(&wd_ty_add5,'\0',sizeof(wd_ty_add5));


    vtcp_log("[%s][%d] spJ421��ʼ�\n",__FILE__,__LINE__);
    vtcp_log("[%s][%d] =========g_pub_tx.file_snd=[%s]\n",__FILE__,__LINE__,g_pub_tx.file_snd);

    pub_base_sysinit();

	get_zd_data("0670",cOptype);
	vtcp_log("[%s][%d]===========cOptype=[%s].0��ѯ1�޸�\n",__FILE__,__LINE__,cOptype);
	get_zd_data("0680",cSendyn);
	vtcp_log("[%s][%d]============�Ƿ��͵���ӡ��������־[%s]\n",__FILE__,__LINE__,cSendyn);
	get_zd_data("0310",payin.F_wtday);
	get_zd_data("0320",payin.F_orderno);
/** ����95���ˣ�ͨ�ý���������̫���ˣ�95�򲻹���
	get_fd_data("0950",Name);
**/
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
        fgets(Name,sizeof(Name),fpr);
        fclose(fpr);

	vtcp_log("[%s][%d]============Name-[%s]\n",__FILE__,__LINE__,Name);
	memcpy(payin.F_pkgno,PKGNO_PTJJ,sizeof(payin.F_pkgno));
	memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));
	vtcp_log("[%s][%d]============payin.F_lw_ind[0]-[%c]\n",__FILE__,__LINE__,payin.F_lw_ind[0]);
	iLvPayinAutoInfo(&payin);
	vtcp_log("[%s][%d]============payin.F_lw_ind[0]-[%c]\n",__FILE__,__LINE__,payin.F_lw_ind[0]);

	vtcp_log("[%s][%d]payin.F_wtday=[%s] payin.F_payactno[%s]\n",__FILE__,__LINE__,payin.F_wtday,payin.F_payactno);
	vtcp_log("[%s][%d]===========cOptype=[%s].0��ѯ1�޸�\n",__FILE__,__LINE__,cOptype);
	payin.F_lw_ind[0]='1';
	/****ȡ����������****/
	if(iLvFetchRecToPayin(&payin)){
		memcpy(payin.F_pkgno,PKGNO_SSJJ,sizeof(payin.F_pkgno));
		if(iLvFetchRecToPayin(&payin)){
			vtcp_log("[%s][%d]iLvFetchRecToPayin����\n",__FILE__,__LINE__);
                	WRITEMSG
        			strcpy( g_pub_tx.reply, "P017" );
                	goto ErrExit;
		}
	}
	iLvResetPayin(&payin);

	if(payin.T_stat[0]!=STAT_WZLR){
		vtcp_log("[%s][%d]ֻ�д���¼��״̬�Ľ��ײ�����ά��������\n",__FILE__,__LINE__);
        		strcpy( g_pub_tx.reply, "P016" );
                WRITEMSG
                goto ErrExit;
	}
	if(cOptype[0]=='0'){
		/**��ѯ**/
		if(payin.T_addid[0]=='\0'||payin.T_addid[0]==' '){
			memset(wd_zp_add2.bscnt,'0',2);
			/***û�б�����****/	
			vtcp_log("[%s][%d]û�б�����\n",__FILE__,__LINE__);
        	strcpy( g_pub_tx.reply, "P012" );
            WRITEMSG
			goto OkExit;
		}
		iRc=iLvGetAddit(&payin,"002",&wd_zp_add2,0);
		if(*((char *)&wd_zp_add2)=='\0'){
			memset(wd_zp_add2.bscnt,'0',2);
			vtcp_log("[%s][%d]û�б�����\n",__FILE__,__LINE__);
        	strcpy( g_pub_tx.reply, "P012" );
            WRITEMSG
			goto OkExit;
		}			
		set_zd_data("0950",wd_zp_add2.bslist[0]);
		set_zd_data("0210",wd_zp_add2.bscnt);
		/***û�б�����****/	
		goto OkExit;
	}
	if(cSendyn[0] == '0')	/** �����Ƿ���ʱ **/
	{
		vtcp_log("[%s][%d]============Name-[%s]\n",__FILE__,__LINE__,Name);
		memcpy(&wd_zp_add2.bslist,Name,sizeof(wd_zp_add2));
		for(sIndex=0;sIndex<10;sIndex++){
			vtcp_log("[%s][%d]-------------bslist=[%s]\n",__FILE__,__LINE__,wd_zp_add2.bslist[sIndex]);
			vtcp_log("[%s][%d]---------strlen=[%d]\n",__FILE__,__LINE__,strlen(wd_zp_add2.bslist[sIndex]));
			vtcp_log("[%s][%d]---------zip_space=[%s]\n",__FILE__,__LINE__,wd_zp_add2.bslist[sIndex]);
/** ������ʹƴ��ʱ���Ŀո�ȥ��
			if(wd_zp_add2.bslist[sIndex][0]==' '||wd_zp_add2.bslist[sIndex][0]=='\0'){
**/
			memset(cTmp,'\0', sizeof(cTmp));
			memcpy(cTmp,wd_zp_add2.bslist[sIndex],sizeof(cTmp)-1);
			zip_space(cTmp);
			if(strlen(cTmp)==0){
				break;
			}else{
				vtcp_log("[%s][%d]---------\n",__FILE__,__LINE__);
/***  ȥ���ո�,��Ϊ�����
				pad_field(wd_zp_add2.bslist[sIndex],sizeof(wd_zp_add2.bslist[0]));
***/
				cnt++;
			}
/** �������ټ�һ
			cnt++;
**/
		}
			vtcp_log("[%s][%d]---------cnt=[%d]\n",__FILE__,__LINE__,cnt);
		vtcp_log("[%s][%d]---------wd_zp_add2=[%s]\n",__FILE__,__LINE__,&wd_zp_add2);
		apitoa(cnt,sizeof(wd_zp_add2.bscnt),wd_zp_add2.bscnt);
		vtcp_log("[%s][%d]---------wd_zp_add2.bscnt=[%s]\n",__FILE__,__LINE__,wd_zp_add2.bscnt);
		iRc=iLvSetAddit(&payin,"002",&wd_zp_add2,cnt*sizeof(wd_zp_add2.bslist[0])+2);
		if(iRc){
			vtcp_log("%s,%d,���ø�����ʧ��\n",__FILE__,__LINE__);
       		strcpy( g_pub_tx.reply, "P012" );
       	 	WRITEMSG
       	 	goto ErrExit;
		}
	}
	else
	{
        if(memcmp(payin.F_txnum,TXNUM_ZPJL,sizeof(payin.F_txnum))==0)
		{
			vtcp_log("[%s][%d]--------��֧Ʊ����\n",__FILE__,__LINE__);
              memcpy(&wd_zp_add1,Name,sizeof(wd_zp_add1));
/***
              pad_zpjl1(&wd_zp_add1);
***/
              iRc=iLvSetAddit(&payin,"001",&wd_zp_add1,sizeof(wd_zp_add1));
              if(iRc){
                	vtcp_log("%s,%d,���ø�����ʧ��\n",__FILE__,__LINE__);
              		sprintf(acErrMsg,"���ø�����ʧ��");
              		strcpy( g_pub_tx.reply,"P012" );
              		WRITEMSG
              		goto ErrExit;
             }
        	memset(&wd_zp_add3,'0',sizeof(wd_zp_add3));
        	memset(wd_zp_add3.bscheckpswd,' ',sizeof(wd_zp_add3.bscheckpswd));
/*** ��ʱ��ӣ�Ϊ�˲��Ժ�С������������� dongxy begin *****/
            memset(wd_zp_add3.bscheckpswd,'x',sizeof(wd_ty_add3.bscheckpswd));
			memcpy(wd_zp_add3.bsbitmaplen,"00000000",8);
			memcpy(wd_zp_add3.bsbitmaplen2,"00000000",8);
/*** ��ʱ��ӣ�Ϊ�˲��Ժ�С������������� dongxy end *****/
        	iRc=iLvSetAddit(&payin,"003",&wd_zp_add3,sizeof(wd_zp_add3));
        	if(iRc){
                	vtcp_log("%s,%d,���ø�����ʧ��\n",__FILE__,__LINE__);
              		sprintf(acErrMsg,"���ø�����ʧ��");
              		strcpy( g_pub_tx.reply,"P012" );
              		WRITEMSG
              		goto ErrExit;
        	}
        }
		else if(memcmp(payin.F_txnum,TXNUM_TYJL,sizeof(payin.F_txnum))==0){
				vtcp_log("[%s][%d]--------��ͨ�ý���\n",__FILE__,__LINE__);
				vtcp_log("[%s][%d]--------name=[%s]\n",__FILE__,__LINE__,Name);
				memcpy(&wd_ty_add1,Name,sizeof(wd_ty_add1));
                memcpy(&wd_ty_add4,Name+sizeof(wd_ty_add1),sizeof(wd_ty_add4));

/****
                pad_tyjl1(&wd_ty_add1);
                pad_tyjl4(&wd_ty_add4);
****/
                iRc=iLvSetAddit(&payin,"001",&wd_ty_add1,sizeof(wd_ty_add1));
                if(iRc){
                      	vtcp_log("%s,%d,���ø�����ʧ��\n",__FILE__,__LINE__);
                   		sprintf(acErrMsg,"���ø�����ʧ��");
                   		strcpy( g_pub_tx.reply,"P012" );
                   		WRITEMSG
                  		goto ErrExit;
                }
                iRc=iLvSetAddit(&payin,"004",&wd_ty_add4,sizeof(wd_ty_add4));
                if(iRc){
                      	vtcp_log("%s,%d,���ø�����ʧ��\n",__FILE__,__LINE__);
                   		sprintf(acErrMsg,"���ø�����ʧ��");
                   		strcpy( g_pub_tx.reply,"P012" );
                   		WRITEMSG
                  		goto ErrExit;
                }
                memset(&wd_ty_add3,'0',sizeof(wd_ty_add3));
                memset(wd_ty_add3.bscheckpswd,'x',sizeof(wd_ty_add3.bscheckpswd));
                iRc=iLvSetAddit(&payin,"003",&wd_ty_add3,sizeof(wd_ty_add3));
                if(iRc){
                      	vtcp_log("%s,%d,���ø�����ʧ��\n",__FILE__,__LINE__);
                   		sprintf(acErrMsg,"���ø�����ʧ��");
                   		strcpy( g_pub_tx.reply,"P012" );
                   		WRITEMSG
                  		goto ErrExit;
                }
                memset(&wd_ty_add5,'0',sizeof(wd_ty_add5));
                iRc=iLvSetAddit(&payin,"005",&wd_ty_add5,sizeof(wd_ty_add5));
                if(iRc){
                      	vtcp_log("%s,%d,���ø�����ʧ��\n",__FILE__,__LINE__);
                   		sprintf(acErrMsg,"���ø�����ʧ��");
                   		strcpy( g_pub_tx.reply,"P012" );
                   		WRITEMSG
                  		goto ErrExit;
                }
		}

	}
	iRc=iLvUpdRecFromPayin(&payin,0);
	if(iRc){
		vtcp_log("%s,%d,���ø�����ʧ��\n",__FILE__,__LINE__);
   		sprintf(acErrMsg,"���ø�����ʧ��");
        strcpy( g_pub_tx.reply, "P012" );
        WRITEMSG
        goto ErrExit;

	}

	if(cSendyn[0] == '1')	/** ���Ƿ���ʱ **/
	{
        iRc = iLvGetAddit(&payin,"QSX",qsx,1);
        if(iRc){
				vtcp_log("%s,%d,ȡ���ͱ�־����\n",__FILE__,__LINE__);
		   		sprintf(acErrMsg,"ȡ���ͱ�־����");
		        strcpy( g_pub_tx.reply, "P076" );
		        WRITEMSG
		        goto ErrExit;
        }
        if(qsx[0] == '1') {
				vtcp_log("%s,%d,CIF�����Ѿ�����,������·���\n",__FILE__,__LINE__);
		   		sprintf(acErrMsg,"CIF�����Ѿ�����,������·���");
		        strcpy( g_pub_tx.reply, "P012" );
		        WRITEMSG
		        goto ErrExit;
        }

		if(lv652020_SendCif())
		{
				vtcp_log("%s,%d,CIF�����Ѿ�����,������·���\n",__FILE__,__LINE__);
		   		sprintf(acErrMsg,"CIF�����Ѿ�����,������·���");
		        strcpy( g_pub_tx.reply, "P012" );
		        WRITEMSG
		        goto ErrExit;
		}
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
lv652020_SendCif()
{  
/****************** ע�����Ȳ���ʱ�ſ�������S027
   	TCIFPACK packcif;
    TCIFPACK *pack=&packcif; ***���Ľṹ****
    int  cifport=0;
	int	 iRc=0;
	int	 iSocketID=0;
	int  iPackLen=0;
    int  tmp_orderno=0;
    char pcFileName[256];
    char cifip[32];
    char tmpbuf[19];
    char tmp_port[32];
    char tlrno[17];
	char status[4];

    memset(pcFileName,'\0',sizeof(pcFileName));
    memset(cifip,'\0',sizeof(cifip));
    memset(tmpbuf,'0',sizeof(tmpbuf));
    memset(tmp_port,'0',sizeof(tmp_port));
    memset(tlrno,'\0',sizeof(tlrno));
	memset(status,'\0',sizeof(status));

    memcpy(wc_zp_add1.cpday,wd_zp_add1.cpday,sizeof(wc_zp_add1.cpday)-1);
    memcpy(wc_zp_add1.cpno,wd_zp_add1.cpno,sizeof(wc_zp_add1.cpno)-1);
    memcpy(wc_zp_add1.use,wd_zp_add1.use,sizeof(wd_zp_add1.use));
    memcpy(wc_zp_add1.tsfkday,wd_zp_add1.tsfkday,sizeof(wd_zp_add1.tsfkday));
    memcpy(wc_zp_add3.bscheckpswd,wd_zp_add3.bscheckpswd,sizeof(wc_zp_add3.bscheckpswd)-1);
    memcpy(wc_ty_add1.cpday,wd_ty_add1.cpday,sizeof(wd_ty_add1.cpday));
    memcpy(wc_ty_add1.cpno,wd_ty_add1.cpno,sizeof(wd_ty_add1.cpno));
    memcpy(wc_ty_add1.use,wd_ty_add1.use,sizeof(wd_ty_add1.use));
    memcpy(wc_ty_add4.ptype,wd_ty_add4.ptype,sizeof(wd_ty_add4.ptype));
    memcpy(wc_ty_add4.tsday,wd_ty_add4.tsday,sizeof(wd_ty_add4.tsday));
    memcpy(wc_ty_add4.mac,wd_ty_add4.mac,sizeof(wd_ty_add4.mac));
    memcpy(wc_ty_add4.stopday,wd_ty_add4.stopday,sizeof(wd_ty_add4.stopday));
    memcpy(wc_ty_add4.xyno,wd_ty_add4.xyno,sizeof(wd_ty_add4.xyno));
    memcpy(wc_ty_add4.htno,wd_ty_add4.htno,sizeof(wd_ty_add4.htno));
    memcpy(wc_ty_add4.cdday,wd_ty_add4.cdday,sizeof(wd_ty_add4.cdday));
    memcpy(wc_ty_add4.cdname,wd_ty_add4.cdname,sizeof(wd_ty_add4.cdday));

    iRc=iLvGetAddit(&payin,"002",&wd_zp_add2,sizeof(wd_zp_add2));
    memcpy(wc_zp_add2.bscnt,wd_zp_add2.bscnt,sizeof(wd_zp_add2.bscnt));
    memcpy(wc_zp_add2.bslist,wd_zp_add2.bslist,sizeof(wd_zp_add2.bslist));
    ************����cif��������******************
    InitCifPack(pack);
    memset(&lv_pkgreg,'\0',sizeof(lv_pkgreg));
    lv_pkgreg.wt_date=apatoi(payin.T_wtday,8);
    memcpy(lv_pkgreg.orderno,payin.T_orderno,sizeof(payin.T_orderno));

    AddCifTag(pack,"30A",payin.T_wtday);
    tlrno[16]='\0';
    memcpy(tlrno,g_pub_tx.tel,sizeof(g_pub_tx.tel));

    vtcp_log("[%s][%d] %s,%d~~~~~~~~~~~~~~\n",__FILE__,__LINE__,payin.T_txnum,sizeof(payin.T_txnum));
  	if(memcmp(payin.T_txnum,"30102",5)==0){
        *******֧Ʊ����ҵ��Ҫ��*********
         memcpy(pack->head.cifno,"100",3);
         vAddCifTag(pack,"D35",wc_zp_add1.cpday);
         vAddCifTag(pack,"D01",wc_zp_add1.cpno);
         vAddCifTag(pack,"D37",wc_zp_add1.use);
         vAddCifTag(pack,"D41",wc_zp_add2.bscnt);
         vAddCifTag(pack,"D42",wc_zp_add2.bslist[0]);
         vAddCifTag(pack,"D30",wc_zp_add3.bscheckpswd);
         vAddCifTag(pack,"D32","01");
         vAddCifTag(pack,"D36",wc_zp_add1.tsfkday);
	}
	else{
         ********ͨ�ý���ҵ��Ҫ��*********
         memcpy(pack->head.cifno,"101",3);

         vAddCifTag(pack,"D35",wc_ty_add1.cpday);
         vAddCifTag(pack,"D01",wc_ty_add1.cpno);
         vAddCifTag(pack,"D37",wc_ty_add1.use);
         vAddCifTag(pack,"D41",wc_zp_add2.bscnt);
         vAddCifTag(pack,"D42",wc_zp_add2.bslist);
         vAddCifTag(pack,"D32",wc_ty_add4.ptype);
         vAddCifTag(pack,"D36",wc_ty_add4.tsday);
         vAddCifTag(pack,"D51",wc_ty_add4.mac);
         vAddCifTag(pack,"D61",wc_ty_add4.stopday);
         vAddCifTag(pack,"D62",wc_ty_add4.xyno);
         vAddCifTag(pack,"D63",wc_ty_add4.htno);
         vAddCifTag(pack,"D64",wc_ty_add4.cdday);
         vAddCifTag(pack,"D66",wc_ty_add4.cdname);
	}
   	***********����Ҫ��**********
   	vAddCifTag(pack,"RES",tlrno); 

	tmp_orderno=apatoi(payin.T_orderno,sizeof(payin.T_orderno));
	sprintf(lv_pkgreg.orderno,"%08d",tmp_orderno);
	vAddCifTag(pack,"0BC",lv_pkgreg.orderno);

	memcpy(lv_pkgreg.cash_opn_br_no,payin.T_cashno,12);
	vAddCifTag(pack,"CC5",lv_pkgreg.cash_opn_br_no);

	memcpy(lv_pkgreg.pay_opn_br_no,payin.T_payno,12);
	vAddCifTag(pack,"D21",lv_pkgreg.pay_opn_br_no);

	memcpy(lv_pkgreg.pay_ac_no,payin.T_payactno,32);
	pad_field(lv_pkgreg.pay_ac_no,sizeof(lv_pkgreg.pay_ac_no)-1);
	vAddCifTag(pack,"D23",lv_pkgreg.pay_ac_no);

	memset(tmpbuf,'\0',sizeof(tmpbuf));
	vtcp_log("[%s][%d],%s\n",__FILE__,__LINE__,payin.T_txamt);
	memcpy(tmpbuf,"RMB",3);
	memcpy(tmpbuf+3,payin.T_txamt,15);
	vAddCifTag(pack,"32A",tmpbuf);

	memcpy(lv_pkgreg.pay_name,payin.T_payname,60);
	vAddCifTag(pack,"D22",lv_pkgreg.pay_name);

	memcpy(lv_pkgreg.cash_ac_no,payin.T_cashactno,32);
	pad_field(lv_pkgreg.cash_ac_no,sizeof(lv_pkgreg.cash_ac_no)-1);
	vAddCifTag(pack,"59C",lv_pkgreg.cash_ac_no);

	memcpy(lv_pkgreg.cash_name,payin.T_cashname,60);
	vAddCifTag(pack,"59A",lv_pkgreg.cash_name);

	memcpy(lv_pkgreg.lv_brf,payin.T_content,60);
	vAddCifTag(pack,"D38",lv_pkgreg.lv_brf);   

	lv_pkgreg.resp_date=apatoi(payin.T_respday,8);
	vAddCifTag(pack,"BS6",payin.T_respday);     ***��ִ����***
	vAddCifTag(pack,"TXC","0100");              ***����ӵĸ����򣺽��״���(4)*****
	vAddCifTag(pack,"DA0","00");                ***��ǩ״̬****
	vtcp_log("[%s][%d]~~~~~~~~~~~~~~\n",__FILE__,__LINE__);
	**********����CIF�������*******

	memset(pcFileName,'\0',sizeof(pcFileName));
	sprintf(pcFileName,"%s/etc/cif.ini",getenv("WORKDIR"));
	memset(cifip,'\0',sizeof(cifip));
	*******��ȡ��ӡ��������IP�Ͷ˿ں�*******

	iRc=hv_readIni(pcFileName,"CIFSERVER","Address",cifip);
	if(iRc){
				vtcp_log("%s,%d,��ȡIP��ַ����\n",__FILE__,__LINE__);
		   		sprintf(acErrMsg,"��ȡIP��ַ����");
		        strcpy( g_pub_tx.reply, "P012" );
		        WRITEMSG
          		return(-1);
	}
	vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,cifip);
	memset(tmp_port,'\0',sizeof(tmp_port));
	iRc=hv_readIni(pcFileName,"CIFSERVER","Port",tmp_port);
	if(iRc){
			vtcp_log("%s,%d,��ȡ�˿ںŴ���\n",__FILE__,__LINE__);
	   		sprintf(acErrMsg,"��ȡ�˿ںŴ���");
	        strcpy( g_pub_tx.reply, "P012" );
	        WRITEMSG
       		return(-1);
	}
	vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,tmp_port);
	vtcp_log("[%s][%d][%s],[%d]~~~~~~~~~~~~~~\n",__FILE__,__LINE__,pack,apatoi(pack->head.length,6));
	cifport=apatoi(tmp_port,strlen(tmp_port));
	*******������ӡ������********
	iSocketID=Cif_connect(cifip,cifport);
	if(iSocketID<=0)
    {
                ****Add by SSH,2005.1.11,Ϊ����ļ���׼��*****
                char filename[64];
                memset(filename,'\0',sizeof(filename));
                fcopy1(filename,pack,FLAG_SENDER);
                *****Add by SSH,end*****
				vtcp_log("%s,%d,�޷������������\n",__FILE__,__LINE__);
	   			sprintf(acErrMsg,"�޷������������");
	        	strcpy( g_pub_tx.reply, "P012" );
	        	WRITEMSG
       			return(-1);
    }
	vtcp_log("%d\n", iSocketID);
	*******����CIF���ĵ���ӡ������****
	iRc=Cif_send(iSocketID,pack,apatoi(pack->head.length,sizeof(pack->head.length)));
        if(iRc<0)
        {
				vtcp_log("%s,%d,�޷����ͱ��ĵ��������\n",__FILE__,__LINE__);
	   			sprintf(acErrMsg,"�޷����ͱ��ĵ��������");
	        	strcpy( g_pub_tx.reply, "P012" );
	        	WRITEMSG
                Cif_close(iSocketID);
       			return(-1);
        }
	iRc=Cif_recv(iSocketID,pack,9);
        if(iRc<0){
				vtcp_log("%s,%d,�������ݴ���\n",__FILE__,__LINE__);
	   			sprintf(acErrMsg,"�������ݴ���");
	        	strcpy( g_pub_tx.reply, "P012" );
	        	WRITEMSG
                Cif_close(iSocketID);
       			return(-1);
        }
	iPackLen=apatoi(pack->head.length,sizeof(pack->head.length));
	vtcp_log("[%s][%d],���ĳ���Ϊ[%s]\n",__FILE__,__LINE__,iPackLen);
	if(iPackLen<sizeof(pack->head) || iPackLen>=sizeof(packcif)){
          		vtcp_log("%s,%d,���յ��ı���[%.9s]����[%d]���Ϸ�!\n",__FILE__,__LINE__,pack,iPackLen);
	   			sprintf(acErrMsg,"���յ��ı��ĳ��Ȳ��Ϸ�");
	        	strcpy( g_pub_tx.reply, "P012" );
	        	WRITEMSG
                Cif_close(iSocketID);
       			return(-1);
    }
	iRc=Cif_recv(iSocketID,((char*)pack)+9,iPackLen-9);
        if(iRc){
                vtcp_log("%s,%d,���ձ��ĳ���,���·���!\n",__FILE__,__LINE__);
	   			sprintf(acErrMsg,"���ձ��ĳ���,���·���");
	        	strcpy( g_pub_tx.reply, "P012" );
	        	WRITEMSG
                Cif_close(iSocketID);
       			return(-1);
        }
	*******�ر�����*******
	Cif_close(iSocketID);
	memcpy(status,pack->head.status,3);
	vtcp_log("[%s][%d],**************���ص�״̬stat=[%s]\n",status);
	if(memcmp(status,"000",3)==0){

	iRc = iLvSetAddit(&payin,"QSX","1",1);
	if(iRc) {
                vtcp_log("%s,%d,CIF���ķ��ͳɹ�,���÷��ͱ�־ʧ��!\n",__FILE__,__LINE__);
	   			sprintf(acErrMsg,"CIF���ķ��ͳɹ�,���÷��ͱ�־ʧ��");
	        	strcpy( g_pub_tx.reply, "P012" );
	        	WRITEMSG
       			return(-1);
      }
	}else{
        iRc = iLvSetAddit(&payin,"QSX","0",1);
        vtcp_log("%s,%d,CIF���ķ���ʧ��,��Ҫ���·���!\n",__FILE__,__LINE__);
	   	sprintf(acErrMsg,"CIF���ķ���ʧ��,��Ҫ���·���");
	    strcpy( g_pub_tx.reply, "P012" );
	    WRITEMSG
       	return(-1);
	}

	vtcp_log("[%s][%d] ������ӡ������Ϣ,���¼�¼********************�ɹ�[%s]\n",__FILE__,__LINE__,status);
	****�ɹ�����cif������ȷ������Ӧ*****
	return(0);
****************/
}

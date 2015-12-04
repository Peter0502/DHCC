/*************************************************
* �� �� ����sp9665.c
* ��������: ��¼�����֤��Ϣ 
* ��    ��: liuyong
* �������: 2009��08��25��
* �޸ļ�¼:   
*     1.��    ��:
*       �� �� ��:
*       �޸�����:
*************************************************/

#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "stdlib.h" 
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#define  EXTERN
#include "public.h"
#include "pub_tx.h"
#include "idcode_chk_inf_c.h"

static struct idcode_chk_inf_c idcode_chk_inf;

sp9665()
{
        int ret = 0;
	char filename[100];
	char tmpfile[100];
	char idno[20];
	char brno[5];
        char ttyno[5];

        /** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&idcode_chk_inf,0x00,sizeof(struct idcode_chk_inf_c));
	memset(idno,'\0',sizeof(idno));
	memset(brno,'\0',sizeof(brno));
	memset(ttyno,'\0',sizeof(ttyno));
	memset(filename,'\0',sizeof(filename));
	memset(tmpfile,0x00,sizeof(tmpfile));
        /** ȡֵ����ֵ **/
        get_zd_data(DC_TX_BR_NO,idcode_chk_inf.br_no); /*������*/
        get_zd_data(DC_TX_BR_NO,idcode_chk_inf.chkbrno);     /*��Ա��*/
        get_zd_data(DC_TTY,idcode_chk_inf.tel);
        get_zd_data("0480",idcode_chk_inf.tx_code); /*���׺�*/
        get_zd_long("0530",&idcode_chk_inf.wk_date); /*��������*/
        get_zd_data("0810",idcode_chk_inf.msgid);    /**/
        get_zd_data("0820",idcode_chk_inf.msgref);   /**/
        get_zd_data("0340",idcode_chk_inf.msgno);
        get_zd_long("0540",&idcode_chk_inf.req_date);
        get_zd_data("0890",idcode_chk_inf.bus_code);
        get_zd_data("0270",idcode_chk_inf.oper_user);
        get_zd_data("0620",idcode_chk_inf.idno);
        get_zd_data("0260",idcode_chk_inf.username);
        get_zd_data("0670",idcode_chk_inf.success);
        get_zd_long("0520",&idcode_chk_inf.end_date);
        get_zd_data("0500",idcode_chk_inf.result);
        get_zd_data("0250",idcode_chk_inf.branch);
        
        get_zd_data(DC_TX_BR_NO,brno);
        get_zd_data(DC_TTY,ttyno); 
        
	/** �������� **/

	/* �����´�ȫ·���ļ���(����) */
	memcpy(brno,idcode_chk_inf.br_no,strlen(idcode_chk_inf.br_no));
	memcpy(idno,idcode_chk_inf.idno,strlen(idcode_chk_inf.idno));
	pub_base_strpack (brno);
        pub_base_strpack (idno);
        /*strcpy(filename,idno);*/
        strcat(filename,brno);
        strcat(filename,ttyno);
        pub_base_strpack (filename);
	
	sprintf(tmpfile,"%s/%s",getenv("FILDIR"),filename);
        pub_base_strpack ( tmpfile );
	
        memcpy(idcode_chk_inf.photo,tmpfile,strlen(tmpfile));
	
        sprintf(acErrMsg,"ͼƬ·��[%s]",idcode_chk_inf.photo);
        WRITEMSG
  
        /*�Ǽ������֤��Ϣ*/
        ret = Idcode_chk_inf_Ins(&idcode_chk_inf);
        if(ret)
	{
	    sprintf(acErrMsg,"�Ǽ������֤��Ϣ����![%d]",ret);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"C021");
	    ERR_DEAL;
	}
  	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�Ǽ������֤��Ϣ�ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�Ǽ������֤��Ϣʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}




/*************************************************
* ��  ����:  sp9666.c
* ��������: ��ȡ�ͻ�ͼƬ��Ϣ
* ��    ��: liuyong
* �������: 2009��08��15��
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
#include "cif_photo_inf_c.h"
#include "cif_cop_photo_c.h"

static struct cif_photo_inf_c cif_photo_inf;
static struct cif_cop_photo_c cif_cop_photo;

sp9666()
{
        int ret = 0;
	char filename[100];

        /** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&cif_photo_inf,0x00,sizeof(struct cif_photo_inf_c));
	memset(&cif_cop_photo,0x00,sizeof(struct cif_cop_photo_c));

	/** ȡֵ����ֵ **/
        get_zd_long("0280",&cif_photo_inf.cif_no);		/* �ͻ��� */
        get_zd_data("0720",cif_photo_inf.photo_type);
        get_zd_long("0280",&cif_cop_photo.cif_no);
        get_zd_data("0720",cif_cop_photo.photo_type);
	/** �������� **/

	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName( filename );
	pub_base_strpack(filename);
       
        sprintf(acErrMsg,"�ļ�·��:[%s]",filename);
        WRITEMSG
	
	if(cif_photo_inf.photo_type[0] =='1')
	{
            sprintf(acErrMsg,"ȡ�ͻ���Ƭ!");
            WRITEMSG
	    strcpy(cif_photo_inf.photo,filename);
	    ret=sql_count("cif_photo_inf","cif_no=%ld",cif_photo_inf.cif_no);
        }
        else if(cif_photo_inf.photo_type[0]=='2')
        {
            sprintf(acErrMsg,"ȡ�ͻ�ӡ����Ϣ��");
            WRITEMSG
            strcpy(cif_cop_photo.photo,filename);	
            ret=sql_count("cif_cop_photo","cif_no=%ld",cif_cop_photo.cif_no);
        }
	
	if(ret<0)
	{
	    sprintf(acErrMsg,"��ѯ�ͻ�ͼƬ��Ϣ����![%d]",ret);
	    WRITEMSG
	    /*strcpy(g_pub_tx.reply,"C006");
	    ERR_DEAL;*/
	}
	else if(ret == 0)
	{
	    sprintf(acErrMsg,"�ͻ���Ϣ�����ڣ�[%d]",ret);
	    WRITEMSG
	    /*strcpy(g_pub_tx.reply,"C006");
	    ERR_DEAL;*/
	}
	else
	{
            sprintf(acErrMsg,"��ʼȡ�ͻ�ͼƬ��Ϣ��");
            WRITEMSG   	
            /*���ɿͻ�ͼƬ�ļ�*/
	    if(cif_photo_inf.photo_type[0] =='1')
	    {
                sprintf(acErrMsg,"��ȡ�ͻ�[%ld]��Ƭ��",cif_photo_inf.cif_no);
                WRITEMSG
	        ret = Cif_photo_inf_Sel(g_pub_tx.reply,&cif_photo_inf,
	              "cif_no=%ld",cif_photo_inf.cif_no);
	    }
	    else if(cif_photo_inf.photo_type[0]=='2')
	    {
                sprintf(acErrMsg,"��ȡ�ͻ�[%ld]ӡ����",cif_cop_photo.cif_no);
                WRITEMSG
                sprintf(acErrMsg,"�ͻ�·��:[%s]",cif_cop_photo.photo);
                WRITEMSG
	        Cif_cop_photo_Debug(&cif_cop_photo);
	        ret = Cif_cop_photo_Sel(g_pub_tx.reply,&cif_cop_photo,
                      "cif_no=%ld",cif_photo_inf.cif_no);
                /*ret = Cif_photo_inf_Sel(g_pub_tx.reply,&cif_photo_inf,"cif_no=%ld",cif_photo_inf.cif_no);*/
	    }
	    if(ret ==100)
	    {
	        sprintf(acErrMsg,"�ͻ��Ų����ڣ����飡[%s]",g_pub_tx.reply);
	        WRITEMSG
	        /*strcpy(g_pub_tx.reply,"C007");
	        ERR_DEAL;*/	
	    }
	    else if(ret)
	    {
	        sprintf(acErrMsg,"��ȡ�ͻ�ͼƬ��Ϣ�쳣��[%s]",g_pub_tx.reply);
	        WRITEMSG
	        /*strcpy(g_pub_tx.reply,"C006");
	        ERR_DEAL;*/
	    }
	    sprintf(acErrMsg,"���ͻ����Ѿ����ڣ�");
	    WRITEMSG

	    set_zd_data(DC_FILE_SND,"2");
	}
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ͻ�ͼƬ��Ϣ��ȡ�ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"��ȡ�ͻ�ͼƬ��Ϣʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

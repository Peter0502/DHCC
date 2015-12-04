 /*************************************************
* ��  ����: sp9664.c
* ��������: �����˿��Ż�ȡ�ͻ���Ϣ
* ��    ��: liuyong
* �������: 2009��08��31��
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
#include "cif_basic_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_per_inf_c.h"
#include "cif_cop_inf_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "dic_data_c.h"

static struct cif_photo_inf_c cif_photo_inf;
static struct cif_cop_photo_c cif_cop_photo;
static struct cif_basic_inf_c cif_basic_inf;
static struct cif_per_inf_c   cif_per_inf;
static struct cif_cop_inf_c   cif_cop_inf;
static struct cif_addr_inf_c cif_addr_inf;
static struct cif_email_inf_c cif_email_inf;
static struct mdm_ac_rel_c mdm_ac_rel;
static struct prdt_ac_id_c prdt_ac_id;
static struct cif_id_code_rel_c cif_id_code_rel;
static struct dic_data_c dic_data;

sp9664()
{
        int ret = 0;
	char filename[100];
	char tmpfile[100];
	char cifno[20];
        char ttyno[10];
        char brno[10];
        long acid;
        char acno[21];
        char idtypes[51];

        /** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&cif_photo_inf,0x00,sizeof(struct cif_photo_inf_c));
	memset(&cif_cop_photo,0x00,sizeof(struct cif_cop_photo_c));
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
        memset(&cif_per_inf,0x00,sizeof(struct cif_per_inf_c));
        memset(&cif_cop_inf,0x00,sizeof(struct cif_cop_inf_c));
        memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
        memset(&prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
        memset(&dic_data,0x00,sizeof(struct dic_data_c));
        memset(filename,0x00,sizeof(filename));
	memset(tmpfile,0x00,sizeof(tmpfile));
	memset(cifno,0x00,sizeof(cifno));
	memset(ttyno,'\0',sizeof(ttyno));
        memset(brno,'\0',sizeof(brno));
        memset(idtypes,'\0',sizeof(idtypes));

        /** ȡֵ����ֵ **/
        get_zd_data("0300",mdm_ac_rel.ac_no);	/* �˺Ż򿨺� */
        get_zd_data("0720",cif_photo_inf.photo_type);       /*ͼƬ����*/
        get_zd_data("0720",cif_cop_photo.photo_type);       /*ͼƬ����*/
	get_zd_data(DC_TTY,ttyno);            /*��Ա��*/
        get_zd_data(DC_TX_BR_NO,brno);        /*������*/
        get_zd_data("0300",acno);
        pub_base_old_acno(acno);	
        /** �������� **/
        /*�����˿��Ų���ac_id*/
        sprintf(acErrMsg,"�˺ţ�[%s]",acno);
        WRITEMSG

        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,
              "ac_no='%s'",acno);
        if(ret == 100)
        {
            sprintf(acErrMsg,"�˺Ż򿨺Ų�����![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"M002");
            ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"��ȡ�������˻���ϵ�����[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"W011");
            ERR_DEAL;
        }
        acid = mdm_ac_rel.ac_id;
        if(acid<=0)
        {
            sprintf(acErrMsg,"��ȡ�˻�ID�����������Ա��ϵ��");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M002");
            ERR_DEAL;
        }
        
        sprintf(acErrMsg,"�˻�ID��[%ld]",acid);
        WRITEMSG

        /*����ac_id���Ҳ�Ʒ���ҵ���Ӧ�Ŀͻ���*/
        prdt_ac_id.ac_id = acid;
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id,"ac_id=%ld",mdm_ac_rel.ac_id); 
        sprintf(acErrMsg,"�ͻ���:[%ld]",prdt_ac_id.cif_no);
        WRITEMSG
        if(ret == 100)
        {
            sprintf(acErrMsg,"�˺Ŷ�Ӧ�Ĳ�Ʒ������![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"M001");
            ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"��ȡ��Ʒ���˻���ϵ�����[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"W012");
            ERR_DEAL;
        }
        /*memcpy(cifno,&prdt_ac_id.cif_no,sizeof(prdt_ac_id.cif_no));*/
        sprintf(cifno,"%ld",prdt_ac_id.cif_no);
        pub_base_strpack(cifno);
        if(strlen(cifno)==0)
        {
            sprintf(acErrMsg,"�˺Ż򿨺Ŷ�Ӧ�Ŀͻ��Ų����ڣ�");
            WRITEMSG
            strcpy(g_pub_tx.reply,"C020");
            ERR_DEAL;
        }
   
        sprintf(acErrMsg,"added new���˺Ŷ�Ӧ�Ŀͻ��ţ�[%s]",cifno);
        WRITEMSG
     
	/* �����´�ȫ·���ļ���(����) */
        /*pub_base_strpack(cifno);
        pub_base_strpack(ttyno);
        pub_base_strpack(brno);
    
        strcpy(filename,cifno);
        pub_base_strpack(filename);
        strcat(filename,brno);
        strcat(filename,ttyno);
        pub_base_strpack(filename);
        sprintf(tmpfile,"%s/%s/%s%s",getenv("HOME"),"image",filename,cif_photo_inf.photo_type);
        */
        pub_base_AllDwnFilName(tmpfile); 
        pub_base_strpack(tmpfile);
        sprintf(acErrMsg,"�ļ������ȣ�[%d]",strlen(tmpfile));
        WRITEMSG
        if(cif_photo_inf.photo_type[0]=='1')
        {
            memcpy(cif_photo_inf.photo,tmpfile,strlen(tmpfile));
            sprintf(acErrMsg,"ͼƬ·����[%s]",cif_photo_inf.photo);
            WRITEMSG
        }
        else if(cif_photo_inf.photo_type[0]=='2')
        {
            memcpy(cif_cop_photo.photo,tmpfile,strlen(tmpfile));
            sprintf(acErrMsg,"ͼƬ·����[%s]",cif_cop_photo.photo);
            WRITEMSG
        }

        /*ȡ�ͻ�����*/
        ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",prdt_ac_id.cif_no);
        if(ret==100)
        {
          sprintf(acErrMsg,"�ͻ��Ų�����[%s],ȡ�ͻ���������",g_pub_tx.reply);
          WRITEMSG
          strcpy(g_pub_tx.reply,"C007");
          ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣��[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;
        }
        set_zd_data("0630",cif_basic_inf.name);
        /*ȡ�ͻ�֤������*/
        ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,
              "cif_no=%ld",prdt_ac_id.cif_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"�ͻ��Ų�����![%s],ȡ֤���ų���",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"��ȡ�ͻ�֤����Ϣ�쳣��[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;
        }
        /*���ÿͻ�֤����Ϣ*/
        set_zd_data("0680",cif_id_code_rel.id_type);
        set_zd_data("0620",cif_id_code_rel.id_no);
        sprintf(acErrMsg,"֤�����ͣ�[%s]",cif_id_code_rel.id_type);
        WRITEMSG
        sprintf(acErrMsg,"֤���ţ�[%s]",cif_id_code_rel.id_no);
        WRITEMSG
        
        strcpy(idtypes,"id_type"); 
        pub_base_strpack(idtypes);      
        ret = Dic_data_Sel(g_pub_tx.reply,&dic_data,"pr_code='%s' and pr_seq='%s'",idtypes,cif_id_code_rel.id_type);
        sprintf(acErrMsg,"ret=[%d]",ret);
        WRITEMSG
        if(ret==100)
        {
            sprintf(acErrMsg,"ȡ֤�����ʹ���[%s]",g_pub_tx.reply);
            WRITEMSG
        }
        else if(ret)
        {
            sprintf(acErrMsg,"��ȡ�ͻ�֤�����ʹ���!",g_pub_tx.reply);
            WRITEMSG
        }
        sprintf(acErrMsg,"֤������:[%s]",dic_data.content);
        WRITEMSG

        if(strlen(dic_data.content)!=0)
        {
            set_zd_data("0830",dic_data.content);
        } 
        ret = Cif_addr_inf_Sel(g_pub_tx.reply,&cif_addr_inf,
                  "cif_no=%ld",prdt_ac_id.cif_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"�ͻ��Ų�����![%s]��ȡ�ͻ���ַ��Ϣ����",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"��ȡ�ͻ���ַ��Ϣ�쳣��[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;*/
        }
        ret = Cif_email_inf_Sel(g_pub_tx.reply,&cif_email_inf,
                   "cif_no=%ld",prdt_ac_id.cif_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"�ͻ��Ų�����![%s]��ȡ�ͻ���ϵ��Ϣ����",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"��ȡ�ͻ���ϵ��Ϣ�쳣��[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;*/
        }
        set_zd_data("0260",cif_addr_inf.addr);
        set_zd_data("0580",cif_email_inf.email);
        set_zd_data("0250",cif_email_inf.link_man);
        
        sprintf(acErrMsg,"�ͻ���ַ��[%s]-��ϵ��ʽ��[%s]-��ϵ�ˣ�[%s]",cif_addr_inf.addr,cif_email_inf.email,cif_email_inf.link_man);
        WRITEMSG
 
        /*ȡ�ͻ�����*/
        ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
              "cif_no=%ld",prdt_ac_id.cif_no);	
        if(ret == 100)
        {
            sprintf(acErrMsg,"�ͻ��Ų�����![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"��ȡ�ͻ���Ϣ�쳣��[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;
        }
        
        /*���˿ͻ�*/
        if(cif_basic_inf.type[0]=='1')
        {
            ret = Cif_per_inf_Sel(g_pub_tx.reply,&cif_per_inf,
                  "cif_no=%ld",prdt_ac_id.cif_no);
            if(ret == 100)
            {
                sprintf(acErrMsg,"�ͻ��Ų�����![%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C007");
                ERR_DEAL;*/
            }   
            else if(ret)
            {
                sprintf(acErrMsg,"��ȡ�ͻ���Ϣ�쳣��[%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C006");
                ERR_DEAL;*/
            }
           
            /*���ÿͻ�������Ϣ*/
            /*set_zd_data("0630",cif_per_inf.name);*/
            set_zd_data("0700",cif_per_inf.sex);
            set_zd_data("0710","0"); 
            sprintf(acErrMsg,"�ͻ�������[%s]-�ͻ��Ա�[%s]",cif_per_inf.name,cif_per_inf.sex);
            WRITEMSG
        }
        else if(cif_basic_inf.type[0]=='2')
        {
            /*��˾�ͻ�*/
            ret = Cif_cop_inf_Sel(g_pub_tx.reply,&cif_cop_inf,
                  "cif_no=%ld",prdt_ac_id.cif_no);
               
            if(ret == 100)
            {
                sprintf(acErrMsg,"��˾�ͻ��Ų�����![%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C007");
                ERR_DEAL;*/
            }   
            else if(ret)
            {
                sprintf(acErrMsg,"��ȡ��˾�ͻ���Ϣ�쳣��[%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C006");
                ERR_DEAL;*/
            }
            /*���ù�˾�ͻ���Ϣ*/
            set_zd_data("0810",cif_cop_inf.crd_no);
            set_zd_data("0820",cif_cop_inf.license);
            set_zd_long("0440",cif_cop_inf.reg_date);
            set_zd_double("0410",cif_cop_inf.reg_fund);
            set_zd_double("0420",cif_cop_inf.paicl_up_capital);
            set_zd_data("0710","1"); 
            sprintf(acErrMsg,"��˾����֤��:[%s]",cif_cop_inf.crd_no);
            WRITEMSG
            sprintf(acErrMsg,"��˾Ӫҵִ�գ�[%s]",cif_cop_inf.license);
            WRITEMSG
            sprintf(acErrMsg,"��˾ע������:[%ld]",cif_cop_inf.reg_date);
            WRITEMSG
            sprintf(acErrMsg,"��˾ע���ʽ�[%f]",cif_cop_inf.reg_fund);
            WRITEMSG
            sprintf(acErrMsg,"��˾ʵ���ʱ���[%f]",cif_cop_inf.paicl_up_capital);            WRITEMSG
        }
	strcpy(cif_photo_inf.photo,tmpfile);
	cif_photo_inf.cif_no = prdt_ac_id.cif_no;
        cif_cop_photo.cif_no = prdt_ac_id.cif_no;

        /*��ѯ�ͻ�ͼƬ��Ϣ�Ƿ����*/
	if(cif_photo_inf.photo_type[0]=='1')
	{
	     ret=sql_count("cif_photo_inf","cif_no=%ld",prdt_ac_id.cif_no);
	}
	else if(cif_photo_inf.photo_type[0]=='2')
	{
	    ret=sql_count("cif_cop_photo","cif_no=%ld",prdt_ac_id.cif_no);
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
		/*���ɿͻ�ͼƬ�ļ�*/
		if(cif_photo_inf.photo_type[0]=='1')
		{
                  sprintf(acErrMsg,"��ȡ����ͼƬ��Ϣ��");
                  WRITEMSG
		  ret = Cif_photo_inf_Sel(g_pub_tx.reply,&cif_photo_inf,
		    "cif_no=%ld",prdt_ac_id.cif_no);
	  }
	  else if(cif_photo_inf.photo_type[0]=='2')
	  {
	  	ret = Cif_cop_photo_Sel(g_pub_tx.reply,&cif_cop_photo,
		    "cif_no=%ld",prdt_ac_id.cif_no);
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
    sprintf(acErrMsg,"��ȡ�ͻ���Ϣ�ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"��ȡ�ͻ���Ϣʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}



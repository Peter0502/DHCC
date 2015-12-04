/*************************************************
* ÎÄ  ¼þÃû:  sp9668.c
* ¹¦ÄÜÃèÊö: »ñÈ¡¿Í»§ÐÅÏ¢£¬°üº¬Í¼Æ¬ÐÅÏ¢Í
* ×÷    Õß: liuyong
* Íê³ÉÈÕÆÚ: 2009Äê08ÔÂ15ÈÕ
* ÐÞ¸Ä¼ÇÂ¼:   
*     1.ÈÕ    ÆÚ:
*       ÐÞ ¸Ä ÈË:
*       ÐÞ¸ÄÄÚÈÝ:
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
#include "cif_id_code_rel_c.h"
#include "dic_data_c.h"

static struct cif_photo_inf_c cif_photo_inf;
static struct cif_cop_photo_c cif_cop_photo;
static struct cif_basic_inf_c cif_basic_inf;
static struct cif_per_inf_c   cif_per_inf;
static struct cif_cop_inf_c   cif_cop_inf;
static struct cif_id_code_rel_c cif_id_code_rel;
static struct cif_addr_inf_c cif_addr_inf;
static struct cif_email_inf_c cif_email_inf;
static struct dic_data_c dic_data;

sp9668()
{
        int ret = 0;
	char filename[100];
	char tmpfile[100];
	char cifno[20];
        char ttyno[5];
        char brno[10];
        char idno[20];
        char idtypes[51];
        char idtyp[2];  /*Ö¤¼þÀàÐÍ*/
  
        /** ³õÊ¼»¯È«¾Ö±äÁ¿ **/
	pub_base_sysinit();

	/** Êý¾Ý³õÊ¼»¯ **/
	memset(&cif_photo_inf,0x00,sizeof(struct cif_photo_inf_c));
	memset(&cif_cop_photo,0x00,sizeof(struct cif_cop_photo_c));
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
        memset(&cif_per_inf,0x00,sizeof(struct cif_per_inf_c));
        memset(&cif_cop_inf,0x00,sizeof(struct cif_cop_inf_c));
        memset(&cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
        memset(filename,0x00,sizeof(filename));
	memset(tmpfile,0x00,sizeof(tmpfile));
	memset(cifno,0x00,sizeof(cifno));
	memset(ttyno,'\0',sizeof(ttyno));
        memset(brno,'\0',sizeof(brno));
        memset(idtypes,'\0',sizeof(idtypes));
        memset(idtyp,'\0',sizeof(idtyp));

        /** È¡Öµ¡¢¸³Öµ **/
        get_zd_data("0620",idno); /*ÓÐÐ§Ö¤¼þºÅÂë*/
        get_zd_data("0620",cif_id_code_rel.id_no);
        get_zd_data("0670",idtyp);
        get_zd_data("0670",cif_id_code_rel.id_type);
        get_zd_long("0280",&cif_photo_inf.cif_no);	/* ¿Í»§ºÅ */
        get_zd_data("0720",cif_photo_inf.photo_type);       /*Í¼Æ¬ÀàÐÍ*/
        get_zd_long("0280",&cif_cop_photo.cif_no);	/* ¿Í»§ºÅ */
        get_zd_data("0720",cif_cop_photo.photo_type);       /*Í¼Æ¬ÀàÐÍ*/
	get_zd_data(DC_TTY,ttyno);            /*¹ñÔ±ºÅ*/
        get_zd_data(DC_TX_BR_NO,brno);        /*»ú¹¹ºÅ*/
        get_zd_data("0280",cifno);
       	
        /** ´¦ÀíÁ÷³Ì **/
        sprintf(acErrMsg,"¿Í»§ºÅ£º[%ld]",cif_photo_inf.cif_no);
        WRITEMSG
        sprintf(acErrMsg,"Ö¤¼þºÅÂë£º[%s]",cif_id_code_rel.id_no);
        WRITEMSG
        sprintf(acErrMsg,"Ö¤¼þÀàÐÍ:[%s]",cif_id_code_rel.id_type);
        WRITEMSG
        /*Èç¹û¿Í»§ºÅÎª¿Õ£¬Ôò¸ù¾ÝÖ¤¼þºÅÂëÈ¡¿Í»§ºÅ*/
        if(cif_photo_inf.cif_no==0)
        {
        	  sprintf(acErrMsg,"¸ù¾ÝÖ¤¼þºÅÂëÈ¡¿Í»§ºÅ!");
            WRITEMSG
            ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,
              "id_no='%s' and id_type='%s'",cif_id_code_rel.id_no,cif_id_code_rel.id_type);
            if(ret == 100)
            {
              sprintf(acErrMsg,"Ö¤¼þºÅÂë²»´æÔÚ![%s],È¡Ö¤¼þºÅ³ö´í£¡",g_pub_tx.reply);
              WRITEMSG
              strcpy(g_pub_tx.reply,"C007");
              ERR_DEAL;
            }
            else if(ret)
            {
              sprintf(acErrMsg,"¶ÁÈ¡¿Í»§Ö¤¼þÐÅÏ¢Òì³££¡[%s]",g_pub_tx.reply);
              WRITEMSG
              strcpy(g_pub_tx.reply,"C006");
              ERR_DEAL;
            }
        }
        if(cif_id_code_rel.cif_no > 0)
        {
          cif_photo_inf.cif_no = cif_id_code_rel.cif_no;
          cif_cop_photo.cif_no = cif_id_code_rel.cif_no;
        }
	/* Éú³ÉÏÂ´«È«Â·¾­ÎÄ¼þÃû(ÅúÁ¿) */
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
        sprintf(acErrMsg,"ÎÄ¼þÃû³¤¶È£º[%d]",strlen(tmpfile));
        WRITEMSG
        if(cif_photo_inf.photo_type[0]=='1')
        {
            memcpy(cif_photo_inf.photo,tmpfile,strlen(tmpfile));
            sprintf(acErrMsg,"Í¼Æ¬Â·¾¶£º[%s]",cif_photo_inf.photo);
            WRITEMSG
        }
        else if(cif_photo_inf.photo_type[0]=='2')
        {
            memcpy(cif_cop_photo.photo,tmpfile,strlen(tmpfile));
            sprintf(acErrMsg,"Í¼Æ¬Â·¾¶£º[%s]",cif_cop_photo.photo);
            WRITEMSG
        }
        
        /*È¡¿Í»§ÐÕÃû*/
        ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",cif_photo_inf.cif_no);
        if(ret==100)
        {
          sprintf(acErrMsg,"¿Í»§ºÅ²»´æÔÚ[%s],È¡¿Í»§ÐÕÃû³ö´í£¡",g_pub_tx.reply);
          WRITEMSG
          strcpy(g_pub_tx.reply,"C007");
          ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"¶ÁÈ¡¿Í»§ÐÕÃûÐÅÏ¢Òì³££¡[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;
        }
        set_zd_data("0630",cif_basic_inf.name);
        
        /*È¡¿Í»§Ö¤¼þºÅÂë*/
        ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,
              "cif_no=%ld",cif_photo_inf.cif_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"¿Í»§ºÅ²»´æÔÚ![%s],È¡Ö¤¼þºÅ³ö´í£¡",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"¶ÁÈ¡¿Í»§Ö¤¼þÐÅÏ¢Òì³££¡[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;
        }
        
        strcpy(idtypes,"id_type");       
        pub_base_strpack(idtypes);
        ret = Dic_data_Sel(g_pub_tx.reply,&dic_data,"pr_code='%s' and pr_seq='%s'",idtypes,cif_id_code_rel.id_type);
        if(ret==100)
        {
            sprintf(acErrMsg,"È¡Ö¤¼þÀàÐÍ´íÎó£¡[%s]",g_pub_tx.reply);
            WRITEMSG
        }
        else
        {
            sprintf(acErrMsg,"¶ÁÈ¡¿Í»§Ö¤¼þÀàÐÍ±í´íÎó![%s]",g_pub_tx.reply);
            WRITEMSG
        }
        if(strlen(dic_data.content)!=0)
        {
            set_zd_data("0830",dic_data.content);
        } 
        /*ÉèÖÃ¿Í»§Ö¤¼þÐÅÏ¢*/
        set_zd_data("0680",cif_id_code_rel.id_type);
        set_zd_data("0620",cif_id_code_rel.id_no);
        
        sprintf(acErrMsg,"Ö¤¼þÀàÐÍ:[%s]-Ö¤¼þºÅÂë:[%s]",cif_id_code_rel.id_type,cif_id_code_rel.id_no);
        WRITEMSG
 
        ret = Cif_addr_inf_Sel(g_pub_tx.reply,&cif_addr_inf,
                  "cif_no=%ld",cif_photo_inf.cif_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"¿Í»§ºÅ²»´æÔÚ![%s]£¬È¡¿Í»§µØÖ·ÐÅÏ¢³ö´í£¡",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"¶ÁÈ¡¿Í»§µØÖ·ÐÅÏ¢Òì³££¡[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;*/
        }
        ret = Cif_email_inf_Sel(g_pub_tx.reply,&cif_email_inf,
                   "cif_no=%ld",cif_photo_inf.cif_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"¿Í»§ºÅ²»´æÔÚ![%s]£¬È¡¿Í»§ÁªÏµÐÅÏ¢³ö´í£¡",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"¶ÁÈ¡¿Í»§ÁªÏµÐÅÏ¢Òì³££¡[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;*/
        }
        set_zd_data("0260",cif_addr_inf.addr);
        set_zd_data("0580",cif_email_inf.email);
        set_zd_data("0250",cif_email_inf.link_man);
     
        sprintf(acErrMsg,"¿Í»§µØÖ·:[%s]-ÁªÏµ·½Ê½:[%s]-ÁªÏµÈË:[%s]",cif_addr_inf.addr,cif_email_inf.email,cif_email_inf.link_man); 
        WRITEMSG
        /*È¡¿Í»§ÀàÐÍ*/
        ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
              "cif_no=%ld",cif_photo_inf.cif_no);	
        if(ret == 100)
        {
            sprintf(acErrMsg,"¿Í»§ºÅ²»´æÔÚ![%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"¶ÁÈ¡¿Í»§ÐÅÏ¢Òì³££¡[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;*/
        }
        
        /*¸öÈË¿Í»§*/
        if(cif_basic_inf.type[0]=='1')
        {
            ret = Cif_per_inf_Sel(g_pub_tx.reply,&cif_per_inf,
                  "cif_no=%ld",cif_photo_inf.cif_no);
            if(ret == 100)
            {
                sprintf(acErrMsg,"¿Í»§ºÅ²»´æÔÚ![%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C007");
                ERR_DEAL;*/
            }   
            else if(ret)
            {
                sprintf(acErrMsg,"¶ÁÈ¡¿Í»§ÐÅÏ¢Òì³££¡[%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C006");
                ERR_DEAL;*/
            }
           
            /*ÉèÖÃ¿Í»§¸öÈËÐÅÏ¢*/
            /*set_zd_data("0630",cif_per_inf.name);*/
            set_zd_data("0700",cif_per_inf.sex);
            set_zd_data("0710","0"); 
            sprintf(acErrMsg,"¿Í»§ÐÕÃû:[%s]-¿Í»§ÐÔ±ð:[%s]",cif_per_inf.name,cif_per_inf.sex);
            WRITEMSG
        }
        else if(cif_basic_inf.type[0]=='2')
        {
            /*¹«Ë¾¿Í»§*/
            ret = Cif_cop_inf_Sel(g_pub_tx.reply,&cif_cop_inf,
                  "cif_no=%ld",cif_photo_inf.cif_no);
               
            if(ret == 100)
            {
                sprintf(acErrMsg,"¹«Ë¾¿Í»§ºÅ²»´æÔÚ![%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C007");
                ERR_DEAL;*/
            }   
            else if(ret)
            {
                sprintf(acErrMsg,"¶ÁÈ¡¹«Ë¾¿Í»§ÐÅÏ¢Òì³££¡[%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C006");
                ERR_DEAL;*/
            }
            /*ÉèÖÃ¹«Ë¾¿Í»§ÐÅÏ¢*/
            set_zd_data("0810",cif_cop_inf.crd_no);
            set_zd_data("0820",cif_cop_inf.license);
            set_zd_long("0440",cif_cop_inf.reg_date);
            set_zd_double("0410",cif_cop_inf.reg_fund);
            set_zd_double("0420",cif_cop_inf.paicl_up_capital);
            set_zd_data("0710","1");

            sprintf(acErrMsg,"¹«Ë¾´úÂëÖ¤:[%s]-¹«Ë¾ÓªÒµÖ´ÕÕ:[%s]",cif_cop_inf.crd_no,cif_cop_inf.license);
            WRITEMSG
            /*sprintf(acErrMsg,"¹«Ë¾×¢²áÈÕÆÚ:[%s]",cif_cop_inf.reg_date);
            WRITEMSG
            sprintf(acErrMsg,"¹«Ë¾×¢²á×Ê½ð:[%f]",cif_cop_inf.reg_fund);
            WRITEMSG
            sprintf(acErrMsg,"¹«Ë¾ÊµÊÕ×Ê±¾:[%f]",cif_cop_inf.paicl_up_capital);
            WRITEMSG*/
        }
	strcpy(cif_photo_inf.photo,tmpfile);
	
	/*²éÑ¯¿Í»§Í¼Æ¬ÐÅÏ¢ÊÇ·ñ´æÔÚ*/
	if(cif_photo_inf.photo_type[0]=='1')
	{
	     ret=sql_count("cif_photo_inf","cif_no=%ld",cif_photo_inf.cif_no);
	}
	else if(cif_photo_inf.photo_type[0]=='2')
	{
	    ret=sql_count("cif_cop_photo","cif_no=%ld",cif_cop_photo.cif_no);
	}
	if(ret<0)
	{
	    sprintf(acErrMsg,"²éÑ¯¿Í»§Í¼Æ¬ÐÅÏ¢³ö´í![%d]",ret);
	    WRITEMSG
	    /*strcpy(g_pub_tx.reply,"C006");
	    ERR_DEAL;*/
	}
	else if(ret == 0)
	{
	    sprintf(acErrMsg,"¿Í»§ÐÅÏ¢²»´æÔÚ£¡[%d]",ret);
	    WRITEMSG
	    /*strcpy(g_pub_tx.reply,"C006");
	    ERR_DEAL;*/
	}
	else
	{
		/*Éú³É¿Í»§Í¼Æ¬ÎÄ¼þ*/
		if(cif_photo_inf.photo_type[0]=='1')
		{
		  ret = Cif_photo_inf_Sel(g_pub_tx.reply,&cif_photo_inf,
		    "cif_no=%ld",cif_photo_inf.cif_no);
	  }
	  else if(cif_photo_inf.photo_type[0]=='2')
	  {
	  	ret = Cif_cop_photo_Sel(g_pub_tx.reply,&cif_cop_photo,
		    "cif_no=%ld",cif_cop_photo.cif_no);
	  }
		if(ret ==100)
		{
		    sprintf(acErrMsg,"¿Í»§ºÅ²»´æÔÚ£¬Çë¼ì²é£¡[%s]",g_pub_tx.reply);
		    WRITEMSG
		    /*strcpy(g_pub_tx.reply,"C007");
		    ERR_DEAL;*/	
		}
		else if(ret)
		{
		    sprintf(acErrMsg,"¶ÁÈ¡¿Í»§Í¼Æ¬ÐÅÏ¢Òì³££¡[%s]",g_pub_tx.reply);
		    WRITEMSG
		    /*strcpy(g_pub_tx.reply,"C006");
		    ERR_DEAL;*/
		}
		sprintf(acErrMsg,"¼ì²é¿Í»§ºÅÒÑ¾­´æÔÚ£¡");
		WRITEMSG
		set_zd_data(DC_FILE_SND,"2");
	}
	
	
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"»ñÈ¡¿Í»§ÐÅÏ¢³É¹¦£¡[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"»ñÈ¡¿Í»§ÐÅÏ¢Ê§°Ü£¡[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}



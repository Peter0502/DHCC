/*************************************************
* �� �� ��:  sp0996.c
* ���������� ƽ̨Ҫ��ѯ�ͻ�ID
*
* ��    ��:  xyy
* ������ڣ� 2007-08-03
*
* �޸ļ�¼��martin
* ��   ��:  2009-12-22
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
        sprintf( acErrMsg, "sql error" ); \
        WRITEMSG \
        goto ErrExit; \
        }
#define EXTERN
#define TX_CODE "0990"
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"

int sp0996()
{
    int i=0;
    int j=0;
    int iNum=0;
    int ret=0;
    long lCifno=0;
    char cTxtype[2];
    char cZjtype[2];
    char cCif_type[2];
    char cDGtmp_type[2];
    char cDGtmp_idno[21];
    char cId_no[21];
    char cCifname[61];
    char cEnt_attri[2];
    struct cif_id_code_rel_c cif_id_code_rel;
    struct cif_id_code_rel_c f_cif_id_code_rel;
    struct cif_basic_inf_c g_cif_basic_inf;
    
    memset(cDGtmp_idno ,0,sizeof(cDGtmp_idno));
    memset(cDGtmp_type ,0,sizeof(cDGtmp_type)); 
    memset(cTxtype     ,0,sizeof(cTxtype));
    memset(cZjtype     ,0,sizeof(cZjtype));
    memset(cCif_type   ,0,sizeof(cCif_type));
    memset(cId_no      ,0,sizeof(cId_no));
    memset(cCifname    ,0,sizeof(cCifname));
    memset(cEnt_attri  ,0,sizeof(cEnt_attri));
    memset(&g_pub_tx   ,0,sizeof(g_pub_tx) );
    memset(&cif_id_code_rel  ,0,sizeof(cif_id_code_rel));
    memset(&f_cif_id_code_rel,0,sizeof(f_cif_id_code_rel));
    memset(&g_cif_basic_inf  ,0,sizeof(struct cif_basic_inf_c));
    vtcp_log("[%s][%d]��ʼ����sp0996\n",__FILE__,__LINE__);
    
    pub_base_sysinit();
    get_zd_data("0670",cTxtype); /**1:���� 2:��˾**/
    vtcp_log("[%s][%d]cTxtype==[%s]\n",__FILE__,__LINE__,cTxtype);
    /***��ʼ��ƽ̨������������ж�***/
    if(cTxtype[0]=='1')/**1:����**/
    {
    	get_zd_data("0680",cCif_type );
      vtcp_log("[%s][%d]cCif_type==[%s]\n",__FILE__,__LINE__,cCif_type);
					/*1���֤2���ڲ�3����4����֤5����֤6ʿ��֤7�۰ľ�������ͨ��֤         
					E��ʱ���֤F����˾���֤H:����G����֤
					*/
    }
    else if(cTxtype[0]=='2')/**2:��˾**/
    {
			get_zd_data("0680",cDGtmp_type);
			get_zd_data("0620",cDGtmp_idno);
			vtcp_log("%s,%d�Թ���֤������[%s]",__FILE__,__LINE__,cDGtmp_type);
			vtcp_log("%s,%d�Թ���֤������[%s]",__FILE__,__LINE__,cDGtmp_idno);
    }
    get_zd_data("0250",cCifname);	/* �ͻ����� */
    vtcp_log("[%s][%d]cCifname==[%s]\n",__FILE__,__LINE__,cCifname);
    if(cTxtype[0]=='1')/***��ѯ���˵Ŀͻ�id***/
    {
			get_zd_data("0250",cCifname);	/* �ͻ����� */
    	  switch(cCif_type[0])
    	  {
    	     case '1':/*�˶����е�֤������ 2009/12/22 17:38:40 martin case*/
             case '2':
             case '3':
             case '4':
             case '5':
             case '6':
             case '7':
             case 'E':
             case 'F':
             case 'G':
             case 'H':
                 get_zd_data("0620",cId_no );  /*֤������-����*/
                 get_zd_data("0250",cCifname);
                 pub_base_strpack(cCifname);
                 vtcp_log("[%s][%d]id_no==[%s]\n",__FILE__,__LINE__,cId_no);
                 vtcp_log("[%s][%d]cCifname=[%s]\n",__FILE__,__LINE__,cCifname);
                 if(strlen(cId_no))
                 {
                    ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&f_cif_id_code_rel,"id_type='%c' and id_no='%s'", \
                                           cCif_type[0], cId_no);
                   if(ret)
                   {
                       sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
                       WRITEMSG
                       strcpy(g_pub_tx.reply,"0001");
                       goto OKExit;/*ȥ0997���ͻ� 2009/12/22 17:40:49 martin */
                   }
                   else
                   {
                     ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld",f_cif_id_code_rel.cif_no);
                     if(ret)
                     {
                      sprintf(acErrMsg,"���ҿͻ�������Ϣ�����!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P135");
                      goto ErrExit;
                     }       
                     if(!memcmp(cCifname,g_cif_basic_inf.name,strlen(g_cif_basic_inf.name)))
                     {
                         vtcp_log("[%s][%d]lCifno====[%ld]\n",__FILE__,__LINE__,f_cif_id_code_rel.cif_no); 
                         set_zd_long("0280",f_cif_id_code_rel.cif_no);
                         goto GoodExit; /*�Ѿ��ҵ���¼���سɹ� 2009/12/22 17:40:49 martin */
                     }else 
                     {
                           	sprintf(acErrMsg,"�ͻ����Ʋ�һ��!");
                      	WRITEMSG
                      	strcpy(g_pub_tx.reply,"CD26");
                      	goto ErrExit;
                     }
                   }
                }
                break;
             default:
                   vtcp_log("[%s][%d]error ! cCif_type=[%s]\n",__FILE__,__LINE__,cCif_type);
                   sprintf(acErrMsg,"����ĸ���ҵ������!! ");
                   WRITEMSG
                   strcpy(g_pub_tx.reply,"0999");
                   goto ErrExit;
    	  }
    }
    else if(cTxtype[0]=='2')/**��ѯ�Թ��Ŀͻ�id**/
    {
        if(cDGtmp_type[0]=='8' ||cDGtmp_type[0]=='A')
        {
        	vtcp_log("[%s][%d]cZjtype==[%s]\n",__FILE__,__LINE__,cZjtype);
        }
        else
        {
          sprintf(acErrMsg,"����Ĺ�˾ҵ������!! ");
          WRITEMSG
          strcpy(g_pub_tx.reply,"0999");
          goto ErrExit;
    	  }
    	  
    	  if(strlen(cDGtmp_idno)==0)
    	  {
    	    vtcp_log("[%s][%d]MS����id_no=[%s]����Ϊ��\n",__FILE__,__LINE__,cId_no);
    	    strcpy(g_pub_tx.reply,"0999");
    	    goto ErrExit;
    	  }
        else/*ID_NO�жϳ��Ȳ�Ϊ��*/
        {
             vtcp_log("[%s][%d]id_no=[%s],cDGtmp_type=[%c]\n",__FILE__,__LINE__,cDGtmp_idno,cDGtmp_type[0]);
             ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&f_cif_id_code_rel,"id_type='%c' and id_no='%s'", \
                                 cDGtmp_type[0], cDGtmp_idno);
             if(ret!=100 && ret!=0)
             {
             	sprintf(acErrMsg,"��������ʺź�֤�����ʹ���!! ");
             	WRITEMSG
             	strcpy(g_pub_tx.reply,"0999");
             	goto ErrExit;
             }
             else if(ret==100)
             {
             	vtcp_log("[%s][%d]û���ҵ���Ӧ�ļ�¼",__FILE__,__LINE__,ret);
             	sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
             	strcpy(g_pub_tx.reply,"0001");
             	goto OKExit;/*ȥ0997���ͻ� 2009/12/22 17:40:49 martin */
             }
             else
             {
             	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld",f_cif_id_code_rel.cif_no);
             	if(ret){
             		sprintf(acErrMsg,"���ҿͻ�������Ϣ�����!");
             		WRITEMSG
             		strcpy(g_pub_tx.reply,"P135");
             		goto ErrExit;
             	}
             	if(g_cif_basic_inf.sts[0]=='2')
             	{
             		vtcp_log("[%s][%d]û���ҵ���Ӧ�ļ�¼",__FILE__,__LINE__,ret);
             		sprintf(acErrMsg,"�ͻ���Ϊע��״̬!! ");
             		strcpy(g_pub_tx.reply,"C225");
             		goto ErrExit;
             	}
             	vtcp_log("%s,%d,�����ͻ����뵽����ʲô[%s]",__FILE__,__LINE__,cCifname);
             	if(!memcmp(cCifname,g_cif_basic_inf.name,strlen(g_cif_basic_inf.name)))/*�ȽϿͻ�����**/
             	{
                 vtcp_log("[%s][%d]lCifno====[%ld]\n",__FILE__,__LINE__,f_cif_id_code_rel.cif_no); 
                 set_zd_long("0280",f_cif_id_code_rel.cif_no);
                 set_zd_data("0160",TX_CODE);
                 goto GoodExit;/*�Ѿ��ҵ���¼���سɹ� 2009/12/22 17:40:49 martin */
              }else 
              {
                  sprintf(acErrMsg,"�ͻ����Ʋ�һ��!");
                  WRITEMSG
                  strcpy(g_pub_tx.reply,"CD26");
                  goto ErrExit;
              }
             }
         vtcp_log("[%s][%d]���¿ͻ�֤����ͻ��Ŷ��ձ�ɹ�\n",__FILE__,__LINE__);
     }
     vtcp_log("[%s][%d]id_no====[%s]\n",__FILE__,__LINE__,cId_no);
    }
GoodExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OKExit:
    sprintf(acErrMsg,"Before OK1 return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit:
    sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 2;
}

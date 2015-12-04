/****************************************************
* �� �� ��:    sp6708.c
* ��������:    �����ʺŻ���֤��֤�����ͺ�֤����
*
* ��    ��:    wanglei
* ������ڣ�   2007��05��22��
* �޸ļ�¼��   
*     1. ��    ��: 2008/12/8 9:56
*        �� �� ��: MARTIN
*        �޸�����: �����������ͻ��� ���Ӳ������ݿͻ��Ų�ѯ֤�����ͻ�����
*                 
*****************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"

sp6708()  
{
	int  ret=0;
	long cif_no=0;
	char do_typ[2];
	char cif_typ[2];
	char ac_no[20];
	char cId_type[2];   /****֤������****/
	char cId_no[21];    /****֤������****/
	char name[61];      /*�ͻ�����*/
	
	struct dd_mst_c               g_dd_mst;
	struct td_mst_c               g_td_mst;
	struct in_mst_c               g_in_mst;
	struct prdt_ac_id_c           sPrdt_ac_id;
	struct cif_basic_inf_c        sCif_basic_inf;
	struct cif_id_code_rel_c      sCif_id_code_rel;

	/** ȡֵ����ֵ **/	
	get_zd_data("0670",do_typ);							/*���� 1: �ʺŲ�ѯ  ; 2: �ͻ��Ų�ѯ*/
	if(do_typ[0]=='1')/*�����ʺŲ�ѯ*/
	{
	   get_zd_data("0300",ac_no);							/*�ʺ�*/
	   get_zd_int ("0340",&g_pub_tx.ac_seqn); /*�˻����*/
	   
	   ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	   if( ret==100 )
	   {
	       sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",ac_no);
	       WRITEMSG
	    	strcpy(g_pub_tx.reply,"M003");
	   	goto ErrExit;
	   }
	   else if(ret)
	   {
	   	sprintf(acErrMsg,"���ҽ����ʻ���ϵ�����! ret=[%d]",ret);
	   	WRITEMSG
	   	strcpy(g_pub_tx.reply,"D103");
	   	goto ErrExit;
	   }
	   
	   memset( &sPrdt_ac_id, 0x00, sizeof(sPrdt_ac_id) );
	   ret = Prdt_ac_id_Sel( g_pub_tx.reply, &sPrdt_ac_id, \
		 "ac_id = '%ld' order by ac_id_type", g_mdm_ac_rel.ac_id );
	   if( ret != 0 )
	   {
	   	strcpy( g_pub_tx.reply,"W011" );
	   	goto ErrExit;
	   }
	   
	   /* �ʻ���Ϣ */
	   /*�����˻����͵���Ӧ�ı���������*/
	   switch( sPrdt_ac_id.ac_id_type[0] )
	   {
	   	case '1':                                          /*1-����*/
	   			/**ret=Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id = %ld  and ac_seqn = %d", \
	   							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);**/
	   			ret=Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
	   			if( ret==100 )
	   			{
	    				sprintf(acErrMsg,"�ʻ�������!");
	     				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	       			goto ErrExit;
	   			}
	   			else if( ret )
	   			{
	   				sprintf(acErrMsg,"���һ����������[%d]",ret);
	   				WRITEMSG
	   				strcpy(g_pub_tx.reply,"D103");
	   				goto ErrExit;
	   			}
	   			strcpy(name,g_dd_mst.name);
	   			
	   			vtcp_log("[%s][%d] g_dd_mst.name=[%s] \n",__FILE__,__LINE__,g_dd_mst.name);
	   			break;
	   case '2':												/*2-����*/
	   			ret=Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id = %ld  and ac_seqn = %d", \
	   							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	   			if( ret==100 )
	   			{
	    				sprintf(acErrMsg,"�ʻ�������!");
	     				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	       			goto ErrExit;
	   			}
	   			else if( ret )
	   			{
	   				sprintf(acErrMsg,"���Ҷ����������[%d]",ret);
	   				WRITEMSG
	   				strcpy(g_pub_tx.reply,"D103");
	   				goto ErrExit;
	   			}
	   			strcpy(name,g_td_mst.name);
	   			
	   			vtcp_log("[%s][%d] g_td_mst.name=[%s] \n",__FILE__,__LINE__,g_td_mst.name);
	   			break;
	   	case '9':
	   			ret=In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id = %ld  and ac_seqn = %d", \
	   							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	   			if( ret==100 )
	   			{
	    				sprintf(acErrMsg,"�ʻ�������!");
	     				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	       			goto ErrExit;
	   			}
	   			else if( ret )
	   			{
	   				sprintf(acErrMsg,"�����ڲ��������[%d]",ret);
	   				WRITEMSG
	   				strcpy(g_pub_tx.reply,"D103");
	   				goto ErrExit;
	   			}
	   			strcpy(name,g_in_mst.name);
	   			
	   			vtcp_log("[%s][%d] g_in_mst.name=[%s] \n",__FILE__,__LINE__,g_in_mst.name);
	   			break;
	   	default:
	   			break;
	   } 
	   /*switch����*/
	   
	   
     /* �ͻ�������Ϣ */
	   memset( &sCif_basic_inf, 0x00, sizeof(sCif_basic_inf) );
	   ret = Cif_basic_inf_Sel( g_pub_tx.reply, &sCif_basic_inf, \
	   	"cif_no = '%ld' and sts='1'", sPrdt_ac_id.cif_no );
	   if( ret != 0 )
	   {
	   	strcpy( g_pub_tx.reply,"W011" );
	   	goto ErrExit;
	   }
     
     /* ���ݿͻ��Ų�ѯ�ͻ�֤�� */	
	   ret = Cif_id_code_rel_Sel(g_pub_tx.reply, &sCif_id_code_rel, 
		 "cif_no=%ld", sPrdt_ac_id.cif_no);
	   if (ret != 0 && ret != 100)
	   {
	   	sprintf(acErrMsg, "�ͻ�֤����ͻ��Ŷ��ձ����!! [%d]", ret);
	   	WRITEMSG
	   	strcpy(g_pub_tx.reply, "C002");
	   	goto ErrExit;
	   }
	   else if (ret == 100)
	   {
	   	sprintf(acErrMsg, "�ͻ�֤����ͻ��Ŷ��ձ�û�д˼�¼!!");
	   	WRITEMSG
	   	strcpy(g_pub_tx.reply, "B115");
	   	goto ErrExit;
	   }
	   
	   sprintf(acErrMsg,"ac_id is [%ld];cif_no is [%ld]-------------", g_mdm_ac_rel.ac_id, sPrdt_ac_id.cif_no );
	   WRITEMSG
     
	   cif_no = sPrdt_ac_id.cif_no;
	
	   /****
	   if( pub_cif_GetCifType ( cif_no , &cif_typ ) )
	   {
	   	sprintf(acErrMsg,"���ҿͻ�����ʧ��!");
	   	WRITEMSG
	   	goto ErrExit;
	   }
	   ****/
	   strcpy(cif_typ,sCif_basic_inf.type);  /****�ڴ˴����ͻ����͸�ֵ�����溯����������****/
	   
	   strcpy(cId_type,g_mdm_ac_rel.id_type);
	   strcpy(cId_no,g_mdm_ac_rel.id_no);
	   
	   /* ������� */
	   set_zd_data("0200",sCif_id_code_rel.id_type);/*֤������*/
	   set_zd_data("0250",sCif_basic_inf.name);     /*�ͻ�����*/
	   set_zd_data("0260",name);                    /*�ʻ��ͻ�����*/
	   set_zd_long("0290",cif_no);                  /*�ͻ���*/
	   set_zd_data("0620",sCif_id_code_rel.id_no);  /*֤������*/
	   set_zd_data("0630",cId_no);                  /*�ʻ�֤������*/
	   set_zd_data("0680",cif_typ);                 /*�ͻ�����*/
	   set_zd_data("0690",cId_type);                /*�ʻ�֤������*/
	   
	}else
	if(do_typ[0]=='2')/*���ݿͻ��Ų�ѯ*/
	{
		get_zd_long("0280",&cif_no);							/*�ͻ���*/
		get_zd_data("0680",cif_typ);							/*�ͻ�����*/
		
		memset (&sCif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
		memset (&sCif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	  sprintf(acErrMsg,"�ͻ���[%d]",cif_no);
	  WRITEMSG
    
	  					  
	  /* ���ݿͻ��Ų�ѯ�ͻ����ͺ����� */			  
	  ret = Cif_basic_inf_Sel( g_pub_tx.reply , &sCif_basic_inf , 
	  				" cif_no=%ld and sts='1' " , cif_no );
	  if( ret==100 )
	  {
	  	sprintf(acErrMsg,"�ÿͻ��Ų�����[%s][%ld]",g_pub_tx.reply,cif_no);
	  	WRITEMSG
	  	strcpy(g_pub_tx.reply,"C007");
	  	goto ErrExit;
	  }
	  else if( ret )
	  {
	  	sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
	  	WRITEMSG
	  	strcpy(g_pub_tx.reply,"C006");
	  	goto ErrExit;
	  }
	  
	  /*�жϿͻ������Ƿ�һ��*/
	  if(sCif_basic_inf.type[0]!=cif_typ[0])
	  	{
	  		sprintf(acErrMsg,"�ÿͻ����Ͳ�����[%s]cif_no=[%ld]cif_type=[%s]",g_pub_tx.reply,cif_no,sCif_basic_inf.type);
	  	  WRITEMSG
	  		strcpy(g_pub_tx.reply,"D112");
	  	  goto ErrExit;
	  	}
	  
	  /* ���ݿͻ��Ų�ѯ�ͻ�֤�� */	
	  ret = Cif_id_code_rel_Sel(g_pub_tx.reply, &sCif_id_code_rel, 
		"cif_no=%ld", cif_no);
	  if (ret != 0 && ret != 100)
	  {
	  	sprintf(acErrMsg, "�ͻ�֤����ͻ��Ŷ��ձ����!! [%d]", ret);
	  	WRITEMSG
	  	strcpy(g_pub_tx.reply, "C002");
	  	goto ErrExit;
	  }
	  else if (ret == 100)
	  {
	  	sprintf(acErrMsg, "�ͻ�֤����ͻ��Ŷ��ձ�û�д˼�¼!!");
	  	WRITEMSG
	  	strcpy(g_pub_tx.reply, "B115");
	  	goto ErrExit;
	  }
	  
	   set_zd_data("0200",sCif_id_code_rel.id_type);       /*֤������*/
	   set_zd_data("0250",sCif_basic_inf.name);           /*�ͻ�����*/
	   set_zd_data("0620",sCif_id_code_rel.id_no);         /*֤������*/
		
	}
		

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


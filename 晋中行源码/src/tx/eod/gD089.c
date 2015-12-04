/*************************************************************
* �� �� ��: gD089.c
* �������������գ�����ǷϢ����90��ת����
*
* ��    ��: lance
* �������: 2003��3��26��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_holiday_c.h"
#include "ln_reg_c.h"
#include "com_sect_acm_c.h"
#include "com_branch_c.h"
#include "com_item_c.h"
#include "dc_acc_rel_c.h"
int pub_base_CalAcm_Ln( long beg_date,long end_date, char *day_type, char *acm_type, double bal,double amt,double *acm
   ,char *intst_type,long intst_moon,long intst_date );


FILE		*fp,*fp1;
int			page=0,page1=0;
struct  com_branch_c g_com_branch;
struct  com_item_c   g_com_item;
struct  dc_acc_rel_c g_dc_acc_rel;
#define ERR_DEAL if(ret){\
                sprintf(acErrMsg,"[%s],[%d],sqlcode=[%d]",__FILE__,__LINE__,ret);\
                WRITEMSG \
                goto ErrExit;\
               } 
#define FOPEN  1
#define FCLOSE 0

gD089()
{

    int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0;
    int    ac_seqn=0,parm_seqn=0;
    int    fpind=0;
     char	gx_brno[9];
    double amt_reg=0.00,amt=0.00;
    double intst_acm=0.00,in_acm=0.00,out_acm=0.00,cmpd_acm=0.00;
	double negamt=0.00;
    char ln_ac_no[20],dd_ac_no[20],flag[2],parm_code[6],intst_sts[2];  
    long day_tmp=0,be_date=0;
    struct S_ln_intst vln_intst[LN_INTST_TYPS];
    struct com_holiday_c com_holiday;
    struct com_sect_acm_c com_sect_acm_tmp;
    struct ln_reg_c ln_reg_tmp;
	memset(gx_brno, 0x0, sizeof(gx_brno));
    memset( vln_intst,0,sizeof(vln_intst) );    
    memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
    memset(&ln_reg_tmp, 0x00, sizeof(struct ln_reg_c));
    memset(&com_sect_acm_tmp, 0x00, sizeof(struct com_sect_acm_c));  
     
    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /* ��ʼ��ȫ�ֱ��� */
    pub_base_sysinit();
    
    if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
	{
		sprintf(acErrMsg,"ȡ������ˮ�Ŵ� [%d]",g_pub_tx.trace_no);
		WRITEMSG
		goto ErrExit;
	}
	set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
	set_zd_long( DC_TRACE_CNT,1 );
    /* ��com_parm��ȡ��ǷϢ��������� */
    strcpy( parm_code , "L0002" );
    parm_seqn = 1 ;
	
    ret = pub_base_GetParm_long( parm_code , parm_seqn , &day_tmp );
    if( ret )
    {
        sprintf(acErrMsg,"���ú���ȡǷϢ�������������!");
        WRITEMSG
        return 1;	
    }	
    sprintf(acErrMsg,"���ú���ȡǷϢ��������� PASS![%ld]",day_tmp);
    WRITEMSG
    
    /* ���� */
    be_date = pub_base_daynumSUB( g_pub_tx.tx_date , day_tmp );
	sprintf(acErrMsg,"����[%ld]",be_date);
	WRITEMSG

    /* ���������ļ�(״̬Ϊ������������ */
    ret = Eod_ln_mst_Dec_Upd(g_pub_tx.reply," ac_sts in('1','2','5') and in_lo_intst > 0  order by opn_br_no" );/*gujy tested*/
    if (ret)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    while(1)
    {
	TRACE
      	amt_reg=0.00;
      	intst_acm=0.00,in_acm=0.00,out_acm=0.00,cmpd_acm=0.00;
      	memset(&g_ln_mst,0x00,sizeof(g_ln_mst));
      	ret = Eod_ln_mst_Fet_Upd(&g_ln_mst, g_pub_tx.reply);
		    if(ret&&ret!=100)
        {
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }       
        else if(ret == 100)
        {
            break;
        }   
/*************************************************
	if(0 != strcmp(gx_brno, g_ln_mst.opn_br_no))
	{
			if(0 != strlen(gx_brno))
			{
					fclose(fp);
					fclose(fp1);
			}
			if(openfile(g_ln_mst.opn_br_no))
			{
					ERR_DEAL
			}
			if(openfile1(g_ln_mst.opn_br_no))
			{
					ERR_DEAL
			}                        
			strcpy(gx_brno, g_ln_mst.opn_br_no);
			memset(&g_com_branch,0x00,sizeof(g_com_branch));
			if(ret = Com_branch_Sel(g_pub_tx.reply, &g_com_branch, "br_no = '%s'", gx_brno))
			{
					ERR_DEAL
			}
			page=0;
			page1=1;
	}
**************************************************/
        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) ); /***JYW**/

        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld", 
												g_ln_mst.ac_id);
        if(ret==100)
        {
            sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
        }else if( ret&&ret!=100){
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
        }
        strcpy( ln_ac_no ,g_mdm_ac_rel.ac_no );
/**JYW  memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) ); JYW END***/
        
        /* ȡ������Ϣ */
        ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
	if( ret ) goto ErrExit;
	/*���׻�����ֵ*/
	strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);

sprintf(acErrMsg,"look this ONE ret==[%d]",ret);
		WRITEMSG
	/* �жϽڼ����Ƿ��� */
	ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
	if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ) continue;
	TRACE
sprintf(acErrMsg,"look this TWO ret==[%d]",ret);
		WRITEMSG

      	/* ȡ����״̬ */
		sprintf(acErrMsg,"[%ld]",g_ln_mst.ac_id);
		WRITEMSG

		if( g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='5')
									/* ֻ�������������ڴ��� */
		{
		TRACE
			continue;
		}
		TRACE

		/* ��ѯǷ������Ƿ��з��������ļ�¼ */
		/* ret=Check_InIntst(g_ln_mst , g_ln_parm ); */
		ret = 0;
		TRACE
		if(!ret)
		{
	    	/* ��ѯ����ǼǱ� */
            ret = Ln_reg_Dec_Upd(g_pub_tx.reply," ac_id=%ld and \
						ac_seqn=%d and type='2' and sts='0' and reg_date_beg<=%ld ",
								g_ln_mst.ac_id,g_ln_mst.ac_seqn,be_date);
            if (ret != 0)
            {
            	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
            }
             while(1)
            {
      	         ret = Ln_reg_Fet_Upd(&ln_reg_tmp, g_pub_tx.reply);
                 if (ret != 0 && ret != 100)
                 {
                     sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
                     WRITEMSG
                     goto ErrExit;
                 }else if (ret == 100){
                      break;
                 }
                 /* �ۼƽ�� */
                 amt_reg+=ln_reg_tmp.lo_bal-ln_reg_tmp.pay_bal;
                 strcpy(ln_reg_tmp.type,"3");
                 
                 ret = Ln_reg_Upd_Upd( ln_reg_tmp, g_pub_tx.reply  );
                 if ( ret )
                 {
                      sprintf( acErrMsg,"Ln_reg_Upd_Upd ����[%d]",ret);
	              	  WRITEMSG
                      return 1;
                 }		                 
            } 
			Ln_reg_Clo_Upd( );
	    sprintf(acErrMsg,"���´���ǼǱ� PASS");
	    WRITEMSG  
		TRACE
	               	
	    /* ���·ֶλ����� */
            ret = Com_sect_acm_Dec_Upd(g_pub_tx.reply," ac_id=%ld and ac_seqn=%d and intst_type='2' and sts='0' and beg_date<=%ld",g_ln_mst.ac_id,g_ln_mst.ac_seqn,be_date);
            if (ret != 0)
            {
            sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
            }
            while(1)
            {
			TRACE
      	         ret = Com_sect_acm_Fet_Upd(&com_sect_acm_tmp, g_pub_tx.reply);
                 if (ret != 0 && ret != 100)
                 {
                     sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
                     WRITEMSG
                     goto ErrExit;
                 }       
                 else if (ret == 100)
                 {
                      break;
                 }
                 /* �޸�"����"--->"����" */
                 strcpy(com_sect_acm_tmp.intst_type,"3");
                 
                 ret = Com_sect_acm_Upd_Upd( com_sect_acm_tmp, g_pub_tx.reply );
                 if ( ret )
                 {
                      sprintf( acErrMsg,"Com_sect_acm_Upd_Upd ����[%d]",ret);
	              WRITEMSG
                      return 1;
                 }		                 
            } 
	    	Com_sect_acm_Clo_Upd( );
	    	sprintf(acErrMsg,"���·ֶλ����� PASS");
	    	WRITEMSG             		                		    
           /* �޸Ĵ������ļ�,������ */
            /* ���㡢���´������ļ������л��� */
			strcpy( intst_sts , "2" );
            /* ���� */
/*sprintf(acErrMsg, "%d   %s    bal[%lf]    intst_acm [%lf][%ld]", __LINE__, __FILE__, g_ln_mst.bal, intst_acm, (long)&intst_acm);
	              WRITEMSG*/
			/*pub_base_CalAcm_tt( g_ln_mst.lst_date,g_pub_tx.tx_date,
              g_ln_parm.intst_days_type,"2",g_ln_mst.bal,0,&intst_acm,"0",0,0 );*/


            ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
			g_ln_parm.intst_days_type,"2",g_ln_mst.bal,0,&intst_acm,"0",0,0 );
            if( ret )
            {
                 sprintf(acErrMsg,"���ú������㱾���������!");
                 WRITEMSG
                 return -1;	
            }
            g_ln_mst.intst_acm+=intst_acm;
			g_ln_mst.intst_acm=pub_base_PubDround_1(g_ln_mst.intst_acm);
			TRACE
            /* ���� */         
            ret =  pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date, 
					g_ln_parm.intst_days_type,intst_sts,g_ln_mst.in_lo_intst,
					0,&in_acm,"0",0,0 );
            if( ret )
            {
                 sprintf(acErrMsg,"���ú���������ڻ�������!");
                 WRITEMSG
                 return -1;	
            }
			sprintf(acErrMsg,"[%lf][%lf]",g_ln_mst.in_lo_acm , in_acm);
			WRITEMSG
		
            g_ln_mst.in_lo_acm+=in_acm;            
			g_ln_mst.in_lo_acm=pub_base_PubDround_1(g_ln_mst.in_lo_acm);
			TRACE
            /* ���� */
			       ret =  pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
					g_ln_parm.intst_days_type,intst_sts,g_ln_mst.out_lo_intst,
					0,&out_acm,"0",0,0 );
            if( ret )
            {
                 sprintf(acErrMsg,"���ú�����������������!");
                 WRITEMSG
                 return -1;	
            }
			       g_ln_mst.out_lo_acm+=out_acm;               
			g_ln_mst.out_lo_acm=pub_base_PubDround_1(g_ln_mst.out_lo_acm);
			TRACE
            /* ���� */
			      ret =  pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
					g_ln_parm.intst_days_type,intst_sts,g_ln_mst.cmpd_lo_intst,
					0,&cmpd_acm,"0",0,0 );
            if( ret )
            {
                 sprintf(acErrMsg,"���ú������㸴����������!");
                 WRITEMSG
                 return -1;	
            }                      
			TRACE
            g_ln_mst.cmpd_lo_acm+=cmpd_acm;             
			g_ln_mst.cmpd_lo_acm=pub_base_PubDround_1(g_ln_mst.cmpd_lo_acm);
			TRACE
            /* �޸ı��ڡ�������Ϣ */
            	/*****START 2007-5-30 15:04******/
            	g_ln_mst.out_lo_intst+=amt_reg;                        
              g_pub_tx.tx_amt2=amt_reg;
              g_pub_tx.tx_amt3=amt_reg;            
              g_ln_mst.in_lo_intst-=amt_reg;
              g_ln_mst.lst_date=g_pub_tx.tx_date;
              /*****END*******/
			TRACE
		 g_ln_mst.cmpd_lo_acm=pub_base_PubDround_1( g_ln_mst.cmpd_lo_acm);
		 g_ln_mst.in_lo_intst=pub_base_PubDround_1(g_ln_mst.in_lo_intst);
		 g_ln_mst.out_lo_intst=pub_base_PubDround_1(g_ln_mst.out_lo_intst);
		 g_ln_mst.cmpd_lo_intst=pub_base_PubDround_1(g_ln_mst.cmpd_lo_intst);

		 sprintf(acErrMsg,"[%lf][%lf][%lf][%lf]",g_ln_mst.cmpd_lo_acm,
		 g_ln_mst.in_lo_intst,g_ln_mst.out_lo_intst,g_ln_mst.cmpd_lo_intst);
		 WRITEMSG

            /* ����ˮ�ͼ��� */
            if( pub_base_CompDblVal( g_pub_tx.tx_amt2 ,0.00 ) )
            {
				 /*g_pub_tx.tx_amt2=0-g_pub_tx.tx_amt2;JYW 133�Ǹ���,���������Ǽ�,���Բ��ܼǸ���������*/
                 strcpy( g_pub_tx.ac_no , ln_ac_no);
                 g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
	        	 strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
	        	 strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	         	 strcpy( g_pub_tx.add_ind,"0" );	/* ����:0��1��2���� */	
	         	 strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	         	 strcpy( g_pub_tx.prt_ind,"0" );	/*�Ƿ��ӡ����(�������۱�־)
													: 1��0�� */
	         	 strcpy( g_pub_tx.hst_ind,"1" );	/*����ϸ:1�ռ���2���յ�����
															 3���ջ�����*/     
                 strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	         	 g_pub_tx.svc_ind=3020;	
                 strcpy ( g_pub_tx.brf,"����ǷϢת����" );
                 strcpy(g_pub_tx.sub_tx_code,"G089");
	         	 if( pub_acct_trance() )
	         	 {
                  	sprintf(acErrMsg,"���׼��ʴ������!");
                  	WRITEMSG
                  	goto ErrExit;
	         	 }

				 /*g_pub_tx.tx_amt2=0-g_pub_tx.tx_amt2;JYW ǰ��133�Ǹ���i,705�ĳ�����,ǰ�治����,�������ﲻ�ø���*/
                 strcpy( g_pub_tx.ac_no , ln_ac_no);
                 g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
	        	 strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
	         	 strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	         	 strcpy( g_pub_tx.add_ind,"1" );	/* ����:0��1��2���� */	
	         	 strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	         	 strcpy( g_pub_tx.prt_ind,"0" );	/*�Ƿ��ӡ����(�������۱�־
														):1��0�� */
	         	 strcpy( g_pub_tx.hst_ind,"1" );	/*����ϸ:1�ռ���2���յ�����
															 3���ջ�����*/      
                 strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	         	 g_pub_tx.svc_ind=3030;	
                 strcpy ( g_pub_tx.brf,"����ǷϢת����" );

	         	 if( pub_acct_trance() )
	         	 {
                 	sprintf(acErrMsg,"���׼��ʴ������!");
                  	WRITEMSG
                  	goto ErrExit;
	         	 }
				 TRACE
	         	         
	         	 /* ���л�Ƽ��� */
                 set_zd_data("0210",g_ln_parm.cur_no);    
				 negamt=-g_pub_tx.tx_amt2;/*JYW*/
                 set_zd_double("0430",negamt);                     
                 set_zd_double("0420",g_pub_tx.tx_amt2);                     
                 set_zd_data(DC_CODE,g_ln_parm.dc_code);/*****����,����Ϊ�ͱ���һ�µ�DC_CODE********/
                 strcpy(g_pub_tx.sub_tx_code,"G089");
                 strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
                 ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
                 if (ret != 0)
                 {
                    sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
                    WRITEMSG
                    goto ErrExit;
                 }
	     	}
	   }
	   TRACE
	   sprintf(acErrMsg,"[%lf][%lf][%lf][%lf]",g_ln_mst.cmpd_lo_acm,
			g_ln_mst.in_lo_intst,g_ln_mst.out_lo_intst,g_ln_mst.cmpd_lo_intst);
   	   WRITEMSG

       ret = Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
       if (ret != 0)
       {
            sprintf(acErrMsg,"���´������ļ�����!!");
            WRITEMSG
            goto ErrExit;
       }
 /*************************************************/
	if(0 != strcmp(gx_brno, g_ln_mst.opn_br_no) || fpind==FCLOSE)
	{
			if(0 != strlen(gx_brno))
			{
					fclose(fp);
					fclose(fp1);
			}
			fpind=FCLOSE;
			if(amt_reg>0)
			{
			  if(openfile(g_ln_mst.opn_br_no))
			  {
			  		ERR_DEAL
			  }
			  if(openfile1(g_ln_mst.opn_br_no))
			  {
			  		ERR_DEAL
			  }
			  fpind=FOPEN;
			}                        
			strcpy(gx_brno, g_ln_mst.opn_br_no);
			memset(&g_com_branch,0x00,sizeof(g_com_branch));
			if(ret = Com_branch_Sel(g_pub_tx.reply, &g_com_branch, "br_no = '%s'", gx_brno))
			{
					ERR_DEAL
			}
			page=0;
			page1=1;
	}
/**************************************************/
       /***********************************************************/
       if(amt_reg>0)
       {
		   char ln_name1[41];
		   char ln_name2[21];
		   memset(ln_name1,0x00,sizeof(ln_name1));
		   memset(ln_name2,0x00,sizeof(ln_name2));
		   memcpy(ln_name1,g_ln_mst.name,40);
		   memcpy(ln_name2,g_ln_mst.name+40,20);
		   memset(&g_dc_acc_rel,0x00,sizeof(g_dc_acc_rel));
		   memset(&g_com_item,0x00,sizeof(g_com_item)); 
       if (g_ln_mst.ac_sts[0]!='1')
		      ret=Dc_acc_rel_Sel(g_pub_tx.reply,&g_dc_acc_rel,"dc_code='%s' and data_code='0158'",g_ln_parm.over_dc_code);
       else
		      ret=Dc_acc_rel_Sel(g_pub_tx.reply,&g_dc_acc_rel,"dc_code='%s' and data_code='0158'",g_ln_parm.dc_code);
		   if(ret)ERR_DEAL
		   ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_hrt='%s'",g_dc_acc_rel.acc_hrt);
		   if(ret)ERR_DEAL
		   printf( "\n                                                   ����ǷϢ����90��ת���⴫Ʊ\n");
			 fprintf(fp, "\n                                  %c%c%c%c%-20s%c%c%c%c\n\n\n\n\n", 0x1B, 0x67, 0x1C, 0x0E,"����ǷϢ����90��ת���⴫Ʊ", 0x1C, 0x14, 0x1B, 0x50);
			fprintf(fp, "   ����������������������������������������������������������������������������������������������������������\n" );
			fprintf(fp, "   �� �� �� ��: %-5s                                             �� ˮ ��: %-6d                         ��\n",g_mdm_ac_rel.opn_br_no,g_pub_tx.trace_no);
			fprintf(fp, "   �� �����ʺ�: %-20s                              ��ת����: %-ld                       ��\n", g_mdm_ac_rel.ac_no,g_pub_tx.tx_date);
			fprintf(fp, "   �� ��    ��: %-40s          ��ת���: %-17.2lf              ��\n", ln_name1,amt_reg);
			fprintf(fp, "   ��           %-20s                              ǷϢ����: %-ld                       ��\n",ln_name2,ln_reg_tmp.reg_date_beg);
		  fprintf(fp, "   ����������������������������������������������������������������������������������������������������������\n");
			fprintf(fp, "   ��                                                                                                      ��\n",ln_name2);
			fprintf(fp, "   �� ��Ŀ(��):13301                       ���: -%-17.2f                                        ��\n",amt_reg);
			fprintf(fp, "   ��                                                                                                      ��\n",ln_name2);
			fprintf(fp, "   �� ��Ŀ(��):%s                       ���: -%-17.2f                                        ��\n",g_com_item.acc_no,amt_reg);
			fprintf(fp, "   ��                                                                                                      ��\n",ln_name2);		
			fprintf(fp, "   �� ��Ŀ(��):70501                       ���:  %-17.2f                                        ��\n",amt_reg);
			fprintf(fp, "   ��                                                                                                      ��\n");
			fprintf(fp, "   ��                                                                                                      ��\n");
			fprintf(fp, "   ����������������������������������������������������������������������������������������������������������\n");
			fprintf(fp, "               ��    ��:                             ��    ��:                                ��    ��: %s\n",  "9999");
			fprintf(fp, "\n\n\n\n\n\n\n\n\n\n");
			printf( "\n                                                   ����ǷϢ����90��ת�����嵥\n");
			if( page==0 )
				{
					fprintf(fp1, "                                               ����ǷϢ����90��ת�����嵥\n\n");
				  fprintf(fp1, "  ��������: %-30s%-5s                 ��������: %ld      �� %2d ҳ\n",g_com_branch.br_name,g_mdm_ac_rel.opn_br_no,g_pub_tx.tx_date,page1);
				  fprintf(fp1, "  ======================================================================================================\n");
				  fprintf(fp1, "   ��ˮ��     �� �� �� ��                    ��       ��                    ��ת���            ǷϢ����\n");
				  fprintf(fp1, "  ------------------------------------------------------------------------------------------------------\n");
				  page1++;
				}
			fprintf(fp1, "  %6d  %-20s  %-40s   %17.2lf    %-ld\n",g_pub_tx.trace_no ,g_mdm_ac_rel.ac_no ,ln_name1 ,amt_reg ,ln_reg_tmp.reg_date_beg);
			page++;
		if(!(page%2)){
		fprintf(fp, "\f\n");
				}
			if(!(page%50)){
		      fprintf(fp1, "\f\n");
		      fprintf(fp1, "                                               ����ǷϢ����90��ת�����嵥\n\n");
				  fprintf(fp1, "  ��������: %-30s%-5s                 ��������: %ld      �� %2d ҳ\n",g_com_branch.br_name,g_mdm_ac_rel.opn_br_no,g_pub_tx.tx_date,page1);
				  fprintf(fp1, "  ======================================================================================================\n");
				  fprintf(fp1, "   ��ˮ��     �� �� �� ��                    ��       ��                    ��ת���            ǷϢ����\n");
				  fprintf(fp1, "  ------------------------------------------------------------------------------------------------------\n");
				  page1++;
				}
       }
/***********************************************************/
		 }
    
    Eod_ln_mst_Clo_Upd();

OkExit:
	sql_commit();	/*--�ύ����--*/
	fclose(fp);
	fclose(fp1);
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���մ������ϢǷϢ�����涨����ת���⴦�����ɹ�!![%s]",
						g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* ���޸� */
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���մ������ϢǷϢ�����涨����ת���⴦�����ʧ��!![%s]",
						g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
int openfile (char tmpjgbm[10])
{
        char fullname[100];
        memset(fullname,0x00,sizeof(fullname) );
        sprintf( fullname,"%s/report/%s/D089.txt",getenv("HOME"),tmpjgbm);
        if( (fp=fopen(fullname,"w"))==NULL )
        {
                sprintf( fullname,"%s/report/%s",getenv("HOME"),tmpjgbm);
                mkdir( fullname, S_IRWXU|S_IRWXG|S_IRWXO);
                sprintf( fullname,"%s/report/%s/D089.txt",getenv("HOME"),tmpjgbm);
                if( (fp=fopen(fullname,"w"))==NULL )
                {
                        printf("open file failed,filename=%s\n",fullname);
                        return(1);
                }
        }
        return(0);
}
int openfile1 (char tmpjgbm[10])
{
        char fullname[100];
        memset(fullname,0x00,sizeof(fullname) );
        sprintf( fullname,"%s/report/%s/gD089.txt",getenv("HOME"),tmpjgbm);
        if( (fp1=fopen(fullname,"w"))==NULL )
        {
                sprintf( fullname,"%s/report/%s",getenv("HOME"),tmpjgbm);
                mkdir( fullname, S_IRWXU|S_IRWXG|S_IRWXO);
                sprintf( fullname,"%s/report/%s/gD089.txt",getenv("HOME"),tmpjgbm);
                if( (fp1=fopen(fullname,"w"))==NULL )
                {
                        printf("open file failed,filename=%s\n",fullname);
                        return(1);
                }
        }
        return(0);
}

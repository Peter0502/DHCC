/*************************************************************
* �� �� ��: gD088.c
* �������������գ������
*
* ��    ��: lance
* �������: 2003��3��24��
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

gD088()
{
    int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0;
    int    ac_seqn=0;
    int    coninue_flag=0;
    double vbnlx,vbwlx,vfllx,vbjlx;
    double amt1,amt2,amt3,amt4;
    double tx_amt1,tx_amt2,tx_amt3,tx_amt4;  
    char ln_ac_no[20],dd_ac_no[20],flag[2];
	long dd_ac_id=0;
    char type[2];
	
    char pact_no_tmp[20];
    memset(pact_no_tmp,0x00,sizeof(pact_no_tmp));
      
    struct S_ln_intst vln_intst[LN_INTST_TYPS];
    struct com_holiday_c com_holiday;
    double bj_acm=0.00;
	double bn_acm=0.00;
	double bw_acm=0.00;
	double fl_acm=0.00;
	double bj_sect_acm=0.00;
	double bn_sect_acm=0.00;
	double bw_sect_acm=0.00;
	double fl_sect_acm=0.00;
	
    	char acm_type[2];
    vbnlx=vbwlx=vfllx=vbjlx=0.00;
    amt1=amt2=amt3=amt4=0.00;
    memset( vln_intst,0,sizeof(vln_intst) );    
    memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
    
  	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /* ��ʼ��ȫ�ֱ��� */
    pub_base_sysinit();
	TRACE
	set_zd_double("0400",0.00);
	set_zd_double("0410",0.00);
	set_zd_double("0440",0.00);
	set_zd_double("1089",0.00);
	set_zd_double("107F",0.00);
	set_zd_double("102F",0.00);
	set_zd_double("108F",0.00);
	set_zd_double("108H",0.00);
	set_zd_double("108I",0.00);
	set_zd_double("1074",0.00);
	set_zd_double("1075",0.00);		
	set_zd_double("1076",0.00);	
	set_zd_double("1077",0.00);
			
	/** dong 20030819 add ln_ac_sts!='*' **/
	TRACE
	/** lance 20030829 ln_ac_sts=='1' ****/
   /** ret = Eod_ln_mst_Dec_Upd(g_pub_tx.reply,"((mtr_date>=20030826 AND mtr_date<=%ld ) OR \
								(exp_ind<>'0' AND exp_mtr_date>=20030826 AND exp_mtr_date<=%ld)) \ 
								AND ac_sts='1' and \
								prdt_no not in('3Z1','3Z2','3Z3','3Z4')",g_pub_tx.tx_date,g_pub_tx.tx_date);
    **/
    /*****modfied by ligl 2006-9-25 15:46*****/
    /**����3Y2��3Y3�������ȶ��ί�д����Ʒ 20150616 wudawei**/
    ret=Eod_ln_mst_Dec_Upd(g_pub_tx.reply,"((mtr_date=%ld) or (exp_ind<>'0' AND exp_mtr_date=%ld)) and ac_sts in('1','2') and prdt_no not in('3Z1','3Z2','3Z3','3Z4','3Y1','3Y2','3Y3','3Y4','3Y5','3Y6','3Y7','3Y8','3Y9','3YA','3YB')",g_pub_tx.tx_date,g_pub_tx.tx_date);
    /******end********/
    if (ret)
    {
    	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    while(1)
    {

		/*** ���ݳ�ʼ�� ***/
   	memset( vln_intst,0,sizeof(vln_intst) );    
		memset( dd_ac_no, 0x00, sizeof(dd_ac_no));
		memset( ln_ac_no, 0x00, sizeof(ln_ac_no));
		memset( flag, 0x00, sizeof(flag));
		dc_flag=0;
		tx_amt1=tx_amt2=tx_amt3=tx_amt4=0.00;
    amt1=amt2=amt3=amt4=0.00;
    bj_acm=0.00;
		bn_acm=0.00;
		bw_acm=0.00;
		fl_acm=0.00;
		bj_sect_acm=0.00;
		bn_sect_acm=0.00;
		bw_sect_acm=0.00;
		fl_sect_acm=0.00;
		coninue_flag=0;
		strcpy( g_pub_tx.tx_code, "G088");		
		TRACE
      	ret = Eod_ln_mst_Fet_Upd(&g_ln_mst, g_pub_tx.reply);
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
			if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
			{
            sprintf(acErrMsg,"ȡ������ˮ�Ŵ� [%d]",g_pub_tx.trace_no);
            WRITEMSG
            goto ErrExit;
			}
      g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;
			strcpy(g_pub_tx.tx_br_no,g_ln_mst.opn_br_no);
      ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld",g_ln_mst.ac_id);
      if ( ret == 100 )
      {
            sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
      }
      else if ( ret && ret!=100)
      {
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
      }
      strcpy( ln_ac_no ,g_mdm_ac_rel.ac_no );
      memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
			TRACE  
        /* ȡ������Ϣ * hao 
        ret = pub_ln_lndis_file(ln_ac_no,g_pub_tx.ac_seqn,
					&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); *��ȡ�����Ϣ*
		if( ret ) {return ret;}
		***************/
		ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
		if( ret ) 
		{
			strcpy(acErrMsg,"ȡ�����Ʒ���������!");
			WRITEMSG
			return ret;
		}
	/***
    if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
    {	
			TRACE
        	continue;
		}
	***/
		TRACE
		sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		WRITEMSG
		/* �жϽڼ����Ƿ��� */
		ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
		if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ){continue;}
		TRACE
    /* ȡ����״̬ */
    chg_flag = pub_ln_check_stsdate();
		if( chg_flag==-1 )	/* �����Ҫ�����ڴ��� */
		{
			TRACE
			/**���Ѿ�չ�ڵĲ��ҵ�ǰ����Ϊ�����յģ����в���ֶλ�������� add by ligl 2006-12-15 23:34**/
			if(g_ln_mst.exp_ind[0]!='0' && g_ln_mst.mtr_date==g_pub_tx.tx_date)
			{
				double acm_rate=0.00;
				acm_rate=g_ln_mst.rate;
				ret=pub_ln_Ins_sect_rate( g_ln_mst, g_ln_parm, acm_rate );
				if( ret )	
				{
						sprintf(acErrMsg,"�ǼǷֶλ�����ʧ��!");
						WRITEMSG
						goto ErrExit;
				}	
				/**���´��������lst_date**/
				g_ln_mst.lst_date=g_pub_tx.tx_date;
				g_ln_mst.ic_date=g_pub_tx.tx_date;
				g_ln_mst.intst_acm=0.00;
				ret=Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
				if ( ret == -239 )
				{
					sprintf( acErrMsg, "�޸Ĵ������ļ���,�����ظ���¼!" );
					WRITEMSG
					strcpy(g_pub_tx.reply,"L031");
					goto ErrExit;
				}else if(ret)
				{
					sprintf( acErrMsg,"ִ��Ln_mst_Upd_Upd��!ret=[%d]",ret );
					WRITEMSG
					goto ErrExit;
				}
				/****/	
			}
			/***end**/
			continue;
		}
		/* ��Ϣ */
		if( g_ln_mst.intst_type[0]=='0' || g_ln_parm.intst_type[0]=='0' )	/*����Ϣ*/
		{
			vbjlx=0.00; bj_acm=0.00;
			vbnlx=0.00; bn_acm=0.00;
			vbwlx=0.00; bw_acm=0.00;
			vfllx=0.00; fl_acm=0.00;
		}
		else
		{
			ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"0",&vbjlx,0 );
			if( ret ) goto ErrExit;
			
			strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
			bj_acm = g_ln_mst.intst_acm ;/* ��Ϣ���� */

			ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
								g_ln_parm.intst_days_type , acm_type , 
								g_ln_mst.bal ,g_ln_mst.bal , &bj_acm , "0",0,0);
			if( ret )
			{
				sprintf(acErrMsg,"���ú��������������!");
				WRITEMSG
				goto ErrExit;
			}
			sprintf(acErrMsg,"���ú����������PASSΪ [%lf]",bj_acm );
			WRITEMSG
			/*ͳ�Ʒֶλ������л���*/
			/* ����ֶλ�����Ϣ */
			strcpy( type , "1" ); /* 1-������� */	
			ret = pub_ln_Get_SectAcm( g_ln_mst.ac_id , g_ln_mst.ac_seqn , 
				type , &bj_sect_acm );
			if( ret )
			{
				sprintf(acErrMsg,"���ú�������ֶλ�����Ϣ����!");
				WRITEMSG
				return -1;	
			}	
			sprintf(acErrMsg,"���ú�������ֶλ�����ϢPASS[%lf]!",bj_sect_acm);
			WRITEMSG			
			if( g_ln_mst.cmpd_intst_ind[0]=='N' || g_ln_parm.cmpd_intst_ind[0]=='N')
			{
				vbnlx=0.00;
				vbwlx=0.00;
				vfllx=0.00;
			}else
			{
				ret = pub_ln_CalIntst ( g_ln_mst,g_ln_parm,"1",&vbnlx,0 );
				if( ret ) goto ErrExit;

				strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
				bn_acm = g_ln_mst.in_lo_acm ;/* ��Ϣ���� */
				ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
									g_ln_parm.intst_days_type , acm_type , 
									g_ln_mst.in_lo_intst ,g_ln_mst.in_lo_intst , &bn_acm , "0",0,0);
				if( ret )
				{
					sprintf(acErrMsg,"���ú��������������!");
					WRITEMSG
					goto ErrExit;
				}
				sprintf(acErrMsg,"���ú���������ڻ���PASSΪ [%lf]",bn_acm );
				WRITEMSG
				/* ����ֶλ�����Ϣ-���� */
				strcpy( type , "2" ); /* 2-���ڻ��� */	
				ret = pub_ln_Get_SectAcm( g_ln_mst.ac_id , g_ln_mst.ac_seqn , 
					type , &bn_sect_acm );
				if( ret )
				{
					sprintf(acErrMsg,"���ú�������ֶλ�����Ϣ����!");
					WRITEMSG
					return -1;	
				}	
				sprintf(acErrMsg,"���ú�������ֶλ�����ϢPASS[%lf]!",bn_sect_acm);
				WRITEMSG
				
				
				
				ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"2",&vbwlx,0 );
				if( ret ) goto ErrExit;
				strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
				bw_acm = g_ln_mst.out_lo_acm ;/* ��Ϣ���� */
	
				ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
									g_ln_parm.intst_days_type , acm_type , 
									g_ln_mst.out_lo_intst ,g_ln_mst.out_lo_intst , &bw_acm , "0",0,0);
				if( ret )
				{
					sprintf(acErrMsg,"���ú��������������!");
					WRITEMSG
					goto ErrExit;
				}
				sprintf(acErrMsg,"���ú�������������PASSΪ [%lf]",bw_acm );
				WRITEMSG
				/* ����ֶλ��������� */
				strcpy( type , "3" ); /* 3-������� */	
				ret = pub_ln_Get_SectAcm( g_ln_mst.ac_id , g_ln_mst.ac_seqn , 
					type , &bw_sect_acm );
				if( ret )
				{
					sprintf(acErrMsg,"���ú�������ֶλ�����Ϣ����!");
					WRITEMSG
					return -1;	
				}	
				sprintf(acErrMsg,"���ú�������ֶλ�����ϢPASS[%lf]!",bw_sect_acm);
				WRITEMSG
				
				
				
				ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"3",&vfllx,0 );
				if( ret ) goto ErrExit;
				strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
				fl_acm = g_ln_mst.cmpd_lo_acm ;/* ��Ϣ���� */
	
				ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
									g_ln_parm.intst_days_type , acm_type , 
									g_ln_mst.cmpd_lo_intst ,g_ln_mst.cmpd_lo_intst , &fl_acm , "0",0,0);
				if( ret )
				{
					sprintf(acErrMsg,"���ú��������������!");
					WRITEMSG
					goto ErrExit;
				}
				sprintf(acErrMsg,"���ú������㸴������PASSΪ [%lf]",fl_acm );
				WRITEMSG
				/* ����ֶλ��������� */
				strcpy( type , "4" ); /* 4-�������� */	
				ret = pub_ln_Get_SectAcm( g_ln_mst.ac_id , g_ln_mst.ac_seqn , 
					type , &fl_sect_acm );
				if( ret )
				{
					sprintf(acErrMsg,"���ú�������ֶλ�����Ϣ����!");
					WRITEMSG
					return -1;	
				}	
				sprintf(acErrMsg,"���ú�������ֶλ�����ϢPASS[%lf]!",fl_sect_acm);
				WRITEMSG				
				TRACE
			}
			TRACE
		}
vtcp_log("JJJJJ2222222222222222");	
		/* �жϱ�����Ϣ */
		bj_flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );/* 1����  2���� */
		TRACE
	
		/*д����Ǽǲ�  0�Ǽ�Ƿ�� 1�����ѵǼ�Ƿ�� 2�Ǽǲ�����*/
		/*********
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,"4",vbnlx+vbnlx+vfllx,"0",0);
		if( ret ) goto ErrExit;
		*******/
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vbnlx,bn_acm+bn_sect_acm,"0",0);
		if( ret ) goto ErrExit;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vbwlx,bw_acm+bw_sect_acm,"0",0);
		if( ret ) goto ErrExit;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vfllx,fl_acm+fl_sect_acm,"0",0);
		if( ret ) goto ErrExit;				
		if( bj_flag==1 )
		{
	  		ret= pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",vbjlx,bj_acm+bj_sect_acm,"0",0);
	  		if( ret ) goto ErrExit;
		}else{
	  		ret= pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",vbjlx,bj_acm+bj_sect_acm,"0",0);
	  		if( ret ) goto ErrExit;
		}
		TRACE
		/* �Խ��������Ϣ���� * del by ly �����ܼ���
		if( pub_base_CompDblVal( vbnlx+vbwlx+vfllx+vbjlx ,0.00 ) )
		{
			** ����ˮ�ͼ���(�������Ϣ����Ӧ�Ŀ�Ŀ) **
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			strcpy( g_pub_tx.ac_id_type,"3" );	* �˺����� 3-���� *
	   		strcpy( g_pub_tx.add_ind,"1" );		* ����:0��1��2���� *	
	   		strcpy( g_pub_tx.ct_ind,"2" );		* ��ת:1��2ת *	
	   		strcpy( g_pub_tx.prt_ind,"0" );*�Ƿ��ӡ����(�������۱�־):1��0��*
	   		strcpy( g_pub_tx.hst_ind,"1" );*����ϸ:1�ռ���2���յ�����
																3���ջ�����* 
			strcpy ( g_pub_tx.intst_type , "1" );	* ��Ϣ���� *
	  		g_pub_tx.svc_ind=3052;	* ��������,ǷϢ���� *
	  		if(bj_flag==1)
	  		{
	   			g_pub_tx.tx_amt2=vbjlx;
	   		}else{
	   			g_pub_tx.tx_amt3=vbjlx;
	   		}
	   		g_pub_tx.tx_amt4=vbnlx+vbwlx+vfllx;	
			strcpy ( g_pub_tx.brf,"�����Ϣ" );
			sprintf(acErrMsg,"~~~~backin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backcmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG	
	   		*** ���׼��ʴ��� ***
	   		if( pub_acct_trance() )
	   		{
           		sprintf(acErrMsg,"���׼��ʴ������!");
           		WRITEMSG
           		goto ErrExit;
	   		}
	   		sprintf(acErrMsg,"~~~~1in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
	   		*** ��Ƽ��� ***
			set_zd_data("0210",g_ln_parm.cur_no); 
			set_zd_double("0400",g_pub_tx.tx_amt2);    
			set_zd_double("0410",g_pub_tx.tx_amt3); 
			set_zd_double("1089",g_pub_tx.tx_amt4);                                     
			strcpy(g_pub_tx.sub_tx_code,"G083");
           	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
           	if (ret != 0)
           	{
           		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
           		WRITEMSG
           		goto ErrExit;
           	}
           	g_pub_tx.tx_amt2=0.00;
           	g_pub_tx.tx_amt3=0.00;
           	g_pub_tx.tx_amt4=0.00;
			set_zd_double("0400",0.00);    
			set_zd_double("0410",0.00); 
			set_zd_double("1089",0.00);         	
      }
      ***************************/
      		/*****************add by ly ���еĸ��ֻ�����Ϣ **************/
  		/*�жϱ����������ڻ��Ǳ���*/
  		if(bj_flag==1)
  		{
   			g_pub_tx.tx_amt2=vbjlx;
   		}else{
   			g_pub_tx.tx_amt3=vbjlx;
   		}
	   	g_pub_tx.tx_amt4=vbnlx+vbwlx+vfllx;	
	   		
vtcp_log("JJJJJ333333333333333333");	
		/* �Խ��������Ϣ����--������Ϣ */
		if( pub_base_CompDblVal( g_pub_tx.tx_amt2 ,0.00 ) )
		{
			/** ����ˮ�ͼ���(�������Ϣ����Ӧ�Ŀ�Ŀ) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	   		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0��*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
															3���ջ�����*/  
			strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
          		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
           		WRITEMSG
			strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
			g_ln_mst.in_lo_acm=0.00;
			ret = sql_execute( "UPDATE ln_mst SET in_lo_acm=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
					g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if( ret ) goto ErrExit;	  		
	  		g_pub_tx.svc_ind=3020;	/* ��������,ǷϢ���� */	
			strcpy ( g_pub_tx.brf,"���������Ϣ�����" );
			sprintf(acErrMsg,"~~~~backin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backcmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG	
	   		/*** ���׼��ʴ��� ***/
	   		if( pub_acct_trance() )
	   		{
           		sprintf(acErrMsg,"���׼��ʴ������!");
           		WRITEMSG
           		goto ErrExit;
	   		}
	   		sprintf(acErrMsg,"~~~~1in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
          		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
           		WRITEMSG
	   		/*** ��Ƽ��� ***/
			set_zd_data("0210",g_ln_parm.cur_no); 
			set_zd_double("0400",g_pub_tx.tx_amt2);                                    
			strcpy(g_pub_tx.sub_tx_code,"G083");
           	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
           	if (ret != 0)
           	{
           		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
           		WRITEMSG
           		goto ErrExit;
           	}
           	g_pub_tx.tx_amt2=0.00;
			set_zd_double("0400",0.00);           	
      }
      
 		/* �Խ��������Ϣ����--������Ϣ */
		if( pub_base_CompDblVal( g_pub_tx.tx_amt3 ,0.00 ) )
		{
			/** ����ˮ�ͼ���(�������Ϣ����Ӧ�Ŀ�Ŀ) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	   		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0��*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
			 													3���ջ�����*/  
			strcpy( g_pub_tx.note_type,"299");
			strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
           		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
           		WRITEMSG			
			strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
			
	  		g_ln_mst.out_lo_acm=0.00;
			ret = sql_execute( "UPDATE ln_mst SET out_lo_acm=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
					g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if( ret ) goto ErrExit;
			
	  		g_pub_tx.svc_ind=3030;	/* ��������,ǷϢ���� */	
			strcpy ( g_pub_tx.brf,"���������Ϣ�����" );
			sprintf(acErrMsg,"~~~~backin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backcmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG	
	   		/*** ���׼��ʴ��� ***/
	   		if( pub_acct_trance() )
	   		{
           		sprintf(acErrMsg,"���׼��ʴ������!");
           		WRITEMSG
           		goto ErrExit;
	   		}
	   		sprintf(acErrMsg,"~~~~1in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
           		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
           		WRITEMSG
	   		/*** ��Ƽ��� ***/
			set_zd_data("0210",g_ln_parm.cur_no); 
			set_zd_double("0410",g_pub_tx.tx_amt3);                                
			strcpy(g_pub_tx.sub_tx_code,"G083");
           	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
           	if (ret != 0)
           	{
           		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
           		WRITEMSG
           		goto ErrExit;
           	}
           	g_pub_tx.tx_amt3=0.00;
			set_zd_double("0410",0.00);         	
      }     
      
		/* �Խ��������Ϣ����--������Ϣ */
		g_pub_tx.tx_amt4=0.00;/**��GXJ����***/
		if( pub_base_CompDblVal( g_pub_tx.tx_amt4 ,0.00 ) )
		{
			/** ����ˮ�ͼ���(�������Ϣ����Ӧ�Ŀ�Ŀ) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	   		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0��*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
																3���ջ�����*/  
			strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
			strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	  		g_ln_mst.cmpd_lo_acm=0.00;
			ret = sql_execute( "UPDATE ln_mst SET cmpd_lo_acm=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
					g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if( ret ) goto ErrExit;
	  		g_pub_tx.svc_ind=3040;	/* ��������,ǷϢ���� */	
			strcpy ( g_pub_tx.brf,"����������Ϣ�븴��" );
			sprintf(acErrMsg,"~~~~backin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backcmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG	
	   		/*** ���׼��ʴ��� ***/
	   		if( pub_acct_trance() )
	   		{
           		sprintf(acErrMsg,"���׼��ʴ������!");
           		WRITEMSG
           		goto ErrExit;
	   		}
	   		sprintf(acErrMsg,"~~~~1in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
	   		/*** ��Ƽ��� ***/
			set_zd_data("0210",g_ln_parm.cur_no); 
			set_zd_double("1089",g_pub_tx.tx_amt4);                                 
			strcpy(g_pub_tx.sub_tx_code,"G083");
           	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
           	if (ret != 0)
           	{
           		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
           		WRITEMSG
           		goto ErrExit;
           	}
           	g_pub_tx.tx_amt4=0.00;
			set_zd_double("1089",0.00);         	
      }    
      /***  add by lance  20030902 �������ļ��л��� ****/
	ret = sql_execute( "UPDATE ln_mst SET intst_acm=0.00,in_lo_acm=0.00,out_lo_acm=0.00,cmpd_lo_acm=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
					g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if( ret ) goto ErrExit;  
      	/*��������*/
	bj_acm=0.00;
	bn_acm=0.00;
	bw_acm=0.00;
	fl_acm=0.00;
	bj_sect_acm=0.00;
	bn_sect_acm=0.00;
	bw_sect_acm=0.00;
	fl_sect_acm=0.00;
	
	  /*��ȡ�������ļ�*/
	  ret= Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld and ac_seqn=%d",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
	  if(ret)
	  {
			goto ErrExit;
	  }
		/* �ж��Զ������־ */
		if ( g_ln_parm.auto_pay_ind[0]=='Y')
		{
		TRACE
	   		/* ���ݱ������ɻ��ڻ����˺�ID */ 
	   		ret=pub_base_CurToSeqn( g_ln_parm.cur_no , &ac_seqn );
	   		if ( ret )
	   		{
				sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
				WRITEMSG
				goto ErrExit;	
	   		}	  
	   		/* ȡ�����˻���Ϣ */
	   		ret=pub_base_currentid_file(g_ln_mst.repay_ac_id,ac_seqn,&g_dd_mst);
	   		if( ret )
	   		{
				sprintf(acErrMsg,"���ø��ݱ��֡���ʾ�˺�ȡ�������ļ���������");
				WRITEMSG
				goto ErrExit;
	   		}
           	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld order by note_sts ", 
															g_dd_mst.ac_id);
           	if ( ret == 100 )
           	{
                sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
                WRITEMSG
                strcpy (g_pub_tx.reply,"W010");
                goto ErrExit;
           	}else if ( ret && ret!=100){
                sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
                WRITEMSG
                strcpy (g_pub_tx.reply,"W011");
                goto ErrExit;
           	}
           	strcpy( dd_ac_no ,g_mdm_ac_rel.ac_no );
           	dd_ac_id =g_mdm_ac_rel.ac_id ; /*����*/
	        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
	        	
	        ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
																g_dd_mst.prdt_no);
	        if( ret==100 )
	        {
	            sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%s]",g_dd_mst.prdt_no);
	            WRITEMSG
	            strcpy(g_pub_tx.reply,"P145");
	            goto ErrExit;
	        }else if( ret ){
	            sprintf(acErrMsg,"��ѯ���ڲ������쳣!!ret=[%d]",ret);
	            WRITEMSG
	            goto ErrExit;
	        }		
			TRACE
			if( g_dd_mst.ac_sts[0]!='1' )
			{
				sprintf(acErrMsg,"�˺�״̬������״̬!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P109");
				coninue_flag++;
			}
			/* �˺���ȫ�������ֻ������ */
			if( g_dd_mst.hold_sts[0]=='1'||g_dd_mst.hold_sts[0]=='2' )
			{
				sprintf(acErrMsg,"���˺��ѱ�����!");
				WRITEMSG
				coninue_flag++;			
			}
			if( g_dd_parm.draw_ind[0]=='N' )
			{
				sprintf(acErrMsg,"������ȡ[%s]!",g_dd_parm.draw_ind);
				WRITEMSG
				strcpy(g_pub_tx.reply,"A023");
				coninue_flag++;
			}
			/* ����� */
			ret = pub_base_CompDblVal(g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal,0.00);
			if( ret<=0 )
			{	
				coninue_flag++;
			}else{
				g_pub_tx.tmp_amt1 = g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;
			}
		
			/* ���ǲ��ֶ���״̬ */
			if( g_dd_mst.hold_sts[0]=='3' )
			{
				ret=pub_base_CompDblVal(g_pub_tx.tmp_amt1-g_dd_mst.hold_amt , 0.00);
				if( ret<=0 )
				{
					coninue_flag++;
				}else{
					g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	
				}
			}
			sprintf(acErrMsg,"ԭʼ���ô���%lf��!",g_pub_tx.tmp_amt1);
			WRITEMSG
	
			if( coninue_flag )
			{
				sprintf(acErrMsg,"����˻�״̬����ȷ![%d]",coninue_flag);
				WRITEMSG
				g_pub_tx.tmp_amt1=0.00;
			}				
			TRACE	            	   

	   		/*ȷ������ǷϢ�Ļ�����  0����1������Ϣ��2����ǷϢ��3���ڸ�����
			4����ǷϢ��5���⸴����6����ǷϢ��7��������*/
	   		i=0;
	   		vln_intst[i].intst_amt=g_ln_mst.bal;           vln_intst[i].type=7;/*ԭ��0*/
	   		i++;
			/****
	   		vln_intst[i].intst_amt=vbjlx;                  vln_intst[i].type=1;
			***/
	   		vln_intst[i].intst_amt=0.00;                  vln_intst[i].type=6;/*ԭ��1*/
	   		i++;
	   		vln_intst[i].intst_amt=g_ln_mst.in_lo_intst;   vln_intst[i].type=5;/*ԭ��2*/
	   		i++;
			/****
	   		vln_intst[i].intst_amt=vbnlx;                  vln_intst[i].type=3;
			****/
	   		vln_intst[i].intst_amt=0.00;                  vln_intst[i].type=4;/*ԭ��3*/
	   		i++;
	   		vln_intst[i].intst_amt=g_ln_mst.out_lo_intst;  vln_intst[i].type=3;/*ԭ��4*/
	   		i++;	  
			/****
	   		vln_intst[i].intst_amt=vbwlx;                  vln_intst[i].type=5;
			****/
	   		vln_intst[i].intst_amt=0.00;                  vln_intst[i].type=2;/*ԭ��5*/
	   		i++;
	/***��GXJ�޸�   		
	vln_intst[i].intst_amt=g_ln_mst.cmpd_lo_intst; vln_intst[i].type=1;***/
    vln_intst[i].intst_amt=0.00; vln_intst[i].type=1;/*ԭ��6*/
	   		i++;
			/*****
	   		vln_intst[i].intst_amt=vfllx;                  vln_intst[i].type=7;
			*****/
	   		vln_intst[i].intst_amt=0.00;                  vln_intst[i].type=0;/*ԭ��7*/
	   		i++;
			TRACE
	  		/* ���ݻ���˳���˳�򻹿� */	  
	   		ret=pub_ln_OrderPay(&vln_intst,g_ln_parm.pay_seq_code,
															&g_pub_tx.tmp_amt1);
	  		if( ret ) goto ErrExit;
	  				
				sprintf(acErrMsg,"����˺�[%ld]",g_dd_mst.ac_id);
				WRITEMSG
				sprintf(acErrMsg,"�����˺�[%ld]",g_ln_mst.ac_id);
				WRITEMSG
			 	sprintf(acErrMsg,"������[%lf]",g_pub_tx.tmp_amt1);
				WRITEMSG
			 	sprintf(acErrMsg,"�������:����[%lf]����[%lf]����[%lf]����[%lf]",
					g_ln_mst.bal,g_ln_mst.in_lo_intst,g_ln_mst.out_lo_intst,g_ln_mst.cmpd_lo_intst);
				WRITEMSG
			 	sprintf(acErrMsg,"�������:����[%lf]����[%lf]����[%lf]����[%lf]",
						vbjlx,vbnlx,vbwlx,vfllx);
				WRITEMSG
       }
    TRACE
	
vtcp_log("JJJJJ4444444444444444444");	
	/* ���� */
	tx_amt1= vln_intst[0].pay_amt; /*���˽��*/
	if( pub_base_CompDblVal(g_ln_mst.bal,vln_intst[0].pay_amt) )
	{
	 	amt1= g_ln_mst.bal-vln_intst[0].pay_amt;	  /*���˽��*/
	 	dc_flag++;
	}	
	TRACE
	/* ������Ϣ */
	if ( bj_flag==1 )
	{
	 	tx_amt2= vln_intst[1].pay_amt; /*���˽��*/
	}else{
	 	tx_amt3= vln_intst[1].pay_amt; /*���˽��*/
	}	  
	if( pub_base_CompDblVal(vbjlx,vln_intst[1].pay_amt) )
	{
		if ( bj_flag==1 )
	  	{	  	
	    	amt2= vbjlx-vln_intst[1].pay_amt;	  /*���˽��*/
	  	}else{
	    	amt3= vbjlx-vln_intst[1].pay_amt;	  /*���˽��*/
	  	}
	  	dc_flag++;
	}
	TRACE
	/* ����ǷϢ */
	tx_amt2+= vln_intst[2].pay_amt; /*���˽��*/
	if( pub_base_CompDblVal(g_ln_mst.in_lo_intst,vln_intst[2].pay_amt) )
	{
		amt2+= g_ln_mst.in_lo_intst-vln_intst[2].pay_amt;	  /*���˽��*/
		dc_flag++;
	}
	/* ���ڸ��� */
	tx_amt4+= vln_intst[3].pay_amt; /*���˽��*/
	if( pub_base_CompDblVal(vbnlx,vln_intst[3].pay_amt) )
	{
		amt4+= vbnlx-vln_intst[3].pay_amt;	  /*���˽��*/
		dc_flag++;
	}	
	/* ����ǷϢ */
	tx_amt3+= vln_intst[4].pay_amt; /*���˽��*/
	if( pub_base_CompDblVal(g_ln_mst.out_lo_intst,vln_intst[4].pay_amt) )
	{
		amt3+= g_ln_mst.out_lo_intst-vln_intst[4].pay_amt;	  /*���˽��*/
		dc_flag++;
	}	
	/* ���⸴�� */
	tx_amt4+= vln_intst[5].pay_amt; /*���˽��*/
	if( pub_base_CompDblVal(vbwlx,vln_intst[5].pay_amt) )
	{
		amt4+= vbwlx-vln_intst[5].pay_amt;	  /*���˽��*/
		dc_flag++;
	}	
	/* ����ǷϢ */
	tx_amt4+= vln_intst[6].pay_amt; /*���˽��*/
	if( pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,vln_intst[6].pay_amt) )
	{
		amt4+= g_ln_mst.cmpd_lo_intst-vln_intst[6].pay_amt;	  /*���˽��*/
		dc_flag++;
	}	
	/* ������Ϣ */
	tx_amt4+= vln_intst[7].pay_amt; /*���˽��*/
	if( pub_base_CompDblVal(vfllx,vln_intst[7].pay_amt) )
	{
		amt4+= vfllx-vln_intst[7].pay_amt;	  /*���˽��*/
		dc_flag++;
	}
	TRACE

	/*д����Ǽǲ�  0�Ǽ�Ƿ�� 1�����ѵǼ�Ƿ�� 2�Ǽǲ�����*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"1",tx_amt1,0.00,"0",1 );
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"1PASS");
	WRITEMSG

	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"2",tx_amt2,0.00,"0",1 );
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"2PASS");
	WRITEMSG
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"3",tx_amt3,0.00,"0",1 );
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"3PASS");
	WRITEMSG
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"4",tx_amt4,0.00,"0",1 );
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"4PASS");
	WRITEMSG

	/*��������������Ϣ�ۼӵ��ֻ���
	if( bj_flag==1 )
	{
		g_ln_mst.in_lo_intst+= vbjlx;
	}else if( bj_flag==2 ){
		g_ln_mst.out_lo_intst+= vbjlx;
	}else{
		goto ErrExit;
	}
	
	g_ln_mst.cmpd_lo_intst+= vbnlx+vbwlx+vfllx;
	***********/

vtcp_log("JZAdd_ac[%s]ln_ac[%s]tx_amt1[%lf]tx_amt2[%lf]tx_amt3=[%lf]tx_amt4[%lf]dc_flag=[%d]",
	dd_ac_no,ln_ac_no,tx_amt1,tx_amt2,tx_amt3,tx_amt4,dc_flag);
vtcp_log("JZBdd_id[%d]ln_id[%d]tx_amt1[%lf]tx_amt2[%lf]tx_amt3=[%lf]tx_amt4[%lf]dc_flag=[%d]",
	g_dd_mst.ac_id,g_ln_mst.ac_id,tx_amt1,tx_amt2,tx_amt3,tx_amt4,dc_flag);
vtcp_log("TRACE_NO=[%d]",g_pub_tx.trace_no);
	vtcp_log("ACID=[%ld]  AC_NO=[%s],%d",g_dd_mst.ac_id,dd_ac_no,__LINE__);
	
    if( pub_base_CompDblVal( tx_amt1 ,0.00 ) )
    {
	TRACE
    	/*************** ����ˮ�ͼ���(�ɻ����𲿷�) **************/
    	/******�����********/
    	strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ; 
    	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
 		strcpy( g_pub_tx.ac_get_ind,"011");	/*δ��ȡ��mdm_ac_rel*/
	   	strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
		/****
		strcpy( ac_get_ind,"111" );			*����ȡ�ֻ��ˣ����������ʱ�*
		***/
		strcpy( g_pub_tx.note_type,"299");
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
   		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
   		WRITEMSG
	   	strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	   	strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
        strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	   	g_pub_tx.svc_ind=3010;
	   	g_pub_tx.tx_amt1=tx_amt1;	
        strcpy ( g_pub_tx.brf,"���յ��ڴ������" );
	
	   	/*** ���׼��ʴ��� ***/
	   	if( pub_acct_trance() )
	   	{
             sprintf(acErrMsg,"���׼��ʴ������!");
             WRITEMSG
             goto ErrExit;
	   	}
   		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
   		WRITEMSG
	   	/* ���л�Ƽ��� */
        set_zd_data("0210",g_ln_parm.cur_no);  
        set_zd_double("107F",g_pub_tx.tx_amt1);       
        strcpy(g_pub_tx.sub_tx_code,"L201");
        strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
        }
        set_zd_double("107F",0.00);                       
        /* ί��Э�����(����) */
    /**ί�д����������������⣬��ʱ���ϲ�����JYW * 
		ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "2" );   
        if( ret )  goto ErrExit;   
	*/
		vtcp_log("������ln_trust����[%s][%d]\n",__FILE__,__LINE__);
	   	g_pub_tx.tx_amt1=0.00;
	    /**********ȡ���************/
        strcpy( g_pub_tx.ac_no , dd_ac_no);
		sprintf(acErrMsg,"dd_ac_no=[%s][%ld][%ld]",dd_ac_no ,g_dd_mst.ac_id ,g_dd_mst.ac_seqn);
		WRITEMSG

		g_pub_tx.ac_id = g_dd_mst.ac_id ; 
        g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
	   	strcpy( g_pub_tx.ac_get_ind,"00");	/*δ��ȡ��mdm_ac_rel*/
	   	strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-���� */
	   	strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0��*/
	   	strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
																3���ջ�����*/ 
 		strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵdd_ac_no*/
        strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.ac_wrk_ind[9]='1';     /*���ڷǽ��㻧ǿ�пۿ�*/
		g_pub_tx.ac_wrk_ind[8]='1';         /*���ռ��˱�־ */

	   	g_pub_tx.svc_ind=1001;
	   	g_pub_tx.tx_amt1=tx_amt1;	
        strcpy ( g_pub_tx.brf,"�����" );
	
vtcp_log("JJJJJ666666666666666666");	
	   	/*** ���׼��ʴ��� ***/
	   	if( pub_acct_trance() )
	   	{
             sprintf(acErrMsg,"���׼��ʴ������!");
             WRITEMSG
             goto ErrExit;
	   	}
	    /* ���л�Ƽ��� */
        set_zd_data("102J",g_ln_parm.cur_no);      
        set_zd_double("102F",g_pub_tx.tx_amt1);             
        strcpy(g_pub_tx.sub_tx_code,"D003");/**ȡ���ӽ���**/
        strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
             sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
             WRITEMSG
             goto ErrExit;
        }
        set_zd_double("102F",0.00);             
	   	g_pub_tx.tx_amt1=0.00;
    }
vtcp_log("JJJJJ77777777777777777777");	

		vtcp_log("ACID=[%ld]  AC_NO=[%s],%d",g_dd_mst.ac_id,dd_ac_no,__LINE__);
	
    if( pub_base_CompDblVal( tx_amt2+tx_amt3+tx_amt4 ,0.00 ) )
    {
	TRACE
     /*************** ����ˮ�ͼ���(�ɻ���Ϣ����) **************/
     /******�����********/
    	strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ; 
    	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
 		strcpy( g_pub_tx.ac_get_ind,"011");	/*δ��ȡ��mdm_ac_rel*/
   		strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
   		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
   		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
   		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
   		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/ 
   		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
 		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/ 
  		g_pub_tx.svc_ind=3050;
  		g_pub_tx.tx_amt2=tx_amt2;
  		g_pub_tx.tx_amt3=tx_amt3;
  		g_pub_tx.tx_amt4=tx_amt4;	
        strcpy ( g_pub_tx.brf,"���յ��ڴ����Ϣ" );
	
	    /*** ���׼��ʴ��� ***/
	    if( pub_acct_trance() )
	    {
             sprintf(acErrMsg,"���׼��ʴ������!");
             WRITEMSG
             goto ErrExit;
	    }
	    /* ���л�Ƽ��� */
        set_zd_data("0210",g_ln_parm.cur_no); 
        set_zd_double("108F",g_pub_tx.tx_amt2);    
        set_zd_double("108H",g_pub_tx.tx_amt3); 
        set_zd_double("108I",g_pub_tx.tx_amt4);                                     
        strcpy(g_pub_tx.sub_tx_code,"L202");
        strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
        }

		vtcp_log("ACID=[%ld]  AC_NO=[%s]",g_dd_mst.ac_id,dd_ac_no);

        /* ί��Э�����(��Ϣ) */
/************JYW*/
/*	if(g_ln_mst.mtr_date==g_pub_tx.tx_date){*/
        ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "3" );
        if( ret )  goto ErrExit;           
/*	}*/
/***************/
		vtcp_log("������ln_trust��Ϣ[%s][%d]\n",__FILE__,__LINE__);
           
	    g_pub_tx.tx_amt2=0.00;
	   	g_pub_tx.tx_amt3=0.00;	   
	   	g_pub_tx.tx_amt4=0.00;	   
        set_zd_double("108F",0.00);    
        set_zd_double("108H",0.00); 
        set_zd_double("108I",0.00);                                     
	   	/**********ȡ���************/
        strcpy( g_pub_tx.ac_no , dd_ac_no);
	/*JYW	g_pub_tx.ac_id = g_dd_mst.ac_id ;***/
		g_pub_tx.ac_id = dd_ac_id ;
        g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;

		vtcp_log("ACID=[%ld]  AC_NO=[%s]",g_dd_mst.ac_id,dd_ac_no);

	    strcpy( g_pub_tx.ac_get_ind,"00");	/*�Ѿ���ȡ��mdm_ac_rel*/
	    strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
	    strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	    strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	    strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	    strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
   		strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵdd_ac_no*/
        strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	    g_pub_tx.svc_ind=1001;
		g_pub_tx.ac_wrk_ind[9]='1';     /*���ڷǽ��㻧ǿ�пۿ�*/
		g_pub_tx.ac_wrk_ind[8]='1';         /*���ռ��˱�־ */

	    /**** add by ly 2003-08-27 23:24  ***/
	    g_pub_tx.tx_amt1=tx_amt2+tx_amt3+tx_amt4;
	    
	    /**** del by ly 2003-08-27 23:24
	    g_pub_tx.tx_amt2=tx_amt2;	
	    g_pub_tx.tx_amt3=tx_amt3;	   
	    g_pub_tx.tx_amt4=tx_amt4;
	    *****/	   
        strcpy ( g_pub_tx.brf,"���մ���ڻ�Ϣ" );
	
	    /*** ���׼��ʴ��� ***/
	    if( pub_acct_trance() )
	    {
             sprintf(acErrMsg,"���׼��ʴ������!");
             WRITEMSG
             goto ErrExit;
	    }
	    /* ���л�Ƽ��� */
        set_zd_data("102J",g_ln_parm.cur_no);      
        set_zd_double("102F",g_pub_tx.tx_amt1);     
        strcpy(g_pub_tx.sub_tx_code,"D003");/**ȡ���ӽ���**/
        strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
        }
	   	g_pub_tx.tx_amt1=0.00;
	   	g_pub_tx.tx_amt2=0.00;
	   	g_pub_tx.tx_amt3=0.00;	   
	   	g_pub_tx.tx_amt4=0.00;
        set_zd_double("102F",0.00);
    }
   
    strncpy(pact_no_tmp,g_ln_mst.pact_no,strlen(g_ln_mst.pact_no)-3); 
    /********houyi20100620*********/
    
		if(pub_base_CompDblVal(tx_amt2 + tx_amt3 + tx_amt4,0.00)>0)  /*mod by lwb 20150413  ��Ϣ�ͱ���ֿ�������*/
		{	
			/*pub_mob_sendmail_ln("�黹����","Z088",g_mdm_ac_rel.ac_no,"1",tx_amt2 + tx_amt3 + tx_amt4,tx_amt1,amt1);*/
			pub_mob_sendmail_ln("�黹��Ϣ","Z088",pact_no_tmp,"1",tx_amt2 + tx_amt3 + tx_amt4,tx_amt1,amt1); /*mod by lwb 20150408*/
   	}
   	if(pub_base_CompDblVal(tx_amt1,0.00)>0)  /*mod by lwb 20150413  ��Ϣ�ͱ���ֿ�������*/
		{	
			/*pub_mob_sendmail_ln("�黹����","Z088",g_mdm_ac_rel.ac_no,"1",tx_amt2 + tx_amt3 + tx_amt4,tx_amt1,amt1);*/
			pub_mob_sendmail_ln("�黹����","Z088",pact_no_tmp,"1",tx_amt1,tx_amt1,amt1); /*mod by lwb 20150408*/
   	}
    
vtcp_log("JJJJJ888888888888888888888");	
    /***********���˴���************************	
    if( dc_flag!=0 )
    {
     	**** ���˽��ת������********
        strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ; 
        g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
 		strcpy( g_pub_tx.ac_get_ind,"011");	δ��ȡ��mdm_ac_rel
	    strcpy( g_pub_tx.ac_id_type,"3" );	 �˺����� 3-���� 
	    strcpy( g_pub_tx.add_ind,"1" );		����:0��1��2���� 	
	    strcpy( g_pub_tx.ct_ind,"2" );		 ��ת:1��2ת 	
	    strcpy( g_pub_tx.prt_ind,"0" );�Ƿ��ӡ����(�������۱�־):1��0�� 
	    strcpy( g_pub_tx.hst_ind,"1" );����ϸ:1�ռ���2���յ�����3���ջ�����
        strcpy ( g_pub_tx.intst_type , "1" );	 ��Ϣ���� 	
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);    Ϊ���մ�Ʊ��ӡ��ֵln_ac_no
	    g_pub_tx.svc_ind=3063;
	    g_pub_tx.tx_amt1=-amt1;
	    **
	    g_pub_tx.tx_amt2=amt2;
	    g_pub_tx.tx_amt3=amt3;
	    g_pub_tx.tx_amt4=amt4;
	    ***	   	  	   	
        strcpy ( g_pub_tx.brf,"���մ����ת��" );
	
	    ** ���׼��ʴ��� ***
	    if( pub_acct_trance() )
	    {
             sprintf(acErrMsg,"���׼��ʴ������!");
             WRITEMSG
             goto ErrExit;
	    }
	    * ���л�Ƽ��� *
        set_zd_data("0210",g_ln_parm.cur_no);  
        set_zd_double("1074",g_pub_tx.tx_amt1);  
        ***   
        set_zd_double("1075",g_pub_tx.tx_amt2);    
        set_zd_double("1076",g_pub_tx.tx_amt3); 
        set_zd_double("1077",g_pub_tx.tx_amt4);    
        ***                        
        strcpy(g_pub_tx.sub_tx_code,"L30B");
        strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
        }
        set_zd_double("1074",0.00);     
        set_zd_double("1075",0.00);    
        set_zd_double("1076",0.00); 
        set_zd_double("1077",0.00);                             
		g_pub_tx.tx_amt1=0.00;
		g_pub_tx.tx_amt2=0.00;
		g_pub_tx.tx_amt3=0.00;
		g_pub_tx.tx_amt4=0.00;
	    **********ת�벿��************
        strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ;
        g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
        strcpy( g_pub_tx.ac_get_ind,"011"); *δ��ȡ��mdm_ac_rel*
	    strcpy( g_pub_tx.ac_id_type,"3" );	* �˺����� 3-���� *
	    strcpy( g_pub_tx.add_ind,"1" );		* ����:0��1��2���� *
	    strcpy( g_pub_tx.ct_ind,"2" );		* ��ת:1��2ת *
	    strcpy( g_pub_tx.prt_ind,"0" );*�Ƿ��ӡ����(�������۱�־):1��0�� *
	    strcpy( g_pub_tx.hst_ind,"1" );*����ϸ:1�ռ���2���յ�����3���ջ�����*        
        strcpy ( g_pub_tx.intst_type , "1" );	* ��Ϣ���� *	
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);    *Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*
	    g_pub_tx.svc_ind=3060;
	    g_pub_tx.tx_amt1=amt1;	   	  	   	
        strcpy ( g_pub_tx.brf,"���մ����ת��" );
	    sprintf( flag , "%d" , chg_flag );
	  	strcpy( g_ln_mst.ac_sts,flag );
		*******
		g_ln_mst.ac_sts[0]='2';				*��Ϊת�������޸ı�־*
		*******

	    *** ���׼��ʴ��� ***
	    if( pub_acct_trance() )
	    {
             sprintf(acErrMsg,"���׼��ʴ������!");
             WRITEMSG
             goto ErrExit;
	    }
	    
		*	if(g_ln_mst.ac_sts[0]=='2')
			{	
				*pub_mob_sendmail_ln("����","Z089",g_mdm_ac_rel.ac_no,"1",g_pub_tx.tx_amt1,0.0,0.0);*
		*		pub_mob_sendmail_ln("����","Z089",pact_no_tmp,"1",g_pub_tx.tx_amt1,0.0,0.0); /*mod by lwb 20150408 *
		 * }*
	    
	    * ���л�Ƽ��� *
        set_zd_data("0210",g_ln_parm.cur_no); 
        set_zd_double("107F",g_pub_tx.tx_amt1);                       
        strcpy(g_pub_tx.sub_tx_code,"L302");
        strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
        }
	    g_pub_tx.tx_amt1=0.00;
	    g_pub_tx.tx_amt2=0.00;
	    g_pub_tx.tx_amt3=0.00;
	    g_pub_tx.tx_amt4=0.00;
        set_zd_double("107F",0.00);                       
	}lpj20151124����ת�����������ڵļ��ˣ�ȫ��Ϊ�������߼�ֵ��Ʒ*/
vtcp_log("JJJJJ9999999999999999999");	
	if( dc_flag==0 )
	{
	   if( g_ln_parm.auto_cls[0]=='Y' )
	   {
	       g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻�id */
	       g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		   strcpy( g_pub_tx.ac_get_ind,"011"); /*δ��ȡ��mdm_ac_rel*/
	       strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	       strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	       strcpy ( g_pub_tx.add_ind , "2" );		/* ����:0��1��2���� */	
	       strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	       strcpy ( g_pub_tx.prt_ind , "0" );/*�Ƿ��ӡ����(�����۱�־):1��0��*/
	       strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����
																3���ջ�����*/
	       strcpy ( g_pub_tx.brf,"��������" );
		   strcpy( g_pub_tx.beg_note_no, ln_ac_no);    /*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
	       g_pub_tx.svc_ind = 2;				/* ���� */
	
           /*** ���׼��ʴ��� ***/
	       if( pub_acct_trance() )
	       {
		    	sprintf(acErrMsg,"���׼��ʴ������!");
		    	WRITEMSG
		    	goto ErrExit;
	       }
           /* ���´������ļ� */
           strcpy(g_ln_mst.ac_sts,"*");
           g_ln_mst.lst_date = g_pub_tx.tx_date;               	               
           ret = Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
           if (ret != 0)
           {
                sprintf(acErrMsg,"���´������ļ�����!!");
                WRITEMSG
                goto ErrExit;
           }
      }
   }else{
           /* ���´������ļ� */
           sprintf( flag , "%d" , chg_flag );
	sprintf(acErrMsg,"���´������ļ�[%s]!!",flag);
	WRITEMSG           
           strcpy( g_ln_mst.ac_sts,flag );  
           g_ln_mst.lst_date = g_pub_tx.tx_date;              	               
           ret = Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
           if (ret != 0)
           {
                sprintf(acErrMsg,"���´������ļ�����!!");
                WRITEMSG
                goto ErrExit;
           }
         }           
vtcp_log("JJJJJaaaaaaaaaaaaaaaaaaaaa");	
		if(g_ln_mst.ac_sts[0]=='2'&&pub_base_CompDblVal(g_ln_mst.bal,0.00)>0)
			{	
				/*pub_mob_sendmail_ln("����","Z089",g_mdm_ac_rel.ac_no,"1",g_pub_tx.tx_amt1,0.0,0.0);*/
				pub_mob_sendmail_ln("����","Z089",pact_no_tmp,"1",g_pub_tx.tx_amt1,0.0,0.0); /*mod by lwb 20150408 */
		  }
  }
    Eod_ln_mst_Clo_Upd();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���մ���ڴ������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* ���޸� */
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���մ���ڴ������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

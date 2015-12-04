/*************************************************************
* �� �� ��: gD083.c
* �������������գ������Ϣ
*
* ��    ��: lance
* �������: 2003��3��31��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define MYSQLERR if(ret) {\
		sprintf(acErrMsg,"SQLERROR [%d]",ret);\
		WRITEMSG\
		goto ErrExit;\
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_holiday_c.h"
#include "com_sys_parm_c.h"
#include "cif_basic_inf_c.h"

gD083()
{
	int    ac_seqn;
	int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0;
	long   time_year=0,time_month=0,time_day=0,time_flag=0;
	int	month_tmp=0;
	int	day_tmp=0;
	long ly_cnt=0;
	char ln_ac_no[20],dd_ac_no[20],flag[2];  
	char time_list[201];
	double vbnlx=0,vbwlx=0,vfllx=0,vbjlx=0;
	double amt1=0,amt2=0,amt3=0,amt4=0;
	double tx_amt1=0,tx_amt2=0,tx_amt3=0,tx_amt4=0;	
	struct S_ln_intst vln_intst[LN_INTST_TYPS];	
	struct com_holiday_c com_holiday;
	struct ln_mst_c ln_mst_tmp;	
	struct ln_parm_c ln_parm;
	struct ln_mst_c ln_mst;
	struct cif_basic_inf_c S_cif_basic_inf;
    double bj_acm=0.00;
	double bn_acm=0.00;
	double bw_acm=0.00;
	double fl_acm=0.00;
	double bj_sect_acm=0.00;
	double bn_sect_acm=0.00;
	double bw_sect_acm=0.00;
	double fl_sect_acm=0.00;
	struct com_sys_parm_c s_com_sys_parm;
	
   	char acm_type[2];
	int    coninue_flag=0;
	char type[2];			  
	amt1=amt2=amt3=amt4=0.00;	
	vbnlx=vbwlx=vfllx=vbjlx=0.00;	
	memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
	memset(&vln_intst,0x00,LN_INTST_TYPS*sizeof(struct S_ln_intst));
	memset(&ln_parm, 0x00, sizeof(struct ln_parm_c));
	memset(&ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset(&S_cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	set_zd_double("0400",0.00);
	set_zd_double("0410",0.00);
	set_zd_double("1089",0.00);
	set_zd_double("108F",0.00);
	set_zd_double("108H",0.00);
	set_zd_double("108I",0.00);
	set_zd_double("102F",0.00);
		
	sprintf(acErrMsg,"�����Ϣ��ʼDKJX");
	WRITEMSG

	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

   	/* ��ʼ��ȫ�ֱ��� */
   	pub_base_sysinit();

	/**xxx****3line********/
	ret = Com_sys_parm_Sel( g_pub_tx.reply,&s_com_sys_parm, "1=1" );
	if( ret ) goto ErrExit;
	g_pub_tx.tx_date=s_com_sys_parm.sys_date;

	strcpy(g_pub_tx.sub_tx_code,"G083");
	strcpy( g_pub_tx.tx_code, "G083");
	/*��������*/
	month_tmp=g_pub_tx.tx_date%10000/100;
	day_tmp=g_pub_tx.tx_date%100;
	
	/**��ǿҪ�����гжһ�Ʊ���ڴ����Ϣ delete by jk 20120213 begin
	strcpy( time_list, " intst_type not in('0','1','6') and repay_type not in ('3','4') and prdt_no !='3Y1' and "); 
	delete by jk 20120213 end**/
	/**��ǿҪ�����гжһ�Ʊ���ڴ����Ϣ add by jk 20120213 begin**/
	/**����3Y2��3Y3�������ȶ��ί�д����Ʒ 20150525 wudawei**/
	strcpy( time_list, " intst_type not in('0','1','6') and repay_type not in ('3','4') and prdt_no not in('3Y1','3Y2','3Y3','3Y4','3Y5','3Y6','3Y7','3Y8','3Y9','3YA','3YB','3Z1','3Z2','3Z3') and ");
	/**��ǿҪ�����гжһ�Ʊ���ڴ����Ϣ add by jk 20120213 end**/
	/* �ж�����ĩ����ĩ������ĩ */
	/* time_flag:0:��ͨ1:Ѯ��2:Ѯĩ 3:�³�4:��ĩ5:����6:��ĩ7:�����
												8:����ĩ 9:���10:��ĩ */	
	/****
	ret = pub_base_get_ymd(&time_year , &time_month , &time_day , 
											g_pub_tx.tx_date , &time_flag);
	if( ret )    goto ErrExit;
	
	if( time_flag==0 )	*��ͨ����*
	{
		strcat( time_list, " intst_type='2' and " );
	}	
	if( time_flag==4 )* ��ĩ *
	{		
		strcat( time_list," intst_type='3' and ");
	}
	if( time_flag==6 )* ��ĩ *
	{
		strcat( time_list," intst_type='4' and ");
	}		
	if( time_flag==10 )* ��ĩ *
	{
		strcat( time_list," intst_type='5' and ");
	}
	*****/
	if( day_tmp==21 )
	{
		strcat( time_list, " (intst_type='3' or " );
		if( month_tmp==3|| month_tmp==6|| month_tmp==9|| month_tmp==12 )
		{
			strcat( time_list," intst_type='4' or ");
		}
		if( month_tmp==12 )
		{
			strcat( time_list," intst_type='5' or ");
		}
		strcat ( time_list , " intst_type='2' ) and " );
	}else{
		strcat ( time_list , " intst_type='2' and " );
	}
		strcat(time_list," ac_sts in('1','2','3','4','5') and 1=1");		
	sprintf(acErrMsg,"������� [%s]",time_list);
	WRITEMSG
	
	ret = Eod_ln_mst_Dec_Upd(g_pub_tx.reply,time_list);
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret = Eod_ln_mst_Fet_Upd(&g_ln_mst, g_pub_tx.reply);
		if (ret&&ret!=100 )
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if( ret==100 ){
			break;
		}
		
		/* ȡ��ˮ�� xxx 7 lines*/
		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
			sprintf(acErrMsg,"Get trace_no error [%d]",g_pub_tx.trace_no);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
		set_zd_long( DC_TRACE_CNT,1 );

		ly_cnt++;
		/***
		if( ly_cnt>100 )
		{
			break;
		}
		***/
		vtcp_log(" DO WITH id='%ld' seqn='%d'",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		TRACE
		sprintf(acErrMsg,"lnly�ڡ������� %ld ����������",ly_cnt);
		WRITEMSG		
		
   		/* ȡ���������Ϣ */
		ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm , " prdt_no='%s' " ,
				        g_ln_mst.prdt_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"��������ļ�����[%s]��Ʒ",ln_mst_tmp.prdt_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L001" );
			goto ErrExit;
		}else if( ret ){
   			sprintf(acErrMsg,"ȡ��������ļ��쳣 [%s][%d]",ln_mst_tmp.prdt_no,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L001" );
   			goto ErrExit;
   		}
		TRACE
		/* ȡ������Ϣ */
		ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_id=%ld and ac_seqn=%d ",
															g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		if(ret)	goto ErrExit;
		strcpy( ln_ac_no, g_mdm_ac_rel.ac_no);
		
		/*���׻�����ֵ*/
		strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);
		sprintf(acErrMsg,"~~~~��ʼʱin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   	sprintf(acErrMsg,"~~~~��ʼʱout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   	sprintf(acErrMsg,"~~~~��ʼʱcmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
   	sprintf(acErrMsg,"~~~~��sssssscmpd_lo_intst[%s]", g_ln_mst.intst_type);
           	WRITEMSG
		/* ��Ϣ���� */
		if( g_ln_mst.intst_type[0]=='0' || g_ln_parm.intst_type[0]=='0' )	/*����Ϣ*/
		{
			vbjlx=0.00; bj_acm=0.00;
			vbnlx=0.00; bn_acm=0.00;
			vbwlx=0.00; bw_acm=0.00;
			vfllx=0.00; fl_acm=0.00;
		}
		else
		{  	sprintf(acErrMsg,"~~~~fffffff[%s]", g_ln_mst.intst_type);
           	WRITEMSG
			ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"0",&vbjlx,0 );
			if( ret ) goto ErrExit;
			
			/* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
			strcpy( acm_type , "2" ); 
			bj_acm = g_ln_mst.intst_acm ;/* ��Ϣ���� */
        	sprintf(acErrMsg,"~~~~mmmmmmm[%s]", g_ln_mst.intst_type);
           	WRITEMSG
			ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
								g_ln_parm.intst_days_type , acm_type , 
								g_ln_mst.bal ,g_ln_mst.bal , &bj_acm , "0",0,0);
			if( ret )
			{
				sprintf(acErrMsg,"���ú��������������!");
				WRITEMSG
				goto ErrExit;
			}
			  	sprintf(acErrMsg,"~~~~��fffffffcmpd_lo_intst[%s]", g_ln_mst.intst_type);
           	WRITEMSG
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
	
		/* �жϱ�����Ϣ */
		bj_flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );/* 1����  2���� */
		TRACE
		
		/*д����Ǽǲ�  0�Ǽ�Ƿ�� 1�����ѵǼ�Ƿ�� 2�Ǽǲ�����*/
		/*********
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,"4",vbnlx+vbnlx+vfllx,"0",0);
		if( ret ) goto ErrExit;
		*******/
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vbnlx,bn_acm+bn_sect_acm,"1",0);
		if( ret ) goto ErrExit;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vbwlx,bw_acm+bw_sect_acm,"1",0);
		if( ret ) goto ErrExit;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vfllx,fl_acm+fl_sect_acm,"1",0);
		if( ret ) goto ErrExit;				
		if( bj_flag==1 )
		{
	  		ret=pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",vbjlx,bj_acm+bj_sect_acm,"1",0);
	  		if(ret) goto ErrExit;
		}else{
	  		ret=pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",vbjlx,bj_acm+bj_sect_acm,"1",0);
	  		if(ret) goto ErrExit;
		}
		TRACE
      	/*****************add by ly ���еĸ��ֻ�����Ϣ **************/
  		/*�жϱ����������ڻ��Ǳ���*/
  		if(bj_flag==1)
  		{
   			g_pub_tx.tx_amt2=vbjlx;
   		}else{
   			g_pub_tx.tx_amt3=vbjlx;
   		}
	   	g_pub_tx.tx_amt4=vbnlx+vbwlx+vfllx;	
	   		
		/* �Խ��������Ϣ����--������Ϣ */
		if( pub_base_CompDblVal( g_pub_tx.tx_amt2 ,0.00 ) )
		{
			/** ����ˮ�ͼ���(�������Ϣ����Ӧ�Ŀ�Ŀ) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			/***����ȡ��ر�20050918******/
			strcpy( g_pub_tx.ac_get_ind,"111");	
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
/**JYW add***/
           	set_zd_double("108F",0.00);    
           	set_zd_double("108H",0.00); 
           	set_zd_double("108I",0.00); 
           	set_zd_double("0410",0.00); 
			
/**JYW add end***/
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
			/***����ȡ��ر�20050918******/
			strcpy( g_pub_tx.ac_get_ind,"111");
			
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
/**JYW add***/
           	set_zd_double("108F",0.00);    
           	set_zd_double("108H",0.00); 
           	set_zd_double("108I",0.00); 
           	set_zd_double("0400",0.00); 
			
/**JYW add end***/
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
		g_pub_tx.tx_amt4=0.00;
		if( pub_base_CompDblVal( g_pub_tx.tx_amt4 ,0.00 ) )
		{
			/** ����ˮ�ͼ���(�������Ϣ����Ӧ�Ŀ�Ŀ) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			/***����ȡ��ر�20050918******/
			strcpy( g_pub_tx.ac_get_ind,"111");
			
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
/**JYW add***/
           	set_zd_double("108F",0.00);    
           	set_zd_double("108H",0.00); 
           	set_zd_double("108I",0.00); 
           	set_zd_double("0400",0.00); 
           	set_zd_double("0410",0.00); 
			
/**JYW add end***/
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
	ret = sql_execute( "UPDATE ln_mst SET intst_acm=0.00,in_lo_acm=0.00,out_lo_acm=0.00,cmpd_lo_acm=0.00  \
								WHERE ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
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
	sprintf(acErrMsg,"~~~~qianin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
   	WRITEMSG
   	sprintf(acErrMsg,"~~~~qianout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
   	WRITEMSG
   	sprintf(acErrMsg,"~~~~qiancmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
   	WRITEMSG
	/*��ȡ�������ļ�*/
	/*memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); JYW*/
	ret=Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld and ac_seqn=%d",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
	if(ret)
	{
		goto ErrExit;
	}
	/* �����ڸ���,������ */
	if( pub_base_CompDblVal(g_ln_mst.bal,0.00)<0 ||pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)<0
			||pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)<0 ||pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,0.00)<0 )
	{
       		sprintf(acErrMsg,"���ڸ���!!!!!!!!!!!!!!!!!!");
       		WRITEMSG			
			TRACE
			continue;
	}
		 
    /** ��Ϣ���� **/
	/* ���Զ����� */
	coninue_flag=0;
sprintf(acErrMsg,"~~~~vln_intst[1].pay_amt[%lf]",vln_intst[1].pay_amt);
WRITEMSG
sprintf(acErrMsg,"~~~~vln_intst[2].pay_amt[%lf]",vln_intst[2].pay_amt);
WRITEMSG
sprintf(acErrMsg,"~~~~vln_intst[3].pay_amt[%lf]",vln_intst[3].pay_amt);
WRITEMSG
	memset( vln_intst,0x00,LN_INTST_TYPS*sizeof(struct S_ln_intst) );

	if ( g_ln_parm.auto_pay_ind[0]=='Y')
	{
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
				/****
				goto ErrExit;
				*****/
				coninue_flag=1;
	   		}else{
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
			}
			
			/** �Թ������� ***
			if( g_ln_mst.cif_no==0 )
			{
				coninue_flag++;
			}else{
				ret= Cif_basic_inf_Sel(g_pub_tx.reply,&S_cif_basic_inf,"cif_no=%ld",g_ln_mst.cif_no);
				if(ret)
				{
					sprintf(acErrMsg,"�ͻ��Ų�����!![%ld]",g_ln_mst.cif_no);
					WRITEMSG
					coninue_flag++;
				}else{
					if( S_cif_basic_inf.type[0]!='1' && S_cif_basic_inf.type[0]!='5')
					{
						coninue_flag++;
					}
				}
			}
		���ζԹ��Զ���******************/
			if( coninue_flag )
			{
				sprintf(acErrMsg,"����˻�״̬����ȷ![%d]",coninue_flag);
				WRITEMSG
				g_pub_tx.tmp_amt1=0.00;
			}				
			TRACE			
			
			vbjlx=0.00;
			vbnlx=0.00;
			vbwlx=0.00;
			vfllx=0.00;	
			/* ȡ������Ϣ *
			ret = pub_ln_lndis_file(ln_ac_no,0,
						&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); 
			if( ret ) { goto ErrExit; }
			***/

			TRACE
							
			
		sprintf(acErrMsg,"~~~~vln_intst[1].pay_amt[%lf]",vln_intst[1].pay_amt);
		WRITEMSG
		sprintf(acErrMsg,"~~~~vln_intst[2].pay_amt[%lf]",vln_intst[2].pay_amt);
		WRITEMSG
		sprintf(acErrMsg,"~~~~vln_intst[3].pay_amt[%lf]",vln_intst[3].pay_amt);
		WRITEMSG

			memset( vln_intst,0x00,LN_INTST_TYPS*sizeof(struct S_ln_intst) );
			/*ȷ������ǷϢ�Ļ�����  0����1������Ϣ��2����ǷϢ��3���ڸ�����
									4����ǷϢ��5���⸴����6����ǷϢ��7��������*/
			i=1;
			vln_intst[i].intst_amt=g_ln_mst.in_lo_intst;   vln_intst[i].type=2;
			i++;
			vln_intst[i].intst_amt=g_ln_mst.out_lo_intst;  vln_intst[i].type=4;
			i++;	  
			/***��GXJ��
			vln_intst[i].intst_amt=g_ln_mst.cmpd_lo_intst; vln_intst[i].type=6;
			****/
			vln_intst[i].intst_amt=0; vln_intst[i].type=6;
			i++;

	sprintf(acErrMsg,"JJJJ~~~~vln_intst[1].pay_amt[%lf]",vln_intst[1].pay_amt);
 WRITEMSG
  sprintf(acErrMsg,"JJJJ~~~~vln_intst[2].pay_amt[%lf]",vln_intst[2].pay_amt);
   WRITEMSG
	sprintf(acErrMsg,"JJJJ~~~~vln_intst[3].pay_amt[%lf]",vln_intst[3].pay_amt);
	WRITEMSG

			/* ���ݻ���˳���˳�򻹿� */	  
			ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
														&g_pub_tx.tmp_amt1 );
			if( ret ) goto ErrExit;										
		}	
		sprintf(acErrMsg,"KKKK~~~~vln_intst[1].pay_amt[%lf]",vln_intst[1].pay_amt);
		 WRITEMSG
		  sprintf(acErrMsg,"KKKK~~~~vln_intst[2].pay_amt[%lf]",vln_intst[2].pay_amt);
		   WRITEMSG
			sprintf(acErrMsg,"KKKK~~~~vln_intst[3].pay_amt[%lf]",vln_intst[3].pay_amt);
			WRITEMSG


		/* ����ɹ黹��������˽�� */
		dc_flag=0;
		tx_amt1=tx_amt2=tx_amt3=tx_amt4=0.00;
		amt1=amt2=amt3=amt4=0.00;
		TRACE
		/* ����ǷϢ */
		tx_amt2= vln_intst[1].pay_amt; /*���˽��*/
		if( pub_base_CompDblVal(g_ln_mst.in_lo_intst,vln_intst[1].pay_amt) )
		{
	  		amt2= g_ln_mst.in_lo_intst-vln_intst[1].pay_amt; /*���˽��*/
	  		dc_flag++;
		}
		/* ����ǷϢ */
		tx_amt3= vln_intst[2].pay_amt; /*���˽��*/
		if( pub_base_CompDblVal(g_ln_mst.out_lo_intst,vln_intst[2].pay_amt) )
		{
	  		amt3= g_ln_mst.out_lo_intst-vln_intst[2].pay_amt; /*���˽��*/
	  		dc_flag++;
		}		
		/* ����ǷϢ */
		tx_amt4= vln_intst[3].pay_amt; /*���˽��*/
		if( pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,vln_intst[3].pay_amt) )
		{
	  		amt4= g_ln_mst.cmpd_lo_intst-vln_intst[3].pay_amt; /*���˽��*/
	 		dc_flag++;
		}
		sprintf(acErrMsg,"~~~~tx_amt4[%lf]",tx_amt4);
           	WRITEMSG	
		TRACE
	   		sprintf(acErrMsg,"~~~~2in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~2out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~2cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~tx_amt2[%lf]",tx_amt2);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~tx_amt3[%lf]",tx_amt3);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~tx_amt4[%lf]",tx_amt4);
           		WRITEMSG

		/*д����Ǽǲ�  0�Ǽ�Ƿ�� 1�����ѵǼ�Ƿ�� 2�Ǽǲ�����*/
		/* ע������ʱ������������ */
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",tx_amt2,0.00,"1",1 );
		if( ret ) 
		{
			sprintf(acErrMsg,"2 ac_id[%d-%d],[%.2lf]",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn,tx_amt2);
			WRITEMSG
			goto ErrExit;
		}
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",tx_amt3,0.00,"1",1 );
		if( ret ) 
		{
			sprintf(acErrMsg,"3 ac_id[%d-%d],[%.2lf]",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn,tx_amt3);
			WRITEMSG
			goto ErrExit;
		}
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",tx_amt4,0.00,"1",1 );
		if( ret ) 
		{
			sprintf(acErrMsg,"4 ac_id[%d-%d],[%.2lf]",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn,tx_amt4);
			WRITEMSG
			goto ErrExit;
		}
	
	TRACE
		if( pub_base_CompDblVal( tx_amt2+tx_amt3+tx_amt4 ,0.00 ) )
		{
			/** ����ˮ�ͼ���(�ɻ���Ϣ����) **/
        	/** ����� **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			/****
			strcpy( g_pub_tx.ac_get_ind,"10");	*�Ѿ���ȡ��mdm_ac_rel*
			*******/
			strcpy( g_pub_tx.ac_get_ind,"011");	/*δ��ȡ��mdm_ac_rel*/
			
			strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	   		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0��*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
																3���ջ�����*/  
			strcpy( g_pub_tx.note_type,"299");
			strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
          	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	  		g_pub_tx.svc_ind=3050;
	   		g_pub_tx.tx_amt2=tx_amt2;
	   		g_pub_tx.tx_amt3=tx_amt3;
	   		g_pub_tx.tx_amt4=tx_amt4;
	   		
           	strcpy ( g_pub_tx.brf,"" );
			
	   		/*** ���׼��ʴ��� ***/
	   		if( pub_acct_trance() )
	   		{
           			sprintf(acErrMsg,"���׼��ʴ������!");
           			WRITEMSG
           			goto ErrExit;
	   		}
	   		
	   		
	   		
				pub_mob_sendmail_ln("�����Ϣ",g_pub_tx.tx_code,g_ln_mst.prdt_no,"0",tx_amt2 + tx_amt3 + tx_amt4,tx_amt1,0.0);
				
	   		
	   		
	   		/*** ��Ƽ��� ***/
/**JYW add***/
           	set_zd_double("0400",0.00); 
           	set_zd_double("0410",0.00); 
			
/**JYW add end***/
           	set_zd_data("0210",g_ln_parm.cur_no); 
           	set_zd_double("108F",g_pub_tx.tx_amt2);    
			vtcp_log("108H���[%.02lf][%.02lf][%.02lf]",g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
           	set_zd_double("108H",g_pub_tx.tx_amt3); 
           	set_zd_double("108I",g_pub_tx.tx_amt4);                                     
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
           	set_zd_double("108F",0.00);    
           	set_zd_double("108H",0.00); 
           	set_zd_double("108I",0.00); 
           		   
	   	/**********ȡ���************/
           	strcpy( g_pub_tx.ac_no , dd_ac_no);
		g_pub_tx.ac_id = g_dd_mst.ac_id;
           	g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
           	strcpy( g_pub_tx.ac_get_ind,"011");	/*δ��ȡ��mdm_ac_rel*/
	   		strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
	   		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0��*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
																3���ջ�����*/ 
           	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
			g_pub_tx.ac_wrk_ind[9]='1';     /*���ڷǽ��㻧ǿ�пۿ�*/
			g_pub_tx.ac_wrk_ind[8]='1';         /*���ռ��˱�־ */

			strcpy( g_pub_tx.note_type,"299");
			strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
	   		g_pub_tx.svc_ind=1001;
	   		g_pub_tx.tx_amt1=tx_amt2+tx_amt3+tx_amt4;		   
           	strcpy ( g_pub_tx.brf,"���Ϣ" );
	
	   		/*** ���׼��ʴ��� ***/
	  		if( pub_acct_trance() )
	   		{
               		sprintf(acErrMsg,"���׼��ʴ������!");
               		WRITEMSG
               		goto ErrExit;
	   		}
	   		/*** ���л�Ƽ��� ***/
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
           	vtcp_log("aaaaaaaaaaa[%lf]\n",g_pub_tx.tx_amt1);	
           	/* ί��Э�����(��Ϣ) */
           	ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "3" );
           	if( ret )	goto ErrExit;
	          		
        }
		TRACE
	   	g_pub_tx.tx_amt1=0.00;
	   	g_pub_tx.tx_amt2=0.00;
	   	g_pub_tx.tx_amt3=0.00;	   
	   	g_pub_tx.tx_amt4=0.00;
        set_zd_double("102F",0.00);	
            /* ���´������ļ� */ 
            g_ln_mst.lst_date = g_pub_tx.tx_date;    
            g_ln_mst.ic_date =  g_pub_tx.tx_date;                                        	               
            ret = Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
            if (ret)
            {
                 sprintf(acErrMsg,"���´������ļ�����!!");
                 WRITEMSG
                 goto ErrExit;
            }         
    }
    Eod_ln_mst_Clo_Upd();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����Ϣ�������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"AT12");/* ���޸� */
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�����Ϣ�������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*************************************************************
* �� �� ��:  rpt91d.c
* ���������� ֧���걨���ӡ(��˳ �� ��������)
* ��    ��:  
* �������:  2014��10��28��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret && ret!=100) {\
		sprintf( acErrMsg, "sql error[%d]",ret ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "gl_sub_c.h"
#include "com_item_c.h"
#define PAGE_CNT 22

struct gl_sub_c	s_gl_sub;
struct gl_sub_c	sGl_sub;
struct gl_sub_c	gl_sub_low; /*�¼�֧��*/
struct com_item_c s_com_item;
int flag=0;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
char acc_type[2];
int opncnt;
FILE *fp; 
int ret = 0;

int get_ratio_lsqd(int bli,int bll,char str[100]);

rpt91d( )
{
	int tag = 0;
	int    ttlnum;
	int    loop_cnt=0; /** aaa **/
	int    cnt=0;  /** aaa***/
	char   old_acc_head[2];
	char   filename[100];
	char   old_br_no[8];
	char   lst_br_no[8];
	
	int    head_flag = 0 ;/***add bye liuyue 20061129 �ڴ���6*��Ŀ��ʱ����Ϊ�е�֧�и����Ŀ��Ϊ0,�����˴�ӡʱ�򲻴��ͷ�����***/

	struct com_sys_parm_c s_com_sys_parm;

	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
         
	ret=sql_begin(); /*������*/
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	old_acc_head[0] = 'A';
	old_acc_head[1] = '\0';
	memset(old_br_no,0,sizeof(old_br_no));
	memset(lst_br_no,0,sizeof(lst_br_no));

	/* ��ʼ��ȫ�ֱ��� */
	pub_base_sysinit();
    
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;

	/*strcpy( filename , "BRrbbsub" );	*/
	strcpy( filename , "RPT91d" );
	/*strcpy( vpfm, "bbrjbsub" );*/
	strcpy( vpfm, "RPT91d");

	/*ret = Gl_sub_Dec_Sel( g_pub_tx.reply, "cur_no='01' ORDER BY br_no,acc_hrt" );*/
	ret = RPT_Gl_Dec_Sel ( g_pub_tx.reply ,"gl_sub","com_item", "gl_sub.cur_no='01' and gl_sub.br_no='10101' and \
	      gl_sub.acc_hrt = com_item.acc_hrt order by gl_sub.br_no,com_item.acc_no" );
	ERR_DEAL
	
	ttlnum=0;
	loop_cnt=0; /** aaa ***/
	while(1)
	{
/*		ret = Gl_sub_Fet_Sel( &s_gl_sub,g_pub_tx.reply );*/
		ret = RPT_Gl_Fet_Sel( &s_gl_sub,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				if( tag==0 )
				{ 
					flag = 1;					 /** aaa ***/
					memcpy(s_gl_sub.br_no,lst_br_no,sizeof(lst_br_no)-1); /* add by martin 20061220*/
					ret = print_body_A( );  /** aaa ***/
					ret = print_tail_B( );
					/*fprintf(fp,"%c%c",0x1b,0x40);*/
				}
				else if( tag==2|| tag==1 ) /** ����6��Ŀ������ **/
					ret = print_tail_C( );
				ERR_DEAL
				fclose(fp);   /** aaa **/
			}
			else 
			{
				sprintf( acErrMsg, "����û��Ҫ��ӡ����ˮ!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		if(!memcmp("59901",s_gl_sub.br_no,5))
	  {
	  	continue;
	  }
		/* ֻ��һ����Ŀ 
	  ret=Com_item_Sel(g_pub_tx.reply, &s_com_item, " acc_lvl='1' and  acc_hrt='%s'", s_gl_sub.acc_hrt);
	  if(ret)
	  {
	  	continue;
	  }
	  */
	  /*if(memcmp("406",s_gl_sub.acc_hrt,3))�����406��Ŀ���ж��Ƿ��п�ֵ add by martin 20061128*/
	  
	  if( !pub_base_CompDblVal(s_gl_sub.ydr_bal,0.00) &&
			  !pub_base_CompDblVal(s_gl_sub.ycr_bal,0.00) &&
		  	!pub_base_CompDblVal(s_gl_sub.dr_bal,0.00) &&
		  	!pub_base_CompDblVal(s_gl_sub.cr_bal,0.00) &&
		  	!pub_base_CompDblVal(s_gl_sub.ydr_amt,0.00) &&
		  	!pub_base_CompDblVal(s_gl_sub.ycr_amt,0.00)  )
			/*{
			       	   continue;
			}*/
			{
				memset(&sGl_sub, 0x00, sizeof(struct gl_sub_c));
				memset(&s_com_item,0x00,sizeof(struct com_item_c));
			  ret = Com_item_Sel( g_pub_tx.reply, &s_com_item, "acc_hrt='%s'", s_gl_sub.acc_hrt );
			  pub_base_strpack( s_com_item.acc_no ); 
			  
				if( strlen(s_com_item.acc_no)==7 )
				{
			        continue;
			  }else{
			  
			       	ret=Gl_sub_Sel( g_pub_tx.reply , &sGl_sub , "br_no='%s' and cur_no='01' and acc_hrt like '%s%%' \
			       	and (ydr_bal<>0 or ycr_bal<>0 or dr_bal<>0 or cr_bal<>0 or ydr_amt<>0 or ycr_amt<>0)" ,s_gl_sub.br_no, s_com_item.acc_no);
			       	 if( ret==100 ) 
			       	 {
			       	   continue;
			         }
			      }
			 }
	  
    sprintf(acErrMsg,"++++br_no=[%s],acc_hrt=[%s]!",s_gl_sub.br_no,s_gl_sub.acc_hrt);
	  WRITEMSG
    if(memcmp(old_br_no,s_gl_sub.br_no,sizeof(s_gl_sub.br_no)-1) )
    {
    	head_flag = 0;  /**ÿ��һ����������Ҫ��ʼ��  liuyue  20061129***/
    	if(strlen(old_br_no))
    	{
    		 tag = 0;
    	   old_acc_head[0] = 'A';
	       old_acc_head[1] = '\0';
	       ret = print_tail_C( );
			   ERR_DEAL
			   fclose(fp);
			 
	    }
			page = 1;
			line_file = 0;
			loop_cnt = 0;   /** aaa **/
			cnt = sql_count( "gl_sub" , "cur_no='01' and br_no='%s'", s_gl_sub.br_no ); /** aaaa **/
		  sprintf(acErrMsg,"����[%s]�м�¼[%d]",s_gl_sub.br_no,cnt);
	    WRITEMSG
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( s_gl_sub.br_no , filename , 0 );
			ERR_DEAL

			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_gl_sub.br_no , filename );
			ERR_DEAL
			
			
	    memset(old_br_no,0,sizeof(old_br_no));
   	  memcpy(old_br_no,s_gl_sub.br_no,sizeof(s_gl_sub.br_no)-1); 
   	  memcpy(lst_br_no,s_gl_sub.br_no,sizeof(s_gl_sub.br_no)-1);
   	  fprintf(fp,"%c%c",0x1b,0x40);/*0607 martin*/
   	}

		/* ��Ŀ��任ʱ */
		if( s_gl_sub.acc_hrt[0] != old_acc_head[0] )
		{
			if( old_acc_head[0] == 'A' )				/* �ʲ��� */
			{
				strcpy( acc_type , "1" );
				old_acc_head[0] = '1';
				ERR_DEAL
				
				/*** add by martin ��Ŀ�o�N�~***/
			 ret=Gl_sub_Sel( g_pub_tx.reply , &sGl_sub , "br_no='%s' and cur_no='01' and acc_hrt like '%s%%' \
			     and (ydr_bal<>0 or ycr_bal<>0 or dr_bal<>0 or cr_bal<>0 or ydr_amt<>0 or ycr_amt<>0)" ,s_gl_sub.br_no, acc_type );
			     if( ret==100 ) 
			      { 
			       	   strcpy( acc_type , "2" );
			       	   old_acc_head[0] = '2';
			       	   continue;
			      }
			      
				ret = print_head( );
				ERR_DEAL
			}
			else 
			{
				/* ��ӡҳβ */
				if( old_acc_head[0] == '5' )
				{
					line_file = 0;  /*add by xg*/
					flag = 1;
					ret = print_tail_A( );
					/***fprintf(fp,"\f"); add by liuyue 20061129***/
					ERR_DEAL
					flag = 0;
				}
				else 
				{
				    if( tag==2 || tag==1 ) /** ����6��Ŀ������ **/
						   ret = print_tail_C( );
					  ERR_DEAL
				}

				switch( old_acc_head[0] )
				{
					case '1':							/* ��ծ�� */
							strcpy( acc_type , "2" );
							break;
					case '2':							/* ������Ȩ���� */
							strcpy( acc_type , "3" );
							break;
					case '3':							/* �ʲ���ծ��ͬ�� */
							strcpy( acc_type , "4" );
							break;
					case '4':							/* ������ */
							strcpy( acc_type , "5" );
							break;
					case '5':							/* ������ */
							tag = 1;					/*�����־ */
							strcpy( acc_type , "6" );
							break;
					case '6':							/* ������ */
							tag = 2;					/* �����־ */
							strcpy( acc_type , "7" );
							break;
				 	default: 
							break;
				}
				/*** add by martin ��Ŀ�o�N�~***/
			 ret=Gl_sub_Sel( g_pub_tx.reply , &sGl_sub , "br_no='%s' and cur_no='01' and acc_hrt like '%s%%' \
			     and (ydr_bal<>0 or ycr_bal<>0 or dr_bal<>0 or cr_bal<>0 or ydr_amt<>0 or ycr_amt<>0)" ,s_gl_sub.br_no, acc_type );
			     if( ret==100 ) 
			      { 
			       	   switch( acc_type[0] )
				               {
				               	case '1':							/* ��ծ�� */
				               			strcpy( acc_type , "2" );
				               			break;
				               	case '2':							/* ������Ȩ���� */
				               			strcpy( acc_type , "3" );
				               			break;
				               	case '3':							/* �ʲ���ծ��ͬ�� */
				               			strcpy( acc_type , "4" );
				               			break;
				               	case '4':							/* ������ */
				               			strcpy( acc_type , "5" );
				               			break;
				               	case '5':							/* ������ */
				               			tag = 1;					/*�����־ */
				               			strcpy( acc_type , "6" );
				               			break;
				               	case '6':							/* ������ */
				               			tag = 2;					/* �����־ */
				               			strcpy( acc_type , "7" );
				               			break;
				                	default: 
				               			break;
				               }
			      }
			}

			old_acc_head[0] = s_gl_sub.acc_hrt[0] ;
			
			if( tag==1 || tag==2 ) /** ����6��Ŀ������ **/
			{
				page++;
				ret = print_head_C( );
			 	 ERR_DEAL 
			 	 head_flag ++;
		  } else if(tag != 2){         /* ���ⲻ����� */
			 	 ret = print_body_A( );
			   ERR_DEAL
			   line_file++;
			}
			
			
			if( line_file>PAGE_CNT )			
		  {
			    line_file=0;
			    page++;
      
			    /* ��ӡҳβ */
			    if( tag==0 )
			    	ret = print_tail_B();
			    else
			    	ret = print_tail_C();
			    ERR_DEAL
      
      
      
      			/******add by liuyue 20061128,�ӱ�β��,�г���ǩ����,ͬʱ�Ŵ��о�
      			    fprintf(fp,"\f");***/
      			    
      			    
			    /* ��ӡҳ�� */
			    if( tag==0 )
			    {
			    	ret = print_head();
			    	ERR_DEAL
			    }
			    else
			    {
			    	ret = print_head_C();
			    	ERR_DEAL
			    	head_flag ++;
			    }
		  }
		}

    	/* ��ӡ��Ŀ��Ϣ */
		if( tag==0 )
			ret = print_body_B( );
		else 
		{
			printf("[%s][%d]head==[%d]\n",__FILE__,__LINE__,head_flag);
			if(head_flag==0)  /**by liuyue 20061129***/
			{
				ret = print_head_C();
			    	ERR_DEAL
			    	head_flag ++;
			}	
			ret = print_body_C( );
		}
		ERR_DEAL

		line_file++;
    loop_cnt++; /** aaa **/
		if( line_file>PAGE_CNT )			
		{
			line_file=0;
			page++;

      	
			/* ��ӡҳβ */
			if( tag==0 )
				ret = print_tail_B();
			else
				ret = print_tail_C();
			ERR_DEAL
		/******add by liuyue 20061128,�ӱ�β��,�г���ǩ����,ͬʱ�Ŵ��о�
      			    fprintf(fp,"\f"); ***/
      			    
      			    
      if(loop_cnt>=cnt) /** aaa **/
      	continue;       /** aaa **/
      	
			/* ��ӡҳ�� */
			if( tag==0 )
				ret = print_head();
			else
				ret = print_head_C();
			ERR_DEAL
		}
		ttlnum++;
		
	}

	Gl_sub_Clo_Sel( );
	
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"֧���걨�����ɴ���ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"֧���걨�����ɴ���ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	if(!strcmp(vrec,"GG"))
		printf("[%s][%d],GGGGGGGGGGGGGGG!!!!!!!\n",__FILE__,__LINE__);
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ���ڻ�ҳ��ӡ���� */
int print_head( )
{
	char vrec[3];
	
	if( page != 1 )
		{
			fprintf ( fp , "\f" );
		}
	vtcp_log("AAAAAAAAAAA----------------------[%s]",acc_type );
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ���ڴ�ӡ���� */
int print_head_B( )
{
	char vrec[3];
	
	if( page != 1 )
		{
			fprintf ( fp , "\f" );
		}
	vtcp_log("BBBBBBBBBBBB----------------------[%s]",acc_type );
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);

	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

	strcpy( vrec,"NN" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* �����ӡ���� */
int print_head_C( )
{
	char vrec[3];
	
	if( page != 1 )
		{
			fprintf ( fp , "\f" );
		}
	vtcp_log("AAAAAAAAAAA----------------------[%s]",acc_type );
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"DD" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}
/* ���ڴ�ӡ��β */
int print_tail_B( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	/**by liuyue 20061129***/
	strcpy( vrec,"GG" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* �����ӡ��β */
int print_tail_C( )
{
	char vrec[3];

	strcpy( vrec,"FF" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
		/**by liuyue 20061129***/
	strcpy( vrec,"GG" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	/*fprintf(fp,"%c%c",0x1b,0x40);*/
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ���ڴ�ӡ���� */
int print_body_A( )
{
	char vrec[3];

	strcpy( vrec,"NN" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}
/* ���ڴ�ӡ���� */
int print_body_B( )
{
	char vrec[3];

	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* �����ӡ���� */
int print_body_C( )
{
	char vrec[3];

	strcpy( vrec,"EE" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* �������п�Ŀ���ܴ�ӡ���� */
int print_tail_A( )
{
	char vrec[3];

	strcpy( vrec,"NN" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
  ERR_DEAL
  
  	/**by liuyue 20061129***/
	strcpy( vrec,"GG" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ��ֵ���� */
/* ˵����bli ��������ַ����� ... bll�������ռ�ĳ��� ... str�������ֵ */
int get_ratio_lsqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	struct com_item_c s_com_item;

	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[41];
	char amt_tmp[41]; 
	char tmp_inf[41];
	char tmp_acc_name[41];
	char fmt[20]; 
	char br_name[71];
	double tmp_amt=0,tmp_amt1=0;
	int i=0; 

	memset(&gl_sub_low, 0x00, sizeof(struct gl_sub_c));
	ret=Gl_sub_Sel( g_pub_tx.reply , &gl_sub_low , "br_no='11901' and cur_no='01' and acc_hrt='%s' " , s_gl_sub.acc_hrt);

	switch( bli ) 
	{ 
		case 'A': /* �������� */ 
			sprintf( t_str , "%4d��%2d��%2d��" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* ��Ŀ���� */
			pub_base_strpack( s_gl_sub.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item,"acc_hrt='%s'", \
								s_gl_sub.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'C': /* �������� */
			ret = pub_base_getbrname( s_gl_sub.br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s  ����  ( %5s )" , br_name , s_gl_sub.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* �������� */
			/*if(memcmp("406",s_gl_sub.acc_hrt,3)==0) 406��ӡ40602 
			{
			  ret = sql_sum_double( "gl_sub","ydr_bal",&tmp_amt,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
		  	
			  sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			  if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  pub_rpt_flttomon( amt_tmp,amt_tmp );
			  if( strlen(amt_tmp)>bll ) 
			  	sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			  sprintf(fmt,"%%%ds",bll); 		
			  sprintf( str, fmt, amt_tmp ); 
			  break;
		  }*/
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_bal + gl_sub_low.ydr_bal); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_bal+ gl_sub_low.ydr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'F': /* �������� */
			/*if(memcmp("406",s_gl_sub.acc_hrt,3)==0) 406��ӡ40601 
			{
			  ret = sql_sum_double( "gl_sub","ycr_bal",&tmp_amt,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
			  	
			  sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			  if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  pub_rpt_flttomon( amt_tmp,amt_tmp );
			  if( strlen(amt_tmp)>bll ) 
			  	sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			  sprintf(fmt,"%%%ds",bll); 		
			  sprintf( str, fmt, amt_tmp ); 
			  break;
		  }*/
		  
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.ycr_bal + gl_sub_low.ycr_bal); 
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ycr_bal + gl_sub_low.ycr_bal); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'G': /* ����跢���� */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_amt + gl_sub_low.ydr_amt); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_amt + gl_sub_low.ydr_amt); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* ����������� */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.ycr_amt + gl_sub_low.ycr_amt); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ycr_amt + gl_sub_low.ycr_amt); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Q': /* ��ĩ����� */
			/*if(memcmp("406",s_gl_sub.acc_hrt,3)==0) 406��ӡ40602 
			{
			  ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
			  	
			  sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			  if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  pub_rpt_flttomon( amt_tmp,amt_tmp );
			  if( strlen(amt_tmp)>bll ) 
			  	sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			  sprintf(fmt,"%%%ds",bll); 		
			  sprintf( str, fmt, amt_tmp ); 
			  break;
		  }*/
		  
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal + gl_sub_low.dr_bal ); 
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal + gl_sub_low.dr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'R': /* ��ĩ����� */
			/*if(memcmp("406",s_gl_sub.acc_hrt,3)==0) 406��ӡ40601 
			{
			  ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
			  	
			  sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			  if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  pub_rpt_flttomon( amt_tmp,amt_tmp );
			  if( strlen(amt_tmp)>bll ) 
			  	sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			  sprintf(fmt,"%%%ds",bll); 		
			  sprintf( str, fmt, amt_tmp ); 
			  break;
		  }*/
		  
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.cr_bal + gl_sub_low.cr_bal); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.cr_bal + gl_sub_low.cr_bal); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'X': /* ���� */
			if( flag==0 )
			{
				if( acc_type[0]=='1' )
					strcpy( tmp_acc_name,"�ʲ���ϼ�" );
				else if( acc_type[0]=='2' )
					strcpy( tmp_acc_name,"��ծ��ϼ�" );
				else if( acc_type[0]=='3' )
					strcpy( tmp_acc_name,"������Ȩ����ϼ�" );
				else if( acc_type[0]=='4' )
					strcpy( tmp_acc_name,"�ʲ���ծ��ϼ�" );
				else if( acc_type[0]=='5' )
					strcpy( tmp_acc_name,"������ϼ�" );
			}
			else
				strcpy( tmp_acc_name,"���ڿ�Ŀ��ϼ�" );

			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
/*		case 'Y':  ���� 
			strcpy( tmp_acc_name, "���ڿ�Ŀ�ϼ�" );
			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;*/
		case 'I': /* ��Ŀ�� */
			pub_base_strpack( s_gl_sub.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item, "acc_hrt='%s'", \
								s_gl_sub.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'K': /* �������� */
			vtcp_log( "acc_type====================[%s]",acc_type );
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","ydr_bal",&tmp_amt,"acc_hrt[1]='%c' and br_no in('10101','11901') and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0]);
			 /* if( acc_type[0]=='4' )
			  {
			  ret = sql_sum_double( "gl_sub","ydr_bal",&tmp_amt1,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  }*/
			}
			else 
			{
				ret = sql_sum_double( "gl_sub","ydr_bal",&tmp_amt,"acc_hrt[1]>='1' and br_no in('10101','11901') and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			/*  ret = sql_sum_double( "gl_sub","ydr_bal",&tmp_amt1,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;*/
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt );
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  } 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L': /* �������� */
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","ycr_bal",&tmp_amt,"acc_hrt[1]='%c' and br_no in('10101','11901') and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0] );
			  /*if(acc_type[0]=='4')
			  {
			  ret = sql_sum_double( "gl_sub","ycr_bal",&tmp_amt1,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  }*/
			}
			else 
			{
				ret = sql_sum_double( "gl_sub","ycr_bal",&tmp_amt,"acc_hrt[1]>='1' and br_no in('10101','11901') and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			/*  ret = sql_sum_double( "gl_sub","ycr_bal",&tmp_amt1,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;*/
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* ����跢���� */
			if( flag==0  )
			{
				ret = sql_sum_double( "gl_sub","ydr_amt",&tmp_amt,"acc_hrt[1]='%c'  and br_no in('10101','11901') and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0] );
			}
			else
			{
				ret = sql_sum_double( "gl_sub","ydr_amt",&tmp_amt,"acc_hrt[1]>='1' and br_no in('10101','11901') and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /* ����������� */
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","ycr_amt",&tmp_amt,"acc_hrt[1]='%c' and br_no in('10101','11901') and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' )", acc_type[0] );
			}
			else
			{
				ret = sql_sum_double( "gl_sub","ycr_amt",&tmp_amt,"acc_hrt[1]>='1' and br_no in('10101','11901') and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'U': /* ��ĩ����� */
			if( flag==0  )
			{
				ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt,"acc_hrt[1]='%c'  and br_no in('10101','11901') and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0] );
			/*  if(acc_type[0]=='4')
			  {
			  ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt1,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  }*/
			}
			else
			{
				ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt,"acc_hrt[1]>='1' and br_no in('10101','11901') and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			/*  ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt1,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;*/
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'V': /* ��ĩ����� */
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt,"acc_hrt[1]='%c' and br_no in('10101','11901') and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' )", acc_type[0] );
			 /* if(acc_type[0]=='4')
			  {
			  ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt1,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  }*/
			}
			else
			{
				ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt,"acc_hrt[1]>='1' and br_no in('10101','11901') and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " );
			/*  ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt1,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;*/
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
			case 'Y': /* ������ */
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_bal+s_gl_sub.ycr_bal+ gl_sub_low.ydr_bal  + gl_sub_low.ycr_bal ); 
				
				if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.ydr_bal+s_gl_sub.ycr_bal + gl_sub_low.ydr_bal  + gl_sub_low.ycr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
			case 'Z': /* ��ĩ��� */
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal+s_gl_sub.cr_bal + gl_sub_low.dr_bal  + gl_sub_low.cr_bal ); 
				
				if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			  {
			  	memset(str,' ',bll );
			  	break;
			  }
			  
			  
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal+s_gl_sub.cr_bal + gl_sub_low.dr_bal  + gl_sub_low.cr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}

/*************************************************************
* 文 件 名:  rpt912.c
* 功能描述： 支行季报表打印
* 作    者:  
* 完成日期:  2006年10月09日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
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

rpt912( )
{
	int tag = 0;
	int    ttlnum;
	int    loop_cnt=0; /** aaa **/
	int    cnt=0;  /** aaa***/
	char   old_acc_head[2];
	char   filename[100];
	char   old_br_no[8];
	char   lst_br_no[8];
	
	int	bn_tail_flag	=	0;	/**by liuyue  结局表外科目没有发生额时候没有打印表内汇总的问题***/
	
	int    head_flag = 0 ;/***add bye liuyue 20061129 在处理6*科目的时候因为有的支行该类科目都为0,导致了打印时候不打表头的情况***/


	struct com_sys_parm_c s_com_sys_parm;

	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
         
	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	old_acc_head[0] = 'A';
	old_acc_head[1] = '\0';
	memset(old_br_no,0,sizeof(old_br_no));
	memset(lst_br_no,0,sizeof(lst_br_no));

	/* 初始化全局变量 */
	pub_base_sysinit();
    
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;

	/*strcpy( filename , "BRrbbsub" );	*/
	strcpy( filename , "RPT912" );
	/*strcpy( vpfm, "bbrjbsub" );*/
	strcpy( vpfm, "RPT912");

	/*ret = Gl_sub_Dec_Sel( g_pub_tx.reply, "cur_no='01' ORDER BY br_no,acc_hrt" );*/
	ret = RPT_Gl_Dec_Sel ( g_pub_tx.reply ,"gl_sub","com_item", "gl_sub.cur_no='01' and \
	      gl_sub.acc_hrt = com_item.acc_hrt order by gl_sub.br_no,com_item.acc_no" );
	ERR_DEAL
	
	ttlnum=0;
	loop_cnt=0; /** aaa ***/
	while(1)
	{
		/*ret = Gl_sub_Fet_Sel( &s_gl_sub,g_pub_tx.reply );*/
		ret = RPT_Gl_Fet_Sel( &s_gl_sub,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				if( tag==0 )
				{ 
					flag = 1;					 /** aaa ***/
					memcpy(s_gl_sub.br_no,lst_br_no,sizeof(lst_br_no)-1); /* add by martin 20061220*/
					ret = print_body_A( );  /** aaa ***/
					ret = print_tail_B( );
					/*fprintf(fp,"%c%c",0x1b,0x40);*/
				}
				else if( tag==2 || tag==1 ) /** 单列6科目晋中左 **/
					ret = print_tail_C( );
				ERR_DEAL
				fclose(fp);   /** aaa **/
			}
			else 
			{
				sprintf( acErrMsg, "本季没有要打印的流水!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		if(!memcmp("59901",s_gl_sub.br_no,5))
	  {
	  	continue;
	  }
		/* 只打一级科目 
	  ret=Com_item_Sel(g_pub_tx.reply, &s_com_item, " acc_lvl='1' and  acc_hrt='%s'", s_gl_sub.acc_hrt);
	  if(ret)
	  {
	  	continue;
	  }
	  */
	  /*if(memcmp("406",s_gl_sub.acc_hrt,3))如果是406科目不判断是否有空值 add by martin 20061128*/
	  	
	  if(   !pub_base_CompDblVal(s_gl_sub.qdr_bal,0.00) &&
		!pub_base_CompDblVal(s_gl_sub.qcr_bal,0.00) &&
		!pub_base_CompDblVal(s_gl_sub.dr_bal,0.00) &&
		!pub_base_CompDblVal(s_gl_sub.cr_bal,0.00) &&
		!pub_base_CompDblVal(s_gl_sub.qdr_amt,0.00) &&
		!pub_base_CompDblVal(s_gl_sub.qcr_amt,0.00)  )
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
			       	and (qdr_bal<>0 or qcr_bal<>0 or dr_bal<>0 or cr_bal<>0 or qdr_amt<>0 or qcr_amt<>0)" ,s_gl_sub.br_no, s_com_item.acc_no);
			       	 if( ret==100 ) 
			       	 {
			       	   continue;
			         }
			      }
			 }
	  
    sprintf(acErrMsg,"++++br_no=[%s],acc_hrt=[%s]!",s_gl_sub.br_no,s_gl_sub.acc_hrt);
	  WRITEMSG
	  
          bn_tail_flag   = 0;
	  
    if(memcmp(old_br_no,s_gl_sub.br_no,sizeof(s_gl_sub.br_no)-1) )
    {
    	head_flag = 0;  /**每到一个机构就需要初始化  liuyue  20061129***/
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
		  sprintf(acErrMsg,"机构[%s]有记录[%d]",s_gl_sub.br_no,cnt);
	    WRITEMSG
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_gl_sub.br_no , filename , 0 );
			ERR_DEAL

			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_gl_sub.br_no , filename );
			ERR_DEAL
			
			/***add by liuyue ****/ /**在换机构之前看看是否打印了表内科目的汇总信息，
		现在的程序 如果 6。7科目都没有发生额的话是不会打印表内汇总的，所以只要看看
		表外科目是否有值就可以判断出有没有打印出表内科目的汇总*******/
		 ret=Gl_sub_Sel( g_pub_tx.reply , &sGl_sub , "br_no='%s' and cur_no='01' and ( acc_hrt like '6%%' or acc_hrt like '7%%' ) \
			     and (qdr_bal<>0 or qcr_bal<>0 or dr_bal<>0 or cr_bal<>0 or qdr_amt<>0 or qcr_amt<>0)" ,s_gl_sub.br_no );
		if( ret==100 ) 
		 { 
		 	bn_tail_flag = 1 ;	/***说明等会需要特别处理的***/
		 }
		 /***add by liuyue  end *****/
			
	    memset(old_br_no,0,sizeof(old_br_no));
   	  memcpy(old_br_no,s_gl_sub.br_no,sizeof(s_gl_sub.br_no)-1); 
   	  memcpy(lst_br_no,s_gl_sub.br_no,sizeof(s_gl_sub.br_no)-1);
   	  fprintf(fp,"%c%c",0x1b,0x40);/*0607 martin*/
   	}

		/* 科目类变换时 */
		if( s_gl_sub.acc_hrt[0] != old_acc_head[0] )
		{
			if( old_acc_head[0] == 'A' )				/* 资产类 */
			{
				strcpy( acc_type , "1" );
				old_acc_head[0] = '1';
				ERR_DEAL
				
				/*** add by martin 科目無餘額***/
			 ret=Gl_sub_Sel( g_pub_tx.reply , &sGl_sub , "br_no='%s' and cur_no='01' and acc_hrt like '%s%%' \
			     and (qdr_bal<>0 or qcr_bal<>0 or dr_bal<>0 or cr_bal<>0 or qdr_amt<>0 or qcr_amt<>0)" ,s_gl_sub.br_no, acc_type );
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
				/* 打印页尾 */
				if( old_acc_head[0] == '5' )
				{
					line_file = 0;  /*add by xg*/
					flag = 1;
					ret = print_tail_A( );
					
					/*fprintf(fp,"\f");*/
					ERR_DEAL
					flag = 0;
				}
				else 
				{
				    if( tag==2|| tag==1 ) /** 单列6科目晋中左 **/
						   ret = print_tail_C( );
					  ERR_DEAL
				}

				switch( old_acc_head[0] )
				{
					case '1':							/* 负债类 */
							strcpy( acc_type , "2" );
							break;
					case '2':							/* 所有者权益类 */
							strcpy( acc_type , "3" );
							break;
					case '3':							/* 资产负债共同类 */
							strcpy( acc_type , "4" );
							break;
					case '4':							/* 损益类 */
							strcpy( acc_type , "5" );
							break;
					case '5':							/* 表外类 */
							tag = 1;					/*表外标志 */
							strcpy( acc_type , "6" );
							break;
					case '6':							/* 表外类 */
							tag = 2;					/* 表外标志 */
							strcpy( acc_type , "7" );
							break;
				 	default: 
							break;
				}
			/*** add by martin 科目無餘額***/
			 ret=Gl_sub_Sel( g_pub_tx.reply , &sGl_sub , "br_no='%s' and cur_no='01' and acc_hrt like '%s%%' \
			     and (qdr_bal<>0 or qcr_bal<>0 or dr_bal<>0 or cr_bal<>0 or qdr_amt<>0 or qcr_amt<>0)" ,s_gl_sub.br_no, acc_type );
			     if( ret==100 ) 
			      { 
			       	   switch( acc_type[0] )
				               {
				               	case '1':							/* 负债类 */
				               			strcpy( acc_type , "2" );
				               			break;
				               	case '2':							/* 所有者权益类 */
				               			strcpy( acc_type , "3" );
				               			break;
				               	case '3':							/* 资产负债共同类 */
				               			strcpy( acc_type , "4" );
				               			break;
				               	case '4':							/* 损益类 */
				               			strcpy( acc_type , "5" );
				               			break;
				               	case '5':							/* 表外类 */
				               			tag = 1;					/*表外标志 */
				               			strcpy( acc_type , "6" );
				               			break;
				               	case '6':							/* 表外类 */
				               			tag = 2;					/* 表外标志 */
				               			strcpy( acc_type , "7" );
				               			break;
				                	default: 
				               			break;
				               }
			      }
			}

			old_acc_head[0] = s_gl_sub.acc_hrt[0] ;
			
			if( tag==1|| tag==2 ) /** 单列6科目晋中左 **/
			{
				page++;
				ret = print_head_C( );
				head_flag ++;
			  ERR_DEAL 
		  } else if(tag != 2){         /* 表外不打汇总 */
			 	 ret = print_body_A( );
			   ERR_DEAL
			   line_file++;
			}
			
			
			if( line_file>PAGE_CNT )			
		  {
			    line_file=0;
			    page++;
      
			    /* 打印页尾 */
			    if( tag==0 )
			    	ret = print_tail_B();
			    else
			    	ret = print_tail_C();
			    ERR_DEAL
			    
			    /******add by liuyue 20061128,加表尾部,行长等签名栏,同时放大行距
      			    fprintf(fp,"\f");***/
			
			    /* 打印页首 */
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

    	/* 打印科目信息 */
		if( tag==0 )
		{	
			/***by liuyue 20061202***/
		
					ret = print_body_B( );        /***打印表内表体，一条一条的记录***/
		}
		else 
		{
			printf("[%s][%d]head==[%d]\n",__FILE__,__LINE__,head_flag);
			if(head_flag==0)  /**by liuyue 20061129***/
			{
				ret = print_head_C();   /*表外表头***/
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

      	
			/* 打印页尾 */
			if( tag==0 )
				ret = print_tail_B();
			else
				ret = print_tail_C();
			ERR_DEAL
	
	                /**fprintf(fp,"\f");**/
	                
      if(loop_cnt>=cnt) /** aaa **/
      	continue;       /** aaa **/
      	
			/* 打印页首 */
			if( tag==0 )
				ret = print_head();
			else
				ret = print_head_C();
			ERR_DEAL
		}
		ttlnum++;
		
	}

	RPT_Gl_Clo_Sel( );
	/*Gl_sub_Clo_Sel( );*/
	
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"支行季报表生成处理成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"支行季报表生成处理失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* 表内换页打印表首 */
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

/* 表内打印表首 */
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

/* 表外打印表首 */
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
/* 表内打印表尾 */
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

/* 表外打印表尾 */
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

/* 表内打印表体 */
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
/* 表内打印表体 */
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

/* 表外打印表体 */
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

/* 表内所有科目汇总打印表体 */
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

/* 赋值函数 */
/* 说明：bli 代表变量字符名称 ... bll代表变量占的长度 ... str代表变量值 */
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
	double tmp_amt,tmp_amt1=0;
	int i=0; 

	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */ 
			sprintf( t_str , "%4d年%2d月%2d日" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 科目名称 */
			pub_base_strpack( s_gl_sub.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item,"acc_hrt='%s'", \
								s_gl_sub.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'C': /* 机构名称 */
			ret = pub_base_getbrname( s_gl_sub.br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_gl_sub.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 季初借余额 */
			/*sprintf( amt_tmp, "%.2lf" , s_gl_sub.qdr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.qdr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;*/
			/* if(memcmp("406",s_gl_sub.acc_hrt,3)==0) 406打印40602 
			{
			  ret = sql_sum_double( "gl_sub","qdr_bal",&tmp_amt,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
		  	
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
		  
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.qdr_bal ); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.qdr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'F': /* 季初贷余额 */
			/*sprintf( amt_tmp, "%.2lf" , s_gl_sub.qcr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.qcr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;*/
			
			/*if(memcmp("406",s_gl_sub.acc_hrt,3)==0) 406打印40601 
			{
			  ret = sql_sum_double( "gl_sub","qcr_bal",&tmp_amt,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
			  	
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
		  
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.qcr_bal ); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.qcr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'G': /* 本季借发生额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.qdr_amt ); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.qdr_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* 本季贷发生额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.qcr_amt ); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.qcr_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Q': /* 季末借余额 */
			/*if(memcmp("406",s_gl_sub.acc_hrt,3)==0) 406打印40602 
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
		  
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal ); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'R': /* 季末贷余额 */
			/*if(memcmp("406",s_gl_sub.acc_hrt,3)==0) 406打印40601 
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
		  
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.cr_bal ); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.cr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'X': /* 标题 */
			if( flag==0 )
			{
				if( acc_type[0]=='1' )
					strcpy( tmp_acc_name,"资产类合计" );
				else if( acc_type[0]=='2' )
					strcpy( tmp_acc_name,"负债类合计" );
				else if( acc_type[0]=='3' )
					strcpy( tmp_acc_name,"所有者权益类合计" );
				else if( acc_type[0]=='4' )
					strcpy( tmp_acc_name,"资产负债类合计" );
				else if( acc_type[0]=='5' )
					strcpy( tmp_acc_name,"损益类合计" );
			}
			else
				strcpy( tmp_acc_name,"表内科目类合计" );

			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
/*		case 'Y':  汇总 
			strcpy( tmp_acc_name, "表内科目合计" );
			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;*/
		case 'I': /* 科目号 */
			pub_base_strpack( s_gl_sub.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item, "acc_hrt='%s'", \
								s_gl_sub.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'K': /* 季初借余额 */
			vtcp_log( "acc_type====================[%s]",acc_type );
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","qdr_bal",&tmp_amt,"acc_hrt[1]='%c' and br_no='%s' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0],s_gl_sub.br_no );
			  /*if( acc_type[0]=='4' )
			  {
			  ret = sql_sum_double( "gl_sub","qdr_bal",&tmp_amt1,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  }*/
			}
			else 
			{
				ret = sql_sum_double( "gl_sub","qdr_bal",&tmp_amt,"acc_hrt[1]>='1' and br_no='%s' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " ,s_gl_sub.br_no);
			/*  ret = sql_sum_double( "gl_sub","qdr_bal",&tmp_amt1,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
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
		case 'L': /* 季初贷余额 */
			if( flag==0 )
			{
				/*ret = sql_sum_double( "gl_sub","qcr_bal",&tmp_amt,"substr(acc_hrt,1,1)='%c' and br_no='%s' and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' and acc_hrt !='40600')  ", acc_type[0] ,s_gl_sub.br_no);*/
			  ret = sql_sum_double( "gl_sub","qcr_bal",&tmp_amt,"substr(acc_hrt,1,1)='%c' and br_no='%s' and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' )  ", acc_type[0] ,s_gl_sub.br_no);
			 /* if(acc_type[0]=='4')
			  {
			  ret = sql_sum_double( "gl_sub","qcr_bal",&tmp_amt1,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  }*/
			}
			else 
			{
				/*ret = sql_sum_double( "gl_sub","qcr_bal",&tmp_amt,"substr(acc_hrt,1,1)>='1' and br_no='%s' and substr(acc_hrt,1,1)<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' and acc_hrt!='40600' ) ",s_gl_sub.br_no );*/
			 ret = sql_sum_double( "gl_sub","qcr_bal",&tmp_amt,"substr(acc_hrt,1,1)>='1' and br_no='%s' and substr(acc_hrt,1,1)<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' ) ",s_gl_sub.br_no );
			 /* ret = sql_sum_double( "gl_sub","qcr_bal",&tmp_amt1,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
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
		case 'O': /* 本季借发生额 */
			if( flag==0  )
			{
				ret = sql_sum_double( "gl_sub","qdr_amt",&tmp_amt,"acc_hrt[1]='%c' and br_no='%s' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0] ,s_gl_sub.br_no);
			}
			else
			{
				ret = sql_sum_double( "gl_sub","qdr_amt",&tmp_amt,"acc_hrt[1]>='1' and br_no='%s' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ",s_gl_sub.br_no );
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
		case 'P': /* 本季贷发生额 */
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","qcr_amt",&tmp_amt,"acc_hrt[1]='%c' and br_no='%s' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' )", acc_type[0],s_gl_sub.br_no );
			}
			else
			{
				ret = sql_sum_double( "gl_sub","qcr_amt",&tmp_amt,"acc_hrt[1]>='1' and br_no='%s' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) " ,s_gl_sub.br_no);
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
		case 'U': /* 季末借余额 */
			if( flag==0  )
			{
				ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt,"acc_hrt[1]='%c' and br_no='%s' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", acc_type[0],s_gl_sub.br_no );
			  /*if(acc_type[0]=='4')
			  {
			  ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt1,"acc_hrt='40602' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  }*/
			}
			else
			{
				ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt,"acc_hrt[1]>='1' and br_no='%s' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ",s_gl_sub.br_no );
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
		case 'V': /* 季末贷余额 */
			if( flag==0 )
			{
				/*ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt,"acc_hrt[1]='%c' and br_no='%s' and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' and acc_hrt!='40600' )", acc_type[0],s_gl_sub.br_no );*/
			  ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt,"acc_hrt[1]='%c' and br_no='%s' and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' )", acc_type[0],s_gl_sub.br_no );
			  /*if(acc_type[0]=='4')
			  {
			  ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt1,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
				tmp_amt+=tmp_amt1;
			  }*/
			}
			else
			{
				/*ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt,"acc_hrt[1]>='1'  and br_no='%s' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' and acc_hrt!='40600' ) " ,s_gl_sub.br_no);*/
			  ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt,"acc_hrt[1]>='1'  and br_no='%s' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl='1' ) " ,s_gl_sub.br_no);
			  /*ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt1,"acc_hrt='40601' and br_no='%s' ",s_gl_sub.br_no);
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
		case 'Y': /* 季初余额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.qdr_bal+s_gl_sub.qcr_bal ); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.qdr_bal+s_gl_sub.qcr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Z': /* 季末余额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal+s_gl_sub.cr_bal ); 
			if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal+s_gl_sub.cr_bal ); 
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

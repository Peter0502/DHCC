#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_prt.c
* 功能描述：补登折函数 pub_base_patch_book();
* 作    者: xxx
* 完成日期：2003/01/12
* 修改记录：  
* 1. 日   期:
*    修 改 人:
*    修改内容:
* 2. ...
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_unprt_hst_c.h"
#include "mdm_attr_c.h"
#include "mo_opn_cls_c.h"
static	FILE *fp;

/**********************************************************************
 * 函 数 名：pub_base_patch_book();
 * 函数功能：补登未打印的存折明细
 *
 * 作者/时间:
 *
 * 参数：
 *     输入：介质属性
 *     输出：未打印笔数
 *           生成未打印文件
 * 返回值：
 *       返回0成功，
 *       返回非0不成功
 * 修改历史：
 *
********************************************************************/
int pub_base_patch_book(struct mdm_attr_c mdm_attr_c)
{
	int   unprt_cnt;
	int   prt_cnt;
	int   prt_cnt1;	
	char	filename[51];
	char	tmpname[101];

	unprt_cnt=0;
	prt_cnt=0;
	prt_cnt1=0;

	pub_base_AllDwnFilName(tmpname);
	vtcp_log("tmpname======================[%s]",tmpname);
	fp=fopen(tmpname,"a");
	if (fp==NULL)
	{
		sprintf(acErrMsg,"生成文件错误![%s]",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P157");
		goto ErrExit;
	}

	unprt_cnt=sql_count("mdm_unprt_hst","ac_id=%ld " ,g_pub_tx.ac_id);
	if( unprt_cnt<0 )
	{
		sprintf(acErrMsg,"此户没用未打折明细![%d]",unprt_cnt);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P158");
		goto ErrExit;
	}
	if( unprt_cnt==0 && strcmp(g_pub_tx.tx_code,CHGBOOK_CODE) )
	{
		prt_cnt=0;
		sprintf(acErrMsg,"此户没用未打折明细!");
		WRITEMSG
		goto GoodExit;	
	}

	vtcp_log("unprt_cnt====================[%d]",unprt_cnt);
	/*XRB if( unprt_cnt<LENG || !strcmp(g_pub_tx.tx_code,CHGBOOK_CODE) ) */
	/*根据长治商行会计科王亚萍的要求将所有未登折明细都打成存折*/
	/*
	if( unprt_cnt<LENG )
	{*/
	vtcp_log("[%s][%d] ac_num_ind=%s\n",__FILE__,__LINE__,mdm_attr_c.ac_num_ind);		
	if( mdm_attr_c.ac_num_ind[0]=='N' )		/**多账户(一本通)**/
		{
			prt_cnt=pub_base_prt_book_ybt(mdm_attr_c,unprt_cnt);
			if( prt_cnt<0 )
			{
				sprintf(acErrMsg,"一本通生成打折文件出错![%d]",prt_cnt);
				WRITEMSG
				strcpy( g_pub_tx.reply, "P159");
				goto ErrExit;
			}
		}
		else
		{
			prt_cnt=pub_base_prt_book(mdm_attr_c,unprt_cnt);
			if( prt_cnt<0 )
			{
				sprintf(acErrMsg,"生成打折文件出错![%d]",prt_cnt);
				WRITEMSG
				/*
				strcpy( g_pub_tx.reply, "P159");
				*/
				goto ErrExit;
			}
		}
	/*
	}
	else
	{
		prt_cnt=pub_base_prt_reck(mdm_attr_c);
		vtcp_log("prt_cnt====================[%d]",unprt_cnt);
		if( prt_cnt<0 )
		{
			sprintf(acErrMsg,"生成对账单文件出错![%d]",prt_cnt);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P159");
			goto ErrExit;
		}
	}
	*/

GoodExit:
	fclose(fp);
	sprintf(acErrMsg,"补登折成功!");
	WRITEMSG
	return prt_cnt;
ErrExit:
	fclose(fp);
	sprintf(acErrMsg,"补登折出错!");
	WRITEMSG
	return -1;
}

int pub_base_prt_book_ybt(struct mdm_attr_c mdm_attr_c,int unprt_cnt)
{
	int	totalnum=0;
	int	prtl=0;
	int prt_cnt=0;
	int l_term=0;
	char tmp_term[3];
	char term_type[3]; /**存期单位**/
	char brf[21];
	struct	mdm_unprt_hst_c	mdm_unprt_hst_c;
	struct	mdm_ac_rel_c	f_mdm_ac_rel;
	struct  mo_opn_cls_c	sMo_opn_cls;
	struct  td_mst_c		tmp_td_mst;

	MEMSET_DEBUG(&brf,0x00,21);
	MEMSET_DEBUG(&mdm_unprt_hst_c,0x00,sizeof(struct mdm_unprt_hst_c));
	MEMSET_DEBUG(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	MEMSET_DEBUG(&sMo_opn_cls,0x00,sizeof(struct mo_opn_cls_c));
	MEMSET_DEBUG(term_type,0x00,sizeof(term_type));
	prtl=0;
	prt_cnt=0;
	totalnum=0;
	totalnum=mdm_attr_c.pg_num*mdm_attr_c.pg_line;

	if( !strcmp(g_pub_tx.tx_code,CHGBOOK_CODE) )
	{
		g_mdm_ac_rel.prt_line=-1;
    	g_mdm_ac_rel.book_bal=0.00;
		if( (g_mdm_ac_rel.prt_line%totalnum+unprt_cnt)<totalnum )
		{
			unprt_cnt=unprt_cnt;
		}
		else
		{
			sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}

vtcp_log("g_mdm_ac_rel.prt_line=[%d],book_bal=[%lf]",g_mdm_ac_rel.prt_line,g_mdm_ac_rel.book_bal);

		g_reply_int=Td_mst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_sts='1' order by opn_date,ac_seqn ",g_pub_tx.ac_id);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取定期主文件错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}

		while(1)
		{
		g_reply_int=Td_mst_Fet_Sel(&g_td_mst,g_pub_tx.reply);
		if( g_reply_int==100 )	break;
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
		
		g_mdm_ac_rel.prt_line=g_mdm_ac_rel.prt_line+2;
		prt_cnt++;
		prtl=(g_mdm_ac_rel.prt_line+prt_cnt)%totalnum;

		/**-------------- 取开户操作员 ----------------**/
		g_reply_int=Mo_opn_cls_Sel(g_pub_tx.reply,&sMo_opn_cls,"ac_id=%ld and ac_seqn=%d ",g_td_mst.ac_id,g_td_mst.ac_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取开销户登记薄错误![%d][%d][%d]", \
					g_reply_int,g_td_mst.ac_id,g_td_mst.ac_seqn);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P230");
			return -1;
		}
		/**------------- 存期 -------------------**/
		g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s' ",g_td_mst.prdt_no);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取定期参数表错误错误![%d][%s]", \
					g_reply_int,g_td_mst.prdt_no);
			WRITEMSG
			strcpy( g_pub_tx.reply, "W026");
			return -1;
		}
		if( g_td_parm.term_type[0]=='M' || g_td_parm.term_type[0]=='D' )
			l_term=g_td_parm.term;
		else if( g_td_parm.term_type[0]=='Y' )
			l_term=g_td_parm.term*12;

		strncpy(brf,"换折",4);
		vtcp_log("---------------------------l_term===[%d]",l_term);
		sprintf(tmp_term,"%02d",l_term);
		vtcp_log("---------------------------tmp_term===[%s]",tmp_term);
		strcpy(term_type,"月");/**存期单位都用"月"**/
		vtcp_log("---------------------------term_type===[%s]",term_type);

	/*定期一本通*/
        fprintf(fp,"YBTZ%d|%ld|%s|%s|%s|%ld|%.2lf|%.2lf|%.5lf|%s|%d|%s|tty%s|%d|%s|\n", g_td_mst.ac_seqn,g_td_mst.opn_date,brf,tmp_term,term_type,g_td_mst.mtr_date,g_td_mst.bal,g_td_mst.bal,g_td_mst.rate,sMo_opn_cls.tel,prtl,g_td_mst.opn_br_no,g_pub_tx.tty,g_td_mst.book_line,"2104");

		g_mdm_ac_rel.book_bal+=g_td_mst.bal;

		g_td_mst.book_line=g_mdm_ac_rel.prt_line;

		/**------------ 更新定期主文件 -------------**/
		g_reply_int=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d ",g_td_mst.ac_id,g_td_mst.ac_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取定期主文件错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
		MEMSET_DEBUG(&tmp_td_mst,0x00,sizeof(struct td_mst_c));
		g_reply_int=Td_mst_Fet_Upd(&tmp_td_mst,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
		g_reply_int=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
		Td_mst_Clo_Upd();
		}
		Td_mst_Clo_Sel();

		g_reply_int=Mdm_unprt_hst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld order by \
			tx_date, trace_no, trace_cnt",g_pub_tx.ac_id);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
	}
	else
	{
	g_reply_int=Mdm_unprt_hst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld order by \
			tx_date, trace_no, trace_cnt",g_pub_tx.ac_id);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P160");
		return -1;
	}
	vtcp_log("本次打印行数[%d]!",unprt_cnt);
	
   	vtcp_log("本次打印行数11123[%4s]",mdm_attr_c.mdm_code);
	for(;;unprt_cnt--)
	{
		if( unprt_cnt<=0 )	break;
		g_reply_int=Mdm_unprt_hst_Fet_Sel(&mdm_unprt_hst_c,g_pub_tx.reply);
		if( g_reply_int==100 )	break;
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
		
		vtcp_log("CHECK=========================!!!");
	vtcp_log("[%s][%d] 主表中记录数=[%d]! 介质属性关系表中=[%d] 未打折明细表中=[%d]\n",__FILE__,__LINE__,tmp_td_mst.book_line,g_mdm_ac_rel.prt_line,mdm_unprt_hst_c.hst_cnt);
	/***根据定期主表中的book_line打存折***/
	switch(mdm_unprt_hst_c.hst_cnt)  /***hst_cnt的值是从td_mst.book_line得来的***/
	{
				    case 80:
					set_zd_data("0130","1");
                    strcpy( g_pub_tx.reply, "P509");
					return -1;
					break;
					case 10:
					case 30:
					case 50:
					case 70:
					set_zd_data("0130","1");
					break;
					default:
					set_zd_data("0130","0");
					break;
    }
		/**
		mdm_ac_rel,prt_line++;
		**/
		prt_cnt++;
		prtl=(g_mdm_ac_rel.prt_line+prt_cnt)%totalnum;
		pub_base_strpack(mdm_unprt_hst_c.brf);

		vtcp_log("CHECK=========================!!!");
		g_reply_int=sql_execute("DELETE FROM mdm_unprt_hst WHERE ac_id=%ld AND tx_date=%ld AND trace_no=%ld AND trace_cnt=%d",mdm_unprt_hst_c.ac_id,mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.trace_no,mdm_unprt_hst_c.trace_cnt);
		if( g_reply_int )
        {
			sprintf(acErrMsg,"删除未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P161");
			Mdm_unprt_hst_Clo_Sel();
			return -1;
        }

		if( mdm_unprt_hst_c.tx_date==11111111 )
		{
			mdm_unprt_hst_c.tx_date = g_pub_tx.tx_date;
		}

        if( g_td_parm.term_type[0]=='M' || g_td_parm.term_type[0]=='D' )
			l_term=g_td_parm.term;
		 else if( g_td_parm.term_type[0]=='Y' )
 			l_term=g_td_parm.term*12;
 		vtcp_log("---------------------------l_term===[%d]",l_term);
 		sprintf(tmp_term,"%02d",l_term);
		vtcp_log("---------------------------tmp_term===[%s]",tmp_term);
		strcpy(term_type,"月");/**存期单位都用"月"**/
		vtcp_log("---------------------------term_type===[%s]",term_type);


		fprintf(fp,"YBTZ%d|%ld|%s|%s|%s|%ld|%.2lf|%.2lf|%.5lf|%s|%d|%s|tty%s|%d|%s|\n",mdm_unprt_hst_c.ac_seqn,mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.brf,tmp_term,term_type,g_td_mst.mtr_date,mdm_unprt_hst_c.tx_amt,mdm_unprt_hst_c.bal,g_td_mst.rate,mdm_unprt_hst_c.tel,prtl,g_td_mst.opn_br_no,g_pub_tx.tty,mdm_unprt_hst_c.hst_cnt,"2104");

		g_mdm_ac_rel.book_bal+=g_td_mst.bal;

        g_td_mst.book_line=g_mdm_ac_rel.prt_line;


	}
	
	Mdm_unprt_hst_Clo_Sel();

	pub_base_strpack( g_mdm_ac_rel.ac_no );
	g_reply_int=Mdm_ac_rel_Dec_Upd( g_pub_tx.reply,"ac_no='%s'", \
									g_mdm_ac_rel.ac_no );
	if(g_reply_int)
	{
		sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}
	}

	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int&&g_reply_int!=100)
	{
		sprintf(acErrMsg,"取记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	f_mdm_ac_rel.book_bal=g_mdm_ac_rel.book_bal;
	f_mdm_ac_rel.prt_line=g_mdm_ac_rel.prt_line;
	if( f_mdm_ac_rel.prt_line<0 )
	{
		sprintf(acErrMsg,"取记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"修改记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	Mdm_ac_rel_Clo_Upd( );

	return (prt_cnt);
}
int pub_base_prt_book(struct mdm_attr_c mdm_attr_c,int unprt_cnt)
{
	int	totalnum;
	int	prtl;
	int prt_cnt;
	int prt_cnt1;
	struct	mdm_unprt_hst_c	mdm_unprt_hst_c;
	struct	mdm_ac_rel_c	f_mdm_ac_rel;

	MEMSET_DEBUG(&mdm_unprt_hst_c,0x00,sizeof(struct mdm_unprt_hst_c));
	MEMSET_DEBUG(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	prtl=0;
	prt_cnt=0;
	totalnum=0;
	totalnum=mdm_attr_c.pg_num*mdm_attr_c.pg_line;

	vtcp_log("存折最大行数[%d]!",totalnum);

	/*换证交易有效
	if( !strcmp(g_pub_tx.tx_code,CHGBOOK_CODE) 
						&& (g_mdm_ac_rel.prt_line%totalnum)!=0)
	{
		g_mdm_ac_rel.prt_line=g_mdm_ac_rel.prt_line+totalnum-(g_mdm_ac_rel.prt_line%totalnum);
	}*/
	if( !strcmp(g_pub_tx.tx_code,CHGBOOK_CODE) )
	{
		g_mdm_ac_rel.prt_line=0;
	}
	
	/*其他情况*/
	if( (g_mdm_ac_rel.prt_line%totalnum+unprt_cnt)<totalnum )
	{
		unprt_cnt=unprt_cnt;
	}
	else
	{
		unprt_cnt=totalnum-g_mdm_ac_rel.prt_line%totalnum;
	}

	g_reply_int=Mdm_unprt_hst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld order by \
			tx_date, trace_no, trace_cnt",g_pub_tx.ac_id);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P160");
		return -1;
	}
	vtcp_log("本次打印行数[%d]!",unprt_cnt);
	
	/*********一户通 add by ligl 2006-12-7 16:59**********/
   
	if(!memcmp(mdm_attr_c.mdm_code,"0011",4))
    {
    	vtcp_log("本次打印行数11123[%4s]",mdm_attr_c.mdm_code);
	switch(g_mdm_ac_rel.prt_line)
	{
				    case 80:
					set_zd_data("0130","1");
                    strcpy( g_pub_tx.reply, "P509");
					return -1;
					break;
					case 30:
					case 50:
					case 70:
					case 10:
					set_zd_data("0130","1");
					break;
					default:
					set_zd_data("0130","0");
					break;
    	}
    }
 /********end*************/
    else
    {
    	vtcp_log("本次打印行数1[%d]!",unprt_cnt);
		/****晋中商行存折为100笔，且首页占两页****/
		switch(g_mdm_ac_rel.prt_line)
    	{
				case 100:
				set_zd_data("0130","1");
                strcpy( g_pub_tx.reply, "P509");
                return -1;
                break;
				case 20:
				case 40:
				case 60:
				case 80:
				set_zd_data("0130","1");
				break;
				default:
				set_zd_data("0130","0");
				break;
    	}
   }
	for(;;unprt_cnt--)
	{
		if( unprt_cnt<=0 )	break;
		g_reply_int=Mdm_unprt_hst_Fet_Sel(&mdm_unprt_hst_c,g_pub_tx.reply);
		if( g_reply_int==100 )	break;
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}
		
		vtcp_log("CHECK=========================!!!");

		g_mdm_ac_rel.prt_line++;
		prt_cnt++;
		prtl=(g_mdm_ac_rel.prt_line+prt_cnt)%totalnum;
		pub_base_strpack(mdm_unprt_hst_c.brf);

		vtcp_log("CHECK=========================!!!");
		g_reply_int=sql_execute("DELETE FROM mdm_unprt_hst WHERE ac_id=%ld AND tx_date=%ld AND trace_no=%ld AND trace_cnt=%d",mdm_unprt_hst_c.ac_id,mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.trace_no,mdm_unprt_hst_c.trace_cnt);
		if( g_reply_int )
        {
			sprintf(acErrMsg,"删除未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P161");
			Mdm_unprt_hst_Clo_Sel();
			return -1;
        }

		if( mdm_unprt_hst_c.tx_date==11111111 )
		{
			mdm_unprt_hst_c.tx_date = g_pub_tx.tx_date;
		}

		fprintf(fp,"CXCZ%d|%s|%.2lf|%s|%.2lf|%s|%d|%s|tty%s|%d|%d|%s|\n",mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.brf,mdm_unprt_hst_c.tx_amt,mdm_unprt_hst_c.add_ind,mdm_unprt_hst_c.bal,mdm_unprt_hst_c.tel,prtl,mdm_unprt_hst_c.tx_br_no,g_pub_tx.tty,g_mdm_ac_rel.prt_line,prt_cnt,mdm_unprt_hst_c.tx_code);
        g_mdm_ac_rel.book_bal=mdm_unprt_hst_c.bal;

	}
	
	Mdm_unprt_hst_Clo_Sel();

	/**xxx*802****
	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	*************/
	pub_base_strpack( g_mdm_ac_rel.ac_no );
	g_reply_int=Mdm_ac_rel_Dec_Upd( g_pub_tx.reply,"ac_no='%s'", \
									g_mdm_ac_rel.ac_no );
	if(g_reply_int)
	{
		sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int&&g_reply_int!=100)
	{
		sprintf(acErrMsg,"取记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	f_mdm_ac_rel.book_bal=g_mdm_ac_rel.book_bal;
	f_mdm_ac_rel.prt_line=g_mdm_ac_rel.prt_line;

	g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"修改记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	Mdm_ac_rel_Clo_Upd( );

	return (prt_cnt);
}

int pub_base_prt_reck( struct mdm_attr_c mdm_attr_c )
{
	int	prtl;
	int prt_cnt;
	int page_cnt;
	int total_cnt;
	double depo_amt=0.00,take_amt=0.00,tmp_bal=0.00;
	struct	mdm_unprt_hst_c	mdm_unprt_hst_c;
	struct	mdm_attr_c		Amdm_attr;
	struct	mdm_ac_rel_c	f_mdm_ac_rel;
	int totalnum;
	int czdy=0;

	MEMSET_DEBUG(&mdm_unprt_hst_c,0x00,sizeof(struct mdm_unprt_hst_c));
	MEMSET_DEBUG(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	MEMSET_DEBUG(&Amdm_attr,0x00,sizeof(struct mdm_attr_c));
	prt_cnt=0;
	page_cnt=0;

	pub_base_strpack(g_mdm_ac_rel.note_no);
	pub_base_strpack(g_dd_mst.name);

	totalnum=mdm_attr_c.pg_num*mdm_attr_c.pg_line;
	/*换证交易有效*/
	if( !strcmp(g_pub_tx.tx_code,CHGBOOK_CODE) 
						&& (g_mdm_ac_rel.prt_line%totalnum)!=0)
	{
		g_mdm_ac_rel.prt_line=g_mdm_ac_rel.prt_line+totalnum-(g_mdm_ac_rel.prt_line%totalnum);
	}
	/**czdy=g_mdm_ac_rel.prt_line%totalnum;**/
	
	/*** MODIFIED BY Azz***/
	pub_base_strpack(g_mdm_ac_rel.ac_no);
	total_cnt = sql_count("mdm_unprt_hst","ac_id=%ld",g_pub_tx.ac_id);
	if( total_cnt<0 )
	{
		sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P160");
		return -1;
	}

	g_reply_int=Mdm_unprt_hst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld order by \
							tx_date, trace_no, trace_cnt ",g_pub_tx.ac_id);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P160");
		return -1;
	}

	while(1)
	{
		g_reply_int=Mdm_unprt_hst_Fet_Sel(&mdm_unprt_hst_c,g_pub_tx.reply);
		if( g_reply_int==100 )	break;
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"取未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P160");
			return -1;
		}

		if( prt_cnt )
		{
			strcpy(g_mdm_ac_rel.note_no,"----------------");
			strcpy(Amdm_attr.mdm_name,"----------------");
		}
		else
		{
			g_reply_int=Mdm_attr_Sel(g_pub_tx.reply,&Amdm_attr,"mdm_code='%s'",
									 g_mdm_ac_rel.mdm_code);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"取介质参数错误![%d]",g_reply_int);
				WRITEMSG
				strcpy( g_pub_tx.reply, "D103");
				return -1;
			}
		}

		prt_cnt++;
		czdy++;
		pub_base_strpack(mdm_unprt_hst_c.brf);
		page_cnt = prt_cnt/RECK_LENG+1;

		vtcp_log("mdm_unprt_hst_c.add_ind===================[%s]", \
				  mdm_unprt_hst_c.add_ind);
		/**处理挂失换证记录**/
		if( mdm_unprt_hst_c.tx_date==11111111 )
		{
			g_mdm_ac_rel.prt_line++;
			czdy=1;
			/* czdy++; */
			fprintf(fp,"CZDZ%ld|%s|%lf|%s|%lf|%lf|%d|\n",
					g_pub_tx.tx_date,mdm_unprt_hst_c.brf,
					mdm_unprt_hst_c.tx_amt,mdm_unprt_hst_c.add_ind,
					mdm_unprt_hst_c.bal,mdm_unprt_hst_c.tx_amt,
					czdy);
		}
		else 
		{
			fprintf(fp,"DZD%d|%s|%.2lf|%s|%.2lf|%s|%d|%s|tty%s|%d|%d|%s|\n",mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.brf,mdm_unprt_hst_c.tx_amt,mdm_unprt_hst_c.add_ind,mdm_unprt_hst_c.bal,mdm_unprt_hst_c.tel,prtl,mdm_unprt_hst_c.tx_br_no,g_pub_tx.tty,czdy,prt_cnt,mdm_unprt_hst_c.tx_code);
        g_mdm_ac_rel.book_bal=mdm_unprt_hst_c.bal;
}
/*****
if( mdm_unprt_hst_c.add_ind[0]=='0' && total_cnt!=prt_cnt )
		{
			fprintf(fp,"DZD%d|%s|%.2lf|%.2lf|%.2lf|%s|%d|%s|tty%s|%s|%s|%s|%s|%s|%d|\n",
			mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.brf,
			mdm_unprt_hst_c.tx_amt,0.00,mdm_unprt_hst_c.bal,
			mdm_unprt_hst_c.tel,prt_cnt,mdm_unprt_hst_c.tx_br_no,
			g_pub_tx.tty,mdm_unprt_hst_c.add_ind,g_mdm_ac_rel.ac_no,
			g_dd_mst.name,Amdm_attr.mdm_name,g_mdm_ac_rel.note_no,page_cnt);

			depo_amt+=mdm_unprt_hst_c.tx_amt;
			tmp_bal = mdm_unprt_hst_c.bal;
		}
		else if( mdm_unprt_hst_c.add_ind[0]=='1' && total_cnt!=prt_cnt )
		{
			fprintf(fp,"DZD%ld|%s|%.2lf|%.2lf|%.2lf|%s|%d|%s|tty%s|%s|%s|%s|%s|%s|%d|\n",
			mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.brf,0.00,
			mdm_unprt_hst_c.tx_amt,mdm_unprt_hst_c.bal,
			mdm_unprt_hst_c.tel,prt_cnt,mdm_unprt_hst_c.tx_br_no,
			g_pub_tx.tty,mdm_unprt_hst_c.add_ind,g_mdm_ac_rel.ac_no,
			g_dd_mst.name,Amdm_attr.mdm_name,g_mdm_ac_rel.note_no,page_cnt);

			take_amt+=mdm_unprt_hst_c.tx_amt;
			tmp_bal = mdm_unprt_hst_c.bal;
		}
		else if( total_cnt==prt_cnt )	
		{
			g_mdm_ac_rel.prt_line++;
			czdy++;

			fprintf(fp,"CZDZ%ld|%s|%.2lf|%s|%lf|%lf|%d|\n",
					mdm_unprt_hst_c.tx_date,"汇总",depo_amt,"",
					tmp_bal,take_amt,czdy);

			g_mdm_ac_rel.prt_line++;
			czdy++;

			fprintf(fp,"CZDZ%ld|%s|%.2lf|%s|%lf|%s|%d|%s|tty%s|%d|%d|%s|\n", \
					mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.brf, \
					mdm_unprt_hst_c.tx_amt,mdm_unprt_hst_c.add_ind, \
					mdm_unprt_hst_c.bal,mdm_unprt_hst_c.tel, \
					0,mdm_unprt_hst_c.tx_br_no,g_pub_tx.tty, \
					czdy,prt_cnt,mdm_unprt_hst_c.tx_code);
		}
***/
		g_reply_int=sql_execute("DELETE FROM mdm_unprt_hst WHERE ac_id=%ld AND tx_date=%d AND trace_no=%d AND trace_cnt=%d",
			mdm_unprt_hst_c.ac_id,mdm_unprt_hst_c.tx_date,
			mdm_unprt_hst_c.trace_no,mdm_unprt_hst_c.trace_cnt);
		if(g_reply_int)
        {
			sprintf(acErrMsg,"删除未打印明细帐错误![%d]",g_reply_int);
			WRITEMSG
			strcpy( g_pub_tx.reply, "P161");
			Mdm_unprt_hst_Clo_Sel();
			return -1;
        }
	}

	Mdm_unprt_hst_Clo_Sel();

	vtcp_log("---look---ac_no=[%s]",g_mdm_ac_rel.ac_no );
	g_reply_int=Mdm_ac_rel_Dec_Upd( g_pub_tx.reply,"ac_no='%s'", \
									g_mdm_ac_rel.ac_no );
	if(g_reply_int)
	{
		sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int&&g_reply_int!=100)
	{
		sprintf(acErrMsg,"取记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}
	f_mdm_ac_rel.prt_line=g_mdm_ac_rel.prt_line;

	g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"修改记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	Mdm_ac_rel_Clo_Upd( );
	set_zd_data("0130","2");
	/*** COMPLETE ***/

	return (prt_cnt);
}
int pub_base_unprt_brf( char *brf )
{
	struct mdm_attr_c mdm_attr;
	int is_crd=0;
	int ret=0;

	strcpy( brf,"" );

    vtcp_log("[%s][%d],BEFORE[%s]",__FILE__,__LINE__,g_pub_tx.brf);
	pub_base_strpack(g_pub_tx.brf);
	
	/**遂心存结息打折内容 add zqbo 20141213**/
	if( strcmp(g_pub_tx.tx_code,"Z156")==0 || strcmp(g_pub_tx.tx_code,"Z157")==0 || 
            strcmp(g_pub_tx.tx_code,"2316")==0
          )
	{
	    strcpy(brf,"结息");
	    goto OkExit;
	}
	
	if( strcmp(g_pub_tx.brf,"存款") &&
		strcmp(g_pub_tx.brf,"取款") &&
		strcmp(g_pub_tx.brf,"开户") &&
		strcmp(g_pub_tx.brf,"销户") &&
		strcmp(g_pub_tx.brf,"部提") &&
		strlen(g_pub_tx.brf) 
		)
	{
		goto OkExit;
	}

	ret=Mdm_attr_Sel( g_pub_tx.reply,&mdm_attr,"mdm_code='%s'",
		g_mdm_ac_rel.mdm_code );
	if( ret ) goto ErrExit;

	if( mdm_attr.mdm_type[0]=='3' || mdm_attr.mdm_type[0]=='4' ||
		mdm_attr.mdm_type[0]=='5' ) /*卡*/
		is_crd=1;
	else
		is_crd=0;
	
vtcp_log("MSTTYPE[%s]%d-ADD%s-CT%s-SVC%d",
	g_pub_tx.ac_id_type,g_pub_tx.cif_no, g_pub_tx.add_ind,g_pub_tx.ct_ind, g_pub_tx.svc_ind);

	switch( g_pub_tx.ac_id_type[0] )
	{
		case '1':	/*活期*/
			if( g_pub_tx.cif_no<50000000 ) /**个人**/
			{
				if( g_pub_tx.add_ind[0]=='0' ) /*取*/
				{
					if( g_pub_tx.ct_ind[0]=='1' ) /*现*/
					{	
						if( g_pub_tx.svc_ind==1003 )
							strcpy(g_pub_tx.brf,"销户");
						else
						{
							if( is_crd ) /*卡*/
							{
								strcpy(brf,"卡取");
							}
							else
							{
								strcpy(brf,"取款");
							}
						}
					}
					else  /*转*/
					{	
						if( g_pub_tx.svc_ind==1003 )
							strcpy(g_pub_tx.brf,"转销");
						else
						{
							if( is_crd ) /*卡*/
							{
								strcpy(brf,"卡转取");
							}
							else
							{
								strcpy(brf,"转取");
							}
						}
					}
				}
				else /*存*/
				{
					if( g_pub_tx.ct_ind[0]=='1' ) /*现*/
					{	
						if( is_crd ) /*卡*/
						{
							strcpy(brf,"卡存");
						}
						else
						{
							strcpy(brf,"存款");
						}
					}
					else  /*转*/
					{	
						if( is_crd ) /*卡*/
						{
							strcpy(brf,"卡转存");
						}
						else
						{
							strcpy(brf,"转存");
						}
					}
				}
			}
			break;
		case '2':
			if( g_pub_tx.cif_no<50000000 ) /**个人**/
			{
				if( g_pub_tx.add_ind[0]=='0' ) /*取*/
				{
					if( g_pub_tx.ct_ind[0]=='1' ) /*现*/
					{	
						if( g_pub_tx.svc_ind==1003 || g_pub_tx.svc_ind==1004 )
							strcpy(g_pub_tx.brf,"销户");
						else
						{
							if( g_pub_tx.add_ind[0]=='0' && 
								g_td_parm.td_type[0] == '0' ) /*定期支取--部提*/
							{
								strcpy(brf,"部提");
							}
							if( g_pub_tx.svc_ind==1001 && 
								g_td_parm.td_type[0] == '2' ) /*整零支取--*/
							{
								strcpy(brf,"取款");
							}
							if( g_pub_tx.svc_ind==1001 && 
								g_td_parm.td_type[0] == '3' ) /*整零支取--*/
							{
								strcpy(brf,"取息");
							}
							if( g_pub_tx.add_ind[0]=='0' && 
								g_td_parm.td_type[0] == '4' ) /*通知支取--部提*/
							{
								strcpy(brf,"部提");
							}
							if( g_pub_tx.svc_ind==1001 && 
								g_td_parm.td_type[0] == '7' ) /*教育支取--*/
							{
								strcpy(brf,"取款");
							}
						}
					}
					else  /*转*/
					{	
						if( g_pub_tx.svc_ind==1003 || g_pub_tx.svc_ind==1004 )
							strcpy(g_pub_tx.brf,"转销");
						else
						{
							if( g_pub_tx.add_ind[0]=='0' && 
								g_td_parm.td_type[0] == '0' ) /*定期支取--部提*/
							{
								strcpy(brf,"部提转");
							}
							if( g_pub_tx.svc_ind==1001 && 
								g_td_parm.td_type[0] == '2' ) /*整零支取--*/
							{
								strcpy(brf,"转取");
							}
							if( g_pub_tx.svc_ind==1001 && 
								g_td_parm.td_type[0] == '3' ) /*整零支取--*/
							{
								strcpy(brf,"取息");
							}
							if( g_pub_tx.add_ind[0]=='0' && 
								g_td_parm.td_type[0] == '4' ) /*通知支取--部提*/
							{
								strcpy(brf,"部提转");
							}
							if( g_pub_tx.svc_ind==1001 && 
								g_td_parm.td_type[0] == '7' ) /*教育支取--*/
							{
								strcpy(brf,"转取");
							}
						}
					}
				}
				else /*存*/
				{
					if( g_pub_tx.ct_ind[0]=='1' ) /*现*/
					{	
						if( g_pub_tx.svc_ind==1000 )
						{
							strcpy(brf,"开户");
						}
						else
						{
							strcpy(brf,"存款");
						}
					}
					else  /*转*/
					{	
						if( g_pub_tx.svc_ind==1000 )
						{
							strcpy(brf,"开户");
						}
						else
						{
							strcpy(brf,"转存");
						}
					}
				}
			}
			break;
		case '3':	
		case '9':	
			sprintf(acErrMsg,"账号类型[%s]不打折",g_pub_tx.ac_type);
			WRITEMSG
			break;
		default:	
			sprintf(acErrMsg,"无此账号类型![%s]",g_pub_tx.ac_type);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P104");
			goto ErrExit;
	}	

OkExit:
	if( !strlen(brf) ) strncpy( brf,g_pub_tx.brf,20 );
	/*2010-2-26 zxp*/
	brf[20]='\0';
	return 0;
ErrExit:
	strncpy( brf,g_pub_tx.brf,20 );
	brf[20]='\0';
	return 0;
}

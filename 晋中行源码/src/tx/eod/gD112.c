/***************************************************************
* 文 件 名: gD112.c
* 功能描述: pos消费错误检查比较
*
* 作    者: rob
* 完成日期: 2003年7月02日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "pos_fee_deal_c.h"
#include "march_mst_c.h"
#include "march_mst_mx_c.h"
#include "pos_fee_mst_c.h"
#include "march_info_c.h"
#include "com_branch_c.h"
#include "chnl_self_trade_c.h"

int page = 0;
struct march_mst_c g_sMarch_mst;
struct march_mst_mx_c g_sMarch_mst_mx;
struct chnl_self_trade_c sChnl_self_trade;
struct pos_fee_mst_c sPos_fee_mst;
struct pos_fee_deal_c sPos_fee_deal;

struct chnl_self_trade_c sChnl_self_trade_cz[128];
struct chnl_self_trade_c sChnl_self_trade_mx[10000];
struct chnl_self_trade_c sChnl_self_trade_yl[10000];

	int cz_num, mx_num, num_yl;

int get_ratio_posdzwb(int bli,int bll,char str[100]);
gD112()
{
    int ret;
	int i=0, j=0;
	char *p;
	char tmp_str[256];
	char mch_name[40];
	char mch_usr[40];
	char mch_pass[40];
	char buf[256];
	char lsbuf[51];

	FILE *fr, *fp;

	char vpfm[21];
	char fname[100];
	char filename[100];

	double total_sh, total_yl;
	int line, opncnt, n;

	char   hdbz[10000] ;

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Pos_fee_deal_Dec_Sel(g_pub_tx.reply, "fname='posmx' and \
		deal_flag = 'Y' and \ 
		pos_date <= %ld order by pos_date", g_pub_tx.tx_date); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE ERROR !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit;
	}

	while(1)
	{
		ret = Pos_fee_deal_Fet_Sel(&sPos_fee_deal, g_pub_tx.reply); 
		if ( ret == 100 ) 
		{ 
			break; 
		}
		else if ( ret != 0 )
		{ 
			sprintf(acErrMsg, "FETCH ERROR !! [%d]", ret); 
			WRITEMSG 
			goto ErrExit;
		}
		


	if ( (p = (char *)getenv("CSADDR")) == NULL ) 
	{ 
		sprintf(acErrMsg, "ftp 地址环境变量中没有定义"); 
		WRITEMSG 
		goto ErrExit;
	} 
	
	memset(tmp_str, 0x0, sizeof(tmp_str)); 
	memset(mch_name, 0x0, sizeof(mch_name)); 
	strcpy(tmp_str, p); 
	strcpy(mch_name, &tmp_str[2]); 
	i = 0; 
	
	while(mch_name[i]!=':') i++; 
	mch_name[i] = 0; 
	sprintf(acErrMsg, "ftp 地址: [%s] [%s]", mch_name, tmp_str); 
	WRITEMSG

	if ((p = (char *)getenv("CSUSR")) == NULL) 
	{ 
		sprintf(acErrMsg, "ftp 用户名和密码环境变量中没有定义!!"); 
		WRITEMSG 
		goto ErrExit;
	} 
	
	i = 0; 
	memset(tmp_str, 0x0, sizeof(tmp_str)); 
	memset(mch_usr, 0x0, sizeof(mch_usr)); 
	memset(mch_pass, 0x0, sizeof(mch_pass)); 
	strcpy(tmp_str, p); 
	strcpy(mch_usr, &tmp_str[2]); 
	while(mch_usr[i]!=':') i++; 
	mch_usr[i] = 0; 
	strcpy(mch_pass, &tmp_str[2+i+1]); 
	
	sprintf(acErrMsg, "-----> ftp : 地址: [%s] ,用户: [%s],密码: [%s]", 
			mch_name, mch_usr, mch_pass); 
	WRITEMSG

	chdir("/home/dhccb/tmp");

	fr=fopen("ftp.txt","w"); 
	fprintf(fr,"open %s\n", mch_name); 
	fprintf(fr,"user %s %s\n", mch_usr, mch_pass); 
	fprintf(fr,"cd file \n"); 
	fprintf(fr,"get posmx%ld \n", sPos_fee_deal.pos_date); 
	fprintf(fr,"get sxf%ld \n"  , sPos_fee_deal.pos_date); 
	fprintf(fr,"quit \n"); 
	fclose(fr); 
	memset(buf, 0x00, sizeof(buf) ); 
	strcpy(buf,"ftp -vin<ftp.txt > errmsg"); 
	system(buf);

	sprintf( filename,"%s%ld","pos_dzwb_",sPos_fee_deal.pos_date); 
	
	sprintf( fname,"rm -f %s/%s/%s%ld.txt",getenv("DATADIR"), 
		"61000","pos_dzwb_",sPos_fee_deal.pos_date); 

	sprintf(acErrMsg, "------filename = [%s]", fname);
	WRITEMSG
	ret=system( fname ); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"rm file error %s %d",fname,ret); 
		WRITEMSG 
		goto ErrExit;
	} 
	strcpy(vpfm, "posmx");

	ret = Chnl_self_trade_Dec_Sel(g_pub_tx.reply, "tx_date=%ld and \
			tx_code in ('6301','6302','6304') and stat in ('0','1')", 
			sPos_fee_deal.pos_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "DELCARE FOR SELECT !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	} 

	pub_rpt_openfile( &fp,"61000", filename); 
	
	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
		get_ratio_posdzwb) ) 
	goto ErrExit; 
	
	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
		get_ratio_posdzwb) )

	cz_num = 0;
	mx_num = 0;
	total_sh = 0.00;

	while(1)
	{
		ret = Chnl_self_trade_Fet_Sel(&sChnl_self_trade, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH FOR SELECT ERROR [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}

		if (strncmp(sChnl_self_trade.tx_code, "6304", 4) == 0)
		{
			/* 冲正流水 */
			memset(&sChnl_self_trade_cz[cz_num], 0x0, 
					sizeof(struct chnl_self_trade_c));
			memcpy(&sChnl_self_trade_cz[cz_num], &sChnl_self_trade,
					sizeof(struct chnl_self_trade_c));
			cz_num++;
			continue;
		}

		if (strncmp(sChnl_self_trade.tx_code, "6302", 4) == 0)
		{
			sChnl_self_trade.tx_amt = - sChnl_self_trade.tx_amt;
		}

		memset(&sChnl_self_trade_mx[mx_num], 0x0, 
					sizeof(struct chnl_self_trade_c));
		memcpy(&sChnl_self_trade_mx[mx_num], &sChnl_self_trade,
					sizeof(struct chnl_self_trade_c));
		mx_num++;
		total_sh += sChnl_self_trade.tx_amt;

		g_pub_tx.ac_seqn = mx_num;
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
			get_ratio_posdzwb) )
			goto ErrExit;

	}
	Chnl_self_trade_Clo_Sel();

	g_pub_tx.tx_amt1 = total_sh;
	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
		get_ratio_posdzwb) )
		goto ErrExit;

sprintf(acErrMsg, "----------------------------------------------->");
WRITEMSG

	/* 读入POSMX */ 
	chdir("/home/dhccb/tmp");
	sprintf(filename,"posmx%ld", sPos_fee_deal.pos_date ); 
	fr=fopen(filename,"r"); 
	if(fr==NULL)
	{ 
		sprintf(acErrMsg, "Can't open file %s\n", filename); 
		WRITEMSG
		goto OkExit;
	}

	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
			get_ratio_posdzwb) )
		goto ErrExit;

	num_yl=0;
	total_yl = 0.00;
	while(fgets(buf, 160, fr) != NULL)
	{
		if(buf[0]=='/') continue;
		n=strlen(buf);

		i=0; 
		memset(&sChnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));

		/* 1: */ 
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		
		/* 2: */ 
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		sChnl_self_trade.tx_time = atol(lsbuf); 
		
		/*3: */ 
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		
		/*4:卡号 */ 
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		strcpy(sChnl_self_trade.ac_no, lsbuf); 
		
		if ( memcmp(sChnl_self_trade.ac_no,"4302",4) != 0 && \
				memcmp(sChnl_self_trade.ac_no,"940071733",9) != 0) 
			continue; 
		
		/* 5:金额 */ 
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		sChnl_self_trade.tx_amt = atof(lsbuf)/100; 
		
		/* 6 */ 
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		
		/* 7 */ 
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		
		/* 8 */ 
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		
		/* 9: 终端号 */ 
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		strcpy(sChnl_self_trade.ttyn, lsbuf); 
		
		/* 10:商户号 */
		while(buf[i]==' ' && i< n) i++; 
		j=0; while(buf[i]!=' ' && i< n) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		strcpy(sChnl_self_trade.spcd, lsbuf);

		memset(&sChnl_self_trade_yl[num_yl], 0x0,
				sizeof(struct chnl_self_trade_c));
		memcpy(&sChnl_self_trade_yl[num_yl], &sChnl_self_trade, 
				sizeof(struct chnl_self_trade_c));

		total_yl += sChnl_self_trade.tx_amt;
		num_yl ++;

		g_pub_tx.ac_seqn = num_yl;
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
				get_ratio_posdzwb) )
			goto ErrExit;
	}

	fclose(fr);
	g_pub_tx.tx_amt1 = total_yl;
	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","EE",&opncnt, 
			get_ratio_posdzwb) )
		goto ErrExit;

	g_pub_tx.tx_amt1 = total_sh;
	sPos_fee_mst.tx_amt = total_yl;
	sChnl_self_trade.tx_amt = total_sh - total_yl;
	if (pub_base_CompDblVal(total_sh, total_yl) == 0)
	{
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","DD",&opncnt, 
				get_ratio_posdzwb) )
			goto ErrExit;
	}
	else
	{
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","FF",&opncnt, 
				get_ratio_posdzwb) )
			goto ErrExit;
	}

sprintf(acErrMsg, "----------------------------------------------->");
WRITEMSG

	/* 取手续费 */ 
	chdir("/home/dhccb/tmp");
	memset(buf, 0x00, sizeof(buf) ); 
	sprintf(buf,"sxf%ld",sPos_fee_deal.pos_date); 
	fr=fopen(buf,"r"); 
	if(fr==NULL)
	{ 
		sprintf(acErrMsg, "Can open file %s\n", buf); 
		WRITEMSG
		goto ErrExit;
	} 

	while(fgets(buf, 160, fr) != NULL)
	{ 
		if(memcmp(buf,"ZZS",3)!=0) continue; 
		n=strlen(buf); 
		i=0;

		for(j=0; j<4; ++j){ while(i<n&&buf[i]!='|')  i++; i++; } 
		j=0;  while(buf[i]!='|' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		sPos_fee_mst.dr_amt = atof(lsbuf); 
		j=0; i++; while(buf[i]!='|' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		sPos_fee_mst.cr_amt = atof(lsbuf) ; 
		j=0; i++; while(buf[i]!='|' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		sPos_fee_mst.n_cr_amt = atof(lsbuf) ; 
		
		j=0; i++; while(buf[i]!='|' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		sPos_fee_mst.adjust_dr_amt = atof(lsbuf); 
		j=0; i++; while(buf[i]!='|' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		sPos_fee_mst.adjust_cr_amt = atof(lsbuf); 
		j=0; i++; while(buf[i]!='|' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		sPos_fee_mst.succ_num = atol(lsbuf); 
		j=0; i++; while(buf[i]!='|' && i< n ) lsbuf[j++]=buf[i++]; lsbuf[j]=0; 
		sPos_fee_mst.mater_amt = atof(lsbuf) ;

		g_pub_tx.tx_amt1 = sPos_fee_mst.dr_amt - sPos_fee_mst.cr_amt - \
			sPos_fee_mst.n_cr_amt - sPos_fee_mst.mater_amt; /* 手续费 */

		sChnl_self_trade.tx_amt = sPos_fee_mst.adjust_dr_amt - \
				sPos_fee_mst.adjust_cr_amt;  /* 调整金额 */

		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","JJ",&opncnt, 
			get_ratio_posdzwb) )
			goto ErrExit;
	}

sprintf(acErrMsg, "----------------------------------------------->");
WRITEMSG

	/* 对帐 */
	for(i=0; i<num_yl; ++i) hdbz[i]='0'; 

	for(i=0; i<mx_num; ++i)
	{
		for(j=0; j<num_yl; ++j) 
			if(hdbz[j]=='0')
			{
				sprintf(acErrMsg, "------[%d], [%d]--ac_no=[%s],[%s]", i, j,
					sChnl_self_trade_mx[j].ac_no, sChnl_self_trade_yl[i].ac_no);
				WRITEMSG
				sprintf(acErrMsg, "--------spcd=[%s],[%s]",
					sChnl_self_trade_mx[j].spcd, sChnl_self_trade_yl[i].spcd);
				WRITEMSG
				sprintf(acErrMsg, "-----tx_amt=[%lf],[%lf]",
					sChnl_self_trade_mx[j].tx_amt, 
					sChnl_self_trade_yl[i].tx_amt);
				WRITEMSG

				if (strcmp(sChnl_self_trade_mx[j].ac_no, \
					sChnl_self_trade_yl[i].ac_no) == 0 && \
					strcmp(sChnl_self_trade_mx[j].spcd, \
					sChnl_self_trade_yl[i].spcd) == 0 && \
					pub_base_CompDblVal(sChnl_self_trade_mx[j].tx_amt, \
					sChnl_self_trade_yl[i].tx_amt) == 0)
				{
					hdbz[j] = '1'; 
					break; 
				}
			} 

		if(j>=num_yl)
		{
			memset(&sChnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));
			memcpy(&sChnl_self_trade, &sChnl_self_trade_mx[i], \
					sizeof(struct chnl_self_trade_c));
			g_pub_tx.ac_seqn = i + 1;
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","GG",&opncnt, 
				get_ratio_posdzwb) )
				goto ErrExit;
		}
	} 
	
	for(i=0; i<mx_num; ++i) hdbz[i]='0'; 
	for(i=0; i<num_yl; ++i)
	{
		for(j=0; j<mx_num; ++j) 
			if(hdbz[j]=='0')
			{
				if (strcmp(sChnl_self_trade_mx[j].ac_no,  \
					sChnl_self_trade_yl[i].ac_no) == 0 && \
					strcmp(sChnl_self_trade_mx[j].spcd, \
					sChnl_self_trade_yl[i].spcd) == 0 && \
					pub_base_CompDblVal(sChnl_self_trade_mx[j].tx_amt, \
					sChnl_self_trade_yl[i].tx_amt) == 0)
				{
					hdbz[j] = '1'; 
					break; 
				}
			} 

		if(j>=mx_num)
		{
			memset(&sChnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));
			memcpy(&sChnl_self_trade, &sChnl_self_trade_yl[i], \
					sizeof(struct chnl_self_trade_c));
			g_pub_tx.ac_seqn = i + 1;
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","II",&opncnt, 
				get_ratio_posdzwb) )
				goto ErrExit;
		}
	}

sprintf(acErrMsg, "----------------------------------------------->");
WRITEMSG

	/* POS冲正交易明细 */
	if ( cz_num > 0 ) 
	{ 
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","KK",&opncnt, 
				get_ratio_posdzwb) )
			goto ErrExit;
		for(i=0; i<cz_num; ++i) 
		{
			memset(&sChnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));
			memcpy(&sChnl_self_trade, &sChnl_self_trade_cz[i], \
						sizeof(struct chnl_self_trade_c));
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","MM",&opncnt, 
					get_ratio_posdzwb) )
				goto ErrExit;
		}
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","NN",&opncnt, 
				get_ratio_posdzwb) )
			goto ErrExit;
	}

	} /* end of while */
	Pos_fee_deal_Clo_Sel();

	ret = sql_execute("delete from pos_fee_deal where fname='posmx' \
		and deal_flag='Y'");
	if ( ret )
	{
		sprintf(acErrMsg, "删除pos_fee_deal表错误!![%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

sprintf(acErrMsg, "----------------------------------------------->");
WRITEMSG

OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"pos消费错误检查比较成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"pos消费错误检查比较失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int get_ratio_posdzwb( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char    l_kmm[31]; 
	char fmt[10]; 
	int i=0; 
	int ret = 0; 

	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': 
			sprintf(fmt,"%%%dd",bll); 
			sprintf( vstr,fmt, g_pub_tx.ac_seqn);
			strcpy(str, vstr); 
			break; 
		case 'B': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( vstr,fmt, sChnl_self_trade.ac_no);
			strcpy(str, vstr); 
			break; 
		case 'C': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( vstr,fmt, sChnl_self_trade.tx_time);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, sChnl_self_trade.spcd); 
			break; 
		case 'E': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, sChnl_self_trade.ttyn); 
			break; 
		case 'F':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, sChnl_self_trade.tx_amt); 
			break; 
		case 'G': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( str, fmt, g_pub_tx.tx_amt1); 
			break; 
		case 'H': 
			sprintf(fmt,"%%%dd",bll); 
			sprintf( str, fmt, mx_num); 
			break; 
		case 'I': 
			sprintf(fmt,"%%%dd",bll); 
			sprintf( str, fmt, num_yl); 
			break; 
		case 'J': 
			sprintf(fmt,"%%%d.2lf", bll); 
			sprintf( str, fmt, sPos_fee_mst.tx_amt); 
			break; 
		case 'K': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, sPos_fee_mst.succ_num); 
			break; 
		case 'L': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, cz_num); 
			break; 
		case 'M': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, sPos_fee_deal.pos_date); 
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

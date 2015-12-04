/********************************************
*文 件 名:sp1799.c 
*功能描述:根据类型获得非税所需文件
*作    者:zzz 
*完成日期:2012-10-15
insert into tx_def(tx_code,tx_name,bit_map,log_ind,tx_type,wrk_sts) values('1799','获生成非税文件','00000000000000000001001111111101010011010001101100000100000001000000000001000000000100000000000000000000000000010000000010000000','0','4','0');
insert into tx_flow_def values('1799',0,'1799','#$');
insert into tx_sub_def values('1799','根据类型获取非税文件','sp1799','0','0');
********************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#define AC_ID 900009045 
#define AC_NO  "9223100000099"
#include "public.h"
#include "com_sys_parm_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "cif_basic_inf_c.h"
#include "in_mst_hst_c.h"
#include "in_mst_c.h"
#include "dd_mst_hst_c.h"
#include "hv_zf_c.h"
#include "lv_pkgreg_c.h"
#include "chnl_tx_detl_c.h"
#include "hv_uniontab_c.h"
#include "trace_log_c.h"
#include "trace_log_rz_c.h"
#include "com_branch_c.h"
int get_branch(char *br,char *name);
int get_opn_branch(char *br,char *name);
double FindBalBydate(long ac_id, long *tx_date);
void ChangeState(struct dd_mst_c *one, char *stat, char *desc);
#define FSROW 100
#define MAX_ACC_LEN 36
typedef struct _ST_FSACC
{
	char account[MAX_ACC_LEN];
	char accName[128];
	char accOpenBrno[12];
	char accSts[16];
	long accEndDate;
	char accHold[16];
	double bal;
	long int acid;
}STFSACC;
int getAccList(STFSACC *list,int *count);
int sp1799()
{
	int ret=0;
	int i=0,iCount=0,iNum=0;
	int  ttlnum = 0;   /**总记录数**/
	long   cEnddate = 0;
	long   cDate = 0;
	long nowDate = 0;
	long startDate = 0;
	double dSumBal =0.00;
	int  page_counter=0;         /**总页数**/
	char current_page[3] = {0};
	char sFileName[128]={'\0'};
		char tmpac_no[20];
	char buf[64]={'\0'};
	char sAccBuf[1024]={'\0'};
	char sType[16]={'\0'};
	char sDate[12]={'\0'},sDateBegin[12]={'\0'},sDateEnd[12]={'\0'};
	char ac_sts[30]  = {0};
	char add_ind[5]  = {0};
	char curType[5]  = {0};      /**查询页码**/
	char tmpstr[512] = {0};
	char cCardno[21] = {0};
	char spaceacno[33]={0};
	char tmp[5]={0};
	char bakname[61];
	char cAcno[21]   = {0};
	char filename[100]   = {0};
	char wherestr[1024]  = {0};
	char wherelist[1024] = {0};  /**查询条件**/
	char br_name[30]={0};
	char br_name1[30]={0};
	char dz_acno[20]={'\0'};
	long ac_id=0;
	STFSACC accList[FSROW];
	FILE *fp=NULL;
	struct in_mst_c		in_mst;
	struct dd_mst_c       b_dd_mst;
	struct dd_mst_c       f_dd_mst;
	struct mdm_ac_rel_c st_mdm_ac_rel;
	struct dd_mst_c dd_mst;
	struct dd_mst_hst_c   vdd_mst_hst;
	struct dd_mst_hst_c  dd_mst_hst;
	struct in_mst_hst_c  in_mst_hst;
	struct cif_basic_inf_c st_cif_basic_inf;
	struct  com_sys_parm_c sys_parm;
	struct trace_log_c    f_trace_log;
	struct trace_log_rz_c vtrace_log_rz;
	struct hv_zf_c	hv_zf;
	struct lv_pkgreg_c	lv_pkgreg;
	struct chnl_tx_detl_c chnl_tx_detl;
	
	memset(&chnl_tx_detl,0x00,sizeof(chnl_tx_detl));
	memset(&lv_pkgreg,0x00,sizeof(lv_pkgreg));
	memset(&hv_zf,0x00,sizeof(hv_zf));
	memset(&in_mst,0x00,sizeof(in_mst));
	memset(&f_trace_log,0x00,sizeof(f_trace_log));
	memset(&vtrace_log_rz,0x00,sizeof(vtrace_log_rz));
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
	memset(bakname,0x00,sizeof(bakname));
	memset(accList,0x00,sizeof(accList));
	memset(tmpac_no,0x00,sizeof(tmpac_no));
	memset(&dd_mst,0x00,sizeof(dd_mst));
	memset(&f_dd_mst,0x00,sizeof(f_dd_mst));
	memset(&b_dd_mst,0x00,sizeof(b_dd_mst));
	memset(&vdd_mst_hst,0x00,sizeof(vdd_mst_hst));
	memset(&dd_mst_hst,0x00,sizeof(dd_mst_hst));
	memset(&in_mst_hst,0x00,sizeof(in_mst_hst));
	memset(&st_mdm_ac_rel,0x00,sizeof(st_mdm_ac_rel));
	memset(&st_cif_basic_inf,0x00,sizeof(st_cif_basic_inf));
	memset(&sys_parm,0x00,sizeof(sys_parm));	
		
	pub_base_sysinit();
	pub_base_AllDwnFilName( sFileName);
	fp = fopen( sFileName,"w" );	
	if( fp==NULL )	
	{		
		vtcp_log( "%s,%d,open file [%s] error ",__FILE__,__LINE__,sFileName);		
		strcpy( g_pub_tx.reply,"S047" );		
		goto ErrExit;	
	}
	vtcp_log( "%s,%d,FILENAME [%s]",__FILE__,__LINE__,sFileName);	
	if(Com_sys_parm_Sel(g_pub_tx.reply,&sys_parm,"1=1"))
	{
		set_zd_data( DC_REPLY,"S005");
		set_zd_data("0130","核心获取当前日期出错");
		goto ErrExit;
	}
	nowDate=sys_parm.sys_date;
	get_zd_data("0380",cCardno);
	zip_space(cCardno);
	get_zd_data("0300",current_page);
	zip_space(current_page);
	get_zd_long("0290",&startDate);
	get_zd_long("0450",&cEnddate);
	if(cEnddate==0)
		{
			set_zd_long("0450", sys_parm.sys_date-1);
			set_zd_long("0290", sys_parm.sys_date-1);
		}
	get_zd_long("0450",&cDate);
	get_zd_long("0290",&startDate);
	get_zd_data("0470",sType);/*文件类型*/
	zip_space(sType);
	if(memcmp(cCardno,"5001620300015",13)==0)
		{
	if (getAccList(accList,&iCount))
	{
		vtcp_log("[%s][%d] 获得账号信息出错",__FILE__,__LINE__);
		goto ErrExit;
	}
	vtcp_log( "[%s][%d] 账号个数[%d]",__FILE__,__LINE__,iCount);
	vtcp_log( "[%s][%d] 文件类型[%s]",__FILE__,__LINE__,sType);	
}
	else
		{
			get_zd_data("0380",accList[0].account);
			pub_base_acno_acid(&accList[0].acid, accList[0].account);
			iCount=1;
		}
	if (strcmp(sType,"9995")==0) /*将当日非税柜面缴款记录生成文件,对账 */
	{
		/*if (getAccList(accList,&iCount))
	{
		vtcp_log("[%s][%d] 获得账号信息出错",__FILE__,__LINE__);
		goto ErrExit;
	}*/
		get_zd_data("0450",sDateBegin);/*对账开始日期*/
		get_zd_data("0460",sDateEnd);/*对账结束日期*/
		get_zd_data("0300",dz_acno);
		zip_space(dz_acno);
		vtcp_log( "[%s][%d] 日期[%s]ac_no==[%s]",__FILE__,__LINE__,sDateBegin,dz_acno);	
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'",dz_acno);
			if(ret)
				  {
						vtcp_log( "查询开销户信息表st_mdm_ac_rel失败ret=[%d]",ret);
						goto ErrExit;
				  }
		ret = In_mst_hst_Dec_Sel( g_pub_tx.reply, " ac_id=%ld and tx_date=%ld ",st_mdm_ac_rel.ac_id,atol(sDateBegin));
		/*ret = In_mst_hst_Dec_Sel( g_pub_tx.reply, " ac_id=%ld and tx_date=%ld ",AC_ID,atol(sDateBegin));*/
			ERR_DEAL
			while(1)
			{
				memset(&in_mst_hst,0x00,sizeof(in_mst_hst));
				ret=In_mst_hst_Fet_Sel( &in_mst_hst, g_pub_tx.reply );
				if( ret==100 ) break;		
				ERR_DEAL
				iNum++;
				fprintf(fp,"%s|%ld|%06ld|%.2lf|%c\n",
				AC_NO,
				in_mst_hst.tx_date,
				in_mst_hst.trace_no,
				in_mst_hst.tx_amt,
				'\0');
			}	
			 ret=In_mst_hst_Clo_Sel();
			 ERR_DEAL

		if (iNum ==0){
			fprintf(fp,"%s\n","notanyrecord");
		}
		vtcp_log( "[%s][%d] 记录数[%d]",__FILE__,__LINE__,iNum);
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1" );
		vtcp_log( "here1 ok");
	}
else if(strncmp(sType,"8102",4)==0||strncmp(sType,"8101",4)==0)/*查账户状态and查账户余额*/
	{
				for(i=0 ; i<iCount;i++)
		{	
			ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst," ac_id = %ld", accList[i].acid);  
			if(ret)
			{
				vtcp_log( "查询活期存款主文件失败");
				goto ErrExit;  
			}
			if(memcmp(dd_mst.opn_br_no,"10201",4)==0)
				{
					ChangeState(&dd_mst,"ac_sts",ac_sts);/*帐户状态*/
					set_zd_data("0380", cCardno);
	        set_zd_data("0250", dd_mst.name);
	        set_zd_data("0370", "博大支行");
	        set_zd_data("0320", ac_sts);
	         if(cEnddate)
	        set_zd_long("0440", cEnddate);	
	        else
	        set_zd_long("0440", cDate);
	      }
	     if(cEnddate)
			    accList[i].bal = FindBalBydate(accList[i].acid ,&cEnddate);
			     else
			    accList[i].bal=dd_mst.bal-dd_mst.hold_amt-dd_mst.ctl_amt;
		}
		for(i=0 ; i<iCount;i++)
		{
			dSumBal+=accList[i].bal;
		}
		set_zd_double("0400",dSumBal);	
}
else if(strncmp(sType,"8103",4)==0||strncmp(sType,"8104",4)==0)/*查当日明细*/
	{				
		ttlnum = 0;
		for(i=0 ; i<iCount;i++)
		{
			memset(tmpac_no,0x00,sizeof(tmpac_no));
			strcpy(tmpac_no,accList[i].account);
	ret=Dd_mst_hst_Dec_Sel( g_pub_tx.reply,"ac_id=%ld and tx_date>=%ld and tx_date<=%ld and add_ind='1' order by tx_date desc,trace_no desc,trace_cnt desc",accList[i].acid,startDate,cDate);
	ERR_DEAL
	while(1)
	{
		memset( &vdd_mst_hst,0,sizeof(vdd_mst_hst));
		ret=Dd_mst_hst_Fet_Sel( &vdd_mst_hst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
		ttlnum++;
		/**每页显示10条**/
		if(((ttlnum/10+1)==atoi(current_page))&&(ttlnum%10!=0)||((ttlnum%10==0)&&ttlnum/10==atoi(current_page)))
		{
			ret=Dd_mst_Sel(g_pub_tx.reply,&f_dd_mst," ac_id=%ld ", vdd_mst_hst.ac_id);
		  if(ret)
		  {
				vtcp_log( "查询活期存款主文件失败");
				goto ErrExit;
		  }
		  memset( cAcno,0,sizeof(cAcno) );
		  if(vdd_mst_hst.ct_ind[0] == '2')         /**转账类的交易**/
		  {
		  	if(memcmp(vdd_mst_hst.tx_code,"5839",4)==0||memcmp(vdd_mst_hst.tx_code,"5869",4)==0)
		  		{
		  			if(strlen(tmpac_no)==13)
		  				{
		  					memset(spaceacno,0x00,sizeof(spaceacno));
		  					sprintf(spaceacno,"%s                   ",tmpac_no);
		  			ret=Hv_zf_Sel(g_pub_tx.reply,&hv_zf,"tx_date=%ld and tx_amt=%.2f and cash_ac_no in('%s','%s') and (trace_no=%ld or chk_trace_no=%ld)",vdd_mst_hst.tx_date,vdd_mst_hst.tx_amt,tmpac_no,spaceacno,vdd_mst_hst.trace_no,vdd_mst_hst.trace_no);
		  		}
		  		else if(strlen(tmpac_no)==15)
		  					{
		  					memset(spaceacno,0x00,sizeof(spaceacno));
		  					sprintf(spaceacno,"%s                 ",tmpac_no);
		  			ret=Hv_zf_Sel(g_pub_tx.reply,&hv_zf,"tx_date=%ld and tx_amt=%.2f and cash_ac_no in('%s','%s') and (trace_no=%ld or chk_trace_no=%ld)",vdd_mst_hst.tx_date,vdd_mst_hst.tx_amt,tmpac_no,spaceacno,vdd_mst_hst.trace_no,vdd_mst_hst.trace_no);
		  		}
		  			if(ret && ret != 100)
				  {
						vtcp_log( "查询大额支付信息失败");
						goto ErrExit;
				  }
				 ChangeState(&f_dd_mst,"ac_sts",ac_sts);
				memset(br_name,0x00,sizeof(br_name));
		  get_branch(vdd_mst_hst.opn_br_no,br_name);
		   memset(br_name1,0x00,sizeof(br_name1));
		  get_opn_branch(hv_zf.pay_opn_br_no,br_name1);
			fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
			tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"无",\
			vdd_mst_hst.tx_amt,vdd_mst_hst.bal,"贷","RMB",hv_zf.pay_ac_no,hv_zf.pay_name,br_name1); 
		  		}
		  	else	if(memcmp(vdd_mst_hst.tx_code,"5840",4)==0||memcmp(vdd_mst_hst.tx_code,"5880",4)==0)
		  		{
		  			ret=Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg,"in_date=%ld and cash_ac_no='%s' and tx_amt=%.2f",vdd_mst_hst.tx_date,tmpac_no,vdd_mst_hst.tx_amt);
		  			if(ret && ret != 100)
				  {
						vtcp_log( "查询小额支付信息失败");
						goto ErrExit;
				  }
				 ChangeState(&f_dd_mst,"ac_sts",ac_sts);
				memset(br_name,0x00,sizeof(br_name));
		  get_branch(vdd_mst_hst.opn_br_no,br_name);
		   memset(br_name1,0x00,sizeof(br_name1));
		  get_opn_branch(lv_pkgreg.pay_opn_br_no,br_name1);
			fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
			tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"无",\
			vdd_mst_hst.tx_amt,vdd_mst_hst.bal,"贷","RMB",lv_pkgreg.pay_ac_no,lv_pkgreg.pay_name,br_name1); 
		  	
		  		}
			else if(memcmp(vdd_mst_hst.tx_code,"6233",4)==0||memcmp(vdd_mst_hst.tx_code,"6236",4)==0||memcmp(vdd_mst_hst.tx_code,"6235",4)==0||memcmp(vdd_mst_hst.tx_code,"7022",4)==0||memcmp(vdd_mst_hst.tx_code,"7023",4)==0||memcmp(vdd_mst_hst.tx_code,"6247",4)==0)
			{
				ret=Chnl_tx_detl_Sel(g_pub_tx.reply,&chnl_tx_detl,"tx_date=%ld and df_ac_no='%s' and tx_amt=%.2f",vdd_mst_hst.tx_date,tmpac_no,vdd_mst_hst.tx_amt);
				if(ret && ret != 100)
				{
					vtcp_log( "查询支付信息失败");
					goto ErrExit;
				}
				ChangeState(&f_dd_mst,"ac_sts",ac_sts);
				memset(br_name,0x00,sizeof(br_name));
				get_branch(vdd_mst_hst.opn_br_no,br_name);
				memset(br_name1,0x00,sizeof(br_name1));
				get_opn_branch(chnl_tx_detl.df_br_no,br_name1);
				fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
				tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"无",\
				vdd_mst_hst.tx_amt,vdd_mst_hst.bal,"贷","RMB",chnl_tx_detl.ac_no,chnl_tx_detl.ac_name,br_name1); 
			}
		  	else
		  	{
		  	/**如果是当天发生的转账交易，查表trace_log获取转账对方帐号**/
		  	if(vdd_mst_hst.tx_date == nowDate)
		  	{
		  		memset( tmpstr,0,sizeof(tmpstr) );
		  		memset( wherestr,0,sizeof(wherestr) );
		  		memset( &f_trace_log,0,sizeof(f_trace_log));
		  		ret = Trace_log_Sel(g_pub_tx.reply, &f_trace_log, "trace_no=%ld and tx_date=%ld and ct_ind='2' and ac_no is not NULL and ac_no !='%s' order by tx_date desc",\
		  		                    vdd_mst_hst.trace_no, vdd_mst_hst.tx_date,accList[i].account);
		  		if(ret && ret != 100)
				  {
						vtcp_log( "查询流水表trace_log信息失败");
						goto ErrExit;
				  }
				  strcpy(cAcno,f_trace_log.ac_no);
		  		sprintf( tmpstr," ac_no ='%s' order by ac_no desc",f_trace_log.ac_no);
		  		strcat( wherestr,tmpstr );
		  	}
		  	else
		  	{
		  		memset( tmpstr,0,sizeof(tmpstr) );
		  		memset( wherestr,0,sizeof(wherestr) );
		  		memset( &vtrace_log_rz,0,sizeof(vtrace_log_rz));
		  		ret = Trace_log_rz_Sel(g_pub_tx.reply, &vtrace_log_rz, "trace_no=%ld and tx_date=%ld and ct_ind='2' and amt=%f and ac_no is not NULL and ac_no !='%s' order by tx_date desc",\
		  		                    vdd_mst_hst.trace_no, vdd_mst_hst.tx_date,vdd_mst_hst.tx_amt,accList[i].account);
		  		if(ret && ret != 100)
				  {
						vtcp_log( "查询流水表trace_log_rz信息失败");
						goto ErrExit;
				  }
				  strcpy(cAcno,vtrace_log_rz.ac_no);
		  		sprintf( tmpstr," ac_no ='%s' order by ac_no desc",vtrace_log_rz.ac_no);
		  		strcat( wherestr,tmpstr );
		  	}
		  	/**如果len=0，说明日期间隔太长，该笔转账对方的信息已经被清理**/
		  	if(strlen(cAcno) > 1)
		  	{
		  		memset(bakname,0x00,sizeof(bakname));
		  		memset( &st_mdm_ac_rel,0,sizeof(st_mdm_ac_rel));
			  	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'",cAcno);
			  	if(ret && ret != 100)
				  {
						vtcp_log( "查询开销户信息表st_mdm_ac_rel失败ret=[%d]",ret);
						goto ErrExit;
				  }
				  
				  if(cAcno[0]=='9')
				  	{
				  		ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",st_mdm_ac_rel.ac_id);
				  		if(ret && ret != 100)
				  {
						vtcp_log( "查询开销户信息表st_mdm_ac_rel失败ret=[%d]",ret);
						goto ErrExit;
				  }
				  strcpy(bakname,in_mst.name);
				  	}
				  	else
				  		{
				  			ret=Dd_mst_Sel(g_pub_tx.reply,&b_dd_mst," ac_id=%ld ",st_mdm_ac_rel.ac_id);
		  if(ret)
		  {
				vtcp_log( "查询活期存款主文件失败");
				goto ErrExit;
		  }
		   strcpy(bakname,b_dd_mst.name);
				  		} 	
		  	}
		  	ChangeState(&f_dd_mst,"ac_sts",ac_sts);   /**帐户状态**/
		  if(vdd_mst_hst.add_ind[0] == '0')         /**增减标志**/
		  {
		  	strcpy(add_ind,"借");
		  }
		  else
		  {
		  	strcpy(add_ind,"贷");
		  }
		  strcpy(curType,"RMB"); /**货币种类**/
		  memset(br_name,0x00,sizeof(br_name));
		  get_branch(f_dd_mst.opn_br_no,br_name);
		   memset(br_name1,0x00,sizeof(br_name1));
		  get_branch(st_mdm_ac_rel.opn_br_no,br_name1);
			fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
			tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"无",\
			vdd_mst_hst.tx_amt,vdd_mst_hst.bal,add_ind,curType,cAcno,bakname,br_name1);
	
		}
		  }
		  else
		  	{
		  	ChangeState(&f_dd_mst,"ac_sts",ac_sts);   /**帐户状态**/
		  if(vdd_mst_hst.add_ind[0] == '0')         /**增减标志**/
		  {
		  	strcpy(add_ind,"借");
		  }
		  else
		  {
		  	strcpy(add_ind,"贷");
		  }
		  strcpy(curType,"RMB"); /**货币种类**/
		  memset(br_name,0x00,sizeof(br_name));
		  get_branch(f_dd_mst.opn_br_no,br_name);
			fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
			tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"无",\
			vdd_mst_hst.tx_amt,vdd_mst_hst.bal,add_ind,curType,"无","无","无");
		  	}
		  }
	}
}
	   page_counter=ttlnum/10;
	   if(ttlnum%10!=0)
	   {
	    	page_counter+=1;
	   }
	vtcp_log( "%s,%d, 总记录数[%d]",__FILE__,__LINE__,ttlnum);
	memset(tmp,0x00,sizeof(tmp));
	sprintf(tmp,"%d",ttlnum);
	zip_space(tmp);
	set_zd_data("0480",tmp);
	vtcp_log( "%s,%d, 总页数[%d]",__FILE__,__LINE__,page_counter);
	if(page_counter<atoi(current_page))
	{
		vtcp_log( "无当前页！");
		strcpy( g_pub_tx.reply,"CF35" );
		goto ErrExit;
	}
	ret=Dd_mst_hst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
  fclose(fp);
  set_zd_data( DC_FILE_SND,"1" );
	set_zd_int( "0230", page_counter);
}
OkExit:
	fclose(fp);
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if (fp!=NULL){ fclose(fp); }
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int getAccList(STFSACC *list,int *count)
{
	int i =0;
	char sAccBuf[1024]={'\0'};
	char *p=NULL; 
	get_zd_data("1230",sAccBuf);/*卡号串*/
	p=sAccBuf;
	char *pNew=NULL;
	while(*p)
	{
	  pNew=p;
	  while(1)
	  {
		if (*p=='|')
		{
			*p='\0';
			zip_space(pNew);
			strcpy(list[i].account,pNew);
			/*vtcp_log( "pNew=[%s]",pNew);	*/
			/**根据帐号获取帐号ID**/	
			if(pub_base_acno_acid(&list[i].acid, list[i].account))
			{
				vtcp_log( "%s,%d,获得[%s]账号ac_id出错",__FILE__,__LINE__,list[i].account); 	
				return(-1);
			}
			i++;
			p++;
			break;
		}
	  	p++;
	  }
	}
	*count = i ;
	return(0);
}

void ChangeState(struct dd_mst_c *one, char *stat, char *desc)
{
	if(!strcmp(stat,"ac_sts"))
	{
		if(one->ac_sts[0] == '1')
		{
			strcpy(desc,"正常");
		}
		else if(one->ac_sts[0] == '0')
		{
			strcpy(desc,"开户待确认");
		}
		else if(one->ac_sts[0] == '3')
		{
			strcpy(desc,"挂失结清");
		}
		else if(one->ac_sts[0] == '4')
		{
			strcpy(desc,"开户更正");
		}
		else if(one->ac_sts[0] == '5')
		{
			strcpy(desc,"临时销户");
		}
		else if(one->ac_sts[0] == '*')
		{
			strcpy(desc,"销户");
		}
		else if(one->ac_sts[0] == '#')
		{
			strcpy(desc,"计息结束");
		}	
	}
	else if (!strcmp(stat,"ac_type"))
	{
		if(one->ac_type[0] == '1')
		{
			strcpy(desc,"基本户");
		}
		else if(one->ac_type[0] == '2')
		{
			strcpy(desc,"一般户");
		}
		else if(one->ac_type[0] == '3')
		{
			strcpy(desc,"专用存款户");
		}
		else if(one->ac_type[0] == '4')
		{
			strcpy(desc,"临时存款户");
		}
		else if(one->ac_type[0] == '5')
		{
			strcpy(desc,"个人存款户（储蓄户）");
		}
		else if(one->ac_type[0] == '6')
		{
			strcpy(desc,"协定（理财）户");
		}
		else if(one->ac_type[0] == '7')
		{
			strcpy(desc,"公积金活期户（个人）");
		}
	}
	else if (!strcmp(stat,"intst_type"))
	{
		if(one->intst_type[0] == '0')
		{
			strcpy(desc,"不计息");
		}
		else if(one->intst_type[0] == '1')
		{
			strcpy(desc,"利随本清");
		}
		else if(one->intst_type[0] == '2')
		{
			strcpy(desc,"按日计息");
		}
		else if(one->intst_type[0] == '3')
		{
			strcpy(desc,"按月计息");
		}
		else if(one->intst_type[0] == '4')
		{
			strcpy(desc,"按季计息");
		}
		else if(one->intst_type[0] == '5')
		{
			strcpy(desc,"按年计息");
		}
	}
	else if(!strcmp(stat,"hold_sts"))
	{
		if(one->hold_sts[0] == '0')
		{
			strcpy(desc,"正常");
		}
		else if(one->hold_sts[0] == '1')
		{
			strcpy(desc,"完全冻结");
		}
		else if(one->hold_sts[0] == '2')
		{
			strcpy(desc,"只进不出");
		}
		else if(one->hold_sts[0] == '3')
		{
			strcpy(desc,"部分冻结");
		}
	}
}

double FindBalBydate(long ac_id, long *tx_date)
{
	int    ret = 0;
	long   max_date = 0;
	long   max_trace = 0;
	long   ac_max_trace = 0;
  double c_bal = 0.00;
	struct dd_mst_hst_c sDd_mst_hst;
	ret=sql_max_long_xier("dd_mst_hst","trace_no",&max_trace," ac_id=%ld and tx_date=%ld ",ac_id,*tx_date);
	if(ret)
	{
		vtcp_log("取最大流水号错误ret=[%d]",ret); 
		return -1.00;
	}
	memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
	ret=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld order by trace_cnt desc ",ac_id,max_trace,*tx_date);
	if( ret == 100 )
	{
	   /*如果没有流水就去查找交易明细最大日期的最大流水号对应的余额，并返回*/
	   ret=sql_max_long_xier("dd_mst_hst","tx_date",&max_date," ac_id=%ld and tx_date <= %ld ",ac_id,*tx_date);
	   if( ret )
	   {
				vtcp_log("取明细表最大交易日期错误ret=[%d]",ret); 
				return -1.00;
	   }
	   *tx_date = max_date; /*余额日期*/
	   ret=sql_max_long_xier("dd_mst_hst","trace_no",&ac_max_trace," ac_id=%ld and tx_date=%ld ",ac_id,max_date);
	   if(ret)
	   {
				vtcp_log("取明细表最大流水号错误ret=[%d]",ret); 
				return -1.00;
	   }
	   memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
	   ret=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld order by trace_cnt desc ",ac_id,ac_max_trace,max_date);
	   if( ret == 100 )
	   {
	       c_bal=0.00;
	   }
	   else if( ret == 0 )
	   {
	       c_bal=sDd_mst_hst.bal;
	   }
	   else
	   {
	       vtcp_log( "查询活期明细表错误!"); 
	       return -1.00;
	   }
	}
	else if( ret == 0 )
	{
	   c_bal=sDd_mst_hst.bal;
	}
	else
	{
	   vtcp_log( "查询活期明细表错误!"); 
	   return -1.00;
	}
	return c_bal;
}
int get_branch(char *br,char *name)
{
	int ret=0;
	struct com_branch_c com_branch;
	memset(&com_branch,0x00,sizeof(com_branch));
	ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",br);
	if(ret)
		{
			vtcp_log("查表失败ret=[%d]",ret); 
			return -1;
		}
		strcpy(name,com_branch.br_name);
		return 0;
}
int get_opn_branch(char *br,char *name)
{
	int ret=0;
	struct hv_uniontab_c hv_uniontab;
	memset(&hv_uniontab,0x00,sizeof(hv_uniontab));
	ret=Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no='%s'",br);
	if(ret)
		{
			vtcp_log("查表失败ret=[%d]",ret); 
			return -1;
		}
		strcpy(name,hv_uniontab.br_name_f);
		return 0;
}

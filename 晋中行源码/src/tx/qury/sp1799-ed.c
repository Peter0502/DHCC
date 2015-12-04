/********************************************
*�� �� ��:sp1799.c 
*��������:�������ͻ�÷�˰�����ļ�
*��    ��:zzz 
*�������:2012-10-15
insert into tx_def(tx_code,tx_name,bit_map,log_ind,tx_type,wrk_sts) values('1799','�����ɷ�˰�ļ�','00000000000000000001001111111101010011010001101100000100000001000000000001000000000100000000000000000000000000010000000010000000','0','4','0');
insert into tx_flow_def values('1799',0,'1799','#$');
insert into tx_sub_def values('1799','�������ͻ�ȡ��˰�ļ�','sp1799','0','0');
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
	int  ttlnum = 0;   /**�ܼ�¼��**/
	long   cEnddate = 0;
	long   cDate = 0;
	long nowDate = 0;
	long startDate = 0;
	double dSumBal =0.00;
	int  page_counter=0;         /**��ҳ��**/
	char current_page[3] = {0};
	char sFileName[128]={'\0'};
		char tmpac_no[20];
	char buf[64]={'\0'};
	char sAccBuf[1024]={'\0'};
	char sType[16]={'\0'};
	char sDate[12]={'\0'},sDateBegin[12]={'\0'},sDateEnd[12]={'\0'};
	char ac_sts[30]  = {0};
	char add_ind[5]  = {0};
	char curType[5]  = {0};      /**��ѯҳ��**/
	char tmpstr[512] = {0};
	char cCardno[21] = {0};
	char spaceacno[33]={0};
	char tmp[5]={0};
	char bakname[61];
	char cAcno[21]   = {0};
	char filename[100]   = {0};
	char wherestr[1024]  = {0};
	char wherelist[1024] = {0};  /**��ѯ����**/
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
		set_zd_data("0130","���Ļ�ȡ��ǰ���ڳ���");
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
	get_zd_data("0470",sType);/*�ļ�����*/
	zip_space(sType);
	if(memcmp(cCardno,"5001620300015",13)==0)
		{
	if (getAccList(accList,&iCount))
	{
		vtcp_log("[%s][%d] ����˺���Ϣ����",__FILE__,__LINE__);
		goto ErrExit;
	}
	vtcp_log( "[%s][%d] �˺Ÿ���[%d]",__FILE__,__LINE__,iCount);
	vtcp_log( "[%s][%d] �ļ�����[%s]",__FILE__,__LINE__,sType);	
}
	else
		{
			get_zd_data("0380",accList[0].account);
			pub_base_acno_acid(&accList[0].acid, accList[0].account);
			iCount=1;
		}
	if (strcmp(sType,"9995")==0) /*�����շ�˰����ɿ��¼�����ļ�,���� */
	{
		/*if (getAccList(accList,&iCount))
	{
		vtcp_log("[%s][%d] ����˺���Ϣ����",__FILE__,__LINE__);
		goto ErrExit;
	}*/
		get_zd_data("0450",sDateBegin);/*���˿�ʼ����*/
		get_zd_data("0460",sDateEnd);/*���˽�������*/
		get_zd_data("0300",dz_acno);
		zip_space(dz_acno);
		vtcp_log( "[%s][%d] ����[%s]ac_no==[%s]",__FILE__,__LINE__,sDateBegin,dz_acno);	
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'",dz_acno);
			if(ret)
				  {
						vtcp_log( "��ѯ��������Ϣ��st_mdm_ac_relʧ��ret=[%d]",ret);
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
		vtcp_log( "[%s][%d] ��¼��[%d]",__FILE__,__LINE__,iNum);
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1" );
		vtcp_log( "here1 ok");
	}
else if(strncmp(sType,"8102",4)==0||strncmp(sType,"8101",4)==0)/*���˻�״̬and���˻����*/
	{
				for(i=0 ; i<iCount;i++)
		{	
			ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst," ac_id = %ld", accList[i].acid);  
			if(ret)
			{
				vtcp_log( "��ѯ���ڴ�����ļ�ʧ��");
				goto ErrExit;  
			}
			if(memcmp(dd_mst.opn_br_no,"10201",4)==0)
				{
					ChangeState(&dd_mst,"ac_sts",ac_sts);/*�ʻ�״̬*/
					set_zd_data("0380", cCardno);
	        set_zd_data("0250", dd_mst.name);
	        set_zd_data("0370", "����֧��");
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
else if(strncmp(sType,"8103",4)==0||strncmp(sType,"8104",4)==0)/*�鵱����ϸ*/
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
		/**ÿҳ��ʾ10��**/
		if(((ttlnum/10+1)==atoi(current_page))&&(ttlnum%10!=0)||((ttlnum%10==0)&&ttlnum/10==atoi(current_page)))
		{
			ret=Dd_mst_Sel(g_pub_tx.reply,&f_dd_mst," ac_id=%ld ", vdd_mst_hst.ac_id);
		  if(ret)
		  {
				vtcp_log( "��ѯ���ڴ�����ļ�ʧ��");
				goto ErrExit;
		  }
		  memset( cAcno,0,sizeof(cAcno) );
		  if(vdd_mst_hst.ct_ind[0] == '2')         /**ת����Ľ���**/
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
						vtcp_log( "��ѯ���֧����Ϣʧ��");
						goto ErrExit;
				  }
				 ChangeState(&f_dd_mst,"ac_sts",ac_sts);
				memset(br_name,0x00,sizeof(br_name));
		  get_branch(vdd_mst_hst.opn_br_no,br_name);
		   memset(br_name1,0x00,sizeof(br_name1));
		  get_opn_branch(hv_zf.pay_opn_br_no,br_name1);
			fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
			tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"��",\
			vdd_mst_hst.tx_amt,vdd_mst_hst.bal,"��","RMB",hv_zf.pay_ac_no,hv_zf.pay_name,br_name1); 
		  		}
		  	else	if(memcmp(vdd_mst_hst.tx_code,"5840",4)==0||memcmp(vdd_mst_hst.tx_code,"5880",4)==0)
		  		{
		  			ret=Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg,"in_date=%ld and cash_ac_no='%s' and tx_amt=%.2f",vdd_mst_hst.tx_date,tmpac_no,vdd_mst_hst.tx_amt);
		  			if(ret && ret != 100)
				  {
						vtcp_log( "��ѯС��֧����Ϣʧ��");
						goto ErrExit;
				  }
				 ChangeState(&f_dd_mst,"ac_sts",ac_sts);
				memset(br_name,0x00,sizeof(br_name));
		  get_branch(vdd_mst_hst.opn_br_no,br_name);
		   memset(br_name1,0x00,sizeof(br_name1));
		  get_opn_branch(lv_pkgreg.pay_opn_br_no,br_name1);
			fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
			tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"��",\
			vdd_mst_hst.tx_amt,vdd_mst_hst.bal,"��","RMB",lv_pkgreg.pay_ac_no,lv_pkgreg.pay_name,br_name1); 
		  	
		  		}
			else if(memcmp(vdd_mst_hst.tx_code,"6233",4)==0||memcmp(vdd_mst_hst.tx_code,"6236",4)==0||memcmp(vdd_mst_hst.tx_code,"6235",4)==0||memcmp(vdd_mst_hst.tx_code,"7022",4)==0||memcmp(vdd_mst_hst.tx_code,"7023",4)==0||memcmp(vdd_mst_hst.tx_code,"6247",4)==0)
			{
				ret=Chnl_tx_detl_Sel(g_pub_tx.reply,&chnl_tx_detl,"tx_date=%ld and df_ac_no='%s' and tx_amt=%.2f",vdd_mst_hst.tx_date,tmpac_no,vdd_mst_hst.tx_amt);
				if(ret && ret != 100)
				{
					vtcp_log( "��ѯ֧����Ϣʧ��");
					goto ErrExit;
				}
				ChangeState(&f_dd_mst,"ac_sts",ac_sts);
				memset(br_name,0x00,sizeof(br_name));
				get_branch(vdd_mst_hst.opn_br_no,br_name);
				memset(br_name1,0x00,sizeof(br_name1));
				get_opn_branch(chnl_tx_detl.df_br_no,br_name1);
				fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
				tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"��",\
				vdd_mst_hst.tx_amt,vdd_mst_hst.bal,"��","RMB",chnl_tx_detl.ac_no,chnl_tx_detl.ac_name,br_name1); 
			}
		  	else
		  	{
		  	/**����ǵ��췢����ת�˽��ף����trace_log��ȡת�˶Է��ʺ�**/
		  	if(vdd_mst_hst.tx_date == nowDate)
		  	{
		  		memset( tmpstr,0,sizeof(tmpstr) );
		  		memset( wherestr,0,sizeof(wherestr) );
		  		memset( &f_trace_log,0,sizeof(f_trace_log));
		  		ret = Trace_log_Sel(g_pub_tx.reply, &f_trace_log, "trace_no=%ld and tx_date=%ld and ct_ind='2' and ac_no is not NULL and ac_no !='%s' order by tx_date desc",\
		  		                    vdd_mst_hst.trace_no, vdd_mst_hst.tx_date,accList[i].account);
		  		if(ret && ret != 100)
				  {
						vtcp_log( "��ѯ��ˮ��trace_log��Ϣʧ��");
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
						vtcp_log( "��ѯ��ˮ��trace_log_rz��Ϣʧ��");
						goto ErrExit;
				  }
				  strcpy(cAcno,vtrace_log_rz.ac_no);
		  		sprintf( tmpstr," ac_no ='%s' order by ac_no desc",vtrace_log_rz.ac_no);
		  		strcat( wherestr,tmpstr );
		  	}
		  	/**���len=0��˵�����ڼ��̫�����ñ�ת�˶Է�����Ϣ�Ѿ�������**/
		  	if(strlen(cAcno) > 1)
		  	{
		  		memset(bakname,0x00,sizeof(bakname));
		  		memset( &st_mdm_ac_rel,0,sizeof(st_mdm_ac_rel));
			  	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'",cAcno);
			  	if(ret && ret != 100)
				  {
						vtcp_log( "��ѯ��������Ϣ��st_mdm_ac_relʧ��ret=[%d]",ret);
						goto ErrExit;
				  }
				  
				  if(cAcno[0]=='9')
				  	{
				  		ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",st_mdm_ac_rel.ac_id);
				  		if(ret && ret != 100)
				  {
						vtcp_log( "��ѯ��������Ϣ��st_mdm_ac_relʧ��ret=[%d]",ret);
						goto ErrExit;
				  }
				  strcpy(bakname,in_mst.name);
				  	}
				  	else
				  		{
				  			ret=Dd_mst_Sel(g_pub_tx.reply,&b_dd_mst," ac_id=%ld ",st_mdm_ac_rel.ac_id);
		  if(ret)
		  {
				vtcp_log( "��ѯ���ڴ�����ļ�ʧ��");
				goto ErrExit;
		  }
		   strcpy(bakname,b_dd_mst.name);
				  		} 	
		  	}
		  	ChangeState(&f_dd_mst,"ac_sts",ac_sts);   /**�ʻ�״̬**/
		  if(vdd_mst_hst.add_ind[0] == '0')         /**������־**/
		  {
		  	strcpy(add_ind,"��");
		  }
		  else
		  {
		  	strcpy(add_ind,"��");
		  }
		  strcpy(curType,"RMB"); /**��������**/
		  memset(br_name,0x00,sizeof(br_name));
		  get_branch(f_dd_mst.opn_br_no,br_name);
		   memset(br_name1,0x00,sizeof(br_name1));
		  get_branch(st_mdm_ac_rel.opn_br_no,br_name1);
			fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
			tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"��",\
			vdd_mst_hst.tx_amt,vdd_mst_hst.bal,add_ind,curType,cAcno,bakname,br_name1);
	
		}
		  }
		  else
		  	{
		  	ChangeState(&f_dd_mst,"ac_sts",ac_sts);   /**�ʻ�״̬**/
		  if(vdd_mst_hst.add_ind[0] == '0')         /**������־**/
		  {
		  	strcpy(add_ind,"��");
		  }
		  else
		  {
		  	strcpy(add_ind,"��");
		  }
		  strcpy(curType,"RMB"); /**��������**/
		  memset(br_name,0x00,sizeof(br_name));
		  get_branch(f_dd_mst.opn_br_no,br_name);
			fprintf( fp,"%s|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
			tmpac_no,f_dd_mst.name,br_name,ac_sts,vdd_mst_hst.tx_date,vdd_mst_hst.brf,vdd_mst_hst.trace_no,"��",\
			vdd_mst_hst.tx_amt,vdd_mst_hst.bal,add_ind,curType,"��","��","��");
		  	}
		  }
	}
}
	   page_counter=ttlnum/10;
	   if(ttlnum%10!=0)
	   {
	    	page_counter+=1;
	   }
	vtcp_log( "%s,%d, �ܼ�¼��[%d]",__FILE__,__LINE__,ttlnum);
	memset(tmp,0x00,sizeof(tmp));
	sprintf(tmp,"%d",ttlnum);
	zip_space(tmp);
	set_zd_data("0480",tmp);
	vtcp_log( "%s,%d, ��ҳ��[%d]",__FILE__,__LINE__,page_counter);
	if(page_counter<atoi(current_page))
	{
		vtcp_log( "�޵�ǰҳ��");
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
	get_zd_data("1230",sAccBuf);/*���Ŵ�*/
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
			/**�����ʺŻ�ȡ�ʺ�ID**/	
			if(pub_base_acno_acid(&list[i].acid, list[i].account))
			{
				vtcp_log( "%s,%d,���[%s]�˺�ac_id����",__FILE__,__LINE__,list[i].account); 	
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
			strcpy(desc,"����");
		}
		else if(one->ac_sts[0] == '0')
		{
			strcpy(desc,"������ȷ��");
		}
		else if(one->ac_sts[0] == '3')
		{
			strcpy(desc,"��ʧ����");
		}
		else if(one->ac_sts[0] == '4')
		{
			strcpy(desc,"��������");
		}
		else if(one->ac_sts[0] == '5')
		{
			strcpy(desc,"��ʱ����");
		}
		else if(one->ac_sts[0] == '*')
		{
			strcpy(desc,"����");
		}
		else if(one->ac_sts[0] == '#')
		{
			strcpy(desc,"��Ϣ����");
		}	
	}
	else if (!strcmp(stat,"ac_type"))
	{
		if(one->ac_type[0] == '1')
		{
			strcpy(desc,"������");
		}
		else if(one->ac_type[0] == '2')
		{
			strcpy(desc,"һ�㻧");
		}
		else if(one->ac_type[0] == '3')
		{
			strcpy(desc,"ר�ô�");
		}
		else if(one->ac_type[0] == '4')
		{
			strcpy(desc,"��ʱ��");
		}
		else if(one->ac_type[0] == '5')
		{
			strcpy(desc,"���˴��������");
		}
		else if(one->ac_type[0] == '6')
		{
			strcpy(desc,"Э������ƣ���");
		}
		else if(one->ac_type[0] == '7')
		{
			strcpy(desc,"��������ڻ������ˣ�");
		}
	}
	else if (!strcmp(stat,"intst_type"))
	{
		if(one->intst_type[0] == '0')
		{
			strcpy(desc,"����Ϣ");
		}
		else if(one->intst_type[0] == '1')
		{
			strcpy(desc,"���汾��");
		}
		else if(one->intst_type[0] == '2')
		{
			strcpy(desc,"���ռ�Ϣ");
		}
		else if(one->intst_type[0] == '3')
		{
			strcpy(desc,"���¼�Ϣ");
		}
		else if(one->intst_type[0] == '4')
		{
			strcpy(desc,"������Ϣ");
		}
		else if(one->intst_type[0] == '5')
		{
			strcpy(desc,"�����Ϣ");
		}
	}
	else if(!strcmp(stat,"hold_sts"))
	{
		if(one->hold_sts[0] == '0')
		{
			strcpy(desc,"����");
		}
		else if(one->hold_sts[0] == '1')
		{
			strcpy(desc,"��ȫ����");
		}
		else if(one->hold_sts[0] == '2')
		{
			strcpy(desc,"ֻ������");
		}
		else if(one->hold_sts[0] == '3')
		{
			strcpy(desc,"���ֶ���");
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
		vtcp_log("ȡ�����ˮ�Ŵ���ret=[%d]",ret); 
		return -1.00;
	}
	memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
	ret=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld order by trace_cnt desc ",ac_id,max_trace,*tx_date);
	if( ret == 100 )
	{
	   /*���û����ˮ��ȥ���ҽ�����ϸ������ڵ������ˮ�Ŷ�Ӧ����������*/
	   ret=sql_max_long_xier("dd_mst_hst","tx_date",&max_date," ac_id=%ld and tx_date <= %ld ",ac_id,*tx_date);
	   if( ret )
	   {
				vtcp_log("ȡ��ϸ����������ڴ���ret=[%d]",ret); 
				return -1.00;
	   }
	   *tx_date = max_date; /*�������*/
	   ret=sql_max_long_xier("dd_mst_hst","trace_no",&ac_max_trace," ac_id=%ld and tx_date=%ld ",ac_id,max_date);
	   if(ret)
	   {
				vtcp_log("ȡ��ϸ�������ˮ�Ŵ���ret=[%d]",ret); 
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
	       vtcp_log( "��ѯ������ϸ�����!"); 
	       return -1.00;
	   }
	}
	else if( ret == 0 )
	{
	   c_bal=sDd_mst_hst.bal;
	}
	else
	{
	   vtcp_log( "��ѯ������ϸ�����!"); 
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
			vtcp_log("���ʧ��ret=[%d]",ret); 
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
			vtcp_log("���ʧ��ret=[%d]",ret); 
			return -1;
		}
		strcpy(name,hv_uniontab.br_name_f);
		return 0;
}

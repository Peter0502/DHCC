/*************************************************
* 文 件 名: sp9321.c
* 功能描述：身份证验证查询
* 
* 作    者: houyi
* 完成日期：20120323

核查结果：
	00	公民身份号码与姓名一致，且存在照片	
	01	公民身份号码与姓名一致，但不存在照片	
	02	公民身份号码存在，但与姓名不匹配	
	03	公民身份号码不存在	
	04	其他错误	
	05	公民身份信息无法核实	
	06	公民身份信息虚假
	07	三年及三年以上不动户
insert into tx_def values('9321','身份证验证查询','10000000000000000000000010001100000000000000000000000000000001000010000000000000000000000000000000000000000000000000000000000100','0','5','0','0',0.00);
insert into tx_flow_def values('9321',0,'9321','#$');
insert into tx_sub_def values('9321','身份证验证登记','sp9321','0','0');
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_per_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "com_tel_c.h"
#include "cif_inf_c.h"
#include "cif_sfhc_inf_c.h"

sp9321()
{
	long lCif_no=0;
	long start_date=0;
	long end_date=0;
	int  ttlnum=0;
	char sSource_type[2],sAcno[25],sName[128],sIdno[25],result[64],wh_flag[2],ID_TYPE[2],cx_flag[2];
	char sWherelist[512],sFilename[256],sTota[128];
	struct cif_basic_inf_c sBasic;
	struct cif_per_inf_c   sPer_inf;
	struct cif_sfhc_inf_c  sCif_sfhc_inf;
	struct cif_addr_inf_c  sAddr_inf;
	struct cif_email_inf_c sEmail_inf1,sEmail_inf2;
	struct cif_id_code_rel_c sCif_id;
	struct com_tel_c sCom_tel;
	struct cif_inf_c sCif_inf;
	FILE   *fp=NULL;
	
	memset(sSource_type,0x00,sizeof(sSource_type));
	memset(sWherelist,0x00,sizeof(sWherelist));
	memset(sAcno,0x00,sizeof(sAcno));
	memset(sName,0x00,sizeof(sName));
  memset(cx_flag,0x00,sizeof(cx_flag));
	memset(sIdno,0x00,sizeof(sIdno));
	memset(sFilename,0x00,sizeof(sFilename));
	memset(result,0x00,sizeof(result));
	memset(wh_flag,0x00,sizeof(wh_flag));
	memset(ID_TYPE,0x00,sizeof(ID_TYPE));
	
	memset(&sBasic,0x00,sizeof(sBasic));
	memset(&sPer_inf,0x00,sizeof(sPer_inf));
	memset(&sAddr_inf,0x00,sizeof(sAddr_inf));
	memset(&sEmail_inf1,0x00,sizeof(sEmail_inf1));
	memset(&sEmail_inf2,0x00,sizeof(sEmail_inf2));
	memset(&sCif_id,0x00,sizeof(sCif_id));
	memset(&sCom_tel,0x00,sizeof(sCom_tel));
	memset(&sCif_inf,0x00,sizeof(sCif_inf));
	memset(&sCif_sfhc_inf,0x00,sizeof(sCif_sfhc_inf));
	
	pub_base_sysinit();
	
	get_zd_data("0670",sSource_type);
	get_zd_data("0680",wh_flag);
	get_zd_data("0690",ID_TYPE);
	get_zd_data("0300",sAcno);
	get_zd_data("0250",sName);
	get_zd_data("0620",sIdno);
	get_zd_long("0280",&lCif_no);
	get_zd_long("0440",&(g_pub_tx.open_date));
	get_zd_long("0450",&start_date);
	get_zd_long("0460",&end_date);
	get_zd_data("0920",g_pub_tx.chk);
	get_zd_data("0020",g_pub_tx.opn_br_no);
	get_zd_data("0200",cx_flag);
	
	pub_base_AllDwnFilName( sFilename );
	fp=fopen(sFilename,"w");
	if( fp==NULL )
	{
		vtcp_log("[%s][%d] 打开文件错误 文件名：%s",sFilename );
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	if(sSource_type[0]=='1')
		fprintf(fp,"~@客户号码|@客户名称|@证件号码|@核查结果|@联系方式|@维护机构|@建立日期|@维护日期|\n");
	else
	{
		if(wh_flag[0]=='1')
			fprintf(fp,"~@客户号码|@客户名称|@证件号码|@核查结果|@联系方式|@维护机构|@维护柜员|@维护日期|\n");
		else
			fprintf(fp,"~@客户号码|@客户名称|@证件号码|@核查结果|@联系方式|@查询机构|@建立日期|\n");
	}
	
	if(sSource_type[0]=='1')
	{/**根据账号查询**/
		if(strlen(sAcno)<=0)
		{
			vtcp_log("[%s][%d] 账号输入 错误 %s",__FILE__,__LINE__,sAcno);
			strcpy(g_pub_tx.reply,"L113");
			goto ErrExit;
		}
		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",sAcno);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] 账号输入 错误 %s ",__FILE__,__LINE__,sAcno);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		g_reply_int=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id,"id_type='%s' and id_no='%s' order by id_type,cif_no",g_mdm_ac_rel.id_type,g_mdm_ac_rel.id_no);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] Cif_id_code_rel查询 错误 ",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"B115");
			goto ErrExit;
		}
		g_reply_int=Cif_basic_inf_Sel(g_pub_tx.reply,&sBasic,"cif_no=%ld",sCif_id.cif_no);
		if(g_reply_int!=100 && g_reply_int)
		{
			vtcp_log("[%s][%d] Cif_basic_inf查询 错误 %d ",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		g_reply_int=Cif_sfhc_inf_Sel(g_pub_tx.reply,&sCif_sfhc_inf,"cif_no=%ld ",sBasic.cif_no);
		if(g_reply_int==100)
		{
			goto OkExit;
		}
		else if(g_reply_int)
		{
			vtcp_log("[%s][%d] Cif_sfhc_inf 错误 %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		switch(sCif_sfhc_inf.country[1])
		{
			case '0' :
				strcpy(result,"公民身份号码与姓名一致，且存在照片");
				break;
			case '1' :
				strcpy(result,"公民身份号码与姓名一致，但不存在照片");
				break;
			case '2' :
				strcpy(result,"公民身份号码存在，但与姓名不匹配");
				break;
			case '3' :
				strcpy(result,"公民身份号码不存在");
				break;
			case '4' :
				strcpy(result,"其他错误");
				break;
			case '5' :
				strcpy(result,"公民身份信息无法核实");
				break;
			case '6' :
				strcpy(result,"公民身份信息虚假");
				break;
			case '7' :
				strcpy(result,"三年及三年以上不动户");
				break;
			default :
				strcpy(result,"");
				break;
		}
		
		g_reply_int=Com_tel_Sel(g_pub_tx.reply,&sCom_tel,"tel='%s' ",sCif_sfhc_inf.clerk_no);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] Com_tel 错误 %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		
		g_reply_int=Cif_addr_inf_Sel(g_pub_tx.reply,&sAddr_inf,"cif_no=%ld and addr_type='2'  order by addr_seqn desc",sBasic.cif_no);
		if(g_reply_int!=100 && g_reply_int)
		{
			vtcp_log("[%s][%d] Cif_addr_inf查询 错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		g_reply_int=Cif_email_inf_Sel(g_pub_tx.reply,&sEmail_inf1,"cif_no=%ld and addr_type='1' order by addr_seqn desc",sBasic.cif_no);
		if(g_reply_int!=100 && g_reply_int)
		{
			vtcp_log("[%s][%d] Cif_email_inf 查询错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		g_reply_int=Cif_email_inf_Sel(g_pub_tx.reply,&sEmail_inf2,"cif_no=%ld and addr_type='5' order by addr_seqn desc",sBasic.cif_no);
		if(g_reply_int!=100 && g_reply_int)
		{
			vtcp_log("[%s][%d] Cif_email_inf 查询错误  %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
	/*	fprintf(fp,"%ld|%s%|%20s|%20s|%14s|%14s|%8s|%61s|\n",sBasic.cif_no,sBasic.name,sCif_id.id_no,result,sEmail_inf1.email,sEmail_inf2.email,sAddr_inf.post_code,sAddr_inf.addr);*/
		fprintf(fp,"%ld|%s%|%20s|%20s|%14s|%s|%ld|%ld|\n",sBasic.cif_no,sBasic.name,sCif_id.id_no,result,sEmail_inf2.email,sCom_tel.br_no,sBasic.crt_date,sCif_sfhc_inf.mar_date);

	}
	else /**根据客户信息查询**/
	{
		/*已维护*/
		if(wh_flag[0]=='1')
		{
			vtcp_log("[%s][%d] cx_flag[0]= %c",__FILE__,__LINE__,cx_flag[0]);
			sprintf(sWherelist,"1=1");
			if(lCif_no>0)
				sprintf(sWherelist+strlen(sWherelist)," and cif_no=%ld ",lCif_no);
			if(strlen(sName)>0)
				sprintf(sWherelist+strlen(sWherelist)," and name like '%%%%%s%%%%' ",sName);
			if(strlen(sIdno)>0)
				sprintf(sWherelist+strlen(sWherelist)," and cif_no=(select cif_no from cif_id_code_rel where id_type='%s' and id_no='%s') ",g_mdm_ac_rel.id_type,sIdno);
			
			/* 如果是客户端来查询 20域标志传1   如果是1 不加下面的条件 */
			if(cx_flag[0] != '1')
				if( (strlen(g_pub_tx.chk)>0)||(strlen(g_pub_tx.opn_br_no)>0)||(g_pub_tx.open_date>0))
				{
					memset(sTota,0x00,sizeof(sTota));
					sprintf(sWherelist+strlen(sWherelist)," and cif_no in(select cif_no from cif_sfhc_inf where ");
					if(strlen(g_pub_tx.chk)>0)
						sprintf(sTota,"  clerk_no='%s' ",g_pub_tx.chk);
					if(g_pub_tx.open_date>0)
					{
						if(strlen(sTota)>0)
							sprintf(sTota+strlen(sTota), " and mar_date=%ld",g_pub_tx.open_date);
						else
							sprintf(sTota,"  mar_date=%ld ",g_pub_tx.open_date);
					}
					if(strlen(g_pub_tx.opn_br_no)>0)
					{
						if(strlen(sTota)>0)
							sprintf(sTota+strlen(sTota), " and ( clerk_no in(select tel from com_tel where br_no='%s' )) ",g_pub_tx.opn_br_no);
						else
							sprintf(sTota, " clerk_no in(select tel from com_tel where br_no='%s' ) " ,g_pub_tx.opn_br_no);
					}
					sprintf(sWherelist+strlen(sWherelist)," %s )",sTota);	
				}
    	
			g_reply_int=Cif_basic_inf_Dec_Sel(g_pub_tx.reply,sWherelist);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d] Cif_basic_inf生命游标错误 错误 %d",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}
			while(1)
			{
				memset(result,0x00,sizeof(result));
				memset(&sBasic,0x00,sizeof(sBasic));
				memset(&sPer_inf,0x00,sizeof(sPer_inf));
				memset(&sAddr_inf,0x00,sizeof(sAddr_inf));
				memset(&sEmail_inf1,0x00,sizeof(sEmail_inf1));
				memset(&sEmail_inf2,0x00,sizeof(sEmail_inf2));
				memset(&sCif_id,0x00,sizeof(sCif_id));
				memset(&sCom_tel,0x00,sizeof(sCom_tel));
				memset(&sCif_sfhc_inf,0x00,sizeof(sCif_sfhc_inf));
				
				g_reply_int=Cif_basic_inf_Fet_Sel(&sBasic,g_pub_tx.reply);
				if(g_reply_int==100)
				{
					break;
				}
				else if(g_reply_int)
				{
					vtcp_log("[%s][%d] Cif_basic_inf_Fet_Sel 错误 %d",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"D102");
					goto ErrExit;
				}
    	
				if(strlen(sIdno)==0)
				{
					g_reply_int=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id,"cif_no=%ld ",sBasic.cif_no);
					if(g_reply_int)
					{
						vtcp_log("[%s][%d] Cif_id_code_rel_Sel 错误 %d",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
				}
				else
					strcpy(sCif_id.id_no,sIdno);
					
				g_reply_int=Cif_sfhc_inf_Sel(g_pub_tx.reply,&sCif_sfhc_inf,"cif_no=%ld",sBasic.cif_no);
				if(g_reply_int==100)
				{
					continue;
				}
				else if(g_reply_int)
				{
					vtcp_log("[%s][%d] Cif_sfhc_inf_Sel 错误 %d",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				switch(sCif_sfhc_inf.country[1])
				{
					case '0' :
						strcpy(result,"00.公民身份号码与姓名一致，且存在照片");
						break;
					case '1' :
						strcpy(result,"01.公民身份号码与姓名一致，但不存在照片");
						break;
					case '2' :
						strcpy(result,"02.公民身份号码存在，但与姓名不匹配");
						break;
					case '3' :
						strcpy(result,"03.公民身份号码不存在");
						break;
					case '4' :
						strcpy(result,"04.其他错误");
						break;
					case '5' :
						strcpy(result,"05.公民身份信息无法核实");
						break;
					case '6' :
						strcpy(result,"06.公民身份信息虚假");
						break;
					case '7' :
						strcpy(result,"07.三年及三年以上不动户");
						break;
						
					default :
						strcpy(result,"");
						break;
				}
				
				g_reply_int=Com_tel_Sel(g_pub_tx.reply,&sCom_tel,"tel=%s",sCif_sfhc_inf.clerk_no);
				if(g_reply_int!=100 && g_reply_int)
				{
					vtcp_log("[%s][%d] Com_tel_Sel 错误 %d ",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_reply_int=Cif_addr_inf_Sel(g_pub_tx.reply,&sAddr_inf,"cif_no=%ld and addr_type='2'  order by addr_seqn desc",sBasic.cif_no);
				if(g_reply_int!=100 && g_reply_int)
				{
					vtcp_log("[%s][%d] Cif_addr_inf_Sel 错误 %d ",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_reply_int=Cif_email_inf_Sel(g_pub_tx.reply,&sEmail_inf1,"cif_no=%ld and addr_type='1' order by addr_seqn desc",sBasic.cif_no);
				if(g_reply_int!=100 && g_reply_int)
				{
					vtcp_log("[%s][%d] Cif_email_inf_Sel 错误 %d ",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_reply_int=Cif_email_inf_Sel(g_pub_tx.reply,&sEmail_inf2,"cif_no=%ld and addr_type='5' order by addr_seqn desc",sBasic.cif_no);
				if(g_reply_int!=100 && g_reply_int)
				{
					vtcp_log("[%s][%d] Cif_email_inf_Sel 错误 %d ",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				fprintf(fp,"%ld|%s%|%20s|%20s|%14s|%5s|%s|%ld|\n",sBasic.cif_no,sBasic.name,sCif_id.id_no,result,sEmail_inf2.email,sCom_tel.br_no,sCom_tel.tel,sCif_sfhc_inf.mar_date);
				ttlnum++;
			}
			Cif_basic_inf_Clo_Sel(); 
			if(!ttlnum)
			{
				sprintf(acErrMsg,"无符合的记录!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "S047");
				goto ErrExit;
			}
		}
		/*未维护*/
		else
		{
/*****
			sprintf(sWherelist,"1=1 and sts='1' ");

			if(lCif_no>0)
				sprintf(sWherelist+strlen(sWherelist)," and cif_no=%ld ",lCif_no);
			if(strlen(sName)>0)
				sprintf(sWherelist+strlen(sWherelist)," and name like '%%%%%s%%%%' ",sName);
			if(strlen(sIdno)>0)
				sprintf(sWherelist+strlen(sWherelist)," and cif_no=(select cif_no from cif_id_code_rel where id_type='1' and id_no='%s') ",sIdno);

			if(strlen(g_pub_tx.opn_br_no)>0)
				sprintf(sWherelist+strlen(sWherelist)," and br_no='%s' ",g_pub_tx.opn_br_no);
			if(start_date>0)
			{
				sprintf(sWherelist+strlen(sWherelist)," and crt_date>=%ld ",start_date);
			}
			if(end_date>0)
			{
				sprintf(sWherelist+strlen(sWherelist)," and crt_date<=%ld ",end_date);
			}
			
			sprintf(sWherelist,"order by crt_date");
******/
			/**** 按财务要求 不采集 客户号已经注销的 modify by wudawei 20130925
			g_reply_int=Cif_inf_Dec_Sel(g_pub_tx.reply,"sts='1' and br_no='%s' and crt_date>=%ld and crt_date<=%ld order by crt_date", g_pub_tx.opn_br_no, start_date, end_date);
			modify by wudawei 20130925 end   ***/
			g_reply_int=Cif_inf_Dec_Sel(g_pub_tx.reply,"sts='1' and br_no='%s' and crt_date>=%ld and crt_date<=%ld and cif_no in (select cif_no from cif_basic_inf where sts='1') order by crt_date", g_pub_tx.opn_br_no, start_date, end_date);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d] Cif_basic_inf生命游标错误 错误 %d",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}
			while(1)
			{
				memset(result,0x00,sizeof(result));
				memset(&sCif_inf,0x00,sizeof(sCif_inf));
				
				g_reply_int=Cif_inf_Fet_Sel(&sCif_inf,g_pub_tx.reply);
				if(g_reply_int==100)
				{
					break;
				}
				else if(g_reply_int)
				{
					vtcp_log("[%s][%d] Cif_basic_inf_Fet_Sel 错误 %d",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"D102");
					goto ErrExit;
				}
    
				g_reply_int=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id,"cif_no=%ld and id_type='%s' ",sCif_inf.cif_no, ID_TYPE);
				if(g_reply_int && g_reply_int!=100)
				{
					vtcp_log("[%s][%d] Cif_id_code_rel_Sel 错误 %d",__FILE__,__LINE__,g_reply_int);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				else if(g_reply_int==100)
					continue;
						
				strcpy(result,"未维护");
				fprintf(fp,"%ld|%s%|%20s|%20s|%s|%s|%ld|\n",sCif_inf.cif_no,sCif_inf.name,sCif_id.id_no,result,"",g_pub_tx.opn_br_no,sCif_inf.crt_date);
				ttlnum++;
			}
			Cif_inf_Clo_Sel();		
			if(!ttlnum)
			{
				sprintf(acErrMsg,"无符合的记录!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "S047");
				goto ErrExit;
			}
		}
	}
	
OkExit:
	fclose(fp);
	set_zd_data( DC_FILE_SND,"1" );
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	set_zd_data("0120",g_pub_tx.reply);
	return g_reply_int;
}

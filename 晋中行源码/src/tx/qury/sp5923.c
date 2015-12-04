/*************************************************
* 文 件 名:  sp5923.c
* 功能描述： 异地交易查询
* 作    者:  ligl
* 完成日期： 2006年10月18日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "lv_pub.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "in_mst_c.h"
#include "dc_log_c.h"
#include "dc_log_bk_c.h"
#include "trace_log_bk_c.h"
sp5923()
{
  int ret=0,ret1=0,ret2=0,i;
	struct trace_log_c trace_log;
	char cBr_no[6];  /*机构号  */
	long lTx_bdate=0;/*交易起始日期*/
	long lTx_edate=0;/*交易终止日期*/
	char cPrtflag[2];/*打印标志*/
	char cSe_type[2];/*交易类型*/
	char cTx_code[5];/*交易代码*/
	char cName1[101];
	char cName2[101];
	struct com_item_c com_item;
	struct in_mst_c in_mst;
	char cFilename[100];    /*文件名*/
	int ttlnum=0;	          /**读取的总条数**/
	FILE *fp;
	char wherelist[1024];  /**查询条件**/
	char cNote_no[17];
	char cPrtname[7];
  char cDCname[5];
  char cXZname[8];
  char cStatname[5];
  char cAc_no[21];
  char tmpstr[200];
  struct prdt_ac_id_c prdt_ac_id1;
	struct cif_basic_inf_c cif_basic_inf1;
	struct mdm_ac_rel_c mdm_ac_rel1;
	struct prdt_ac_id_c prdt_ac_id2;
	struct cif_basic_inf_c cif_basic_inf2;
	struct mdm_ac_rel_c mdm_ac_rel2;
	struct dc_log_c dc_log;
	struct dc_log_bk_c dc_log_bk;
	struct trace_log_bk_c trace_log_bk;
	memset(&trace_log_bk,0x0,sizeof(struct trace_log_bk_c));
	memset(&dc_log_bk,0x0,sizeof(struct dc_log_bk_c));
	memset(&prdt_ac_id1,0x0,sizeof(prdt_ac_id1));
  memset(&cif_basic_inf1,0x0,sizeof(cif_basic_inf1));
  memset(&mdm_ac_rel1,0x0,sizeof(mdm_ac_rel1));
  memset(&prdt_ac_id2,0x0,sizeof(prdt_ac_id2));
  memset(&cif_basic_inf2,0x0,sizeof(cif_basic_inf2));
  memset(&mdm_ac_rel2,0x0,sizeof(mdm_ac_rel2));
  memset(&dc_log,0x0,sizeof(struct dc_log_c));
  memset(cName1,0x0,sizeof(cName1));
  memset(cName2,0x0,sizeof(cName2));
  memset(&com_item,0x0,sizeof(struct com_item_c));
  memset(&in_mst,0x0,sizeof(struct in_mst_c));
  memset(tmpstr,0,sizeof(tmpstr));
  memset(cStatname,0,sizeof(cStatname));
	memset(cBr_no     ,0, sizeof(cBr_no ));    /*机构号  */
	memset(cPrtflag   ,0, sizeof(cPrtflag));  /*打印标志*/
	memset(cSe_type   ,0, sizeof(cSe_type ));/*交易类型*/
	memset(cTx_code   ,0, sizeof(cTx_code)); /*交易代码*/
	memset(&trace_log,0x0,sizeof(struct trace_log_c));
	memset(cFilename  ,0, sizeof(cFilename));  /*文件名*/
	memset(cNote_no,0,sizeof(cNote_no));
	memset(cPrtname   ,0, sizeof(cPrtname));  /*打印标志*/
  memset(cDCname ,0, sizeof(cDCname ));/*借贷标志*/
	memset(cXZname   ,0, sizeof(cXZname ));/*现转标志*/
	memset(cAc_no,0,sizeof(cAc_no));
	memset(&trace_log,0x00,sizeof(struct trace_log_c));

	vtcp_log("sp5923开始！[%s][%d]",__FILE__,__LINE__);
	/**初始变量**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0680",cSe_type);
	get_zd_long("0440",&lTx_bdate);
	get_zd_data("0670",cPrtflag);
	get_zd_data("0490",cTx_code);
	/**开始组成查询条件**/
	vtcp_log("sp5923开始组成查询条件！[%s][%d]",__FILE__,__LINE__);
			if(cPrtflag[0]=='0')/*未打印*/
	    {		 
			   	
			   			vtcp_log("sp5983开始！[%s][%d]",__FILE__,__LINE__);
			   			sprintf(wherelist,"tx_date=%ld and tx_code not in('2201','2202','2203','2204','2410') and opn_br_no='%s' and tx_br_no!='%s' and no_show='0' and tel not like '%%%%%s%%%%'",lTx_bdate,cBr_no,cBr_no,"99");
			}
			if(cPrtflag[0]=='2')/*已打印*/
			{
			   	vtcp_log("sp5983开始！[%s][%d]",__FILE__,__LINE__);
			   	sprintf(wherelist,"tx_date=%ld and tx_code not in('2201','2202','2203','2204','2410') and opn_br_no='%s' and tx_br_no!='%s' and no_show='2' and tel not like '%%%%%s%%%%'",lTx_bdate,cBr_no,cBr_no,"99");
			}	
			if(cPrtflag[0]=='3')/*全部*/
			{
				  vtcp_log("sp5983开始！[%s][%d]",__FILE__,__LINE__);
			   	sprintf(wherelist,"tx_date=%ld and tx_code not in('2201','2202','2203','2204','2410') and opn_br_no='%s' and tx_br_no!='%s' and no_show!='1' and tel not like '%%%%%s%%%%'",lTx_bdate,cBr_no,cBr_no,"99");
			}	
	/**组成查询条件完毕**/
	vtcp_log("sp5983开始查询记录！[%s][%d]",__FILE__,__LINE__);
 /*开始查询*/
 if(lTx_bdate==g_pub_tx.tx_date)
{
 	ret = Trace_log_Dec_Sel(g_pub_tx.reply,wherelist);
	vtcp_log("sp5983开始查询记录！[%s][%d]",__FILE__,__LINE__);

 ttlnum=0;
 while(1)
 {
 			memset(cPrtname   ,0, sizeof(cPrtname)); /*打印标志*/
  		memset(cDCname ,0, sizeof(cDCname ));    /*借贷标志*/
			memset(cXZname   ,0, sizeof(cXZname ));  /*现转标志*/
			memset(cNote_no,0,sizeof(cNote_no));
  		memset(cStatname,0,sizeof(cStatname));
			memset(cAc_no,0,sizeof(cAc_no));
			memset(&trace_log,0x0,sizeof(struct trace_log_c));
 			memset(&dc_log,0x0,sizeof(struct dc_log_c));
 			ret=Trace_log_Fet_Sel(&trace_log,g_pub_tx.reply);
 			vtcp_log("sp5983开始查询记录！[%s][%d]",__FILE__,__LINE__);
 			if( ret==100) 
 			{
 				break;
 			}
 			ERR_DEAL
			if( !ttlnum )
			{
					pub_base_AllDwnFilName(cFilename);
					fp = fopen(cFilename,"w");
					if( fp==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}        
					fprintf( fp,"~流水号码|交易日期|开户机构|交易机构|交易账号|户      名|存转帐号|户      名|交易金额|现转标志|借贷标志|打印标志|摘要备注|凭证号码|状   态|\n" );
			}		
			if(trace_log.no_show[0]=='2')
			{
					memcpy(cPrtname,"已打印", sizeof(cPrtname)-1);
	    }
	    else
	    {
	     		memcpy(cPrtname,"未打印", sizeof(cPrtname)-1);
	    }
		  if(trace_log.add_ind[0]=='0')
			{
					memcpy(cDCname,"贷", sizeof(cDCname)-1);
	    }
	    else
	    {
	     		memcpy(cDCname,"借", sizeof(cDCname)-1);
	    }
	    if(trace_log.ct_ind[0]=='1')
			{
					memcpy(cXZname,"现金", sizeof(cXZname)-1);
	    }
	    else
	    {
	     		memcpy(cXZname,"转帐", sizeof(cXZname)-1);
	    }
	    if(trace_log.sts[0]=='0')
	    {
	    	memcpy(cStatname,"正常",4);
	    }
	    else
	    {
	    	memcpy(cStatname,"撤消",4);
	    }
	    vtcp_log("sp5923开始查询记录！[%s][%d]",__FILE__,__LINE__);
     		memset(cName1,0x0,sizeof(cName1));
     		memset(cName2,0x0,sizeof(cName2));
     		memset(&com_item,0x0,sizeof(struct com_item_c));
     		memset(&in_mst,0x0,sizeof(struct in_mst_c));
				memset(&prdt_ac_id1,0x0,sizeof(prdt_ac_id1));
  			memset(&cif_basic_inf1,0x0,sizeof(cif_basic_inf1));
  			memset(&mdm_ac_rel1,0x0,sizeof(mdm_ac_rel1));
  			memset(&prdt_ac_id2,0x0,sizeof(prdt_ac_id2));
  			memset(&cif_basic_inf2,0x0,sizeof(cif_basic_inf2));
 				memset(&mdm_ac_rel2,0x0,sizeof(mdm_ac_rel2));	
 			if(memcmp(trace_log.tx_code,"2205",4)==0)
	    {
	     		ret=distinct_sel("trace_log","ac_no",cAc_no,"trace_no=%ld and trace_cnt=2 ",trace_log.trace_no);
	     		pub_base_old_acno(trace_log.ac_no);
	     		pub_base_old_acno(cAc_no);
	     		pub_base_strpack(trace_log.ac_no);
	     		pub_base_strpack(cAc_no);
	     		ret1=strlen(trace_log.ac_no);
	     		ret2=strlen(cAc_no);
	     		if(ret1==3||ret1==5||ret1==7)
	     		{
	     			 ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",trace_log.ac_no);
	     		   strcpy(cName1,com_item.acc_name);
	     		}
	     		else
	     		{
	    					
	    					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel1,"ac_no='%s'",trace_log.ac_no);
								if(ret==100)
								{
									
								}
								if (trace_log.ac_no[0]=='9')
	     					{
	     			
	     							ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",mdm_ac_rel1.ac_id);
	     							strcpy(cName1,in_mst.name);
	     					}
	     					else
	     					{
										ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id1,"ac_id=%ld",mdm_ac_rel1.ac_id);
										if(ret==100)
										{
										
										}	
										
										ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf1,"cif_no=%ld",prdt_ac_id1.cif_no);
										if(ret==100)
										{
											
										}
										strcpy(cName1,cif_basic_inf1.name);
							  }
					}
					if(ret2==3||ret2==5||ret2==7)
	     		{
	     			 ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",trace_log.ac_no);
	     		   strcpy(cName2,com_item.acc_name);
	     		}
	     		else
	     		{
							/**得到存取款客户名字**/
							ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel2,"ac_no='%s'",cAc_no);
							if(ret==100)
							{
							
							}
							if (trace_log.ac_no[0]=='9')
	     				{
	     						ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",mdm_ac_rel2.ac_id);
	     						strcpy(cName2,in_mst.name);
	     				}
							else
							{
									ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id2,"ac_id=%ld",mdm_ac_rel2.ac_id);
									if(ret==100)
									{
										
									}	
									ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf2,"cif_no=%ld",prdt_ac_id2.cif_no);
									if(ret==100)
									{
										
									}
						  		strcpy(cName2,cif_basic_inf2.name);
							}
				  }
				   ret=Dc_log_Sel(g_pub_tx.reply,&dc_log,"trace_no=%ld and tx_date=%ld",trace_log.trace_no,trace_log.tx_date);
					 if(ret==100)
					 {
					 		continue;
					 }			  
					fprintf( fp,"%ld|%ld|%s|%s|%s|%s|%s|%s|%.2f|%s|%s|%s|%s|%s|%s|\n",trace_log.trace_no,trace_log.tx_date,trace_log.opn_br_no,trace_log.tx_br_no,trace_log.ac_no,cName1,cAc_no,cName2,trace_log.amt,cDCname,cXZname,cPrtname,trace_log.brf,trace_log.note_no,cStatname);
	     }
	    else
	    {
	    	  ret=distinct_sel("trace_log","ac_no",cAc_no,"trace_no=%ld and trace_cnt=1 ",trace_log.trace_no);
	     		pub_base_old_acno(trace_log.ac_no);
	     		pub_base_old_acno(cAc_no);
	     		pub_base_strpack(trace_log.ac_no);
	     		pub_base_strpack(cAc_no);
	     		ret1=strlen(trace_log.ac_no);
	     		ret2=strlen(cAc_no);
	     		if(ret1==3||ret1==5||ret1==7)
	     		{
	     			 ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",trace_log.ac_no);
	     		   strcpy(cName1,com_item.acc_name);
	     		}
	     		else
	     		{
	     			    		
	    					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel1,"ac_no='%s'",cAc_no);
								if(ret==100)
								{
									
								}
								if (trace_log.ac_no[0]=='9')
	     					{
	     							ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",mdm_ac_rel1.ac_id);
	     							strcpy(cName1,in_mst.name);
	     					}
	     					else
	     					{	
										ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id1,"ac_id=%ld",mdm_ac_rel1.ac_id);
										if(ret==100)
										{
										
										}	
										ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf1,"cif_no=%ld",prdt_ac_id1.cif_no);
										if(ret==100)
										{
											
										}
										strcpy(cName1,cif_basic_inf1.name);
								}
						}
					if(ret2==3||ret2==5||ret2==7)
	     		{
	     			 ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",trace_log.ac_no);
	     		   strcpy(cName2,com_item.acc_name);
	     		}
	     		else
	     		{	
					/**得到存取款客户名字**/
					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel2,"ac_no='%s'",trace_log.ac_no);
					if(ret==100)
					{
					
					}
					if (trace_log.ac_no[0]=='9')
	     		{
	     			ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",mdm_ac_rel2.ac_id);
	     			strcpy(cName2,in_mst.name);
	     		}
	     		else
	     		{
							ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id2,"ac_id=%ld",mdm_ac_rel2.ac_id);
							if(ret==100)
							{
								
							}	
							ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf2,"cif_no=%ld",prdt_ac_id2.cif_no);
							if(ret==100)
							{
				  		
							}
				  		strcpy(cName2,cif_basic_inf2.name);
					}
				}
					 ret=Dc_log_Sel(g_pub_tx.reply,&dc_log,"trace_no=%ld and tx_date=%ld",trace_log.trace_no,trace_log.tx_date);
					 if(ret==100)
					 {
					 		continue;
					 }
					 memset(cNote_no,0,sizeof(cNote_no));
					 ret=distinct_sel("trace_log","note_no",cNote_no,"trace_no=%ld and trace_cnt=1 ",trace_log.trace_no);		
					fprintf( fp,"%ld|%ld|%s|%s|%s|%s|%s|%s|%.2f|%s|%s|%s|%s|%s|%s|\n",trace_log.trace_no,trace_log.tx_date,trace_log.opn_br_no,trace_log.tx_br_no,trace_log.ac_no,cName2,cAc_no,cName1,trace_log.amt,cDCname,cXZname,cPrtname,trace_log.brf,cNote_no,cStatname);
					
	    }
			ttlnum++;
  }
	ret=Trace_log_Clo_Sel();
	ERR_DEAL
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}
}
else
{
	ret = Trace_log_rz_Dec_Sel(g_pub_tx.reply,wherelist);
	vtcp_log("sp5983开始查询记录！[%s][%d]",__FILE__,__LINE__);
 ttlnum=0;
 while(1)
 {
 			memset(cPrtname   ,0, sizeof(cPrtname)); /*打印标志*/
  		memset(cDCname ,0, sizeof(cDCname ));    /*借贷标志*/
			memset(cXZname   ,0, sizeof(cXZname ));  /*现转标志*/
			memset(cAc_no,0,sizeof(cAc_no));
			memset(cNote_no,0,sizeof(cNote_no));
  		memset(cStatname,0,sizeof(cStatname));

			memset(&trace_log_bk,0x0,sizeof(struct trace_log_bk_c));
 			memset(&dc_log_bk,0x0,sizeof(struct dc_log_bk_c));
 			ret=Trace_log_rz_Fet_Sel(&trace_log_bk,g_pub_tx.reply);
 			if( ret==100) 
 			{
 				break;
 			}
 			ERR_DEAL
			if( !ttlnum )
			{
					pub_base_AllDwnFilName(cFilename);
					fp = fopen(cFilename,"w");
					if( fp==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}        
					fprintf( fp,"~流水号码|交易日期|开户机构|交易机构|交易账号|户      名|存转帐号|户      名|交易金额|现转标志|借贷标志|打印标志|摘要备注|凭证号码|状   态|\n" );
			}		
			if(trace_log_bk.no_show[0]=='2')
			{
					memcpy(cPrtname,"已打印", sizeof(cPrtname)-1);
	    }
	    else
	    {
	     		memcpy(cPrtname,"未打印", sizeof(cPrtname)-1);
	    }
		  if(trace_log_bk.add_ind[0]=='0')
			{
					memcpy(cDCname,"贷", sizeof(cDCname)-1);
	    }
	    else
	    {
	     		memcpy(cDCname,"借", sizeof(cDCname)-1);
	    }
	    if(trace_log_bk.ct_ind[0]=='1')
			{
					memcpy(cXZname,"现金", sizeof(cXZname)-1);
	    }
	    else
	    {
	     		memcpy(cXZname,"转帐", sizeof(cXZname)-1);
	    }
	    if(trace_log_bk.sts[0]=='0')
	    {
	    	  memcpy(cStatname,"正常",4);
	    }
	    else
	    {
	    	  memcpy(cStatname,"撤销",4);
	    }
	    vtcp_log("sp5923开始查询记录！[%s][%d]",__FILE__,__LINE__);
     		memset(cName1,0x0,sizeof(cName1));
     		memset(cName2,0x0,sizeof(cName2));
     		memset(&com_item,0x0,sizeof(struct com_item_c));
     		memset(&in_mst,0x0,sizeof(struct in_mst_c));
				memset(&prdt_ac_id1,0x0,sizeof(prdt_ac_id1));
  			memset(&cif_basic_inf1,0x0,sizeof(cif_basic_inf1));
  			memset(&mdm_ac_rel1,0x0,sizeof(mdm_ac_rel1));
 			  memset(&prdt_ac_id2,0x0,sizeof(prdt_ac_id2));
 			  memset(&cif_basic_inf2,0x0,sizeof(cif_basic_inf2));
  			memset(&mdm_ac_rel2,0x0,sizeof(mdm_ac_rel2));
     if(memcmp(trace_log_bk.tx_code,"2205",4)==0)
	    {
	     		ret=distinct_sel("trace_log_rz","ac_no",cAc_no,"trace_no=%ld and trace_cnt=2 and tx_date=%ld ",trace_log_bk.trace_no,trace_log_bk.tx_date);
	     		pub_base_old_acno(trace_log_bk.ac_no);
	     		pub_base_old_acno(cAc_no);
	     		pub_base_strpack(trace_log_bk.ac_no);
	     		pub_base_strpack(cAc_no);
	     		ret1=strlen(trace_log_bk.ac_no);
	     		ret2=strlen(cAc_no);
	     		if(ret1==3||ret1==5||ret1==7)
	     		{
	     			 ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",trace_log_bk.ac_no);
	     		   strcpy(cName1,com_item.acc_name);
	     		}
	     		else
	     		{
	    					
	    					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel1,"ac_no='%s'",trace_log_bk.ac_no);
								if(ret==100)
								{
									
								}
								if (trace_log_bk.ac_no[0]=='9')
	     					{
	     			
	     							ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",mdm_ac_rel1.ac_id);
	     							strcpy(cName1,in_mst.name);
	     					}
	     					else
	     					{
										ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id1,"ac_id=%ld",mdm_ac_rel1.ac_id);
										if(ret==100)
										{
										
										}	
										
										ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf1,"cif_no=%ld",prdt_ac_id1.cif_no);
										if(ret==100)
										{
											
										}
										strcpy(cName1,cif_basic_inf1.name);
							  }
					}
					if(ret2==3||ret2==5||ret2==7)
	     		{
	     			 ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",trace_log_bk.ac_no);
	     		   strcpy(cName2,com_item.acc_name);
	     		}
	     		else
	     		{
							/**得到存取款客户名字**/
							ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel2,"ac_no='%s'",cAc_no);
							if(ret==100)
							{
							
							}
							if (trace_log_bk.ac_no[0]=='9')
	     				{
	     						ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",mdm_ac_rel2.ac_id);
	     						strcpy(cName2,in_mst.name);
	     				}
							else
							{
									ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id2,"ac_id=%ld",mdm_ac_rel2.ac_id);
									if(ret==100)
									{
										
									}	
									ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf2,"cif_no=%ld",prdt_ac_id2.cif_no);
									if(ret==100)
									{
										
									}
						  		strcpy(cName2,cif_basic_inf2.name);
							}
				  }
				   ret=Dc_log_rz_Sel(g_pub_tx.reply,&dc_log_bk,"trace_no=%ld and tx_date=%ld",trace_log_bk.trace_no,trace_log_bk.tx_date);
					 if(ret==100)
					 {
					 		continue;
					 }
					 			  
					fprintf( fp,"%ld|%ld|%s|%s|%s|%s|%s|%s|%.2f|%s|%s|%s|%s|%s|%s|\n",trace_log_bk.trace_no,trace_log_bk.tx_date,trace_log_bk.opn_br_no,trace_log_bk.tx_br_no,trace_log_bk.ac_no,cName1,cAc_no,cName2,trace_log_bk.amt,cDCname,cXZname,cPrtname,trace_log_bk.brf,trace_log_bk.note_no,cStatname);
	     }
	    else
	    {
	    	  ret=distinct_sel("trace_log_rz","ac_no",cAc_no,"trace_no=%ld and trace_cnt=1 and tx_date=%ld ",trace_log_bk.trace_no,trace_log_bk.tx_date);
	     		pub_base_old_acno(trace_log_bk.ac_no);
	     		pub_base_old_acno(cAc_no);
	     		pub_base_strpack(trace_log_bk.ac_no);
	     		pub_base_strpack(cAc_no);
	     		ret1=strlen(trace_log_bk.ac_no);
	     		ret2=strlen(cAc_no);
	     		if(ret1==3||ret1==5||ret1==7)
	     		{
	     			 ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",trace_log_bk.ac_no);
	     		   strcpy(cName1,com_item.acc_name);
	     		}
	     		else
	     		{
	     			    		
	    					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel1,"ac_no='%s'",cAc_no);
								if(ret==100)
								{
									
								}
								if (trace_log_bk.ac_no[0]=='9')
	     					{
	     							ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",mdm_ac_rel1.ac_id);
	     							strcpy(cName1,in_mst.name);
	     					}
	     					else
	     					{	
										ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id1,"ac_id=%ld",mdm_ac_rel1.ac_id);
										if(ret==100)
										{
										
										}	
										ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf1,"cif_no=%ld",prdt_ac_id1.cif_no);
										if(ret==100)
										{
											
										}
										strcpy(cName1,cif_basic_inf1.name);
								}
						}
					if(ret2==3||ret2==5||ret2==7)
	     		{
	     			 ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",trace_log_bk.ac_no);
	     		   strcpy(cName2,com_item.acc_name);
	     		}
	     		else
	     		{	
					/**得到存取款客户名字**/
					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel2,"ac_no='%s'",trace_log_bk.ac_no);
					if(ret==100)
					{
					
					}
					if (trace_log_bk.ac_no[0]=='9')
	     		{
	     			ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld",mdm_ac_rel2.ac_id);
	     			strcpy(cName2,in_mst.name);
	     		}
	     		else
	     		{
							ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id2,"ac_id=%ld",mdm_ac_rel2.ac_id);
							if(ret==100)
							{
								
							}	
							ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf2,"cif_no=%ld",prdt_ac_id2.cif_no);
							if(ret==100)
							{
				  		
							}
				  		strcpy(cName2,cif_basic_inf2.name);
					}
				}
					 ret=Dc_log_rz_Sel(g_pub_tx.reply,&dc_log_bk,"trace_no=%ld and tx_date=%ld",trace_log_bk.trace_no,trace_log_bk.tx_date);
					 if(ret==100)
					 {
					 		continue;
					 }
					 memset(cNote_no,0,sizeof(cNote_no));
					 ret=distinct_sel("trace_log_rz","note_no",cNote_no,"trace_no=%ld and trace_cnt=1 ",trace_log_bk.trace_no);		
		
					fprintf( fp,"%ld|%ld|%s|%s|%s|%s|%s|%s|%.2f|%s|%s|%s|%s|%s|%s|\n",trace_log_bk.trace_no,trace_log_bk.tx_date,trace_log_bk.opn_br_no,trace_log_bk.tx_br_no,trace_log_bk.ac_no,cName2,cAc_no,cName1,trace_log_bk.amt,cDCname,cXZname,cPrtname,trace_log_bk.brf,cNote_no,cStatname);
					
	    }
			ttlnum++;
  }
	ret=Trace_log_rz_Clo_Sel();
	ERR_DEAL
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}
}	

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
}

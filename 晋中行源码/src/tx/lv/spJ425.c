/*************************************************
* 文 件 名:  spJ425.c
* 功能描述： 实时信息业务查询
*
* 作    者:  
* 完成日期： 
*
* 修改记录：
* 日   期:2004.12.7
* 修 改 人:
* 修改内容:非本网点或本网点下属网点帐户信息不能查询
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"
#include "lv_addit_c.h"
#include "lv_add.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"

int spJ425()
{
	vtcp_log("%s,%d开始查询实时信息业务!",__FILE__,__LINE__);
	struct lv_pkgreg_c cLvpkgreg;
	struct lv_addit_c cLvaddit;
	struct mdm_ac_rel_c cMdmacrel;
	struct dd_mst_c cDdmst;
	ZHCX_ADD cZhcx_add;
	ZHCXYD_ADD cZhcxyd_add;
	ZSSQ_ADD_AREA cZssq_add;
	ZSYD_ADD_AREA cZsyd_add;
	QCSQ_ADD_AREA cQcsq_add;
	QCYD_ADD_AREA cQcyd_add;
	FILE *fp;
	memset(&cLvpkgreg,0,sizeof(cLvpkgreg));
	memset(&cLvaddit,0,sizeof(cLvaddit));
	memset(&cMdmacrel,0,sizeof(cMdmacrel));
	memset(&cDdmst,0,sizeof(cDdmst));
	memset(&cZhcx_add,0,sizeof(ZHCX_ADD));
	memset(&cZhcxyd_add,0,sizeof(ZHCXYD_ADD));
	memset(&cZssq_add,0,sizeof(ZSSQ_ADD_AREA));
	memset(&cZsyd_add,0,sizeof(ZSYD_ADD_AREA));
	memset(&cQcsq_add,0,sizeof(QCSQ_ADD_AREA));
	memset(&cQcyd_add,0,sizeof(QCYD_ADD_AREA));
	char cTxbrno[8];
	char cPackid[9];
	char cAcbrno[13];
	char cLw_ind[2];
	char cTxnum[6];
	char tmpstr[512];
	char wherelist[1024];
	char cActno[33];
	char Txnum_name[51];
	char Stat_name[51];
	char Yd_name[51];
	char Lw_ind[51];
	char filename[51];
	int  iWtdate=0,iPackid=0,count=0,ttlnum=0;
	double dTxamt=0.00;
	memset(cTxbrno,0,sizeof(cTxbrno));
	memset(cPackid,0,sizeof(cPackid));
	memset(cAcbrno,0,sizeof(cAcbrno));
	memset(cTxnum,0,sizeof(cTxnum));
	memset(cLw_ind,0,sizeof(cLw_ind));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(wherelist,0,sizeof(wherelist));
	memset(cActno,0,sizeof(cActno));
	memset(Txnum_name,0,sizeof(Txnum_name));
	memset(filename,0,sizeof(filename));
	vtcp_log("%s,%d开始查询实时信息业务!\n",__FILE__,__LINE__);
	get_zd_int("0280",&iPackid);
	sprintf(cPackid,"%08d",iPackid);
	vtcp_log("%s,%d开始查询实时信息业务!packid[%s]\n",__FILE__,__LINE__,cPackid);
	get_zd_data("0910",cTxnum);
	get_zd_data("0520",cAcbrno);
	get_zd_int("0440",&iWtdate);
	vtcp_log("%s,%d开始查询实时信息业务![%d],cAcbrno[%s]\n",__FILE__,__LINE__,iWtdate,cAcbrno);
	get_zd_data("0030",cTxbrno);
	get_zd_data("0720",cLw_ind);
	sprintf(wherelist,"br_no='%s' ",cTxbrno);
	sprintf(tmpstr," and txnum='%s'",cTxnum);
	strcat(wherelist,tmpstr);
	if(iPackid!=0){
			sprintf(tmpstr," and packid='%s'",cPackid);
			strcat(wherelist,tmpstr);
	}
	vtcp_log("%s,%d开始查询实时信息业务!\n",__FILE__,__LINE__);
	if(cAcbrno[0]!='0'){
			sprintf(tmpstr," and ac_br_no='%s'",cAcbrno);
			strcat(wherelist,tmpstr);
	}
	if(iWtdate!=0){
			sprintf(tmpstr," and wt_date=%d",iWtdate);
			strcat(wherelist,tmpstr);
	}
	sprintf(tmpstr," and lw_ind='%s'",cLw_ind);
	strcat(wherelist,tmpstr);
	strcat(wherelist," and 1=1 order by br_no,wt_date");
	vtcp_log("%s,%d开始查询实时信息业务!\n",__FILE__,__LINE__);
	count=sql_count("lv_pkgreg",wherelist);
	if(!count){
			sprintf(acErrMsg,"未找到符合条件的记录!");
			strcpy(g_pub_tx.reply,"S214");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			goto ErrExit;
	}
	int ret=0;
	ret=Lv_pkgreg_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	while(1)
	{
		ret=Lv_pkgreg_Fet_Sel( &cLvpkgreg, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			/**显示列标题**/
			if(memcmp(cTxnum,TXNUM_ZHCX,TXNUM_LEN)==0 || memcmp(cTxnum,TXNUM_CXYD,TXNUM_LEN)==0){
					fprintf( fp,"~业务类型|交易日期|交易序号|接收行行号|查  询  帐  号|应答标示|帐户状态|帐户余额|来往标识|\n" );
			}
		}
		memset(Lw_ind,0,sizeof(Lw_ind));
		memset(Txnum_name,0,sizeof(Txnum_name));
		if(memcmp(cLvpkgreg.txnum,TXNUM_ZHCX,TXNUM_LEN)==0){
					strcpy(Txnum_name,"帐号查询业务");	
		}else if(memcmp(cLvpkgreg.txnum,TXNUM_CXYD,TXNUM_LEN)==0){
					strcpy(Txnum_name,"帐号应答查询业务");
		}else if(memcmp(cLvpkgreg.txnum,TXNUM_ZPQC,TXNUM_LEN)==0){
					strcpy(Txnum_name,"支票圈存业务");
		}else if(memcmp(cLvpkgreg.txnum,TXNUM_ZPYD,TXNUM_LEN)==0){
					strcpy(Txnum_name,"支票圈存应答业务");
		}else if(memcmp(cLvpkgreg.txnum,TXNUM_ZSCX,TXNUM_LEN)==0){
					strcpy(Txnum_name,"证书查询业务");
		}else if(memcmp(cLvpkgreg.txnum,TXNUM_ZSYD,TXNUM_LEN)==0){
					strcpy(Txnum_name,"证书查询应答业务");
		}
		if(cLvpkgreg.lw_ind[0]=='1'){
					strcpy(Lw_ind,"往帐");
		}else if(cLvpkgreg.lw_ind[0]=='2'){
					strcpy(Lw_ind,"来帐");
		}
		if(strlen(cLvpkgreg.addid)==0){
			sprintf(acErrMsg,"位找到附加域序号!");
			strcpy(g_pub_tx.reply,"S089");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("%s,%d开始查询实时信息业务!addid[%s],wt_date[%d]\n",__FILE__,__LINE__,cLvpkgreg.addid,cLvpkgreg.wt_date);
		ret=Lv_addit_Sel(g_pub_tx.reply,&cLvaddit,"tx_date=%d and addid='%s' and tagname='000'",cLvpkgreg.wt_date,cLvpkgreg.addid);
		if(ret){
			sprintf(acErrMsg,"位找到附加域内容!");
			strcpy(g_pub_tx.reply,"S089");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("%s,%d开始查询实时信息业务1!addid[%s],wt_date[%d]\n",__FILE__,__LINE__,cLvpkgreg.addid,cLvpkgreg.wt_date);
		if(memcmp(cLvpkgreg.txnum,TXNUM_ZHCX,TXNUM_LEN)==0){
			int flag=0;
			if(cLvpkgreg.lw_ind[0]=='2'){
				memcpy(&cZhcx_add,cLvaddit.tagdata,sizeof(ZHCX_ADD));
				memcpy(cActno,cZhcx_add.actno,32);
				pub_base_old_acno(cActno);
				pub_base_strpack(cActno);
				memset(&cMdmacrel,0,sizeof(cMdmacrel));
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&cMdmacrel," ac_no='%s' ",cActno);
				if(ret){
					sprintf(acErrMsg,"帐号不存在!");
					WRITEMSG
					ret=0;
					flag=1;
					/*strcpy(g_pub_tx.reply,"S190");
					set_zd_data(DC_GET_MSG,acErrMsg);
					goto ErrExit;*/
				}
				memset(&cDdmst,0,sizeof(cDdmst));
				ret=Dd_mst_Sel(g_pub_tx.reply,&cDdmst," ac_id=%d",cMdmacrel.ac_id);
				if(ret){
					sprintf(acErrMsg,"帐号不存在!");
					WRITEMSG
					ret=0;
					flag=1;
					/*strcpy(g_pub_tx.reply,"S190");
					set_zd_data(DC_GET_MSG,acErrMsg);
					goto ErrExit;*/
				}
				memset(Stat_name,0,sizeof(Stat_name));
				memset(Yd_name,0,sizeof(Yd_name));
				if(cDdmst.hold_sts[0]=='0'){
					strcpy(Stat_name,"帐户正常状态");			
				}else if(cDdmst.hold_sts[0]=='1'|| cDdmst.hold_sts[0]=='3'){
					strcpy(Stat_name,"帐户冻结状态");
				}else{
					strcpy(Stat_name,"帐户其他状态");
				}
				vtcp_log("%s,%d,wangyongwei帐号:[%s],余额:[%f]\n",__FILE__,__LINE__,cMdmacrel.ac_no,cDdmst.bal);
				dTxamt=cDdmst.bal;
				if(flag==1){
					strcpy(Stat_name,"帐户不存在");
				}
			}else if(cLvpkgreg.lw_ind[0]=='1'){
				ret=Lv_addit_Sel(g_pub_tx.reply,&cLvaddit,"tx_date=%d and tagname='000' and addid in(select addid from lv_pkgreg where wt_date=%d and lw_ind='2' and pkgno='013' and txnum='30503') and substr(tagdata,1,16)=%d%s",cLvpkgreg.wt_date,cLvpkgreg.pack_date,cLvpkgreg.pack_date,cLvpkgreg.packid);
				if(ret==100){
					memset(&cLvaddit,0,sizeof(cLvaddit));
					memset(&cZhcx_add,0,sizeof(ZHCX_ADD));
					ret=Lv_addit_Sel(g_pub_tx.reply,&cLvaddit,"tx_date=%d and tagname='000' and addid=%s",cLvpkgreg.wt_date,cLvpkgreg.addid);
					if(ret){
						vtcp_log("未找到附加于错误",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"D001");
						goto ErrExit;
					}
					memcpy(&cZhcx_add,cLvaddit.tagdata,sizeof(ZHCX_ADD));
					memcpy(cActno,cZhcx_add.actno,32);
					strcpy(Stat_name,"未得到应答!");
					dTxamt=0.0;
				}else if(ret==0){
					memcpy(&cZhcxyd_add,cLvaddit.tagdata,sizeof(ZHCXYD_ADD));
					memcpy(cActno,cZhcxyd_add.actno,32);

					memset(Yd_name,0,sizeof(Yd_name));
					if(cZhcxyd_add.resptype=='1'){
						strcpy(Yd_name,"帐户不存在");
					}else if(cZhcxyd_add.resptype=='2'){
						strcpy(Yd_name,"帐户密码错误");
					}else if(cZhcxyd_add.resptype=='3'){
						strcpy(Yd_name,"禁止查询");
					}else if(cZhcxyd_add.resptype=='9'){
						strcpy(Yd_name,"其他应答");
					}else if(cZhcxyd_add.resptype=='0'){
						strcpy(Yd_name,"成功");
					}

					memset(Stat_name,0,sizeof(Stat_name));
					if(memcmp(cZhcxyd_add.actStat,"00",2)==0){
						strcpy(Stat_name,"帐户正常状态");			
					}else if(memcmp(cZhcxyd_add.actStat,"01",2)==0){
						strcpy(Stat_name,"帐户冻结状态");
					}else if(memcmp(cZhcxyd_add.actStat,"99",2)==0){
						strcpy(Stat_name,"帐户其他状态");
					}
					vtcp_log("%s,%d,账号余额为:[%.18s]\n",__FILE__,__LINE__,cZhcxyd_add.otxamt);
					char tmpstr[16];
					memset(tmpstr,0,sizeof(tmpstr));
					memcpy(tmpstr,cZhcxyd_add.otxamt+3,15);
					str2dbl(tmpstr,sizeof(tmpstr)-1,2,&dTxamt);
					vtcp_log("%s,%d,账号余额为:[%.2lf]\n",__FILE__,__LINE__,dTxamt);
				}
				
			}
		}else if(memcmp(cLvpkgreg.txnum,TXNUM_CXYD,TXNUM_LEN)==0){
				memcpy(&cZhcxyd_add,cLvaddit.tagdata,sizeof(ZHCXYD_ADD));
				memset(Stat_name,0,sizeof(Stat_name));
				if(memcmp(cZhcxyd_add.actStat,"00",2)==0){
						strcpy(Stat_name,"帐户正常状态");			
				}else if(memcmp(cZhcxyd_add.actStat,"01",2)==0){
						strcpy(Stat_name,"帐户冻结状态");
				}else if(memcmp(cZhcxyd_add.actStat,"99",2)==0){
						strcpy(Stat_name,"帐户其他状态");
				}
				memset(Yd_name,0,sizeof(Yd_name));
				if(cZhcxyd_add.resptype=='1'){
					strcpy(Yd_name,"帐户不存在");
				}else if(cZhcxyd_add.resptype=='2'){
					strcpy(Yd_name,"帐户密码错误");
				}else if(cZhcxyd_add.resptype=='3'){
					strcpy(Yd_name,"禁止查询");
				}else if(cZhcxyd_add.resptype=='9'){
					strcpy(Yd_name,"其他应答");
				}
				pub_base_strpack(Yd_name);
				vtcp_log("%s,%d,账号余额为:[%.18s]\n",__FILE__,__LINE__,cZhcxyd_add.otxamt);
				char tmpstr[16];
				memset(tmpstr,0,sizeof(tmpstr));
				memcpy(tmpstr,cZhcxyd_add.otxamt+3,15);
				str2dbl(tmpstr,sizeof(tmpstr)-1,2,&dTxamt);
				vtcp_log("%s,%d,账号余额为:[%.2lf]\n",__FILE__,__LINE__,dTxamt);
				memcpy(cActno,cZhcxyd_add.actno,32);
		}
				vtcp_log("%s,%d开始查询实时信息业务2!addid[%s],wt_date[%d],cActno[%s]\n",__FILE__,__LINE__,cLvpkgreg.addid,cLvpkgreg.wt_date,cActno);
		fprintf(fp,"%s|%d|%s|%s|%.24s|%s|%s|%.2lf|%s|\n",Txnum_name,cLvpkgreg.wt_date,cLvpkgreg.orderno,cLvpkgreg.ac_br_no,cActno,Yd_name,Stat_name,dTxamt,Lw_ind);
	 	ttlnum++;
	}
	ret=Lv_pkgreg_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1" );
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

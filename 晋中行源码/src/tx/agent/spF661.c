/*************************************************
* 文 件 名:  spF661.c
* 功能描述： 批量开户打折查询
* 作    者:  ligl
* 完成日期： 2007-1-27 22:26
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
#include "ag_peritemmap_c.h"
spF661()
{
	char cUnitno[5];/*单位编号*/
	long lOpndate=0;/*开户日期*/
	char cPrtflag[2];/*打印标志*/
	char wherelist[1024];  /**查询条件**/
	char tmpstr[512];
	char cPrtname[7];
	char id_type[20];
	char cFilename[100];/*文件名*/
	char sItem_no[21],sItem_no1[21];/*记录个人编号起始和终止号码*/
	int ttlnum=0;	    /**读取的总条数**/
	int ret=0;
	FILE *fp;
	struct ag_peritemmap_c ag_peritemmap;
	memset(wherelist  ,0x00, sizeof(wherelist ));
	memset(cFilename  ,0x00, sizeof(cFilename ));
	memset(cPrtname   ,0x00, sizeof(cPrtname ));
	memset(id_type    ,0x00, sizeof(id_type));
	memset(tmpstr     ,0x00, sizeof(tmpstr ));
	memset(cUnitno     ,0x00, sizeof(cUnitno ));
	memset(cPrtflag   ,0x00, sizeof(cPrtflag));
	memset(&ag_peritemmap,0x00,sizeof(struct ag_peritemmap_c));
	memset(sItem_no    ,0,sizeof(sItem_no));
	memset(sItem_no    ,0,sizeof(sItem_no1));
	pub_base_sysinit();
	get_zd_data("0920",cUnitno);
	get_zd_long("0440",&lOpndate);
	get_zd_data("0670",cPrtflag);
	get_zd_data("0620",sItem_no);
	get_zd_data("0630",sItem_no1);
	pub_base_ltrim_zero(sItem_no);
	pub_base_ltrim_zero(sItem_no1);

	if(lOpndate)
	{
			sprintf(tmpstr,"opn_date=%ld and ",lOpndate);
		  strcat( wherelist,tmpstr);
	}
	else
	{
			sprintf(tmpstr,"opn_date!=%ld and ");
		  strcat( wherelist,tmpstr);
	}


  switch(cPrtflag[0])
  {
      case '1':
			         sprintf(tmpstr,"remark like '%%%%%s%%%%' and cls_tel='0000' and ","1");
		           strcat( wherelist,tmpstr);
               break;
      case '0':
			         sprintf(tmpstr,"remark not like '%%%%%s%%%%' and cls_tel='0000' and ","1");
		  				 strcat( wherelist,tmpstr);
               break;
      case '2':
			         sprintf(tmpstr,"cls_tel='0000' and ");
		  				 strcat( wherelist,tmpstr);			         
               break; 
      default:
               break;   
  }
	sprintf(tmpstr,"unit_no='%s' and ",cUnitno);
    strcat( wherelist,tmpstr);
    /****************添加个人编号查询条件***************/
	if(sItem_no[0]!='\0')
	{
		sprintf(tmpstr," to_number(item_no)>=%d and ",atoi(sItem_no));
		strcat(wherelist,tmpstr);
	}

	if(sItem_no1[0]!='\0')
	{
		sprintf(tmpstr," to_number(item_no)<=%d and ",atoi(sItem_no1));
		strcat(wherelist,tmpstr);
	}

	strcat(wherelist," 1=1 ORDER BY unit_no,to_number(item_no)");
	ret=Ag_peritemmap_Dec_Sel( g_pub_tx.reply,wherelist);
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
		memset(cPrtname   ,0x00, sizeof(cPrtname ));
		memset(id_type    ,0x00, sizeof(id_type));
		ret=Ag_peritemmap_Fet_Sel(&ag_peritemmap, g_pub_tx.reply );
		if( ret==100 ) break;
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
			fprintf( fp,"~单位编号|个人编号|个人账号|凭证号码|凭证种类|用户姓名|证件类型|证件号码|开户日期|操作柜员|打印标志|  |\n" );
		}
		if(ag_peritemmap.remark[8]=='1')
		{
			memcpy(cPrtname,"已打印", sizeof(cPrtname)-1);
	  }
	  else
	  {
	    memcpy(cPrtname,"未打印", sizeof(cPrtname)-1);
	  }
	  pub_base_dic_show_str(id_type,"id_type",ag_peritemmap.id_type);
	  char str_note_type[16];memset(str_note_type,'\0',sizeof(str_note_type));
	  pub_base_dic_show_str(str_note_type,"note_type",g_mdm_ac_rel.mdm_code+1);
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s' ",ag_peritemmap.ac_no);  
	
    fprintf( fp,"%s|%s|%s|%s|%s|%s|%s|%s|%ld|%s|%s|%s|\n",ag_peritemmap.unit_no,ag_peritemmap.item_no,ag_peritemmap.ac_no,g_mdm_ac_rel.note_no,str_note_type,ag_peritemmap.name,id_type,ag_peritemmap.id_no,ag_peritemmap.opn_date,ag_peritemmap.opn_tel,cPrtname,g_mdm_ac_rel.mdm_code);
		ttlnum++;
	}
	ret=Hv_zf_Clo_Sel();
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

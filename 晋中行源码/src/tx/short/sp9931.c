/*************************************************
* 文 件 名:  sp9931.c
* 功能描述： 由卡号回显户名__2103有介定期开户
*
* 作    者:  rob 
* 完成日期： 2003年2月21日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

int sp9931()
{
	int	ret,i=0,vcnt;
	char ac_no[21],filename[300],tmp_name[51],cif_type[3],wherelist[300];
	FILE *fp;

	struct 	prdt_ac_id_c   g_prdt_ac_id;			/*产品账户结构*/
	struct 	mdm_attr_c     g_mdm_attr;			   

	/*清空*/
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	/*前台取值*/
	get_zd_data("1039",ac_no);       				/*帐/卡号*/
	pub_base_old_acno( ac_no );                   /**对旧帐号的处理**/

   	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
   	if( ret==100 )
   	{
        sprintf( acErrMsg,"无此卡号，请检查!ac_no=[%s]", ac_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "M003" );
        goto ErrExit;
   	}
    else if( ret )
   	{
        sprintf( acErrMsg, "读取介质账户对照表异常![%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "W011" );
        goto ErrExit;
    }

   	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'" , \
						g_mdm_ac_rel.mdm_code);
   	if( ret==100 )
   	{
        sprintf( acErrMsg,"无此介质!mdm_code=[%s]",g_mdm_ac_rel.mdm_code );
        WRITEMSG
        strcpy( g_pub_tx.reply, "M003" );
        goto ErrExit;
   	}
    else if( ret )
   	{
        sprintf( acErrMsg, "读取异常![%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "D103" );
        goto ErrExit;
    }

	if( g_mdm_attr.ac_num_ind[0]=='Y' )
   	{
        sprintf( acErrMsg, " 单账户介质不允许办理此业务! ");
        WRITEMSG
        strcpy( g_pub_tx.reply, "D218" );
        goto ErrExit;
    }

	/*根据账户ID找出账户类型*/
   	ret = Prdt_ac_id_Sel( g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
				  		  g_mdm_ac_rel.ac_id);
   	if( ret==100 )
   	{
        sprintf( acErrMsg,"无此卡号，请检查!ac_no=[%ld]", ac_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "M003" );
        goto ErrExit;
   	}
    else if( ret )
   	{
        sprintf( acErrMsg, "读取产品账户对照表异常![%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "W013" );
        goto ErrExit;
    }

	switch(g_prdt_ac_id.ac_id_type[0])
	{
		case '1':							/*1-活期*/
   				ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld", \
								g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"该账户ID不存在!ac_no=[%s]",ac_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "读取活期主文件异常![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W015" );
        			goto ErrExit;
    			}

				strcpy(tmp_name,g_dd_mst.name);

   				ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
								g_prdt_ac_id.prdt_no);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"该产品不存在!prdt_no=[%s]", 
							g_prdt_ac_id.prdt_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
				strcpy(cif_type,g_dd_parm.cif_type);
				break;
		case '2':							/*2-定期*/
   				ret=Td_mst_Sel( g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
								g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"该账户ID不存在!ac_no=[%s]",ac_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "读取定期主文件异常![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W017" );
        			goto ErrExit;
    			}
				strcpy(tmp_name,g_td_mst.name);

   				ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
							"prdt_no='%s'",g_prdt_ac_id.prdt_no);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"该产品不存在!prdt_no=[%s]", 
							g_prdt_ac_id.prdt_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
				strcpy(cif_type,g_td_parm.cif_type);
				break;
		default : 	
        		sprintf( acErrMsg, "该帐号异常!![%d]", ret );
        		WRITEMSG
        		strcpy( g_pub_tx.reply, "O024" );
        		goto ErrExit;
				break;
	}

	/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	if( g_td_parm.td_type[0]=='4' )
	{
	sprintf( wherelist," cif_type in('0','1') and td_type='4'" );
	}
	else
	{
	/***将阳光助学去除,不返回 modify by wanglei 20060917***/
	/********* 增加了除阳光助学，整整八年外的全部定期产品  gyf.20060919
	sprintf( wherelist," cif_type in('0','1') and prdt_no in (select prdt_no from mdm_prdt_rel where mdm_code1='%s' or mdm_code2='%s' or mdm_code3='%s') and ( prdt_no[1,3]<='207' and prdt_no[1,3]>='201' )", g_mdm_ac_rel.mdm_code,g_mdm_ac_rel.mdm_code,g_mdm_ac_rel.mdm_code );
	*********/
		sprintf( wherelist," cif_type in('0','1') and prdt_no in (select prdt_no from mdm_prdt_rel where mdm_code1='%s') and ( prdt_no[1,3] not in ('208','2A4','2A5','2A6','2A7') )", g_mdm_ac_rel.mdm_code,g_mdm_ac_rel.mdm_code,g_mdm_ac_rel.mdm_code );
	}
	
	/*** 生成产品列表 ***/
	ret=Td_parm_Dec_Sel( g_pub_tx.reply," %s order by prdt_no",wherelist );
	if( ret ) goto ErrExit;

	i=0;
	while(1)
	{
		ret = Td_parm_Fet_Sel( &g_td_parm , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( i==0 )
		{
			set_zd_data("1031",g_td_parm.prdt_no);
			set_zd_data("1032",g_td_parm.title);
		}

		pub_base_strpack(g_td_parm.title);
		fprintf( fp,"1031|%-18s|%s|\n",g_td_parm.title,g_td_parm.prdt_no );

		i++;
		vcnt++;
	}
	Mdm_prdt_rel_Clo_Sel();
	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);

	if(cif_type[0]!='1')
  	{
   		sprintf( acErrMsg,"请输入储蓄账号!cif_type=[%s]",cif_type );
      	WRITEMSG
       	strcpy( g_pub_tx.reply, "O164" );
       	goto ErrExit;
   	}

    /* 向前台屏幕赋值 */
	set_zd_data("1038",tmp_name);						/*户名*/
	set_zd_data("1039",ac_no);       					/*帐/卡号*/

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

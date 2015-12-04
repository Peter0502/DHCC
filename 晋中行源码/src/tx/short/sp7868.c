/*************************************************
* 文 件 名:  sp7868.c
* 功能描述： 支付宝银行卡信息的验证
*
* 作    者:  QL
* 完成日期：
* 修改记录：
* 日   期: 2013/7/4 9:15:26
* 修 改 人:
* 修改内容:
*************************************************/
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "mob_acct_type_c.h"

int sp7868()
{
	int iRc=0;
	char cIdNo[21] = {0};
	char sName[60] = {0};
	char sPhone[12]= {0};
	char sChkPhone[2]= {0};
	char sChkName[2]= {0};
	char sChkId[2]= {0};
	
	struct cif_basic_inf_c    cif_basic_inf;
	struct cif_id_code_rel_c  cif_id_code_rel;
	struct cif_email_inf_c    cif_email_inf;
	struct mdm_ac_rel_c       mdmAR;
	struct dd_mst_c      dd_Mst;
	struct mob_acct_type_c  sMob_acct_type;
	     
	memset(&cif_basic_inf,0x00,sizeof(cif_basic_inf));
	memset(&cif_id_code_rel,0x00,sizeof(cif_id_code_rel));
	memset(&cif_email_inf,0x00,sizeof(cif_email_inf));
	memset(&mdmAR,0x00,sizeof(mdmAR) );
	memset(&dd_Mst,0x00,sizeof(dd_Mst));
	memset(&sMob_acct_type, 0x00, sizeof(sMob_acct_type));
	             
    if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		goto ErrExit;
	}
	   
    get_zd_data("0300",mdmAR.ac_no);
    get_zd_data("0250",sName);
    get_zd_data("0620",cIdNo);
    get_zd_data("0380",sPhone);
    
    get_zd_data("0670",sChkId);      /*是否校验证件类型、证件号*/
    get_zd_data("0680",sChkPhone);   /*是否校验手机号*/
    get_zd_data("0690",sChkName);    /*是否校验户名*/
    cif_id_code_rel.id_type[0] = cIdNo[0];      /**证件类型**/
    memcpy (cif_id_code_rel.id_no , cIdNo+1,18);/**证件号码**/
    vtcp_log("证件类型加号码是[%s]\n",cIdNo);
    
    get_zd_data("0300",g_pub_tx.crd_no); /* 卡号 */
	
    /* 检查卡的合法性 
	if (pub_base_CheckCrdNo())
	{
		sprintf(acErrMsg,"检查卡的合法性错误!!");
		WRITEMSG
		goto ErrExit;
	}
	*/
	
    iRc = 0;
    iRc =Mdm_ac_rel_Sel(g_pub_tx.reply,&mdmAR," ac_no='%s'",mdmAR.ac_no);
    if(iRc == 100 || iRc == 1403)
	{
		vtcp_log("账号不存在[%s][%s]",	mdmAR.ac_no);
		sprintf(acErrMsg,"账号不存在[%s]",g_pub_tx.reply);
		strcpy(g_pub_tx.reply,"AP30");
		WRITEMSG
		goto ErrExit;
	}
	
	iRc = 0;
	iRc =Dd_mst_Sel(g_pub_tx.reply,&dd_Mst," ac_id=%ld and ac_seqn=1 ",mdmAR.ac_id);
	if(iRc == 100 || iRc == 1403)
	{
		vtcp_log("账号不存在[%s][%s]",mdmAR.ac_no);
		sprintf(acErrMsg,"账号不存在[%s]",g_pub_tx.reply);
		strcpy(g_pub_tx.reply,"AP30");
		WRITEMSG
		goto ErrExit;
	}
	
	
	if( mdmAR.note_sts[0]=='1' )
	{
		vtcp_log("账号已挂失[%s][%s]",dd_Mst.ac_sts,mdmAR.note_sts);
	 	sprintf(acErrMsg,"账号已挂失[%s][%s]",dd_Mst.ac_sts,mdmAR.note_sts);
	 	strcpy(g_pub_tx.reply,"AP31");
	 	WRITEMSG
	 	goto ErrExit;
	}
	else if(dd_Mst.ac_sts[0]=='*' || mdmAR.note_sts[0]=='*')
	{
		vtcp_log("账号已销户[%s][%s]",dd_Mst.ac_sts,mdmAR.note_sts);
	 	sprintf(acErrMsg,"账号已销户[%s][%s]",dd_Mst.ac_sts,mdmAR.note_sts);
	 	strcpy(g_pub_tx.reply,"AP32");
	 	WRITEMSG
	 	goto ErrExit;
	} 
	else if(dd_Mst.ac_sts[0]!='1' || mdmAR.note_sts[0]!='0')
	{
	 	vtcp_log("账号状态不对[%s][%s]",dd_Mst.ac_sts,mdmAR.note_sts);
	 	sprintf(acErrMsg,"账号状态不对[%s][%s]",dd_Mst.ac_sts,mdmAR.note_sts);
	 	strcpy(g_pub_tx.reply,"AP33");
	 	WRITEMSG
	 	goto ErrExit;
	}
	
	/*1：完全冻结；2：只进不出 */
	if(dd_Mst.hold_sts[0]=='1' || dd_Mst.hold_sts[0]=='2')
	{
	 	vtcp_log("账号已冻结[%s]",dd_Mst.hold_sts);
	 	sprintf(acErrMsg,"账号已冻结[%s]",dd_Mst.hold_sts);
	 	strcpy(g_pub_tx.reply,"AP34");
	 	WRITEMSG
	 	goto ErrExit;
	}

	/* 检查卡的有效性 */
	if ((g_pub_tx.tx_date < mdmAR.beg_date)
	    || (g_pub_tx.tx_date >= mdmAR.end_date))
	{
		sprintf(acErrMsg,"卡的有效期限已过!![%ld] [%ld] [%ld]",g_pub_tx.tx_date,mdmAR.beg_date,mdmAR.end_date);
		WRITEMSG
		strcpy(g_pub_tx.reply,"AP35");
		goto ErrExit;
	}
	
	 /*校验户名*/
	if(sChkName[0]=='Y')
	{
		if(strcmp(dd_Mst.name,sName))
		{
			vtcp_log("户名不相符[%s][%s]", dd_Mst.name,sName);
			sprintf(acErrMsg,"户名不相符[%s]",g_pub_tx.reply);
			strcpy(g_pub_tx.reply,"AP36");
			WRITEMSG
			goto ErrExit;
		}
 	}   
 	cif_id_code_rel.cif_no=dd_Mst.cif_no;
    
    /*校验证件*/
    if(sChkId[0]=='Y')
    {
		if(mdmAR.id_type[0] != cif_id_code_rel.id_type[0])
	    {
	    	sprintf(acErrMsg,"证件类型不符[%s][%s]",mdmAR.id_type,cif_id_code_rel.id_type);
			WRITEMSG
			strcpy(g_pub_tx.reply,"AP37");
			goto ErrExit;
	    }

		if(strncmp(mdmAR.id_no,cif_id_code_rel.id_no,strlen(mdmAR.id_no))!= 0)
	    {
	    	sprintf(acErrMsg,"证件号码不符[%s][%s]",mdmAR.id_no,cif_id_code_rel.id_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"AP38");
			goto ErrExit;
	    }
		
		iRc = 0;
        iRc=Cif_basic_inf_Sel( g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",cif_id_code_rel.cif_no);
    	if(iRc==100 || iRc == 1403)
        {
                sprintf(acErrMsg,"无此客户号，请检查!![%s]",g_pub_tx.reply);
                WRITEMSG
                strcpy(g_pub_tx.reply,"AP38");
                WRITEMSG
                goto ErrExit;
        }
    }
	
	iRc = 0;
	iRc=Cif_basic_inf_Sel( g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",dd_Mst.cif_no);
    if(iRc==100 || iRc == 1403)
	{
		sprintf(acErrMsg,"无此客户号，请检查!![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"AP40");
		WRITEMSG
		goto ErrExit;
	}
       
	/**校验手机号**/
	/**if(sChkPhone[0]=='Y')
	{
	    iRc = 0;
	    iRc= Cif_email_inf_Sel(g_pub_tx.reply,&cif_email_inf,"cif_no=%ld ",dd_Mst.cif_no);
			if(iRc)
			{
				sprintf(acErrMsg,"无此客户号，请检查!![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"AP40");
				WRITEMSG
				goto ErrExit;
			}
			if(strlen(cif_email_inf.email)==0)
			{
				vtcp_log("no phone left[%s]",	sPhone);
				sprintf(acErrMsg,"未留手机号[%s]",g_pub_tx.reply);
				strcpy(g_pub_tx.reply,"AP41");
				WRITEMSG
				goto ErrExit;  			
			}	
	    if(memcmp(cif_email_inf.email,sPhone,strlen(sPhone)))
			{
				vtcp_log("手机号不符[%s]",	sPhone);
				sprintf(acErrMsg,"手机号不符[%s]",g_pub_tx.reply);
				strcpy(g_pub_tx.reply,"AP39");
				WRITEMSG
				goto ErrExit;  			
			}
	}**/
	if(sChkPhone[0]=='Y')
	{
				iRc = 0;
				iRc = Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type," ac_no='%s' and mob_sts<>'3' ",mdmAR.ac_no);
				if( iRc==100 || iRc==1403 )
				{
					vtcp_log("[%s],[%d] 此账户未做过短信签约或已解约!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"AP41");
					set_zd_data("0130","此账户未做过短信签约或已解约!");
					goto ErrExit;	
				}
				else if( iRc )
				{
					vtcp_log("[%s][%d]查询表Mob_acct_type_Sel错误!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0130","查询短信通知签约表报错!");	
					goto ErrExit;
				}
				else
				{
					if ( strlen(sMob_acct_type.mobile) > 0)
					{
				    if( memcmp(sMob_acct_type.mobile,sPhone,strlen(sPhone)) )
						{
							vtcp_log("[%s],[%d],手机号不符!传来手机号[%s],签约手机号[%s]",__FILE__,__LINE__,sPhone,sMob_acct_type.mobile);
							strcpy(g_pub_tx.reply,"AP39");
							goto ErrExit;
						}
					}
					else
					{
						vtcp_log("[%s],[%d]账户签约手机号为空[%s]!",__FILE__,__LINE__,strlen(sMob_acct_type.mobile));      
						strcpy(g_pub_tx.reply,"AP41");
						set_zd_data("0130","账户签约手机号为空,请联系科技部!");
						goto ErrExit;	
					}
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

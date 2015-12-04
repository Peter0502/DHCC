/***************************************************************
* �� �� ��:  sp8320.c
* ��������:  ���ӹ���ǩԼ��ѯ	
* ��    ��:  lq

* ������ڣ�    
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_qzgl_c.h"
#include "mo_qzgl_hst_c.h"

int sp8320()
{   	
    int			iRet=0;
    long		child_ac_id=0;
    long		part_ac_id=0;
    long		sel_ac_id=0;
    long		tmp_ac_id=0;
    
    char    child_ac_no[25];
    char    part_ac_no[25];
    char		tmp_ac_no[25];
    char    cFlag[2];
    
    struct    mo_qzgl_c				sMo_qzgl;
    struct    mdm_ac_rel_c		sMdm_ac_rel_1;
    struct    mdm_ac_rel_c		sMdm_ac_rel_2;
    
    memset(child_ac_no, 0x00, sizeof(child_ac_no));
    memset(part_ac_no,  0x00, sizeof(part_ac_no));
    memset(tmp_ac_no,  0x00, sizeof(tmp_ac_no));
    memset(cFlag,   0x00, sizeof(cFlag));
    memset(&sMo_qzgl,     0x00, sizeof(sMo_qzgl));
    memset(&sMdm_ac_rel_1, 0x00, sizeof(sMdm_ac_rel_1));
    memset(&sMdm_ac_rel_2, 0x00, sizeof(sMdm_ac_rel_2));
    
    pub_base_sysinit();
    get_zd_data("0300", child_ac_no);
    get_zd_data("0310", part_ac_no);
    get_zd_data("0680", cFlag);
    pub_base_strpack(child_ac_no);
    pub_base_strpack(part_ac_no);
    pub_base_old_acno(child_ac_no);	/**�Ծ��ʺŵĴ���**/
    pub_base_old_acno(part_ac_no);
    
    if(cFlag[0]=='1')
    {
	    /*���ݶ�ͯ���˺Ŵӱ�mdm_ac_rel�л�ȡ��ͯ��ac_id*/
	    iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_1, "ac_no='%s'", child_ac_no);
	    if(iRet == 100)
	    {
	      vtcp_log("[%s][%d]�޴˿��ţ����飡��ac_no=[%s]", __FILE__, __LINE__, child_ac_no);
	    	strcpy(g_pub_tx.reply, "M003");
	    	goto ErrExit;
	    }
	    else if(iRet)
	    {
	    	vtcp_log("[%s][%d]��iRet=[%d]", __FILE__, __LINE__, iRet);
	    	strcpy(g_pub_tx.reply, "W011");
	    	goto ErrExit;	
	    }
	    child_ac_id=sMdm_ac_rel_1.ac_id;
	
	    
	    /*���ݸ�ĸ���˺Ŵӱ�mdm_ac_rel�л�ȡ��ĸ��ac_id*/
	    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_2, "ac_no='%s'", part_ac_no);
	    if(iRet == 100)
	    {
	      vtcp_log("[%s][%d]�޴˿��ţ����飡��ac_no=[%s]", __FILE__, __LINE__, part_ac_no);
	    	strcpy(g_pub_tx.reply, "M003");
	    	goto ErrExit;
	    }
	    else if(iRet)
	    {
	    	vtcp_log("[%s][%d]��iRet=[%d]", __FILE__, __LINE__, iRet);
	    	strcpy(g_pub_tx.reply, "W011");
	    	goto ErrExit;	
	    }
	    part_ac_id=sMdm_ac_rel_2.ac_id;
	    
	    iRet = Mo_qzgl_Sel(g_pub_tx.reply, &sMo_qzgl, "child_ac_id=%ld and part_ac_id=%ld and sts='1'order by tx_date", child_ac_id,part_ac_id);
	    vtcp_log("%d,sMo_qzgl.sts=[%s]", iRet, sMo_qzgl.sts);
	    if(iRet != 100 && iRet != 0)
	    {
		    strcpy(g_pub_tx.reply, "D101");
		    sprintf( acErrMsg, "Mo_qzgl_Dec_Upd����!!!" );
		    WRITEMSG
		    goto ErrExit;
	    }
	    else if(iRet == 100)
	    {
	    	int ret = 0;
	    	set_zd_data("0690", "0"); /**δǩԼ**/
	    	
	    	/**�ҳ���������������̫����**/
	    	ret = sql_count("mo_qzgl","part_ac_id=%ld and sts='1'",part_ac_id);
		    if(ret == 3)
		    {
		    	set_zd_data("0690", "2");
		    }
		  	
		  	/**��̫����ֻ�ܹ���һ���ҳ���**/
		    ret = sql_count("mo_qzgl","child_ac_id=%ld and sts='1'",child_ac_id);
		    if(ret > 0)
		    {
		    	set_zd_data("0690", "3");
		    }

	    }
	    else
	    {
	    	
		  	set_zd_data("0690", "1");
	    }
	  }
	  else if(cFlag[0]=='2')
	  {
	  	/*���ݿ��˺Ŵӱ�mdm_ac_rel�л�ȡ��ac_id*/
	    iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_1, "ac_no='%s'", child_ac_no);
	    if(iRet == 100)
	    {
	      vtcp_log("[%s][%d]�޴˿��ţ����飡��ac_no=[%s]", __FILE__, __LINE__, child_ac_no);
	    	strcpy(g_pub_tx.reply, "M003");
	    	goto ErrExit;
	    }
	    else if(iRet)
	    {
	    	vtcp_log("[%s][%d]��iRet=[%d]", __FILE__, __LINE__, iRet);
	    	strcpy(g_pub_tx.reply, "W011");
	    	goto ErrExit;	
	    }
	    sel_ac_id=sMdm_ac_rel_1.ac_id;
	    
	    iRet = Mo_qzgl_Sel(g_pub_tx.reply, &sMo_qzgl, "child_ac_id=%ld or part_ac_id=%ld and sts='1'order by tx_date", sel_ac_id,sel_ac_id);
	    vtcp_log("%d,sMo_qzgl.sts=[%s]", iRet, sMo_qzgl.sts);
	    if(iRet != 100 && iRet != 0)
	    {
		    strcpy(g_pub_tx.reply, "D101");
		    sprintf( acErrMsg, "Mo_qzgl_Dec_Upd����!!!" );
		    WRITEMSG
		    goto ErrExit;
	    }
	    if(iRet == 100)
	    {
	    	iRet = Mo_qzgl_Sel(g_pub_tx.reply, &sMo_qzgl, "child_ac_id=%ld or part_ac_id=%ld and sts='*'order by tx_date", sel_ac_id,sel_ac_id);
	      vtcp_log("%d,sMo_qzgl.sts=[%s]", iRet, sMo_qzgl.sts);
	      if(iRet != 100 && iRet != 0)
	      {
		    strcpy(g_pub_tx.reply, "D101");
		    sprintf( acErrMsg, "Mo_qzgl_Dec_Upd����!!!" );
		    WRITEMSG
		    goto ErrExit;
	      }
	      if (iRet==0){
	    			set_zd_data("0690", "4");  /*�ѽ�Լ*/
	    	}
	    	 if(iRet == 100){
	    	 set_zd_data("0690", "0"); /**δǩԼ**/
	       }
	    }
	  }
	    else
	    {
	    	/**����ǩԼ����ѯ������˺ţ�6223����Ҫ���ش˰��˺�**/
	    	vtcp_log("%d, sel_ac_id=[%ld],child_ac_id=[%ld],part_ac_id=[%ld]", iRet, sel_ac_id,sMo_qzgl.child_ac_id,sMo_qzgl.part_ac_id);
	    	if(sel_ac_id==sMo_qzgl.child_ac_id)
	    		tmp_ac_id = sMo_qzgl.part_ac_id;
	    	else if(sel_ac_id==sMo_qzgl.part_ac_id)
	    		tmp_ac_id = sMo_qzgl.child_ac_id;
	    	
	    	iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_2, "ac_id=%ld ", tmp_ac_id);
		    if(iRet == 100)
		    {
		      vtcp_log("[%s][%d]�޴˿��˻�id�����飡��ac_id=[%ld]", __FILE__, __LINE__, tmp_ac_id);
		    	strcpy(g_pub_tx.reply, "M003");
		    	goto ErrExit;
		    }
		    else if(iRet)
		    {
		    	vtcp_log("[%s][%d]��iRet=[%d]", __FILE__, __LINE__, iRet);
		    	strcpy(g_pub_tx.reply, "W011");
		    	goto ErrExit;	
		    }
		    strcpy(tmp_ac_no,sMdm_ac_rel_2.ac_no);
		    
		  	set_zd_data("0690", "1");			/**����ǩԼ��־**/
		  	set_zd_long("0440", sMo_qzgl.tx_date);		/**6223�з���ǩԼ����**/
		  	set_zd_long("0450", sMo_qzgl.untx_date);	/**6223�з��ؽ�Լ����**/
		  	set_zd_data("0330", tmp_ac_no);
			set_zd_double("0390", sMo_qzgl.qy_amt); /**6223�з���ǩԼ���**/   
	    }


OkExit:
  strcpy(g_pub_tx.reply, "0000");
  sprintf(acErrMsg, "success!!");
  WRITEMSG
  set_zd_data(DC_REPLY, g_pub_tx.reply);
  return 0;
    
ErrExit:
  sprintf(acErrMsg, "failed !! reply is[%s]", g_pub_tx.reply);
  WRITEMSG
  if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply, "0000") == 0)
  {
  	strcpy(g_pub_tx.reply, "S035");
  }
  set_zd_data(DC_REPLY, g_pub_tx.reply);
  return 1;
}

/*************************************************
* �� �� ��: sp8891.c
* �����������ͻ�����֪ͨ��Լ����
* ��    ��: houyi
* ������ڣ�
* �޸ļ�¼��.....................................................
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
* 2. 
*�������ã�
insert into tx_def values('8891','����֪ͨҵ��','10000000000000010000000010000000000010000000000100000000000000000000111100000000000001000000000000000000000000000000000000000100','1','4','0','3','');

insert into tx_sub_def values('8891','����֪ͨҵ��','sp8891','1','0');

insert into tx_flow_def values ('8891', 0, '8891', '#$');

drop  table mob_sys_parm;
create table mob_sys_parm(
	jmbz varchar2(2),  
	jmsj varchar2(2) 	 
)tablespace users;
insert into mob_sys_parm values('0','3');

insert into rsp_code values('F035','�Թ������ɰ�4���ֻ�����','');
insert into rsp_code values('F036','��˽�����ɰ�1���ֻ�����','');
insert into rsp_code values('F037','����֪ͨ�ѿ�ͨ,���ȹر�','');
insert into rsp_code values('F038','���˻���ǩԼ','');
insert into rsp_code values('F039','�ǻ��ڴ���˻�������ǩԼ','');
�޸Ľ��׶����Ľ�������Ϊ6���������ֲ���ʾ��Ȩ������
�޸ķ������а����ֻ����ֶ�
��˶��޸�������������81��  �����ֶ�
update tx_def set tx_type='6',bit_map='10000000000000010000000010100000000010000000000100000000000000000000111100000000100001000000000000000000000000000000000000000100' where tx_code='8891';
mob_acct_type����ǩԼ״̬(mob_sts)˵��: 1��ǩԼ 2����ͣ 3����Լ

*************************************************/
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "ln_mst_c.h"
#include "prdt_ac_id_c.h"
#include "mob_acct_type_c.h"
#include "mob_sendmail_c.h"
#include "mob_server_type_c.h"
#include "mob_sys_parm_c.h"
/*�绰��ǩԼ����У���ѯ*/
#include "mdm_ac_rel_c.h"
/*#include "cr_credityear_c.h"
#include "cif_auth_reg_c.h"	*//*add by liaohx 2010-9-18 15:38:04*/	 

sp8891()
{
		
    int  iLen;
		int  ret=-1;
    char cOpt[2];/**��������0��ͨ1����2�ر�**/
    char flag[2];
    char cAcno[21];
    char cActype[2];
    char cPhones[12];
    char cTmpPhone[13];
    char cTmpBuff_old[61];
    char cTmpBuff_new[61];
    char cTmpBuff_del[61];
    char cServertype[11];
    char cSfbz[2];
    char cOperation[10];
    char cMessage[141];
    char cAcno_temp[5];
    char cLn_type[2];
    char auth_flag[2];/*add by liaohx 2010-9-18 15:38:04*/
    char kfacno[21];
    char cDkbz[5]; /*add by lwb 20150325 ���Ӵ���ǩԼ��־*/
    char cDkbz_old[5];/*add by lwb 20150325 ���Ӵ���ǩԼ��־*/
    
    /*struct cif_auth_reg_c sCif_auth_reg_c;add by liaohx 2010-9-18 15:38:04*/
    struct mob_acct_type_c sMob ,sMob_acct_type,sMbank1,stype;
    struct mob_sendmail_c  sMail;
    struct ln_mst_c sLn;
    struct dd_mst_c sDd;
    struct mob_server_type_c sMob_server_type;
    struct mob_sys_parm_c sMobSysParm;
    /*�绰��ǩԼ����У���ѯ ����*/
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct mdm_ac_rel_c sMdmm;
    char cId_no[20];
    char cQDbz[2];
    memset(kfacno,0x00,sizeof(kfacno));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(cId_no,0x00,sizeof(cId_no));
    memset(cQDbz,0x00,sizeof(cQDbz));
    memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
    memset(&sMobSysParm,0x00,sizeof(sMobSysParm));
    memset(&sLn,0x00,sizeof(sLn));
    memset(&sDd,0x00,sizeof(sDd));
    memset(&sMbank1,0x00,sizeof(sMbank1));
    memset(cDkbz,0x00,sizeof(cDkbz));
    memset(cDkbz_old,0x00,sizeof(cDkbz_old));
    /*�绰�˶������*/
    memset(&sMob,0x00,sizeof(sMob));
    memset(&sMail,0x00,sizeof(sMail));
    memset(cOpt,0x00,sizeof(cOpt));
    memset(flag,0x00,sizeof(flag));
    memset(cAcno,0x00,sizeof(cAcno));
    memset(cPhones , 0 , sizeof(cPhones));
    memset(cTmpBuff_old , 0 , sizeof(cTmpBuff_old));
    memset(cTmpBuff_new , 0 , sizeof(cTmpBuff_new));
    memset(cTmpBuff_del , 0 , sizeof(cTmpBuff_del));
    memset(cActype,0x00,sizeof(cActype));
    memset(cServertype,0x00,sizeof(cServertype));
    memset(cSfbz,0x00,sizeof(cSfbz));
    memset(cLn_type,0x00,sizeof(cLn_type));
    memset(cOperation,0x00,sizeof(cOperation));
    memset(cMessage,0x00,sizeof(cMessage));
    memset(cAcno_temp,0x00,sizeof(cAcno_temp));
    memset(&sMdmm,0x00,sizeof(sMdmm));
    memset(auth_flag,0x00,sizeof(auth_flag));/*add by liaohx 2010-9-18 15:38:04*/
       
    pub_base_sysinit();
    vtcp_log("[%s][%d] ��ʼд��ˮ! sub_tx_code=[%s]",__FILE__,__LINE__,g_pub_tx.sub_tx_code);
    get_zd_data("0370",cAcno);
    
		if(cAcno[0]>='0' && cAcno[0]<='9'){
			pub_base_old_acno(cAcno);
		}
		
		ret= Mob_sys_parm_Sel(g_pub_tx.reply,&sMobSysParm,"1=1");
		if(ret)
		{
			vtcp_log("[%s][%d]�õ�������������![%d]\n",__FILE__,__LINE__,ret);
			goto ErrExit;
		}
		strcpy(g_pub_tx.tmp_ind,sMobSysParm.jmsj);
		
    vtcp_log("[%s][%d] ��������jmsj=[%s]",__FILE__,__LINE__,g_pub_tx.tmp_ind);
    get_zd_data("0720",cOpt);
    get_zd_data("0710",cActype);
    get_zd_data("0860",cServertype);
    get_zd_data("0690",cSfbz);
    get_zd_data("0700",cLn_type);
    get_zd_data("0660",auth_flag);/*add by liaohx 2010-9-18 15:38:04*/
    get_zd_data("0620",g_pub_tx.id_no);/*add by liaohx 2010-9-18 15:38:04*/
    get_zd_data("0680",g_pub_tx.ct_ind);/*add by liaohx 2010-9-18 15:38:04*/
    get_zd_double("0400",&g_pub_tx.tx_amt1);/*add by xujian 2010-12-30 17:48 ���׽���Լ��*/
    get_zd_data("0310",kfacno); /*�۷��˻�*/

    get_zd_data("0270",cTmpBuff_old);
    get_zd_data("0250",cTmpBuff_new);
 		get_zd_data("0260",cTmpBuff_del);
 		get_zd_data("0320",cDkbz);
 		get_zd_data("0330",cDkbz_old); /*add by lwb 20150325 ����ǩԼ��־*/
 		
 		/*�绰�˻�ȡ��֤begin*/
    get_zd_data("0300",cId_no);
    get_zd_data("0980",cQDbz);
		
		if(0 == strcmp(cQDbz,"1"))
		{
			if(cOpt[0]!='1')
			{
				ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s' and id_no = '%s'",cAcno,cId_no);
				if (ret)
	      {
	         vtcp_log("%s,%d,��֤�û���Ϣʧ��!",__FILE__,__LINE__);
	         strcpy(g_pub_tx.reply,"CU14");
	         goto ErrExit;
	      }
	      set_zd_data("0310",cAcno);
	    }
      /*
      if(cOpt[0]=='1')
      {
      	ret = Mob_acct_type_Sel(g_pub_tx.reply,&stype,"ac_no = '%s' and mobile = '%s'",cAcno,cTmpBuff_old);
      	if(ret)
      	{
      		vtcp_log("%s,%d,ԭǩԼ�ֻ����������!",__FILE__,__LINE__);
         	strcpy(g_pub_tx.reply,"CU15");
         	goto ErrExit;
      	}
      }
      else if(cOpt[0]=='2')
      {
      	ret = Mob_acct_type_Sel(g_pub_tx.reply,&stype,"ac_no = '%s' and mobile = '%s'",cAcno,cTmpBuff_del);
      	if(ret)
      	{
      		vtcp_log("%s,%d,��Լ�ֻ������������!",__FILE__,__LINE__);
         	strcpy(g_pub_tx.reply,"CU15");
         	goto ErrExit;
      	}
      }
      */
      if(cOpt[0]=='1')
      {
      	ret = Mob_acct_type_Sel(g_pub_tx.reply,&stype,"ac_no = '%s'",cAcno);
      	if(ret)
      	{
      		vtcp_log("%s,%d,���˻�δǩԼ!",__FILE__,__LINE__);
         	strcpy(g_pub_tx.reply,"F038");
         	goto ErrExit;
      	}
      	if(0 != strncmp(cTmpBuff_old,stype.mobile,11))
      	{
      		vtcp_log("%s,%d,ԭǩԼ�ֻ����������!",__FILE__,__LINE__);
      		/*likd������������81���ֻ������ֶ�*/
					set_zd_data("0810",stype.mobile);
         	strcpy(g_pub_tx.reply,"CU15");
         	goto ErrExit;
      	}
      }
      else if(cOpt[0]=='2')
      {
      	ret = Mob_acct_type_Sel(g_pub_tx.reply,&stype,"ac_no = '%s'",cAcno);
      	if(ret)
      	{
      		vtcp_log("%s,%d,���˻�δǩԼ!",__FILE__,__LINE__);
         	strcpy(g_pub_tx.reply,"F038");
         	goto ErrExit;
      	}
      	if(0 != strncmp(cTmpBuff_del,stype.mobile,11))
      	{
      		vtcp_log("%s,%d,��Լ�ֻ������������!",__FILE__,__LINE__);
         	strcpy(g_pub_tx.reply,"CU15");
         	goto ErrExit;
      	}
      }
		}
		/*�绰�˻�ȡend*/
 		
    zip_space(cTmpBuff_old);
    zip_space(cTmpBuff_new);
    zip_space(cTmpBuff_del);
    vtcp_log("[%s][%d] g_pub_tx.ct_ind=[%s]---cSfbz=[%s]---cServertype[%s]",__FILE__,__LINE__,g_pub_tx.ct_ind,cSfbz,cServertype);
    vtcp_log("[%s][%d] cTmpBuff_old=[%s]---cTmpBuff_new=[%s]",__FILE__,__LINE__,cTmpBuff_old,cTmpBuff_new);
 		vtcp_log("[%s][%d] cTmpBuff_del=[%s]",__FILE__,__LINE__,cTmpBuff_del);
 		vtcp_log("[%s][%d] cDkbz_old=[%s]",__FILE__,__LINE__,cDkbz_old);
 		vtcp_log("[%s][%d] cDkbz=[%s]",__FILE__,__LINE__,cDkbz);

    iLen=strlen(cAcno);
    
    if(cOpt[0]=='0')
        strcpy(cOperation,"��ͨ");
    else if(cOpt[0]=='1')
        strcpy(cOperation,"����");
    else
    { 
        strcpy(cOperation,"�ر�");
        memset(cTmpBuff_old , 0 , sizeof(cTmpBuff_old));
        memcpy(cTmpBuff_old,cTmpBuff_del,sizeof(cTmpBuff_old)-1);
        zip_space(cTmpBuff_old);
    }
    cAcno_temp[0]=cAcno[iLen-4];
    cAcno_temp[1]=cAcno[iLen-3];
    cAcno_temp[2]=cAcno[iLen-2];
    cAcno_temp[3]=cAcno[iLen-1];
    zip_space(cServertype);
    vtcp_log("rrrrrrrrrrrrrrrrrrrrrrrrr[%s]",cActype);
    memcpy(sMob.ac_no,cAcno,sizeof(sMob.ac_no)-1);
    memcpy(sMob.ac_type,cActype,sizeof(sMob.ac_type)-1);
    memcpy(sMob.br_no,g_pub_tx.tx_br_no,BRNO_LEN);
    vtcp_log("ttttttttttttttttttttttttt[%s]",sMob.ac_type);
    
    memcpy(sMob.server_type,cServertype,sizeof(sMob.server_type)-1);
    memcpy(sMob.sfbz,cSfbz,sizeof(sMob.sfbz)-1);
    
    memcpy(sMob.tel,g_pub_tx.tel,sizeof(sMob.tel)-1);
    sMob.tx_date=g_pub_tx.tx_date;
    
    /**3Ϊ�����1Ϊ���ڻ�***/
		if(sMob.ac_type[0]=='3')
		{
			g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&sLn,"pact_no like '%%%s%%' ",cAcno);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Ln_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D104");
	        goto ErrExit;
	    }
	    g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdmm,"ac_id= %ld ",sLn.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Mdm_ac_rel_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D104");
	        goto ErrExit;
	    }
		}
		else
		{
			g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdmm,"ac_no='%s'",cAcno);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Mdm_ac_rel_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D104");
	        goto ErrExit;
	    }
	    /***���ڻ��ſɿ�ͨ�������ѷ���****/	    
	  	g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&sDd,"ac_id=%ld ",sMdmm.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Dd_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
	  }

	  /**�Թ���˽��ǩԼ�����˻���������ʹ��ǩԼҳ��*
 		if(cOpt[0]=='0')
		{
  		g_reply_int=Mob_acct_type_Sel(g_pub_tx.reply,&sMbank1,"ac_no='%s'",cAcno);
	    if(g_reply_int==0)
	    {
	        vtcp_log("[%s][%d]�Ѿ�ǩԼ[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F038");
	        goto ErrExit;
	    }
	  }
	  */
		int new_len;
		int old_len;
		int bb = 0;
		int cc = 0;
		int dd = 0;
		char tmp[12] = {0};
		char tmp1[5] = {0};
		new_len = strlen(cTmpBuff_new);
		old_len = strlen(cTmpBuff_old);
    /**�Թ������Ƕ���ֻ��ţ��������***/ 
    vtcp_log("[aaaaaaaaaaaa[%c],[%c],[%c]",sMob.ac_type[0],cAcno[0],cOpt[0]);  
  	if(sMob.ac_type[0]=='1' && cAcno[0]=='5')/**���ڻ�**/
  	{
	    while(old_len >= 11 || new_len >= 11)
    	{	
    		memset(cPhones , 0 , sizeof(cPhones));
    		memset(flag , 0 , sizeof(flag));
    		memcpy(flag ,cTmpBuff_old+cc,1);
    		memcpy(sMob.mobile ,cTmpBuff_new+bb,sizeof(cPhones)-1);	
    		strncpy(sMob.filler,cDkbz+dd,1);
    		
    		
        sprintf(cMessage,"��λ%s�˻���%d��%d��%dʱ%d�ֶ���ǩԼ%s��"\
            ,cAcno_temp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,cOperation);
        if(cOpt[0]=='1')
        {
        	strncpy(tmp,cTmpBuff_old+cc+1,11); 
        	strncpy(tmp1,cDkbz_old+dd,1);
        	vtcp_log("[vvvvvvvvvvvvvvvvvvv==[%c]---[%s]",flag[0],tmp); 
        	if(flag[0] == '1') /**���ʱ���Ӳ���**/
        	{
        		sprintf(cMessage,"��λ%s�˻���%d��%d��%dʱ%d�ֶ���ǩԼ��ͨ��"\
            ,cAcno_temp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,cOperation);
			      g_reply_int=proc_dd(sMob,"0",sMob.mobile,cMessage,g_pub_tx.tx_amt1);
			      if(g_reply_int )
				    {
				        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
				        goto ErrExit;
				    }
        	} 
        	else if(flag[0] == '3') /**���ʱɾ������**/
        	{
        		sprintf(cMessage,"��λ%s�˻���%d��%d��%dʱ%d�ֶ���ǩԼ�رա�"\
            ,cAcno_temp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,cOperation);
        		g_reply_int=proc_dd_del(sMob,"2",tmp,cMessage);	
        		if(g_reply_int )
				    {
				        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
				        goto ErrExit;
				    }
        	} 
        	else
        	{ 
						g_reply_int=proc_dd_upd(sMob,cOpt,tmp,tmp1,cMessage);	
						if(g_reply_int )
				    {
				        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
				        goto ErrExit;
				    }
        	}
        }
        else if(cOpt[0]=='2')
        {
        	strncpy(tmp,cTmpBuff_old+bb,11);       	
        	g_reply_int=proc_dd_del(sMob,cOpt,tmp,cMessage);	
        	if(g_reply_int )
			    {
			        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
			        goto ErrExit;
			    }
        }
        else
        {
        	strncpy(tmp,cTmpBuff_old,11);
        	g_reply_int=proc_dd(sMob,cOpt,tmp,cMessage,g_pub_tx.tx_amt1);
        	if(g_reply_int )
			    {
			        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
			        goto ErrExit;
			    }
        }
        new_len -= 11;
        old_len -= 11;
        bb+=11;
        cc+=12;
        dd+=1;
        
      }

    }/***���˻��ڻ���һ��ִֻ��һ�β������**/
    else if(sMob.ac_type[0]=='1' && cAcno[0]!='5')/**���ڻ�**/
    {
    	memcpy(sMob.mobile,cTmpBuff_new,11);
    	memcpy(tmp,cTmpBuff_old,11);
    	memcpy(sMob.filler,cDkbz,1);
	    if(memcmp(cAcno ,"621223" , 6)==0 && strlen(cAcno)==19 )
	    {
	    	sprintf(cMessage,"��β��%s��%d��%d��%02d:%02d���ж���ǩԼ%s��",cAcno_temp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,cOperation);
	    }
	    else if(memcmp(cAcno ,"621780" , 6)==0 && strlen(cAcno)==19)
	    {
	    	sprintf(cMessage,"��β��%s��%d��%d��%02d:%02d���ж���ǩԼ%s��",cAcno_temp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,cOperation);
	    }
	    else
	    {
	    	sprintf(cMessage,"����%s��%d��%d��%dʱ%d�ֽ��ж���ǩԼ%s���ɵ�4006535666",cAcno_temp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,cOperation);
	    }
	    g_reply_int=proc_dd(sMob,cOpt,tmp,cMessage,g_pub_tx.tx_amt1);
      if(g_reply_int )
	    {
	        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
	        goto ErrExit;
	    }
    }   
    else /***�������ͬ�Ž���ǩԼ**/
    {
    	while(old_len >= 11 || new_len >= 11)
    	{	
    		memset(cPhones , 0 , sizeof(cPhones));
    		memcpy(sMob.mobile ,cTmpBuff_new+bb,sizeof(cPhones)-1);	
    		memset(flag , 0 , sizeof(flag));
    		memcpy(flag ,cTmpBuff_old+cc,1);
        sprintf(cMessage,"��λ%s�����ͬ��%d��%d��%dʱ%d�ֶ���ǩԼ%s��"\
        ,cAcno_temp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,cOperation);
				if(cOpt[0]=='1')
        {
        	strncpy(tmp,cTmpBuff_old+cc+1,11); 
        	strncpy(tmp1,cDkbz_old+dd,1);
        	vtcp_log("[vvvvvvvvvvvvvvvvvvv==[%c]---[%s]",flag[0],tmp); 
        	if(flag[0] == '1') /**���ʱ���Ӳ���**/
        	{
        		/*memcpy(sMob.sfbz,cSfbz,sizeof(sMob.sfbz)-1);*/
        		sprintf(cMessage,"��λ%s�����ͬ��%d��%d��%dʱ%d�ֶ���ǩԼ��ͨ��"\
            ,cAcno_temp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,cOperation);
			      g_reply_int=proc_ln(sMob,kfacno,"0",sLn,sMob.mobile,cMessage,g_pub_tx.tx_amt1);
			      if(g_reply_int )
				    {
				        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
				        goto ErrExit;
				    }
        	} 
        	else if(flag[0] == '3') /**���ʱɾ������**/
        	{
        		sprintf(cMessage,"��λ%s�����ͬ��%d��%d��%dʱ%d�ֶ���ǩԼ�رա�"\
            ,cAcno_temp,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100,cOperation);
        		g_reply_int=proc_dd_del(sMob,"2",tmp,cMessage);	
        		if(g_reply_int )
				    {
				        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
				        goto ErrExit;
				    }
        	} 
        	else
        	{ 
						g_reply_int=proc_dd_upd(sMob,cOpt,tmp,tmp1,cMessage);	
						if(g_reply_int )
				    {
				        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
				        goto ErrExit;
				    }
        	}
        }
        else if(cOpt[0]=='2')
        {
        	strncpy(tmp,cTmpBuff_old+bb,11);       	
        	g_reply_int=proc_dd_del(sMob,cOpt,tmp,cMessage);	
        	if(g_reply_int )
			    {
			        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
			        goto ErrExit;
			    }
        }
        else
        {
        	strncpy(tmp,cTmpBuff_old,11); 
        	g_reply_int=proc_ln(sMob,kfacno,cOpt,sLn,tmp,cMessage,g_pub_tx.tx_amt1);
        	if(g_reply_int )
			    {
			        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);
			        goto ErrExit;
			    }
        }
        new_len -= 11;
        old_len -= 11;
        bb+=11;
        cc+=12;
        dd+=1;
      }
    }
    
    vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,g_reply_int);

		Mob_acct_type_Clo_Sel();
		/*    
    g_reply_int = pub_ins_trace_log();
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"S024");
    }
    */
    vtcp_log("[%s][%d] %s���׳ɹ�!!!!!",__FILE__,__LINE__,cOperation);
    
GoodExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
        memcpy(g_pub_tx.reply , "8891" ,4);
    sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

int proc_dd(struct mob_acct_type_c sMob,char *cOpt,char *ctmp,char *message,double fee_standard)
{
		long enddata = 0; /**���������***/
    char cAcno[21];
    char cAcno1[21];
    int cou;
    struct mob_acct_type_c sMbank;
    struct mdm_ac_rel_c sMdm;
    struct dd_mst_c sDd;
		struct mob_acct_type_c sMob_1;
		char allphones[50+1];
		char old_cTmpBuff[61];
		char brf[30];
		memset(&brf,0x00,sizeof(brf));
    memset(old_cTmpBuff , 0 , sizeof(old_cTmpBuff)); 
    memset(cAcno,0x00,sizeof(cAcno));
    memset(cAcno1,0x00,sizeof(cAcno1));
    memset(&sMbank,0x00,sizeof(sMbank));
    memset(&sMdm,0x00,sizeof(sMdm));
    memset(&sDd,0x00,sizeof(sDd));
    
    strcpy(cAcno,sMob.ac_no);
    pub_base_old_acno(cAcno);        /* �������˺�*/
    g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm,"ac_no='%s' ",cAcno);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]��ѯ��Mdm_ac_rel_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"D104");
        return -1;
    }
    g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&sDd,"ac_id=%ld",sMdm.ac_id);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]��ѯ��Dd_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"D104");
        return -1;
    }
		
		/**��������ռ���***/
		vtcp_log("ϵͳʱ��=[%ld]��������[%s]",g_pub_tx.tx_date,g_pub_tx.tmp_ind);
		vtcp_log("1111111111111��������[%d]",atoi(g_pub_tx.tmp_ind));
		pub_base_deadlineM(g_pub_tx.tx_date,atoi(g_pub_tx.tmp_ind),&enddata);
		vtcp_log("�������շ�����=[%ld]",enddata);
		
    vtcp_log("qqqqqqqqqqqqqqqqqqqq==[%c]",cOpt[0]);
    if(cOpt[0]=='0' )/**ǩԼ**/
    {
        if(sMdm.note_sts[0]!='0')/**����״̬������**/
        {
            g_reply_int=-1;
            vtcp_log("[%s][%d]����״̬������[%d]",__FILE__,__LINE__,g_reply_int);
            strcpy(g_pub_tx.reply,"M013");
            return -1;
        }
        if(sDd.ac_sts[0]!='1' ||sDd.hold_sts[0]=='1')/**�˻�״̬������**/
        {
            g_reply_int=-1;
            vtcp_log("[%s][%d]�ý���״̬������![%d]",__FILE__,__LINE__,g_reply_int);
            strcpy(g_pub_tx.reply,"M013");
            return -1;
        }
        g_reply_int=Mob_acct_type_Sel(g_pub_tx.reply,&sMbank,"ac_no='%s'",cAcno);
        
        if(g_reply_int==0 && cAcno[0]=='5' )
        {
            sMob.ac_id=sMdm.ac_id;
            sMob.ac_seqn=1;
            /*
            sMob.lst_fee_date=g_pub_tx.tx_date;
            */
            sMob.lst_fee_date=enddata;
            strcpy(sMob.mob_sts , "1");
            memcpy(sMob.sfbz,sMbank.sfbz,sizeof(sMob.sfbz)-1);
						
						Mob_acct_type_Debug(&sMob);
			
						g_reply_int=proc_mob_hst_ins(sMob);
						if(g_reply_int)
						{
							vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
							vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
							WRITEMSG
							return -1;
						}						
            g_reply_int=Mob_acct_type_Ins(sMob,g_pub_tx.reply);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d]���ݿ�����¼�쳣[%d]",__FILE__,__LINE__,g_reply_int);
                strcpy(g_pub_tx.reply,"D107");
                return -1;
            }
            /*likd������������81���ֻ������ֶ�*/
						set_zd_data("0810",sMob.mobile);
	        	cou = sql_count( "mob_acct_type" , "ac_no='%s'" , sMob.ac_no);
				    if(cou > 4)
				    {
				        vtcp_log("[%s][%d]ǩԼ�ֻ������4������[%d]",__FILE__,__LINE__,cou);
				        strcpy(g_pub_tx.reply,"F035");
				        return 1;
				    }
				    
				    strcpy(brf,"����ǩԼ��ͨ");	
				    mob_sendmail_dx(sMob,cOpt,message,brf);		
				    strcpy(g_pub_tx.ac_wrk_ind,"010000000");
				    g_pub_tx.ac_id = sMbank.ac_id;
						g_pub_tx.ac_seqn = 1;
						strcpy(g_pub_tx.sub_tx_code,"8891");
						g_pub_tx.svc_ind=0;
						g_pub_tx.tx_amt1 = 0.00;
						g_reply_int = pub_ins_trace_log();
				    if(g_reply_int)
				    {
				        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
				        strcpy(g_pub_tx.reply,"S024");
				    }				    
        }
        else if(g_reply_int==0 && cAcno[0]!='5')
        {
            g_reply_int=-1;
            vtcp_log("[%s][%d]�Ѿ�ǩԼ[%d]",__FILE__,__LINE__,g_reply_int);
            /*likd������������81���ֻ������ֶ�*/
						set_zd_data("0810",sMbank.mobile);
            strcpy(g_pub_tx.reply,"F036");
            return -1;
        }
        else if(g_reply_int==100 )/**δǩԼ���ѽ�Լ**/
        {
            sMob.ac_id=sMdm.ac_id;
            sMob.ac_seqn=1;
             /*
            sMob.lst_fee_date=g_pub_tx.tx_date;
            */
            sMob.lst_fee_date=enddata;
            strcpy(sMob.mob_sts , "1");
						/*�����Ǽ�ǩԼ��ʷ�Ǽǲ� add by wangxd 20101226*/
						Mob_acct_type_Debug(&sMob);
			
						g_reply_int=proc_mob_hst_ins(sMob);
						if(g_reply_int)
						{
							vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
							vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
							WRITEMSG
							return -1;
						}						
            g_reply_int=Mob_acct_type_Ins(sMob,g_pub_tx.reply);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d]���ݿ�����¼�쳣[%d]",__FILE__,__LINE__,g_reply_int);
                strcpy(g_pub_tx.reply,"D107");
                return -1;
            }
            /*likd������������81���ֻ������ֶ�*/
						set_zd_data("0810",sMob.mobile);
		        cou = sql_count( "mob_acct_type" , "ac_no='%s'" , sMob.ac_no);
				    if(cou > 4)
				    {
				        vtcp_log("[%s][%d]ǩԼ�ֻ������4������[%d]",__FILE__,__LINE__,cou);
				        strcpy(g_pub_tx.reply,"F035");
				        return 1;
				    }	
				    /*
				    g_reply_int = sendmail_jz(sMob.ac_no,sMob.ac_no,fee_standard,sMob.sfbz,"2"); 
     				if(g_reply_int)
				    {
				        vtcp_log("[%s][%d]���˲��ɹ�[%d][%s]",__FILE__,__LINE__,g_reply_int,g_pub_tx.reply);
				        strcpy(g_pub_tx.reply,"LS51");
				        return -1;
				    }
				    */
				    strcpy(brf,"����ǩԼ��ͨ");	
				    mob_sendmail_dx(sMob,cOpt,message,brf);	
				    
				    strcpy(g_pub_tx.ac_wrk_ind,"010000000");
						strcpy(g_pub_tx.sub_tx_code,"8891");
						g_pub_tx.svc_ind=0;
						g_pub_tx.tx_amt1 = 0.00;
						g_pub_tx.ac_id = sMob.ac_id;
						g_pub_tx.ac_seqn = 1;
				    g_reply_int = pub_ins_trace_log();
				    if(g_reply_int)
				    {
				        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
				        strcpy(g_pub_tx.reply,"S024");
				    }
	

        }
        else
        {
            vtcp_log("[%s][%d]��ѯ���Mob_acct_type_Selִ��ʧ��[%d]",__FILE__,__LINE__,g_reply_int);
            strcpy(g_pub_tx.reply,"D104");
            return -1;
        }
  
    }
    else if(cOpt[0]=='1')
    { 
    	g_reply_int=Mob_acct_type_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",cAcno);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]�����α����[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"M003");
          return -1;
      }
      g_reply_int=Mob_acct_type_Fet_Upd(&sMbank,g_pub_tx.reply);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]ȡ���ݴ���[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"D102");
          return -1;
      }
      sMbank.mob_sts[0] = sMob.mob_sts[0];
      memcpy(sMbank.sfbz , sMob.sfbz ,sizeof(sMbank.sfbz)-1);
      vtcp_log("%s %d ��ʼupdate",__FILE__,__LINE__);
      g_reply_int = sql_execute("update mob_acct_type set mobile = '%s',filler='%s' WHERE ac_no='%s' and mobile = '%s' ", \
      sMob.mobile,sMob.filler,cAcno,ctmp);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]����Э�����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"M003");
	        return -1;
	    }
			/*likd������������81���ֻ������ֶ�*/
			set_zd_data("0810",sMob.mobile);
	    strcpy(brf,"����ǩԼ����");
	    
	    /***���Ÿ���ʱ�������ζ��ţ���ǩԼ���ֻ��ŷ���ǩԼ�ɹ����ţ�ԭ�ֻ��ŷ���ǩԼ�ر���Ϣ��***/
	    if(memcmp(cAcno ,"621" , 3)==0 && strlen(cAcno)==19 )
	    {
		    /***������ʷ���¸��ĵ��ֻ���״̬Ϊ1��ԭ�е��ֻ���״̬Ϊ3****/
		    strcpy(sMob.mob_sts,"1");
				g_reply_int=proc_mob_hst_ins(sMob);
				if(g_reply_int)
				{
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
					WRITEMSG
					return -1;
				}		
		    memcpy(cAcno1 , sMob.ac_no+strlen(sMob.ac_no)-4 ,sizeof(cAcno1)-1);
				sprintf(message,"��β��%s��%d��%d��%02d:%02d���ж���ǩԼ��ͨ��"\
					    ,cAcno1,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);		
		    mob_sendmail_dx(sMob,cOpt,message,brf);
		    	
		    memset(&sMob.mobile,0x00,sizeof(sMob.mobile));
		    memcpy(sMob.mobile , ctmp ,sizeof(sMob.mobile)-1);
		    
		    strcpy(sMob.mob_sts,"3");
				g_reply_int=proc_mob_hst_ins(sMob);
				if(g_reply_int)
				{
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
					WRITEMSG
					return -1;
				} 	
		    sprintf(message,"��β��%s��%d��%d��%02d:%02d���ж���ǩԼ�رա�"\
		    ,cAcno1,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);
				
			}
			else
			{
				strcpy(sMob.mob_sts,"1");
				g_reply_int=proc_mob_hst_ins(sMob);
				if(g_reply_int)
				{
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
					WRITEMSG
					return -1;
				}
		    memcpy(cAcno1 , sMob.ac_no+strlen(sMob.ac_no)-4 ,sizeof(cAcno1)-1);
				sprintf(message,"����%s��%d��%d��%dʱ%d�ֽ��ж���ǩԼ��ͨ���ɵ�4006535666"\
					    ,cAcno1,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);		
		    mob_sendmail_dx(sMob,cOpt,message,brf);
		    	
		    memset(&sMob.mobile,0x00,sizeof(sMob.mobile));
		    memcpy(sMob.mobile , ctmp ,sizeof(sMob.mobile)-1); 	
		    
		    strcpy(sMob.mob_sts,"3");
				g_reply_int=proc_mob_hst_ins(sMob);
				if(g_reply_int)
				{
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
					WRITEMSG
					return -1;
				}
				
		    sprintf(message,"����%s��%d��%d��%dʱ%d�ֽ��ж���ǩԼ�رա��ɵ�4006535666"\
		    ,cAcno1,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);
					
			}
			
			mob_sendmail_dx(sMob,cOpt,message,brf);
	    g_pub_tx.ac_id = sMbank.ac_id;
			g_pub_tx.ac_seqn = 1;
	    g_reply_int = pub_ins_trace_log();
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"S024");
	    }
  	}
    else if(cOpt[0]=='2')
    {
    	
    	g_reply_int=Mob_acct_type_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",cAcno);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]�����α����[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"M003");
          return -1;
      }
      g_reply_int=Mob_acct_type_Fet_Upd(&sMbank,g_pub_tx.reply);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]ȡ���ݴ���[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"D102");
          return -1;
      }
      
      g_reply_int = sql_execute("delete from mob_acct_type  WHERE ac_no='%s' and mobile = '%s' ", cAcno,ctmp);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]ɾ��Э�����[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"D102");
          return -1;
      }

			strcpy(sMbank.mob_sts,"3");
			g_reply_int=proc_mob_hst_ins(sMbank);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
				vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
				WRITEMSG
				return -1;
			}

      strcpy(brf,"����ǩԼ�ر�");
	    mob_sendmail_dx(sMob,cOpt,message,brf);
	    g_pub_tx.ac_id = sMbank.ac_id;
			g_pub_tx.ac_seqn = 1;
	    g_reply_int = pub_ins_trace_log();
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"S024");
	    }
    }
    else
    {
        vtcp_log("[%s][%d]ǰ̨����Ĳ������ʹ���[%s]",__FILE__,__LINE__,cOpt);
        strcpy(g_pub_tx.reply,"D105");
        return -1;
    }
    return 0;
}


int proc_dd_upd(struct mob_acct_type_c sMob,char *cOpt, char *ctmp,char *ctmp1,char *message)
{
    char cAcno[21];
    int cou;
    struct mob_acct_type_c sMbank;
    struct mdm_ac_rel_c sMdm;
    struct dd_mst_c sDd;
		struct mob_acct_type_c sMob_1;
		char allphones[50+1];
		char old_cTmpBuff[61];
		char brf[30];
		memset(&brf,0x00,sizeof(brf));
		
    memset(old_cTmpBuff , 0 , sizeof(old_cTmpBuff)); 
    memset(cAcno,0x00,sizeof(cAcno));
    memset(&sMbank,0x00,sizeof(sMbank));
    memset(&sMdm,0x00,sizeof(sMdm));
    memset(&sDd,0x00,sizeof(sDd));
    
    sMbank.mob_sts[0] = sMob.mob_sts[0];
    /*memcpy(sMbank.sfbz , sMob.sfbz ,sizeof(sMbank.sfbz)-1);*/
    if(!strcmp(sMob.mobile,ctmp)&&!strcmp(sMob.filler,ctmp1))
    {
    		strcpy(ctmp,"xxxxxxxxxxx");
        vtcp_log("�ֻ�����δ�ı�!!");
        return 0;
		}
		 vtcp_log("%s %d ��ʼupdate",__FILE__,__LINE__);
    g_reply_int = sql_execute("update mob_acct_type set mobile = '%s',filler='%s' WHERE  mobile = '%s' and ac_no='%s'", \
    sMob.mobile,sMob.filler,ctmp,sMob.ac_no);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]����Э�����[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"D102");
        return -1;
		}


    /***������ʷ���¸��ĵ��ֻ���״̬Ϊ1��ԭ�е��ֻ���״̬Ϊ3****/
    strcpy(sMob.mob_sts,"1");

		g_reply_int=proc_mob_hst_ins(sMob);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
			vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
			WRITEMSG
			return -1;
		}	

		strcpy(brf,"����ǩԼ����");
		
		
    /***���Ÿ���ʱ�������ζ��ţ���ǩԼ���ֻ��ŷ���ǩԼ�ɹ����ţ�ԭ�ֻ��ŷ���ǩԼ�ر���Ϣ��***/
    
    if(sMob.ac_type[0]=='3')
    {
	    memcpy(cAcno , sMob.ac_no+strlen(sMob.ac_no)-4 ,sizeof(cAcno)-1);
			sprintf(message,"��λ%s�����ͬ��%d��%d��%dʱ%d�ֶ���ǩԼ��ͨ��"\
				    ,cAcno,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);		
	    mob_sendmail_dx(sMob,cOpt,message,brf);
	    	
	    memset(&sMob.mobile,0x00,sizeof(sMob.mobile));
	    memcpy(sMob.mobile , ctmp ,sizeof(sMob.mobile)-1); 	
	    sprintf(message,"��λ%s�����ͬ��%d��%d��%dʱ%d�ֶ���ǩԼ�رա�"\
	    ,cAcno,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);
			mob_sendmail_dx(sMob,cOpt,message,brf);
		}
		else
		{
			memcpy(cAcno , sMob.ac_no+strlen(sMob.ac_no)-4 ,sizeof(cAcno)-1);
			sprintf(message,"��λ%s�˻���%d��%d��%dʱ%d�ֶ���ǩԼ��ͨ��"\
				    ,cAcno,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);		
	    mob_sendmail_dx(sMob,cOpt,message,brf);
	    	
	    memset(&sMob.mobile,0x00,sizeof(sMob.mobile));
	    memcpy(sMob.mobile , ctmp ,sizeof(sMob.mobile)-1); 	
	    sprintf(message,"��λ%s�˻���%d��%d��%dʱ%d�ֶ���ǩԼ�رա�"\
	    ,cAcno,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);
			mob_sendmail_dx(sMob,cOpt,message,brf);
		}
		
		
		strcpy(sMob.mob_sts,"3");
		g_reply_int=proc_mob_hst_ins(sMob);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
			vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
			WRITEMSG
			return -1;
		}
		
		g_pub_tx.ac_id = sMob.ac_id;
		g_pub_tx.ac_seqn = 1; 
    g_reply_int = pub_ins_trace_log();
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"S024");
    }
		
    return 0;
}


int proc_ln(struct mob_acct_type_c sMob,char * kfacno,char *cOpt,struct ln_mst_c sLn,char *ctmp, char *message,double fee_standard)
{ 
		long enddata = 0;
		int cou = 0;
		char cAcno1[21];
	  struct ln_mst_c sLn_mst;
		char allphones[50+1];
	  struct mob_acct_type_c sMbank;
		struct mob_acct_type_c sMob_1;
	  char brf[30];
		memset(&brf,0x00,sizeof(brf));  
    memset(&sLn_mst , 0 , sizeof(sLn_mst));
    memset(&sMbank,0x00,sizeof(sMbank));
    memset(&cAcno1,0x00,sizeof(cAcno1));  
    
    /**��������ռ���***/
		vtcp_log("ϵͳʱ��=[%ld]��������[%s]",g_pub_tx.tx_date,g_pub_tx.tmp_ind);
		vtcp_log("1111111111111��������[%d]",atoi(g_pub_tx.tmp_ind));
		pub_base_deadlineM(g_pub_tx.tx_date,atoi(g_pub_tx.tmp_ind),&enddata);
		vtcp_log("�������շ�����=[%ld]",enddata);
		
    g_reply_int=Mob_acct_type_Sel(g_pub_tx.reply,&sMbank,"ac_no ='%s' ",sMob.ac_no);

    if(cOpt[0]=='0')/**ǩԼ**/
    {
    	if(g_reply_int==0)/**��ǩԼ������Ҫ�ն��ŷ�**/
      {
      	vtcp_log("[%s][%d] �����˺�Ϊ[%s]\n",__FILE__,__LINE__,sMob.ac_no);

        sMob.ac_id=sLn.ac_id;
        sMob.ac_seqn=1;
        sMob.lst_fee_date=g_pub_tx.tx_date;
     		strcpy(sMob.mob_sts , "1");
				/*�����Ǽ�ǩԼ��ʷ�Ǽǲ� add by wangxd 20101226*/
				Mob_acct_type_Debug(&sMob);
	
				g_reply_int=proc_mob_hst_ins(sMob);
				if(g_reply_int)
				{
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
					WRITEMSG
					return -1;
				}	
        g_reply_int=Mob_acct_type_Ins(sMob,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]���ݿ�����¼�쳣[%d]",__FILE__,__LINE__,g_reply_int);
            strcpy(g_pub_tx.reply,"D107");
            return -1;
        }        
        cou = sql_count( "mob_acct_type" , "ac_no='%s'" , sMob.ac_no);
		    if(cou > 4)
		    {
		        vtcp_log("[%s][%d]ǩԼ�ֻ������4������[%d]",__FILE__,__LINE__,cou);
		        strcpy(g_pub_tx.reply,"F035");
		        return 1;
		    }
		    
    	  strcpy(brf,"����ǩԼ��ͨ");	
		    mob_sendmail_dx(sMob,cOpt,message,brf);		

				strcpy(g_pub_tx.ac_wrk_ind,"010000000");
		    g_pub_tx.ac_id = sMbank.ac_id;
				g_pub_tx.ac_seqn = 1;
				strcpy(g_pub_tx.sub_tx_code,"8891");
				g_pub_tx.svc_ind=0;
				g_pub_tx.tx_amt1 = 0.00;
				
				g_reply_int = pub_ins_trace_log();
		    if(g_reply_int)
		    {
		        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
		        strcpy(g_pub_tx.reply,"S024");
		    }			    
    	}
    	else if(g_reply_int==100)/**δǩԼ��¼����۶��ŷ�**/
      {
      	vtcp_log("[%s][%d] �����˺�Ϊ[%s]\n",__FILE__,__LINE__,sMob.ac_no);

        sMob.ac_id=sLn.ac_id;
        sMob.ac_seqn=1;
        /*
        sMob.lst_fee_date=g_pub_tx.tx_date;
        */
        sMob.lst_fee_date=enddata;
     		strcpy(sMob.mob_sts , "1");
				/*�����Ǽ�ǩԼ��ʷ�Ǽǲ� add by wangxd 20101226*/
				Mob_acct_type_Debug(&sMob);
	
				g_reply_int=proc_mob_hst_ins(sMob);
				if(g_reply_int)
				{
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
					vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
					WRITEMSG
					return -1;
				}	
        g_reply_int=Mob_acct_type_Ins(sMob,g_pub_tx.reply);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d]���ݿ�����¼�쳣[%d]",__FILE__,__LINE__,g_reply_int);
            strcpy(g_pub_tx.reply,"D107");
            return -1;
        }        
        cou = sql_count( "mob_acct_type" , "ac_no='%s'" , sMob.ac_no);
		    if(cou > 4)
		    {
		        vtcp_log("[%s][%d]ǩԼ�ֻ������4������[%d]",__FILE__,__LINE__,cou);
		        strcpy(g_pub_tx.reply,"F035");
		        return 1;
		    }	
				/*
		    g_reply_int = sendmail_jz(sMob.ac_no,kfacno,fee_standard,sMob.sfbz,"2"); 	
 				if(g_reply_int)
		    {
		        vtcp_log("[%s][%d]���˲��ɹ�[%d][%s]",__FILE__,__LINE__,g_reply_int,g_pub_tx.reply);
		        strcpy(g_pub_tx.reply,"LS51");
		        return -1;
		    }
		    */
    		strcpy(brf,"����ǩԼ��ͨ");	
		    mob_sendmail_dx(sMob,cOpt,message,brf);	
		    
		    strcpy(g_pub_tx.ac_wrk_ind,"010000000");
				strcpy(g_pub_tx.sub_tx_code,"8891");
				g_pub_tx.svc_ind=0;
		    g_pub_tx.tx_amt1 = 0.00;
				g_pub_tx.ac_id = sMob.ac_id;
				g_pub_tx.ac_seqn = 1;
		    g_reply_int = pub_ins_trace_log();
		    if(g_reply_int)
		    {
		        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
		        strcpy(g_pub_tx.reply,"S024");
		    }	    
			}
      else
      {
				vtcp_log("[%s][%d]��ѯ��Mob_acct_type_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"D104");
        return -1;
      }
    }
    else if(cOpt[0]=='1')/**�޸�**/
    {
    	g_reply_int=Mob_acct_type_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",sMob.ac_no);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]�����α����[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"M003");
          return -1;
      }
      g_reply_int=Mob_acct_type_Fet_Upd(&sMbank,g_pub_tx.reply);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]ȡ���ݴ���[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"D102");
          return -1;
      }
      sMbank.mob_sts[0] = sMob.mob_sts[0];
      memcpy(sMbank.sfbz , sMob.sfbz ,sizeof(sMbank.sfbz)-1);

		 vtcp_log("%s %d ��ʼupdate",__FILE__,__LINE__);
      g_reply_int = sql_execute("update mob_acct_type set mobile = '%s' WHERE ac_no='%s' and mobile = '%s' ", \
      sMob.mobile,sMob.ac_no,ctmp);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]����Э�����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D102");
	        return -1;
	    }

	    strcpy(brf,"����ǩԼ����");

			strcpy(sMob.mob_sts,"1");
			g_reply_int=proc_mob_hst_ins(sMob);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
				vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
				WRITEMSG
				return -1;
			}

	    /***���Ÿ���ʱ�������ζ��ţ���ǩԼ���ֻ��ŷ���ǩԼ�ɹ����ţ�ԭ�ֻ��ŷ���ǩԼ�ر���Ϣ��***/
	    memcpy(cAcno1 , sMob.ac_no+strlen(sMob.ac_no)-4 ,sizeof(cAcno1)-1);
			sprintf(message,"��λ%s�����ͬ��%d��%d��%dʱ%d�ֶ���ǩԼ��ͨ��"\
				    ,cAcno1,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);		
	    mob_sendmail_dx(sMob,cOpt,message,brf);
	    	
	    memset(&sMob.mobile,0x00,sizeof(sMob.mobile));
	    memcpy(sMob.mobile , ctmp ,sizeof(sMob.mobile)-1); 	
	    sprintf(message,"��λ%s�����ͬ��%d��%d��%dʱ%d�ֶ���ǩԼ�رա�"\
	    ,cAcno1,(g_pub_tx.tx_date%10000)/100,g_pub_tx.tx_date%100,g_pub_tx.tx_time/10000,g_pub_tx.tx_time/100%100);
			
			strcpy(sMob.mob_sts,"3");
			g_reply_int=proc_mob_hst_ins(sMob);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
				vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
				WRITEMSG
				return -1;
			}
			
			mob_sendmail_dx(sMob,cOpt,message,brf);
			g_pub_tx.ac_id = sMbank.ac_id;
			g_pub_tx.ac_seqn = 1;
	    g_reply_int = pub_ins_trace_log();
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"S024");
	    }
    }
    else if(cOpt[0]=='2')/**ע��**/
    {
    	g_reply_int=Mob_acct_type_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",sMob.ac_no);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]�����α����[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"M003");
          return -1;
      }
      g_reply_int=Mob_acct_type_Fet_Upd(&sMbank,g_pub_tx.reply);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]ȡ���ݴ���[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"D102");
          return -1;
      }
      g_reply_int = sql_execute("delete from mob_acct_type  WHERE ac_no='%s' and mobile = '%s' ", sMob.ac_no,ctmp);
      if(g_reply_int)
      {
          vtcp_log("[%s][%d]ɾ��Э�����[%d]",__FILE__,__LINE__,g_reply_int);
          strcpy(g_pub_tx.reply,"D102");
          return -1;
      }

      strcpy(brf,"����ǩԼ�ر�");
      
      strcpy(sMob.mob_sts,"3");
			g_reply_int=proc_mob_hst_ins(sMob);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
				vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
				WRITEMSG
				return -1;
			}
      
	    mob_sendmail_dx(sMob,cOpt,message,brf);
			g_pub_tx.ac_id = sMbank.ac_id;
			g_pub_tx.ac_seqn = 1;
	    g_reply_int = pub_ins_trace_log();
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"S024");
	    }
    }
    else
    {
        vtcp_log("[%s][%d]ǰ̨����Ĳ������ʹ���[%s]",__FILE__,__LINE__,cOpt);
        strcpy(g_pub_tx.reply,"D105");
        return -1;
    }
    return 0;
}

int proc_dd_del(struct mob_acct_type_c sMob,char *cOpt, char *ctmp,char *message)
{
    char cAcno[21];
    int cou;
    struct mob_acct_type_c sMbank;
    struct mdm_ac_rel_c sMdm;
    struct dd_mst_c sDd;
		struct mob_acct_type_c sMob_1;
		char allphones[50+1];
		char old_cTmpBuff[61];
		char brf[30];
		memset(&brf,0x00,sizeof(brf));
    memset(old_cTmpBuff , 0 , sizeof(old_cTmpBuff)); 
    memset(cAcno,0x00,sizeof(cAcno));
    memset(&sMbank,0x00,sizeof(sMbank));
    memset(&sMdm,0x00,sizeof(sMdm));
    memset(&sDd,0x00,sizeof(sDd));
    
    sMbank.mob_sts[0] = sMob.mob_sts[0];
    memcpy(sMbank.sfbz , sMob.sfbz ,sizeof(sMbank.sfbz)-1);
		

    g_reply_int = sql_execute("delete from mob_acct_type WHERE ac_no='%s' and mobile = '%s' ", sMob.ac_no,ctmp);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]ɾ��Э�����[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"D102");
        return -1;
    }
    memcpy(sMob.mobile , ctmp ,sizeof(sMob.mobile)-1);

    /***������ʷ���¸��ĵ��ֻ���״̬Ϊ1��ԭ�е��ֻ���״̬Ϊ3****/
    strcpy(sMob.mob_sts,"3");
		g_reply_int=proc_mob_hst_ins(sMob);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���g_reply_int[%d]",__FILE__,__LINE__,g_reply_int);
			vtcp_log("[%s][%d]proc_mob_hst_ins�������ݴ���reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
			WRITEMSG
			return -1;
		}	

		strcpy(brf,"����ǩԼ�ر�");
    mob_sendmail_dx(sMob,cOpt,message,brf);
		g_pub_tx.ac_id = sMob.ac_id;
		g_pub_tx.ac_seqn = 1;
    g_reply_int = pub_ins_trace_log();
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]д��־���ɹ�[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"S024");
    }
    return 0;
}


int mob_sendmail_dx(struct mob_acct_type_c sMob,char *cOpt,char *message,char *brf)
{			
		struct mob_sendmail_c  sMail;
	  struct mdm_ac_rel_c sMdmm;
	  struct ln_mst_c sLn;
	  memset(&sMail,0x00,sizeof(sMail));
    memset(&sMdmm,0x00,sizeof(sMdmm));
	  memset(&sLn,0x00,sizeof(sLn));
	 	strcpy( sMail.tx_code,g_pub_tx.tx_code);
	  strcpy( sMail.ac_no, sMob.ac_no);
		strcpy( sMail.message, message );
	  strcpy( sMail.tel, g_pub_tx.tel );
	  strcpy( sMail.mobile,sMob.mobile);
	  sMail.trace_no = g_pub_tx.trace_no;
	  sMail.tx_date = g_pub_tx.tx_date;
	  sMail.tx_time = g_pub_tx.tx_time;
		strcpy( sMail.mobile,sMob.mobile);
		strcpy( sMail.opt_no,"110");
	  strcpy( sMail.timeflag, "0" );
	  sMail.t_date = 0;
	  sMail.t_time = 0;
	  strcpy( sMail.send_level, "1" );
	  sMail.sene_times = 1;
	  sMail.send_date = 0;
	  sMail.send_time = 0;
	  sMail.sxf_amt = 0.00;
	  strcpy(sMail.proc_flag, "0" );
	  sMail.mobseq = 0;
	  
	  /**3Ϊ�����1Ϊ���ڻ�***/
		if(sMob.ac_type[0]=='3')
		{
			g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&sLn,"pact_no like '%%%s%%'",sMob.ac_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Ln_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D104");
	        return -1;
	    }
	    sMail.ac_id = sLn.ac_id;
	    
		}
		else
		{
			g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdmm,"ac_no='%s'",sMob.ac_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]��ѯ��Mdm_ac_rel_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"D104");
	        return -1;
	    }
	    sMail.ac_id = sMdmm.ac_id;
	  }
	  
	  if(cOpt[0]=='0') 
	  {      
	      strcpy( sMail.tx_type, "1" ); 
	  }
	  else  
	  {
	      strcpy( sMail.tx_type, "2" );  
	  }
	  
	  Mob_sendmail_Debug(&sMail);
	  g_reply_int = Mob_sendmail_Ins( sMail, g_pub_tx.reply );
	  if ( g_reply_int != 0 )
	  {
	      vtcp_log("[%s][%d]���ݿ�������[%d]",__FILE__,__LINE__,g_reply_int);
	      strcpy(g_pub_tx.reply,"S054");
	  }
    vtcp_log("[%s][%d] ��ʼд��ˮ! ac_no=[%s]",__FILE__,__LINE__,sMail.ac_no);
    vtcp_log("[%s][%d] ��ʼд��ˮ! sub_tx_code=[%s]",__FILE__,__LINE__,g_pub_tx.sub_tx_code);
    strcpy(g_pub_tx.ac_no,sMail.ac_no);
    strcpy(g_pub_tx.brf,brf);

}

int sendmail_jz(char *qyacno,char *sfacno,double amt,char *sfbz,char *ct_ind)
{
	  
	   char cOpt[2];/**��������0��ͨ1�����ͣ2ע��**/
	   char acc_no[20];    /*�������ѿ�Ŀ*/
	   char tx_br_no[5+1] ;
	   char in_br_no[5+1];
	   int  months=0;
	   int  txflag=0; /*�ж��Ƿ���Ҫ���б����˽��� 0 --��Ҫ 1--����Ҫ*/
	   int  g_reply_int = 0;
	   int  ret = 0;
	   struct mdm_ac_rel_c stMdm_ac_rel;
	   struct com_branch_c stCom_branch;
	   struct dd_mst_c stDd_mst;
     memset(&stMdm_ac_rel,0x00,sizeof(stMdm_ac_rel));
     memset(&stCom_branch,0x00,sizeof(stCom_branch));
     memset(&stDd_mst,0x00,sizeof(stDd_mst));
     
vtcp_log("1111111111111111111111111=[%c]",ct_ind[0]);
	   if( ct_ind[0]=='2' )
     {/*ת��*/
          memset(&stMdm_ac_rel,0,sizeof(stMdm_ac_rel));
          g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&stMdm_ac_rel,"ac_no='%s'",sfacno);  
          if (g_reply_int)
          {
               vtcp_log("[%s][%d]��ѯ��Mdm_ac_rel_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
               strcpy(g_pub_tx.reply,"D104");
               goto ErrExit;
          }
          g_reply_int = Com_branch_Sel(g_pub_tx.reply,&stCom_branch,"br_no='%s'",stMdm_ac_rel.opn_br_no); 
          if (g_reply_int)
          {
               vtcp_log("[%s][%d]��ѯ��Com_branch����[%d]",__FILE__,__LINE__,g_reply_int);
               strcpy(g_pub_tx.reply,"D104");
               goto ErrExit;
          } 
          set_zd_data("0650",stMdm_ac_rel.opn_br_no);
          set_zd_data("0820",stCom_branch.br_name);
          g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&stDd_mst,"ac_id=%d",stMdm_ac_rel.ac_id);
          if(g_reply_int){
               vtcp_log("[%s][%d]��ѯ��Dd_mst_Sel����[%d]",__FILE__,__LINE__,g_reply_int);
               strcpy(g_pub_tx.reply,"D104");
               goto ErrExit;
          }
          set_zd_data("0830",stDd_mst.name);
          /*-----��ǿͻ���,����Ƿ��˻�����------*/
          if(stMdm_ac_rel.note_sts[0]!='0')/**����״̬������**/
          { 
               vtcp_log("[%s][%d]����״̬������[%d]",__FILE__,__LINE__,g_reply_int);
               strcpy(g_pub_tx.reply,"M013");
               goto ErrExit;
          }
          if(stDd_mst.ac_sts[0]!='1' ||stDd_mst.hold_sts[0]!='0')/**�˻�״̬������**/
          {
               vtcp_log("[%s][%d]�ý���״̬������![%d]",__FILE__,__LINE__,g_reply_int);
               strcpy(g_pub_tx.reply,"M013");
               goto ErrExit;
          }
          /*
          if(g_pub_tx.tx_amt1 ==0.0)  
          {
	          vtcp_log("%s,%d,�����շѲ�����[%.2f]\n",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	          strcpy(g_pub_tx.reply, "0001"); 
	          goto ErrExit;
     			}
          vtcp_log("%s,%d,stMdm_ac_rel.ac_id=[%ld]!",__FILE__,__LINE__,stMdm_ac_rel.ac_id);
         */
         
         	/***�Ƚϸ����˻�����Ƿ��㹻֧�����ŷ����***/        	
         	if(pub_base_CompDblVal(stDd_mst.bal,amt) < 0)
         	{
         		vtcp_log("[%s][%d]�ۿ��˻�������֧�����ŷ���ѣ�������",__FILE__,__LINE__);
        		strcpy(g_pub_tx.reply,"LS51");
        		goto ErrExit;
         	}
         	
          /**�¿ͻ���**/
          g_reply_int=sub_tran_acct(stMdm_ac_rel.ac_no,amt,"���ſ۷�",stDd_mst.opn_br_no,"2");
          if(g_reply_int)
          {
            goto ErrExit;  
          }
          /**�ǿ�Ŀ����Ϣ����ҵ������**/
          g_reply_int=sub_cr_acct("51110",amt,"���ſ۷�",stDd_mst.opn_br_no,"2");
          if(g_reply_int)
          {
            goto ErrExit;
          }
     }   
      
     if( ct_ind[0]=='1' )
     {
          /*�����ŷ���ҵ������60219903 ���ֽ�M001*/
          memcpy(g_pub_tx.ac_no1,"100102",6);
          g_reply_int=sub_dr_cash_acct(g_pub_tx.ac_no1,g_pub_tx.tx_amt1,"���ſ۷�",g_pub_tx.tx_br_no,"1");
          if(g_reply_int)
          {
              goto ErrExit;
            }
          g_reply_int=sub_cr_acct("51110",g_pub_tx.tx_amt1,"���ſ۷�",g_pub_tx.tx_br_no,"1");
          if(g_reply_int)
          {
              goto ErrExit;
          }
     }
     return 0;
	ErrExit:
     return 1;
}

int sub_tran_acct( char *dr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{
     int ret=0;
     vtcp_log("�跽���ʿ�Ŀ[%s][%.2lf]",dr_ac_no,amt);
     g_pub_tx.ac_id = 0;  /*test clean */
     g_pub_tx.ac_seqn = 1;
     strcpy( g_pub_tx.tx_code, "8891");
     strcpy( g_pub_tx.ac_no, dr_ac_no );
     strcpy(g_pub_tx.tx_br_no,g_pub_tx.tx_br_no);
     strcpy(g_pub_tx.opn_br_no,br_no);
     g_pub_tx.tx_amt1 = amt;
     if(dr_ac_no[0]=='9')
     {
          strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
          strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
          g_pub_tx.svc_ind=9001;
          memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
          strcpy( g_pub_tx.sub_tx_code, "A016" );
          set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
          set_zd_data( "1204", "01" );
          set_zd_data( "1205", "2" );
          set_zd_double( "1208", g_pub_tx.tx_amt1 );
     }
     else
     {
          strcpy( g_pub_tx.ac_id_type,"1" ); /*�˻�����Ϊ����*/
          strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
          g_pub_tx.svc_ind=1001;
          memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
          strcpy( g_pub_tx.sub_tx_code, "D003" );
          set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
          strcpy( g_pub_tx.prt_ind, "0" );
          set_zd_data( "102O", "0" );
          set_zd_data( "102J", "01" );
          set_zd_data( "102K", "2" );
          set_zd_double( "102F", g_pub_tx.tx_amt1 );
          set_zd_double( "102P", 0.00 );
          set_zd_double( "102Q", 0.00 );
     }
     /*strcpy( g_pub_tx.brf,brf );*/
     strcpy( g_pub_tx.brf,"���ն���֪ͨ�����" );
     strcpy( g_pub_tx.no_show,"0" );
     strcpy(g_pub_tx.add_ind,"0"); 
     
     strcpy(g_pub_tx.ct_ind,"2");
     strcpy(g_pub_tx.ac_wrk_ind,"000000101");
     strcpy(g_pub_tx.hst_ind,"1");
     ret=pub_acct_trance();
     if( ret ) goto ErrExit;
     /* ���л�Ƽ��� */
     ret = pubf_acct_proc(g_pub_tx.sub_tx_code );
     if (ret != 0)
     {
          sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
          WRITEMSG
          goto ErrExit;
     }
     set_zd_data("102O","" );
     set_zd_data("102J","" );
     set_zd_double("102F",0.00 );
     set_zd_data("102K","" );
     set_zd_double("102P",0.00 );
     set_zd_double("102Q",0.00 );
     set_zd_data( "1204", "" );
     set_zd_data( "1205", "" );
     set_zd_double( "1208", 0.00 );
     strcpy( g_pub_tx.add_ind,"" );

     return 0;
ErrExit:
     return 1;
}
int sub_cr_acct(char * cr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{
     int ret=0;
     vtcp_log("�������ʿ�Ŀ[%s][%.2lf]",cr_ac_no,amt);
     strcpy( g_pub_tx.tx_code, "8891");
     strcpy(g_pub_tx.tx_br_no,g_pub_tx.tx_br_no);
     strcpy(g_pub_tx.opn_br_no,br_no);
     strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
     strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
     strcpy(g_pub_tx.cur_no,"01");
     strcpy(g_pub_tx.hst_ind,"1");
     g_pub_tx.svc_ind=9001;
     strcpy( g_pub_tx.ct_ind,ct_ind );
     strcpy( g_pub_tx.add_ind,"1" );
     strcpy( g_pub_tx.prdt_code,"" );
     strcpy( g_pub_tx.ac_no,cr_ac_no );
     g_pub_tx.ac_id=0;
     g_pub_tx.ac_seqn=1;
     g_pub_tx.tx_amt1=amt;
     strcpy( g_pub_tx.brf,"���ն���֪ͨ�����" );
     strcpy( g_pub_tx.no_show,"0");
     strcpy(g_pub_tx.ct_ind,"2");
     strcpy(g_pub_tx.ac_wrk_ind,"000000101");
     strcpy(g_pub_tx.hst_ind,"1");
     strcpy( g_pub_tx.sub_tx_code, "A017");
     /*** ���û�Ʒ�¼�Զ��� ***/
     ret=pub_acct_trance();
     if( ret ) goto ErrExit;
     /* ���л�Ƽ��� */
     set_zd_data("1214","01" );
     set_zd_data("1215",ct_ind );
     set_zd_double("1218",amt);
     ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
     if (ret != 0)
     {
          sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
          WRITEMSG
          goto ErrExit;
     }  
     
     set_zd_data("1204","" );
     set_zd_data("1205","" );
     set_zd_double("1208",0.00 );
     set_zd_data("1214","" );
     set_zd_data("1215","" );
     set_zd_double("1218",0.00 );
     strcpy( g_pub_tx.add_ind,"" );

     return 0;
ErrExit:
     return 1;
}
/************************
���ֽ�
*************************/
int sub_dr_cash_acct(char * dr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{
     int ret=0;
     
     vtcp_log("�ֽ��Ŀ[%s][%.2lf]",dr_ac_no,amt);
     strcpy( g_pub_tx.tx_code, "8891");
     strcpy( g_pub_tx.sub_tx_code, "M001");
     strcpy(g_pub_tx.tx_br_no,br_no);
     strcpy(g_pub_tx.opn_br_no,br_no);
     strcpy(g_pub_tx.cur_no,"01");
     strcpy(g_pub_tx.ac_wrk_ind,"0000000");
     g_pub_tx.svc_ind=0;
     strcpy( g_pub_tx.ct_ind,ct_ind );
     strcpy( g_pub_tx.add_ind,"1" );
     strcpy( g_pub_tx.prdt_code,"" );
     g_pub_tx.ac_id=0;
     g_pub_tx.ac_seqn=1;
     g_pub_tx.tx_amt1=amt;
     strcpy(g_pub_tx.brf,brf);
     strcpy( g_pub_tx.no_show,"0" );
     get_zd_data("0070",g_pub_tx.tel);
     strcpy(g_pub_tx.ac_no,dr_ac_no);
     strcpy(g_pub_tx.hst_ind,"1");
     strcpy( g_pub_tx.cash_code,"12J08" );
     ret=pub_acct_cash();
     if( ret ) goto ErrExit;
     /* ���л�Ƽ��� */
     set_zd_data("1181","01" );
     set_zd_double("1184",amt);
     ret = pubf_acct_proc("M001");
     if (ret != 0)
     {
         sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
         WRITEMSG
         goto ErrExit;
     }
     set_zd_data("1181","" );
     set_zd_double("1184",0.00 );
     strcpy( g_pub_tx.add_ind,"" );
     return 0;
ErrExit:
     return 1;
}

/*********************************************************************
* �� �� �� :  
* �������� :  �ؽ���
* ��    �� :  2014/10/21
* ����ģ�� :  ��Ʋ�Ʒ�ͻ����׼�¼��ѯ
* �������� :  spW210.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into tx_def values ('5056', '��Ʋ�Ʒ�ͻ����׼�¼��ѯ', '10000000000000000000110010010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '0', '4', '2',);
insert into tx_flow_def values ('5056', 0, 'W210', '#$');
insert into tx_sub_def values ('W210', '��Ʋ�Ʒ�ͻ����׼�¼��ѯ', 'spW210', '0', '0');
***********************************************************************/

#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "string.h"

int spW210()
{
    char cCertNo[21];
    memset(cCertNo,0x00,sizeof(cCertNo));
    /**��ʼ�����ýṹadd by guanjianfeng 20150907**/
	if (pub_base_sysinit())
	{
	    vtcp_log("��ʼ�����ýṹ����!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
    get_zd_data("0620",cCertNo);
    
    if(strlen(cCertNo) == 0)/*�����˺Ų�ѯ*/
    {
        if(tx_0001())
        {
            vtcp_log("�����˺Ų�ѯ��Ʋ�Ʒ�ͻ����׼�¼����");
            goto ErrExit;
        }
    }
    else					/*����֤�������ѯ*/
    {
    	if(tx_0002())
        {
            vtcp_log("����֤�����ͣ�֤�������ѯ��Ʋ�Ʒ�ͻ����׼�¼����");
            goto ErrExit;
        }
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    set_zd_data("0120","0000");
	set_zd_data("0130","��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣ�ɹ�");
    return 0;
ErrExit:
	set_zd_data("0120","9999");
    strcpy(g_pub_tx.reply,"9999");
	set_zd_data("0130","��ѯ��Ʋ�Ʒ�ͻ����׼�¼��Ϣʧ��");
    vtcp_log("��ѯ��Ʋ�Ʒ�ͻ����׼�¼��Ϣʧ��[%s]", g_pub_tx.reply);
    return 1;  
}
/*�����˺Ų�ѯ*/
int tx_0001()
{
	int 		ret = 0;
	int 		num=1;
	int 		num2=0;
    char 		cAc_no[32];
    char 		cFlag[2];/*�Ƿ��Ϲ�������־*/
    char 		cSts[12];
    struct 		fn_parm_c sFn_parm;
    struct 		fn_reg_c sFn_reg;
    FILE 		*fp  = NULL;
    char 		cFilename[256];
    char        pcfilename[100];
    char 		cWherelist[256];
    char 		cTerm_type[3];
    char 		begin[4];
    char 		total[4];
    char        tx_date[8+1];
	char        sw_traceno[10+1];
	char		Num[8];
	int			count=0;
/*add gjf 20141119*/
	char 		sDateMark[1+1];		/*���ڱ�־*/
	char 		sbeginDate[8+1];	/*��ʼ����*/
	char		sendDate[8+1];		/*��������*/
	struct mdm_ac_rel_c mdm_ac_rel;/*�������˻���ϵ*/
	memset(&mdm_ac_rel,0x00,sizeof(mdm_ac_rel));
	
	memset(sDateMark,0x00,sizeof(sDateMark));
	memset(sbeginDate,0x00,sizeof(sbeginDate));
	memset(sendDate,0x00,sizeof(sendDate));
	
	get_zd_data("0660",sDateMark);
	get_zd_data("0460",sbeginDate);
	get_zd_data("0470",sendDate);
	
	long beginDate=0;
	long endDate=0;
	
	beginDate = atol(sbeginDate);
	endDate   = atol(sendDate);

/*add gjf 20141119*/
	
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(pcfilename, 0x00, sizeof(pcfilename));
    memset(cWherelist,0x00,sizeof(cWherelist));
    memset(cSts,0x00,sizeof(cSts));
    memset(cFlag,0x00,sizeof(cFlag));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(begin,0x00,sizeof(begin));
    memset(total,0x00,sizeof(total));
    memset(tx_date,0x00,sizeof(tx_date));
    memset(sw_traceno,0x00,sizeof(sw_traceno));
    memset(Num,0x00,sizeof(Num));
       
    get_zd_data("0300",cAc_no);		/*�˺�*/
    get_zd_data("0680",cFlag);		/*�Ƿ��Ϲ�����*/
    get_zd_data("0500",begin);		/*��ѯ��ʼ��¼��*/
    get_zd_data("0510",total);		/*�����¼��*/
    get_zd_data("0050",tx_date);
		get_zd_data("0520",sw_traceno);
		
		ret = -1;
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",cAc_no);
		if(ret)
		{
			vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"W011");
			goto ErrExit;	
		}
    
    int start=atoi(begin);
    int sum=atoi(total);
    if(sum == 0)
    {
    	sum = 20;
    }
    sprintf( cFilename,"5056netbank%s%s",tx_date,sw_traceno );
    sprintf( pcfilename,"%s/%s",getenv("FILDIR"),cFilename );
    fp=fopen(pcfilename,"w");
    if(fp == NULL)
    {
        vtcp_log("���ļ�ʧ��[%s]",pcfilename);
        strcpy(g_pub_tx.reply,"SM100");
        goto ErrExit;
    }
    
    fprintf(fp,"~�˺�|��������|������ˮ��|���׻���|Э����|��Ʒ����|�Ϲ�����|�Ϲ����|��ؽ��|���ر���|���ʸ�������|������������|Ԥ������|��Ϣ����|��������|��������|����|������|�����־|����ժҪ|״̬|�ۿ�����|�ۿ���ˮ��|������������|����������ˮ��|��Ʒ����|�Ƿ������|�Ƿ��ܳ���|����|Ԥ���껯������|���յȼ�|\n");
    vtcp_log("%s,%d,ʱ���־[%s]",__FILE__,__LINE__,sDateMark);
    if(!memcmp(cFlag,"0",1))
    {
    	/* start add by gjf 20141119*/
    	
    	if(!memcmp(sDateMark,"0",1) && beginDate!=0 && endDate!=0)
    	{
    		sprintf(cWherelist,"ac_id =%ld and tx_date between '%s' and '%s' order by tx_date desc,trace_no desc",mdm_ac_rel.ac_id,sbeginDate,sendDate);
    	}
    	else if(!memcmp(sDateMark,"1",1) && beginDate!=0 && endDate!=0)
    	{
    		sprintf(cWherelist,"ac_id =%ld and end_date between '%s' and '%s' order by tx_date desc,trace_no desc",mdm_ac_rel.ac_id,sbeginDate,sendDate);
    	}
    	else
    	{
    		sprintf(cWherelist,"ac_id =%ld order by tx_date desc,trace_no desc",mdm_ac_rel.ac_id);
    	}	
    	/* end add by gjf 20141119*/
    	/*��Ʒ����*/
		count=sql_count("fn_reg",cWherelist);
		set_zd_int("0450",count);
		vtcp_log("%s %d::��Ʒ����count=%d",__FILE__,__LINE__,count);
    	
    	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
	    if(ret)
	    {
	        vtcp_log("������ƵǼǲ��α����");
	        strcpy(g_pub_tx.reply,"D101");
	        goto ErrExit;
	    } 
	    
	    while(1)
	    {
	        memset(&sFn_parm,0x00,sizeof(sFn_parm));
	        memset(&sFn_reg,0x00,sizeof(sFn_reg));
	        memset(cSts,0x00,sizeof(cSts));
	        memset(cTerm_type,0x00,sizeof(cTerm_type));
	        
	        ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
	       	if(ret != 100 && ret != 0)
	        {
	            vtcp_log("ȡ��ƵǼǲ��α����");
	            strcpy(g_pub_tx.reply,"D330");
	             Fn_reg_Clo_Sel();
	            goto ErrExit;
	        }
	        else if(ret == 100 || ret == 1403)
	    	{
	    		break;
	    	}
	    	else
	    	{
				ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
		        if(ret)
		        {
		            vtcp_log("��ѯ��Ʋ�Ʒ��������");
		            strcpy(g_pub_tx.reply,"D324");
		             Fn_reg_Clo_Sel();
		            goto ErrExit;
		        }      
				if(num >= start && (num-start) < sum)
				{
		        	fprintf(fp,"%24s|%ld|%9ld|%5s|%20s|%20s|%9ld|%16.2f|%16.2f|%16.2f|%9.2f|%9.2f|%16.2f|%ld|%ld|%9ld|%16.2lf|%16.2f|%s|%20s|%s|%ld|%9ld|%ld|%9ld|%60s|%s|%s|%ld|%9.2f|%s|\n",\
		        		sFn_reg.ac_no,sFn_reg.tx_date,sFn_reg.trace_no,sFn_reg.tx_br_no,sFn_reg.pact_no,\
		       			sFn_reg.prdt_code,sFn_reg.count,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
		        		sFn_reg.return_capital,sFn_reg.flt_radio,sFn_reg.return_rate,sFn_reg.intst,\
		        		sFn_parm.ic_date,sFn_parm.mtr_date,sFn_reg.date1,sFn_reg.fct_intst,sFn_reg.profit_amt,\
		        		sFn_reg.profit_flag[0] == 'Y' ?"Y":"N",sFn_reg.brf,sFn_reg.sts,sFn_reg.pay_date,\
		        		sFn_reg.pay_trace_no,sFn_reg.return_date,sFn_reg.return_trace_no,\
		        		sFn_parm.title,sFn_parm.redempt[0] == 'Y' ? "Y":"N",sFn_parm.cancel_flag[0] == 'Y' ?"Y":"N",sFn_parm.term,sFn_parm.rate,sFn_parm.risk_lvl);
		        		num2++;
		        }
		        if((num-start) >= sum)
		        {
		        	break;
		        }
		        ++num;
		    }
	    }
	    Fn_reg_Clo_Sel();
	}
    else 
    {
    	/* start add by gjf 20141119*/
    	if(!memcmp(sDateMark,"0",1) && beginDate!=0 && endDate!=0)
    	{
    		sprintf(cWherelist,"ac_id =%ld and sts = '0' and tx_date between '%s' and '%s' order by tx_date desc,trace_no desc",mdm_ac_rel.ac_id,sbeginDate,sendDate);
    	}
    	else if(!memcmp(sDateMark,"1",1) && beginDate!=0 && endDate!=0)
    	{
    		sprintf(cWherelist,"ac_id =%ld and sts = '0' and end_date between '%s' and '%s' order by tx_date desc,trace_no desc",mdm_ac_rel.ac_id,sbeginDate,sendDate);
    	}
    	else
    	{
    		sprintf(cWherelist,"ac_id =%ld and sts = '0' order by tx_date desc,trace_no desc",mdm_ac_rel.ac_id);
    	}	
    	/* end add by gjf 20141119*/
    	
    	/*��Ʒ����*/
		count=sql_count("fn_reg",cWherelist);
		set_zd_int("0450",count);
		vtcp_log("%s %d::��Ʒ����count=%d",__FILE__,__LINE__,count);
	    
	    ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
	    if(ret)
	    {
	        vtcp_log("������ƵǼǲ��α����");
	        strcpy(g_pub_tx.reply,"D101");
	        goto ErrExit;
	    }
	    while(1)
	    {
	        memset(&sFn_parm,0x00,sizeof(sFn_parm));
	        memset(&sFn_reg,0x00,sizeof(sFn_reg));
	        memset(cSts,0x00,sizeof(cSts));
	        memset(cTerm_type,0x00,sizeof(cTerm_type));
	        
	        ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
	        if(ret != 100 && ret != 0)
	        {
	            vtcp_log("ȡ��ƵǼǲ��α����");
	            strcpy(g_pub_tx.reply,"D330");
	             Fn_reg_Clo_Sel();
	            goto ErrExit;
	        }
	        else if(ret == 100 || ret == 1403)
	     	{
	     		break;
	        }
	        else
	        {
		       	ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
		        if(ret)
		        {
		            vtcp_log("��ѯ��Ʋ�Ʒ��������");
		            strcpy(g_pub_tx.reply,"D324");
		             Fn_reg_Clo_Sel();
		            goto ErrExit;
		        }
		        if(sFn_parm.cancel_flag[0] == 'Y' && sFn_parm.stat[0] == '0')
		        {
			    	if(num >= start && (num-start) < sum)
					{
		        		fprintf(fp,"%24s|%ld|%9ld|%5s|%20s|%20s|%9ld|%16.2f|%16.2f|%16.2f|%9.2f|%9.2f|%16.2f|%ld|%ld|%9ld|%16.2lf|%16.2f|%s|%20s|%s|%ld|%9ld|%ld|%9ld|%60s|%s|%s|%ld|%9.2f|%s|\n",\
			        		sFn_reg.ac_no,sFn_reg.tx_date,sFn_reg.trace_no,sFn_reg.tx_br_no,sFn_reg.pact_no,\
			       			sFn_reg.prdt_code,sFn_reg.count,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
			        		sFn_reg.return_capital,sFn_reg.flt_radio,sFn_reg.return_rate,sFn_reg.intst,\
			        		sFn_parm.ic_date,sFn_parm.mtr_date,sFn_reg.date1,sFn_reg.fct_intst,sFn_reg.profit_amt,\
			        		sFn_reg.profit_flag[0] == 'Y' ?"Y":"N",sFn_reg.brf,sFn_reg.sts,sFn_reg.pay_date,\
			        		sFn_reg.pay_trace_no,sFn_reg.return_date,sFn_reg.return_trace_no,\
			        		sFn_parm.title,sFn_parm.redempt[0] == 'Y' ? "Y":"N",sFn_parm.cancel_flag[0] == 'Y' ?"Y":"N",sFn_parm.term,sFn_parm.rate,sFn_parm.risk_lvl);
		        			num2++;
		       	 	}
		       	 	if((num-start) >= sum)
	        		{
	        			break;
	        		}   
	        		++num;
		       	 } 	
		     }
	   	}
    	Fn_reg_Clo_Sel();
 	}
    fclose(fp);
    fp = NULL;  
    int iRt = -1;
    char cFtpfile[256];
    memset(cFtpfile,0x00,sizeof(cFtpfile));
	set_zd_data("0260",cFilename);
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),\
		get_env_info("NSW_FTP_PWD"),cFilename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	iRt=system(cFtpfile);
	if (iRt)
	{
		vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨����[%d]", __FILE__, __LINE__,cFilename,iRt);
		strcpy(g_pub_tx.reply,"9999");
		goto ErrExit;
	}	
	vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨�ɹ�", __FILE__, __LINE__,cFilename);   
OkExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp=NULL;	
	}
	set_zd_data("0130","��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣ�ɹ�");
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣ�ɹ�[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp=NULL;	
	}
    strcpy(g_pub_tx.reply,"9999");
	set_zd_data("0130","��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣʧ��");
    vtcp_log("��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣʧ��[%s]", g_pub_tx.reply);
    return 1;
}
/*����֤�������ѯ*/
int tx_0002()
{
		int 			ret = 0;
		char 			cAc_no[32];
		char 			cSts[12];
    char 			cCertType[2];
    char 			cCertNo[21];
    char 			cFlag[2];/*�Ƿ��Ϲ�������־*/
    FILE 			*fp  = NULL;
    char 			cFilename[256];
    char      pcfilename[100];
    char 			cWherelist[256];
    char 			cTerm_type[3];
    char 			begin[4];
    char 			total[4];
    char      tx_date[8+1];
		char      sw_traceno[10+1];
		char 			Num[12];
    int 			num=1;
    int 			num2=0;
    struct 			fn_parm_c sFn_parm;
    struct 			fn_reg_c sFn_reg;
    struct 			mdm_ac_rel_c sMdm_ac_rel;
    int		count=0;
    
    /*add gjf 20141119*/
	char 		sDateMark[1+1];		/*���ڱ�־*/
	char 		sbeginDate[8+1];	/*��ʼ����*/
	char		sendDate[8+1];		/*��������*/
	struct mdm_ac_rel_c mdm_ac_rel;/*�������˻���ϵ*/
	memset(&mdm_ac_rel,0x00,sizeof(mdm_ac_rel));
	memset(sDateMark,0x00,sizeof(sDateMark));
	memset(sbeginDate,0x00,sizeof(sbeginDate));
	memset(sendDate,0x00,sizeof(sendDate));
	
	get_zd_data("0660",sDateMark);
	get_zd_data("0460",sbeginDate);
	get_zd_data("0470",sendDate);
	
	long beginDate=0;
	long endDate=0;
	
	beginDate = atol(sbeginDate);
	endDate   = atol(sendDate);
/*add gjf 20141119*/

    memset(cFilename,0x00,sizeof(cFilename));
    memset(pcfilename, 0x00, sizeof(pcfilename));
    memset(cWherelist,0x00,sizeof(cWherelist));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cCertType,0x00,sizeof(cCertType));
    memset(cCertNo,0x00,sizeof(cCertNo));
    memset(cFlag,0x00,sizeof(cFlag));
    memset(cTerm_type,0x00,sizeof(cTerm_type));
    memset(begin,0x00,sizeof(begin));
    memset(total,0x00,sizeof(total));
    memset(Num,0x00,sizeof(Num));
    memset(tx_date,0x00,sizeof(tx_date));
    memset(sw_traceno,0x00,sizeof(sw_traceno));

    get_zd_data("0670",cCertType);		/*֤������*/
    get_zd_data("0620",cCertNo);		/*֤������*/
    get_zd_data("0680",cFlag);			/*�Ƿ��Ϲ�����*/
    get_zd_data("0500",begin);			/*��ѯ��ʼ��¼��*/
    get_zd_data("0510",total);			/*�����¼��*/
    get_zd_data("0050",tx_date);
		get_zd_data("0520",sw_traceno);

    int start=atoi(begin);
    int sum=atoi(total);
    if(sum == 0)
    {
    	sum = 20;
    }
    sprintf( cFilename,"5056netbank%s%s",tx_date,sw_traceno );
    sprintf( pcfilename,"%s/%s",getenv("FILDIR"),cFilename );
    fp=fopen(pcfilename,"w");
    if(fp == NULL)
    {
        vtcp_log("���ļ�ʧ��[%s]",pcfilename);
        strcpy(g_pub_tx.reply,"S066");
        goto ErrExit;
    }
    fprintf(fp,"~�˺�|��������|������ˮ��|���׻���|Э����|��Ʒ����|�Ϲ�����|�Ϲ����|��ؽ��|���ر���|���ʸ�������|������������|Ԥ������|��Ϣ����|��������|��������|����|������|�����־|����ժҪ|״̬|�ۿ�����|�ۿ���ˮ��|������������|����������ˮ��|��Ʒ����|�Ƿ������|�Ƿ��ܳ���|����|Ԥ���껯������|���յȼ�|\n");
	vtcp_log("%s,%d,ʱ���־[%s]",__FILE__,__LINE__,sDateMark);
    if(!memcmp(cFlag,"0",1))
    {
    	ret = Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"id_type='%s' and id_no='%s' order by ac_no desc ",cCertType,cCertNo);
    	if(ret)
    	{
       	 	strcpy(g_pub_tx.reply,"P084");
        	vtcp_log("�����ѯ�����˺Ź�ϵ����α����[%d]",ret);
        	goto ErrExit;
    	}
    	while(1)
    	{
       	 	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
        	ret = Mdm_ac_rel_Fet_Sel(&sMdm_ac_rel,g_pub_tx.reply);
        	if(ret != 0 && ret != 100)
        	{
            	strcpy(g_pub_tx.reply,"P088");
            	vtcp_log("�����ѯ�����˺Ź�ϵ����α����[%d]",ret);
            	Mdm_ac_rel_Clo_Sel();
            	goto ErrExit;
        	}
        	else if(ret == 100 || ret == 1403)
        	{
        		break;
        	}
			else
			{
		    	/* start add by gjf 20141119*/
		    	if(!memcmp(sDateMark,"0",1) && beginDate!=0 && endDate!=0)
		    	{
		    		sprintf(cWherelist,"ac_id =%ld and tx_date between '%s' and '%s' order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id,sbeginDate,sendDate);
		    	}
		    	else if(!memcmp(sDateMark,"1",1) && beginDate!=0 && endDate!=0)
		    	{
		    		sprintf(cWherelist,"ac_id =%ld and end_date between '%s' and '%s' order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id,sbeginDate,sendDate);
		    	}
		    	else
		    	{
		    		sprintf(cWherelist,"ac_id =%ld order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id);
		    	}	
		    	/* end add by gjf 20141119*/
		    	
		    	/*��Ʒ����*/
				count=sql_count("fn_reg",cWherelist);
				set_zd_int("0450",count);
				vtcp_log("%s %d::��Ʒ����count=%d",__FILE__,__LINE__,count);
		    	
		    	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
		    	if(ret)
		    	{
		        	vtcp_log("������ƵǼǲ��α����");
		        	strcpy(g_pub_tx.reply,"D101");
		        	Mdm_ac_rel_Clo_Sel();
		        	goto ErrExit;
		    	}
		    	while(1)
		    	{
		        	memset(&sFn_parm,0x00,sizeof(sFn_parm));
		        	memset(&sFn_reg,0x00,sizeof(sFn_reg));
		        	memset(cSts,0x00,sizeof(cSts));
		        	memset(cTerm_type,0x00,sizeof(cTerm_type));
		        
		        	ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
					if(ret != 100 && ret != 0)
		        	{
		           		vtcp_log("ȡ��ƵǼǲ��α����");
		          		strcpy(g_pub_tx.reply,"D330");
		          		Mdm_ac_rel_Clo_Sel();
		          		Fn_reg_Clo_Sel();
		            	goto ErrExit;
		        	}
					else if(ret == 100 || ret == 1403)
					{
						break;
					}
					else
					{
			        	ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
			        	if(ret)
			        	{
			            	vtcp_log("��ѯ��Ʋ�Ʒ��������");
			            	strcpy(g_pub_tx.reply,"D324");
			            	Mdm_ac_rel_Clo_Sel();
		          		Fn_reg_Clo_Sel();
			            	goto ErrExit;
			        	}
			    		if(num >= start && (num-start) < sum)
						{
		        			fprintf(fp,"%24s|%ld|%9ld|%5s|%20s|%20s|%9ld|%16.2f|%16.2f|%16.2f|%9.2f|%9.2f|%16.2f|%ld|%ld|%9ld|%16.2lf|%16.2f|%s|%20s|%s|%ld|%9ld|%ld|%9ld|%60s|%s|%s|%ld|%9.2f|%s|\n",\
			        		sFn_reg.ac_no,sFn_reg.tx_date,sFn_reg.trace_no,sFn_reg.tx_br_no,sFn_reg.pact_no,\
			       			sFn_reg.prdt_code,sFn_reg.count,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
			        		sFn_reg.return_capital,sFn_reg.flt_radio,sFn_reg.return_rate,sFn_reg.intst,\
			        		sFn_parm.ic_date,sFn_parm.mtr_date,sFn_reg.date1,sFn_reg.fct_intst,sFn_reg.profit_amt,\
			        		sFn_reg.profit_flag[0] == 'Y' ?"Y":"N",sFn_reg.brf,sFn_reg.sts,sFn_reg.pay_date,\
			        		sFn_reg.pay_trace_no,sFn_reg.return_date,sFn_reg.return_trace_no,\
			        		sFn_parm.title,sFn_parm.redempt[0] == 'Y' ? "Y":"N",sFn_parm.cancel_flag[0] == 'Y' ?"Y":"N",sFn_parm.term,sFn_parm.rate,sFn_parm.risk_lvl);
		        			num2++;
		       	 		}
		       	 	
		        		if((num-start) >= sum)
		        		{
		        			break;
		        		}
		        		++num;
		        	}
		   		}
	    		Fn_reg_Clo_Sel();	    		
			}
			if((num-start) >= sum)
	    	{
	    		break;
	    	}		
	    }
	    Mdm_ac_rel_Clo_Sel();
	}
    else
    {
    	ret = Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"id_type='%s' and id_no='%s' order by ac_no desc ",cCertType,cCertNo);
    	if(ret)
    	{
       	 	strcpy(g_pub_tx.reply,"P084");
        	vtcp_log("�����ѯ�����˺Ź�ϵ����α����[%d]",ret);
        	goto ErrExit;
    	}
    	while(1)
    	{
       	 	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
        	ret = Mdm_ac_rel_Fet_Sel(&sMdm_ac_rel,g_pub_tx.reply);
        	if(ret != 0 && ret != 100)
        	{
            	strcpy(g_pub_tx.reply,"P088");
            	vtcp_log("�����ѯ�����˺Ź�ϵ����α����[%d]",ret);
            	Mdm_ac_rel_Clo_Sel();
            	goto ErrExit;
        	}
        	else if(ret == 100 || ret == 1403)
        	{
        		break;
        	}
        	else
        	{
		    	if(!memcmp(sDateMark,"0",1) && beginDate!=0 && endDate!=0)
		    	{
		    		sprintf(cWherelist,"ac_id =%ld and sts = '0' and tx_date between '%s' and '%s' order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id,sbeginDate,sendDate);
		    	}
		    	else if(!memcmp(sDateMark,"1",1) && beginDate!=0 && endDate!=0)
		    	{
		    		sprintf(cWherelist,"ac_id =%ld and sts = '0' and end_date between '%s' and '%s' order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id,sbeginDate,sendDate);
		    	}
		    	else
		    	{
		    		sprintf(cWherelist,"ac_id =%ld and sts = '0' order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id);
		    	}
		    	
		    	/*��Ʒ����*/
				count=sql_count("fn_reg",cWherelist);
				set_zd_int("0450",count);
				vtcp_log("%s %d::��Ʒ����count=%d",__FILE__,__LINE__,count);
		    	
		    	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
		    	if(ret)
		    	{
		        	vtcp_log("������ƵǼǲ��α����");
		        	strcpy(g_pub_tx.reply,"D101");
		        	Mdm_ac_rel_Clo_Sel();
		        	goto ErrExit;
		    	}
		    	while(1)
		    	{
		       	 	memset(&sFn_parm,0x00,sizeof(sFn_parm));
		        	memset(&sFn_reg,0x00,sizeof(sFn_reg));
		        	memset(cSts,0x00,sizeof(cSts));
		        	memset(cTerm_type,0x00,sizeof(cTerm_type));
		        
		        	ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
					if(ret != 100 && ret != 0)
		        	{
		            	vtcp_log("ȡ��ƵǼǲ��α����");
		            	strcpy(g_pub_tx.reply,"D330");
		            	Mdm_ac_rel_Clo_Sel();
		          	Fn_reg_Clo_Sel();
		            	goto ErrExit;
		        	}
					else if(ret == 100 || ret == 1403)
					{
						break;
					}
					else
					{
			        	ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
			        	if(ret)
			        	{
			            	vtcp_log("��ѯ��Ʋ�Ʒ��������");
			            	strcpy(g_pub_tx.reply,"D324");
			            	Mdm_ac_rel_Clo_Sel();
		          		Fn_reg_Clo_Sel();
			            	goto ErrExit;
			        	}
			        	if(sFn_parm.cancel_flag[0] == 'Y' && sFn_parm.stat[0] == '0')
				    	{	
				    		if(num >= start && (num-start) < sum)
							{
			        			fprintf(fp,"%24s|%ld|%9ld|%5s|%20s|%20s|%9ld|%16.2f|%16.2f|%16.2f|%9.2f|%9.2f|%16.2f|%ld|%ld|%9ld|%16.2lf|%16.2f|%s|%20s|%s|%ld|%9ld|%ld|%9ld|%60s|%s|%s|%ld|%9.2f|%s|\n",\
				        		sFn_reg.ac_no,sFn_reg.tx_date,sFn_reg.trace_no,sFn_reg.tx_br_no,sFn_reg.pact_no,\
				       			sFn_reg.prdt_code,sFn_reg.count,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
				        		sFn_reg.return_capital,sFn_reg.flt_radio,sFn_reg.return_rate,sFn_reg.intst,\
				        		sFn_parm.ic_date,sFn_parm.mtr_date,sFn_reg.date1,sFn_reg.fct_intst,sFn_reg.profit_amt,\
				        		sFn_reg.profit_flag[0] == 'Y' ?"Y":"N",sFn_reg.brf,sFn_reg.sts,sFn_reg.pay_date,\
				        		sFn_reg.pay_trace_no,sFn_reg.return_date,sFn_reg.return_trace_no,\
				        		sFn_parm.title,sFn_parm.redempt[0] == 'Y' ? "Y":"N",sFn_parm.cancel_flag[0] == 'Y' ?"Y":"N",sFn_parm.term,sFn_parm.rate,sFn_parm.risk_lvl);
			        			num2++;
			       	 		}
			       	 		if((num-start) >= sum)
		        			{
		        				break;
		        			}  
		        			++num; 
			       	 	}
			       	}	
		   		}
		   		Fn_reg_Clo_Sel();
		   	}
		   	if((num-start) >= sum)
        	{
        		break;
        	}
		}
		Mdm_ac_rel_Clo_Sel();
    } 
    fclose(fp);
    fp = NULL;
    int iRt = -1;
    char cFtpfile[1024];
    memset(cFtpfile,0x00,sizeof(cFtpfile));
	set_zd_data("0260",cFilename);
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),\
		get_env_info("NSW_FTP_PWD"),cFilename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	iRt=system(cFtpfile);
	if (iRt)
	{
		vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨����[%d]", __FILE__, __LINE__,cFilename,iRt);
		strcpy(g_pub_tx.reply,"9999");
		goto ErrExit;
	}
	vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨�ɹ�", __FILE__, __LINE__,cFilename);
OkExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp=NULL;	
	}
	set_zd_data("0130","��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣ�ɹ�");
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣ�ɹ�[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp=NULL;	
	}
	strcpy(g_pub_tx.reply,"9999");
	set_zd_data("0130","��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣʧ��");
    vtcp_log("��ѯ�˺Ŷ�Ӧ����Ʋ�Ʒ��Ϣʧ��[%s]", g_pub_tx.reply);
    return 1;
}


/*************************************************
* �� �� ��: spC105.c
* �����������ͻ���ϵ��ʽ��Ϣ¼��
* ��    ��: andy
* ������ڣ�2004��01��15��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_email_inf_c.h"
#include "cif_basic_inf_c.h"

static struct cif_email_inf_c   cif_email_inf;
static struct cif_email_inf_c   g_cif_email_inf;
static struct cif_basic_inf_c   cif_basic_inf;  

spC105()
{
	int i=0;
 	int ret=0;
 	long cif_no;

    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();

    /** ���ݳ�ʼ�� **/
    memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
    memset (&g_cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
    memset (&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));

    /** ȡֵ����ֵ **/
   	get_zd_data("0670",cif_email_inf.addr_type);   		/*��ϵ��ʽ*/
	get_zd_long("0290",&cif_email_inf.cif_no);     		/*�ͻ�ID*/
    get_zd_data("0250",cif_email_inf.email);       		/*���ӵ�ַ*/
    get_zd_data("0270",cif_email_inf.link_man);    		/*��ϵ��*/
    get_zd_long("0480",&cif_email_inf.addr_seqn);   		/*��ַ���*/

    /** �������� **/
    /* ���ͻ����Ƿ��Ѿ����� */
    ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf ,
           					" cif_no=%ld " , cif_email_inf.cif_no );
    if( ret==100 )
    {
       	sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]",g_pub_tx.reply);
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"C007");
       	ERR_DEAL;
    }
    else if( ret )
    {
       	sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"C006");
       	ERR_DEAL;
    }

	/*����������ϵ��ʽ�����е���ϵ��ϸ�Ƿ��ظ�*/
	ret=Cif_email_inf_Dec_Sel(g_pub_tx.reply,"cif_no=%ld",cif_email_inf.cif_no);
    if( ret )
    {
       	sprintf(acErrMsg,"DECLARE Cif_email_inf_Dec_Sel�쳣![%s]",
				g_pub_tx.reply);
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"C039");
       	ERR_DEAL;
    }

	while(1)
	{
       	sprintf(acErrMsg,"cif_email_inf.cif_no!!==[%ld]",cif_email_inf.cif_no);
       	WRITEMSG
		ret =Cif_email_inf_Fet_Sel(&g_cif_email_inf,g_pub_tx.reply);
    	if( ret && ret!=100 )
    	{
       		sprintf(acErrMsg,"FETCH Cif_email_inf_Dec_Sel�α��쳣![%s]",
					g_pub_tx.reply);
       		WRITEMSG
       		strcpy(g_pub_tx.reply,"C038");
       		ERR_DEAL;
    	}else if(ret==100)
		{
			TRACE
			break;
		}
		TRACE

		i++;
		if(g_cif_email_inf.addr_type[0]!=cif_email_inf.addr_type[0])continue;
		sprintf(acErrMsg,"!!!!!!![%s][%s]", g_cif_email_inf.addr_type,cif_email_inf.addr_type);
	 	WRITEMSG

		pub_base_strpack(g_cif_email_inf.email);	/*�ո�ѹ��*/
		switch(atoi(cif_email_inf.addr_type))
		{
		TRACE
			case 1:									/*סլ�绰*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"�ÿͻ��Ѵ��ڴ�סլ�绰!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C032");
       					ERR_DEAL;
					}
			case 2:									/*��˾�绰*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"�ÿͻ��Ѵ��ڴ˹�˾�绰!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C033");
       					ERR_DEAL;
					}
			case 3:									/*����*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"�ÿͻ��Ѵ��ڴ˴������!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C034");
       					ERR_DEAL;
					}
			case 4:									/*����*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"�ÿͻ��Ѵ��ڴ˴�������!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C035");
       					ERR_DEAL;
					}
			case 5:									/*�ֻ�*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"�ÿͻ��Ѵ��ڴ��ֻ�����!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C036");
       					ERR_DEAL;
					}
			case 6:									/*�����ʼ�*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"�ÿͻ��Ѵ��ڴ˵����ʼ�!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C037");
       					ERR_DEAL;
					}
			default:
					break;
		}
		TRACE
	}

	Cif_email_inf_Clo_Sel ();
    sprintf(acErrMsg,"!11111!!!![%d]",i);
	WRITEMSG


   	/* �Ǽǿͻ����ӵ�ַ��Ϣ�� */
	if(i) 
	{
		cif_email_inf.addr_seqn=g_cif_email_inf.addr_seqn+1;
		TRACE
	}else{ 
		TRACE
		cif_email_inf.addr_seqn=1;
	}
    ret=Cif_email_inf_Ins(cif_email_inf,g_pub_tx.reply);
    if( ret && ret!=-239 )
    {
     	sprintf(acErrMsg,"�Ǽǿͻ����ӵ�ַ��Ϣ��ʧ��![%d]",ret);         
        WRITEMSG
        strcpy(g_pub_tx.reply,"C024");
        goto ErrExit;
    }else if( ret ==-239 ){
		sprintf(acErrMsg,"�Ǽǿͻ����ӵ�ַ��Ϣ�����ظ���¼![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C089");
		goto ErrExit;
	}


    /* �Ǽǽ�����ˮ */
 	if( pub_ins_trace_log() )
 	{
 		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }
 vtcp_log("arrrrrrrrrrrr");
    set_zd_long("0480",cif_email_inf.addr_seqn);   		/*��ַ���*/
    
    vtcp_log("aaaaaaaaaaaaa");
		  /* add begin �����ͻ���Ϣͬ�� lzy 20141216 */
			char today[9]={0};
			/*char nextday[10]={0};*/
			long nextday = 0;
			char cifno[9]={0};
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(cif_email_inf.cif_no,8,cifno);
			apitoa(g_pub_tx.tx_date,8,today);
			/*memcpy(nextday,pub_base_daynumLAT(today,1),8);*/
			nextday = pub_base_daynumLAT(today,1);
			 vtcp_log("bbbbbbbbbbb");

			if ( cifno[0] == '1' )
				{
					sprintf( filename,"PersCustSyn%ld.txt",nextday);
					 vtcp_log("cccccccccccccc");
				}
			else if ( cifno[0] == '5' )
				{
					sprintf( filename,"CorpCustSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::�ͻ���[%s],����Ҫ������ͬ��.",__FILE__,__LINE__,cifno);
					goto OkExit;
				}
			
			sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
			vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
			fp=fopen(pcfilename,"at");
			if( fp==NULL )
			{
				vtcp_log("[%s],[%d] �������ļ�[%s]",__LINE__,__FILE__,pcfilename);
				fp = fopen(pcfilename, "wt");
				if (fp == NULL)
				{
					vtcp_log("[%s],[%d] open file[%s] error",__LINE__,__FILE__,pcfilename);
					strcpy(g_pub_tx.reply,"SM83");
					goto ErrExit;
				}
			}
 vtcp_log("ddddddddddddddd");
			if ( cifno[0] == '1' )  /**���˿ͻ�**/
			{
					if ( g_cif_email_inf.addr_type[0] == '1' ) /**�绰**/
					{/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,�ֻ�,����,�Ա�\n");**/
						fprintf(fp, "%ld,,,,,,%s,,,\n",cif_email_inf.cif_no,cif_email_inf.email);
					}
					else if (  g_cif_email_inf.addr_type[0] == '5' )  /**�ֻ�**/
					{
						fprintf(fp, "%ld,,,,,,,%s,,\n",cif_email_inf.cif_no,cif_email_inf.email);
					}
					else if ( g_cif_email_inf.addr_type[0] == '6' )  /**����**/
					{
						fprintf(fp, "%ld,,,,,,,,%s,\n",cif_email_inf.cif_no,cif_email_inf.email);
					}
					else
					{
						vtcp_log("%s %d::����Ҫ������ͬ��.",__FILE__,__LINE__);
						fclose(fp);
						goto OkExit;
					}
			}
			else if (cifno[0] == '5' && g_cif_email_inf.addr_type[0] == '2' )  /**��˾�ͻ��绰**/
			{/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,���˴���\n");**/
				fprintf(fp, "%ld,,,,,,%s,\n",cif_email_inf.cif_no,cif_email_inf.email);
			}
			else
			{
				vtcp_log("%s %d::����Ҫ������ͬ��.",__FILE__,__LINE__);
				fclose(fp);
				goto OkExit;
			}
			fclose(fp);
				
			sprintf(cFtpfile,"%s/bin/ftp_to_wyzt.sh %s",getenv("WORKDIR"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
				{
					vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨����[%d]", __FILE__, __LINE__,filename,ret);
					strcpy(g_pub_tx.reply,"S047");
					goto ErrExit;
				}	
			vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨�ɹ�", __FILE__, __LINE__,filename);				
		  /* add end �����ͻ���Ϣͬ�� lzy 20141216 */
     vtcp_log("aeeeeeeeeeeeeee");
    

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    WRITEMSG
    sprintf(acErrMsg,"�ͻ���ϵ��ʽ��Ϣ¼��ɹ�![%s]",g_pub_tx.reply);
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�ͻ���ϵ��ʽ��Ϣ¼��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

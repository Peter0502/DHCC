/***** �������ռ��Ϣ�嵥��ѯ(sp5347)��������**********/
/*****************************************************
* �� �� ��:  sp5887.c
* ���������� ͸֧��Ϣ��ѯ
* ��    ��:  wangyh
* ������ڣ� 2006��8��29�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "hv_zf_c.h"
#include "hv_addit_c.h"
int sp5887()
{
	int ret=0,i;
	int j;
	char k[5];
	char tx_date[9];
	char cmtno[4];
	char cBuff[2001];
	char cPrtname[5];
	char cPrtname1[5];
	char cFilename[100];/*�ļ���*/
	struct  hv_zf_c       f_hv_zf;
	struct  hv_addit_c    f_hv_addit;
  	HV_803_MAIN_ADD fd124;
	FILE *fp;
	memset(tx_date,0,sizeof(tx_date));
	memset(cmtno,0,sizeof(cmtno));
	memset(&fd124, ' ',sizeof(fd124));
	memset(cBuff, 0 , sizeof(cBuff));
	memset(k,0,sizeof(k));
	memset(&f_hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(&f_hv_addit,0x00,sizeof(struct hv_addit_c));
	memset(cPrtname1,0,sizeof(cPrtname1));
	vtcp_log("sp5887 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	pub_base_sysinit();
	get_zd_data("0440",tx_date);
	vtcp_log("[%s][%d]tx_date=[%s]",__FILE__,__LINE__,tx_date);	
	vtcp_log("sp5887 ��ʼ��[%s][%d]",__FILE__,__LINE__);	
	ret=Hv_zf_Dec_Sel ( g_pub_tx.reply,"(tx_date='%s' or wt_date='%s' ) and cmtno='803'",tx_date,tx_date);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴�͸֧��Ϣ�嵥");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ͸֧��Ϣ�嵥��ϸ������Ϣ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}		
	vtcp_log(" f_hv_zf.receco=[%s] f_hv_zf.tx_amt=[%f]",f_hv_zf.receco,f_hv_zf.tx_amt);
	/*memcpy(fd124.receco, f_hv_zf.receco, sizeof(fd124.receco)); 
	vtcp_log("fd124.receco=[%s]",fd124.receco);	*/
		
	i=0;
	while(1)
	{
	    
		  vtcp_log("%s,%d �õ��ĸ�����hahaha",__FILE__,__LINE__);  
	  	ret=Hv_zf_Fet_Sel (&f_hv_zf,g_pub_tx.reply);
		  vtcp_log("%s,%d ret=[%d]",__FILE__,__LINE__,ret);  
	  	if( ret==100 ) 
        	{
        		vtcp_log("%s,%d �õ��ĸ�����hahaha",__FILE__,__LINE__);  
        		break;
        	}
		 ERR_DEAL
		 vtcp_log("%s,%d �õ��ĸ�����hahaha[%s]",f_hv_zf.addid,__FILE__,__LINE__); 
		  
		 if(memcmp(f_hv_zf.receco,HV_HD,sizeof(f_hv_zf.receco)-1) && memcmp(f_hv_zf.receco,HV_GKZJHH,sizeof(f_hv_zf.receco)-1))
	      	{
		       ret= Hv_addit_Dec_Sel(g_pub_tx.reply,"addid='%.8s' and tx_date=%d order by addid",f_hv_zf.addid,apatoi(tx_date,8));
					ERR_DEAL
					j=0;
					while(1)
					{
						    memset(&fd124,0x00,sizeof(fd124));
		  					vtcp_log("%s,%d �õ��ĸ�����hahaha",__FILE__,__LINE__);  
	  						ret=Hv_addit_Fet_Sel(&f_hv_addit,g_pub_tx.reply);
								if ( ret==100 )
	        			{
	        				break;
	        			}
	        				else if ( ret )
	        			{
	        					sprintf( acErrMsg,"���Ҹ������ݱ����![%s][%d][%d]",__FILE__,__LINE__,ret);
	        					set_zd_data(DC_GET_MSG,"���Ҹ������ݱ����");
	        					WRITEMSG
	          				strcpy(g_pub_tx.reply,"D103");
										goto ErrExit;
	        			}	
	      			if( !j && !i)
	  	 				{
		      					vtcp_log("%s,%d �õ��ĸ�����hahaha",__FILE__,__LINE__);    		
			    					pub_base_AllDwnFilName(cFilename);
			    					fp = fopen(cFilename,"w" );
			    					if( fp==NULL )
			       				{
				        				sprintf(acErrMsg," open file [%s] error ",cFilename);
				        				WRITEMSG
				        				strcpy( g_pub_tx.reply,"S047" );
				        				goto ErrExit;
			       				}
			    						fprintf( fp,"~���|��Ϣʱ��|͸֧���|������|͸֧��Ϣ|\n" );
		   				}
		   					memcpy(&fd124,f_hv_addit.tagdata,sizeof(fd124)-1);
		      		  vtcp_log("[%s],%s,%d �õ��ĸ�����hahaha",f_hv_addit.tagdata,__FILE__,__LINE__);    		
		   					
		   				  pub_base_full_space((char*)&fd124,sizeof(fd124));
	      		    fprintf( fp,"%.3s|%.6s|%.22s|%.6s|%.22s|\n",f_hv_addit.tagname,fd124.tag_CLD[0].rate_time,fd124.tag_CLD[0].over_amt,fd124.tag_CLD[0].rate,fd124.tag_CLD[0].over_intr);	          		
             			j++;
	 		 }	 
	 	i++;	 	 	          	    
       		}
  }
	ret=Hv_zf_Clo_Sel();
	ERR_DEAL
	set_zd_data( DC_FILE_SND,"2");
	if(!i)
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{				
		fclose(fp);
	}	
 

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

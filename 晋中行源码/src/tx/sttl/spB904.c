/*************************************************
* �� �� ��:  spB904.c
* ���������� ͬ��Ʊ�����㵥��ӡ
*
* ��    ��:  liuxuan 
* ������ڣ� 2006-8-19
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "mo_city_c.h"                                         
#include "accountreg_c.h" 
#include "changetab_c.h"                                      	
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char titstr[1024];
		char tmpstr[512];

	  char cBr_no[6];
		int  ttlnum=0;			/**��ȡ��������**/
		int  i=0;
		int  ret=0;
		FILE *fp;
		double z_accountreg_amt=0;
struct accountreg_c accountreg;
struct changetab_c changetab;
int spB904()
{
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset(cBr_no,0x0,sizeof(cBr_no));
	memset(&accountreg,'\0',sizeof(accountreg));	
	memset(&changetab,'\0',sizeof(changetab));	
	pub_base_sysinit();

	get_zd_long("0460",&accountreg.tx_date);	   /* ��������*/
	get_zd_data("0880",accountreg.cash_chg_no);    /* ������ */
	get_zd_data("0030",cBr_no);
z_accountreg_amt=0;
	ret = get_ac_hst();
	if( ret ) goto ErrExit;

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



int get_ac_hst()
{
	char tmp_no[64];
	char o_put_flag[2];
	char o_change_no[11];
	char o_cash_chg_no[11];
	double hj_bal = 0;

	/**��ɲ�ѯ����**/

  memset(o_put_flag,0,sizeof(o_put_flag));
  memset(o_change_no,0,sizeof(o_change_no));
  memset(o_cash_chg_no,0,sizeof(o_cash_chg_no));
	memset( wherelist,0,sizeof(wherelist)); 
	pub_base_strpack(accountreg.cash_chg_no);
	if( strlen(accountreg.cash_chg_no) )
	{
		sprintf( tmpstr," cash_chg_no='%s' and",accountreg.cash_chg_no );
		strcat( wherelist,tmpstr );                                                 
	}
	/**pub_base_strpack(accountreg.tx_date);**/
	if( accountreg.tx_date )
	{
		sprintf( tmpstr," tx_date=%ld and",accountreg.tx_date );
		strcat( wherelist,tmpstr );
	}
	 strcat(wherelist," stat!='9' order by put_flag,change_no,cash_chg_no");
	/**��ɲ�ѯ**/
	vtcp_log("��ѯ���� wherelist =[%s]",wherelist);
	ret=Accountreg_Dec_Sel( g_pub_tx.reply,wherelist);
	vtcp_log("ret =[%d]",ret);	
	if(ret)
			{
	     	sprintf( acErrMsg, "sql error" );
		    WRITEMSG 
		    return 1;
			}


	ttlnum=0;

	while( 1 )
	{		
	  memset(&accountreg,'\0',sizeof(accountreg));		  	
		ret = Accountreg_Fet_Sel( &accountreg, g_pub_tx.reply );
	  vtcp_log("accountreg.tx_date=[%d]",accountreg.tx_date);
	  vtcp_log("ret =[%d]",ret);

		if(ret == 100) /*��ȡ����*/
		{     if( ttlnum!=0 )
			    {
			       if  ( o_put_flag[0] =='1')
             {
             	  fprintf( fp," �������������������������������㩥���������������������������㩥���������۩��������������㩥�����������۩���������\n" );
				      	fprintf( fp," ��     �ϼ�                   ��  %16.2lf          ��     �ϼ�                 ��                      �� \n" ,z_accountreg_amt);       
				      	fprintf( fp," �������������������������������۩����������������������������۩��������������������������۩�����������������������\n" );			
				      	fprintf( fp,"           ���ˣ�              ��ƣ�              ���ɣ�              ���ˣ�                  �Ʊ�               " );
				      	fprintf( fp," \n" );
                fprintf( fp," \n" );
             }else{
                fprintf( fp," �����������������㩥�������������۩��������㩥�������������������������㩥�������������������������۩�����������\n" );
				      	fprintf( fp," ��     �ϼ�     ��                        ��     �ϼ�                 ��    %16.2lf                  �� \n" ,z_accountreg_amt);       
				      	fprintf( fp," �����������������۩������������������������۩��������������������������۩���������������������������������������\n" );			
				        fprintf( fp,"           ���ˣ�              ��ƣ�              ���ɣ�              ���ˣ�                  �Ʊ�               " );				      	
				      	fprintf( fp," \n" );
                fprintf( fp," \n" );
             }
          }
			  return 0;
		}	  	   		
		else if(ret)
			{
	     	sprintf( acErrMsg, "sql error" );
		    WRITEMSG 
		    return 1;
			}
		 
   	
		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				return 1;
			}  	
		  	
     	} 
      ret=Changetab_Sel(g_pub_tx.reply,&changetab,"change_no='%s'",accountreg.change_no);
     if(ret)
			{
	     	sprintf( acErrMsg, "sql error" );
		    WRITEMSG 
		    return 1;
			}

			if(!memcmp(changetab.br_no,"51801",5)||!memcmp(changetab.br_no,"51701",5)||!memcmp(changetab.br_no,"51601",5)||!memcmp(changetab.br_no,"52001",5)||!memcmp(changetab.br_no,"50401",5)||!memcmp(changetab.br_no,"51901",5))
      {
      	memcpy(accountreg.br_name,"���г���֧��",12);
      	memcpy(accountreg.change_no,"613",3);
      }

      else
      {
      	memcpy(accountreg.br_name,changetab.br_name,sizeof(accountreg.br_name));
      }

      if(memcmp(accountreg.br_no,cBr_no,5)!=0)
      {
      	continue;
      }
      
      if(memcmp(accountreg.put_flag,o_put_flag,sizeof(accountreg.put_flag)-1) ||\
      	 memcmp(accountreg.change_no,o_change_no,sizeof(accountreg.change_no)-1) ||\
      	 memcmp(accountreg.cash_chg_no,o_cash_chg_no,sizeof(accountreg.cash_chg_no)-1)      	 
      	)
      {
       if( ttlnum!=0 )
       {
          if  ( o_put_flag[0] =='1')
          {
          	fprintf( fp," �������������������������������㩥���������������������������㩥���������۩��������������㩥�����������۩���������\n" );
				   	fprintf( fp," ��     �ϼ�                   ��  %16.2lf          ��     �ϼ�                 ��                      �� \n" ,z_accountreg_amt);       
				   	fprintf( fp," �������������������������������۩����������������������������۩��������������������������۩�����������������������\n" );			
				   	
				   	fprintf( fp," \n" );
             fprintf( fp," \n" );
          }else{
            fprintf( fp," �����������������㩥�������������۩��������㩥�������������������������㩥�������������������������۩�����������\n" );
				   	fprintf( fp," ��     �ϼ�     ��                        ��     �ϼ�                 ��    %16.2lf                  �� \n" ,z_accountreg_amt);       
				   	fprintf( fp," �����������������۩������������������������۩��������������������������۩���������������������������������������\n" );			
				   	fprintf( fp," \n" );
            fprintf( fp," \n" );
          }
       }
       z_accountreg_amt=0;
       if  ( accountreg.put_flag[0] =='1')
       {
         fprintf( fp,"^[@^[3^P^\v1\n");
          fprintf( fp," \n" );
					fprintf( fp,"                                    %sƱ���嵥�Ǽǿ� \n" , BANK_CNAME);
					fprintf( fp," ������������������������������������������������������������������������������������������������������������������\n" );
					fprintf( fp," ��                                     ����ʱ��     %8d                                                    ��\n",accountreg.tx_date);      
					fprintf( fp," ���������������������������ө������������������������������������ө��������������������ө�������������������������\n" );										
					fprintf( fp," ��     ����д���         ��    %4s                            ��     �����д���     ��       %4s             ��  \n","613",accountreg.cash_chg_no );
					fprintf( fp," ���������������������������㩥�����������������������������������㩥�������������������㩥������������������������\n" );
					fprintf( fp," ��     ���������         ��%16s                    ��     ����������     �� %16s       ��\n","���г���֧��",accountreg.cash_opn_name);
					fprintf( fp," ���������������������������۩������������������������������������۩������ө������������۩�������������������������\n" );
					fprintf( fp," ��                            ����������                                ��            �跽������                ��\n"  );     
					fprintf( fp," �������������������������������ө����������������������������ө����������㩥�������������ө������������ө���������\n" );
					fprintf( fp," ��  �տλ�ʺ�              ��  ���λ�ʺ�              ��   ���   ��  �տλ�ʺũ����λ�ʺũ�   ��� �� \n" );     
       }
     else 
       {
         fprintf( fp,"^[@^[3^P^\v1\n");
          fprintf( fp," \n" );
					fprintf( fp,"                                    %sƱ���嵥�Ǽǿ� \n", BANK_CNAME );
					fprintf( fp," ����������������������������������������������������������������������������������������������������������������\n" );
					fprintf( fp," ��                                     ����ʱ��     %8d                                                  ��\n",accountreg.tx_date);      
					fprintf( fp," �����������������������ө��������������������������������ө��������������������ө�������������������������������\n" );										
					fprintf( fp," ��     ����д���     ��    %4s                        ��     �����д���     ��       %4s                   ��  \n",accountreg.change_no,accountreg.cash_chg_no	);
					fprintf( fp," �����������������������㩥�������������������������������㩥�������������������㩥������������������������������\n" );
					fprintf( fp," ��     ���������     ��%16s                ��     ����������     �� %16s             ��\n",accountreg.cash_opn_name,accountreg.br_name);
					fprintf( fp," �����������������������۩������������������ө������������۩��������������������۩�������������������������������\n" );
					fprintf( fp," ��           ����������                   ��                         �跽������                               ��\n"  );     
					fprintf( fp," �����������������ө��������������ө��������㩥�������������������������ө��������������������������ө�����������\n" );
					fprintf( fp," �� �տλ�ʺ� �� ���λ�ʺ� ��  ���  ��       �տλ�ʺ�       ��       ���λ�ʺ�       ��   ���   �� \n" );     
         }
        
         memset(o_put_flag,0,sizeof(o_put_flag));
         memset(o_change_no,0,sizeof(o_change_no));
         memset(o_cash_chg_no,0,sizeof(o_cash_chg_no));
         memcpy(o_put_flag,accountreg.put_flag,sizeof(accountreg.put_flag)-1);
      	 memcpy(o_change_no,accountreg.change_no,sizeof(accountreg.change_no)-1) ;
      	 memcpy(o_cash_chg_no,accountreg.cash_chg_no,sizeof(accountreg.cash_chg_no)-1) ;
      }
        z_accountreg_amt = z_accountreg_amt + accountreg.amt;
   /**
      
			fprintf( fp,
			"~@��������|����д���|���������|�������ʺ�|����������|������|�ϼ�|�����д���|����������|�տ����ʺ�|�տ�������|���|�ϼ�|\n");
		
		  fprintf( fp, "%d|%s|%s|%s|%s|%.2lf|%.2lf|%s|%s|%s|%s|%.2lf|%.2lf|\n",
			accountreg.tx_date,accountreg.change_no,accountreg.br_name,accountreg.pay_ac_no,accountreg.pay_name,accountreg.amt,z_accountreg_amt,\                
			accountreg.cash_opn_br_no,accountreg.cash_opn_name,accountreg.cash_ac_no,accountreg.cash_name,accountreg.amt,z_accountreg_amt);
   
     **/
    vtcp_log("!!!!!!!accountreg.put_flag=[%s]",accountreg.put_flag); 
     vtcp_log("!!!!!!!accountreg.br_name=[%s]",accountreg.br_name);
   	  memset(&changetab,'\0',sizeof(changetab));		  	


     
     if  ( accountreg.put_flag[0] =='1')
      {
          fprintf( fp," �������������������������������㩥���������������������������㩥���������㩥�������������㩥�����������㩥��������\n" );
					fprintf( fp," ��%26s  �� %26s ��%8.2lf  ��              ��            ��        ��\n",accountreg.cash_ac_no,accountreg.pay_ac_no,accountreg.amt);

          
       }
     else 
       {
          fprintf( fp," �����������������㩥�������������㩥�������㩥�������������������������㩥�������������������������㩥����������\n" );
					fprintf( fp," ��              ��              ��        ��%26s��%26s��%8.2lf  ��\n",accountreg.cash_ac_no,accountreg.pay_ac_no,accountreg.amt);          
         }
   
		ttlnum++;    
	}
	
	Accountreg_Clo_Sel();
       
	return 0;
}

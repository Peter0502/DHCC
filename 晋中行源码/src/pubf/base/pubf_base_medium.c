#include "find_debug.h" 
/*************************************************
* �� �� ��:  pubf_base_medium.c
* ����������
*           ���� ��Ʒ����ʹ�ϵУ�麯�� pub_base_check_promedi();
*           	 �趨���ʵ�֧ȡ��ʽ���� pub_base_set_drawtype();
*                У����ʵ�֧ȡ��ʽ���� pub_base_check_draw();
*                У����ʵ�֧ȡ��ʽ���� pub_base_check_drawtype();
*                ������״̬���� 		pub_base_check_medstate();
* ��    ��: rob
* ������ڣ�2003/12/24
* �޸ļ�¼��  
* 1. ��   ��:
*    �� �� ��:
*    �޸�����:
* 2. ...
*************************************************/

#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "mdm_prdt_rel_c.h"
#include "mdm_unprt_hst_c.h"
#include "com_chrg_hst_c.h"
#include "com_parm_c.h"

struct  mdm_prdt_rel_c g_mdm_prdt_rel;
struct  mdm_unprt_hst_c g_mdm_unprt_hst;     /*δ��ӡ��ϸ�ṹ*/
struct	com_parm_c sCom_parm;

static int ret=0;
/**********************************************************************
 * �� �� ����pub_base_check_promedi();
 * �������ܣ�����ʱ�����ݲ�Ʒ���������н��ʺ�����Ľ������
 *           ���Ƿ�����ڲ�Ʒ���ʹ�ϵ���С�
 * ����/ʱ�䣺
 * 
 * ������
 *     ���룺��Ʒ��š�		char(4)        (Ψһ)
 *           ���ʴ���1		char(4)
 *           ���ʴ���2		char(4)
 *           ���ʴ���3		char(4)		ע:�ս���,��ڲ���ΪNULL_MDM
 *     ���: 
 *           ��Ʒ���ʹ�ϵ�ṹ
 *   ����ֵ��
 *           ��Ӧ���� 
 *           ����0��ϵ���ڣ�
 *           ���ط�0��ϵ�����ڣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_base_check_promedi(char *prdt_num,char *mdm_num1,
							char *mdm_num2,char *mdm_num3)
{
	int count1=0;						/*������ڷǿս��ʸ���*/
	int count2=0;						/*�����α���ȡ���ʷǿո���*/
	int count3=0;						/*һ����¼��ƥ��ĸ���*/
	char flag2[3];						/*���ڼ�¼�����Ƿ�Ϊ�ձ�־*/
	int overflag=0;

	pub_base_strpack(mdm_num1);
	pub_base_strpack(mdm_num2);
	pub_base_strpack(mdm_num3);
	
	sprintf(acErrMsg," prdt_num [%s]",prdt_num);
	WRITEMSG
	sprintf(acErrMsg," MDM [%s][%s][%s]",mdm_num1,mdm_num2,mdm_num3);
	WRITEMSG

	overflag=0;
	count1=0;
	if ( strcmp(mdm_num1,NULL_MDM) )
		count1++;
	if ( strcmp(mdm_num2,NULL_MDM) )
		count1++;
	if ( strcmp(mdm_num3,NULL_MDM) )
		count1++;

	MEMSET_DEBUG( &g_mdm_prdt_rel, 0x00, sizeof(struct mdm_prdt_rel_c) );
	ret = Mdm_prdt_rel_Dec_Sel(g_pub_tx.reply, "prdt_no='%s'",prdt_num);
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Mdm_prdt_rel �α����[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D102" );
		return 1;
	}

	while(1)
	{
		if( overflag==1 ) break;
		ret = Mdm_prdt_rel_Fet_Sel( &g_mdm_prdt_rel, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch mdm_prdt_rel error code = [%d]",ret);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply ,"D103" );
	 		return 1;
 		}
		else if( ret==100 )
		{
			if( overflag==0 )
			{
				overflag=1;
				/**û�м�¼��ʾ�������޽���**/
				strcpy(g_mdm_prdt_rel.mdm_code1,NULL_MDM_CODE);
				strcpy(g_mdm_prdt_rel.mdm_code2,NULL_MDM);
				strcpy(g_mdm_prdt_rel.mdm_code3,NULL_MDM);
			}
			else
				break;
		}

		if( overflag==0 ) overflag=2;

		pub_base_strpack(g_mdm_prdt_rel.mdm_code1);
		pub_base_strpack(g_mdm_prdt_rel.mdm_code2);
		pub_base_strpack(g_mdm_prdt_rel.mdm_code3);

		count2=0;
		if ( strcmp(g_mdm_prdt_rel.mdm_code1,NULL_MDM) )
			count2++;
		if ( strcmp(g_mdm_prdt_rel.mdm_code2,NULL_MDM) )
			count2++;
		if ( strcmp(g_mdm_prdt_rel.mdm_code3,NULL_MDM) )
			count2++;
		
		if( count1!=count2 )  continue;

		strcpy(flag2,"000");

		if ( !strcmp(mdm_num1,NULL_MDM) )
			;
		else if ( !strcmp(mdm_num1,g_mdm_prdt_rel.mdm_code1) )
			flag2[0]='1';
		else if ( !strcmp(mdm_num1,g_mdm_prdt_rel.mdm_code2) )
			flag2[1]='1';
		else if ( !strcmp(mdm_num1,g_mdm_prdt_rel.mdm_code3) )
			flag2[2]='1';
		else
			continue;

		if ( !strcmp(mdm_num2,NULL_MDM) )
			;
		else if ( flag2[0]!='1' && !strcmp(mdm_num2,g_mdm_prdt_rel.mdm_code1) )
			flag2[0]='1';
		else if ( flag2[1]!='1' && !strcmp(mdm_num2,g_mdm_prdt_rel.mdm_code2) )
			flag2[1]='1';
		else if ( flag2[2]!='1' && !strcmp(mdm_num2,g_mdm_prdt_rel.mdm_code3) )
			flag2[2]='1';
		else
			continue;

		if ( !strcmp(mdm_num3,NULL_MDM) )
			;
		else if ( flag2[0]!='1' && !strcmp(mdm_num3,g_mdm_prdt_rel.mdm_code1) )
			flag2[0]='1';
		else if ( flag2[1]!='1' && !strcmp(mdm_num3,g_mdm_prdt_rel.mdm_code2) )
			flag2[1]='1';
		else if ( flag2[2]!='1' && !strcmp(mdm_num3,g_mdm_prdt_rel.mdm_code3) )
			flag2[2]='1';
		else
			continue;

		count3=0;
		if( flag2[0]=='1' ) count3++;
		if( flag2[1]=='1' ) count3++;
		if( flag2[2]=='1' ) count3++;
		if( count1==count3 ) break;
	}

	Mdm_prdt_rel_Clo_Sel ();

	if( count1!=count3 ) 
	{
		sprintf( acErrMsg,"��Ʒ���ʶ��ձ����޴˼�¼[%d][%s]",ret,prdt_num);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"W043" );
		return 1;
	}

    return 0;
}

/**********************************************************************
 * �� �� ����pub_base_set_drawtype();
 * �������ܣ��趨���ʵ�֧ȡ��ʽ
 *           
 * ����/ʱ��:
 *
 * ������
 *     ���룺�����ʻ���ϵ�ṹ
 *          
 *     �������Ӧ��
 *         
 *   ����ֵ������0�趨�ɹ���
 *           ���ط�0�趨���ɹ�
 * �޸���ʷ��
 *           
********************************************************************/
int pub_base_set_drawtype(mdm_ac_rel_c)
	struct mdm_ac_rel_c mdm_ac_rel_c;
{
  	ret = Mdm_ac_rel_Ins(mdm_ac_rel_c, g_pub_tx.reply) ;
	
	if( ret && ret!=-239 )
	{
		sprintf( acErrMsg,"���ӽ����˻����ձ����[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D107" );
		return 1;
	}
	else if( ret ==-239 )
	{
		sprintf( acErrMsg,"���ӽ����˻����ձ����[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D108" );
		return 1;
	}
  
 	return 0;
}

/**********************************************************************
 * �� �� ����pub_base_check_draw(char mode[4]);
 * �������ܣ�������ʵ�֧ȡ��ʽ
 *
 * ����/ʱ��:
 *
 * ������
 *     ���룺mode[4]  	��һλ�Ƿ��ȡmdm_ac_rel:0-��1-�ǣ�
 *						�ڶ�λ�������У���ѯ���뻹��֧ȡ���룺0-֧ȡ��1-��ѯ��
 *						����λ�Ƿ��������:0-��1-�ǣ�
 *     �����
 *
 *   ����ֵ��
 *			 ��Ӧ��
 *			 ����0�ɹ�
 *           ���ط�0���ɹ�
 * �޸���ʷ��
 *
********************************************************************/
int pub_base_check_draw(char mode[4])
{
	char tmp_draw_pwd[7];
	char tmp_qry_pwd[7];
	
	char filename[128];
	FILE *fp = NULL;
	char tmpstr[128];
	int i = 0;
	int iVal = 0;
	char command[128];
	
	MEMSET_DEBUG( filename, 0x0, sizeof(filename) );
	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf(acErrMsg,"%s,%d,draw_pwd_yn=[%c],draw_id_yn=[%c]",
		__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0],g_mdm_ac_rel.draw_id_yn[0]);
	WRITEMSG
	
    /*mod by ChengGX ���� 2010-1-27 15:10*/
    if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)==0)
    {
        sprintf(acErrMsg,"%s,%d,���п���������� ֱ�ӷ���",__FILE__,__LINE__);
	    WRITEMSG
	    return 0;
    }
	if(strcmp(g_pub_tx.tx_code,"2410")==0)
	{
		sprintf(acErrMsg,"%s,%d,���п���������� ֱ�ӷ���",__FILE__,__LINE__);
		    WRITEMSG
		return 0;
	}
    /*mod by ChengGX ���� 2010-1-27 15:10*/
	strcpy(tmp_draw_pwd,g_pub_tx.draw_pwd);
	strcpy(tmp_qry_pwd,g_pub_tx.qry_pwd);
	sprintf(acErrMsg,"mode===[%s]",&mode[0]);
        
	pub_base_strpack(g_pub_tx.ac_no);
	WRITEMSG
	if( mode[0]=='1' )
	{		
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
		if( ret==100 )
		{  
			sprintf( acErrMsg,"�����ʺŶ��ձ��в����ڸü�¼[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"M002" );
			return 1;
		}
		else if(ret)
		{
			sprintf( acErrMsg,"��ѯ�����ʺŶ��˱��쳣[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return 1;
		}
	}

	WRITEMSG
	/*** if( g_mdm_ac_rel.draw_uncon_yn[0]=='Y' )	return 0;  XXXXX **/

	sprintf( acErrMsg,"%s,%d,g_mdm_ac_rel.draw_pwd_yn=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0]);
	WRITEMSG
	/*���֧ȡ��ʽΪ����֧ȡ*/
	if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y')
	{    
		/*--add by dgf 20051114 for �ж����������Ƿ���-------------------*/
		sprintf( filename, "/backup/pass_error/%s.%ld",  g_pub_tx.ac_no, g_pub_tx.tx_date );
		vtcp_log( "/backup/pass_error/%s.%ld",  g_pub_tx.ac_no, g_pub_tx.tx_date );
		fp = fopen( filename, "r" );
		if ( fp == NULL )
		{
			goto NoErr;
		}
	sprintf( acErrMsg,"%s,%d,g_mdm_ac_rel.draw_pwd_yn=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0]);
	WRITEMSG
		
		while( fgets( tmpstr, 1023, fp ) != NULL )
		{
			i = i + 1;
		}
		
		fclose( fp );
		
		MEMSET_DEBUG( &sCom_parm, 0x0, sizeof(sCom_parm) );
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code = 'MMSR'" );
		if ( ret )
		{
			strcpy( acErrMsg, "��ѯ�������������!!" );
			WRITEMSG
		  	return 1;
		}
		
		iVal = atoi(sCom_parm.val);
		vtcp_log("������������������iVal=[%d],i=[%d]!",iVal,i);
		
		if ( i >= iVal )
		{
			strcpy( acErrMsg, "����������������Ѿ���������,���ղ��ܼ�������!!" );
			WRITEMSG
			/* add by martin 2008-11-19 14:30*/
					{
						sprintf( acErrMsg,"������������!" );
				    set_zd_data( DC_GET_MSG,acErrMsg );
					}
		  	strcpy( g_pub_tx.reply, "M036" );
		  	return 1;
		}
		
NoErr:		
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
		if( mode[1]=='0' )
		{
			/*����
			pub_base_strpack(g_pub_tx.draw_pwd);
			pub_base_strpack(g_mdm_ac_rel.draw_pwd);
			*/

			if(mode[2]=='0' )
			{
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
				if( strcmp(g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd)
					&& strcmp(g_pub_tx.tx_code,"2410")!=0)
				{
				/*sprintf(acErrMsg,">>>>����[%s]----[%s]",g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
				WRITEMSG*/
				strcpy( acErrMsg,"PASSWORD is not correct!!");
				WRITEMSG
				/*if( 4-i>0 )2008-11-19 14:35*/
				if(((iVal-1)-i)>0)
				{
				sprintf( acErrMsg,"�������� %d ��",(iVal-1)-i );
				set_zd_data( DC_GET_MSG,acErrMsg );
				}
		  		strcpy( g_pub_tx.reply ,"M005" );
		  		goto ErrExit;
				}
			}
			else if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) 
				&& strcmp(g_pub_tx.tx_code,"2410")!=0 )
			{
				sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
				WRITEMSG
				strcpy( acErrMsg,"PASSWORD is not correct!!");
				WRITEMSG
				/*if( 4-i>0 )2008-11-19 14:35*/
				if((iVal-1)-i>0)
				{
				sprintf( acErrMsg,"�������� %d ��",(iVal-1)-i );
				set_zd_data( DC_GET_MSG,acErrMsg );
				}
		  		strcpy( g_pub_tx.reply ,"M005" );
		  		goto ErrExit;
			}
		}
		else if( mode[1]=='1' )
		{
			/*
			pub_base_strpack(g_pub_tx.qry_pwd);
			pub_base_strpack(g_mdm_ac_rel.qry_pwd);
			*/
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG

			if(mode[2]=='0' )
			{
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
				if(strcmp(g_pub_tx.qry_pwd,g_mdm_ac_rel.qry_pwd)
				  &&strcmp(g_pub_tx.tx_code,"2410")!=0)
				{
				strcpy( acErrMsg,"PASSWORD is not correct!!");
				WRITEMSG
		  		strcpy( g_pub_tx.reply ,"M005" );
		  		goto ErrExit;
				}
			}
			else if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.qry_pwd,g_mdm_ac_rel.qry_pwd) )
			{
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
				strcpy( acErrMsg,"PASSWORD is not correct!!");
				WRITEMSG
		  		strcpy( g_pub_tx.reply ,"M005" );
		  		goto ErrExit;
			}
		}
		else
		{
	sprintf( acErrMsg,"%s,%d,mode=[%s]",__FILE__,__LINE__,mode);
	WRITEMSG
			strcpy( acErrMsg,"PASSWORD is not correct!!");
			WRITEMSG
		  	strcpy( g_pub_tx.reply ,"M005" );
		  	goto ErrExit;
		}
		
		MEMSET_DEBUG( command, 0x00, sizeof(command) );
    		sprintf( command,"rm -f -r %s", filename );

    		ret = system( command );		
    }
	WRITEMSG
	sprintf(acErrMsg,"%s,%d,draw_pwd_yn=[%c],draw_id_yn=[%c]",
		__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0],g_mdm_ac_rel.draw_id_yn[0]);
	WRITEMSG

	if(g_mdm_ac_rel.draw_id_yn[0]=='Y')
	{
		pub_base_strpack(g_mdm_ac_rel.id_no);
		pub_base_strpack(g_pub_tx.id_no);

	sprintf( acErrMsg,"g_mdm_ac_rel.id_type=[%s]g_pub_tx.id_type=[%s]",
		g_mdm_ac_rel.id_type,g_pub_tx.id_type);
	WRITEMSG
	sprintf( acErrMsg,"g_mdm_ac_rel.id_no=[%s],g_pub_tx.id_no=[%s]",
		g_mdm_ac_rel.id_no,g_pub_tx.id_no);
	WRITEMSG
		if( strcmp(g_mdm_ac_rel.id_type,g_pub_tx.id_type) )
		{
			sprintf( acErrMsg,"֤�����Ͳ���ȷ!!g_mdm_ac_rel.id_type=[%s], \
					g_pub_tx.id_type=[%s]",g_mdm_ac_rel.id_type,g_pub_tx.id_type);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"P210" );
		   	return 1;
		}
		else if( strcmp(g_mdm_ac_rel.id_no,g_pub_tx.id_no) )
		{	
			sprintf( acErrMsg,"֤�����벻��ȷ!!g_mdm_ac_rel.id_no=[%s], \
					g_pub_tx.id_no=[%s]",g_mdm_ac_rel.id_no,g_pub_tx.id_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"M004" );
		   	return 1;
		}	
    }

	if( g_mdm_ac_rel.pwd_mach_yn[0]=='Y' )
	{
		/*** У�������� ***/	
	}

	strcpy(g_pub_tx.draw_pwd,tmp_draw_pwd);
	strcpy(g_pub_tx.qry_pwd,tmp_qry_pwd);

  return 0;
  
ErrExit:

	sprintf( acErrMsg, "----------->��������ļ���Ϊ[%s]!!", filename );
	WRITEMSG

	fp = fopen( filename, "a+" );
	if ( fp == NULL )
	{
		strcpy( acErrMsg, "�����������־�ļ���!!" );
		WRITEMSG
	  	strcpy( g_pub_tx.reply, "M005" );
		return 1;
	}
	
	fprintf( fp, "��/����[%s]������[%s]���׻���[%s]ʱ��[%ld]���������\n", g_pub_tx.ac_no, \
		g_pub_tx.tx_code, g_pub_tx.tx_br_no, g_pub_tx.tx_time );
	
	fclose( fp );
	
	if ( i == iVal - 2 )
	{
		strcpy( acErrMsg, "�������,�������������һ�ε��ղ���ʹ��" );
		WRITEMSG
	  	strcpy( g_pub_tx.reply, "M037" );
	  	return 1;
	}

	if ( i == iVal - 1 )
	{
		strcpy( acErrMsg, "����������������Ѿ���������,���ղ��ܼ�������!!" );
		WRITEMSG
	  	strcpy( g_pub_tx.reply, "M036" );
	  	return 1;
	}

	return 1;
}
/**********************************************************************
 * �� �� ����pub_base_check_medstate();
 * �������ܣ�������״̬
 *
 * ����/ʱ��:
 *
 * ������
 *     ���룺��/���ţ�			char(19);
 *			 ���ʴ��ţ�			char(5);
 *     �����
 *			 ����״̬:			char(2);
 *			 ��Ӧ��				char(4);
 *   ����ֵ��
 *			 ����0�ɹ���
 *
********************************************************************/
int pub_base_check_medstate(char *ac_num,char *mdm_num,char *note_sts)
{
	/*����ʻ�ӵ�н��ʵ�״̬�Ƿ����*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel( &g_pub_tx.reply, &g_mdm_ac_rel, 
			  			"ac_no='%s' and mdm_code='%s'", ac_num,mdm_num);
	if (ret==100)
	{
		sprintf( acErrMsg,"�����ʺŶ��ձ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"M002" );
		return 1;
	}
	else if( ret && ret !=100 )
	{
	 	sprintf( acErrMsg,"��ѯ�����ʺŶ��ձ��쳣[%d]",ret);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"D103" );
	 	return 1;
	}

	strcpy(note_sts,g_mdm_ac_rel.note_sts);

	return 0;
}

int pub_base_ins_chrg_mst(char chrg_rate_code[4],char sts[2],double tx_amt)
{
	struct com_chrg_hst_c	sCom_chrg_hst;
	int	ret=0;
	MEMSET_DEBUG(&sCom_chrg_hst,0x00,sizeof(struct com_chrg_hst_c));

	sCom_chrg_hst.tx_date=g_pub_tx.tx_date;
	sCom_chrg_hst.trace_no=g_pub_tx.trace_no;
	strcpy(sCom_chrg_hst.ac_no,g_pub_tx.ac_no);
	strcpy(sCom_chrg_hst.tx_code,g_pub_tx.tx_code);
	strcpy(sCom_chrg_hst.chrg_rate_code,chrg_rate_code);
	strcpy(sCom_chrg_hst.name,g_pub_tx.brf);
	sCom_chrg_hst.chrg_amt=g_pub_tx.tx_amt1;
	sCom_chrg_hst.tx_amt=tx_amt;
	strcpy(sCom_chrg_hst.sts,sts);
	ret=Com_chrg_hst_Ins(sCom_chrg_hst,g_pub_tx.reply);
	if( ret )
	{
	 	sprintf( acErrMsg,"�Ǽ��շ���ϸ�����[%d]",ret);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"P448" );
	 	return 1;
	}
	return 0;
}

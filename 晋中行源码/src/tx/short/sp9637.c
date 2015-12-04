/*************************************************
* �� �� ��: sp9637.c
* ���������������ֵ��Ʒת����,����������ʾ�����ֵ��Ʒ�б�
* ��    ��: lpj
* ������ڣ�2015��11��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "prdt_jz_dz_c.h"

sp9637()  
{ 		
	/** ���ݳ�ʼ�� **/
	int ret=0, time_type=0;
	int sel_flag=0;
	int vcnt=0;
	char cif_type[2];
	char ln_type[2];
	char time_typ[2];
	char cpbh[4];
	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	char type[2];/*������־��1-����ת��ֵ��2-��ֵת����*/
	struct ln_parm_c st_ln_parm;
	FILE *fp;
	int i=0;
	
	memset(cif_type,0x00,sizeof(cif_type));
	memset(ln_type,0x00,sizeof(ln_type));
	memset(time_typ,0x00,sizeof(time_typ));
	memset(type,0x00,sizeof(type));
	memset(wherelist,0x00,sizeof(wherelist));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(filename,0x00,sizeof(filename));
	memset(cpbh,0x00,sizeof(cpbh));
	memset(&st_ln_parm,0x00,sizeof(struct ln_parm_c));
	
	/** ȡֵ����ֵ **/	
	get_zd_data( "0680", cif_type );    /* �ͻ����� */
	get_zd_data( "0580" , cpbh ); /* ԭ��Ʒ��� */
	get_zd_data( "0700" , type ); /*�������� */

	
	ret=Ln_parm_Sel( g_pub_tx.reply,&st_ln_parm," prdt_no='%s' ",cpbh);
	if( ret ){ 
			sprintf( acErrMsg,"���Ҵ����Ʒ��Ϣ���� prdt_no=[%s]",cpbh );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	
	
	TRACE
	
	memset(wherelist,0x00,sizeof(wherelist));
		if(type[0]=='1')
		{
				sprintf( wherelist," prdt_no in(select jz_prdt_no from prdt_jz_dz where prdt_no='%s') ",cpbh );
		}else if(type[0]=='2')
		{
				sprintf( wherelist,"  prdt_no in(select old_prdt_no from new_jz_prdt where new_prdt_no='%s') ",cpbh );
		}
		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG			
		
	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	/***��Ʒ�б�***/
	ret=Ln_parm_Dec_Sel( g_pub_tx.reply,"%s order by prdt_no",wherelist );
	if( ret ) goto ErrExit;

	i=0;
	while(1)
	{
		ret = Ln_parm_Fet_Sel( &g_ln_parm , g_pub_tx.reply );
		if( ret==100 ) break;		
		if( ret ) goto ErrExit;
		
		sel_flag++;
		    
		if( i==0 )
		{
			set_zd_data("0240",g_ln_parm.prdt_no);
			set_zd_data("0270",g_ln_parm.title);
		}

		pub_base_strpack(g_ln_parm.title);
		fprintf( fp,"0240|%-30s|%s|\n",g_ln_parm.title,g_ln_parm.prdt_no );

		i++;
		vcnt++;
	}
	Ln_parm_Clo_Sel();

		if ( sel_flag==0 )
		{    
		    sprintf(acErrMsg,"û���������������ļ�¼![%s]",g_pub_tx.reply);
		    WRITEMSG
		    strcpy(g_pub_tx.reply,"L123");
		    goto ErrExit;	
		}
		
	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
	/* ������� */

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

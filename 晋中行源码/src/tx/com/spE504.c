/*************************************************
* �� �� ��:  spE504.c
* ���������� ��Ա���ӽ���
* ��    ��:  andy
* ������ڣ� 2004��3��9��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"
#include "note_mst_hst_c.h"
#include "com_tel_c.h"
#include "com_tel_connect_c.h"
#include "note_parm_c.h"

int upd_ins_cash(char * ,int );

int spE504()
{
	int	ret;
	int Note_Tag,Cash_Tag;
	char sts[2],tw_sts[2];					/*ƾ֤״̬*/ /*�Է�ƾ֤״̬*/
	char mode[2],teller[5];				 	/*��������*/ /*�Է���Ա*/
	char tw_brno[6],tw_tel[5];				/*�Է�����*/ /*�Է���Ա*/
	char cInOut[2];							/*�ո���־*/
	char filename[50];
	FILE *fp;
	int  hour,min,sec,num;
	char prtime[9];
  long sumlong=0;
	struct	com_tel_c			g_com_tel;
	struct	com_tel_connect_c	g_com_tel_connect;
	struct	cash_mst_c			g_cash_mst;
	struct  cash_mst_c      t_cash_mst;
	struct	note_mst_c			g_note_mst;
	struct	note_mst_c			f_note_mst;
	struct	note_mst_hst_c		g_note_mst_hst;
	struct	note_mst_c			p_note_mst;
	struct	note_parm_c			p_note_parm;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));
	memset(&g_cash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&g_note_mst,0x00,sizeof(struct note_mst_c));
	memset(&f_note_mst,0x00,sizeof(struct note_mst_c));
	memset(&g_note_mst_hst,0x00,sizeof(struct note_mst_hst_c));
	memset(&p_note_mst,0x00,sizeof(struct note_mst_c));

	/*ȡǰֵ̨*/
	get_zd_data("0670",mode);							/* �������� */
	get_zd_data("0920",teller);							/* �Է���Ա */

	/* ��ʼ���ṹ */
	pub_base_sysinit();
	strcpy( g_pub_tx.brf, "��Ա����" );
	
	strcpy(sts,"4");									/* 2��ʾ���� �޸�Ϊ 4 ƾ֤ת��ת�� modify by martin 2009/4/23 11:07:09*/

	switch(mode[0])
	{
		case '1':										/*����*/
				strcpy( g_pub_tx.brf,"���Ӹ���" );
				strcpy( cInOut, "O" );					/*O-����ת��ƾ֤*/
				pub_base_strpack(g_pub_tx.tel);


				/****add by wanglei 20070502 ���ݽ���Ҫ�󣬽���ʱ��������Ա��״̬���붼��ǩ��״̬****/
				ret =Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel = '%s' ",teller);
				if(ret != 0)
				{
					sprintf(acErrMsg,"��ѯ��Ա��Ϣ�����!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"O065");
					goto ErrExit;
				}
				
				if(g_com_tel.csts[0] != '2')
				{
					sprintf(acErrMsg,"��ҵ��ֻ���ڹ�Աǩ�˺���!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P094");
					goto ErrExit;
				}
				/****end 20070502****/
				/**********************ƾ֤����************************/

				/* �Է���Ա����Ϊ�Լ� */
				if( !strcmp(g_pub_tx.tel,teller) )
				{
					sprintf( acErrMsg, "�Է���Ա����Ϊ������Ա!" );
					WRITEMSG
				  	sprintf( g_pub_tx.reply, "O058" );
					goto ErrExit;
				}

				/* �ж϶Է���Ա�ͽ��׹�Ա�Ƿ���ͬһ���������㣩*/
				ret = pub_base_GetTelInfo( teller, &g_com_tel );
				if( ret )
				{
				 	sprintf( acErrMsg, "�������뷽��Ա�ļ���,ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				if( strcmp( g_pub_tx.tx_br_no, g_com_tel.br_no ) )
				{
					sprintf( acErrMsg, "�Է���Ա�����ڱ�����[%s][%s]", \
							 g_pub_tx.tx_br_no,g_com_tel.br_no );
					WRITEMSG
					sprintf( g_pub_tx.reply, "S041" );
					goto ErrExit;
			 	}

				ret = Cash_mst_Sel( g_pub_tx.reply,&g_cash_mst,"tel='%s'", \
									teller);
				if( ret )
				{
					sprintf( acErrMsg, "�Է���Ա��Ǯ�䣬���ܽ���!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O231" );
					goto ErrExit;
				}/***add by ligl 2006-10-11 13:48**/
        if(g_cash_mst.bal>0.001)
        {
        	sprintf( acErrMsg, "�Է���ԱǮ�䲻Ϊ�㣬���ܽ���!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O121" );
					goto ErrExit;
        }
        ret=sql_sum_long("note_mst", "cnt",&sumlong,"tel='%s' and sts='0'",teller);
        if(ret)
        {
        	sprintf(acErrMsg,"��ѯ��Աƾ̨֤�ʴ���!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"N038");
        	goto ErrExit;
        }
        if(sumlong>0)
        {
        	sprintf(acErrMsg,"�ù�Ա����ƾ֤,��������!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"T052");
        	goto ErrExit;
        }
        /**end**/
				strcpy(tw_brno,g_com_tel.br_no);		/*** �Է����� ***/
				strcpy(tw_tel,teller);					/*** �Է���Ա ***/

				/*�жϹ�Ա֮ǰ�Ƿ���ڸ���*/
				ret = Com_tel_connect_Sel( g_pub_tx.reply,&g_com_tel_connect,
										   "tel='%s' and ind='0'",g_pub_tx.tel);
				if( ret&&ret!=100 )
				{
					sprintf( acErrMsg, "��ѯ��Ա���ӵǼǱ�����!!" );
					WRITEMSG
					goto ErrExit;
				}
				else if( !ret )
				{
					sprintf( acErrMsg, "�ù�Ա�Ѹ���,�����������!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O057" );
					goto ErrExit;
				}
				/*�жϹ�Ա֮ǰ�Ƿ����δ�ռ�¼,�����ڲ�������  add by wanglei 20070502*/
				ret = Com_tel_connect_Sel( g_pub_tx.reply,&g_com_tel_connect,
										   "tw_tel='%s' and ind='0'",g_pub_tx.tel);
				if( ret==100 )
				{
				}
				else if( ret==0 )
				{
					sprintf( acErrMsg, "��Ա���ӵǼǲ�������Ҫ����ļ�¼,��������!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "P092" );
					goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "��ѯ��Ա���ӵǼǱ�����!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O056" );
					goto ErrExit;
				}
				/****end by wanglei ****/

				/*��ѯ��Աӵ�еĿ���ƾ֤*/

				sprintf(acErrMsg,"tel='%s' and sts='0'", g_pub_tx.tel);
				WRITEMSG
				ret = Note_mst_Dec_Sel1( g_pub_tx.reply,"tel='%s' and sts='0'",
										g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "��ѯ��Աƾ̨֤�ʳ���!" );
					WRITEMSG
					goto ErrExit;
				}

				while(1)
				{
					ret = Note_mst_Fet_Sel1(&g_note_mst,g_pub_tx.reply);
					if( ret==100 ) break;
					else if( ret )
				 	{
						sprintf( acErrMsg, "Fetchƾ̨֤�ʳ���!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

					sprintf( acErrMsg, "/////////////////ƾ֤����Ϊ====[%s]!!",
							g_note_mst.note_type);
					WRITEMSG
TRACE
				 	/* ����ƾ֤----����ƾ֤(��������)���� */
				 	ret = pub_com_ChkNote(sts,g_note_mst);
					if( ret )
				 	{
						sprintf( acErrMsg, "����ƾ֤---����ƾ֤����!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

				 	/* �Ǽ�ת����Աƾ̨֤�ʣ��ֶδ��� */
				 	ret = pub_com_NoteApart( sts,g_note_mst );
				 	if( ret )
				 	{
					 	sprintf(acErrMsg,"�Ǽ�ת����Աƾ̨֤�ʣ��ֶδ�����");
					 	WRITEMSG
					 	goto ErrExit;
				 	}

					/* �Ǽǹ�Աƾ֤��ϸ */
					ret = pub_com_RegNoteHst( sts,g_note_mst,tw_brno,tw_tel );
					if( ret==-1 )
					{
						sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
						WRITEMSG
						goto ErrExit;
					}

					/* �Ǽ�ƾ֤��С�� */
					ret = pub_com_RegNoteMin( g_pub_tx.tx_br_no, \
							 g_note_mst.note_type, g_note_mst.beg_note_no, \
							 g_pub_tx.tel, cInOut );
					if( ret )
					{
						sprintf( acErrMsg, "�Ǽ�ƾ֤��С�Ŵ�" );
						WRITEMSG
						goto ErrExit;
					}

					Note_Tag++;

			 	}/*End of while*/

				Note_mst_Clo_Sel1();

				/**********************�ֽ���************************/
				/*****
				ret = Cash_mst_Dec_Sel(g_pub_tx.reply,"tel='%s' and bal>0.00", \
										g_pub_tx.tel);
				*********/
				ret = Cash_mst_Dec_Sel(g_pub_tx.reply,"tel='%s'and sts='0' ", \
										g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "��ѯ��ԱǮ�����!!" );
					WRITEMSG
					goto ErrExit;
				}

				Cash_Tag = 0;
				while(1)
				{
					ret = Cash_mst_Fet_Sel(&g_cash_mst,g_pub_tx.reply);
					if( ret==100 && Cash_Tag == 0 )
					{
						sprintf( acErrMsg, "�޴˹�ԱǮ����Ϣ" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "M035" );
					}else if( ret == 100 && Cash_Tag > 0 )
					{
						break;
					}else if( ret )
					{
						sprintf( acErrMsg, "��ѯ��ԱǮ�����!!" );
						WRITEMSG
						goto ErrExit;
					}
vtcp_log( "zy1   cur_no[%s]", g_cash_mst.cur_no );
					strcpy(g_pub_tx.cur_no,g_cash_mst.cur_no);	/*����*/
					strcpy(g_pub_tx.note_type,"");				/*ƾ֤����*/
					strcpy(g_pub_tx.beg_note_no,"");   			/*ƾ֤����*/
					g_pub_tx.tx_amt1=g_cash_mst.bal;			/*Ǯ�����*/
					strcpy(g_pub_tx.cash_code,"");				/*�ֽ������*/
					strcpy(g_pub_tx.brf,"");					/*ժҪ*/
					strcpy(g_pub_tx.add_ind,"0");				/*�ֽ�֧��*/
vtcp_log( "zy2   cur_no[%s]", g_pub_tx.cur_no );
vtcp_log("%d@%s __________ trace_no = %ld and trace_cnt = %ld and br_no='%s' and tel='%s' ",__LINE__, __FILE__, g_pub_tx.trace_no, g_pub_tx.trace_cnt, g_pub_tx.tx_br_no, g_pub_tx.tel);

					ret = pub_acct_cash( );
					if( ret )
					{
						sprintf(acErrMsg,"�����ֽ��ո�����ʧ��!");
				    	WRITEMSG
						goto ErrExit;
					}
vtcp_log("%d@%s __________ trace_no = %ld and trace_cnt = %ld and br_no='%s' and tel='%s' ",__LINE__, __FILE__, g_pub_tx.trace_no, g_pub_tx.trace_cnt, g_pub_tx.tx_br_no, g_pub_tx.tel);

					ret = upd_ins_cash("1",g_pub_tx.trace_no );
					if( ret )
					{
						sprintf(acErrMsg,"����ȯ������޸�ʧ��!");
				    	WRITEMSG
						goto ErrExit;
					}

					strcpy(g_com_tel_connect.cur_no,g_cash_mst.cur_no);/*����*/
					strcpy(g_com_tel_connect.tel,g_pub_tx.tel);		/*��Ա��*/
					strcpy(g_com_tel_connect.tw_tel,teller);		/*�Է���Ա*/
					g_com_tel_connect.amt=g_cash_mst.bal;			/*���ӽ��*/
					g_com_tel_connect.tx_date=g_pub_tx.tx_date;		/*��������*/
					g_com_tel_connect.trace_no=g_pub_tx.trace_no;	/*��ˮ��*/
					strcpy(g_com_tel_connect.br_no,g_pub_tx.tx_br_no);/*�����*/
					strcpy(g_com_tel_connect.ind,"0");	  /*��ȡ��־-0��ʾδȡ*/

					/*�Ǽǹ�Ա���ӵǼǱ�*/	
					ret=Com_tel_connect_Ins(g_com_tel_connect,g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"�Ǽǹ�Ա���ӵǼǱ�ʧ��!ret=[%d]",ret);
				    	WRITEMSG
				  		sprintf( g_pub_tx.reply, "O059" );
						goto ErrExit;
					}

					Cash_Tag++;

				}/*End of while*/
				Cash_mst_Clo_Sel();

				if( !Cash_Tag && !Note_Tag )
				{
					sprintf(acErrMsg," ��Աû�пɸ�����ƾ֤���ֽ� " );
					WRITEMSG
					strcpy( g_pub_tx.reply,"D228" );
					goto ErrExit;
				}

				/*add by wangjing ��ӡ��Ա���ӵ� begin*/
				pub_base_AllDwnFilName( filename );
				fp = fopen( filename,"w" );
				if( fp==NULL )
				{
					sprintf(acErrMsg," open file [%s] error ",filename );
					WRITEMSG
					strcpy( g_pub_tx.reply,"S047" );
					goto ErrExit;
				}
 
				/*��ӡʱ�� Ǯ�������ң�*/
				pub_base_GetTime2( &hour, &min, &sec );
				memset( prtime, 0x00, sizeof( prtime ) );
				sprintf( prtime, "%02d:%02d:%02d", hour, min, sec );    /*��ӡʱ��*/
				memset( &g_com_tel_connect, 0x00, sizeof( struct com_tel_connect_c ) );
				ret = Com_tel_connect_Sel( g_pub_tx.reply, &g_com_tel_connect, 
					"ind='0' and br_no='%s' and tel='%s' and tw_tel='%s' and cur_no='01' and tx_date=%ld and trace_no=%ld", 
					g_pub_tx.tx_br_no,g_pub_tx.tel,teller,
					g_pub_tx.tx_date,g_pub_tx.trace_no );
				if( ret )
				{
					vtcp_log( "XXXX ind='0' and br_no='%s' and tel='%s' and tw_tel='%s' and cur_no='01' and tx_date=%ld and trace_no=%ld", 
					g_pub_tx.tx_br_no,g_pub_tx.tel,teller,
					g_pub_tx.tx_date,g_pub_tx.trace_no );
					sprintf( acErrMsg, "��ӡ ȡ��Ա���ӵǼǱ���Ϣ����!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O056" );
					goto ErrExit;
				}
				fprintf( fp, "				       [%s]��Ա���ӵ�[���]	\n",g_pub_tx.tel);
				fprintf( fp, "�������������Щ����������������Щ���������\n" );
				fprintf( fp, "��ʱ     �� ��    Ǯ�� ���   ���ƽ���Ա��\n" );
				fprintf( fp, "�������������੤���������������੤��������\n" );
				fprintf( fp, "��%10s��%16.2lf��%8s��\n", prtime, g_com_tel_connect.amt,teller);
				fprintf( fp, "�������������ة����������������ة��������� \n\n\n\n\n" );
				/*ƾ֤*/
				ret = Note_mst_Dec_Sel2( g_pub_tx.reply, "tel='%s' and sts='2' order by note_type", g_pub_tx.tel );
				if( ret )
				{
					sprintf( acErrMsg, "��ѯ��Աƾ̨֤�ʳ���!!" );
					WRITEMSG
					goto ErrExit;
				}
				fprintf( fp, "				       [%s]��Ա���ӵ�[ƾ֤]	\n",g_pub_tx.tel);
				fprintf( fp, "���������������������Щ����������������Щ����������������Щ��������Щ���������\n" );
				fprintf( fp, "��  ƾ  ֤  ��  ��  ��    ��ʼ����    ��    ��ֹ����    ��  ����  ���ƽ���Ա��\n" );	
				while(1)
				{
					ret=Note_mst_Fet_Sel2(&p_note_mst,g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
				 	{
						sprintf( acErrMsg, "Fetchƾ̨֤�ʳ���!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

					memset(&p_note_parm,0x00,sizeof(struct note_parm_c));
					ret = Note_parm_Sel( g_pub_tx.reply, &p_note_parm, 
						"note_type='%s'", p_note_mst.note_type );
					if( ret )
					{
						sprintf( acErrMsg, "Fetchƾ֤���������!!" );
					 	WRITEMSG
					 	goto ErrExit;
					}
					fprintf( fp, "���������������������੤���������������੤���������������੤�������੤��������\n" );
					fprintf( fp, "��%18s��%16s��%16s��%8d��%8s��\n", p_note_parm.name, p_note_mst.beg_note_no, p_note_mst.end_note_no, p_note_mst.cnt,teller );
				}/*while����*/
					fprintf( fp, "���������������������ة����������������ة����������������ة��������ة��������� \n\n\n\n\n" );
				Note_mst_Clo_Sel2();

				fclose(fp);
				set_zd_data( DC_FILE_SND,"1" );
				/*add by wangjing ��ӡ��Ա���ӵ� end*/

				break;
		case '2':										/*����*/
				strcpy( g_pub_tx.brf,"��������" );
				strcpy( cInOut, "I" );					/*I-����ת��ƾ֤*/

				/****add by wanglei 20070502 ���ݽ���Ҫ�󣬽���ʱ��������Ա��״̬���붼��ǩ��״̬****/
				ret =Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel = '%s' ",teller);
				if(ret != 0)
				{
					sprintf(acErrMsg,"��ѯ��Ա��Ϣ�����!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"O065");
					goto ErrExit;
				}
				
				if(g_com_tel.csts[0] != '2')
				{
					sprintf(acErrMsg,"��ҵ��ֻ���ڹ�Աǩ�˺���!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P094");
					goto ErrExit;
				}
				/****end 20070502****/

				/*�Է���Ա�������Լ�*/
				if( !strcmp(g_pub_tx.tel,teller) )
				{
					sprintf( acErrMsg, "ֻ������������Ա!" );
					WRITEMSG
				  	sprintf( g_pub_tx.reply, "O055" );
					goto ErrExit;
				}
        /**��������Ա��Ǯ���ƾ֤ add by ligl 2006-10-11 9:21**/
        ret=Cash_mst_Sel(g_pub_tx.reply,&t_cash_mst,"tel='%s' and cur_no='01'",g_pub_tx.tel);
        if(ret)
        {
        	sprintf(acErrMsg,"��ѯǮ�����!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"P153");
        	goto ErrExit;
        }
        if(t_cash_mst.bal>0)
        {
        	sprintf(acErrMsg,"���չ�ԱǮ�仹��Ǯ,��������!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"O121");
        	goto ErrExit;
        }
        ret=sql_sum_long("note_mst", "cnt",&sumlong,"tel='%s' and sts='0'",g_pub_tx.tel);
        if(ret)
        {
        	sprintf(acErrMsg,"��ѯ��Աƾ̨֤�ʴ���!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"N038");
        	goto ErrExit;
        }
        if(sumlong>0)
        {
        	sprintf(acErrMsg,"�ù�Ա����ƾ֤,��������!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"T052");
        	goto ErrExit;
        }
        /****������***/
				/*��齻�ӵǼǱ��Ƿ��жԷ���Ա�ĸ�����¼*/
				ret = Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect, \
					"tel='%s' and tw_tel='%s' and ind='0'",teller,g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "�Է���Աδ��������!!" );
					WRITEMSG
				  	sprintf( g_pub_tx.reply, "O054" );
					goto ErrExit;
				}

TRACE
				/*��ѯ�Է���Աӵ�еĿ���ƾ֤*/
				ret =Note_mst_Dec_Sel1(g_pub_tx.reply,"tel='%s' and sts='2'", \
										teller);
				if( ret )
				{
					sprintf( acErrMsg, "��ѯ��Աƾ̨֤�ʳ���!!" );
					WRITEMSG
					goto ErrExit;
				}

				while(1)
				{
					ret=Note_mst_Fet_Sel1(&g_note_mst,g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
				 	{
						sprintf( acErrMsg, "Fetchƾ̨֤�ʳ���!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

			 		/* ���ҹ�Աƾ֤��ϸ */
			 		/*���ݽ�����.ƾ֤����.��ʼƾ֤.��ֹƾ֤.�Է���Ա.ƾ֤��Ϊ*/
					strcpy(sts,"4"); /* 2��ʾ���� �޸�Ϊ 4 ƾ֤ת��ת�� modify by martin 2009/4/23 11:07:09*/
				 	ret = pub_com_GetNoteHst_this( g_com_tel_connect.trace_no, \
						g_pub_tx.tx_date, g_note_mst.note_type, \
						g_note_mst.beg_note_no, g_note_mst.end_note_no, \
						g_pub_tx.tx_br_no, g_pub_tx.tel, sts, &g_note_mst_hst );
				 	if( ret )
				 	{
						sprintf( acErrMsg, "��ѯ��Աƾ֤��ϸ�� \
								 [%s][%s][%s][%s][%ld]",
								 g_note_mst.note_type, g_note_mst.beg_note_no, \
								 g_note_mst.end_note_no,sts,g_pub_tx.tx_date);
					 	WRITEMSG
					 	goto ErrExit;
				 	}

					strcpy( tw_brno, g_note_mst_hst.br_no );
					strcpy( tw_tel, g_note_mst_hst.tel );

				  	/* ����Ա�Ƿ��Ǳ������Ĺ�Ա */
				  	if( strcmp( g_pub_tx.tx_br_no, g_note_mst_hst.br_no ) )
				  	{
					  	sprintf( acErrMsg, "�Է���Ա�����ڱ�����[%s][%s]", \
								g_pub_tx.tx_br_no,g_note_mst_hst.br_no );
					  	WRITEMSG
					  	sprintf( g_pub_tx.reply, "S041" );
					  	goto ErrExit;
				 	 }
					 

				  	/* ���ƾ֤���ڣ�ȡ��״̬ */
				 	strcpy(f_note_mst.tel,teller);
				  	strcpy(f_note_mst.br_no,g_note_mst_hst.br_no);
					/*
				 	strcpy(f_note_mst.note_type,g_note_mst_hst.note_type);
				  	strcpy(f_note_mst.beg_note_no,g_note_mst_hst.beg_note_no);
				  	strcpy(f_note_mst.end_note_no,g_note_mst_hst.end_note_no);
					*/
				 	strcpy(f_note_mst.note_type,g_note_mst.note_type);
				  	strcpy(f_note_mst.beg_note_no,g_note_mst.beg_note_no);
				  	strcpy(f_note_mst.end_note_no,g_note_mst.end_note_no);

					strcpy(sts,"5");  /* 3��ʾ���� �޸�Ϊ 5 ƾ֤ת��ת��modify by martin 62009/4/23 11:08:17*/	

				  	/* ���Է�����Ա�Ƿ�ӵ������ƾ֤ */
				  	ret = pub_com_ChkNote( sts, f_note_mst );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "����Աƾ֤��" );
				  	  	WRITEMSG
					  	goto ErrExit;
				 	 }

					/* �Ǽǹ�Աƾ̨֤�ʣ����δ��� */
					ret = pub_com_RegTelNote(g_pub_tx.tel, \
									g_pub_tx.tx_br_no,\
									sts, g_note_mst.note_type, \
				  					g_note_mst.beg_note_no,	\
									g_note_mst.end_note_no );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "�Ǽǹ�Աƾ̨֤�ʣ����δ�����" );
					  	WRITEMSG
					  	goto ErrExit;
				  	}

				 	/* ɾ��/�޸ĸ�����Ա��ƾ֤ */
					strcpy(tw_sts,"4"); /* 2��ʾ���� �޸�Ϊ 4 ƾ֤ת��ת�� modify by martin 2009/4/23 11:07:09*/
				  	ret = pub_com_NoteDel( tw_sts, f_note_mst );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "ɾ��/�޸��·���Ա��ƾ֤��" );
					  	WRITEMSG
					  	goto ErrExit;
				  	}

					/* �Ǽǹ�Աƾ֤��ϸ */
					ret = pub_com_RegNoteHst( sts,g_note_mst,tw_brno,tw_tel );
					if( ret==-1 )
					{
						sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
						WRITEMSG
						goto ErrExit;
					}

				}/*while����*/

				Note_mst_Clo_Sel1 ();

				/**********************�ֽ���************************/
				/*�ӹ�Ա���ӵǼǱ�ȡ����Ϣ*/
				ret = Com_tel_connect_Dec_Upd(g_pub_tx.reply, \
									"tel='%s' and tw_tel='%s' and ind='0'", \
									teller,g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "ȡ��Ա���ӵǼǱ���Ϣ����!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O056" );
					goto ErrExit;
				}

				while(1)
				{
					ret=Com_tel_connect_Fet_Upd(&g_com_tel_connect, \
												g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
					{
						sprintf( acErrMsg, "ȡ��Ա���ӵǼǱ���Ϣ����!!" );
						WRITEMSG
						sprintf( g_pub_tx.reply, "O056" );
						goto ErrExit;
					}

					strcpy(g_pub_tx.cur_no,g_com_tel_connect.cur_no);/*����*/
					strcpy(g_pub_tx.note_type,"");				/*ƾ֤����*/
					strcpy(g_pub_tx.beg_note_no,"");   			/*ƾ֤����*/
					g_pub_tx.tx_amt1=g_com_tel_connect.amt;		/*������*/
					strcpy(g_pub_tx.cash_code,"");				/*�ֽ������*/
					strcpy(g_pub_tx.brf,"");					/*ժҪ*/
					strcpy(g_pub_tx.add_ind,"1");				/*�ֽ�����*/

					ret = pub_acct_cash();
					if( ret )
					{
						sprintf(acErrMsg,"�����ֽ��ո�����ʧ��!");
				    	WRITEMSG
						goto ErrExit;
					}

					/*�޸Ľ��ӵǼǱ���¼״̬Ϊ��ȡ*/
					strcpy(g_com_tel_connect.ind,"1");			/*��ȡ״̬*/

					ret=Com_tel_connect_Upd_Upd(g_com_tel_connect, \
												g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"���Ĺ�Ա���ӵǼǱ�״̬ʧ��!");
				    	WRITEMSG
						goto ErrExit;
					}
				
				}/*End of while*/

				Com_tel_connect_Clo_Upd( );

				break;
		case '3':										/*�ջ�*/
				strcpy( g_pub_tx.brf,"�����ջ�" );
				strcpy( cInOut, "I" );

				/* �ջ�ʱ�Է���Ա*/
				if( !strcmp(g_pub_tx.tel,teller) )
				{
					sprintf( acErrMsg, "�ջ�ʱ��Ա��ӦΪ��ǰ��Ա!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O053" );
					goto ErrExit;
				}

				/*��齻�ӵǼǱ��Ƿ����Լ������ļ�¼*/
				ret = Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect, \
										 "tel='%s' and tw_tel='%s' and ind='0'",
										 g_pub_tx.tel,teller);
				if( ret )
				{
					sprintf( acErrMsg, "�Է���Աδ��������!!" );
					WRITEMSG
				  	sprintf( g_pub_tx.reply, "O054" );
					goto ErrExit;
				}

				/*��ѯ��Աӵ�еĿ���ƾ֤*/
				ret =Note_mst_Dec_Sel1(g_pub_tx.reply,"tel='%s' and sts='2'", \
									    g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "��ѯ��Աƾ̨֤�ʳ���!!" );
					WRITEMSG
					goto ErrExit;
				}

				while(1)
				{
					ret=Note_mst_Fet_Sel1(&g_note_mst,g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
				 	{
						sprintf( acErrMsg, "Fetchƾ̨֤�ʳ���!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

					sts[0] = '4';

			 		/* ���ҹ�Աƾ֤��ϸ */
					/*���ݽ�����.ƾ֤����.��ʼƾ֤.��ֹƾ֤.�Է���Ա.ƾ֤��Ϊ*/
				 	ret = pub_com_GetNoteHst_this( g_com_tel_connect.trace_no, \
						g_pub_tx.tx_date, g_note_mst.note_type, \
						g_note_mst.beg_note_no, g_note_mst.end_note_no, \
						g_com_tel_connect.br_no, g_com_tel_connect.tw_tel, \
						sts, &g_note_mst_hst );
				 	if( ret )
				 	{
						sprintf( acErrMsg, "��ѯ��Աƾ֤��ϸ��" );
					 	WRITEMSG
					 	goto ErrExit;
				 	}

					strcpy( tw_brno, g_note_mst_hst.br_no );
					strcpy( tw_tel, g_note_mst_hst.tel );

				  	/* ����Ա�Ƿ��Ǳ������Ĺ�Ա */
				  	if( strcmp( g_pub_tx.tx_br_no, g_note_mst_hst.br_no ) )
				  	{
					  	sprintf( acErrMsg, "�Է���Ա�����ڱ�����" );
					  	WRITEMSG
					  	sprintf( g_pub_tx.reply, "S041" );
					  	goto ErrExit;
				 	}

				  	/* ���ƾ֤���ڣ�ȡ��״̬ */
				 	strcpy(f_note_mst.tel,g_pub_tx.tel);
				  	strcpy(f_note_mst.br_no,g_note_mst_hst.br_no);
					/*
				 	strcpy(f_note_mst.note_type,g_note_mst_hst.note_type);
				  	strcpy(f_note_mst.beg_note_no,g_note_mst_hst.beg_note_no);
				  	strcpy(f_note_mst.end_note_no,g_note_mst_hst.end_note_no);
					*/
				 	strcpy(f_note_mst.note_type,g_note_mst.note_type);
				  	strcpy(f_note_mst.beg_note_no,g_note_mst.beg_note_no);
				  	strcpy(f_note_mst.end_note_no,g_note_mst.end_note_no);

					strcpy(sts,"5"); /* 3��ʾ���� �޸�Ϊ 5 ƾ֤ת��ת��modify by martin 62009/4/23 11:08:17*/	

				  	/* ���Է�����Ա�Ƿ�ӵ������ƾ֤ */
				  	ret = pub_com_ChkNote( sts, f_note_mst );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "����Աƾ֤��" );
				  	  	WRITEMSG
					  	goto ErrExit;
				 	 }

					/* �Ǽǹ�Աƾ̨֤�ʣ����δ��� */
					ret = pub_com_RegTelNote(g_pub_tx.tel, \
									g_pub_tx.tx_br_no,\
									sts, g_note_mst.note_type, \
				  					g_note_mst.beg_note_no,	\
									g_note_mst.end_note_no );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "�Ǽǹ�Աƾ̨֤�ʣ����δ�����" );
					  	WRITEMSG
					  	goto ErrExit;
				  	}

				 	/* ɾ��/�޸ĸ�����Ա��ƾ֤ */
					strcpy(tw_sts,"4"); /* 2��ʾ���� �޸�Ϊ 4 ƾ֤ת��ת�� modify by martin 2009/4/23 11:07:09*/
				  	ret = pub_com_NoteDel( tw_sts, f_note_mst );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "ɾ��/�޸��·���Ա��ƾ֤��" );
					  	WRITEMSG
					  	goto ErrExit;
				  	}

					/* �Ǽǹ�Աƾ֤��ϸ */
					ret = pub_com_RegNoteHst( sts,g_note_mst,tw_brno,tw_tel );
					if( ret==-1 )
					{
						sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
						WRITEMSG
						goto ErrExit;
					}

				}/*while����*/

				Note_mst_Clo_Sel1 ();

				/**********************�ֽ���************************/
				/* �ӹ�Ա���ӵǼǱ�ȡ����Ϣ */
				ret = Com_tel_connect_Dec_Upd(g_pub_tx.reply, \
						"tel='%s' and ind='0'",g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "ȡ��Ա���ӵǼǱ���Ϣ����!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "S041" );
					goto ErrExit;
				}

				while(1)
				{
					ret=Com_tel_connect_Fet_Upd(&g_com_tel_connect, \
												g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
					{
						sprintf( acErrMsg, "ȡ��Ա���ӵǼǱ���Ϣ����!!" );
						WRITEMSG
						sprintf( g_pub_tx.reply, "S041" );
						goto ErrExit;
					}

					strcpy(g_pub_tx.cur_no,g_com_tel_connect.cur_no);/*����*/
					strcpy(g_pub_tx.note_type,"");				/*ƾ֤����*/
					strcpy(g_pub_tx.beg_note_no,"");   			/*ƾ֤����*/
					g_pub_tx.tx_amt1=g_com_tel_connect.amt;		/*������*/
					strcpy(g_pub_tx.cash_code,"");				/*�ֽ������*/
					strcpy(g_pub_tx.brf,"");					/*ժҪ*/
					strcpy(g_pub_tx.add_ind,"1");				/*�ֽ�����*/

					ret = pub_acct_cash( );
					if( ret )
					{
						sprintf(acErrMsg,"�����ֽ��ո�����ʧ��!");
				    	WRITEMSG
						goto ErrExit;
					}

					ret = upd_ins_cash("3",g_com_tel_connect.trace_no );
					if( ret )
					{
						sprintf(acErrMsg,"����ȯ������޸�ʧ��!");
				    	WRITEMSG
						goto ErrExit;
					}

					/*�޸Ľ��ӵǼǱ���¼״̬Ϊ��ȡ*/
					strcpy(g_com_tel_connect.ind,"1");			/*��ȡ״̬*/

					ret = Com_tel_connect_Upd_Upd( g_com_tel_connect, \
													g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"���Ĺ�Ա���ӵǼǱ�״̬ʧ��!");
				    	WRITEMSG
						goto ErrExit;
					}
				
				}/*End of while*/

				Com_tel_connect_Clo_Upd ();
				break;
		default :
				 sprintf( acErrMsg, "ƾ֤�����벻����,������===[%s]",mode );
				 WRITEMSG
				 goto ErrExit;
	}/*switch����*/

	/* д������־ *
	ret = pub_ins_trace_log();
	if( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
	 	WRITEMSG
		goto ErrExit;
	}
	**/

	strcpy( g_pub_tx.brf, "��Ա����" );
	switch(mode[0])
	{
		case '1':										/*����*/
			set_zd_data("0680","2");
			set_zd_data("0690","1");
			set_zd_double("0430",g_pub_tx.tx_amt1);
			strcpy(g_pub_tx.add_ind,"1");				/* ����־ */
			break;
		case '2':										/*����*/
			set_zd_data("0680","1");
			set_zd_data("0690","2");
			strcpy(g_pub_tx.add_ind,"0");				/* ����־ */
			set_zd_double("0430",g_pub_tx.tx_amt1);
			break;
		case '3':										/*�ջ�*/
			set_zd_data("0680","1");
			set_zd_data("0690","2");
			strcpy(g_pub_tx.add_ind,"0");				/* ����־ */
			set_zd_double("0430",g_pub_tx.tx_amt1);
			break;
	}
	strcpy( g_pub_tx.cur_no, "01" );	/* ���� */
	/*strcpy( g_pub_tx.ac_no, "10104" );	* ��;������ֽ� ��������*/
	strcpy( g_pub_tx.ct_ind, "1" );	/* ��ת */
	strcpy( g_pub_tx.hst_ind, "1" );	/* ��ת */
	
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
int pub_com_GetNoteHst_this(
	long	trace_no,
	long	tx_date,
	char	*note_type,
	char	*beg_note_no,
	char	*end_note_no,
	char	*tw_br_no,
	char	*tw_tel,
	char	*note_act,
	struct	note_mst_hst_c	*note_mst_hst)
{
	int	icount = 0;
	int	iret;

	iret = Note_mst_hst_Dec_Sel( g_pub_tx.reply , 
		"note_type = '%s' and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s' and trace_no=%ld", 
			note_type, tx_date, tw_br_no, 
			tw_tel, note_act, trace_no);
	
	if ( iret )
	{
		sprintf( acErrMsg, "ִ��	Note_mst_hst_Dec_Sel��iret=[%d]", iret );
		WRITEMSG
		return(-1);
	}
	while( 1 )
	{
		iret = Note_mst_hst_Fet_Sel( note_mst_hst , g_pub_tx.reply );
		if ( icount > 0 && iret == 100 )
		{
			break;	
		}
		if ( iret == 100 )
		{
			sprintf( acErrMsg, "��Աƾ֤��ϸ������note_type = '%s' and beg_note_no = '%s' and end_note_no = '%s' and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s' and trace_no=%ld", note_type, beg_note_no, end_note_no, tx_date, tw_br_no, tw_tel, note_act, trace_no);
			WRITEMSG
			strcpy( g_pub_tx.reply, "N041" );
			return(-1);
		}else if ( iret )
		{
			sprintf( acErrMsg, "��ѯ��Աƾ֤��ϸ��!iret=[%d]", iret );
			WRITEMSG
			return(-1);				
		}

		break;

		/*icount++; */
	}
	
		/*
	if ( icount > 1 )
	{
		sprintf( acErrMsg, "��Աƾ֤��ϸ���ڶ�����¼" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N042" );
		return(-1);			
	}
		*/
	Note_mst_hst_Clo_Sel ();

	return(0);
}

/*********************************************************************************************
            add      by wanglei      20070502
���ݽ���Ҫ�󣬹�Ա����ʱ����Cash_mst�е�ȯ���������㣬Cash_mst_hst�в���һ��������ȯ��ǼǼ�¼
                  �ջ�ʱ�����෴������Ҳ��Cash_mst_hst�м�һ�������ȯ��ǼǼ�¼
**********************************************************************************************/
int upd_ins_cash(char * mode,int trace_no)
{
	int ret=0;

	struct cash_mst_c	cash_mst_c;
	struct cash_mst_hst_c	cash_mst_hst_c; 
	memset(&cash_mst_c,0x00,sizeof(struct cash_mst_c));
	memset(&cash_mst_hst_c,0x00,sizeof(struct cash_mst_hst_c));


	if(mode[0] == '1')/****����****/
	{
		ret =Cash_mst_Sel1(g_pub_tx.reply,&cash_mst_c,"tel = '%s' ",g_pub_tx.tel);
		if(ret != 0)
		{
			sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		/*** �����ٵǼ�һ��ȯ��Ǽ���ϸ��������ȯ��ֵ��Ϊ��ֵ����ʾ���� ***/
		cash_mst_hst_c.trace_no=g_pub_tx.trace_no;
		vtcp_log("B TRACE [%d]",g_pub_tx.trace_no );
		cash_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
		strcpy(cash_mst_hst_c.br_no,g_pub_tx.tx_br_no);
		strcpy(cash_mst_hst_c.ac_no,g_pub_tx.ac_no);
		strcpy(cash_mst_hst_c.tw_ac_no,g_pub_tx.name);
		strcpy(cash_mst_hst_c.tel,g_pub_tx.tel);
		strcpy(cash_mst_hst_c.cur_no,g_pub_tx.cur_no);
		strcpy(cash_mst_hst_c.add_ind,"9");    /****�Ǽ�ȯ���־****/
		cash_mst_hst_c.amt=g_pub_tx.tx_amt1;
		cash_mst_hst_c.tx_date=g_pub_tx.tx_date;
		cash_mst_hst_c.tx_time=g_pub_tx.tx_time;
		strcpy(cash_mst_hst_c.brf,"��Ա����");
		strcpy(cash_mst_hst_c.cash_code,g_pub_tx.cash_code);
		cash_mst_hst_c.t10000 = -cash_mst_c.t10000;
		cash_mst_hst_c.t5000  = -cash_mst_c.t5000;
		cash_mst_hst_c.t2000  = -cash_mst_c.t2000;
		cash_mst_hst_c.t1000  = -cash_mst_c.t1000;
		cash_mst_hst_c.t500   = -cash_mst_c.t500;
		cash_mst_hst_c.t200   = -cash_mst_c.t200;
		cash_mst_hst_c.t100   = -cash_mst_c.t100;
		cash_mst_hst_c.t50    = -cash_mst_c.t50;
		cash_mst_hst_c.t20    = -cash_mst_c.t20;
		cash_mst_hst_c.t10    = -cash_mst_c.t10;
		cash_mst_hst_c.t5     = -cash_mst_c.t5;
		cash_mst_hst_c.t2     = -cash_mst_c.t2;
		cash_mst_hst_c.t1     = -cash_mst_c.t1;
		vtcp_log("[%s][%d] hst_t10000 = %ld \n",__FILE__,__LINE__,cash_mst_hst_c.t10000);
		
		ret=Cash_mst_hst_Ins(cash_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"����Ǯ����ϸ����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P155");
			return -1;
		}
	
	
		ret=Cash_mst_Dec_Upd1(g_pub_tx.reply,
			" br_no='%s' and cur_no='%s' and tel='%s' ",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		if( ret )
		{
			sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
        	
		ret=Cash_mst_Fet_Upd1(&cash_mst_c,g_pub_tx.reply);
		vtcp_log("brno[%s] cur_no[%s] tel[%s]",g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�˹�Ա��Ǯ��[%s][%s][%s]!",
				g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P154");
			return -1;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		
        	/****����������ȯ�����Ϊ��****/
        	cash_mst_c.t10000 = 0;
		cash_mst_c.t5000  = 0;
		cash_mst_c.t2000  = 0;
		cash_mst_c.t1000  = 0;
		cash_mst_c.t500   = 0;
		cash_mst_c.t200   = 0;
		cash_mst_c.t100   = 0;
		cash_mst_c.t50    = 0;
		cash_mst_c.t20    = 0;
		cash_mst_c.t10    = 0;
		cash_mst_c.t5     = 0;
		cash_mst_c.t2     = 0;
		cash_mst_c.t1     = 0;
        	
		ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		Cash_mst_Clo_Upd1();
	}
	else if(mode[0] == '3')/****�ջ�****/
	{
		ret=Cash_mst_hst_Sel1(g_pub_tx.reply,&cash_mst_hst_c,"trace_no = %ld  and add_ind = '9' and tx_date=%ld ",trace_no,g_pub_tx.tx_date);
		if(ret != 0)
		{
			sprintf(acErrMsg,"����Ǯ����ϸ�����!");
			WRITEMSG
			return -1;
		}
		ret=Cash_mst_Dec_Upd1(g_pub_tx.reply,
			" br_no='%s' and cur_no='%s' and tel='%s' ",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		if( ret )
		{
			sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
        	
		ret=Cash_mst_Fet_Upd1(&cash_mst_c,g_pub_tx.reply);
		vtcp_log("brno[%s] cur_no[%s] tel[%s]",g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�˹�Ա��Ǯ��[%s][%s][%s]!",
				g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P154");
			return -1;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		
		cash_mst_c.t10000 = -cash_mst_hst_c.t10000; 
		cash_mst_c.t5000  = -cash_mst_hst_c.t5000;
		cash_mst_c.t2000  = -cash_mst_hst_c.t2000;
		cash_mst_c.t1000  = -cash_mst_hst_c.t1000;
		cash_mst_c.t500   = -cash_mst_hst_c.t500; 
		cash_mst_c.t200   = -cash_mst_hst_c.t200; 
		cash_mst_c.t100   = -cash_mst_hst_c.t100; 
		cash_mst_c.t50    = -cash_mst_hst_c.t50;  
		cash_mst_c.t20    = -cash_mst_hst_c.t20;  
		cash_mst_c.t10    = -cash_mst_hst_c.t10;  
		cash_mst_c.t5     = -cash_mst_hst_c.t5;
		cash_mst_c.t2     = -cash_mst_hst_c.t2;
		cash_mst_c.t1     = -cash_mst_hst_c.t1;
		
		ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		Cash_mst_Clo_Upd1();

		/****�ջ�ʱ,��ԭ���ĸ�����������ϸ��¼ɾ���������ղŲ���������ϸҲɾ��,�൱�ڸù�Աû���������Ӹ���****/
		ret=Cash_mst_hst_Dec_Upd2(g_pub_tx.reply, " (trace_no = %ld  or trace_no = %ld) and tx_date=%ld ", trace_no,g_pub_tx.trace_no,g_pub_tx.tx_date);
		if(ret != 0)
		{
			sprintf(acErrMsg,"����Ǯ����ϸ�����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P095");
			return -1;
		}

		while(1)
		{
			ret=Cash_mst_hst_Fet_Upd2(&cash_mst_hst_c,g_pub_tx.reply);
			if(ret == 100)
			{
				break;
			}
			else if(ret != 0)
			{
				sprintf(acErrMsg,"����Ǯ����ϸ�����!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P095");
				return -1;
			}
			vtcp_log("ROWID=[%s]",cash_mst_hst_c.rowid);
			ret=Cash_mst_hst_Del_Upd(cash_mst_hst_c,g_pub_tx.reply);
			if(ret != 0)
			{
				vtcp_log("ROWID=[%s]",cash_mst_hst_c.rowid);
				sprintf(acErrMsg,"ɾ��Ǯ����ϸ�����!");
				WRITEMSG
				return -1;
			}
		}
		Cash_mst_hst_Clo_Upd2();
	}

	strcpy( g_pub_tx.dc_code,NULL_DC_CODE );

	return 0;
}
/****end by wanglei 20070502****/

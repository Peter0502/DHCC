/*************************************************************
* ÎÄ ¼ş Ãû:     spD699.c
* ¹¦ÄÜÃèÊö£º    ´
* ×÷    Õß:     xxxxxxx
* Íê³ÉÈÕÆÚ£º    2003Äê3ÔÂ15ÈÕ
*
* ĞŞ¸Ä¼ÇÂ¼£º
*    ÈÕ    ÆÚ:
*    ĞŞ ¸Ä ÈË:
*    ĞŞ¸ÄÄÚÈİ:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

spD699()
{
	int ret=0;
	int num=0;
	char ac_no[20],ac_type[2];
	char name[50],cur_no[2];
	double amt;
	FILE *fp;
	char filename[50];
	char tmpname[101];

	struct prdt_ac_id_c 		g_prdt_ac_id;
	struct dd_mst_c				g_dd_mst;
	struct td_mst_c				g_td_mst;
	struct mdm_ac_rel_c			g_mdm_ac_rel;
	struct dd_parm_c			g_dd_parm;
	struct td_parm_c			g_td_parm;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));

	/** ³õÊ¼»¯È«¾Ö±äÁ¿ **/
	pub_base_sysinit();
	
	/** È¡Öµ¡¢¸³Öµ **/
	get_zd_data("0300",g_pub_tx.ac_no);

	/** ´¦ÀíÁ÷³Ì **/
	/* ¸ù¾İÕËºÅÈ¡µÃÕË»§IDºÍÕÊ»§ĞòºÅ */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,\
	                      "ac_no='%s'",g_pub_tx.ac_no);
	if(ret)
	{
		sprintf(acErrMsg,"²éÑ¯½éÖÊÓëÕË»§¹ØÏµ±í´í[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;

TRACE		
	if(g_pub_tx.ac_seqn!=9999)
	{
TRACE		
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id, \
							"ac_id=%ld and  ac_seqn=%d", \
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
		if(ret)
		{
			sprintf(acErrMsg,"²éÑ¯²úÆ·ÕËºÅ¶ÔÕÕ±íÊ§°Ü![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

TRACE		
		switch(g_prdt_ac_id.ac_id_type[0])
		{
TRACE		
			 case '1':                                   /* »îÆÚ */
					ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,\
						"ac_id=%ld and ac_seqn=%d",\
						 g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
TRACE		
					if(ret)
					{	
						sprintf(acErrMsg,"²éÑ¯»îÆÚ´æ¿îÖ÷ÎÄ¼şÊ§°Ü![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
TRACE		
					strcpy(name,g_dd_mst.name);
					amt=g_dd_mst.bal;
					/*¸ù¾İ²úÆ·±àºÅ²éÕÒ±ÒÖÖ*/
					ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,\
									"prdt_no='%s'",g_dd_mst.prdt_no);
					if(ret)
					{
						sprintf(acErrMsg,"²éÑ¯»îÆÚ²úÆ·²ÎÊı±íÊ§°Ü![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
TRACE		
					strcpy(cur_no,g_dd_parm.cur_no);
					break;
			case '2':
					ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,\
								"ac_id=%ld and ac_seqn=%d",\
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
					if(ret)
					{
						sprintf(acErrMsg,"²éÑ¯¶¨ÆÚÖ÷ÎÄ¼şÊ§°Ü![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
					strcpy(name,g_td_mst.name);
					amt=g_td_mst.bal;
				
					ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,\
									"prdt_no='%s'",g_td_mst.prdt_no);
					if(ret)
					{
						sprintf(acErrMsg,"²éÑ¯¶¨ÆÚ²úÆ·²ÎÊı±í´íÎó![%d]");
						WRITEMSG
						goto ErrExit;
					}
					strcpy(cur_no,g_td_parm.cur_no);
					break;
			default:
					break;
			}
TRACE		
			/*´òÓ¡ÏÔÊ¾²¿·Ö*/
			pub_base_GenDwnFilName(filename);
TRACE		
			pub_base_strpack(filename);
			sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
TRACE		
			fp=fopen(tmpname,"w");

TRACE		
			fprintf(fp,"~@ÕËºÅ|ÕËºÅĞòºÅ|@¿Í»§Ãû|@±ÒÖÖ|$½ğ¶î|\n");
			fprintf(fp,"%s|%d|%s|%s|2lf|\n",g_pub_tx.ac_no,g_pub_tx.ac_seqn,\
						name,cur_no,amt);
			fclose(fp);
TRACE		
			set_zd_data(DC_FILE_SND,"1");
		}
		else
		{
			ret=Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",\
									g_mdm_ac_rel.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"Ö´ĞĞPrdt_ac_id_Dec_Sel´í![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			num=0;
			pub_base_GenDwnFilName(filename);
			pub_base_strpack(filename);
			sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
			fp=fopen(tmpname,"w");


			fprintf(fp,"~@ÕËºÅ|ÕËºÅĞòºÅ|@¿Í»§Ãû|@±ÒÖÖ|$½ğ¶î|\n");
			while(1)
			{
				ret=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
				if(ret==100&& num==0) 
				{
					sprintf(acErrMsg,"²éÑ¯²úÆ·ÕËºÅ¶ÔÕÕ±í£¬ÎŞ¼ÇÂ¼![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				else if ( ret == 100 && num > 0 ) break;

				else if(ret)
				{
					sprintf(acErrMsg,"Ö´ĞĞPrdt_ac_id_Fet_Sel´í![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				g_pub_tx.ac_seqn=g_prdt_ac_id.ac_seqn;


		
		switch(g_prdt_ac_id.ac_id_type[0])
		{
			case '1':									/* »îÆÚ */
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
							"ac_id=%ld and ac_seqn=%d",\
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"²éÑ¯»îÆÚ´æ¿îÖ÷ÎÄ¼şÊ§°Ü![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
TRACE
		sprintf(acErrMsg,"OKOKOKOOKOO");
		WRITEMSG
			strcpy(name,g_dd_mst.name);
			amt=g_dd_mst.bal;
			/*¸ù¾İ²úÆ·±àºÅ²éÕÒ±ÒÖÖ*/
			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
							"prdt_no='%s'",g_dd_mst.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"²éÑ¯»îÆÚ²úÆ·²ÎÊı±íÊ§°Ü![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(cur_no,g_dd_parm.cur_no);
TRACE
		sprintf(acErrMsg,"OKOKOKOOKOO");
		WRITEMSG
			break;
			
			case '2':								/* ¶¨ÆÚ */
			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
							"ac_id=%ld and ac_seqn=%d",\
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"²éÑ¯¶¨ÆÚÖ÷ÎÄ¼şÊ§°Ü![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_td_mst.name);
			amt=g_td_mst.bal;

			ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
							"prdt_no='%s'",g_td_mst.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"²éÑ¯¶¨ÆÚ²úÆ·²ÎÊı±íÊ§°Ü![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(cur_no,g_td_parm.cur_no);
			break;
		default:
			break;
		}

TRACE

  	/*´òÓ¡ÏÔÊ¾²¿·Ö*/

	fprintf(fp,"%s|%d|%s|%s|2lf|\n",g_pub_tx.ac_no,g_pub_tx.ac_seqn,name,amt);
TRACE
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	}
	num++;
	/*while ½áÊø*/

	Mdm_ac_rel_Clo_Sel();
	}
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"µÇ¼Ç½»Ò×Á÷Ë®´íÎó!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"²éÑ¯´òÓ¡³É¹¦![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"²éÑ¯´òÓ¡Ê§°Ü![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}

/*************************************************
* �� �� ��:    pubf_card_num.c
* ����������
*              pub_card_CardToNote   ���ݿ���ת���õ�ƾ֤���� 
*
* �������ܣ�
* ��    ��:
* ������ڣ�   2005��03��03�� 
* �޸ļ�¼�� 
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
*************************************************/

/**********************************************************************
 * �� �� ����  pub_card_CardToNote
 * �������ܣ�  ���ݿ���ת���õ�ƾ֤���� 
 * ��    �ߣ�  jack
 * ���ʱ�䣺  2003��03��03�� 
 *
 * ��    ���� 
 *     ��  �룺 
 *				char			card_no[20]
 *
 *     ��  ���� 
 *				char			note_no[17]
 *
 *     ����ֵ��
 *				0->�ɹ� 
 *				��0->ʧ�� 
 *
 * �޸���ʷ�� 
 *
***********************************************************************/
pub_card_CardToNote(char *card_no,char *note_no)
{
	 int ret=0;
	 char tmp_str[20];

	 memset(tmp_str,0,sizeof(tmp_str));

	 ret=strlen(card_no);
	 if(ret==19)
	 {
	   strcpy(note_no,"0000");
	   strncpy(tmp_str,card_no+6,12);/**mob by hxc091022 for JinZhong Card**/
	   strcat(note_no,tmp_str);
	 }
	 else if(ret==16)
	 {
		vtcp_log("%s,%d card_no=[%s]",__FILE__,__LINE__,card_no);
		/* rem by LiuHuafeng 20070125
		strcpy(note_no,"000000000000");
	    strncpy(tmp_str,card_no+8,7);
		*****************************/
		strcpy(note_no,"00000000");
	    strncpy(tmp_str,card_no+8,8);
		vtcp_log("%s,%d tmp_str=[%s]",__FILE__,__LINE__,tmp_str);
		strcat(note_no,tmp_str);
	 }
	 else if(ret==0)
	 {
	  	strcpy(note_no,"");
	 }

	return 0;
}

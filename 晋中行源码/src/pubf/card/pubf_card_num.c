/*************************************************
* 文 件 名:    pubf_card_num.c
* 功能描述：
*              pub_card_CardToNote   根据卡号转化得到凭证号码 
*
* 函数功能：
* 作    者:
* 完成日期：   2005年03月03日 
* 修改记录： 
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/

/**********************************************************************
 * 函 数 名：  pub_card_CardToNote
 * 函数功能：  根据卡号转化得到凭证号码 
 * 作    者：  jack
 * 完成时间：  2003年03月03日 
 *
 * 参    数： 
 *     输  入： 
 *				char			card_no[20]
 *
 *     输  出： 
 *				char			note_no[17]
 *
 *     返回值：
 *				0->成功 
 *				非0->失败 
 *
 * 修改历史： 
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

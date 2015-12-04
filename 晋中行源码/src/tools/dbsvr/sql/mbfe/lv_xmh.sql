/******************************************************************************/
/* Table Name:  LV_XMH - ��Ŀ��ű�                                           */
/* Location:    On region server                                              */
/* Index:       xmno                                                          */
/* Create for   XiaoE            2005/12/08                                   */
/******************************************************************************/
drop index lv_xmh_idx;
drop table lv_xmh;
CREATE TABLE lv_xmh (
  xmno     	varchar2(60),  /* ��Ŀ��� */
  yw_ind   	varchar2(12),  /* ҵ������  */
  xm_ind   	varchar2(1),    /* ��Ŀ���� 0-������Ŀ1-�����Ŀ*/
  ac_no    	varchar2(19),  /* �����ʺ�xmtype=0�����ʺ�,=1�տ��ʺ� */
  name     	varchar2(60),   /* ����*/
  o_br_no    	varchar2(12),  /* �Է��к�*/
  o_qs_no  	varchar2(12),  /* �Է������к� */
  o_opn_no  	varchar2(12),  /* �Է������к� */
  br_no     	varchar2(7),   /* ¼����� */
  last_date  	varchar2(8),   /* ����޸����� */
  tel    	varchar2(6),  /* ����޸Ĳ���Ա�� */
  flg     	varchar2(1),  /* ��־ */
  brf   	varchar2(60)  /* ��ע */
) tablespace users;
CREATE UNIQUE INDEX lv_xmh_idx on lv_xmh(xmno) tablespace indx;

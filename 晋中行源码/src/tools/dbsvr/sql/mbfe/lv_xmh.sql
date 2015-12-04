/******************************************************************************/
/* Table Name:  LV_XMH - 项目编号表                                           */
/* Location:    On region server                                              */
/* Index:       xmno                                                          */
/* Create for   XiaoE            2005/12/08                                   */
/******************************************************************************/
drop index lv_xmh_idx;
drop table lv_xmh;
CREATE TABLE lv_xmh (
  xmno     	varchar2(60),  /* 项目编号 */
  yw_ind   	varchar2(12),  /* 业务种类  */
  xm_ind   	varchar2(1),    /* 项目种类 0-贷记项目1-借记项目*/
  ac_no    	varchar2(19),  /* 本行帐号xmtype=0付款帐号,=1收款帐号 */
  name     	varchar2(60),   /* 户名*/
  o_br_no    	varchar2(12),  /* 对方行号*/
  o_qs_no  	varchar2(12),  /* 对方清算行号 */
  o_opn_no  	varchar2(12),  /* 对方开户行号 */
  br_no     	varchar2(7),   /* 录入机构 */
  last_date  	varchar2(8),   /* 最近修改日期 */
  tel    	varchar2(6),  /* 最后修改操作员号 */
  flg     	varchar2(1),  /* 标志 */
  brf   	varchar2(60)  /* 备注 */
) tablespace users;
CREATE UNIQUE INDEX lv_xmh_idx on lv_xmh(xmno) tablespace indx;

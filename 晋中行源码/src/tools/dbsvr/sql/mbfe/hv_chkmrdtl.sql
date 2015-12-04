drop index hv_chkmrdtl_idx;
drop  table hv_chkmrdtl;
create table hv_chkmrdtl(
	tx_date number(9), /*交易日期 */  
	cmtno varchar2(6), /*报文种类 */  
	s_cnt number(9),   /*发送总笔数 */
	s_amt number(22,2),/*发送总金额 */
	r_cnt number(9),   /*接收总笔数 */
	r_amt number(22,2),/*接收总金额 */
	brf varchar2(30)   /* 备注 */     
)tablespace users;
create	 unique index hv_chkmrdtl_idx on hv_chkmrdtl( tx_date,cmtno)tablespace indx;

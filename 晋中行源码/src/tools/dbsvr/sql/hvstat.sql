/***支付系统状态表***/
drop table hvstat;      
create table hvstat(    
	tx_date		number(8),		/*	交易日期 */
	stat		varchar2(1),		/*  状态*/
	chflag		varchar2(1),		/*	最近一次更新标志:0-成功,1-失败 */
	message		varchar2(60),		/*  更新描述*/
	filler1		varchar2(30),			
	filler2		varchar2(30)			
)tablespace users;

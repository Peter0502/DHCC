      /**小额报文发送登记簿 **/
drop index lv_sendlist_idx;
drop index lv_sendlist_idx2;
drop table lv_sendlist;
create table lv_sendlist(
   	tx_date		number(8)	, /** 交易日期		**/
   	br_no		varchar2(7)	, /** 交易机构		**/
	tel		varchar2(6)	, /** 操作员		**/
	trace_no	number(9)	, /** 流水号		**/
	refno		varchar2(20), /** 报文参考号	**/
	flagno		varchar2(8), /** 报文标识号   	**/
	packtype	varchar2(3)	, /** 包类型		**/
	packno		varchar2(3)	, /** 包编号		**/
	orderno		varchar2(8)	, /** 支付交易序号	**/
	or_br_no	varchar2(12), /** 发起行行号	**/
	pay_qs_no	varchar2(12), /** 发起清算行	**/
	ac_br_no	varchar2(12), /** 接收行行号	**/
	cash_qs_no	varchar2(12), /** 接收清算行	**/
	tx_amt		number(17,2), /** 交易金额		**/
	respflag	varchar2(1)	, /** 是否收到911报文**/
	respno		varchar2(8)	, /** 应答码		**/
	respdesc	varchar2(60), /** 应答描述		**/
	filler		varchar2(60)  /** 备注			**/
)tablespace users;
create unique index lv_sendlist_idx on lv_sendlist(tx_date,refno)tablespace indx;
create index lv_sendlist_idx2 on lv_sendlist(br_no,tel,respflag)tablespace indx;


// 配股登记簿：mo_stok_sale_hst
//Table index : mo_stok_sale_hst_idx (stokno) 

create table mo_stok_sale_hst(
stokno					varchar2(18),					//股东证号	
name						varchar2(60),					//户名	
befstokquant		number(16,2),					//配股前股数	
befbal					number(16,2),					//配股前金额	
rate						number(7,2),					//配股比率	
stokquant				number(16,2),					//配股数	
txamt						number(16,2),					//配股金额	
stokamt					number(16,2),					//配股单价	
payamt					number(16,2),					//应缴金额	
aftstokquant		number(16,2),					//配股后股数	
aftbal					number(16,2),					//配股后金额	
txdate					varchar2(8)	,					//交易日期	
txday						varchar2(8)	,					//交易时间	
stat						varchar2(1)						//配股状态	1：配股，2：不配
) tablespace users
;

create index mo_stok_sale_hst_idx on mo_stok_sale_hst(stokno) tablespace indx;

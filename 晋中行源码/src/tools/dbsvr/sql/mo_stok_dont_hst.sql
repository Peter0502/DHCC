// 红股登记簿：mo_stok_dont_hst
// Table index : mo_stok_dont_hst_idx (stokno) 

create table  mo_stok_dont_hst (
stokno						varchar2(18),					//股东证号	
name							varchar2(60),					//户名	
befstokquant			number(16,2),					//送股前股数	
befbal						number(16,2),					//送股前金额	
rate							number(7,2)	,					//送股比率	
stokquant					number(16,2),					//送股数	
txamt							number(16,2),					//送股金额	
aftstokquant			number(16,2),					//送股后股数	
aftbal						number(16,2),					//送股后金额	
txdate						varchar2(8)	,					//交易日期	
txtime						varchar2(8)						//交易时间	

) tablespace users ;
create   index mo_stok_dont_hst_idx on mo_stok_dont_hst(stokno) tablespace indx;         


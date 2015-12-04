// 红利登记簿：mo_stok_intr_hst
// Table index : mo_stok_intr_hst_idx (stokno)
create table  mo_stok_intr_hst (
stokno					varchar2(18),					//股东证号	         
name						varchar2(60),					//户名	               
stoknum					number(16,2),					//股数	             
bal							number(16,2),					//金额	                 
rate						number(7,2)	,					//利率	                 
intr						number(16,2),					//应付利息	           
taxrate					number(16,2),					//税率	             
intrtax					number(16,2),					//利息税	           
payintr					number(16,2),					//实付利息	         
txdate					varchar2(8)	,					//交易日期	           
txtime					varchar2(8)	,					//交易时间	           
bookcnt					number(4,0)						//存折位置	? ? ? ? ? ?

) tablespace users
;
create  index mo_stok_intr_hstt_idx on mo_stok_intr_hst(stokno) tablespace indx;
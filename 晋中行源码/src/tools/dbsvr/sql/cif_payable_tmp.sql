/* 客户应付利息登记簿:本表只登记上次ic_date到计算日期，
若账户存在分段时，积数、利率有可能不准确 */
drop table cif_payable_tmp;
create table cif_payable_tmp
(
	ac_id		number(9),
	ac_seqn		number(6),
	cif_no		number(9),
	ac_no		varchar2(32),
	name		varchar2(60),
	opn_br_no	varchar(5),
	prdt_no		varchar2(3),
	beg_date	number(8), ---ic_date起息日
	end_date	number(8),
	bal		number(16,2),
	intst_acm	number(19,2),
	rate		number(9,6),
	intst		number(16,2),
	intst_tax	number(16,2),
	tx_date		number(8),
	trace_no	number(9)
) tablespace users;
create unique index cif_intst_tmp_idx on cif_payable_tmp(ac_id,ac_seqn,beg_date,end_date);
create index cif_intst_tmp_idx1 on cif_payable_tmp(end_date,prdt_no);

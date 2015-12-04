drop index new_jz_prdt_1;
drop  table new_jz_prdt;
create table new_jz_prdt(
	pact_no varchar2(20),--借据号
	ac_no varchar2(19),--账号
	ac_name varchar2(60),--户名
	old_prdt_no varchar2(3),--新产品号
	old_prdt_name varchar2(60),--新产品名称
	new_prdt_no varchar2(3),--减值产品号
	new_prdt_name varchar2(60),--减值产品名称
	id_no varchar2(20),--证件号码
	bj_amt number(16,2),--账户余额
	in_lo_intst number(16,2),--表内欠息
	out_lo_intst number(16,2),--表外欠息
	sts varchar2(1), --状态-0未减值 1-已减值
	ac_id number(9),---账号id
	tx_date number(8),--交易日期
	tx_time number(9),--交易时间
	trace_no number(9),--交易流水
	tel varchar2(4),---交易柜员
	tx_br_no varchar2(5),---交易机构
	amt1 number(16,2),--备用金额1
	amt2 number(16,2),---备用金额2
	code1 varchar2(60),--备用字段1
	code2 varchar2(60) ---备用字段2
)tablespace users;
create	 unique index new_jz_prdt_1 on new_jz_prdt(ac_no,pact_no);

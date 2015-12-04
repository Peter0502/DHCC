drop index new_old_prdt_1;
drop  table new_old_prdt;
create table new_old_prdt(
	pact_no varchar2(20),--借据号
	ac_no varchar2(19),--账号
	ac_name varchar2(60),--户名
	old_prdt_no varchar2(3),--旧产品号
	old_prdt_name varchar2(60),--旧产品名称
	new_prdt_no varchar2(3),--新产品号
	new_prdt_name varchar2(60),--新产品名称
	id_no varchar2(20),--证件号码
	bj_amt number(16,2),--账户余额
	in_lo_intst number(16,2),--表内欠息
	out_lo_intst number(16,2),--表外欠息
	sts varchar2(1) --状态-0未入账 1-已入账
)tablespace users;
create	 unique index new_old_prdt_1 on new_old_prdt(ac_no,pact_no );

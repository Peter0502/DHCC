drop index mbfebillrtureg_idx;
drop  table mbfebillrtureg;
create table mbfebillrtureg(
	msssno varchar2(8),
	consigndate number(9),
	odficode varchar2(12),
	rdficode varchar2(12),
	sts varchar2(1),
	payeropenaccbkcode varchar2(12),
	amount number(15,2),
	billdate number(9),
	billcode varchar2(8),
	billofxchngseal varchar2(10),
	appuser varchar2(4),
	billtype varchar2(1),
	origrdficode varchar2(12),
	sendtime varchar2(14),
	setdate number(9),
	setflowno number(9),
	errcode varchar2(8),
	errdesc varchar2(60),
	strinfo varchar2(60) 
)tablespace users;
create	 unique index mbfebillrtureg_idx on mbfebillrtureg( msssno,consigndate)tablespace indx;

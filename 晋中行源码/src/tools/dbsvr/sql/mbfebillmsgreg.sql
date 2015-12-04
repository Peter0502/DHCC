drop index mbfebillmsgreg_idx;
drop  table mbfebillmsgreg;
create table mbfebillmsgreg(
	msssno varchar2(8),
	sts varchar2(1),
	consigndate number(9),
	rdficode varchar2(12),
	payeropenaccbkcode varchar2(12),
	payeracc varchar2(32),
	payername varchar2(60),
	payeraddr varchar2(60),
	billdate number(9),
	billcode varchar2(8),
	billofxchngseal varchar2(10),
	remittanceamnt number(15,2),
	repudiationamnt number(15,2),
	rdficenter varchar2(4),
	recipientopenaccbk varchar2(12),
	lastbillholderacc varchar2(32),
	lastbillholdername varchar2(60),
	prmptpaydate number(9),
	strinfo varchar2(60) 
)tablespace users;
create	 unique index mbfebillmsgreg_idx on mbfebillmsgreg( msssno,consigndate)tablespace indx;

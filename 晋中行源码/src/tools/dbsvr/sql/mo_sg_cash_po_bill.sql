drop  table mo_sg_cash_po_bill;
create table mo_sg_cash_po_bill(
	pact_no varchar2(20),
	bill_num number(6),
	lading_bill varchar2(19),
	pickup_amt number(16,2) 
)tablespace users;

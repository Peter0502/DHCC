drop  table mo_sg_cash_po;
create table mo_sg_cash_po(
	pact_no varchar2(20),
	br_no varchar2(5),
	pickup_sum number(16,2),
	safeg_sum number(16,2) 
)tablespace users;

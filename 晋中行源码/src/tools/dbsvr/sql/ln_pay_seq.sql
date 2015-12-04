drop  table ln_pay_seq;
create table ln_pay_seq(
	pay_seq_code varchar2(3),
	pay_seqn varchar2(15),
	deal_mode varchar2(1) 
)tablespace users;

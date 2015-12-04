drop  table auth_flow;
create table auth_flow(
	flow_code varchar2(3),
	seqn number(6),
	act_code varchar2(3),
	spe_ind varchar2(10) 
)tablespace users;

drop index mbfenetbank_idx;
drop  table mbfenetbank;
create table mbfenetbank(
	nbkcode varchar2(12),
	sabkcode varchar2(12),
	bnkcity varchar2(4),
	nbkname varchar2(60),
	nbksname varchar2(20),
	nbkaddrss varchar2(60),
	cnttel varchar2(20),
	cntper varchar2(16),
	postcode varchar2(6),
	nbkstate varchar2(1),
	bkemail varchar2(30),
	content varchar2(60) 
)tablespace users;
create	 unique index mbfenetbank_idx on mbfenetbank( nbkcode)tablespace indx;

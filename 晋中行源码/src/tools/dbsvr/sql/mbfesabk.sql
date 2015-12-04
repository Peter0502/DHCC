drop index mbfesabk_idx;
drop  table mbfesabk;
create table mbfesabk(
	sabkcode varchar2(12),
	sabkname varchar2(60),
	sabksname varchar2(20),
	sabkstat varchar2(1),
	sabkclass varchar2(2),
	sbksupr varchar2(70),
	blccpc varchar2(4),
	bnkcity varchar2(4),
	sabkaddress varchar2(60),
	postcode varchar2(6),
	cnttel varchar2(20),
	bkemail varchar2(30),
	content varchar2(60) 
)tablespace users;
create	 unique index mbfesabk_idx on mbfesabk( sabkcode)tablespace indx;

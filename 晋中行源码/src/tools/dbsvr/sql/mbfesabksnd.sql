drop index mbfesabksnd_idx;
drop  table mbfesabksnd;
create table mbfesabksnd(
	sabkcode varchar2(12),
	sabkname varchar2(60),
	sabksname varchar2(20),
	sabkclass varchar2(2),
	sabkstat varchar2(1),
	sbksupr varchar2(70),
	blccpc varchar2(4),
	bnkcity varchar2(4),
	sabkaddress varchar2(60),
	postcode varchar2(6),
	cnttel varchar2(20),
	bkemail varchar2(30),
	opertype varchar2(1),
	opereffdate number(9),
	content varchar2(60) 
)tablespace users;
create	 unique index mbfesabksnd_idx on mbfesabksnd( sabkcode,opertype,opereffdate)tablespace indx;

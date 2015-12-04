drop  table mbfeodoprtqryrpl;
create table mbfeodoprtqryrpl(
	consigndate number(9),
	querybkcode varchar2(12),
	origconsigndate number(9),
	origodficode varchar2(12),
	origtxtype varchar2(1),
	origtxssno varchar2(8),
	sndcmtcentrecvtime varchar2(14),
	sndcmtcentmvtime varchar2(14),
	sndcmtcenttranstat varchar2(2),
	npcrecvtime varchar2(14),
	statetime varchar2(14),
	npcmvtime varchar2(14),
	npctranstat varchar2(2),
	recvcmtcentrecvtime varchar2(14),
	recvcmtcentmvtime varchar2(14),
	recvcmtcenttranstat varchar2(2),
	errcode varchar2(8),
	errdesc varchar2(60) 
)tablespace users;

drop index jz_super_1;
drop  table jz_super;
create table jz_super(
	  UUID  varchar(38),
	  Trandate  varchar(10),
	  AreaNo  varchar(16),
	  TellerID  varchar(8),
	  Serseqno  varchar(16),
	  Ywcode  varchar(4),
	  Tranamt  varchar(18),
	  Faccno  varchar(20),
	  Faccname  varchar(60),
	  Faccbank  varchar(60),
	  Saccno  varchar(20),
	  Saccname  varchar(60),
	  Saccbank  varchar(60),
	  Mondyb  varchar(50),
	  Accountname  varchar(60),
	  Idcode  varchar(20),
	  Savetype  varchar(10),
	  BusinessSort  varchar(20),
	  Dealmode  varchar(10),
	  Warrantname  varchar(50),
	  Unit  varchar(2),
	  Count  varchar(5),
	  Startno  varchar(10),
	  Endno  varchar(18),
	  Flatcost  varchar(10),
	  Poundage  varchar(10),
	  Accode  varchar(10),
	  Isup  varchar(1),
	  State  varchar(2)
);
create   unique index jz_super_1 on jz_super(UUID);


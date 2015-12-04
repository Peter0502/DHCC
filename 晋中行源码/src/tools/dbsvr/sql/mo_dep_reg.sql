/**** ÒøĞĞ³Ğ¶Ò»ãÆ±ÖÊÑºÈë¿âµÇ¼Ç²¾ **/
drop  table mo_dep_reg;
create table mo_dep_reg(
	pact_no varchar2(20),	/** ÖÊÑº»ãÆ±±àºÅ **/
	po_no varchar2(16),	/** »ãÆ±±àºÅ **/
	po_amt number(16,2) 	/** Æ±Ãæ½ğ¶î **/
)tablespace users;

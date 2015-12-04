---tx_rtpe  1关联帐号   2关联客户号模式  
---sts      1正常关联   *解除模式
drop index idx_tel_save_rel_1;
drop index idx_tel_save_rel_2;
drop table tel_save_rel;
create table tel_save_rel (
	tel             varchar2(4),
	ac_no           varchar2(19),
  ac_seqn         number(6),
  cif_no          number(9),
  tx_type         varchar2(1),
  sts							varchar2(1),
  tx_date         number(9),
  tx_time         number(9),
  tx_tel          varchar2(4),
  br_no           varchar2(5),
  trace_no        number(9),
  brf             varchar2(30)
);
create index idx_tel_save_rel_1 on tel_save_rel(ac_no,sts);
create index idx_tel_save_rel_2 on tel_save_rel(cif_no,sts);

drop index vtm_note_mst_1;
drop table vtm_note_mst;
create table vtm_note_mst (
	tel             varchar2(4),
	tw_tel          varchar2(4),
	note_type       varchar2(3),
	beg_note_no1    varchar2(16),
	end_note_no1    varchar2(16),
	beg_note_no2    varchar2(16),
	end_note_no2    varchar2(16),
	beg_note_no3    varchar2(16),
	end_note_no3    varchar2(16),
	ind             varchar2(1),
	tx_date         number,
	trace_no        number
);
create unique index vtm_note_mst_1 on vtm_note_mst(tx_date,trace_no);


comment on table vtm_note_mst
	is 'VTM加清卡登记簿';
comment on column vtm_note_mst.tel
	is '柜员号码';
comment on column vtm_note_mst.tw_tel
	is '对方柜员';
comment on column vtm_note_mst.note_type
	is '凭证类型';
comment on column vtm_note_mst.beg_note_no1
	is '起始号段1';
comment on column vtm_note_mst.end_note_no1
	is '终止号段1';
comment on column vtm_note_mst.beg_note_no2
	is '起始号段2';
comment on column vtm_note_mst.end_note_no2
	is '终止号段2';
comment on column vtm_note_mst.beg_note_no3
	is '起始号段3';
comment on column vtm_note_mst.end_note_no3
	is '终止号段3';
comment on column vtm_note_mst.ind
	is '凭证状态:[2:付出 3:收入]';
comment on column vtm_note_mst.tx_date
	is '交易日期';
comment on column vtm_note_mst.trace_no
	is '交易流水';

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
	is 'VTM���忨�Ǽǲ�';
comment on column vtm_note_mst.tel
	is '��Ա����';
comment on column vtm_note_mst.tw_tel
	is '�Է���Ա';
comment on column vtm_note_mst.note_type
	is 'ƾ֤����';
comment on column vtm_note_mst.beg_note_no1
	is '��ʼ�Ŷ�1';
comment on column vtm_note_mst.end_note_no1
	is '��ֹ�Ŷ�1';
comment on column vtm_note_mst.beg_note_no2
	is '��ʼ�Ŷ�2';
comment on column vtm_note_mst.end_note_no2
	is '��ֹ�Ŷ�2';
comment on column vtm_note_mst.beg_note_no3
	is '��ʼ�Ŷ�3';
comment on column vtm_note_mst.end_note_no3
	is '��ֹ�Ŷ�3';
comment on column vtm_note_mst.ind
	is 'ƾ֤״̬:[2:���� 3:����]';
comment on column vtm_note_mst.tx_date
	is '��������';
comment on column vtm_note_mst.trace_no
	is '������ˮ';

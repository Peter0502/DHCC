drop index seqctl_idx;
dbaccess << --
!
{ ��ſ��Ƶ� }
database dhccb;
drop table seqctl;
create table seqctl
(
	br_no       char(5),{ ������ }
	ac_ind      char(2),{ �������,99-���֧������������ţ�98-���֧�����ײο��� }
	seqno       integer	{ ���˳��� }
);
close database;
!

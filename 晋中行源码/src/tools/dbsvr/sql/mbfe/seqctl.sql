 /****��ſ��Ƶ�****/ 
drop index seqctl_idx;
drop table seqctl;
create table seqctl(
	br_no       char(12),/*** �������к� ******/
	ac_ind      char(2), /****�������,99-���֧������������ţ� 
			98-���֧�����ײο��� 97 ��������� 
			96-С��֧���������,95 С��֧�����ײο���
			94-С��֧�����׸����� ******/
	seqno       number(10)	/**** ���˳��� ****/
)tablespace users;
create unique index seqctl_idx on seqctl (br_no,ac_ind) tablespace indx;

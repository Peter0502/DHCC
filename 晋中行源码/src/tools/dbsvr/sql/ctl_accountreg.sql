/*  ���Ƹ���֧��ÿ��ͬ�ǽ��ı� */
drop index  ctl_accountreg_idx;
drop table  ctl_accountreg;
create table ctl_accountreg
(
	seqno 	varchar2(13),      /*   ���=������5λ+����8λ */
	amt     number(16,2),       /*  ÿ�챾֧�п���ͬ�ǵ��ܽ��        */
	brf     varchar2(20)         /* �����ֶ� */
);
create unique index ctl_accountreg_idx on ctl_accountreg (seqno);

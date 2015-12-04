/*  控制各个支行每天同城金额的表 */
drop index  ctl_accountreg_idx;
drop table  ctl_accountreg;
create table ctl_accountreg
(
	seqno 	varchar2(13),      /*   序号=机构号5位+日期8位 */
	amt     number(16,2),       /*  每天本支行可以同城的总金额        */
	brf     varchar2(20)         /* 备用字段 */
);
create unique index ctl_accountreg_idx on ctl_accountreg (seqno);

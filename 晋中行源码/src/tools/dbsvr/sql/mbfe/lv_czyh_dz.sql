/***小额村镇银行对账***/
drop index lv_czyh_dz_idx;
drop  table lv_czyh_dz;
create table lv_czyh_dz(
	lw_ind               varchar2(1),   /*来往标志 1-往帐，2-来帐*/
	pack_date            number(8),     /*包日期*/
	pack_id              varchar2(8),   /*包序号*/ 
	pack_type            varchar2(3),   /*包类型*/ 
	check_cnt            number(6),     /*笔数*/ 
	check_amt            number(16,2),  /*金额*/
	check_flag           varchar2(1),   /*对账标志 1-对平，2-未平*/ 
	pay_qs_no            varchar2(12),  /*发起清算行行号*/
	tx_date              number(8),     /*系统日期*/
	br_no                varchar2(5)    /*对方机构*/
)tablespace users;
create	 unique index lv_czyh_dz_idx on lv_czyh_dz( lw_ind,pack_date,pack_id)tablespace indx;

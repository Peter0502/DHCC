      /**	С�����ϸ�˶����뱨����ϸ�����嵥�Ǽǲ�**/
drop index lv_chk852_d_idx;
drop table lv_chk852_d;
create table lv_chk852_d(
	lw_flg        varchar2(1),  /* ���������ʶ 1����2���� */
	pkgno         varchar2(3),  /* �����ͺ� */
	pay_qs_no     varchar2(12), /* ���������� */
	pack_date     number(8),  /* ί������    **/
	packid        varchar2(8),  /* ��id */
	filler      varchar2(60)  /* ��ע            **/
)tablespace users;
create unique index lv_chk852_d_idx on lv_chk852_d(pay_qs_no,packid,pack_date)tablespace indx;


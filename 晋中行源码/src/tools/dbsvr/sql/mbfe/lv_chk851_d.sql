      /**	С����ϸ�˶�Ӧ���ĵǼǲ�**/
drop index lv_chk851_d_idx;
drop table lv_chk851_d;
create table lv_chk851_d(
	lw_flg        varchar2(1),  /* ���������ʶ 1����2���� */
	zc_jd_lx      varchar2(1),  /* ����ڵ����� */
	zc_date       number(8),  /* ��������    **/
	zc_cc         varchar2(2),  /* ����� */
	pkgno         varchar2(3),  /* �����ͺ� */
	cnt           number(8)  ,  /* �������Ŀ */
	pay_qs_no     varchar2(12), /* ���������� */
	pack_date     number(8),  /* ί������    **/
	packid        varchar2(8),  /* ��id */
	dtlcnt        number(8),    /* ��ϸҵ���ܱ��� */
	tot_amt       number(24,2), /* ���ܽ��*/
	qs_date       number(8),  /* �������� */
	pack_sts      varchar2(2),  /* ccpc��״̬ */
	dhcc_sts      varchar2(2),  /* dhcc��״̬00 ��ʾҪ���·���  ������ccpc��ʶһ��*/
	filler        varchar2(60)  /* ��ע            **/
)tablespace users;
create unique index lv_chk851_d_idx on lv_chk851_d(pack_date,pay_qs_no,packid,lw_flg)tablespace indx;

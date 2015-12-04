     /**С�����������֪ͨ���յǼǲ� **/
drop index lv_c254r_idx;                                 
drop index lv_c254r_idx2;
drop table lv_c254r;
create table lv_c254r(
	cmtno			Varchar2(3)	, /** Cmt���			**/
	in_date			number(8)	, /** ����ϵͳ����		**/
	wt_date			number(8)	, /** ί������			**/
	dtlid			Number(3)	, /** ��ϸ���			**/
	o_pkgno			Varchar2(3)	, /** ԭ�����ͺ�		**/
	o_wtdate		number(8)	, /** ԭ��ί������		**/
	o_pay_qs_no		Varchar2(12)	, /** ԭ������������	**/
	zc_type			Varchar2(1)	, /** ��������			**/
	zc_node			Varchar2(1)	, /** ����ڵ�����		**/
	zc_date			number(8)	, /** ��������			**/
	zc_id			Varchar2(8)	, /** �����			**/
	crdb			Varchar2(1)	, /** �����־			**/
	zc_amt			Number(17,2)	, /** ������			**/
	o_packid		Varchar2(8)	, /** ԭ�����			**/
	qs_date			number(8)	, /** ��������			**/
	qs_sts			Varchar2(1)	, /** ���㴦��״̬		**/
	tx_time			Varchar2(14)	, /** ����ʱ��			**/
	brno			Varchar2(7)	, /** ���׻���			**/
	tel			Varchar2(6)	, /** ���ղ���Ա��	    **/
	tr_ac_no		Varchar2(6)	, /** ������ˮ��		**/
	sts			Varchar2(1)	, /** ����״̬			**/
	brprt_flg		Varchar2(64)	, /** ֧�д�ӡ��־		**/
	qsprt_flg		Varchar2(64)	, /** �������Ĵ�ӡ��־	**/
	filler			Varchar2(60)  /** ��ע            	**/
)tablespace users;
create unique index lv_c254r_idx on lv_c254r(wt_date,o_packid,dtlid)tablespace indx;
create index lv_c254r_idx2 on lv_c254r(in_date,brno)tablespace indx;

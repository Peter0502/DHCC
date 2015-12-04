/**	CMT554С����ⷵ��֪ͨ���ĵǼǲ� **/
drop index lv_c554r_idx;
drop index lv_c554r_idx2;
drop table lv_c554r;
create table lv_c554r(
	cmtno      	varchar2(3),/**  Cmt���	**/
	in_date		number(8),/**  ����ϵͳ����**/
	wt_date		number(8),/**  ί������	**/
	brno            varchar2(7) ,/**  �����к�  **/
	tlrno           varchar2(6) ,/**  ����Ա    **/
	sendco		varchar2(4) ,/**  �����߱��**/
	acbrno		varchar2(12),/**  �����߱��**/
	iftype		varchar2(1),/**  ֪ͨ���� 1 ����֪ͨ 2 �տ�֪ͨ 3 �����ִ 4 �տ��ִ **/
	dtlid		number(3)   ,/**  ��ϸ���	**/
	payno		varchar2(12),/**  �������к�	**/
	paymon		varchar2(6), /**  �Ʒ��·�	**/
	sdate		number(8), /**  �Ʒ���ʼ����	**/
	edate		number(8), /**  �Ʒ���ֹ����	**/
	aorm		varchar2(1),/**  �Զ�/�ֹ��۷� 0�Զ� 1 �ֹ�**/
	rebakno		varchar2(12),/**  �����к�**/
	rebamt		number(17,2),/**  �����ܽ��**/
	rebrate		varchar2(6), /**  ֱ�Ӳ����߷������� **/
	rebnum		varchar2(6),/**  ������¼��**/
	seqno		number(6)   ,/**  ������¼���	**/
	pkgno		varchar2(3),/**  ��ѷ����嵥֮�������ͺ�	**/
	totnum		varchar2(8) ,/**  ��ѷ����嵥֮�ܱ���	**/
	brate		varchar2(6),/**  ��ѷ����嵥֮�����������ʵ�λ����/��**/
	price		varchar2(6),/**  ��ѷ����嵥֮�Ʒѵ��۵�λ����/�� 	**/
	txamt		varchar2(15)  /**  ��ѷ����嵥֮�Ʒѽ��  ��λ����		**/
)tablespace users;
create unique index lv_c554r_idx on lv_c554r(in_date,dtlid,seqno)tablespace indx;

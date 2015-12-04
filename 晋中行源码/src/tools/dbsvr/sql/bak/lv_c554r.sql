/**	CMT554С����ⷵ��֪ͨ���ĵǼǲ� **/
drop index lv_c554r_idx;
drop index lv_c554r_idx2;
drop table lv_c554r;
create table lv_c554r(
	cmtno      	Varchar2(3),/**  Cmt���	**/
	in_date		number(8),/**  ����ϵͳ����**/
	wt_date		number(8),/**  ί������	**/
	brno            Varchar2(7) ,/**  �����к�  **/
	tlrno           Varchar2(6) ,/**  ����Ա    **/
	sendco		Varchar2(4) ,/**  �����߱��**/
	acbrno		Varchar2(12),/**  �����߱��**/
	iftype		Varchar2(1),/**  ֪ͨ���� 1 ����֪ͨ 2 �տ�֪ͨ 3 �����ִ 4 �տ��ִ **/
	dtlid		Number(3)   ,/**  ��ϸ���	**/
	payno		Varchar2(12),/**  �������к�	**/
	paymon		Varchar2(6), /**  �Ʒ��·�	**/
	sdate		Varchar2(8), /**  �Ʒ���ʼ����	**/
	edate		Varchar2(8), /**  �Ʒ���ֹ����	**/
	aorm		Varchar2(1),/**  �Զ�/�ֹ��۷� 0�Զ� 1 �ֹ�**/
	rebakno		Varchar2(12),/**  �����к�**/
	rebamt		Number(17,2),/**  �����ܽ��**/
	rebrate		Varchar2(6), /**  ֱ�Ӳ����߷������� **/
	rebnum		Varchar2(6),/**  ������¼��**/
	seqno		Number(6)   ,/**  ������¼���	**/
	pkgno		Varchar2(3),/**  ��ѷ����嵥֮�������ͺ�	**/
	totnum		Varchar2(8) ,/**  ��ѷ����嵥֮�ܱ���	**/
	brate		Varchar2(6),/**  ��ѷ����嵥֮�����������ʵ�λ����/��**/
	price		Varchar2(6),/**  ��ѷ����嵥֮�Ʒѵ��۵�λ����/�� 	**/
	txamt		Varchar2(15)  /**  ��ѷ����嵥֮�Ʒѽ��  ��λ����		**/
)tablespace users;
create unique index lv_c554r_idx on lv_c554r(in_date,dtlid,seqno)tablespace indx;

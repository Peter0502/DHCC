drop index hv_uniontab_idx;
drop  table hv_uniontab;
create table hv_uniontab(
	or_br_no varchar2(12),         /*֧��������    ���ר��ȫ���ţ���֧��ϵͳ�к�*/
	br_sts varchar2(1),            /*״̬��־ 0����Чǰ 1����Ч 2��ע�� */
	br_categ varchar2(2),          /*���01--ֱ�Ӳ�������02--ֱ�Ӳ������ 03--EISת������04--ֱ�Ӳ�����ҵ����05-��������ֱ�Ӳ�����06-���������Ӳ�����07--��Ӳ�����08-δ��������ֱ�Ӳ�����(ծȯ)*/
	br_code varchar2(3),           /*�б����    */
	sendco varchar2(4),            /*�������Ĵ���*/
	dir_br_code varchar2(12),      /*����ֱ�Ӳ������к�   ���Ϊ'07'�ı�ѡ */
	node_code varchar2(5),         /*���ڽڵ�������Ϊ'01','02','03','04','05','08'�ı�ѡ */ 
	superior varchar2(130),        /*�����ϼ�������  ���Ϊ'01','02', '04'�ı�ѡ  */
	pb_code varchar2(12),          /*�������д������Ϊ'02','03','04','05','06'�ı�ѡ*/
	city_code varchar2(4),         /*���ڳ������Ϊ'01','02','03','04','05','07','08'�ı�ѡ */
	acct_sts varchar2(1),          /*�����ʻ�״̬1-������2-����0-��ǿ���3-������4-ע�����Ϊ'01','02','03','04','05','06'�ı�ѡ*/
	acctsts_chg_dt varchar2(8),    /*�����ʻ�״̬������������Ϊ'01'��'02', '03'��'04'��'05', '06'�ı�ѡ*/
	acctsts_chg_tm varchar2(14),   /*�����ʻ�״̬������ں�ʱ�����Ϊ'01'��'02', '03'��'04'��'05', '06'�ı�ѡ*/
	br_name_f varchar2(60),        /*����ȫ��    */
	br_name_s varchar2(20),        /*�������    */
	addr varchar2(60),             /*��ַ        */
	phone varchar2(30),            /*�绰/���   */
	email varchar2(30),            /*�����ʼ���ַ*/
	postcode varchar2(6),          /*�ʱ�        */
	start_date varchar2(8),        /*��Ч����    */
	end_date varchar2(8),          /*ʧЧ����    */
	chg_date varchar2(8),          /*��������    */
	fax varchar2(20),              /*����        */
	telegraph varchar2(8),         /*�籨        */
	qs_no varchar2(12),            /*�����к�    */
	acct_lvl varchar2(1),          /*�ʻ����� 1-ֱ��2-����� */
	obr_no_attr varchar2(1),       /*�Է������� 1-�����ִ�֧��ϵͳ2--�����������ϵͳ3--��������֧��ϵͳ4--ע��*/
	upd_type varchar2(1),          /*������²��� 1:���2������3��ע��  */
	rec_upd_no varchar2(8),        /*��¼��������  ��ݣ���ţ� YYYYXXXX */
	km_actno varchar2(16)          /*������Ŀ�ʻ��ʺ�  */
)tablespace users;
create	 unique index hv_uniontab_idx on hv_uniontab( or_br_no)tablespace indx;

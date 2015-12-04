/******************************************************************************/
/* Table Name:  ccomcust - ��ҵ�ͻ�����		                                  */
/* Location:    On center server                                              */
/* Index:       cif_no,busslic											      */
/* Create by Atlantics						2002/01/15						  */
/******************************************************************************/
drop index czcust_idx;
drop table czcust;
CREATE TABLE czcust (
	opnbr		varchar2(7),	/* opening branch */
	ac_id   number(10),	/* address */
	ac_no   varchar(19),/* �ʺ� */
	cif_no  number(10),	/* custtomer number */
	name		varchar2(80),
	pact_no varchar2(20),/* ��ݺ� */
	avbal   number(16,2),
	opday       varchar2(8),
	endday      varchar2(8),
	id_no   varchar2(20),	/* bussiness number	*/
	crd_no      varchar2(20),  /* ����֤�� */
	tablename   varchar2(20),    /* ����*/
	flag    varchar2(1),    /* 1�������ļ��еõ��ģ�0��ʾ���ݿͻ����ҵ����µ��ʺ� */
	id_type varchar2(1),
	ncif_no number(9)   /* new custtomer */
) tablespace users;
CREATE UNIQUE INDEX czcust_idx on czcust(ac_no,flag) tablespace indx;

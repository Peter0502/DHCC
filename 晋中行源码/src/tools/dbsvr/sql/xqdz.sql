drop table xqdz;
create table xqdz
  (
    xqdz100 varchar2(24), /* С�����˺� */
    xqdz101 varchar2(10), /* С����� */
    xqdz110 varchar2(8),  /* �������� */
    xqdz111 varchar2(8),  /* ���׻��� */
    xqdz15 varchar2(60),  /* С������ */
    xqdz16 varchar2(60),  /* С����ַ */
    xqdz17 varchar2(20),  /* ��ϵ�� */
    xqdz18 varchar2(20),  /* ��ϵ�绰 */
    xqdz20 number(9),     /* �Ǽ����� */
    xqdz21 number(9), 	  /* �Ǽ�ʱ�� */
    xqdz22 varchar2(6),	  /* �Ǽǹ�Ա */
    xqdz23 number(9),
    xqdz24 number(9),
    xqdz25 varchar2(6),
    xqdz30 varchar2(1),
    xqdz31 varchar2(60),  /* С�������� */
    primary key (xqdz100,xqdz101)
  );

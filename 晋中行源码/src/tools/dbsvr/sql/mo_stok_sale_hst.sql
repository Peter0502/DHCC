// ��ɵǼǲ���mo_stok_sale_hst
//Table index : mo_stok_sale_hst_idx (stokno) 

create table mo_stok_sale_hst(
stokno					varchar2(18),					//�ɶ�֤��	
name						varchar2(60),					//����	
befstokquant		number(16,2),					//���ǰ����	
befbal					number(16,2),					//���ǰ���	
rate						number(7,2),					//��ɱ���	
stokquant				number(16,2),					//�����	
txamt						number(16,2),					//��ɽ��	
stokamt					number(16,2),					//��ɵ���	
payamt					number(16,2),					//Ӧ�ɽ��	
aftstokquant		number(16,2),					//��ɺ����	
aftbal					number(16,2),					//��ɺ���	
txdate					varchar2(8)	,					//��������	
txday						varchar2(8)	,					//����ʱ��	
stat						varchar2(1)						//���״̬	1����ɣ�2������
) tablespace users
;

create index mo_stok_sale_hst_idx on mo_stok_sale_hst(stokno) tablespace indx;

// �����Ǽǲ���mo_stok_intr_hst
// Table index : mo_stok_intr_hst_idx (stokno)
create table  mo_stok_intr_hst (
stokno					varchar2(18),					//�ɶ�֤��	         
name						varchar2(60),					//����	               
stoknum					number(16,2),					//����	             
bal							number(16,2),					//���	                 
rate						number(7,2)	,					//����	                 
intr						number(16,2),					//Ӧ����Ϣ	           
taxrate					number(16,2),					//˰��	             
intrtax					number(16,2),					//��Ϣ˰	           
payintr					number(16,2),					//ʵ����Ϣ	         
txdate					varchar2(8)	,					//��������	           
txtime					varchar2(8)	,					//����ʱ��	           
bookcnt					number(4,0)						//����λ��	? ? ? ? ? ?

) tablespace users
;
create  index mo_stok_intr_hstt_idx on mo_stok_intr_hst(stokno) tablespace indx;
drop index mo_ppe_idx;
drop table mo_ppe;
CREATE TABLE mo_ppe(
				ppeno                 	varchar2(19) NOT NULL ,/*�̶��ʲ����*/                                    
				ppename               	varchar2(80)          ,/*�̶��ʲ�����*/                                         
				opdate                  varchar2(8)           ,/*��������*/                                             
				type                    varchar2(1)           ,/*���*/                                                
				spemodel                varchar2(20)          ,/*����ͺ�*/                                             
				quant		                number(10,2)          ,/*����*/                                                 
				oriamt		            	number(16,2)          ,/*�̶��ʲ�ԭֵ*/                                         
				chgactno              	varchar2(19)     		  ,/*�̶��ʲ���������ֵ�˺�*/                               
				chgamt                	number(16,2)          ,/*��������ֵ*/                                           
				depramtyear           	number(16,2)     		  ,/*�̶��ʲ����������۾�*/                                 
				depractno             	varchar2(19)     		  ,/*�̶��ʲ������۾��ʺ�*/                                 
				depramtttl            	number(16,2)     		  ,/*�̶��ʲ��ۼ��۾�*/                                     
				remamt		            	number(16,2)     		  ,/*�̶��ʲ���ֵ*/                                         
				deprstat	            	varchar2(1)      		  ,/*�ʲ����۾�״��*/                                       
				stat                  	varchar2(1)      		  ,/*�ʲ�״̬*/                                             
				moden	                	varchar2(1)      		  ,/*�۾ɷ�ʽ*/                                             
				remainratio           	number(6,3)      		  ,/*��ֵ��*/                                               
				period                	varchar2(1)      		  ,/*�۾�����*/                                             
				term		              	number(5,2)      		  ,/*�۾�����*/                                             
        usedterm                number(5,2)           ,/*��������*/                                            
        usedquant               number(16,2)          ,/*ʵ����ɹ�����*/                                       
        ttlquant                number(16,2)          ,/*�ܹ�����*/                                             
        ttlcnt                  number(6,0)           ,/*�ۼ��۾ɴ��� */                                        
        deprradio               number(6,3)           ,/*�۾���*/                                               
        asstype                 varchar2(1)           ,/*�ʲ�����*/                                             
        deprtype                varchar2(1)           ,/*��̯��ʽ */                                            
        placebrno               varchar2(7)           ,/*���֧���к� */                                        
        addr		                varchar2(200)         ,/*��ŵص�*/                                             
        ppestat		              varchar2(20)          ,/*�ʲ�״��*/                                             
        chkdate                 varchar2(8)           ,/*���һ���̵�����*/                                     
        brf	                    varchar2(200)         /*��ע*/                                                 
)tablespace users;
CREATE UNIQUE INDEX mo_ppe_idx on mo_ppe (ppeno) tablespace indx;
        
        
        
        
        
        
        
        
        
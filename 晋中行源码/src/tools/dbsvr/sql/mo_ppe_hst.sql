drop index mo_ppe_hst_idx;
drop table mo_ppe_hst;
CREATE TABLE mo_ppe_hst(
				ppeno                 varchar2(19)   NOT NULL ,/*固定资产编号*/                                                                        
				ppename               varchar2(80)          ,/*固定资产名称*/                                                                             
				opdate                varchar2(8)           ,/*建帐日期*/                                                                                 
				type                  varchar2(1)           ,/*类别*/                                                                                    
				spemodel              varchar2(20)          ,/*规格型号*/                                                                                 
				quant		              number(10,2)          ,/*数量*/                                                                                     
				oriamt		            number(16,2)          ,/*固定资产原值*/                                                                             
				chgactno              varchar2(19)     		  ,/*固定资产评估增减值账号*/                                                             
				chgamt                number(16,2)          ,/*评估增减值*/                                                                             
				depramtyear           number(16,2)     		  ,/*固定资产本年已提折旧*/                                                             
				depractno             varchar2(19)     		  ,/*固定资产已提折旧帐号*/                                                             
				depramtttl            number(16,2)     		  ,/*固定资产累计折旧*/                                                                         
				remamt		            number(16,2)     		  ,/*固定资产净值*/                                                                             
				deprstat	            varchar2(1)      		  ,/*资产的折旧状况*/                                                                           
				stat                  varchar2(1)      		  ,/*资产状态*/                                                                                 
				moden	                varchar2(1)      		  ,/*折旧方式*/                                                                                 
				remainratio           number(6,3)      		  ,/*残值率*/                                                                                   
				period                varchar2(1)      		  ,/*折旧周期*/                                                                                 
				term		              number(5,2)      		  ,/*折旧期限*/                                                                                 
        usedterm              number(5,2)           ,/*已用期限*/                                                                                
        usedquant             number(16,2)          ,/*实际完成工作量*/                                                                           
        ttlquant              number(16,2)          ,/*总工作量*/                                                                                 
        ttlcnt                number(6,0)           ,/*累计折旧次数*/                                                                             
        deprradio             number(6,3)           ,/*折旧率*/                                                                                   
        placebrno             varchar2(7)           ,/*存放支行行号*/                                                                             
        addr		              varchar2(200)         ,/*存放地点*/                                                                                 
        ppestat		            varchar2(20)          ,/*资产状况*/                                                                                 
        brf	                  varchar2(200)         ,/*备注*/                                                                                     
        dealtype              varchar2(1)           ,/*操作类型*/                                                                                 
        chkrst                varchar2(1)           ,/*盘点结果*/                                                                                 
        chkchgamt             number(16,2)          ,/*盘点增减额*/                                           
        Txdate                varchar2(8)           ,/*交易日期*/                                             
        Txtime                varchar2(8)           ,/*交易时间*/                                             
        tranceno              number(10)            ,/*流水号*/                                               
        trancecnt             number(10)            ,/*流水笔次*/                                             
        txbrno                varchar2(7)           ,/*交易支行号*/                                           
        tel                   varchar2(6)            /*操作员号*/                                             
)tablespace users;                                  
CREATE INDEX mo_ppe_hst_idx on mo_ppe_hst (ppeno) tablespace indx;
                     
                     
                     
                     
        
        
        
        
        
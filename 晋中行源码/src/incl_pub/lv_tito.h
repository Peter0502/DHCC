#ifndef INCLUDE_LV_TITO
#define INCLUDE_LV_TITO
/**���涨��tis���Ŀɱ䲿��**/
struct {/**���ҵ��*/
	char Respday[8];/*�ڴ�Ӧ�����ڣ����˲���*/
	char Ptype[2];	/**Ʊ������**/
	char Pdate[8];	/**Ʊ������**/
	char Pnum[8];	/**Ʊ�ݺ���**/
	char Ptxamt[15];/**�⳥���**/
	char Jtxamt[15];/**�ܸ����**/
	char Owtday[8];	/**ԭί������**/
	char Otxnum[8];	/**ԭҵ����**/
	char Oorderno[8];	/**ԭ֧���������**/
	char Thretcode[2];	/**�˻�ԭ�����**/
	char Rcpstat[2];	/**��ִ״̬**/
	char Resp1[8];		/**mbfeӦ��**/
	char Resp2[8];		/*ccpcӦ��**/
}PAY_IN_DR_AREA;
/****Modified by SSH,2005.7.28,ʹ����ͬ��tis�ṹ***/
#define PAY_IN_WT_AREA PAY_IN_DR_AREA 
#define PAY_IN_TSCF_AREA PAY_IN_DR_AREA 
#define PAY_IN_DJTH_AREA PAY_IN_DR_AREA 
#define PAY_IN_RECEIPT_AREA PAY_IN_DR_AREA 
#define PAY_IN_RESP_AREA PAY_IN_DR_AREA 
typedef struct{
	char	actype[1];		/**�տ��ʺ�����**/
	char	pactype[1];		/**�����ʺ�����**/
	char	pswdtype[1];		/**��������**/
	char	pswd[8];		/**����**/
}PAY_IN_TCTD_AREA;			/**ͨ��ͨ��**/
typedef struct{
	char	payunit[60];	/**����(˰)��λ**/
	char	payterm[16];	/**����(˰)�ڼ�**/
	char	paytype[2];		/**����(˰)����**/
}PAY_IN_FEE_AREA;/**���ѡ���˰**/



	
	





#endif	/**end INCLUDE_LV_TITO**/

#include"ejfno.h"
#include "find_debug.h"
void main(){
	int no;
	initEjfnoFile(1,"20040101");
	no=fetchEjfno(1,EJF_IDTNO);
	printf("idtno=%d\n",no);
	rollbackEjfno(1);
	printf("rollback\n");
	no=fetchEjfno(1,EJF_IDTNO);
    printf("idtno=%d\n",no);
	commitEjfno(1);
	printf("commit\n");
	no=fetchEjfno(1,EJF_IDTNO);
    printf("idtno=%d\n",no);
	commitEjfno(1);
	printf("commit\n");
	/*******************/
	no=fetchEjfno(1,EJF_CANCNO);
	printf("cancno=%d\n",no);
	rollbackEjfno(1);
	printf("rollback\n");
	no=fetchEjfno(1,EJF_CANCNO);
    printf("cancno=%d\n",no);
	commitEjfno(1);
	printf("commit\n");
	no=fetchEjfno(1,EJF_CANCNO);
    printf("cancno=%d\n",no);
	commitEjfno(1);
	printf("commit\n");
}

#include "find_debug.h" 
/*************************************************
* �� �� ��:    pubf_comm_TlsrNo.c
* ����������
*              get_tlsrno     ������ˮ��
*
* ��    ��:    rob
* ������ڣ�   2003��12��16��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:SSH,�޸���ˮ�Ŵ�������ȡ
*        �޸�����:
*     2.
*************************************************/
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
/*#include <sys/mode.h>*/
#include <sys/types.h>

static int shm_id,sem_id;
static struct sembuf p={0,-1,0},v={0,1,0};
int init_id()
{
    key_t shm_key,sem_key;
    static long *par;
    union semun {
         int val;
         struct semid_ds *buf;
         unsigned short *array;
    } arg;
	/****SSH:���в��ó�ʼ��****/
	return(0);

    shm_key = ftok((char *)getenv("HOME"),'h');
    sem_key = ftok((char *)getenv("HOME"),'e');

    if (-1 == (shm_id = shmget(shm_key ,sizeof(long),IPC_EXCL|IPC_CREAT|0700)))
    {
        if (-1 == (shm_id = shmget(shm_key ,sizeof(long),IPC_CREAT)))
        {
            printf("Get shm_key err\n");
            return (-1);
        }
    }
    else{
        if ((long *)-1 == (par = (long *)shmat(shm_id,(char *)0,0700)))
        {
            printf("init - shmat  err\n");
            return (-1);
        }
        *par = 0L;

        if (-1 == (shmdt((char *)par)))
        {
            printf("init - shmdt  err\n");
            return (-1);
        }
    }
    if (-1 == (sem_id = semget(sem_key,1,IPC_EXCL|IPC_CREAT|0700)))
    {
        if (-1 == (sem_id = semget(sem_key,1,IPC_CREAT)))
        {
            printf("Get sem_key err\n");
            return(-1);
        }
    }
    else {
        arg.val = 1;
        if (-1 == (semctl(sem_id, 0 ,SETVAL,arg)))
        {
            printf("Set sectl  err\n");
            return (-1);
        }
    } 
    return(0);
}

int move_id()
{
	/****SSH:���ù����ڴ���****/
	return(0);
    if (shmctl(shm_id,IPC_RMID,NULL) == -1)
    {
        printf("error in closing shmctl...\n");
        return(-1);
    }
    if (semctl(sem_id,0,IPC_RMID,NULL) <0)
    {
        printf("error in closing semctl...\n");
        return(-1);
    }
    return (0);
}
long get_tlsrno()
{
    static long *par;
    static long tlsrno;
	/****SSH:ʹ������seq_trace_no****/
	return(nGetTraceSeqn());

    if ( -1 == init_id() )
        return(-1);

    semop(sem_id,&p,1);
    if ((long *)-1 == (par = (long *)shmat(shm_id,(char *)0,0600)))
    {
        printf("In Get  shmat  err\n");
        semop(sem_id,&v,1);
        return (-1);
    }
    *par += 1;
    tlsrno = *par ;

    if (-1 == (shmdt((char *)par)))
    {
        printf("In Get  shmdt err\n");
        semop(sem_id,&v,1);
        return (-1);
    }

    semop(sem_id,&v,1);

    return(tlsrno) ;
}

long set_tlsrno(long tlsrno)
{
    long *par;
	/****SSH:ʹ������seq_trace_no****/
	return(nSetTraceSeqn(tlsrno));

    if ( -1 == init_id())
    {
        return(-1);
    }

    if (0> (par = (long *)shmat(shm_id,(char *)0,0)))
        return(-1);

    *par = tlsrno;

    if (-1 == (shmdt((char *)par)))
    {
        return (-1);
    }

    return(tlsrno) ;
}

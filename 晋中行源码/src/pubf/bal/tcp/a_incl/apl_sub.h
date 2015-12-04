/*************************************************************************/
/*   FILENAME : apl_sub.h                                                */
/*   PURPOSE  : for APCTL client library apcslib.a , this client         */
/*              library is linked by UNIX client program which will      */
/*              not use APCTL but only link with APCTL API               */
/*************************************************************************/
ClientConnectId    conn_id;
char               service_name[30] = "";
char               fault_server[30] = "KWJ_SYS_FAULT";
char               client_name[30];
char               Xact_name[128];           /* 1993.8.22 from 30 to 128 */
RCBP               the_rcb;
RCBP               client_rcb;
void               TpNoWaitCallBack();
void               (*func)()        = NULL;
int                debug_flag;
long               Xact_id;                  /* 1993.3.12 for client     */
                                             /* 1993.5.16 short --> long */
char               Xact_obj[30];             /* library request wait     */

Boolean            requestXactWait();
ClientRequestId    requestXactNoWait();
Boolean            requestBLOBWait();
ClientRequestId    requestBLOBNoWait();

Boolean            (*request)(RCBP client_rcb)       = requestXactWait;
ClientRequestId    (*requestno)(RCBP client_rcb)     = requestXactNoWait;
Boolean            (*requestBLOB)(RCBP client_rcb)   = requestBLOBWait;
ClientRequestId    (*requestBLOBno)(RCBP client_rcb) = requestBLOBNoWait;


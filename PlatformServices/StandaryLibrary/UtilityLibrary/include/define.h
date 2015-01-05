#ifndef DEFINE_H
#define DEFINE_H

//--------------------------------�����豸����---------------------------------

#define TYPE_USER				1	// �û�
#define TYPE_TERMINAL			2	// ǰ��
#define TYPE_DISPATCH			3	// �ַ�
#define TYPE_STORE				4	// �洢
#define TYPE_CAMERA				5	// ����ͷ
#define TYPE_ALARM				6	// ������
#define TYPE_DISPLAY			7	// ��ʾǽ
#define TYPE_ALARMOUT			8	// �������

#define MEGAEYES_VERSION 257  //Э��汾��

//--------------------------------���������---------------------------------

#define ERR_LACKMEMORY			        301		// �ڴ治��
#define ERR_UNKNOWNUSER			        302		// δ֪�ͻ���
#define ERR_MSGFORMAT			        303		// ָ���ʽ����
#define ERR_RETFORMAT			        304		// ���ؽ����ʽ����
#define ERR_AUTHRET                     305		// ��֤Ȩ�޷��ؽ������
#define ERR_CONNECT                     306		// ͨѶ���Ӵ���
#define ERR_SEND                        307		// ͨѶ���ʹ���
#define ERR_RECV                        308		// ͨѶ���մ���
#define ERR_VERSION                       309		// Э��ͷ�汾����
#define ERR_RECV_CLOSE			        310		// ͨѶ���մ���,�Է��ر�
#define ERR_RECV_SOCK			        311		// ͨѶ���մ���socket��Ч
#define ERR_GETIP       		        312		// �õ��豸 IP ��ַʧ��
#define ERR_OPERSUCCESS			        313		// ����˷��ز������ɹ�
#define ERR_OPERSUCCESS_ONE             314     // ����˷��ز������ɹ�
#define ERR_OPERSUCCESS_TWO             315     // ����˷��ز������ɹ�
#define ERR_RECV_LENERR	                316		// Ԥ���䳤�ȴ���
#define ERR_INVALID_MESGID              317     // ���������id             
#define ERR_SERVICENAME                 318     // ��������ķ�������
#define ERR_CURL_INIT                   319     //��ʼ��curlʧ��
#define ERR_CURL_HTTP                   320     //����http�����������
#define ERR_UNKNOWNDISPLAY              321     //δ֪��ʾǽ
#define ERR_NULL_USERNAME               322     //�û���Ϊ��
#define ERR_OUTRANGE_USERNAME           323     //�û���̫��
#define ERR_NULL_MAC_ADDR               324     //MAC��ַΪ��
#define ERR_FORMAT_MAC_ADDR             325     //MAC��ַ��ʽ����

#define ERR_NOTEXISTSHISTORY            400		// ��������ʷ��Ƶ
#define ERR_PTZLOCKED			        401		// ��Դ�Ѿ�����������
#define ERR_TERMREGISTER		        402		// ǰ��ע��ʧ��
#define ERR_DISPATCHREGISTER            403		// �ַ�ע��ʧ��
#define ERR_HDLREGISTER			        404		// ������ע��ʧ��
#define ERR_NOTEXISTSDEV		        405		// �����ڸ��豸
#define ERR_REACHLIMITATION		        406		// ��Ƶ�����Ѵﵽ�������
#define ERR_LOGINLIMITATION		        407		// �û���¼��ʱ
#define ERR_NOTEXISTSCTRLPTZ            408		// �����ڸ���̨�Ŀ���
#define ERR_MUTEX                       409		// ������ʧ��
#define ERR_NOTEXISTSVIDEOSVR           410		// �����ڸ���Ƶ����
#define ERR_REALTIMESVRRET              411		// ����ʵʱ��Ƶʱǰ��/�ַ�������Ϣ����
#define ERR_STOPALARMEVENTMSG           412		// ����ֹͣ�������û�ָ���ʽ����
#define ERR_DISPREGJ2EERET		        413		// �ַ�ע��ʱ�� J2EE ���ش���
#define ERR_J2EESENDSESSIONID           414		// J2EE ���û���¼ʱ�����û��Ự ID ��ʽ����
#define ERR_COMMUNICATIONTOJ2EE         415		// ͬ J2EE ͨѶ��������ͨ�ýӿڣ�
#define ERR_VIDEOBEGIN_END		        416		// ��Ƶ��ʼ/ָֹͣ���ʽ����
#define ERR_TERMREGJ2EERET		        417		// ǰ��ע��ʱ J2EE ���ش���
#define ERR_TERMALARMBEGIN		        418		// ǰ�˱�����ʼʱָ���ʽ����
#define ERR_TERMALARMBEGINJ2EERET       419		// ǰ�˱�����ʼʱ J2EE ���ش���
#define ERR_TERMALARMEND		        420		// ǰ�˱�������ʱָ���ʽ����
#define ERR_REALTIMELOCKMEMORY		    421		// ����ʵʱ��Ƶʱ��Ƶ��Ϣ�ڴ�ز���
#define ERR_NOTEXISTSALARMEVENT		    422		// �����¼�����ʱ�����ڱ����¼�
#define ERR_STOREREGJ2EERET		        423		// �洢ע��ʧ��
#define ERR_FTPINFO			            424		// ��ȡftp��Ϣ����
#define ERR_STOREREGISTER               425     // �洢ע��ʧ��
#define ERR_INVALID_USERID              426     // ��Ч���û�id
#define ERR_USER_STATUS_NULL            427     // �û�״̬Ϊ��
#define ERR_INVALID_NAME                428     // namingΪ��  
#define ERR_UNKOWN_NODETYPE             429
#define ERR_NOEXITCACHE                 430
#define ERR_REACHSOFTLIMITATION		    431		// ��Ƶ�����Ѵﵽ�������
#define ERR_INVALID_XML					432			// ��Ч��XML
#define ERR_CENTER_NOT_RESP			433       // ������Ӧ��
#define ERR_CENTER_RESP_XML_ERR   434		// ����Ӧ��xml��ʽ����
#define ERR_INVALID_STRORAGE_ID	435	   // ��Ч�����Ĵ洢ҵ��ID��

#define ERR_NO_HANDLER                  438
#define ERR_TIMEOUT                     440     //����ʱ
#define ERR_ALARM_TIMEOUT               441     //��������ʱ���ڵı�?
#define ERR_REMOTEIP                    460     //�豸Naming�е�Զ��IP������127.0.0.1

#define ERR_NOACCESS_DISPLAY	        500		//û��Ȩ�޲�����ʾǽ
#define ERR_NOACCESS_REALTIME	        501		//û��Ȩ�޲���ʵʱ��Ƶ
#define ERR_NOACCESS_HISTORY	        502		//û��Ȩ�޲�����ʷ��Ƶ
#define ERR_SERNOBOOT			        503		//����û������

#define ERR_CONNECT_FAULT		        510		// ͨѶ���ӹ���
#define ERR_DEVLIST_EMPTY               511 
#define ERR_NO_VIDEO_ROUTE              512     //û�з�����Ƶ·��
#define ERR_VIDEO_ROUTE_FAULT           513     //��Ƶ·���й���
#define ERR_NULL_NODEID                 514

#define ERR_WOULDBLOCK                  515 
#define ERR_PERSISTENCE_CONN            516 

#define ERR_USERDONTREF_PRIVATERELAY    520     //û��ָ���ַ�
#define ERR_USERDONTUSE_PRIVATERELAY    521     //û��ʹ�÷ַ�
#define ERR_USERERRREF_PRIVATERELAY     522     //ָ���ķַ�������
#define ERR_NOUSE_DISPATCH			    523	    // û�п��õķַ�

#define ERR_CLUSTER_CONN                600     // �����ڵ�ͨ��ʧ��
#define ERR_CLUSTER_TRA                 601     // ���ڵ㴦���Ĭ�ϴ����
#define ERR_CLUSTER_TRA_INVALID_MESGID  602
#define ERR_CLUSTER_TRA_SAMENODE        603     // �豸���ڽڵ�ͷ�������Ľڵ�����ͬ��

#define ERR_NOTEXISTSUSER		        701		// �����ڸ��û� add by ydx
#define ERR_GETDISPATCHIP   	        702		// ��ȡ�ַ�IPʧ��
#define ERR_INVALID_STOREID             703     // ��Ч�Ĵ洢id

#define ERR_NOT_FOUND_XML		800		//��Ч��XML

#define ERR_UNKONWN_MSGID               80000001    //��ʶ���ָ��
#define ERR_DISUSE_MSGID                80000002    //������ָ��

//--------------------------------ָ���Ŷ���---------------------------------
#define CMD_MEGA_SERVER_REG_REQ         5002        //ע�������(mega eyes)
#define CMD_MEGA_SERVER_REG_REP         5003

#define CMD_MSG_SERVER_REG_REQ          150012      //ע����Ϣ������(message private private)
#define CMD_MSG_SERVER_REG_REP          150013

#define CMD_MSG_SERVER_HEARTBEAT_REQ    150014      //��Ϣ����������
#define CMD_MSG_SERVER_HEARTBEAT_REP    150015

#define CMD_DOOR_STATUS_REPORT_REQ      20010       //��״̬�ϱ�
#define CMD_DOOR_STATUS_REPORT_REP      20011

#define CMD_SWIPING_CARD_REPORT_REQ     20006       //ˢ���¼��ϱ�
#define CMD_SWIPING_CARD_REPORT_REP     20007

#define CMD_AUTH_REPORT_REQ             150016      //��֤�¼��ϱ�
#define CMD_AUTH_REPORT_REP             150017

#define CMD_FINGERPRINT_REPORT_REQ      150018      //ָ���¼��ϱ�
#define CMD_FINGERPRINT_REPORT_REP      150019

#define CMD_BW_UNREGISTER_REQ           190002      //ע������(bandwidth)����
#define CMD_BW_UNREGISTER_REP           190003

#define CMD_BW_UPLOAD_PAYLOAD_REQ       190004      //�ϱ�����
#define CMD_BW_UPLOAD_PAYLOAD_REP       190005 

#define CMD_BW_VIDEO_VALIDATE_REQ       190006      //��Ƶ������֤
#define CMD_BW_VIDEO_VALIDATE_REP       190007

#define CMD_BW_UPDATE_COUNT_REQ         190008      //���´���ռ�ü���
#define CMD_BW_UPDATE_COUNT_REP         190009

#define CMD_BW_GET_BANK_INFO_REQ        190010      //��ȡ����״̬��Ϣ
#define CMD_BW_GET_BANK_INFO_REP        190011

#define CMD_UNHANDLE_ALARM_REQ          190014      //͸��δ������
#define CMD_UNHANDLE_ALARM_REP          190015 

#define CMD_ISSUED_PUSH_MESSAGE_REQ     190016      
#define CMD_ISSUED_PUSH_MESSAGE_REP     190017      //�������·���Ϣ 

#define CMD_VIDEO_DISCONNECT_REQ        110004      //��Ƶ���ӶϿ�
#define CMD_VIDEO_DISCONNECT_REP        110005

#define CMD_DOOR_SERVER_REG_REQ         20000       //�Ž�����ע��
#define CMD_DOOR_SERVER_REG_REP         20001

#define CMD_DOOR_MULTI_TIMEOUT_REQ      20004       //�����Ž��࿨ˢ����ʱʱ��
#define CMD_DOOR_MULTI_TIMEOUT_REP      20005   

#define CMD_DOOR_REMOTE_OPEN_REQ        20008       //Զ�̿���
#define CMD_DOOR_REMOTE_OPEN_REP        20009

#define CMD_DOOR_OFFLINELOG_REPORT_REQ  20012       //����ˢ����־�ϱ�
#define CMD_DOOR_OFFLINELOG_REPORT_REP  20013

#define CMD_DOOR_SET_EXTRA_CARD_REQ     20014       //����Ѳ��/в�ȿ�
#define CMD_DOOR_SET_EXTRA_CARD_REP     20015 

#define CMD_DOOR_DEL_EXTRA_CARD_REQ     20016       //���Ѳ��/в�ȿ�
#define CMD_DOOR_DEL_EXTRA_CARD_REP     20017

#define CMD_DOOR_STRATEGY_ISSUED_REQ    20018       //�����·�
#define CMD_DOOR_STRATEGY_ISSUED_REP    20019 

#define CMD_DOOR_TIME_RANGE_ISSUED_REQ  20024       //�Ž�ˢ��ʱ��β����·�
#define CMD_DOOR_TIME_RANGE_ISSUED_REP  20025

#define CMD_DOOR_CARD_ID_ISSUED_REQ     20020       //�Ž������·�
#define CMD_DOOR_CARD_ID_ISSUED_REP     20021

#define CMD_DOOR_DEL_CARD_ID_REQ        20022       //ɾ���Ž�����
#define CMD_DOOR_DEL_CARD_ID_REP        20023

#define CMD_DOOR_DEL_TIME_RANGE_REQ     20026       //ɾ��ʱ��β���
#define CMD_DOOR_DEL_TIME_RANGE_REP     20027

//--------------------------------����ָ��---------------------------------
#define MSG_HEARTBEAT			        2		// ����
#define MSG_HEARTBEAT_RESP     3     //�����ظ�
#define MSG_HEARTBEAT_TICKET		4 
#define MSG_HEARTBEAT_SYSSTATUS         20
#define MSG_HEARTBEAT_CAMSTATUS         22
#define MSG_PROTOCOLTRANSTOR_HEART		40000 //Эת����ָ��

//--------------------------------��Ƶ�豸ָ��---------------------------------
#define MSG_FAULTNOTIFY_NEW		1100	// ��J2EE���͹���֪ͨ

#define MSG_REGTERM			    2002	//ǰ��ע��
#define MSG_REGTERM_RESP		2003	//ǰ��ע���Ӧ

#define MSG_PERFORMANCE_THRESHOLD	2004 // ���ܷ�ֵ

//�ַ�������������ָ����
#define MSG_DISPATCHREGISTER		3002	// �ַ�ע��
#define MSG_REGDISPATCH_RESP    3003   //�ַ�ע��ظ�

#define MSG_REGSTORE			4002 //ǰ��֧�ֱ��ش洢ע������
#define MSG_REGSTORE_RESP		4003

#define MSG_DEVREALTIME		    5010	//������ʵʱ��Ƶ
#define MSG_DEVREALTIME_RESP	5011	//����ʵʱ��Ƶ��Ӧ
#define MSG_STORESER_QUERY      1200    // �û�������ʷ��Ƶ
#define REAL_VIDEO_CONN       	8000	//	
#define REAL_VIDEO_CONN_RESP    8001	//
#define REAL_VIDEOHEAD_SEND   	8002	//
#define REAL_VIDEOSTREAM_SEND	8004    //

#define MSG_USERHISTORY			5004	//��ѯ��ʷ��Ƶ�б�
#define MSG_USERHISTORY_RESP	5005	//��ѯ��ʷ��Ƶ�б��Ӧ
#define HISTORY_VIDEO_CONN      8300	//
#define HISTORY_VIDEO_CONN_RESP 8301	//
#define HISTORY_Data_SEND   	8303	//
#define HISTORY_VIDEO_HEART   	8304	//

#define MSG_ALARM_NOTIFY            5214    //�����������������֪ͨ
#define MSG_USERSETVIDEOPARAM		5032	// �û�������Ƶ��������
#define MSG_USERGETVIDEOPARAM		5034	// �û��õ���Ƶ��������
#define MSG_CENTERSTORAGE_QUERY	5408	// ���Ĵ洢��Ƶ��ѯ
#define MSG_CENTERSTORAGE_RESP		5409    // ���Ĵ洢��ƵӦ��
#define MSG_CENTERSTORAGE_REQ		5410   // ���Ĵ洢��Ƶ����

#define MSG_USERSETALARMOUTSTATE	5064
#define MSG_USERGETALARMOUTSTATE	5066
#define MSG_REMOTE_SETTING          5046
#define MSG_USERSETMOTION_PLAN          5080    //�����ƶ����Ĳ�����ʱ��
#define MSG_USERGETMOTION_PLAN          5082    //��ѯ�ƶ����Ĳ�����ʱ��
#define MSG_USERSETGPIN_PLAN            5084    //���ñ��������豸�Ĳ�����ʱ��
#define MSG_USERGETGPIN_PLAN            5086    //��ѯ���������豸�Ĳ�����ʱ��
#define MSG_USERGETPTZSTATUS		5042	// �û��õ���̨״̬
#define MSG_USERPTZ			5006	// �û�������̨
#define MSG_USERPTZ_CONTROL	8200    // ��̨����ָ��
#define MSG_USERPTZ_CONTROL_STOP   8202  // ֹͣ��̨����
#define MSG_USERPTZ_CLEARPRESET		 8208  // ���Ԥ��λ
#define MSG_USERPTZ_SETPRESET				8210	// ����Ԥ��λ
#define MSG_USERPTZ_CLEARALLPRESET		8212	// �������Ԥ��λ
#define MSG_USERPTZ_ZOOM						8302  // �佹
#define MSG_USERPTZ_ZOOM_STOP			8308  // ֹͣ�佹

#define MSG_VOICETALK_QUERY		6112		// �û���ѯ�����Խ���Ϣ
#define MSG_VOICETALK_RESP			6113		// �����Խ�Ӧ����Ϣ
#define MSG_VOICETALK_HEAD		6114			// �����Խ���ͷ
#define MSG_DEVVOICETALK_REQ			6116		// ��������
#define MSG_DEVVOICETALK_RESP			6117        // ��������Ӧ����Ϣ

//--------------------------------�����豸ָ��---------------------------------
#define MSG_DISPLAY_REGISTER	6000	 //ע��
#define MSG_DISPLAY_REALTIME	6002	 //ʵʱ��Ƶ
#define MSG_DISPLAY_HISTORY		6006	 //��ʷ��Ƶ      
#define MSG_DISPLAY_RESPONSE_STATUS	6008 //����״̬��Ϣ����Ӧ״̬��ѯMSG_USER_QUERY_DISPLAY_STATUS

//---------------------�ͻ��˷����Ķ���ʾǽ�Ŀ���ָ��   �ͻ���-->���������-------------------
#define MSG_USERREALTIME_TO_DISPLAY	5100	// 1.1 ʵʱ��Ƶ
#define MSG_USERLOOP_TO_DISPLAY		5102	// 1.2 ʵʱ��ѯ
#define MSG_USERHISTORY_TO_DISPLAY	5104	// 1.3 ��ʷ��Ƶ
#define MSG_USER_STOP_DISPLAY_PLAY	5106	// 1.4 ֹͣ����
#define MSG_USER_QUERY_DISPLAY_STATUS	5108	// 1.5 ״̬��ѯ

//-----------------------------�洢ͨѶ������������ָ����--------------------------------------
#define MSG_STOREREGISTER_DV            4102    // DV�洢������ע��

//----------------------------------vod���ָ��-----------------------------------------------------
#define MSG_RECORD_QUERY                5408    //¼���ѯ         client->store   
#define MSG_CU_VOD                      5410    //�û�����¼��㲥
#define MSG_VOD_GETCAMCOUNT             4012    //GET CAMERA COUNT store->center
#define MSG_VOD_GETCAMSTOREINFO         4016    //GET CAMERA STORE INFO store->center
#define MSG_VOD_STOREPLANUPDATE         4024    //�洢�ƻ����֪ͨ center->store
#define MSG_VOD_RECORDSTATUS            4020    //�洢״����ѯ client->store
#define MSG_VOD_RECORD_UPLOAD           4028    //ǰ��֪ͨ���Ĵ洢�ϴ�¼�� terminal->store
#define MSG_VOD_CLICONNSTORE            4032    //�ͻ�������ʹ洢����������ֱ�� client -> store 
#define MSG_VOD_PARAGET                 4080    //�洢������������ѯ center->store
#define MSG_VOD_PARASET                 4084    //              ���� center->store
#define MSG_VOD_REBOOT                  4088    //�����洢������     center->store
#define MSG_KEYFRAME_QUERY              5110

//--------------------------------������Ϣת��-----------------------
#define EVENT_ALARM_NOTIFY              9000   //from access server
#define EVENT_ALARM_NOTIFY2USER         9002   //from access server
#define EVENT_ALARM_NOTIFY2DISPLAY      9004   //from access server
#define EVENT_ALARM_NOTIFY2TERM         9006   //from access server
#define EVENT_ALARM_NOTIFY2DISPATCH     9008   //from access server
#define EVENT_ALARM_NOTIFY2STORE        9010   //from access server

//--------------------------------����ָ��---------------------------------
#define MSG_FAULTNOTIFY_NEW		1100	// ��J2EE���͹���֪ͨ

//--------------------------------Эת�ְ�-------------------------------
#define SECOND_FRAME_HEADER_LEN 12
#define PACK_LENGTH 1024 * 10


//--------------------------------j2ee url path-------------------------------
//#define CMSURLPATH   "/megaeyes_access_support/main"
#define CMSURLPATH   "/route/rest/service/serve.do"/*"/megaeyes_access_support/main"*/ 
#define NMSURLPATH   "/nms/gate/msgd"
#define LOGURLPATH   "/logserver/add.log"

//----------------------------�����Զ����-------------------------------
#define LEN_INTEGER 9			// ������󳤶�
#define LEN_TICKET  32
#define BUFFER_LEN			10 * 1024 //��ͨ�������֧����󳤶�
#define HISBUFFER_LEN		10 * 1024 * 1024 //¼���ѯ���֧�����ĳ���
#define PASSIVE_MODE					8006			// ����ģʽ
#define ACTIVE_MODE				8008			// ����ģʽ
#define ACTIVE_CONN				8000
#define PASSIVE_CONN				8022
#define  MSG_ONOFF_MEDIA             110004     //�ַ��ϱ���Ƶ

#define  MSG__DISCONNECT	110008		//�豸���ӳɹ�
#define MSG__CONNECTED   110009        //�ͻ��˶Ͽ�����
#endif
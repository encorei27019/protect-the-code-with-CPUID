/*
----------------------------------------
|           �@��:ChingYue              |
|   �{�ҫO�@�t��(���XMySQL + �����X)   |
|                                      |
|                                      |
----------------------------------------
*/

#include <windows.h>
#include <stdio.h>
#include <mysql.h>
#include <string>
#include "Iphlpapi.h"
#include <Tchar.h>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "BaseBoard.h"
#include "CpuID.h"
#pragma comment(lib,"Iphlpapi.lib") //ŪMAC�n�ϥΪ�lib

//���~�N�X#416 = �s��SQL��A�d��CPU�A_stprintf_s(sos, MAX_PATH, _TEXT("SELECT * FROM test WHERE CPU ='%s'"),WCPU); ����
//���~�N�X#549 = �䤣��CPU���G��A�d�ߨϥΪ̿�JUserKey�A_stprintf_s(sos, MAX_PATH, _TEXT("SELECT * FROM test WHERE UseKey ='%s'"), UserKey); ����
//���~�N�X#102 = ����CPU�d�ߵ��G�Aresult = mysql_store_result(&myCont) ����
//���~�N�X#267 = ����UseKey�d�ߵ��G�Aresult = mysql_store_result(&myCont) ����

//�H�W���o�Ϳ��~�ɼu�X�������W��  �ҥi�b�U��CODE���ק�

//------------------------------------------------ --------------
// �D�O�ǦC�� -- �������ɬ� None
//------------------------------------------------ --------------
BOOL GetBaseBoardByCmd(char *lpszBaseBoard, int len/*=128*/)
{
	const long MAX_COMMAND_SIZE = 10000; // �R�O���X�w�Ĥj�p
	WCHAR szFetCmd[] = L"wmic BaseBoard get SerialNumber"; // ����D�O�ǦC���R�O��
	const string strEnSearch = "SerialNumber"; // �D�O�ǦC�����e�ɫH��

	BOOL bret = FALSE;
	HANDLE hReadPipe = NULL; //Ū���޹D
	HANDLE hWritePipe = NULL; //�g�J�޹D
	PROCESS_INFORMATION pi; //�i�{�H��
	STARTUPINFO si; //����R�O�浡�f�H��
	SECURITY_ATTRIBUTES sa; //�w���ݩ�

	char szBuffer[MAX_COMMAND_SIZE + 1] = { 0 }; // ��m�R�O�浲�G����X�w�İ�
	string strBuffer;
	unsigned long count = 0;
	long ipos = 0;

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	memset(&sa, 0, sizeof(sa));

	pi.hProcess = NULL;
	pi.hThread = NULL;
	si.cb = sizeof(STARTUPINFO);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	//1.0 �Ыغ޹D
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if (!bret)
	{
		goto END;
	}

	//2.0 �]�m�R�O�浡�f���H�������w��Ū�g�޹D
	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE; //���éR�O�浡�f
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//3.0 �Ы�����R�O�檺�i�{
	bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!bret)
	{
		goto END;
	}

	//4.0 Ū����^���ƾ�
	WaitForSingleObject(pi.hProcess, 500/*INFINITE*/);
	bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
	if (!bret)
	{
		goto END;
	}

	//5.0 �d��D�O�ǦC��
	bret = FALSE;
	strBuffer = szBuffer;
	ipos = strBuffer.find(strEnSearch);

	if (ipos < 0) // �S�����
	{
		goto END;
	}
	else
	{
		strBuffer = strBuffer.substr(ipos + strEnSearch.length());
	}

	memset(szBuffer, 0x00, sizeof(szBuffer));
	strcpy_s(szBuffer, strBuffer.c_str());

	//�h���������Ů� \r \n
	int j = 0;
	for (int i = 0; i < strlen(szBuffer); i++)
	{
		if (szBuffer[i] != ' ' && szBuffer[i] != '\n' && szBuffer[i] != '\r')
		{
			lpszBaseBoard[j] = szBuffer[i];
			j++;
		}
	}

	bret = TRUE;

END:
	//�����Ҧ����y�`
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return(bret);
}

//------------------------------------------------ --------------
// CPU�ǦC��
//------------------------------------------------ --------------
BOOL GetCpuByCmd(char *lpszCpu, int len/*=128*/)
{
	const long MAX_COMMAND_SIZE = 10000; // �R�O���X�w�Ĥj�p
	WCHAR szFetCmd[] = L"wmic cpu get processorid"; // ���CPU�ǦC���R�O��
	const string strEnSearch = "ProcessorId"; // CPU�ǦC�����e�ɫH��

	BOOL bret = FALSE;
	HANDLE hReadPipe = NULL; //Ū���޹D
	HANDLE hWritePipe = NULL; //�g�J�޹D
	PROCESS_INFORMATION pi; //�i�{�H��
	STARTUPINFO si; //����R�O�浡�f�H��
	SECURITY_ATTRIBUTES sa; //�w���ݩ�

	char szBuffer[MAX_COMMAND_SIZE + 1] = { 0 }; // ��m�R�O�浲�G����X�w�İ�
	string strBuffer;
	unsigned long count = 0;
	long ipos = 0;

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	memset(&sa, 0, sizeof(sa));

	pi.hProcess = NULL;
	pi.hThread = NULL;
	si.cb = sizeof(STARTUPINFO);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	//1.0 �Ыغ޹D
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if (!bret)
	{
		goto END;
	}

	//2.0 �]�m�R�O�浡�f���H�������w��Ū�g�޹D
	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE; //���éR�O�浡�f
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//3.0 �Ы�����R�O�檺�i�{
	bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!bret)
	{
		goto END;
	}

	//4.0 Ū����^���ƾ�
	WaitForSingleObject(pi.hProcess, 500/*INFINITE*/);
	bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
	if (!bret)
	{
		goto END;
	}

	//5.0 �d��CPU�ǦC��
	bret = FALSE;
	strBuffer = szBuffer;
	ipos = strBuffer.find(strEnSearch);

	if (ipos < 0) // �S�����
	{
		goto END;
	}
	else
	{
		strBuffer = strBuffer.substr(ipos + strEnSearch.length());
	}

	memset(szBuffer, 0x00, sizeof(szBuffer));
	strcpy_s(szBuffer, strBuffer.c_str());

	//�h���������Ů� \r \n
	int j = 0;
	for (int i = 0; i < strlen(szBuffer); i++)
	{
		if (szBuffer[i] != ' ' && szBuffer[i] != '\n' && szBuffer[i] != '\r')
		{
			lpszCpu[j] = szBuffer[i];
			j++;
		}
	}

	bret = TRUE;

END:
	//�����Ҧ����y�`
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return(bret);
}

//------------------------------------------------ --------------
// �����dMAC��
//------------------------------------------------ --------------
void GetMac(byte * mac)
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO(); //PIP_ADAPTER_INFO���c����w�s�x�������d�H��
	unsigned long stSize = sizeof(IP_ADAPTER_INFO); 	//�o�쵲�c��j�p,�Ω�GetAdaptersInfo�Ѽ�
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize); //�ե�GetAdaptersInfo���,��RpIpAdapterInfo���w�ܶq;�䤤stSize�ѼƬJ�O�@�ӿ�J�q�]�O�@�ӿ�X�q
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//�p�G��ƪ�^���OERROR_BUFFER_OVERFLOW
		//�h����GetAdaptersInfo�Ѽƶǻ������s�Ŷ�����,�P�ɨ�ǥXstSize,��ܻݭn���Ŷ��j�p
		//�o�]�O����������stSize�J�O�@�ӿ�J�q�]�O�@�ӿ�X�q
		delete pIpAdapterInfo; //�����Ӫ����s�Ŷ�
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];  //���s�ӽФ��s�Ŷ��ΨӦs�x�Ҧ����d�H��
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);  //�A���ե�GetAdaptersInfo���,��RpIpAdapterInfo���w�ܶq
	}
	if (ERROR_SUCCESS == nRel) //��X���d�H��
	{
		while (pIpAdapterInfo)
		{
			memcpy(mac, pIpAdapterInfo->Address, 8); //�ϥ�memcpy�ƻs���s��
			break; //�����Ĥ@�Ӻ��d��MAC����X�j��
		}
	}
	if (pIpAdapterInfo)delete pIpAdapterInfo;
}
	//unsigned  ���S���t�ƪ��N��

void main()
{

	/*--------------------------------
	|       ����X���ܼ�             |
	--------------------------------*/
	char MB[128] = ""; //MBID�s���ܼ�
	TCHAR WMB[MAX_PATH]; //�Nchar�ରwchar_t
	char CPU[128] = ""; //CpuID�s���ܼ�
	TCHAR WCPU[MAX_PATH];//�Nchar�ରwchar_t
	byte mac[8] = ""; //Mac�s���ܼ�

	/*--------------------------------
	|       �ާ@MYSQL���e���ܼ�      |
	--------------------------------*/
	TCHAR SqlUserID[MAX_PATH];//��ƮwŪ�X�Ӫ�UserID
	TCHAR SqlCPU[MAX_PATH];//��ƮwŪ�X�Ӫ�CPU
	TCHAR SqlMB[MAX_PATH]; //��ƮwŪ�X�Ӫ�MB	
	TCHAR SqlYorN[MAX_PATH]; //��ƮwŪ�X�Ӫ�YorN
	TCHAR SqlUseKey[MAX_PATH];//��ƮwŪ�X�Ӫ�UseKey
	TCHAR sos[MAX_PATH]; //�d��(Unicode)
	char plz[MAX_PATH] = ""; //�d��(UTF-8)
	TCHAR  UserKey[MAX_PATH]; //�ϥΪ̿�JUserKey
	TCHAR  UserKey1[MAX_PATH]; //�ϥΪ̿�JUserKey
	/*--------------------------------
	|        ��Ʈw�ܼ�              |
	--------------------------------*/
	char user[] = "ChingYue";
	char pswd[] = "";
	char host[] = "127.0.0.1";
	char table[] = "user";
	unsigned int port = 3306;
	MYSQL myCont;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res;

	/*--------------------------------
	|           �}�l                 |
	--------------------------------*/
	GetCpuByCmd(CPU, 128); //ŪCpuID
	MultiByteToWideChar(CP_ACP, 0, CPU, -1, WCPU, sizeof(WCPU)+1);
	GetBaseBoardByCmd(MB, 128); //ŪMBID
	MultiByteToWideChar(CP_ACP, 0, MB, -1, WMB, sizeof(WMB)+1);
	mysql_init(&myCont);
	mysql_options(&myCont, MYSQL_SET_CHARSET_NAME, "utf8"); //�]�wMySQL�s�X��k1
	_setmode(_fileno(stdout), _O_WTEXT); //�Y���[�o��h�L�k _tprintf �X����r

	if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0)) //�s����Ʈw
	{
		MessageBoxA(0, "�s����Ʈw���\", "���\", MB_OK);
		_stprintf_s(sos, MAX_PATH, _TEXT("SELECT * FROM test WHERE CPU ='%s'"), WCPU);
		WideCharToMultiByte(CP_UTF8, NULL, sos, -1, plz, sizeof(plz), NULL, FALSE);
		res = mysql_query(&myCont, plz); //�d�ߦ��\�^��0�A���Ѧ^��1(�dCPU)
		if (res) //�d�ߥ���
		{
			MessageBoxA(0, "�d�ߥ���", "���~�N�X#416", MB_OK);
			mysql_close(&myCont);
			exit(EXIT_FAILURE);
		}
		else
		{
			result = mysql_store_result(&myCont); //�N�d�ߵ��G�s��MYSQL_RES *result �A�Y���ѱN�^��NULL
			if (result) //������ȡA�DNULL�A��ܬd�ߦ��\
			{
				if (mysql_num_rows(result) == 0) //0��ܸ�Ʈw���S���o�����
				{
					_tprintf(_T("�п�J�ҰʧǸ�: "));
					_getts(UserKey); //���o�ϥΪ̿�J
					mysql_free_result(result); //�M�ŤW���d���x�s���G
					_stprintf_s(sos, MAX_PATH, _TEXT("SELECT * FROM test WHERE UseKey ='%s'"), UserKey); //�ϥΨϥΪ̪���J���d��
					WideCharToMultiByte(CP_UTF8, NULL, sos, -1, plz, sizeof(plz), NULL, FALSE);
					int  i = mysql_query(&myCont, plz);//�d�ߦ��\�^��0�A���Ѧ^��1(�d�ϥΪ̿�J�Ǹ�)
					if (i) //�d�ߥ���
					{
						MessageBoxA(0, "�d�ߥ���", "���~�N�X#549", MB_OK);
						mysql_close(&myCont);
						exit(EXIT_FAILURE);
					}
					else
					{
						result = mysql_store_result(&myCont);
						if (result)
						{
							if (mysql_num_rows(result) == 0)
							{
								MessageBoxA(0, "�Ǹ����~", "���~", MB_OK);
								mysql_free_result(result); //�M�ŤW���d���x�s���G
								mysql_close(&myCont); //����MYSQL
								exit(EXIT_FAILURE);
							}
							else
							{
								sql_row = mysql_fetch_row(result);
								MultiByteToWideChar(CP_UTF8, NULL, sql_row[4], -1, SqlYorN, sizeof(SqlYorN)+1); //�N�qMYSQLŪ�쪺���(UTF8)�নTCHAR(Unicode) (YorN)
								int j = _tcscmp(SqlYorN, _T("Y")); //���MYSQL��YorN�����e�O���OY�A�ۦP�^��0
								if (j == 0)
								{
									MessageBoxA(0, "�Ǹ��w�Q�ϥ�", "���~", MB_OK);
									mysql_free_result(result); //�M�ŤW���d���x�s���G
									mysql_close(&myCont); //����MYSQL
									exit(EXIT_FAILURE);
								}
								else
								{
									MessageBoxA(0, "��s�ϥΪ̸��", "��s", MB_OK);
									_stprintf_s(sos, MAX_PATH, _TEXT("UPDATE test SET CPU='%s',MB='%s',YorN='Y' WHERE UseKey='%s';"), WCPU, WMB, UserKey);
									WideCharToMultiByte(CP_UTF8, NULL, sos, -1, plz, sizeof(plz), NULL, FALSE);
									mysql_query(&myCont, plz);
								}
							}
						}
						else
						{
							MessageBoxA(0, "�d�ߥ���", "���~�N�X#267", MB_OK);
						}
					}
				}
				else
				{
					_tprintf(_T("�{�ҧ���\n�w��z\n"));
					sql_row = mysql_fetch_row(result);
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[1], -1, SqlUserID, sizeof(SqlUserID)+1); //�N�qMYSQLŪ�쪺���(UTF8)�নTCHAR(Unicode) (UserID)
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[2], -1, SqlCPU, sizeof(SqlCPU)+1); //�N�qMYSQLŪ�쪺���(UTF8)�নTCHAR(Unicode) (CPU)
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[3], -1, SqlMB, sizeof(SqlMB)+1); //�N�qMYSQLŪ�쪺���(UTF8)�নTCHAR(Unicode) (MB)
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[4], -1, SqlYorN, sizeof(SqlYorN)+1); //�N�qMYSQLŪ�쪺���(UTF8)�নTCHAR(Unicode) (YorN)
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[5], -1, SqlUseKey, sizeof(SqlUseKey)+1); //�N�qMYSQLŪ�쪺���(UTF8)�নTCHAR(Unicode) (UseKey)
					_tprintf(_T("�z���ϥΪ�ID��: %s\n"), SqlUserID);
					_tprintf(_T("�z���ϥΪ�CPU��: %s\n"), SqlCPU);
					_tprintf(_T("�z���ϥΪ�MB��: %s\n"), SqlMB);
					_tprintf(_T("�z���ϥΪ�YorN��: %s\n"), SqlYorN);
					_tprintf(_T("�z���ϥΪ�UseKey��: %s\n"), SqlUseKey);
					_tprintf(_T("Retrieved %lu rows\n"), mysql_num_rows(result));
				}
			}
			else
			{
				MessageBoxA(0, "�d�ߥ���", "���~�N�X#102", MB_OK);
			}
		}
	}
	else
	{
		MessageBoxA(0, "�s����Ʈw���ѡA�нT�{�������p", "���~", MB_OK);
		mysql_close(&myCont);
		exit(EXIT_FAILURE);
	}
	mysql_close(&myCont);
	MessageBoxA(0, "Hello Woid", "�L��", MB_OK);
	exit(EXIT_SUCCESS);
}
/*
----------------------------------------
|           作者:ChingYue              |
|   認證保護系統(結合MySQL + 機器碼)   |
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
#pragma comment(lib,"Iphlpapi.lib") //讀MAC要使用的lib

//錯誤代碼#416 = 連接SQL後，查詢CPU，_stprintf_s(sos, MAX_PATH, _TEXT("SELECT * FROM test WHERE CPU ='%s'"),WCPU); 失敗
//錯誤代碼#549 = 找不到CPU結果後，查詢使用者輸入UserKey，_stprintf_s(sos, MAX_PATH, _TEXT("SELECT * FROM test WHERE UseKey ='%s'"), UserKey); 失敗
//錯誤代碼#102 = 接收CPU查詢結果，result = mysql_store_result(&myCont) 失敗
//錯誤代碼#267 = 接收UseKey查詢結果，result = mysql_store_result(&myCont) 失敗

//以上為發生錯誤時彈出的提醒名稱  皆可在下方CODE中修改

//------------------------------------------------ --------------
// 主板序列號 -- 獲取不到時為 None
//------------------------------------------------ --------------
BOOL GetBaseBoardByCmd(char *lpszBaseBoard, int len/*=128*/)
{
	const long MAX_COMMAND_SIZE = 10000; // 命令行輸出緩衝大小
	WCHAR szFetCmd[] = L"wmic BaseBoard get SerialNumber"; // 獲取主板序列號命令行
	const string strEnSearch = "SerialNumber"; // 主板序列號的前導信息

	BOOL bret = FALSE;
	HANDLE hReadPipe = NULL; //讀取管道
	HANDLE hWritePipe = NULL; //寫入管道
	PROCESS_INFORMATION pi; //進程信息
	STARTUPINFO si; //控制命令行窗口信息
	SECURITY_ATTRIBUTES sa; //安全屬性

	char szBuffer[MAX_COMMAND_SIZE + 1] = { 0 }; // 放置命令行結果的輸出緩衝區
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

	//1.0 創建管道
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if (!bret)
	{
		goto END;
	}

	//2.0 設置命令行窗口的信息為指定的讀寫管道
	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE; //隱藏命令行窗口
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//3.0 創建獲取命令行的進程
	bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!bret)
	{
		goto END;
	}

	//4.0 讀取返回的數據
	WaitForSingleObject(pi.hProcess, 500/*INFINITE*/);
	bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
	if (!bret)
	{
		goto END;
	}

	//5.0 查找主板序列號
	bret = FALSE;
	strBuffer = szBuffer;
	ipos = strBuffer.find(strEnSearch);

	if (ipos < 0) // 沒有找到
	{
		goto END;
	}
	else
	{
		strBuffer = strBuffer.substr(ipos + strEnSearch.length());
	}

	memset(szBuffer, 0x00, sizeof(szBuffer));
	strcpy_s(szBuffer, strBuffer.c_str());

	//去掉中間的空格 \r \n
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
	//關閉所有的句柄
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return(bret);
}

//------------------------------------------------ --------------
// CPU序列號
//------------------------------------------------ --------------
BOOL GetCpuByCmd(char *lpszCpu, int len/*=128*/)
{
	const long MAX_COMMAND_SIZE = 10000; // 命令行輸出緩衝大小
	WCHAR szFetCmd[] = L"wmic cpu get processorid"; // 獲取CPU序列號命令行
	const string strEnSearch = "ProcessorId"; // CPU序列號的前導信息

	BOOL bret = FALSE;
	HANDLE hReadPipe = NULL; //讀取管道
	HANDLE hWritePipe = NULL; //寫入管道
	PROCESS_INFORMATION pi; //進程信息
	STARTUPINFO si; //控制命令行窗口信息
	SECURITY_ATTRIBUTES sa; //安全屬性

	char szBuffer[MAX_COMMAND_SIZE + 1] = { 0 }; // 放置命令行結果的輸出緩衝區
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

	//1.0 創建管道
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if (!bret)
	{
		goto END;
	}

	//2.0 設置命令行窗口的信息為指定的讀寫管道
	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE; //隱藏命令行窗口
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//3.0 創建獲取命令行的進程
	bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!bret)
	{
		goto END;
	}

	//4.0 讀取返回的數據
	WaitForSingleObject(pi.hProcess, 500/*INFINITE*/);
	bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
	if (!bret)
	{
		goto END;
	}

	//5.0 查找CPU序列號
	bret = FALSE;
	strBuffer = szBuffer;
	ipos = strBuffer.find(strEnSearch);

	if (ipos < 0) // 沒有找到
	{
		goto END;
	}
	else
	{
		strBuffer = strBuffer.substr(ipos + strEnSearch.length());
	}

	memset(szBuffer, 0x00, sizeof(szBuffer));
	strcpy_s(szBuffer, strBuffer.c_str());

	//去掉中間的空格 \r \n
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
	//關閉所有的句柄
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return(bret);
}

//------------------------------------------------ --------------
// 網路卡MAC號
//------------------------------------------------ --------------
void GetMac(byte * mac)
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO(); //PIP_ADAPTER_INFO結構體指針存儲本機網卡信息
	unsigned long stSize = sizeof(IP_ADAPTER_INFO); 	//得到結構體大小,用於GetAdaptersInfo參數
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize); //調用GetAdaptersInfo函數,填充pIpAdapterInfo指針變量;其中stSize參數既是一個輸入量也是一個輸出量
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函數返回的是ERROR_BUFFER_OVERFLOW
		//則說明GetAdaptersInfo參數傳遞的內存空間不夠,同時其傳出stSize,表示需要的空間大小
		//這也是說明為什麼stSize既是一個輸入量也是一個輸出量
		delete pIpAdapterInfo; //釋放原來的內存空間
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];  //重新申請內存空間用來存儲所有網卡信息
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);  //再次調用GetAdaptersInfo函數,填充pIpAdapterInfo指針變量
	}
	if (ERROR_SUCCESS == nRel) //輸出網卡信息
	{
		while (pIpAdapterInfo)
		{
			memcpy(mac, pIpAdapterInfo->Address, 8); //使用memcpy複製內存值
			break; //獲取到第一個網卡的MAC後跳出迴圈
		}
	}
	if (pIpAdapterInfo)delete pIpAdapterInfo;
}
	//unsigned  為沒有負數的意思

void main()
{

	/*--------------------------------
	|       抓機碼用變數             |
	--------------------------------*/
	char MB[128] = ""; //MBID存放變數
	TCHAR WMB[MAX_PATH]; //將char轉為wchar_t
	char CPU[128] = ""; //CpuID存放變數
	TCHAR WCPU[MAX_PATH];//將char轉為wchar_t
	byte mac[8] = ""; //Mac存放變數

	/*--------------------------------
	|       操作MYSQL內容用變數      |
	--------------------------------*/
	TCHAR SqlUserID[MAX_PATH];//資料庫讀出來的UserID
	TCHAR SqlCPU[MAX_PATH];//資料庫讀出來的CPU
	TCHAR SqlMB[MAX_PATH]; //資料庫讀出來的MB	
	TCHAR SqlYorN[MAX_PATH]; //資料庫讀出來的YorN
	TCHAR SqlUseKey[MAX_PATH];//資料庫讀出來的UseKey
	TCHAR sos[MAX_PATH]; //查詢(Unicode)
	char plz[MAX_PATH] = ""; //查詢(UTF-8)
	TCHAR  UserKey[MAX_PATH]; //使用者輸入UserKey
	TCHAR  UserKey1[MAX_PATH]; //使用者輸入UserKey
	/*--------------------------------
	|        資料庫變數              |
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
	|           開始                 |
	--------------------------------*/
	GetCpuByCmd(CPU, 128); //讀CpuID
	MultiByteToWideChar(CP_ACP, 0, CPU, -1, WCPU, sizeof(WCPU)+1);
	GetBaseBoardByCmd(MB, 128); //讀MBID
	MultiByteToWideChar(CP_ACP, 0, MB, -1, WMB, sizeof(WMB)+1);
	mysql_init(&myCont);
	mysql_options(&myCont, MYSQL_SET_CHARSET_NAME, "utf8"); //設定MySQL編碼方法1
	_setmode(_fileno(stdout), _O_WTEXT); //若不加這行則無法 _tprintf 出中文字

	if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0)) //連接資料庫
	{
		MessageBoxA(0, "連接資料庫成功", "成功", MB_OK);
		_stprintf_s(sos, MAX_PATH, _TEXT("SELECT * FROM test WHERE CPU ='%s'"), WCPU);
		WideCharToMultiByte(CP_UTF8, NULL, sos, -1, plz, sizeof(plz), NULL, FALSE);
		res = mysql_query(&myCont, plz); //查詢成功回傳0，失敗回傳1(查CPU)
		if (res) //查詢失敗
		{
			MessageBoxA(0, "查詢失敗", "錯誤代碼#416", MB_OK);
			mysql_close(&myCont);
			exit(EXIT_FAILURE);
		}
		else
		{
			result = mysql_store_result(&myCont); //將查詢結果存到MYSQL_RES *result ，若失敗將回傳NULL
			if (result) //有收到值，非NULL，表示查詢成功
			{
				if (mysql_num_rows(result) == 0) //0表示資料庫內沒有這筆資料
				{
					_tprintf(_T("請輸入啟動序號: "));
					_getts(UserKey); //取得使用者輸入
					mysql_free_result(result); //清空上次查詢儲存結果
					_stprintf_s(sos, MAX_PATH, _TEXT("SELECT * FROM test WHERE UseKey ='%s'"), UserKey); //使用使用者的輸入做查詢
					WideCharToMultiByte(CP_UTF8, NULL, sos, -1, plz, sizeof(plz), NULL, FALSE);
					int  i = mysql_query(&myCont, plz);//查詢成功回傳0，失敗回傳1(查使用者輸入序號)
					if (i) //查詢失敗
					{
						MessageBoxA(0, "查詢失敗", "錯誤代碼#549", MB_OK);
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
								MessageBoxA(0, "序號錯誤", "錯誤", MB_OK);
								mysql_free_result(result); //清空上次查詢儲存結果
								mysql_close(&myCont); //關閉MYSQL
								exit(EXIT_FAILURE);
							}
							else
							{
								sql_row = mysql_fetch_row(result);
								MultiByteToWideChar(CP_UTF8, NULL, sql_row[4], -1, SqlYorN, sizeof(SqlYorN)+1); //將從MYSQL讀到的資料(UTF8)轉成TCHAR(Unicode) (YorN)
								int j = _tcscmp(SqlYorN, _T("Y")); //比對MYSQL中YorN的內容是不是Y，相同回傳0
								if (j == 0)
								{
									MessageBoxA(0, "序號已被使用", "錯誤", MB_OK);
									mysql_free_result(result); //清空上次查詢儲存結果
									mysql_close(&myCont); //關閉MYSQL
									exit(EXIT_FAILURE);
								}
								else
								{
									MessageBoxA(0, "更新使用者資料", "更新", MB_OK);
									_stprintf_s(sos, MAX_PATH, _TEXT("UPDATE test SET CPU='%s',MB='%s',YorN='Y' WHERE UseKey='%s';"), WCPU, WMB, UserKey);
									WideCharToMultiByte(CP_UTF8, NULL, sos, -1, plz, sizeof(plz), NULL, FALSE);
									mysql_query(&myCont, plz);
								}
							}
						}
						else
						{
							MessageBoxA(0, "查詢失敗", "錯誤代碼#267", MB_OK);
						}
					}
				}
				else
				{
					_tprintf(_T("認證完成\n歡迎您\n"));
					sql_row = mysql_fetch_row(result);
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[1], -1, SqlUserID, sizeof(SqlUserID)+1); //將從MYSQL讀到的資料(UTF8)轉成TCHAR(Unicode) (UserID)
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[2], -1, SqlCPU, sizeof(SqlCPU)+1); //將從MYSQL讀到的資料(UTF8)轉成TCHAR(Unicode) (CPU)
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[3], -1, SqlMB, sizeof(SqlMB)+1); //將從MYSQL讀到的資料(UTF8)轉成TCHAR(Unicode) (MB)
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[4], -1, SqlYorN, sizeof(SqlYorN)+1); //將從MYSQL讀到的資料(UTF8)轉成TCHAR(Unicode) (YorN)
					MultiByteToWideChar(CP_UTF8, NULL, sql_row[5], -1, SqlUseKey, sizeof(SqlUseKey)+1); //將從MYSQL讀到的資料(UTF8)轉成TCHAR(Unicode) (UseKey)
					_tprintf(_T("您的使用者ID為: %s\n"), SqlUserID);
					_tprintf(_T("您的使用者CPU為: %s\n"), SqlCPU);
					_tprintf(_T("您的使用者MB為: %s\n"), SqlMB);
					_tprintf(_T("您的使用者YorN為: %s\n"), SqlYorN);
					_tprintf(_T("您的使用者UseKey為: %s\n"), SqlUseKey);
					_tprintf(_T("Retrieved %lu rows\n"), mysql_num_rows(result));
				}
			}
			else
			{
				MessageBoxA(0, "查詢失敗", "錯誤代碼#102", MB_OK);
			}
		}
	}
	else
	{
		MessageBoxA(0, "連接資料庫失敗，請確認網路狀況", "錯誤", MB_OK);
		mysql_close(&myCont);
		exit(EXIT_FAILURE);
	}
	mysql_close(&myCont);
	MessageBoxA(0, "Hello Woid", "過關", MB_OK);
	exit(EXIT_SUCCESS);
}
// WebServer.cpp : Defines the entry point for the console application.
//
#include<iostream>
#include "winsock2.h"
#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable : 4996)

typedef struct sinhvien {
	char username[50];
	char password[50];
	char fullname[50];
	char id[10];
}SV;


int num;
int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8080);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	// khoi tao bo nho cho cau truc SV
	SV danhsach[50];
	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);

		char buf[1024];
		int ret;
		char getDir[64], tmps[64];
		// Nhan yeu cau tu trinh duyet
		ret = recv(client, buf, sizeof(buf), 0);
		buf[ret] = 0;
		sscanf(buf, "%s %s", getDir, tmps);
		const char *dir = tmps + 1;
		int o = strlen(dir);
		printf("%s", buf);
		//GET
		if (strncmp(buf + 4, "/xinchao", 8) == 0)
		{
			// Tra ket qua cho trinh duyet
			const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
			send(client, header, strlen(header), 0);

			char sendbuf[256];
			char tmp[256];
			FILE *f = fopen("client.txt", "rb");
			//fgets(sendbuf, 255, (FILE *)f);
			while (1)
			{
				ret = fread(sendbuf, 1, sizeof(sendbuf), f);

				if (ret > 0)
					send(client, sendbuf, ret, 0);
				else
					break;
			}

			fclose(f);

		}
		//POST
		else if (strncmp(buf + 5, "/signup", 7) == 0)
		{
#pragma region getJSON-Data
			printf("%s", buf);
			int k;
			for (k = strlen(buf); k > 0; k--) {
				if (buf[k] == '\n') {
					break;
				}
			}
			char json[128];
			int m = 0;
			for (int h = k + 1; h < strlen(buf); h++) {
				json[m] = buf[h];
				m++;
			}
			json[m] = 0;

		
#pragma endregion
#pragma region seperateData
			for (int h = 0; h < strlen(json); h++) {
				if (json[h] == '&' || json[h] == '=') {
					json[h] = ' ';
				}
			}
			
			// khai bao struct de luu thong tin sinh vien
	
			char name[64],nameValue[64],pass[64],passValue[64],fullname[64],fullnameValue[64],id[64],idValue[64];
			sscanf(json, "%s %s %s %s %s %s %s %s", name, nameValue,pass,passValue,fullname,fullnameValue,id,idValue);
			for (int index = 0; index < strlen(fullnameValue); index++) {
				if (fullnameValue[index] == '+') fullnameValue[index] == ' ';
			}

			char file[1024],buf_search[64];
			const char *success = "Ban dang ki thanh cong!";
			FILE *f = fopen("C:\\data.txt", "a+");
			if (f == NULL) printf("\n File khong ton tai!");
			else {
				fseek(f, 0, SEEK_END);
				int size = ftell(f);
				if (size == 0) { // neu file chua ton tai
					fprintf(f, "%s\n%s\n%s\n%s\n", nameValue, passValue, fullnameValue, idValue);
					const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
					send(client, header, strlen(header), 0);
					send(client, success, strlen(success), 0);
				}	
				else { // neu file da ton tai
					int found = 0;
					fseek(f, 0, SEEK_SET);				
				/*	while (!feof(f)) {
						 fgets(buf_search, sizeof(buf_search), f);
						 int len = strlen(buf_search);
						 //xoa ki tu xuong dong
						 buf_search[len - 1] = 0;
						if (strcmp(buf_search, nameValue) == 0) {
							found = 1; break;			
						}
					}*/
					num = 0;
					int line = 1;
					do {
						char buff[100];
						fgets(buff, sizeof(buff), f);
						//xoa ky tu xuong dong
						int ret = strlen(buff);
						buff[ret - 1] = 0;

						if (line % 4 == 1) {
							strcpy(danhsach[num].username, buff);
							line++;
						}
						else if (line % 4 == 2) {
							strcpy(danhsach[num].password, buff);
							line++;
						}
						else if (line % 4 == 3) {
							strcpy(danhsach[num].fullname, buff);
							line++;
						}
						else if (line % 4 == 0)
						{
							strcpy(danhsach[num].id, buff);
							line++;
							num++;
						}
					} while (!feof(f));
					// check tai khoan ton tai chua
					for (int index = 0; index < num; index++) {
						if (strcmp(danhsach[index].username, nameValue) == 0) {
							found = 1;
							break;
						}
					}
					if (found == 1) { // neu username co ton tai trong file
						const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
						send(client, header, strlen(header), 0);
						const char *fail = "Tai khoan da ton tai!";
						send(client,fail , strlen(fail), 0);
					}
					if (found == 0) { // neu username khong ton tai trong file
						fseek(f,0, SEEK_END);
						fprintf(f, "%s\n%s\n%s\n%s\n", nameValue, passValue, fullnameValue, idValue);
						const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
						send(client, header, strlen(header), 0);					
						send(client, success, strlen(success), 0);
					}
					
				}
				fclose(f);
			}
#pragma endregion
			// Tra ket qua cho trinh duyet
		/*	const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
			send(client, header, strlen(header), 0);

			char jsonFile[1024];
			sprintf(jsonFile, "{\"%s\" : \"%s\"}", name, nameValue);
			send(client, jsonFile, strlen(jsonFile), 0);*/
		}
		//GET
		else if (strncmp(buf + 4, "/hello", 6) == 0)
		{

			//char buf2[256];
			//ret = recv(client, buf2, strlen(buf2), 0);
			// Tra ket qua cho trinh duyet
			const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
			send(client, header, strlen(header), 0);
			char sg[100] = "p {color:red;}";
			send(client, sg, strlen(sg), 0);

		}
		//POST
		else if (strncmp(buf + 5, "/hello", 6) == 0)
		{

			char buf2[256];
			ret = recv(client, buf2, strlen(buf2), 0);
			printf("%s", buf2);
			// Tra ket qua cho trinh duyet
			const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
			send(client, header, strlen(header), 0);
			char sg[100] = "p {color:red;}";
			send(client, sg, strlen(sg), 0);

		}
		//GET
		else if (strncmp(buf + 4, "/image", 6) == 0 || strncmp(buf + 4, "/favicon.ico", 12) == 0)
		{
			// Tra ket qua cho trinh duyet
			const char *header = "HTTP/1.1 200 OK\nContent-Type: image/jpg\n\n";
			send(client, header, strlen(header), 0);

			FILE *f = fopen("flag.jpg", "rb");
			while (1)
			{
				ret = fread(buf, 1, sizeof(buf), f);
				if (ret > 0)
					send(client, buf, ret, 0);
				else
					break;
			}

			fclose(f);
		}
		else
		{
			// Tra ket qua cho trinh duyet
			const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
			send(client, header, strlen(header), 0);

			const char *content = "<html><body><h1>Yeu cau khong duoc ho tro</h1></body></html>";
			send(client, content, strlen(content), 0);
		}

		// Dong ket noi
		closesocket(client);
	}

	closesocket(listener);
	WSACleanup();

	return 0;
}


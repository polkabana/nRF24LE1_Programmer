#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define FLASH_TRIGGER_WRITEMAINPAGE   0x01    // Magic character to trigger uploading of flash
#define FLASH_TRIGGER_READMAINPAGE    0x02
#define FLASH_TRIGGER_WRITEINFOPAGE   0x11
#define FLASH_TRIGGER_READINFOPAGE    0x12
#define FLASH_TRIGGER_DUMP            0x20


int nupp = 0xff;
int rdismb = 0xff;
char* port;
char* command;
char* filename;
int hex_fd, hex_size;
unsigned char *hex_buffer;
HANDLE serialHandle;
DWORD dwBytesRead = 0;
unsigned char buffer[1024];
int buffersize = 1024;

void usage() {
  printf("Uploads .hex file into nRF24LE1 via Arduino\n");
  printf("Usage: programmer.exe \\\\.\\COM13 CMD filename.hex [NUPP] [RDISMB]\n");
  printf("where CMD is d(dump), w (write)\n");
}

char *get_str(unsigned char *string, int max_len) {
  unsigned char buffer[2] = {0};
  int pos = 0, buffersize = 1;
  DWORD dwBytesRead = 0;
  
  memset(string, 0, max_len);
  while (ReadFile(serialHandle, buffer, buffersize, &dwBytesRead, NULL)) {
	// printf("%02x ", buffer[0]);
	if ((buffer[0] == '\r') || (buffer[0] == '\n')) {
		break;
	}
	if (pos > max_len) {
		break;
	}
    string[pos++] = buffer[0];
  }
  
  return string;
}

void print_ch() {
  unsigned char buffer[100] = {0};
  int buffersize = 100;
  DWORD dwBytesRead = 0;

  if (ReadFile(serialHandle, buffer, buffersize, &dwBytesRead, NULL)) {
	printf(buffer);
  }
}

void send_flash_trigger(char trigger) {
  WriteFile(serialHandle, &trigger, sizeof(trigger), &dwBytesRead, NULL);
}


void dump() {
  send_flash_trigger(FLASH_TRIGGER_DUMP);

  while(1) {
    get_str(buffer, buffersize);
	if (strlen(buffer)) {
		printf("%s\n", buffer);
	}
	if (strcmp(buffer, "DONE") == 0) {
		break;
	}
  }
}

void write_flash() {
  send_flash_trigger(FLASH_TRIGGER_WRITEMAINPAGE);

  printf("* Wait for READY\n");

  // wait for READY
  while(1) {
    get_str(buffer, buffersize);
	if (strlen(buffer)) {
		printf("%s\n", buffer);
	}
	if (strcmp(buffer, "READY") == 0) {
		break;
	}
  }

  sprintf(buffer, "GO %d %d\n", nupp, rdismb);
  printf("* %s", buffer);
  if (!WriteFile(serialHandle, buffer, strlen(buffer), &dwBytesRead, NULL)) {
    printf("! Write error\n");
  }

  printf("* Upload %s\n", filename);

  while(1) {
	unsigned char ch;

    get_str(buffer, buffersize);
	if (strlen(buffer)) {
		printf("%s\n", buffer);
	}
	if (strcmp(buffer, "OK") == 0) {
		while (read(hex_fd, &ch, 1) > 0) {
			WriteFile(serialHandle, &ch, 1, &dwBytesRead, NULL);
			if (ch == '\n') {
				break;
			}
		}

	}
	if (strcmp(buffer, "EOF") == 0) {
		break;
	}
  }

  // wait for DONE
  while(1) {
    get_str(buffer, buffersize);
	if (strlen(buffer)) {
		printf("%s\n", buffer);
	}
	if (strcmp(buffer, "DONE") == 0) {
		break;
	}
  }
}

int main(int argc, char *argv[]) {
  if ((argc < 3) || (argc > 5)) {
    usage();
	return -1;
  }
  
  port = argv[1];
  command = argv[2];
  filename = argv[3];
  
  
  if ((hex_fd = open(filename, O_RDWR | O_TEXT)) == -1) {
    printf("! Can't open file %s\n", filename);
	return -1;
  }

  serialHandle = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (serialHandle == INVALID_HANDLE_VALUE) {
    printf("! Can't open port %s\n", port);
	return -1;
  }

  DCB serialParams = { 0 };
  serialParams.DCBlength = sizeof(serialParams);

  // Set baudrate
  GetCommState(serialHandle, &serialParams);
  serialParams.BaudRate = CBR_57600;
  serialParams.ByteSize = 8;
  serialParams.StopBits = ONESTOPBIT;
  serialParams.Parity = NOPARITY;
  SetCommState(serialHandle, &serialParams);

  // Set timeouts
  COMMTIMEOUTS timeout = { 0 };
  timeout.ReadIntervalTimeout = 50;
  timeout.ReadTotalTimeoutConstant = 50;
  timeout.ReadTotalTimeoutMultiplier = 50;
  timeout.WriteTotalTimeoutConstant = 50;
  timeout.WriteTotalTimeoutMultiplier = 10;
  SetCommTimeouts(serialHandle, &timeout);
  
  Sleep(500);
  
  if (strcmp(command, "d") == 0) {
    printf("* Reading ..\n");
	dump();
  }
  else if (strcmp(command, "w") == 0) {
    printf("* Flashing %s..\n", filename);
	write_flash();
  }

  CloseHandle(serialHandle);
  close(hex_fd);
  
  return 0;
}

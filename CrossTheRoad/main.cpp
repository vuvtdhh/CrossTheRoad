#include<windows.h>
#include<iostream>
#include<thread>
#include<conio.h>
#include"Function.h"

using namespace std;
//Buoc 1
void FixConsoleWindow() {
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}
//Buoc 2
void GotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
//Buoc 3
//Hằng số
#define MAX_CAR 17
#define MAX_CAR_LENGTH 40 
#define MAX_SPEED 3 
//Biến toàn cục 6 
POINT** X; //Mảng chứa MAX_CAR xe
POINT Y; // Đại diện người qua đường 
int cnt = 0;//Biến hỗ trợ trong quá trình tăng tốc độ xe di chuyển 
int MOVING;//Biến xác định hướng di chuyển của người 
int SPEED;// Tốc độ xe chạy (xem như level) 
int HEIGH_CONSOLE = 20, WIDTH_CONSOLE = 70;// Độ rộng và độ cao của màn hình console 

// 4.3
int *STOP_TIME = NULL; // <0 chạy cho tới khi dừng, >=0 số chạy cho tới khi chạy tiếp
				// khi xe chạy thì khi tới giới hạn [-1;-40] thì chạy tiếp ra số ngẫu nhiên dương để bắt đầu dừng
				// khi xe dừng tới giá trị [0; 39] thì tiếp tục ra số ngẫu nhiên âm để bắt đầu chạy tiếp cho tới khi dừng

bool STOP = false;

bool haveGARBAGE = false;
bool ClearConsole() {
	system("cls");
	return false;
}
// end 4.3

bool STATE; // Trạng thái sống/chết của người qua đường 

//Buoc 4
//Hàm khởi tạo dữ liệu mặc định ban đầu 
void ResetData() {
	MOVING = 'D'; // Ban đầu cho người di chuyển sang phải   
	SPEED = 1; // Tốc độ lúc đầu 
	Y = { 18,19 }; // Vị trí lúc đầu của người 
				   // Tạo mảng xe chạy 
	if (X == NULL) {
		X = new POINT*[MAX_CAR];

		// 5.3
		STOP_TIME = new INT[MAX_CAR];
		// end 5.3
		
		for (int i = 0; i < MAX_CAR; i++)
			X[i] = new POINT[MAX_CAR_LENGTH];
		for (int i = 0; i < MAX_CAR; i++)
		{
			STOP_TIME[i] = 0;

			int temp = (rand() % (WIDTH_CONSOLE - MAX_CAR_LENGTH)) + 1;
			for (int j = 0; j < MAX_CAR_LENGTH; j++)
			{
				X[i][j].x = temp + j;
				X[i][j].y = 2 + i;
			}

		}
	}
}

//Buoc 5
void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
	GotoXY(x, y);
	cout << 'X';
	for (int i = 1; i < width; i++)
		cout << 'X';
	cout << 'X';
	GotoXY(x, height + y);
	cout << 'X';
	for (int i = 1; i < width; i++)
		cout << 'X';
	cout << 'X';
	for (int i = y + 1; i < height + y; i++)
	{
		GotoXY(x, i);
		cout << 'X';
		GotoXY(x + width, i);
		cout << 'X';
	}
	GotoXY(curPosX, curPosY);
}

//Buoc 9
//Hàm vẽ các toa xe 
void DrawCars(char* s)
{
	for (int i = 0; i < MAX_CAR; i++) {
		for (int j = 0; j < MAX_CAR_LENGTH; j++)
		{
			GotoXY(X[i][j].x, X[i][j].y);
			printf(".");
		}
	}
}

//Hàm vẽ người qua đường 
void DrawSticker(const POINT& p, char* s, int color_sticker = 15) {
	GotoXY(p.x, p.y);

	// 4.3 - đổi màu Sticker thành màu color_sticker
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_sticker);
	printf(s);

	// phục hồi lại màu của Sticker thành màu mặc định
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	// end 4.3
}

//Buoc 6
void StartGame() {
	ClearConsole();
	ResetData(); // Khởi tạo dữ liệu gốc 
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
	// Vẽ màn hình game 
	
	DrawCars(".");
	DrawSticker(Y, "0");

	GotoXY(0, HEIGH_CONSOLE + 3);
	printf("Input any key to play");

	haveGARBAGE = true;
	STATE = true;//Bắt đầu cho Thread chạy 
}

//Buoc 7
//Hàm dọn dẹp tài nguyên 
void GabageCollect()
{
	if (X == NULL) return;
	for (int i = 0; i < MAX_CAR; i++)
	{
		delete[] X[i];
	}
	delete[] X;

	delete[] STOP_TIME;
	X = NULL;
}
//Hàm thoát game 
void ExitGame(HANDLE t) {
	ClearConsole();
	TerminateThread(t, 0);
	GabageCollect();
}
//Hàm dừng game 
void PauseGame(HANDLE t) {
	SuspendThread(t);
}

//Buoc 8
//Hàm xử lý khi người đụng xe 
void ProcessDead() {
	STATE = 0;

	// 4.3
	DrawSticker(Y, "Y", 12);
	// end 4.3
	GotoXY(0, HEIGH_CONSOLE + 1);
	printf("Finished %d people.", countPeopleFinished());

	GotoXY(0, HEIGH_CONSOLE + 2);
	printf("Dead because impack with car, type y to continue or anykey to exit");
}

// 4.1
// hàm xử lý khi người mới về đích va chạm với người về đích trước đó
void ProcessImpackPeoplePre() {
	// 4.3
	DrawSticker(Y, "Y", 12);
	// end 4.3

	STATE = 0;
	GotoXY(0, HEIGH_CONSOLE + 2);
	printf("Dead because impack with people before, type y to continue or anykey to exit");

	GotoXY(0, HEIGH_CONSOLE + 1);
	printf("Finished %d people.", countPeopleFinished());

	destroyHistoryPeople(); // xóa tất cả lịch sử người về đích trước đó

}
// end 4.1

//Hàm xử lý khi người băng qua đường thành công 
void ProcessFinish(POINT& p) {

	// 4.1 xét xem có chạm với người đi trước không
	if (testImpactWithPeoplePre(p.x)) { // nếu có chạm
		ProcessImpackPeoplePre(); // thì xử lý chạm người chơi

		// 4.3
		DrawSticker(Y, "Y", 12);
		// end4.3

	}
	else {
		// 4.3
		DrawSticker(Y, "Y", 10);
		// end4.3

		addPeopleFinish(p.x); // thêm vị trí của người tới đích
	}
	// end 4.1
	//SPEED = 1;
	SPEED == MAX_SPEED ? SPEED = 1 : SPEED++;
	p = { 18,19 }; // Vị trí lúc đầu của người
	MOVING = 'D'; // Ban đầu cho người di chuyển sang phải 
}

//Buoc 10
//Hàm kiểm tra xem người qua đường có đụng xe không 
bool IsImpact(const POINT& p, int d)
{
	if (d == 1 || d == 19)return false;
	for (int i = 0; i < MAX_CAR_LENGTH; i++)
	{
		if (p.x == X[d - 2][i].x && p.y == X[d - 2][i].y)
			return true;
	}
	return false;
}

//Buoc 11

// 4.3
void ProcessStopAndRun() {

	srand((unsigned int)time(0));

	for (int i = 0; i < MAX_CAR; i++)
	{
		if (STOP_TIME[i] >= 0 && STOP_TIME[i] < 1) STOP_TIME[i] = -1 * ((rand() % (100/SPEED))*2); // nếu xe đang dừng tới 0 thì chuyển sang thời gian chạy
		if (STOP_TIME[i] >= -1 && STOP_TIME[i] < 0) STOP_TIME[i] = 1 * ((rand() % (100/SPEED))); // nếu xe đang chạy tới -1 chuyển sang thời gian dừng

		STOP_TIME[i] += (STOP_TIME[i] >= 0) ? -1 : 1; // nếu đang dừng (>=0) thì giảm để chuyển qua thời gian chạy
														// ngược lại nếu đang chạy (<0) thì tăng lên để xe dừng
	}
}
// end 4.3

void MoveCars() {
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
	ProcessStopAndRun();
	for (int i = 1; i < MAX_CAR; i += 2)
	{
		if (STOP_TIME[i] < 0) {
			cnt = 0;
			do {
				cnt++;
				for (int j = 0; j < MAX_CAR_LENGTH - 1; j++) {
					X[i][j] = X[i][j + 1];
				}
				X[i][MAX_CAR_LENGTH - 1].x + 1 == WIDTH_CONSOLE ? X[i][MAX_CAR_LENGTH - 1].x = 1 : X[i][MAX_CAR_LENGTH - 1].x++;
				// Kiểm tra xem xe có đụng màn hình không
			} while (cnt < SPEED);
		}
	}
	for (int i = 0; i < MAX_CAR; i += 2)
	{
		if (STOP_TIME[i] < 0) {
			cnt = 0;
			do {
				cnt++;
				for (int j = MAX_CAR_LENGTH - 1; j > 0; j--)
				{
					X[i][j] = X[i][j - 1];
				}
				X[i][0].x - 1 == 0 ? X[i][0].x = WIDTH_CONSOLE - 1 : X[i][0].x--; // Kiểm tra xem xe có đụng màn hình không 
			} while (cnt < SPEED);
		}
	}
}

//Buoc 12
// Hàm xóa xe (xóa có nghĩa là không vẽ) 
void EraseCars()
{
	//system("cls");

	for (int i = 0; i < MAX_CAR; i += 2) {
		cnt = 0;
		do {
			GotoXY(X[i][MAX_CAR_LENGTH - 1 - cnt].x, X[i][MAX_CAR_LENGTH - 1 - cnt].y);
			printf(" ");
			cnt++;
		} while (cnt < SPEED);
	}
	for (int i = 1; i < MAX_CAR; i += 2) {
		cnt = 0;
		do {
			GotoXY(X[i][0 + cnt].x, X[i][0 + cnt].y);
			printf(" ");
			cnt++;
		} while (cnt < SPEED);
	}
}
//Buoc 13
void MoveRight() {
	if (Y.x < WIDTH_CONSOLE - 1)
	{
		DrawSticker(Y, " ");
		Y.x++;
		DrawSticker(Y, "Y");
	}
}
void MoveLeft() {
	if (Y.x > 1)
	{
		DrawSticker(Y, " ");
		Y.x--;
		DrawSticker(Y, "Y");
	}
}
void MoveDown() {
	if (Y.y < HEIGH_CONSOLE - 1)
	{
		DrawSticker(Y, " ");
		Y.y++;
		DrawSticker(Y, "Y");
	}
}
void MoveUp() {
	if (Y.y > 1)
	{
		DrawSticker(Y, " ");
		Y.y--;
		DrawSticker(Y, "Y");
	}
}

// 4.2
void SaveGame(HANDLE t) {

	SuspendThread(t);

	//fflush(stdin);
	GotoXY(0, HEIGH_CONSOLE + 2);
	printf("Input file name to save game: ");

	char file_name[64];
	GotoXY(31, HEIGH_CONSOLE + 2);
	gets_s(file_name, 64);
	FILE *f = fopen(file_name, "wt");
	
	// xuất vị trí hiện tại của người đang qua đường
	fprintf(f, "%d %d ", Y.x, Y.y);

	// xuất tốc độ đang chạy
	fprintf(f, "%d\n", SPEED);

	// hàm xuất lịch sử người đã về đích
	writeHistoryToFile(f);
	
	fprintf(f, "\n");

	// xuất tất cả vị trí (trục Ox) các xe đang chạy
	for (int i = 0; i < MAX_CAR; i++)
		fprintf(f, "%d ", X[i][0].x - 1);

	fprintf(f, "\n");
	
	// xuất thời gian chạy/dừng của tất cả các xe
	for (int i = 0; i < MAX_CAR; i++)
		fprintf(f, "%d ", STOP_TIME[i]);

	fclose(f);
	
	GotoXY(0, HEIGH_CONSOLE + 3);
	printf("Game saved!!! Input any key to play");

	haveGARBAGE = true;
}

bool LoadFile(HANDLE t) {
	if (t != NULL)	SuspendThread(t);
	//fflush(stdin);

	system("cls");
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
	if (X != NULL) {
		DrawCars("-");
		DrawSticker(Y, "Y");
	}

	for (PointPeoplePre *p = getHeadPeoplePre(); p!=NULL; p=p->next)
	{
		DrawSticker({ p->x,1 }, "Y", 12);
	}

	GotoXY(0, HEIGH_CONSOLE + 2);
	printf("Input file name to load game: ");

	char file_name[64];

	
	gets_s(file_name, 64);
	FILE *f = fopen(file_name, "rt");

	if (f == NULL) return false;

	fscanf(f, "%d%d%d", &Y.x, &Y.y, &SPEED);

	readHistoryFromFile(f);

	if (X == NULL) {
		STOP_TIME = new INT[MAX_CAR];

		X = new POINT*[MAX_CAR];
		for (int i = 0; i < MAX_CAR; i++)
			X[i] = new POINT[MAX_CAR_LENGTH];
	}

	for (int i = 0; i < MAX_CAR; i++)
	{
		int temp;
		fscanf(f, "%d", &temp);
		for (int j = 0; j < MAX_CAR_LENGTH; j++)
		{
			//X[i][j].x = ((temp % (WIDTH_CONSOLE - MAX_CAR_LENGTH)) + 1 + j);
			X[i][j].x = ((temp + j) % (WIDTH_CONSOLE - 1)) + 1;
			X[i][j].y = 2 + i;
		}
	}

	for (int i = 0; i < MAX_CAR; i++)
		fscanf(f, "%d ", &STOP_TIME[i]);

	haveGARBAGE = true;

	return true;
}

void LoadGame(HANDLE t = NULL) {
	
	MOVING = 'D'; // Ban đầu cho người di chuyển sang phải   
	//SPEED = 1; // Tốc độ lúc đầu 
	//Y = { 18,19 }; // Vị trí lúc đầu của người 
				   // Tạo mảng xe chạy
	int temp = 0;
	while (!LoadFile(t))
	{
		GotoXY(0, HEIGH_CONSOLE + 2);
		printf("Type N to start new game or other to continue input file name other");
		if (toupper(getch()) == 'N') {
			StartGame();
			return;
		}
	}

	system("cls");
		
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
	// Vẽ màn hình game 

	// Vẽ từng người đã tới đích
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);

	for (PointPeoplePre *i = getHeadPeoplePre(); i != NULL; i=i->next)
	{
		GotoXY(i->x, 1);
		printf("Y");
	}
	// phục hồi lại màu của Sticker thành màu mặc định
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	STATE = true;//Bắt đầu cho Thread chạy 
	DrawCars(".");
	DrawSticker(Y, "0");
	//if (t != NULL) SuspendThread(t);

	GotoXY(0, HEIGH_CONSOLE + 3);
	printf("Game loaded!!! Input any key to play");
	haveGARBAGE = true;
}
// end 4.2

//Buoc 14
void SubThread()
{
	while (1) {
		if (STATE) //Nếu người vẫn còn sống    
		{
			switch (MOVING) //Kiểm tra biến moving 
			{
			case 'A':
				MoveLeft();
				break;
			case 'D':
				MoveRight();
				break;
			case 'W':
				MoveUp();
				break;
			case 'S':
				MoveDown();
				break;
			}
			MOVING = ' ';// Tạm khóa không cho di chuyển, chờ nhận phím từ hàm main 
			EraseCars();
			if (STOP == FALSE) MoveCars();
			DrawCars(".");
			if (IsImpact(Y, Y.y))
			{
				ProcessDead(); // Kiểm tra xe có đụng không 
			}
			if (Y.y == 1)
				ProcessFinish(Y); // Kiểm tra xem về đích chưa 
			Sleep(50);//Hàm ngủ theo tốc độ SPEED 
		}
	}
}

// 4.5
int MenuStart(){
	//STATE = 0;
	GotoXY(2, 5); // gắn vị trí ký tự đầu tiên của câu thông báo
	printf("Type t to play from file or any key to new game"); 
	GotoXY(0, 0); // gán lại vị trí nếu muốn ghi ký tự ra ngoài màn hình
	haveGARBAGE = true;
	return toupper(getch()); // trả về ký tự đã nhập
}
//end 4.5

void main()
{
	int temp;
	FixConsoleWindow();
	srand((unsigned int)time(NULL));

	while (1) {

		// 4.5
		if (MenuStart() == 'T') // nếu mới vào nhập từ t || T
			LoadGame(); // vào game bằng dữ liệu từ file
		else StartGame(); // còn không thì bắt đầu trò chơi với dữ liệu gốc mặc định
		// end 4.5

		thread t1(SubThread);

		SuspendThread(t1.native_handle());
		while (1)
		{
			temp = toupper(getch());
			if (STATE == 1)
			{
				if (temp == 27 || temp == 'E') {
					ExitGame(t1.native_handle());
					break;
				}
				else if (temp == 'P') {
					PauseGame(t1.native_handle());
				}

				// 4.2
				else if (temp == 'L') {
					STOP = TRUE;
					SaveGame(t1.native_handle());
					STOP = FALSE;
				}
				else if (temp == 'T') {
					LoadGame(t1.native_handle());
				}
				// end 4.2

				else {
					ResumeThread((HANDLE)t1.native_handle());
					haveGARBAGE = haveGARBAGE == true ? ClearConsole() : false;
					if (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S')
					{
						MOVING = temp;
					}
				}
			}
			else
			{
				if (temp == 'Y')
					StartGame();
				else {
					ExitGame(t1.native_handle());
					break;
				}
			}
		}
	}
}
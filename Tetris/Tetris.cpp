// Tetris.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "DxLib.h"
#include "string.h"

int field[22][12], block[7][4][4], bsi[7], bsj[7], bsn[7], ranking[3][7];//グローバル変数の定義1
int gameset[100][3], deletei[6], next[3], gamemode[4], shufflebl[7];//グローバル変数の定義2
int gameover, point, ro, bbi, bbj, holdn, hold, now, line, keyflag, set, shufflebli, gamestarttime;//グローバル変数の定義3
char deletestr[4][10], keyset[11][15];//グローバル変数の定義4

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);//ウインドウ作成用関数
void Menu();//メニュー用関数
void Menu_S(int select);//メニュー表示用関数
void Game();//ゲーム管理用関数
void Init();//初期化用関数
void File();//設定ファイル用関数
int Keyset(char c[15]);//キー設定用関数
void GamemodeOpen();//ゲームモードファイルを開く関数
void GamemodeSave();//ゲームモードファイルをセーブする関数
void RankingFileOpen();//ランキングファイルを開く関数
void RankingFileSave();//ランキングファイルをセーブする関数
void Ranking();//ランキング表示用関数
void Show();//画面表示関数
void ShowField(int i, int j, int n, int m);//ブロック画面表示関数
int Key();//キー入力関数
int Change(int cx, int cy);//ブロックを移動させる関数
int Check(int cx, int cy);//ブロックが移動できるか判断する関数
void Delete(int n);//ブロックを削除する関数
void Setting(int h);//ブロックを設置する関数
int Shuffle();//ブロックの順番を指定する関数
void Rotation(int r);//ブロックを回転させる関数
int Rotationf(int i, int j, int n);//回転したときの座標値を演算する関数
void Disturbance();//邪魔ブロック出現用関数
void DropPoint();//落下地点算出関数
void Result();//ゲームオーバー後の画面用関数

int main() {
	WinMain(0, 0, 0, 0);
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	SetMainWindowText("Tetris");
	ChangeWindowMode(TRUE);
	SetGraphMode(300, 600, 32);
	SetBackgroundColor(255, 255, 255);
	if (DxLib_Init() == -1)return -1;
	Menu();
	DxLib_End();
	return 0;
}

void Menu() {
	int i;
	Init();
	for (i = 0; i < 4; i++)gamemode[i] = 0;
	GamemodeOpen();
	Menu_S(0);
}

void Menu_S(int select) {
	int i, cl, menux;
	char selectname[7][20], gamemodename[4][11][20];
	strcpy_s(selectname[0], "ゲーム開始");
	strcpy_s(selectname[1], "速度アップ");
	strcpy_s(selectname[2], "ホールド");
	strcpy_s(selectname[3], "邪魔ブロック");
	strcpy_s(selectname[4], "シャッフル方法");
	strcpy_s(selectname[5], "ランキング");
	strcpy_s(selectname[6], "終了");
	for (i = 0; i < 11; i++)sprintf_s(gamemodename[0][i], 20, "Lv.%d", i);
	for (i = 0; i < 6; i++)sprintf_s(gamemodename[2][i], 20, "Lv.%d", i);
	strcpy_s(gamemodename[1][0], "ON");
	strcpy_s(gamemodename[1][1], "OFF");
	strcpy_s(gamemodename[3][0], "ノーマル");
	strcpy_s(gamemodename[3][1], "完全シャッフル");
	strcpy_s(gamemodename[3][2], "Iミノ出現率低下");
	while (ProcessMessage() == 0) {
		ClearDrawScreen();
		for (i = 0; i < 7; i++) {
			if (select == i) {
				cl = GetColor(255, 0, 0);
				menux = 0;
			}
			else {
				cl = GetColor(0, 0, 0);
				menux = 20;
			}
			DrawFormatString(menux, i * 20, cl, "%s", selectname[i]);
			if (i > 0 && i < 5)DrawFormatString(menux + 120, i * 20, cl, "%s", gamemodename[i - 1][gamemode[i - 1]]);
		}
		DrawFormatString(0, 280, GetColor(0, 0, 0), "%s:上　%s:下　%s:決定", keyset[7], keyset[8], keyset[9]);
		ScreenFlip();
		if (CheckHitKey(gameset[23][0]) == 1) {
			if (select == 0)select = 6;
			else select--;
			WaitTimer(100);
		}
		else if (CheckHitKey(gameset[24][0]) == 1) {
			if (select == 6)select = 0;
			else select++;
			WaitTimer(100);
		}
		else if (CheckHitKey(gameset[25][0]) == 1)break;
	}
	if (select == 0) {
		GamemodeSave();
		WaitTimer(200);
		Game();
	}
	else if (select > 0 && select < 5) {
		if (select == 1 && gamemode[0] == 10)gamemode[0] = 0;
		else if (select == 2 && gamemode[1] == 1)gamemode[1] = 0;
		else if (select == 3 && gamemode[2] == 5)gamemode[2] = 0;
		else if (select == 4 && gamemode[3] == 2)gamemode[3] = 0;
		else gamemode[select - 1]++;
		GamemodeSave();
		WaitTimer(200);
		Menu_S(select);
	}
	else if (select == 5)Ranking();
}

void Game() {
	int i, timer, starttime, turn, key;
	gamestarttime = GetNowCount();
	if (gameset[13][0]) {
		ClearDrawScreen();//画面クリア
		DrawStringToHandle(40, 60, "操作方法", GetColor(0, 0, 0), CreateFontToHandle("", 25, 4, DX_FONTTYPE_NORMAL));
		DrawFormatString(40, 100, GetColor(0, 0, 0), "%s:右", keyset[1]);
		DrawFormatString(40, 120, GetColor(0, 0, 0), "%s:左", keyset[0]);
		DrawFormatString(40, 140, GetColor(0, 0, 0), "%s:下", keyset[2]);
		DrawFormatString(40, 160, GetColor(0, 0, 0), "%s:ホールド", keyset[3]);
		DrawFormatString(40, 180, GetColor(0, 0, 0), "%s:回転", keyset[4]);
		DrawFormatString(40, 200, GetColor(0, 0, 0), "%s:逆回転", keyset[5]);
		DrawFormatString(40, 220, GetColor(0, 0, 0), "%s:ハードドロップ", keyset[6]);
		DrawFormatString(40, 240, GetColor(0, 0, 0), "%s:ゲーム終了", keyset[10]);
		DrawFormatString(40, 580, GetColor(0, 0, 0), "キーボードを押してください");
		ScreenFlip();//画面反映
		WaitKey();//キーが押されるまで待機
		WaitTimer(200);
	}
	turn = 0;
	key = 0;
	while (gameover&&ProcessMessage() == 0 && key == 0) {//ゲームオーバーになるまでループ
		Show();
		starttime = GetNowCount();
		if (gamemode[0] != 0)timer = 1000 - (point / (110 - gamemode[0] * 10));
		else timer = 1000;
		if (timer < 10)timer = 10;
		while (GetNowCount() - starttime < timer&&key == 0)key = Key();
		if (key == 0) {
			if (deletei[0] != -1 && deletei[1] != 0 && gameset[14][0]) {
				deletei[1]--;
			}
			else {
				deletei[0] = -1;
				deletei[1] = 0;
			}
			i = Change(1, 0);//下に落とす
			turn++;
			if (turn % ((6 - gamemode[2]) * 10) == 0 && gamemode[2] != 0)Disturbance();
			set++;
		}
	}
	if (key == 0) {
		Show();
		WaitKey();//キーが押されるまで待機
		Result();
	}
	else if (key == -1) {
		Result();
		Menu();
	}
}

void Init() {
	int i, j, k;
	point = 0;
	gameover = 1;
	now = -1;
	set = 0;
	for (i = 0; i < 7; i++)shufflebl[i] = 0;
	shufflebli = 0;
	for (i = 0; i < 3; i++)next[i] = -1;
	hold = -1;
	line = 0;
	keyflag = -1;
	strcpy_s(deletestr[0], 10, "Single");
	strcpy_s(deletestr[1], 10, "Double");
	strcpy_s(deletestr[2], 10, "Triple");
	strcpy_s(deletestr[3], 10, "Tetris");
	deletei[0] = -1;
	for (i = 1; i < 6; i++)deletei[i] = 0;
	for (i = 0; i < 22; i++) {
		for (j = 0; j < 12; j++) {
			if (i < 7 && j < 4) {
				for (k = 0; k < 4; k++) {
					block[i][j][k] = 0;
				}
			}
			if (i == 0 || i == 21 || j == 0 || j == 11) {//外壁
				field[i][j] = -1;
			}
			else {
				field[i][j] = 0;
			}
		}
	}
	gameset[12][0] = 1;
	gameset[13][0] = 1;
	gameset[14][0] = 1;
	gameset[14][1] = 2;
	gameset[15][0] = 1;
	gameset[15][1] = 0;
	gameset[16][0] = KEY_INPUT_A;
	gameset[17][0] = KEY_INPUT_D;
	gameset[18][0] = KEY_INPUT_S;
	gameset[19][0] = KEY_INPUT_Q;
	gameset[20][0] = KEY_INPUT_W;
	gameset[21][0] = KEY_INPUT_E;
	gameset[22][0] = KEY_INPUT_SPACE;
	gameset[23][0] = KEY_INPUT_W;
	gameset[24][0] = KEY_INPUT_S;
	gameset[25][0] = KEY_INPUT_SPACE;
	gameset[27][0] = KEY_INPUT_ESCAPE;
	strcpy_s(keyset[0], 15, "A");
	strcpy_s(keyset[1], 15, "D");
	strcpy_s(keyset[2], 15, "S");
	strcpy_s(keyset[3], 15, "Q");
	strcpy_s(keyset[4], 15, "W");
	strcpy_s(keyset[5], 15, "E");
	strcpy_s(keyset[6], 15, "SPACE");
	strcpy_s(keyset[7], 15, "W");
	strcpy_s(keyset[8], 15, "S");
	strcpy_s(keyset[9], 15, "SPACE");
	strcpy_s(keyset[10], 15, "ESCAPE");
	gameset[11][0] = 105;
	gameset[11][1] = 105;
	gameset[11][2] = 105;
	gameset[7][0] = 255;
	gameset[7][1] = 255;
	gameset[7][2] = 255;
	gameset[26][0] = 50;
	gameset[26][1] = 50;
	gameset[26][2] = 50;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			gameset[i + 8][j] = 192;
		}
	}
	for (i = 0; i < 7; i++) {//ミノの設定
		switch (i) {
		case 0://I
			block[i][1][0] = 1;
			block[i][1][1] = 1;
			block[i][1][2] = 1;
			block[i][1][3] = 1;
			bsi[i] = 1;
			bsj[i] = 2;
			bsn[i] = 2;
			gameset[i][0] = 255;
			gameset[i][1] = 0;
			gameset[i][2] = 0;
			break;
		case 1://O
			block[i][1][1] = 1;
			block[i][1][2] = 1;
			block[i][2][1] = 1;
			block[i][2][2] = 1;
			bsi[i] = 1;
			bsj[i] = 1;
			bsn[i] = 0;
			gameset[i][0] = 255;
			gameset[i][1] = 255;
			gameset[i][2] = 0;
			break;
		case 2://Z
			block[i][1][0] = 1;
			block[i][1][1] = 1;
			block[i][2][1] = 1;
			block[i][2][2] = 1;
			bsi[i] = 1;
			bsj[i] = 1;
			bsn[i] = 4;
			gameset[i][0] = 0;
			gameset[i][1] = 255;
			gameset[i][2] = 0;
			break;
		case 3://S
			block[i][2][0] = 1;
			block[i][2][1] = 1;
			block[i][1][1] = 1;
			block[i][1][2] = 1;
			bsi[i] = 1;
			bsj[i] = 1;
			bsn[i] = 4;
			gameset[i][0] = 255;
			gameset[i][1] = 0;
			gameset[i][2] = 255;
			break;
		case 4://J
			block[i][1][0] = 1;
			block[i][2][0] = 1;
			block[i][2][1] = 1;
			block[i][2][2] = 1;
			bsi[i] = 2;
			bsj[i] = 1;
			bsn[i] = 4;
			gameset[i][0] = 0;
			gameset[i][1] = 0;
			gameset[i][2] = 255;
			break;
		case 5://L
			block[i][2][0] = 1;
			block[i][2][1] = 1;
			block[i][2][2] = 1;
			block[i][1][2] = 1;
			bsi[i] = 2;
			bsj[i] = 1;
			bsn[i] = 4;
			gameset[i][0] = 255;
			gameset[i][1] = 165;
			gameset[i][2] = 0;
			break;
		case 6://T
			block[i][2][0] = 1;
			block[i][2][1] = 1;
			block[i][1][1] = 1;
			block[i][2][2] = 1;
			bsi[i] = 2;
			bsj[i] = 1;
			bsn[i] = 4;
			gameset[i][0] = 0;
			gameset[i][1] = 255;
			gameset[i][2] = 255;
			break;
		}
	}
	File();
	Setting(1);
}

void File() {
	int n, i;
	char c[15], fco[100][15], tff[4][10];
	FILE *fp;
	strcpy_s(fco[0], 15, "mino-I");
	strcpy_s(fco[1], 15, "mino-O");
	strcpy_s(fco[2], 15, "mino-Z");
	strcpy_s(fco[3], 15, "mino-S");
	strcpy_s(fco[4], 15, "mino-J");
	strcpy_s(fco[5], 15, "mino-L");
	strcpy_s(fco[6], 15, "mino-T");
	strcpy_s(fco[7], 15, "background");
	strcpy_s(fco[8], 15, "frame-NEXT");
	strcpy_s(fco[9], 15, "frame-HOLD");
	strcpy_s(fco[10], 15, "frame-FIELD");
	strcpy_s(fco[11], 15, "mino-DROP");
	strcpy_s(fco[12], 15, "droppoint");
	strcpy_s(fco[13], 15, "how_to_play");
	strcpy_s(fco[14], 15, "judgment");
	strcpy_s(fco[15], 15, "input");
	strcpy_s(fco[16], 15, "key-LEFT");
	strcpy_s(fco[17], 15, "key-RIGHT");
	strcpy_s(fco[18], 15, "key-DOWN");
	strcpy_s(fco[19], 15, "key-HOLD");
	strcpy_s(fco[20], 15, "key-TURN-0");
	strcpy_s(fco[21], 15, "key-TURN-1");
	strcpy_s(fco[22], 15, "key-HARD-DROP");
	strcpy_s(fco[23], 15, "key-MenuUP");
	strcpy_s(fco[24], 15, "key-MenuDOWN");
	strcpy_s(fco[25], 15, "key-MenuENTER");
	strcpy_s(fco[26], 15, "disturbance");
	strcpy_s(fco[27], 15, "key-FIN");//ここまでコマンド設定
	strcpy_s(tff[0], 10, "false");
	strcpy_s(tff[1], 10, "true");
	strcpy_s(tff[2], 10, "FALSE");
	strcpy_s(tff[3], 10, "TRUE");
	if (fopen_s(&fp, "Data\\Setting.txt", "r") != 0)printf("Don't open the setting file\n");//ファイルが開けない場合
	else while (!feof(fp)) {//ファイルが開ける場合
		n = -1;
		fscanf_s(fp, "%s", c, 15);
		for (i = 0; i < 100; i++)if (strcmp(c, fco[i]) == 0)n = i;
		if ((n != -1 && n < 12) || n == 26) {
			for (i = 0; i < 3; i++)fscanf_s(fp, "%d", &gameset[n][i]);
		}
		else if (n != -1 && (n >= 12 && n <= 15)) {
			fscanf_s(fp, "%s", c, 15);
			if (strcmp(c, tff[0]) == 0 || strcmp(c, tff[2]) == 0)gameset[n][0] = 0;
			else if (strcmp(c, tff[1]) == 0 || strcmp(c, tff[3]) == 0)gameset[n][0] = 1;
			if (n == 14)fscanf_s(fp, "%d", &gameset[n][1]);
		}
		else if (n != -1 && ((n >= 16 && n <= 25) || n == 27)) {
			fscanf_s(fp, "%s", c, 15);
			if (Keyset(c) != -1) {
				gameset[n][0] = Keyset(c);
				if (n != 27)strcpy_s(keyset[n - 16], 15, c);
				else if (n == 27)strcpy_s(keyset[10], 15, c);
			}
			else printf("setting file error\n");
		}
		else {
			printf("setting file error\n");
			break;
		}
	}
	fclose(fp);
}

int Keyset(char c[15]) {
	if (strncmp(c, "NUMPADENTER\0", 12) == 0)return KEY_INPUT_NUMPADENTER;
	else if (strncmp(c, "PREVTRACK\0", 10) == 0)return KEY_INPUT_PREVTRACK;
	else if (strncmp(c, "SEMICOLON\0", 10) == 0)return KEY_INPUT_SEMICOLON;
	else if (strncmp(c, "BACKSLASH\0", 10) == 0)return KEY_INPUT_BACKSLASH;
	else if (strncmp(c, "LCONTROL\0", 9) == 0)return KEY_INPUT_LCONTROL;
	else if (strncmp(c, "RCONTROL\0", 9) == 0)return KEY_INPUT_RCONTROL;
	else if (strncmp(c, "LBRACKET\0", 9) == 0)return KEY_INPUT_LBRACKET;
	else if (strncmp(c, "RBRACKET\0", 9) == 0)return KEY_INPUT_RBRACKET;
	else if (strncmp(c, "CAPSLOCK\0", 9) == 0)return KEY_INPUT_CAPSLOCK;
	else if (strncmp(c, "MULTIPLY\0", 9) == 0)return KEY_INPUT_MULTIPLY;
	else if (strncmp(c, "SUBTRACT\0", 9) == 0)return KEY_INPUT_SUBTRACT;
	else if (strncmp(c, "NUMPAD0\0", 8) == 0)return KEY_INPUT_NUMPAD0;
	else if (strncmp(c, "NUMPAD1\0", 8) == 0)return KEY_INPUT_NUMPAD1;
	else if (strncmp(c, "NUMPAD2\0", 8) == 0)return KEY_INPUT_NUMPAD2;
	else if (strncmp(c, "NUMPAD3\0", 8) == 0)return KEY_INPUT_NUMPAD3;
	else if (strncmp(c, "NUMPAD4\0", 8) == 0)return KEY_INPUT_NUMPAD4;
	else if (strncmp(c, "NUMPAD5\0", 8) == 0)return KEY_INPUT_NUMPAD5;
	else if (strncmp(c, "NUMPAD6\0", 8) == 0)return KEY_INPUT_NUMPAD6;
	else if (strncmp(c, "NUMPAD7\0", 8) == 0)return KEY_INPUT_NUMPAD7;
	else if (strncmp(c, "NUMPAD8\0", 8) == 0)return KEY_INPUT_NUMPAD8;
	else if (strncmp(c, "NUMPAD9\0", 8) == 0)return KEY_INPUT_NUMPAD9;
	else if (strncmp(c, "DECIMAL\0", 8) == 0)return KEY_INPUT_DECIMAL;
	else if (strncmp(c, "RETURN\0", 7) == 0)return KEY_INPUT_RETURN;
	else if (strncmp(c, "LSHIFT\0", 7) == 0)return KEY_INPUT_LSHIFT;
	else if (strncmp(c, "RSHIFT\0", 7) == 0)return KEY_INPUT_RSHIFT;
	else if (strncmp(c, "ESCAPE\0", 7) == 0)return KEY_INPUT_ESCAPE;
	else if (strncmp(c, "INSERT\0", 7) == 0)return KEY_INPUT_INSERT;
	else if (strncmp(c, "DELETE\0", 7) == 0)return KEY_INPUT_DELETE;
	else if (strncmp(c, "PERIOD\0", 7) == 0)return KEY_INPUT_PERIOD;
	else if (strncmp(c, "SCROLL\0", 7) == 0)return KEY_INPUT_SCROLL;
	else if (strncmp(c, "DIVIDE\0", 7) == 0)return KEY_INPUT_DIVIDE;
	else if (strncmp(c, "SLASH\0", 6) == 0)return KEY_INPUT_SLASH;
	else if (strncmp(c, "SPACE\0", 6) == 0)return KEY_INPUT_SPACE;
	else if (strncmp(c, "RIGHT\0", 6) == 0)return KEY_INPUT_RIGHT;
	else if (strncmp(c, "MINUS\0", 6) == 0)return KEY_INPUT_MINUS;
	else if (strncmp(c, "COLON\0", 6) == 0)return KEY_INPUT_COLON;
	else if (strncmp(c, "COMMA\0", 6) == 0)return KEY_INPUT_COMMA;
	else if (strncmp(c, "PAUSE\0", 6) == 0)return KEY_INPUT_PAUSE;
	else if (strncmp(c, "PGDN\0", 5) == 0)return KEY_INPUT_PGDN;
	else if (strncmp(c, "HOME\0", 5) == 0)return KEY_INPUT_HOME;
	else if (strncmp(c, "LEFT\0", 5) == 0)return KEY_INPUT_LEFT;
	else if (strncmp(c, "DOWN\0", 5) == 0)return KEY_INPUT_DOWN;
	else if (strncmp(c, "LALT\0", 5) == 0)return KEY_INPUT_LALT;
	else if (strncmp(c, "RALT\0", 5) == 0)return KEY_INPUT_RALT;
	else if (strncmp(c, "BACK\0", 5) == 0)return KEY_INPUT_BACK;
	else if (strncmp(c, "YEN\0", 4) == 0)return KEY_INPUT_YEN;
	else if (strncmp(c, "F10\0", 4) == 0)return KEY_INPUT_F10;
	else if (strncmp(c, "F11\0", 4) == 0)return KEY_INPUT_F11;
	else if (strncmp(c, "F12\0", 4) == 0)return KEY_INPUT_F12;
	else if (strncmp(c, "TAB\0", 4) == 0)return KEY_INPUT_TAB;
	else if (strncmp(c, "END\0", 4) == 0)return KEY_INPUT_END;
	else if (strncmp(c, "ADD\0", 4) == 0)return KEY_INPUT_ADD;
	else if (strncmp(c, "AT\0", 3) == 0)return KEY_INPUT_AT;
	else if (strncmp(c, "F1\0", 3) == 0)return KEY_INPUT_F1;
	else if (strncmp(c, "F2\0", 3) == 0)return KEY_INPUT_F2;
	else if (strncmp(c, "F3\0", 3) == 0)return KEY_INPUT_F3;
	else if (strncmp(c, "F4\0", 3) == 0)return KEY_INPUT_F4;
	else if (strncmp(c, "F5\0", 3) == 0)return KEY_INPUT_F5;
	else if (strncmp(c, "F6\0", 3) == 0)return KEY_INPUT_F6;
	else if (strncmp(c, "F7\0", 3) == 0)return KEY_INPUT_F7;
	else if (strncmp(c, "F8\0", 3) == 0)return KEY_INPUT_F8;
	else if (strncmp(c, "F9\0", 3) == 0)return KEY_INPUT_F9;
	else if (strncmp(c, "UP\0", 3) == 0)return KEY_INPUT_UP;
	else if (strncmp(c, "A\0", 2) == 0)return KEY_INPUT_A;
	else if (strncmp(c, "B\0", 2) == 0)return KEY_INPUT_B;
	else if (strncmp(c, "C\0", 2) == 0)return KEY_INPUT_C;
	else if (strncmp(c, "D\0", 2) == 0)return KEY_INPUT_D;
	else if (strncmp(c, "E\0", 2) == 0)return KEY_INPUT_E;
	else if (strncmp(c, "F\0", 2) == 0)return KEY_INPUT_F;
	else if (strncmp(c, "G\0", 2) == 0)return KEY_INPUT_G;
	else if (strncmp(c, "H\0", 2) == 0)return KEY_INPUT_H;
	else if (strncmp(c, "I\0", 2) == 0)return KEY_INPUT_I;
	else if (strncmp(c, "J\0", 2) == 0)return KEY_INPUT_J;
	else if (strncmp(c, "K\0", 2) == 0)return KEY_INPUT_K;
	else if (strncmp(c, "L\0", 2) == 0)return KEY_INPUT_L;
	else if (strncmp(c, "M\0", 2) == 0)return KEY_INPUT_M;
	else if (strncmp(c, "N\0", 2) == 0)return KEY_INPUT_N;
	else if (strncmp(c, "O\0", 2) == 0)return KEY_INPUT_O;
	else if (strncmp(c, "P\0", 2) == 0)return KEY_INPUT_P;
	else if (strncmp(c, "Q\0", 2) == 0)return KEY_INPUT_Q;
	else if (strncmp(c, "R\0", 2) == 0)return KEY_INPUT_R;
	else if (strncmp(c, "S\0", 2) == 0)return KEY_INPUT_S;
	else if (strncmp(c, "T\0", 2) == 0)return KEY_INPUT_T;
	else if (strncmp(c, "U\0", 2) == 0)return KEY_INPUT_U;
	else if (strncmp(c, "V\0", 2) == 0)return KEY_INPUT_V;
	else if (strncmp(c, "W\0", 2) == 0)return KEY_INPUT_W;
	else if (strncmp(c, "X\0", 2) == 0)return KEY_INPUT_X;
	else if (strncmp(c, "Y\0", 2) == 0)return KEY_INPUT_Y;
	else if (strncmp(c, "Z\0", 2) == 0)return KEY_INPUT_Z;
	else if (strncmp(c, "0\0", 2) == 0)return KEY_INPUT_0;
	else if (strncmp(c, "1\0", 2) == 0)return KEY_INPUT_1;
	else if (strncmp(c, "2\0", 2) == 0)return KEY_INPUT_2;
	else if (strncmp(c, "3\0", 2) == 0)return KEY_INPUT_3;
	else if (strncmp(c, "4\0", 2) == 0)return KEY_INPUT_4;
	else if (strncmp(c, "5\0", 2) == 0)return KEY_INPUT_5;
	else if (strncmp(c, "6\0", 2) == 0)return KEY_INPUT_6;
	else if (strncmp(c, "7\0", 2) == 0)return KEY_INPUT_7;
	else if (strncmp(c, "8\0", 2) == 0)return KEY_INPUT_8;
	else if (strncmp(c, "9\0", 2) == 0)return KEY_INPUT_9;
	else return -1;
}

void GamemodeOpen() {
	int i, n;
	char c[9];
	FILE *fp;
	if (fopen_s(&fp, "Data\\gamemode.dat", "rb") != 0)printf("Don't open the gamemode file\n");
	else {
		fread(&c, 1, 9, fp);
		sscanf_s(c, "%d", &n);
		for (i = 0; i < 4; i++) {
			gamemode[3 - i] = n % 100;
			n = n - n % 100;
			n /= 100;
		}
	}
	fclose(fp);
}

void GamemodeSave() {
	char c[9];
	FILE *fp;
	if (fopen_s(&fp, "Data\\gamemode.dat", "wb") != 0)printf("Don't open the gamemode file\n");
	else {
		sprintf_s(c, 9, "%02d%02d%02d%02d", gamemode[0], gamemode[1], gamemode[2], gamemode[3]);
		fwrite(c, 1, 9, fp);
	}
	fclose(fp);
}

void RankingFileOpen() {
	int i, j;
	FILE *fp;
	if (fopen_s(&fp, "Data\\ranking.txt", "r") != 0)printf("Don't open the ranking file\n");
	else {
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 7; j++) {
				fscanf_s(fp, "%d", &ranking[i][j]);
			}
		}
	}
	fclose(fp);
}

void RankingFileSave() {
	int i, j;
	FILE *fp;
	if (fopen_s(&fp, "Data\\ranking.txt", "w") != 0)printf("Don't open the ranking file\n");
	else {
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 7; j++) {
				fprintf_s(fp, "%d\t", ranking[i][j]);
			}
			fprintf_s(fp, "\n");
		}
	}
	fclose(fp);
}

void Ranking() {
	int i;
	RankingFileOpen();
	ClearDrawScreen();
	for (i = 0; i < 3; i++) {
		DrawFormatString(10, i * 60 + 40, GetColor(0, 0, 0), "%d位 %04d/%02d/%02d %02d:%02d:%02d", i + 1, ranking[i][0], ranking[i][1], ranking[i][2], ranking[i][3], ranking[i][4], ranking[i][5]);
		DrawFormatString(80, i * 60 + 60, GetColor(0, 0, 0), "SCORE:%d", ranking[i][6]);
	}
	DrawFormatString(40, 580, GetColor(0, 0, 0), "キーボードを押してください");
	ScreenFlip();
	WaitKey();
	WaitTimer(200);
	Menu();
}

void Show() {
	int i, j;
	ClearDrawScreen();//画面クリア
	DrawBox(30, 140, 280, 580, GetColor(gameset[7][0], gameset[7][1], gameset[7][2]), TRUE);//背景
	DrawFormatString(30, 0, GetColor(0, 0, 0), "LINE:%d　SCORE:%d", line, point);//ライン数・ポイント表示
	if (gamemode[1] == 0)DrawFormatString(28, 20, GetColor(0, 0, 0), "HOLD");
	DrawFormatString(128, 20, GetColor(0, 0, 0), "NEXT");
	if (gamemode[1] == 0)DrawBox(28, 38, 112, 122, GetColor(gameset[9][0], gameset[9][1], gameset[9][2]), TRUE);//枠作成1
	DrawBox(128, 38, 212, 122, GetColor(gameset[8][0], gameset[8][1], gameset[8][2]), TRUE);//枠作成2
	DrawBox(219, 39, 261, 79, GetColor(gameset[8][0], gameset[8][1], gameset[8][2]), TRUE);//枠作成3
	DrawBox(219, 81, 261, 121, GetColor(gameset[8][0], gameset[8][1], gameset[8][2]), TRUE);//枠作成4

	for (i = 0; i < 22; i++) {//ゲーム画面の表示
		for (j = 0; j < 12; j++) {
			ShowField(i, j, 0, 0);
		}
	}
	if (gameover == 1) {
		if (gameset[12][0])DropPoint();
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (next[0] != -1)ShowField(i, j, 1, 2);
				if (next[1] != -1)ShowField(i, j, 1, 3);
				if (next[2] != -1)ShowField(i, j, 1, 4);
				if (hold != -1)ShowField(i, j, 1, 1);
			}
		}
	}
	else {
		DrawStringToHandle(50, 300, "GAMEOVER", GetColor(0, 0, 0), CreateFontToHandle("", 50, 9, DX_FONTTYPE_NORMAL));
		DrawFormatString(30, 580, GetColor(0, 0, 0), "キーボードを押してください");
	}
	if (deletei[0] != -1 && deletei[1] != 0 && gameset[14][0])DrawFormatString(130, 350, GetColor(0, 0, 0), "%s", deletestr[deletei[0]]);
	ScreenFlip();//画面反映
}

void ShowField(int i, int j, int n, int m) {
	int x, a, bx, by, c, xp, yp, xy;
	if (m == 1)a = hold;
	else if (m >= 2) {
		a = next[m - 2];
	}
	if (n == 0) {
		x = field[i][j];
		if (x == 102)c = 26;
		else  c = (x - (x % 10)) / 10;
		xy = 20;
		xp = 30 + j * 20;
		yp = 140 + i * 20;
	}
	else if (n == 1 && m <= 2) {
		x = block[a][i][j];
		c = a;
		xy = 20;
		xp = 30 + j * xy + (m - 1) * 100;
		yp = 40 + i * xy;
	}
	else if (n == 1 && m > 2) {
		switch (m) {
		case 3:bx = 220; by = 40; break;
		case 4:bx = 220; by = 82; break;
		}
		x = block[a][i][j];
		c = a;
		xy = 10;
		xp = bx + j * xy;
		yp = by + i * xy;
	}
	if (x > 0) {
		DrawBox(xp + xy / 10, yp + xy / 10, xp + xy - xy / 10, yp + xy - xy / 10, GetColor(gameset[c][0], gameset[c][1], gameset[c][2]), TRUE);//ボックス絵画
	}
	else if (x < 0) {
		DrawBox(xp + xy / 20, yp + xy / 20, xp + xy - xy / 20, yp + xy - xy / 20, GetColor(gameset[10][0], gameset[10][1], gameset[10][2]), TRUE);//外壁絵画
	}
}

int Key() {
	int a;
	char Buf[256];
	GetHitKeyStateAll(Buf);
	if (gameover && (keyflag == -1 || (gameset[15][0] == 1 && Buf[keyflag] != 1) || gameset[15][0] == 0)) {
		if (Buf[keyflag = gameset[16][0]] == 1)a = Change(0, -1);//左へ移動
		else if (Buf[keyflag = gameset[17][0]] == 1)a = Change(0, 1);//右へ移動
		else if (Buf[keyflag = gameset[18][0]] == 1)a = Change(1, 0);//下へ移動
		else if (Buf[keyflag = gameset[22][0]] == 1) {//ハードドロップ
			while (Change(1, 0));
			WaitTimer(50);
		}
		else if (Buf[keyflag = gameset[20][0]] == 1 && bsn[now] != 0)Rotation(1);//回転
		else if (Buf[keyflag = gameset[21][0]] == 1 && bsn[now] != 0)Rotation(-1);//逆回転
		else if (Buf[keyflag = gameset[19][0]] == 1 && holdn == 1 && gamemode[1] == 0) {//ホールド
			Delete(1);
			Setting(2);
		}
		else if (Buf[keyflag = gameset[27][0]] == 1)return -1;//ゲーム終了
		else keyflag = -1;
	}
	Show();
	return 0;
}

int Change(int cx, int cy) {
	int i, j, n, in = 0, jn = 0, ci, cj;
	if (cx > 0)in = 21;
	if (cy > 0)jn = 11;
	bbi += cx;
	bbj += cy;
	n = Check(cx, cy);
	for (i = 0; i < 22; i++) {
		for (j = 0; j < 12; j++) {
			ci = i - in;
			cj = j - jn;
			if (ci < 0)ci *= -1;
			if (cj < 0)cj *= -1;
			if (field[ci][cj] % 10 == 1 && n == 1 && cy == 0) {
				field[ci][cj] = 2 + now * 10;
			}
			else if (field[ci][cj] % 10 == 1 && n == 0) {
				field[ci + cx][cj + cy] = 1 + now * 10;
				field[ci][cj] = 0;
			}
		}
	}
	if (n == 1 && cy == 0) {
		Delete(2);
		Setting(1);
		return 0;
	}
	return 1;
}

int Check(int cx, int cy) {
	int i, j, n = 0;
	for (i = 21; i >= 0; i--) {
		for (j = 0; j < 12; j++) {
			if (field[i][j] % 10 == 1 && (field[i + cx][j + cy] == -1 || field[i + cx][j + cy] % 10 == 2))n = 1;
		}
	}
	return n;
}

void Delete(int n) {
	int i, j, k, c, p = 0;
	for (i = 21; i >= 0; i--) {
		c = 0;
		for (j = 11; j >= 0; j--) {
			if (field[i][j] % 10 == 1 && n == 1)field[i][j] = 0;//現在移動中ブロックの削除
			else if (field[i][j] % 10 == 2 && n == 2)c++;
		}
		if (c == 10 && n == 2) {//一列削除
			p++;
			for (j = 1; j < 11; j++) {
				field[i][j] = 0;
			}
			for (j = i; j >= 0; j--) {
				for (k = 0; k < 12; k++) {
					if (field[j][k] % 10 == 2) {
						field[j + 1][k] = 2 + field[j][k] - field[j][k] % 10;
						field[j][k] = 0;
					}
				}
			}
			i = 21;
		}
	}
	if (p != 0) {
		point += 100 * p + 20 * (p - 1);//ポイント計算
		deletei[0] = p - 1;
		deletei[1] = gameset[14][1];
		deletei[p + 1]++;
		line += p;
	}
}

void Setting(int h) {
	int a, i, j;
	bbi = 0 + bsi[now];
	bbj = 4 + bsj[now];
	if (h == 2) {//ホールドの設置
		holdn = 0;
		a = now;
		if (hold == -1) {
			now = next[0];
			for (i = 0; i < 2; i++)next[i] = next[i + 1];
			next[2] = Shuffle();
		}
		else now = hold;
		hold = a;
	}
	else if (h == 1) {//通常のブロック設置
		holdn = 1;
		if (now != -1)now = next[0];
		else now = Shuffle();
		for (i = 0; i < 2; i++) {
			if (next[i + 1] != -1)next[i] = next[i + 1];
			else next[i] = Shuffle();
			if (i == 1)next[2] = Shuffle();
		}
	}
	ro = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[now][i][j] == 1 && field[i][j + 4] % 10 != 2) {
				field[i][j + 4] = 1 + now * 10;
				set = 0;
			}
			else if (block[now][i][j] == 1 && field[i][j + 4] % 10 == 2)gameover = 0;//ゲームオーバー判断
		}
	}
}

int Shuffle() {
	int sreturn, i, j, a, flags;
	if (gamemode[3] == 0) {
		if (shufflebli == 0) {
			for (i = 0; i < 7; i++) {
				flags = -1;
				while (flags != 0) {
					flags = 0;
					a = GetRand(6);
					for (j = 0; j < i; j++) {
						if (shufflebl[j] == a)flags++;
					}
				}
				shufflebl[i] = a;
			}
		}
		sreturn = shufflebl[shufflebli];
		if (shufflebli == 6)shufflebli = 0;
		else shufflebli++;
	}
	else if (gamemode[3] == 2) {
		a = GetRand(30);
		if (a < 5)sreturn = 1;
		else if (a >= 5 && a < 10)sreturn = 2;
		else if (a >= 10 && a < 15)sreturn = 3;
		else if (a >= 15 && a < 20)sreturn = 4;
		else if (a >= 20 && a < 25)sreturn = 5;
		else if (a >= 25 && a < 30)sreturn = 6;
		else if (a == 30)sreturn = 0;
	}
	else sreturn = GetRand(6);
	return sreturn;
}

void Rotation(int r) {
	int i, j, f = 0, in, jn;
	if (ro == 0)ro = 4;
	ro += r;//回転できるかの判断
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[now][i][j] == 1) {
				in = Rotationf(i, j, 0);
				jn = Rotationf(i, j, 1);
				if (field[in][jn] == 0 || field[in][jn] % 10 == 1)f++;
			}
		}
	}
	ro -= r;
	if (f == 4) {//回転させる
		ro += r;
		Delete(1);
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (block[now][i][j] == 1) {
					in = Rotationf(i, j, 0);
					jn = Rotationf(i, j, 1);
					field[in][jn] = 1 + now * 10;
				}
			}
		}
	}
}

int Rotationf(int i, int j, int n) {
	int in, jn, x, ii, jj;
	ii = bsi[now] - i;
	jj = bsj[now] - j;
	switch (ro % bsn[now]) {
	case 0:
		in = bbi + ii;
		jn = bbj + jj;
		break;
	case 1:
		in = bbi - jj;
		jn = bbj + ii;
		break;
	case 2:
		in = bbi - ii;
		jn = bbj - jj;
		break;
	case 3:
		in = bbi + jj;
		jn = bbj - ii;
		break;
	}
	if (n == 0)x = in;
	else if (n == 1)x = jn;
	return x;
}

void Disturbance() {
	int i, j, flag = 0;
	for (i = 0; i < 12; i++)if (field[1][i] > 0)flag++;
	if (flag == 0) {
		for (i = 2; i < 21; i++) {
			for (j = 1; j < 11; j++) {
				field[i - 1][j] = field[i][j];
				field[i][j] = 0;
			}
		}
	}
	else if (set != 0) gameover = 0;
	for (i = 1; i < 11; i++)field[20][i] = 102;
	field[20][GetRand(9) + 1] = 0;
}

void DropPoint() {
	int fields[22][12], i, j, xp, yp;
	for (i = 0; i < 22; i++) {//fields配列にfield配列を代入
		for (j = 0; j < 12; j++) {
			fields[i][j] = field[i][j];
		}
	}
	while (Check(1, 0) == 0) {//テトリス落下地点
		for (i = 21; i >= 0; i--) {
			for (j = 0; j < 12; j++) {
				if (field[i][j] % 10 == 1) {
					field[i + 1][j] = 1 + now * 10;
					field[i][j] = 0;
				}
			}
		}
	}
	for (i = 0; i < 22; i++) {
		for (j = 0; j < 12; j++) {
			if (field[i][j] != fields[i][j] && fields[i][j] % 10 != 1) {
				xp = 30 + j * 20;
				yp = 140 + i * 20;
				DrawBox(xp + 2, yp + 2, xp + 18, yp + 18, GetColor(gameset[11][0], gameset[11][1], gameset[11][2]), TRUE);//落下地点表示
			}
			field[i][j] = fields[i][j];
		}
	}
}

void Result() {
	int i, j, k, rankingnow = -1;
	double gamep;
	DATEDATA Date;
	if (gameover == 0)gamep = 1.05*(line * 10000.0) / (GetNowCount() - gamestarttime);
	else gamep = 1 * (line * 10000.0) / (GetNowCount() - gamestarttime);
	point = (int)(point * (gamemode[0] * 0.1 + 1)*(gamemode[1] * 0.1 + 1)*(gamemode[2] * 0.1 + 1)*gamep);
	RankingFileOpen();
	for (i = 0; i < 3; i++) {
		if (ranking[i][6] < point) {
			for (j = 2; j > i; j--) {
				for (k = 0; k < 7; k++) {
					ranking[j][k] = ranking[j - 1][k];
				}
			}
			GetDateTime(&Date);
			ranking[i][0] = Date.Year;
			ranking[i][1] = Date.Mon;
			ranking[i][2] = Date.Day;
			ranking[i][3] = Date.Hour;
			ranking[i][4] = Date.Min;
			ranking[i][5] = Date.Sec;
			ranking[i][6] = point;
			rankingnow = i + 1;
			break;
		}
	}
	RankingFileSave();
	ClearDrawScreen();//画面クリア
	DrawFormatString(50, 100, GetColor(0, 0, 0), "SCORE:%d", point);//ポイント表示
	DrawFormatString(50, 120, GetColor(0, 0, 0), "LINE:%d", line);//ライン数表示
	for (i = 0; i < 4; i++)DrawFormatString(50, 140 + i * 20, GetColor(0, 0, 0), "%s:%d", deletestr[i], deletei[i + 2]);
	if (rankingnow != -1)DrawFormatString(50, 340, GetColor(255, 0, 0), "記録更新:%d位", rankingnow);
	DrawFormatString(40, 580, GetColor(0, 0, 0), "キーボードを押してください");
	ScreenFlip();//画面反映
	WaitKey();//キーが押されるまで待機
	WaitTimer(200);
	Menu();
}
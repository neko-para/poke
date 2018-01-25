#include <bits/stdc++.h>
#include <ncurses.h>
using namespace std;

int color[4] = {
	COLOR_GREEN, COLOR_YELLOW,
	COLOR_BLUE, COLOR_MAGENTA
};

const char* text[13] = {
	"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
};

int hide[7] = { 0, 1, 2, 3, 4, 5, 6 };
vector<int> world[7];
vector<int> rest;
int restptr = -1;
int cur = 0;
int cho = -1;
int done[4];

int gen() {
	int arr[52];
	for (int i = 0; i < 52; ++i) {
		arr[i] = i;
	}
	random_shuffle(arr, arr + 52);
	int p = 0;
	for (int i = 0; i < 7; ++i) {
		for (int j = 0; j <= i; ++j) {
			world[i].push_back(arr[p++]);
		}
	}
	rest = vector<int>(arr + p, arr + 52);
}

void draw(int p, int r, int c) {
	move(r, c);
	int cp = COLOR_PAIR((p & 3) + 1);
	for (const char* s = text[p >> 2]; *s; ++s) {
		addch(*s | cp);
	}
}

void show(int st, int pr) {
	if (st == -1) {
		return;
	} else if (st < 7) {
		mvaddch(2, 10 + (st << 2), 'V' | COLOR_PAIR(pr));
	} else if (st < 11) {
		mvaddch(2, (st << 2) - 6, '^' | COLOR_PAIR(pr));
	} else {
		mvaddch(2, 5, '^' | COLOR_PAIR(pr));
	}
}

void rend() {
	for (int i = 0; i < 24; ++i) {
		mvhline(i, 0, ' ' | COLOR_PAIR(10), 80);
	}
	for (int i = 0; i < 7; ++i) {
		int p = 4;
		int col = 10 + (i << 2);
		for (int j = 0; j < world[i].size(); ++j) {
			if (j < hide[i]) {
				mvaddch(p++, col, '?' | COLOR_PAIR(10));
			} else {
				draw(world[i][j], p++, col);
			}
		}
	}
	for (int i = 0; i < 4; ++i) {
		if (done[i]) {
			draw(((done[i] - 1) << 2) | i, 1, (i << 2) + 22);
		}
	}
	if (rest.size()) {
		if (restptr + 1 < rest.size()) {
			mvaddch(1, 1, '?' | COLOR_PAIR(10));
		}
		if (restptr >= 0) {
			draw(rest[restptr], 1, 5);
		}
	}
	show(cho, 11);
	show(cur, 10);
	refresh();
}

void trymove(int from, int to) {
	if (from == to) {
		return;
	}
	if (to > 6) {
		return;
	}
	if (from < 7 && world[from].size() == 0) {
		return;
	}
	if (world[to].size() == 0) {
		if (from < 7 && world[from].size() && (world[from][hide[from]] >> 2) == 12) {
			world[to].insert(world[to].end(), world[from].begin() + hide[from], world[from].end());
			world[from].erase(world[from].begin() + hide[from], world[from].end());
			if (hide[from]) {
				--hide[from];
			}
		} else if (from == 11 && rest.size() && restptr >= 0 && (rest[restptr] >> 2) == 12) {
			world[to].push_back(rest[restptr]);
			rest.erase(rest.begin() + restptr--);
		} else if (done[from - 7] == 13) {
			world[to].push_back((from - 7) | (12 << 2));
		}
	} else {
		int t = world[to].back();
		int n = t >> 2;
		int c = (t & 3) >> 1;
		if (from < 7) {
			int ptr = world[from].size() - 1;
			while (ptr >= hide[from]) {
				int tt = world[from][ptr];
				if ((tt >> 2) == n - 1 && ((tt & 3) >> 1) ^ c == 1) {
					world[to].insert(world[to].end(), world[from].begin() + ptr, world[from].end());
					world[from].erase(world[from].begin() + ptr, world[from].end());
					if (world[from].size() == hide[from] && world[from].size()) {
						--hide[from];
					}
					break;
				} else {
					--ptr;
				}
			}
		} else {
			int k;
			if (from < 11) {
				k = ((done[from - 7] - 1) << 2) | (from - 7);
			} else if (restptr >= 0) {
				k = rest[restptr];
			} else {
				return;
			}
			if ((k >> 2) == n - 1 && ((k & 3) >> 1) ^ c == 1) {
				world[to].push_back(k);
				if (from < 11) {
					--done[from - 7];
				} else {
					rest.erase(rest.begin() + restptr--);
				}
			}
		}
	}
}

int main() {
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, 1);
	init_pair(10, COLOR_BLACK, COLOR_WHITE);
	init_pair(11, COLOR_WHITE, COLOR_BLACK);
	for (int i = 0; i < 4; ++i) {
		init_pair(i + 1, color[i], COLOR_WHITE);
	}
	srand(time(0));
	gen();
	bool out = false;
	while (!out) {
		rend();
		switch(getch()) {
		case 'q':
			out = true;
			break;
		case 'n':
			if (++restptr == rest.size()) {
				restptr = -1;
			}
			break;
		case 'u':
			if (cur < 7) {
				if (world[cur].size()) {
					int k = world[cur].back();
					if (done[k & 3] == (k >> 2)) {
						++done[k & 3];
						world[cur].pop_back();
						if (world[cur].size() <= hide[cur] && hide[cur]) {
							--hide[cur];
						}
					}
				}
			} else if (cur == 11) {
				if (rest.size() && restptr >= 0) {
					int k = rest[restptr];
					if (done[k & 3] == (k >> 2)) {
						++done[k & 3];
						rest.erase(rest.begin() + restptr--);
					}
				}
			}
			break;
		case KEY_UP:
			cho = cur;
			break;
		case KEY_DOWN:
			trymove(cho, cur);
			cho = -1;
			break;
		case KEY_LEFT:
			--cur;
			if (cur < 0) {
				cur = 11;
			}
			break;
		case KEY_RIGHT:
			cur = (cur + 1) % 12;
			break;
		}
	}
	endwin();
}

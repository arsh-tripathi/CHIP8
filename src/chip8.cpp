#include "include/chip8.h"

using namespace std;

vector<vector<Uint8>> Chip8::SPRITES = {
	{0xF0, 0x90, 0x90, 0x90, 0xF0}, {0x20, 0x60, 0x20, 0x20, 0x70}, {0xF0, 0x10, 0xF0, 0x80, 0xF0}, {0xF0, 0x10, 0xF0, 0x10, 0xF0},
	{0x90, 0x90, 0xF0, 0x10, 0x10}, {0xF0, 0x80, 0xF0, 0x10, 0xF0}, {0xF0, 0x80, 0xF0, 0x90, 0xF0}, {0xF0, 0x10, 0x20, 0x40, 0x40},
	{0xF0, 0x90, 0xF0, 0x90, 0xF0}, {0xF0, 0x90, 0xF0, 0x10, 0xF0}, {0xF0, 0x90, 0xF0, 0x90, 0x90}, {0xE0, 0x90, 0xE0, 0x90, 0xE0},
	{0xF0, 0x80, 0x80, 0x80, 0xF0}, {0xE0, 0x90, 0x90, 0x90, 0xE0}, {0xF0, 0x80, 0xF0, 0x80, 0xF0}, {0xF0, 0x80, 0xF0, 0x80, 0x80}};

Chip8::Chip8(Uint8 scale) : PC{0x200}, I{0}, DT{0}, ST{0}, d{scale}, scale{scale} {
	for (int i = 0; i < SPRITES.size(); ++i) {
		for (int j = 0; j < 5; ++j) {
			memory[5 * i + j] = SPRITES[i][j];
		}
	}
	SP = STACK;
	*SP = PC;
}

void Chip8::tick() {
	if (DT > 0) DT--;
	if (ST > 0) {
		ST--;
		d.buzz();
	}
}

void Chip8::convertCommad(Uint16 cmd) {
	Uint16 x;
	Uint16 y;
	Uint16 kk;
	Uint16 nnn;
	Uint16 n;
	Uint16 sum;
	switch (cmd >> 12) {
		case 0x0:
			if (cmd == 0x00E0)
				d.clearDisplay();
			else if (cmd == 0x00EE) {
				PC = *SP;
				SP--;
				return;
			} else
				cerr << "Invalid command passed" << endl;
			break;
		case 0x1:
			cmd = cmd & 0x0fff;
			PC = cmd;
			return;
		case 0x2:
			cmd = cmd & 0x0fff;
			SP++;
			*SP = PC;
			PC = cmd;
			return;
		case 0x3:
			x = (cmd & 0x0f00) >> 8;
			kk = (cmd & 0x00ff);
			if (V[x] == kk) PC += 2;
			break;
		case 0x4:
			x = (cmd & 0x0f00) >> 8;
			kk = (cmd & 0x00ff);
			if (V[x] != kk) PC += 2;
			break;
		case 0x5:
			x = (cmd & 0x0f00) >> 8;
			y = (cmd & 0x00f0) >> 4;
			if (V[x] == V[y]) PC += 2;
			break;
		case 0x6:
			x = (cmd & 0x0f00) >> 8;
			kk = (cmd & 0x00ff);
			V[x] = kk;
			break;
		case 0x7:
			x = (cmd & 0x0f00) >> 8;
			kk = (cmd & 0x00ff);
			V[x] += kk;
			break;
		case 0x8:
			x = (cmd & 0x0f00) >> 8;
			y = (cmd & 0x00f0) >> 4;
			switch (cmd & 0x000f) {
				case 0x0:
					if (V[x] == V[y]) PC += 2;
					break;
				case 0x1:
					V[x] = V[x] | V[y];
					break;
				case 0x2:
					V[x] = V[x] & V[y];
					break;
				case 0x3:
					V[x] = V[x] ^ V[y];
					break;
				case 0x4:
					sum = V[x] + V[y];
					V[x] = sum & 0x00ff;
					V[0xf] = (sum & 0x0f00) >> 8;
					break;
				case 0x5:
					if (V[x] > V[y]) {
						V[0xf] = 1;
						V[x] -= V[y];
					} else {
						V[0xf] = 0;
						V[x] = V[y] - V[x];
					}
					break;
				case 0x6:
					V[0xf] = V[x] & 0x000f;
					V[x] = V[x] >> 1;
					break;
				case 0x7:
					if (V[x] > V[y]) {
						V[0xf] = 0;
						V[x] -= V[y];
					} else {
						V[0xf] = 1;
						V[x] = V[y] - V[x];
					}
				case 0xE:
					V[0xf] = (V[x] & 0xf000) >> 12;
					V[x] = V[x] << 2;
					break;
				default:
					cerr << "Invalid command passed" << endl;
					break;
			}
			break;
		case 0x9:
			if ((cmd & 0x000f) == 0x0) {
				if (V[x] != V[y]) PC += 2;
			} else
				cerr << "Invalid command passed" << endl;
			break;
		case 0xA:
			nnn = cmd & 0x0fff;
			I = nnn;
			break;
		case 0xB:
			nnn = cmd & 0x0fff;
			PC = nnn + V[0];
			return;
		case 0xC: {
			x = (cmd & 0x0f00) >> 8;
			kk = (cmd & 0x00ff);
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> distrib(0, 0xffff);
			V[x] = distrib(gen);
			V[x] = V[x] & kk;
		} break;
		case 0xD:
			x = (cmd & 0x0f00) >> 8;
			y = (cmd & 0x00f0) >> 4;
			n = cmd & 0x000f;
			// draw the sprite here somehow, need to add functionality
			break;
		case 0xE:
			x = (cmd & 0x0f00) >> 8;
			if ((cmd & 0x00ff) == 0x9E) {
				if (d.isKeyDown(V[x])) PC += 2;
			} else if ((cmd & 0x00ff) == 0xA1) {
				if (!d.isKeyDown(V[x])) PC += 2;
			} else
				cerr << "Invalid Command" << endl;
			break;
		case 0xF:
			x = (cmd & 0x0f00) >> 8;
			switch (cmd & 0x00ff) {
				case 0x07:
					V[x] = DT;
					break;
				case 0x0A:
					SDL_Event e;
					while (true) {
						SDL_PollEvent(&e);
						if (e.type == SDL_KEYDOWN) {
							V[x] = e.key.keysym.sym;
							break;
						}
					}
					break;
				case 0x15:
					DT = V[x];
					break;
				case 0x18:
					ST = V[x];
					break;
				case 0x1E:
					I += V[x];
					break;
				case 0x29:
					I = V[x] * 5;
					break;
				case 0x33:
					memory[I] = (V[x] / 100) % 10;
					memory[I + 1] = (V[x] / 10) % 10;
					memory[I + 2] = V[x] % 10;
					break;
				case 0x55:
					for (int i = 0; i < 0x10; ++i) memory[I + i] = V[i];
					break;
				case 065:
					for (int i = 0; i < 0x10; ++i) V[i] = memory[I + i];
					break;
				default:
					break;
			}
		default:
			break;
	}
}

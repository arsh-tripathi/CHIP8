#include "include/chip8.h"

using namespace std;

Uint32 timerCallback(Uint32 Interval, void *param) {
	Chip8 *c = static_cast<Chip8 *>(param);
	if (c) c->tick();
	return 19;
}

vector<vector<Uint8>> Chip8::SPRITES = {
	{0xF0, 0x90, 0x90, 0x90, 0xF0}, {0x20, 0x60, 0x20, 0x20, 0x70}, {0xF0, 0x10, 0xF0, 0x80, 0xF0}, {0xF0, 0x10, 0xF0, 0x10, 0xF0},
	{0x90, 0x90, 0xF0, 0x10, 0x10}, {0xF0, 0x80, 0xF0, 0x10, 0xF0}, {0xF0, 0x80, 0xF0, 0x90, 0xF0}, {0xF0, 0x10, 0x20, 0x40, 0x40},
	{0xF0, 0x90, 0xF0, 0x90, 0xF0}, {0xF0, 0x90, 0xF0, 0x10, 0xF0}, {0xF0, 0x90, 0xF0, 0x90, 0x90}, {0xE0, 0x90, 0xE0, 0x90, 0xE0},
	{0xF0, 0x80, 0x80, 0x80, 0xF0}, {0xE0, 0x90, 0x90, 0x90, 0xE0}, {0xF0, 0x80, 0xF0, 0x80, 0xF0}, {0xF0, 0x80, 0xF0, 0x80, 0x80}};

Chip8::Chip8(Uint8 scale, char *rom)
	: PC{0x200}, I{0}, DT{0}, ST{0}, reald{32, vector<bool>(64, false)}, d{scale}, timer{SDL_AddTimer(19, timerCallback, nullptr)}, scale{scale} {
	// SETUP REGISTERS TO DEFAULT VALUES
	for (int i = 0; i < 0x10; ++i) {
		V[i] = 0;
	}
	SP = STACK;

	// MEMORY SETUP
	// 1. Setting up sprites
	for (int i = 0; i < SPRITES.size(); ++i) {
		for (int j = 0; j < 5; ++j) {
			memory[5 * i + j] = SPRITES[i][j];
		}
	}
	// Loading rom into memory
	ifstream in{rom, ios::binary};
	if (!in) {
		cerr << "The file cannot be accessed" << endl;
		exit(1);
	}
	Uint8 value;
	int j = 0x200;
	in >> noskipws;
	while (in >> value) {
		memory[j++] = value;
	}
	for (int i = j; i < 4096; ++i) {
		memory[i] = 0;
	}
	// Resetting PC
	ofstream out{"memory"};
	for (int i = 0; i < 4096; ++i) {
		out << i << ": ";
		out << hex;
		out << +memory[i] << endl;
		out << dec;
	}
}

Chip8::~Chip8() { SDL_RemoveTimer(timer); }

void Chip8::run() {
	// executeInstruction(0x00E0); // clear display
	// executeInstruction(0x6200); // set the register 2 to the number 07
	// executeInstruction(0xF229); // set I to sprite for the number at the register at second place
	// executeInstruction(0x6000); // set register 0 to 0x10
	// executeInstruction(0x6100); // set register 1 to 0x10
	// executeInstruction(0xD015); // draw
	// executeInstruction(0x7003); // set register 0 to 0x10
	// executeInstruction(0xD015); // draw
	// ofstream out{"memory"};
	// for (int i = 0; i < 4096; ++i) {
	// 	out << i << ": ";
	// 	out << hex;
	// 	out << +memory[i] << endl;
	// 	out << dec;
	// }

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		SDL_PollEvent(&e);
		switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				d.updateKeyStatus(e.key.keysym.sym, true);
				break;
			case SDL_KEYUP:
				d.updateKeyStatus(e.key.keysym.sym, false);
				break;
			default:
				break;
		}
		Uint16 instruction = (memory[PC] << 8) | memory[PC + 1];
		// cerr << hex;
		// if (instruction != 0) cerr << instruction << endl;
		// cerr << dec;
		executeInstruction(instruction);
	}
	// for (int i = 0; i < 4096; ++i) {
	// 		out << i << ": ";
	// 		out << hex;
	// 		out << +memory[i] << endl;
	// 		out << dec;
	// }
}

void Chip8::tick() {
	if (DT > 0) DT--;
	if (ST > 0) {
		ST--;
		d.buzz();
	}
}

void Chip8::executeInstruction(Uint16 cmd) {
	cerr << "[CMD] ";
	cerr << hex;
	cerr << cmd;
	cerr << dec;
	cerr << ": ";
	Uint16 x;
	Uint16 y;
	Uint16 kk;
	Uint16 nnn;
	Uint16 n;
	Uint16 sum;
	switch (cmd >> 12) {
		case 0x0:
			if (cmd == 0x00E0) {
				d.clearDisplay();
				cerr << "Clearing display" << endl;
			} else if (cmd == 0x00EE) {
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
			// cerr << "Set register "<< x <<" to " << kk << endl;
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
					V[0xf] = V[x] & 0b1;
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
					V[0xf] = (V[x] & 0b10000000) >> 8;
					V[x] = V[x] << 1;
					break;
				default:
					cerr << "Invalid command passed" << endl;
					break;
			}
			break;
		case 0x9:
			x = (cmd & 0x0f00) >> 8;
			y = (cmd & 0x00f0) >> 4;
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
		case 0xD: {
			x = (cmd & 0x0f00) >> 8;
			y = (cmd & 0x00f0) >> 4;
			n = cmd & 0x000f;
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < 8; ++j) {
					int X = (V[x] + j) % 64;
					int Y = (V[y] + i) % 32;
					bool newpixel = (memory[I + i] >> (7 - j)) & 0b1;
					V[0xf] = reald[Y][X] & newpixel ? 1 : V[0xf];
					reald[Y][X] = reald[Y][X] ^ newpixel;
					// cerr << "set " << X << ", " << Y << "to " << reald[Y][X] << endl;
				}
			}
			// cerr << endl;
			// for (int i = 0; i < reald.size(); ++i) {
			// 	for (int j = 0; j < reald[i].size(); ++j) {
			// 		cerr << reald[i][j];
			// 	}
			// 	cerr << endl;
			// }
			cerr << "Drew the sprite of size " << n << " at " << I << " to position " << +V[x] << ", " << +V[y] << endl;
			d.updateDisplay(reald);
		} break;
		case 0xE:
			x = (cmd & 0x0f00) >> 8;
			if ((cmd & 0x00ff) == 0x9E) {
				if (d.isKeyDown(V[x])) PC += 2;
			} else if ((cmd & 0x00ff) == 0xA1) {
				if (!d.isKeyDown(V[x])) PC += 2;
			} else
				cerr << "Invalid command passed" << endl;
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
					cerr << "Set I to " << I << endl;
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
					cerr << "Invalid command passed" << endl;
					break;
			}
		default:
			cerr << "Invalid command passed" << endl;
			break;
	}
	PC += 2;
	cerr << "V[0-F]: ";
	cerr << hex;
	cerr << +V[0] << " " << +V[1] << " " << +V[2] << " " << +V[3] << " " << +V[4] << " " << +V[5] << " " << +V[6] << " " << +V[7] << " " << +V[8]
		 << " " << +V[9] << " " << +V[0xA] << " " << +V[0xB] << " " << +V[0xC] << " " << +V[0xD] << " " << +V[0xE] << " " << +V[0xF] << endl;
	cerr << "PC: " << PC << " ";
	cerr << "I: " << I << " ";
	cerr << "DT: " << DT << " ";
	cerr << "ST: " << ST << endl;
}

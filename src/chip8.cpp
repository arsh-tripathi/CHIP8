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
	: PC{0x200},
	  SP{-1},
	  I{0},
	  DT{0},
	  ST{0},
	  reald{32, vector<bool>(64, false)},
	  d{scale},
	  timer{0},
	  debug{"commands"},
	  scale{scale} {
	// SETUP REGISTERS TO DEFAULT VALUES
	for (int i = 0; i < 0x10; ++i) {
		V[i] = 0;
	}

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

Chip8::~Chip8() { 
	debug.close();
	SDL_RemoveTimer(timer);
	timer = 0;
}

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
	timer = SDL_AddTimer(17, timerCallback, this);
	while (!quit) {
		SDL_PollEvent(&e);
		switch (e.type) {
			case SDL_QUIT:
				quit = true;
				continue;
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
		// debug << hex;
		// if (instruction != 0) debug << instruction << endl;
		// debug << dec;
		executeInstruction(instruction);
		SDL_Delay(2);
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
	debug << "[CMD] ";
	debug << hex;
	debug << cmd;
	debug << dec;
	debug << ": ";
	Uint16 x;
	Uint16 y;
	Uint16 kk;
	Uint16 nnn;
	Uint16 n;
	Uint16 sum;
	switch (cmd >> 12) {
		case 0x0:
			if (cmd == 0x00E0) {
				for (int i = 0; i < reald.size(); ++i) for (int j = 0; j < reald[i].size(); ++j) reald[i][j] = false;
				d.clearDisplay();
				debug << "Clearing display" << endl;
			} else if (cmd == 0x00EE) {
				PC = STACK[SP];
				SP--;
				return;
			} else
				debug << "Invalid command passed" << endl;
			break;
		case 0x1:
			nnn = cmd & 0x0fff;
			PC = nnn;
			debug << "Set PC to " << nnn << endl;
			return;
		case 0x2:
			nnn = cmd & 0x0fff;
			SP++;
			STACK[SP] = PC + 2;
			PC = nnn;
			debug << hex;
			debug << "Entered sub process, PC set to " << +PC << " return address " << STACK[SP] << endl;
			debug << dec;
			return;
		case 0x3:
			x = (cmd & 0x0f00) >> 8;
			kk = (cmd & 0x00ff);
			if (V[x] == kk) PC += 2;
			if (V[x] == kk) debug << "skipping next instruction" << PC << endl;
			break;
		case 0x4:
			x = (cmd & 0x0f00) >> 8;
			kk = (cmd & 0x00ff);
			if (V[x] != kk) PC += 2;
			if (V[x] != kk) debug << "skipping next instruction" << PC << endl;
			break;
		case 0x5:
			x = (cmd & 0x0f00) >> 8;
			y = (cmd & 0x00f0) >> 4;
			if (V[x] == V[y]) PC += 2;
			if (V[x] == V[y]) debug << "skipping next instruction" << PC << endl;
			break;
		case 0x6:
			x = (cmd & 0x0f00) >> 8;
			kk = (cmd & 0x00ff);
			V[x] = kk;
			debug << "Set register "<< x <<" to " << kk << endl;
			break;
		case 0x7:
			x = (cmd & 0x0f00) >> 8;
			kk = (cmd & 0x00ff);
			V[x] += kk;
			debug << "Increase register "<< x <<" by " << kk << endl;
			break;
		case 0x8:
			x = (cmd & 0x0f00) >> 8;
			y = (cmd & 0x00f0) >> 4;
			switch (cmd & 0x000f) {
				case 0x0:
					V[x] = V[y];
					debug << "set register " << x << " to value " << y << endl;
					break;
				case 0x1:
					V[x] = V[x] | V[y];
					V[0xf] = 0;
					debug << "set register " << x << " to bitwise or with " << y << endl;
					break;
				case 0x2:
					V[x] = V[x] & V[y];
					V[0xf] = 0;
					debug << "set register " << x << " to bitwise and with " << y << endl;		
					break;
				case 0x3:
					V[x] = V[x] ^ V[y];
					V[0xf] = 0;
					debug << "set register " << x << " to bitwise xor with " << y << endl;		
					break;
				case 0x4:
					sum = V[x] + V[y];
					V[x] = sum & 0x00ff;
					V[0xf] = (sum & 0x0f00) >> 8;
					debug << "set register " << x << " to sum with " << y << endl;	
					break;
				case 0x5:
					n = (V[x] >= V[y])? 1 : 0;
					V[x] -= V[y];
					V[0xf] = n;
					debug << "set register " << x << " to subtraction by " << y << endl;		
					break;
				case 0x6:
					V[x] = V[y];
					n = V[x] & 0b1;
					V[x] = V[x] >> 1;
					V[0xf] = n;
					debug << "set register " << x << " to bit shift right "<< endl;		
					break;
				case 0x7:
					n = (V[y] >= V[x]) ? 1 : 0;
					V[x] = V[y] - V[x];
					V[0xf] = n;
					debug << "set register " << x << " to subtracted from " << y << endl;		
					break;
				case 0xE:
					V[x] = V[y];
					n = (V[x] & 0b10000000) >> 7;
					V[x] = V[x] << 1;
					V[0xf] = n;
					break;
				default:
					debug << "Invalid command passed" << endl;
					break;
			}
			break;
		case 0x9:
			x = (cmd & 0x0f00) >> 8;
			y = (cmd & 0x00f0) >> 4;
			if ((cmd & 0x000f) == 0x0) {
				if (V[x] != V[y]) PC += 2;
			} else
				debug << "Invalid command passed" << endl;
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
					Uint8 X = (V[x] % 64) + j;
					Uint8 Y = (V[y] % 32) + i;
					if (X > 63 || Y > 31) continue;
					bool newpixel = (memory[I + i] >> (7 - j)) & 0b1;
					V[0xf] = reald[Y][X] & newpixel ? 1 : V[0xf];
					reald[Y][X] = reald[Y][X] ^ newpixel;
					// debug << "set " << X << ", " << Y << "to " << reald[Y][X] << endl;
				}
			}
			// debug << endl;
			// for (int i = 0; i < reald.size(); ++i) {
			// 	for (int j = 0; j < reald[i].size(); ++j) {
			// 		debug << reald[i][j];
			// 	}
			// 	debug << endl;
			// }
			debug << "Drew the sprite of size " << n << " at " << I << " to position " << +V[x] << ", " << +V[y] << endl;
			d.updateDisplay(reald);
		} break;
		case 0xE:
			x = (cmd & 0x0f00) >> 8;
			if ((cmd & 0x00ff) == 0x9E) {
				if (d.isKeyDown(V[x])) PC += 2;
			} else if ((cmd & 0x00ff) == 0xA1) {
				if (!d.isKeyDown(V[x])) PC += 2;
			} else
				debug << "Invalid command passed" << endl;
			break;
		case 0xF:
			x = (cmd & 0x0f00) >> 8;
			switch (cmd & 0x00ff) {
				case 0x07:
					V[x] = DT;
					break;
				case 0x0A:{
					SDL_Event e;
					std::map<SDL_Keycode, Uint16> m {
						{SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC}, 
						{SDLK_q, 0x4}, {SDLK_w, 0x5}, {SDLK_e, 0x6}, {SDLK_r, 0xD},
						{SDLK_a, 0x7}, {SDLK_s, 0x8}, {SDLK_d, 0x9}, {SDLK_f, 0xE},
						{SDLK_z, 0xA}, {SDLK_x, 0x0}, {SDLK_c, 0xB}, {SDLK_v, 0xF}
					};
					while (true) {
						SDL_WaitEvent(&e);
						if (e.type == SDL_KEYDOWN) {
							V[x] = m[e.key.keysym.sym];
						}
						else if (e.type = SDL_KEYUP) {
							if (m[e.key.keysym.sym] == V[x]) break;
						}
					}}
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
					debug << "Set I to " << I << endl;
					break;
				case 0x33:
					memory[I] = (V[x] / 100) % 10;
					memory[I + 1] = (V[x] / 10) % 10;
					memory[I + 2] = V[x] % 10;
					break;
				case 0x55:
					for (int i = 0; i <= x; ++i) memory[I + i] = V[i];
					I += x + 1;
					break;
				case 0x65:
					for (int i = 0; i <= x; ++i) V[i] = memory[I + i];
					I += x + 1;
					break;
				default:
					debug << "Invalid command passed" << endl;
					break;
			}
			break;
		default:
			debug << "Invalid command passed" << endl;
			break;
	}
	PC += 2;
	debug << "V[0-F]: ";
	debug << hex;
	debug << +V[0] << " " << +V[1] << " " << +V[2] << " " << +V[3] << " " << +V[4] << " " << +V[5] << " " << +V[6] << " " << +V[7] << " " << +V[8]
		 << " " << +V[9] << " " << +V[0xA] << " " << +V[0xB] << " " << +V[0xC] << " " << +V[0xD] << " " << +V[0xE] << " " << +V[0xF] << endl;
	debug << "PC: " << PC << " I: " << I << " SP: " << SP;
	if (SP >= 0) debug << " SUB: " << STACK[SP];
	debug << " DT: " << DT << " ST: " << ST << endl;
}

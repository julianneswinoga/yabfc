#include "assembly.h"

//{0x31, 0xFF, 0xBF, 0x2A, 0x00, 0x00, 0x00, 0xB8, 0x3C, 0x00, 0x00, 0x00, 0x0F, 0x05}

void construct_arbitrary(CODE *code, uint8_t *machCode, uint16_t size) {
	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + size * sizeof(uint8_t)));
	for (int i = code->size; i < code->size + size; i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += size;
}

// 0:  83 ec 04                sub    esp,0x8
// 3:  b8 01 00 00 00          mov    eax,0x1
// 8:  67 01 04 24             add    DWORD PTR [esp],eax
void construct_INC(CODE *code) {
	uint8_t machCode[] = {0x89, 0x04, 0x25, 0xA9, 0x01, 0x00, 0x04};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

// DEC [esp] (FF-1 r/m16/32/64)
void construct_DEC() {
}

// ADD 4, esp (01-r r/m16/32/64 r16/32/64)
void construct_ADDESP() {
}

// SUB 4, esp (29-4 r/m16/32/64 r16/32/64)
void construct_SUBESP() {
}

void construct_LPSTART() {
}

void construct_LPEND() {
}

void construct_INPUT() {
}

void construct_PRINT(CODE *code) {
	uint8_t machCode[] = {0x67, 0x8B, 0x14, 0x24, 0xB8, 0x02, 0x00, 0x00, 0x00, 0xCD, 0x21}; //{0x67, 0x8A, 0x14, 0x24, 0xB4, 0x02, 0xCD, 0x21};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

void construct_END(CODE *code) {
	uint8_t machCode[] = {0x31, 0xFF, 0xBF, 0x2A, 0x00, 0x00, 0x00, 0xB8, 0x3C, 0x00, 0x00, 0x00, 0x0F, 0x05};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

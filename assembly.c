#include "assembly.h"

/*

mov ds, 0xFFFF

#mov rax, [rsp]
#inc    rax
#mov [rsp], rax

#mov    rdx, [rsp]
#mov    rax, 2
#int    33

 */

void construct_arbitrary(CODE *code, uint8_t *machCode, uint16_t size) {
	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + size * sizeof(uint8_t)));
	for (int i = code->size; i < code->size + size; i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += size;
}

void construct_INC(CODE *code) {
	uint8_t machCode[] = {0x48, 0x8B, 0x04, 0x24, 0x48, 0xFF, 0xC0, 0x48, 0x89, 0x04, 0x24};

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

/*
mov rax, 4
mov rbx, 1
mov rcx, rsp
mov rdx, 1
int 0x80
 */
void construct_PRINT(CODE *code) {
	uint8_t machCode[] = {0x48, 0xC7, 0xC0, 0x04, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC3, 0x01, 0x00, 0x00, 0x00, 0x48, 0x89, 0xE1, 0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0xCD, 0x80};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

/*
mov eax, 1
mov ebx, [rsp]
int 0x80
 */
void construct_END(CODE *code) {
	uint8_t machCode[] = {0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x1C, 0x24, 0xCD, 0x80};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

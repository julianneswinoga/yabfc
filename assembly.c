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

/*
mov rax, [rsp]
inc rax
mov [rsp], rax
 */
void construct_INC(CODE *code) {
	uint8_t machCode[] = {0x48, 0x8B, 0x04, 0x24, 0x48, 0xFF, 0xC0, 0x48, 0x89, 0x04, 0x24};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

// DEC [esp] (FF-1 r/m16/32/64)
/*
mov rax, [rsp]
dec rax
mov [rsp], rax
 */
void construct_DEC() {
	uint8_t machCode[] = {0x48, 0x8B, 0x04, 0x24, 0x48, 0xFF, 0xC8, 0x48, 0x89, 0x04, 0x24};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
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
mov edx, 1 # num bytes to output
mov ecx, rsp # Address of the buffer
mov ebx, 1 # stdout
mov eax, 4 # __NR_write
int 0x80
 */
void construct_PRINT(CODE *code) {
	uint8_t machCode[] = {0xBA, 0x01, 0x00, 0x00, 0x00, 0x89, 0xE1, 0xBB, 0x01, 0x00, 0x00, 0x00, 0xB8, 0x04, 0x00, 0x00, 0x00, 0xCD, 0x80};

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

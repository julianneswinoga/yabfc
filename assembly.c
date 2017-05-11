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
void construct_DEC(CODE *code) {
	uint8_t machCode[] = {0x48, 0x8B, 0x04, 0x24, 0x48, 0xFF, 0xC0, 0x48, 0x89, 0x04, 0x24};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

// ADD 4, esp (01-r r/m16/32/64 r16/32/64)
/*
add rsp, 4
 */
void construct_ADDESP(CODE *code) {
	uint8_t machCode[] = {0x48, 0x83, 0xC4, 0x04};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

// SUB 4, esp (29-4 r/m16/32/64 r16/32/64)
/*
sub, rsp, 4
 */
void construct_SUBESP(CODE *code) {
	uint8_t machCode[] = {0x48, 0x83, 0xEC, 0x04};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

void construct_LPSTART(CODE *code) {
}

void construct_LPEND(CODE *code) {
}

/*
mov rdx, 1 # 1 char
mov rsi, rsp # char *buf
mov rdi, 0 # fd = stdin
mov rax, 0 # sys_read
syscall
 */
void construct_INPUT(CODE *code) {
	uint8_t machCode[] = {0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x48, 0x89, 0xE6, 0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x05};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

/*
mov rdx, 1 # 1 char
mov rsi, rsp # char *buf
mov rdi, 1 # fd = stdout
mov rax, 1 # sys_write
syscall
 */
void construct_PRINT(CODE *code) {
	uint8_t machCode[] = {0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x48, 0x89, 0xE6, 0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x05};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

/*
mov rax, 1
mov rbx, [rsp]
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

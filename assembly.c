#include "assembly.h"

/**
 * Construct arbitrary machine code
 * @param code     A pointer to the current CODE struct
 * @param machCode An array of machine code bytes to add
 * @param size     The size of the machine code array being added
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
add rax, 0x00000000
mov [rsp], rax
 */
void construct_ADD(CODE *code, uint32_t add) {
	uint8_t machCode[] = {0x48, 0x8B, 0x04, 0x24, 0x48, 0x05, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0x04, 0x24};
	memcpy(&machCode[6], &add, sizeof(add));

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

/*
mov rax, [rsp]
sub rax, 0x00000000
mov [rsp], rax
 */
void construct_SUB(CODE *code, uint32_t sub) {
	uint8_t machCode[] = {0x48, 0x8B, 0x04, 0x24, 0x48, 0x2D, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0x04, 0x24};
	memcpy(&machCode[6], &sub, sizeof(sub));

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

/*
add rsp, 0x00000000
 */
void construct_ADDESP(CODE *code, uint32_t num) {
	uint8_t machCode[] = {0x48, 0x81, 0xC4, 0x00, 0x00, 0x00, 0x00};
	memcpy(&machCode[3], &num, sizeof(num));

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

/*
sub, rsp, 0x00000000
 */
void construct_SUBESP(CODE *code, uint32_t num) {
	uint8_t machCode[] = {0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00};
	memcpy(&machCode[3], &num, sizeof(num));

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

/*
Jump if zero to past loop end

mov rax, [rsp]
cmp rax, 0
je 0x0
 */
void construct_LPSTART(CODE *code) {
	uint8_t machCode[] = {0x48, 0x8B, 0x04, 0x24, 0x48, 0x83, 0xF8, 0x00, 0x0F, 0x84, 0x00, 0x00, 0x00, 0x00};

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

/*
Jump if not zero to past loop start
Backwards search to find the previous loop start,
then set the loop start to be the difference between loop start and end, (Forward jump)
Set the end loop offset to be the negative of that, 2s complement (Backward jump)

next_instr = current + imm + sizeof(jump machine code)

mov rax, [rsp]
cmp rax, 0
jne 0x0
 */
void construct_LPEND(CODE *code) {
	uint8_t machCode[]          = {0x48, 0x8B, 0x04, 0x24, 0x48, 0x83, 0xF8, 0x00, 0x0F, 0x85, 0x00, 0x00, 0x00, 0x00};
	uint8_t bracketSearchCode[] = {0x0F, 0x84, 0x00, 0x00, 0x00, 0x00};
	int     i, jumpDistance, jumpBackwardCode, jumpForwardCode;

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));

	for (i = code->size - sizeof(machCode); i > 0; i--) {
		if (memcmp(&code->bytes[i], bracketSearchCode, sizeof(bracketSearchCode) * sizeof(uint8_t)) == 0) {
			jumpDistance = i - (code->size - sizeof(machCode));
			break;
		}
	}
	if (i == 0) {
		fprintf(stderr, "Something went very wrong when searching for a previous bracket\n");
		exit(1);
	}

	jumpBackwardCode = jumpDistance - sizeof(bracketSearchCode) - 2;
	jumpForwardCode  = -jumpDistance + sizeof(bracketSearchCode) + 2;
	debugPrintf(4, "Writing jump of %i backward and %i forward\n", jumpBackwardCode, jumpForwardCode);
	memcpy(&code->bytes[code->size - 4], &jumpBackwardCode, 4); // Write the jump distance to the last four bytes of machine code (End loop)
	memcpy(&code->bytes[i + 2], &jumpForwardCode, 4);           // Write forward jump distance (Open loop)
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

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
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

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

/*
xor rbp, rbp
mov r9, rdx
pop rsi
mov rdx, rsp
and rsp, 0xfffffffffffffff0
sub rsp, 8
 */
void construct_START(CODE *code) {
	uint8_t machCode[] = {0x48, 0x31, 0xED, 0x49, 0x89, 0xD1, 0x5E, 0x48, 0x89, 0xE2, 0x48, 0x83, 0xE4, 0xF0, 0x48, 0x83, 0xEC, 0x08};

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

/*
mov rax, 0x3C ; sys_exit
xor rdi, rdi
syscall
*/
void construct_END(CODE *code) {
	uint8_t machCode[] = {0x48, 0xC7, 0xC0, 0x3C, 0x0, 0x0, 0x0, \
		0x48, 0x31, 0xf6, \
		0x0f, 0x05};

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

/*
mov rax, 0
mov [rsp], rax
 */
void construct_CLEAR(CODE *code) {
	uint8_t machCode[] = {0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0x04, 0x24};

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

/*
mov rbx, 0
mov rcx, 0
add rcx, rsp

mov rax, [rsp]
imul rbx
add [rcx], rax
 */
void construct_MULTIPLY(CODE *code, int a, int offset) {
	uint8_t machCode[] = {0x48, 0xC7, 0xC3, 0x00, 0x00, 0x00, 0x00,
	                      0x48, 0xC7, 0xC1, 0x00, 0x00, 0x00, 0x00,
	                      0x48, 0x01, 0xE1, 0x48, 0x8B, 0x04, 0x24, 0x48, 0xF7, 0xEB, 0x48, 0x01, 0x01};
	memcpy(&machCode[3], &a, 4);
	offset *= -8;
	memcpy(&machCode[10], &offset, 4);

	construct_arbitrary(code, (uint8_t *)&machCode, sizeof(machCode));
}

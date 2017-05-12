#include "assembly.h"

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

/*
mov rax, [rsp]
dec rax
mov [rsp], rax
 */
void construct_DEC(CODE *code) {
	uint8_t machCode[] = {0x48, 0x8B, 0x04, 0x24, 0x48, 0xFF, 0xC8, 0x48, 0x89, 0x04, 0x24};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

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

/*
Jump if zero to past loop end

mov eax, [rsp]
cmp eax, 0
je 0x0
 */
void construct_LPSTART(CODE *code) {
	uint8_t machCode[] = {0x8B, 0x04, 0x24, 0x83, 0xF8, 0x00, 0x0F, 0x84, 0x00, 0x00, 0x00, 0x00};

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (int i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);
}

/*
Jump if not zero to past loop start
Backwards search to find the previous loop start,
then set the loop start to be the difference between loop start and end, (Forward jump)
Set the end loop offset to be the negative of that, 2s complement (Backward jump)

next_instr = current + imm + sizeof(jump machine code)

mov eax, [rsp]
cmp eax, 0
jne 0x0
 */
void construct_LPEND(CODE *code) {
	uint8_t machCode[]          = {0x8B, 0x04, 0x24, 0x83, 0xF8, 0x00, 0x0F, 0x85, 0x00, 0x00, 0x00, 0x00};
	uint8_t bracketSearchCode[] = {0x0F, 0x84, 0x00, 0x00, 0x00, 0x00};
	int     i, jumpDistance, jumpBackwardCode, jumpForwardCode;

	code->bytes = (uint8_t *)realloc(code->bytes, (code->size + sizeof(machCode)) * sizeof(uint8_t));
	for (i = code->size; i < code->size + sizeof(machCode); i++) {
		code->bytes[i] = machCode[i - code->size];
	}
	code->size += sizeof(machCode);

	for (i = code->size - sizeof(machCode); i > 0; i--) {
		if (memcmp(&code->bytes[i], bracketSearchCode, sizeof(bracketSearchCode) * sizeof(uint8_t)) == 0) {
			jumpDistance = i - (code->size - sizeof(machCode));
			debugPrintf("Found matching bracket %i opcodes before\n", jumpDistance);
			break;
		}
	}
	if (i == 0) {
		fprintf(stderr, "Something went very wrong when searching for a previous bracket\n");
		exit(1);
	}

	jumpBackwardCode = jumpDistance - sizeof(bracketSearchCode);
	jumpForwardCode  = -jumpDistance + sizeof(bracketSearchCode);
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

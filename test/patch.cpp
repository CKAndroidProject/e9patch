/*
 * Patching.
 */

#include "stdlib.c"

/*
 * Representation of the state structure.
 */
struct STATE
{
    union
    {
        uint16_t rflags;
        uint64_t __padding;
    };
    union
    {
        int64_t r15;
        int32_t r15d;
        int16_t r15w;
        int8_t r15b;
    };
    union
    {
        int64_t r14;
        int32_t r14d;
        int16_t r14w;
        int8_t r14b;
    };
    union
    {
        int64_t r13;
        int32_t r13d;
        int16_t r13w;
        int8_t r13b;
    };
    union
    {
        int64_t r12;
        int32_t r12d;
        int16_t r12w;
        int8_t r12b;
    };
    union
    {
        int64_t r11;
        int32_t r11d;
        int16_t r11w;
        int8_t r11b;
    };
    union
    {
        int64_t r10;
        int32_t r10d;
        int16_t r10w;
        int8_t r10b;
    };
    union
    {
        int64_t r9;
        int32_t r9d;
        int16_t r9w;
        int8_t r9b;
    };
    union
    {
        int64_t r8;
        int32_t r8d;
        int16_t r8w;
        int8_t r8b;
    };
    union
    {
        int64_t rdi;
        int32_t edi;
        int16_t di;
        int8_t dil;
    };
    union
    {
        int64_t rsi;
        int32_t esi;
        int16_t si;
        int8_t sil;
    };
    union
    {
        int64_t rbp;
        int32_t ebp;
        int16_t bp;
        int8_t bpl;
    };
    union
    {
        int64_t rbx;
        int32_t ebx;
        int16_t bx;
        struct
        {
            int8_t bl;
            int8_t bh;
        };
    };
    union
    {
        int64_t rdx;
        int32_t edx;
        int16_t dx;
        struct
        {
            int8_t dl;
            int8_t dh;
        };
    };
    union
    {
        int64_t rcx;
        int32_t ecx;
        int16_t cx;
        struct
        {
            int8_t cl;
            int8_t ch;
        };
    };
    union
    {
        int64_t rax;
        int32_t eax;
        int16_t ax;
        struct
        {
            int8_t al;
            int8_t ah;
        };
    };
};

template <typename T>
static void do_cmp(T x, T y, uint16_t *flags)
{
    asm volatile (
        "cmp %1,%2\n"
        "seto %%al\n"
        "lahf\n"
        "mov %%ax,%0\n"
        : "=m"(*flags) : "r"(x), "r"(y): "rax");
}

void cmp(int64_t x, int64_t y, uint16_t *flags, const char *_asm)
{
    fprintf(stderr, "%s\n", _asm); 
	do_cmp(x, y, flags);
}
void cmp(int32_t x, int32_t y, uint16_t *flags, const char *_asm)
{
    fprintf(stderr, "%s\n", _asm); 
	do_cmp(x, y, flags);
}
void cmp(int16_t x, int16_t y, uint16_t *flags, const char *_asm)
{
    fprintf(stderr, "%s\n", _asm); 
	do_cmp(x, y, flags);
}
void cmp(int8_t x, int8_t y, uint16_t *flags, const char *_asm)
{
    fprintf(stderr, "%s\n", _asm); 
	do_cmp(x, y, flags);
}

void cmp_and_clr_z(int64_t x, int64_t y, uint16_t *flags, const char *_asm)
{
    fprintf(stderr, "%s\n", _asm); 
	do_cmp(x, y, flags);
    *flags &= ~0x4000;
}

const void *never_take(const void *addr, size_t size, const char *_asm)
{
    const uint8_t *next = (const uint8_t *)addr + size;
    fprintf(stderr, "%s\t# NO_TAKE %p\n", _asm, next);
    return (const void *)next;
}
const void *always_take(const void *target, const char *_asm)
{
    fprintf(stderr, "%s\t# TAKE %p\n", _asm, target);
    return target;
}
const void *jump(const void *target, const char *_asm)
{
    fprintf(stderr, "%s\t# JUMP %p\n", _asm, target);
    return target;
}

void swap(intptr_t *a, intptr_t *b, const char *_asm)
{
    fprintf(stderr, "%s\t# SWAP\n", _asm);
    intptr_t tmp = *a;
    *a = *b;
    *b = tmp;
}

void zero(void *state_0, const char *_asm)
{
    fprintf(stderr, "%s\t# ZERO ALL REGS\n", _asm);
    STATE *state = (STATE *)state_0;
    state->rax = state->rcx = state->rdx = state->rbx = 
        state->rbp = state->rsi = state->rdi = state->r8 =
        state->r9 = state->r10 = state->r11 = state->r12 =
        state->r13 = state->r14 = state->r15 = 0x0;
}

void zero_flags(uint16_t *rflags, const char *_asm)
{
    fprintf(stderr, "%s\t# ZERO FLAGS %c%c%c%c%c\n", _asm,
        (*rflags & 0x8000? 'S': '-'),
        (*rflags & 0x4000? 'Z': '-'),
        (*rflags & 0x1000? 'A': '-'),
        (*rflags & 0x0400? 'P': '-'),
        (*rflags & 0x0001? 'O': '-'));
    *rflags = 0x0;
}

void zero_flags(void *state_0, const char *_asm)
{
    STATE *state = (STATE *)state_0;
    zero_flags(&state->rflags, _asm);
}

void sum(intptr_t x, const char *_asm)
{
    static intptr_t s = 0;
    s += x;
    fprintf(stderr, "%s # %zd\n", _asm, s);
}

void dst_zero(int64_t *dst, const char *_asm)
{
    fprintf(stderr, "%s # DST %.16lX --> 0x0\n", _asm, *dst);
    *dst = 0x0;
}

intptr_t ret_0(const char *_asm)
{
    fprintf(stderr, "%s\n", _asm);
    return 0;
}

intptr_t ret_1(const char *_asm)
{
    fprintf(stderr, "%s\n", _asm);
    return 1;
}

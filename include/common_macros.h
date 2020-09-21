#ifndef COMMON_MACROS_H_
#define COMMON_MACROS_H_

#define csr_read(CSR_ADDR) ({perfcounter csrr_ret; \
								__asm__ __volatile__ ("csrr %[dest_reg], %[csr];" \
													: [dest_reg]	"=r" ( csrr_ret ) \
													: [csr]			"i" (CSR_ADDR) : ); \
													csrr_ret; })

#ifdef SPIKE
#define instr_timer() ({	perfcounter ret; \
							u32 high; \
							u32 low; \
							__asm__ __volatile__ ("rdinstreth %0; \
												rdinstret  %1;" \
												: "=r" ( high ), \
												  "=r" ( low ) );  \
							ret = ((perfcounter) high << 32) | low; \
							ret; \
												})
#define cycle_timer(var) ({	perfcounter ret; \
							u32 high; \
							u32 low; \
							__asm__ __volatile__ ("rdcycleh %0; \
											  rdcycle  %1;" \
											  : "=r" ( high ), \
											    "=r" ( low  ) ); \
							ret = ((perfcounter) high << 32) | low; \
							ret; \
							})
#elif defined RISCY

#define instr_timer() csr_read(0x781)
#define cycle_timer() csr_read(0x780)

#endif

#endif // COMMON_MACROS_H_
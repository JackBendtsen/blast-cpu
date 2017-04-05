#include "cpu.h"
#include "ram.h"

void cpu_tick(regs *r) {
	if (r->state == 0) r->insr = read_triple(0, r->pc);
	else if (r->state == 1) decode(r);
	else execute(r);

	if (r->state < 0 || r->state >= 3 || (r->state >= 1 && !r->cmd))
		r->state = 0;
	else
		r->state++;
}

#include "utils/board.h"

__EXTERN_C_BEGIN

void board_reboot() {
	__DSB();
	SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
	RSTC->RSTC_CR = RSTC_CR_KEY(0xA5) | RSTC_CR_PERRST | RSTC_CR_PROCRST;
	NVIC_SystemReset();
}

__EXTERN_C_END

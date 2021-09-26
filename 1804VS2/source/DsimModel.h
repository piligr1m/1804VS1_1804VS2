#pragma once
#include <windows.h>
#include "stdafx.h"
#include "sdk/vsm.hpp"
#include <vector>
#include <stdint.h>

class DsimModel : public IDSIMMODEL {
	IINSTANCE* _inst;
	IDSIMCKT* _ckt;

	uint32_t _dbg_counter;
	uint8_t _regs[16];
	uint8_t _reg_q;
	uint8_t _reg_y;
	bool _reg_z;
	bool _reg_f3;
	bool _reg_c4;
	bool _reg_ovr;
	IDSIMPIN* _pin_A[4];
	IDSIMPIN* _pin_B[4];
	IDSIMPIN* _pin_DA[4];
	IDSIMPIN* _pin_DB[4];
	IDSIMPIN* _pin_I[9];
	IDSIMPIN* _pin_Y[4];
	IDSIMPIN* _pin_PQ0;
	IDSIMPIN* _pin_PQ3;
	IDSIMPIN* _pin_EA;
	IDSIMPIN* _pin_C0;
	IDSIMPIN* _pin_C4;
	IDSIMPIN* _pin_POVR;
	IDSIMPIN* _pin_GN;
	IDSIMPIN* _pin_OEY;
	IDSIMPIN* _pin_PF0;
	IDSIMPIN* _pin_PF3;
	IDSIMPIN* _pin_Z;
	IDSIMPIN* _pin_OEB;
	IDSIMPIN* _pin_WE;
	IDSIMPIN* _pin_IEN;
	IDSIMPIN* _pin_LSS;
	IDSIMPIN* _pin_WMSS;
	IDSIMPIN* _pin_T;
	uint8_t genValue(IDSIMPIN** pins, size_t n, size_t offset = 0);
	bool isHigh(IDSIMPIN* pin);
	bool isLow(IDSIMPIN* pin);
	bool isNegedge(IDSIMPIN* pin);
	bool isPosedge(IDSIMPIN* pin);
	void setState(ABSTIME time, IDSIMPIN* pin, bool set);
public:
	INT isdigital(CHAR* pinname);
	VOID setup(IINSTANCE* inst, IDSIMCKT* dsim);
	VOID runctrl(RUNMODES mode);
	VOID actuate(REALTIME time, ACTIVESTATE newstate);
	BOOL indicate(REALTIME time, ACTIVEDATA* data);
	VOID simulate(ABSTIME time, DSIMMODES mode);
	VOID callback(ABSTIME time, EVENTID eventid);
};

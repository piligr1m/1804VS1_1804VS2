// MT1804BC1.cpp: определяет экспортированные функции для приложения DLL.
//

#include "1804VS1.h"

uint8_t K1804BC1::genValue(IDSIMPIN** pins, size_t n, size_t offset) {
	uint8_t res = 0;
	for (size_t i = 0; i < n; ++i)
		res |= isHigh(pins[offset + i]) << i;
	return res;
}

VOID K1804BC1::setup(IINSTANCE *instance, IDSIMCKT *dsimckt) {
	_inst = instance;
	_ckt = dsimckt;

	char buffer[16];
	for (size_t i = 0; i < 4; ++i) {
		sprintf_s(buffer, "A%d", i);
		_pin_A[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < 4; ++i) {
		sprintf_s(buffer, "B%d", i);
		_pin_B[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < 4; ++i) {
		sprintf_s(buffer, "D%d", i);
		_pin_D[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < 9; ++i) {
		sprintf_s(buffer, "I%d", i);
		_pin_I[i] = _inst->getdsimpin(buffer, true);
	}

	_pin_T = _inst->getdsimpin((CHAR*)"T", true);
	_pin_OE = _inst->getdsimpin((CHAR*)"OE", true);
	_pin_C0 = _inst->getdsimpin((CHAR*)"C0", true);

	for (size_t i = 0; i < 4; ++i) {
		sprintf_s(buffer, "Y%d", i);
		_pin_Y[i] = _inst->getdsimpin(buffer, true);
		setState(0, _pin_Y[i], false);
	}
	_pin_C4 = _inst->getdsimpin((CHAR*)"C4", true);
	setState(0, _pin_C4, false);
	_pin_Z = _inst->getdsimpin((CHAR*)"Z", true);
	setState(0, _pin_Z, false);
	_pin_F3 = _inst->getdsimpin((CHAR*)"F3", true);
	setState(0, _pin_F3, false);
	_pin_OVR = _inst->getdsimpin((CHAR*)"O", true);
	setState(0, _pin_OVR, false);
}


VOID K1804BC1::simulate(ABSTIME time, DSIMMODES mode) {
	if (isPosedge(_pin_T)) {
		uint8_t from = genValue(_pin_I, 3, 0);
		uint8_t alu = genValue(_pin_I, 3, 3);
		uint8_t to = genValue(_pin_I, 3, 6);
		uint8_t a = genValue(_pin_A, 4);
		uint8_t b = genValue(_pin_B, 4);
		uint8_t d = genValue(_pin_D, 4);
		uint8_t c0 = isHigh(_pin_C0);

		uint8_t opA = 0;
		uint8_t opB = 0;

		std::string logmessage = "CMD #" + std::to_string(_dbg_counter++) + "\n";
		switch (from) {

		case 0:
			opA = _regs[a];
			opB = _reg_q;
			logmessage += "from = A(" + std::to_string(opA) + "), Q(" + std::to_string(opB) + ");\n";
			break;

		case 1:
			opA = _regs[a];
			opB = _regs[b];
			logmessage += "from = A(" + std::to_string(opA) + "), B(" + std::to_string(opB) + ");\n";
			break;

		case 2:
			opA = 0;
			opB = _reg_q;
			logmessage += "from = 0, Q(" + std::to_string(opB) + ");\n";
			break;

		case 3:
			opA = 0;
			opB = _regs[b];
			logmessage += "from = 0, B(" + std::to_string(opB) + ");\n";
			break;

		case 4:
			opA = 0;
			opB = _regs[a];
			logmessage += "from = 0, A(" + std::to_string(opB) + ");\n";
			break;

		case 5:
			opA = d;
			opB = _regs[a];
			logmessage += "from = D(" + std::to_string(opA) + "), A(" + std::to_string(opB) + ");\n";
			break;

		case 6:
			opA = d;
			opB = _reg_q;
			logmessage += "from = D(" + std::to_string(opA) + "), Q(" + std::to_string(opB) + ");\n";
			break;

		case 7:
			opA = d;
			opB = 0;
			logmessage += "from = D(" + std::to_string(opA) + "), 0;\n";
			break;
		}

		uint8_t result;
		switch (alu) {

		case 0:
			result = opA + opB + c0;
			logmessage += "R + S + C0(" + std::to_string(c0) + ") = " + std::to_string(result) + ";\n";
			break;

		case 2:
			result = opA + (~opB & 0b1111) + c0;
			logmessage += "R - S - 1 + C0(" + std::to_string(c0) + ") = " + std::to_string(result) + ";\n";
			break;

		case 1:
			result = opB + (~opA & 0b1111) + c0;
			logmessage += "S - R - 1 + C0(" + std::to_string(c0) + ") = " + std::to_string(result) + ";\n";
			break;

		case 3:
			result = opB | opA;
			logmessage += "R | S = " + std::to_string(result) + ";\n";
			break;

		case 4:
			result = opB & opA;
			logmessage += "R & S = " + std::to_string(result) + ";\n";
			break;

		case 5:
			result = opB & (~opA & 0b1111);
			logmessage += "~R | S = " + std::to_string(result) + ";\n";
			break;

		case 6:
			result = opB ^ opA;
			logmessage += "R ^ S = " + std::to_string(result) + ";\n";
			break;

		case 7:
			result = (~(opB ^ opA)) & 0b1111;
			logmessage += "~(R ^ S) = " + std::to_string(result) + ";\n";
			break;
		}

		_reg_z = !(result & 0b1111);
		_reg_f3 = result & 0b1000;
		_reg_c4 = result & 0b10000;
		_reg_ovr = _reg_c4 ^ _reg_f3;


		switch (to) {
		case 0:
			_reg_q = result & 0b1111;
			logmessage += "to = Q;\n";
			break;

		case 1:
			break;

		case 2:
			_regs[b] = result & 0b1111;
			logmessage += "to = B;\n";
			break;

		case 3:
			_regs[b] = result & 0b1111;
			result = _regs[a];
			logmessage += "to = B, Y = A;\n";
			break;

		case 4:
			_reg_q = _reg_q >> 1;
		case 5:
			logmessage += "to = B >> 1;\n";
			_regs[b] = (result >> 1) & 0b1111;
			break;

		case 6:
			_reg_q = _reg_q << 1;
		case 7:
			logmessage += "to = B << 1;\n";
			_regs[b] = (result << 1) & 0b1111;
			break;
		}


		logmessage += "Z = " + std::to_string(_reg_z) + "; ";
		logmessage += "F3 = " + std::to_string(_reg_f3) + "; ";
		logmessage += "C4 = " + std::to_string(_reg_c4) + "; ";
		logmessage += "OVR = " + std::to_string(_reg_ovr) + ";\n";

		_reg_y = result & 0b1111;
		logmessage += "Y = " + std::to_string(_reg_y) + "; ";

		_inst->log((CHAR*)logmessage.c_str());

	}

	if (isHigh(_pin_OE)) {
		for (size_t i = 0; i < 4; ++i)
			setState(time, _pin_Y[i], _reg_y & (1 << i));
		setState(time, _pin_Z, _reg_z);
		setState(time, _pin_F3, _reg_f3);
		setState(time, _pin_C4, _reg_c4);
		setState(time, _pin_OVR, _reg_ovr);
	}
	else {
		for (size_t i = 0; i < 4; ++i)
			setState(time, _pin_Y[i], false);
		setState(time, _pin_Z, false);
		setState(time, _pin_F3, false);
		setState(time, _pin_C4, false);
		setState(time, _pin_OVR, false);
	}

}

bool K1804BC1::isHigh(IDSIMPIN* pin) {
	return ishigh(pin->istate());
}
bool K1804BC1::isLow(IDSIMPIN* pin) {
	return islow(pin->istate());
}
bool K1804BC1::isNegedge(IDSIMPIN* pin) {
	return pin->isnegedge();
}
bool K1804BC1::isPosedge(IDSIMPIN* pin) {
	return pin->isposedge();
}
void K1804BC1::setState(ABSTIME time, IDSIMPIN* pin, bool set) {
	pin->setstate(time, 1, set ? SHI : SLO);
}

INT K1804BC1::isdigital(CHAR *pinname) {
	return 1;
}
VOID K1804BC1::runctrl(RUNMODES mode) {}
VOID K1804BC1::actuate(REALTIME time, ACTIVESTATE newstate) {}
VOID K1804BC1::callback(ABSTIME time, EVENTID eventid) {}
BOOL K1804BC1::indicate(REALTIME time, ACTIVEDATA *data) {
	return FALSE;
}

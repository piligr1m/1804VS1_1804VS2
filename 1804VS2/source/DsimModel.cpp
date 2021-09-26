// MT1804BC2 определяет экспортированные функции для приложения DLL.
//
#include "StdAfx.h"
#include "DsimModel.h"
#include <string>
#include <iostream>

uint8_t DsimModel::genValue(IDSIMPIN** pins, size_t n, size_t offset) {
	uint8_t res = 0;
	for (size_t i = 0; i < n; ++i)
		res |= isHigh(pins[offset + i]) << i;
	return res;
}

VOID DsimModel::setup(IINSTANCE* instance, IDSIMCKT* dsimckt) {
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
		sprintf_s(buffer, "DA%d", i);
		_pin_DA[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < 4; ++i) {
		sprintf_s(buffer, "DB%d", i);
		_pin_DB[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < 9; ++i) {
		sprintf_s(buffer, "I%d", i);
		_pin_I[i] = _inst->getdsimpin(buffer, true);
	}

	_pin_T = _inst->getdsimpin((CHAR*)"T", true);
	_pin_OEY = _inst->getdsimpin((CHAR*)"OE", true);
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
	_pin_PF0 = _inst->getdsimpin((CHAR*)"F3", true);
	setState(0, _pin_PF3, false);
	_pin_PF3 = _inst->getdsimpin((CHAR*)"F3", true);
	setState(0, _pin_PF3, false);
	_pin_PQ0 = _inst->getdsimpin((CHAR*)"F3", true);
	setState(0, _pin_PQ0, false);
	_pin_PQ3 = _inst->getdsimpin((CHAR*)"F3", true);
	setState(0, _pin_PQ3, false);
	_pin_POVR = _inst->getdsimpin((CHAR*)"O", true);
	setState(0, _pin_POVR, false);
}


VOID DsimModel::simulate(ABSTIME time, DSIMMODES mode) {
	uint8_t WMSS = isHigh(_pin_WMSS);
	uint8_t LSS = isHigh(_pin_LSS);
	uint8_t c4 = isHigh(_pin_C4);
	uint8_t PF3 = isHigh(_pin_PF3);
	uint8_t PF0 = isHigh(_pin_PF0);
	uint8_t PQ3 = isHigh(_pin_PQ3);
	uint8_t PQ0 = isHigh(_pin_PQ0);
	uint8_t GN = 0;

	if (isPosedge(_pin_T)) {
		uint8_t alu = genValue(_pin_I, 4, 1);
		uint8_t to = genValue(_pin_I, 4, 5);
		uint8_t IO = isHigh(_pin_I[0]);
		uint8_t a = genValue(_pin_A, 4);
		uint8_t b = genValue(_pin_B, 4);
		uint8_t da = genValue(_pin_DA, 4);
		uint8_t db = genValue(_pin_DB, 4);
		uint8_t c0 = isHigh(_pin_C0);
		uint8_t Z = isHigh(_pin_Z);
		uint8_t EA = isHigh(_pin_EA);
		uint8_t OEB = isHigh(_pin_OEB);

		uint8_t opR = 0;
		uint8_t opS = 0;

		std::string logmessage = "CMD #" + std::to_string(_dbg_counter++) + "\n";

		if (!EA)
		{
			opR = _regs[a];
			logmessage += "from = A(" + std::to_string(opR) + ")";
		}
		else
		{
			opR = da;
			logmessage += "from = DA(" + std::to_string(opR) + ")";
		}

		if (!IO)
		{
			opS = _reg_q;
			logmessage += ", to Q(" + std::to_string(opS) + ");\n";
		}
		else
		{

			if (!OEB)
			{
				opS = _regs[b];
				logmessage += ", to B(" + std::to_string(opS) + ");\n";
			}
			else
			{
				opS = db;
				logmessage += ", to DB(" + std::to_string(opS) + ");\n";
			}
		}

		uint8_t F;
		switch (alu) {

		case 0:

			if (!IO)
			{

				switch (to) {

				case 0:
					if (!Z)
					{
						F = opS + c0;
						logmessage += "S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					}
					else
					{
						F = opR + opS + c0;
						logmessage += "R + S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					}
					break;



				case 2:
					if (!Z)
					{
						F = opS + c0;
						logmessage += "S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					}
					else
					{
						F = opR + opS + c0;
						logmessage += "R + S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					}
					break;

				case 4:
						F = opS + 1 + c0;
						logmessage += "S + 1 + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					break;

				case 5:
					F = opS + c0;
					logmessage += "S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					break;

				case 6:
					if (!Z)
					{
						F = opS + c0;
						logmessage += "S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					}
					else
					{
						F = opS + (~opR & 0b1111) + c0;
						logmessage += "S - R - 1 + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					}
					break;

				case 8:
				case 10:
					F = opS + c0;
					logmessage += "S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					break;

				case 12:
				case 14:
					if (!Z)
					{
						F = opR + opS + c0;
						logmessage += "R + S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					}
					else
					{
						F = opS + (~opR & 0b1111) + c0;
						logmessage += "S - R - 1 + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
					}
					break;
				}
			}
			else
			{
				F = 0b1111;
				logmessage += "F = 1111 ;\n";
			}
			break;



		case 1:
			F = opS + (~opR & 0b1111) + c0;
			logmessage += "S - R - 1 + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
			break;

		case 2:
			F = opR + (~opS & 0b1111) + c0;
			logmessage += "R - S - 1 + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
			break;

		case 3:
			F = opR + opS + c0;
			logmessage += "R + S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
			break;

		case 4:
			F = opS + c0;
			logmessage += "S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
			break;

		case 5:
			F = ~opS + c0;
			logmessage += "~S + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
			break;

		case 6:
			F = opR + c0;
			logmessage += "R + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
			break;

		case 7:
			F = ~opR + c0;
			logmessage += "~R + C0(" + std::to_string(c0) + ") = " + std::to_string(F) + ";\n";
			break;

		case 8:
			F = 0;
			logmessage += "zero = 0000 ;\n";
			break;

		case 9:
			F = opS & (~opR & 0b1111);
			logmessage += "~R | S = " + std::to_string(F) + ";\n";
			break;

		case 10:
			F = ~(opR ^ opS) & 0b1111;
			logmessage += "~(R ^ S) = " + std::to_string(F) + ";\n";
			break;

		case 11:
			F = opR ^ opS;
			logmessage += " R ^ S = " + std::to_string(F) + ";\n";
			break;

		case 12:
			F = opR & opS;
			logmessage += "R & S = " + std::to_string(F) + ";\n";
			break;

		case 13:
			F = ~(opR | opS) & 0b1111;
			logmessage += "~(R | S) = " + std::to_string(F) + ";\n";
			break;

		case 14:
			F = ~(opR & opS) & 0b1111;
			logmessage += "~(R & S) = " + std::to_string(F) + ";\n";
			break;

		case 15:
			F = opR | opS;
			logmessage += " R | S = " + std::to_string(F) + ";\n";
			break;
		}

		uint8_t slice = 0;

		if (LSS & !WMSS)
		{
			//most slice
			slice = 0;
			

		}else if( LSS & WMSS)
		{
			slice = 1;
			//second slice
		}
		else
		{
			slice = 2;
			//last slice
		}



		_reg_z = !(F & 0b1111);
		_reg_f3 = F & 0b1000;
		_reg_c4 = F & 0b10000;
		_reg_ovr = _reg_c4 ^ _reg_f3;

		F &= 0b1111;

		switch (to) {
		case 0:
			_reg_y = F >> 1;
			if (!slice)
			{
				_reg_y |= c4 << 3;
			}
			else
			{
				_reg_y |= PF3 << 3;
			}
			PF0 = F >> 3;
			logmessage += "Arith F/2 -> Y;\n";

			PQ0 = _reg_q & 0b0001;
			_reg_q = (_reg_q >> 1) | (PQ3 << 3);

			break;

		case 2:
			_reg_y = F >> 1;
			if (!slice)
			{
				_reg_y |= _reg_ovr;
			}
			else
			{
				_reg_y |= PF3 << 3;
			}
			PF0 = F >> 3;
			logmessage += "Log F/2 -> Y;\n";

			PQ0 = _reg_q & 0b0001;
			_reg_q = (_reg_q >> 1) | (PQ3 << 3);
			break;

		case 4:
			_reg_y = F;
			PF0 = PF3 ^ (F >> 3) ^ ((F & 0b0100) >> 2) ^ ((F & 0b0010) >> 1) ^ (F & 0b0001);
			logmessage += "F -> Y;\n";

			_reg_q = F;
			break;

		case 5:
			_reg_y = F;
			if (!slice)
			{
				_reg_y &= F & opS & 0b1000;
			}
			PF0 = PF3 ^ (F >> 3) ^ ((F & 0b0100) >> 2) ^ ((F & 0b0010) >> 1) ^ (F & 0b0001);
			logmessage += "F -> Y;\n";

			_reg_q = F;
			break;

		case 6:
			_reg_y = F >> 1;
			if (!slice)
			{
				_reg_y |= _reg_ovr;
			}
			else
			{
				_reg_y |= PF3 << 3;
			}
			PF0 = F >> 3;
			logmessage += "Log F/2 -> Y;\n";

			PQ0 = _reg_q & 0b0001;
			_reg_q = (_reg_q >> 1) | (PQ3 << 3);
			break;

		case 8:
			_reg_y = F;
			PF3 = F >> 3;
			logmessage += "F -> Y;\n";

			PQ3 = _reg_q >> 3;
			_reg_q = (_reg_q << 1) | PQ0;
			break;

		case 10:
			_reg_y = F << 1;
			if (!slice)
			{
				PF3 = (opR ^ F) >> 3;
			}
			else
			{
				PF3 = F >> 3;
			}
			_reg_y |= PF0;
			logmessage += "Log 2F -> Y;\n";

			PQ3 = _reg_q >> 3;
			_reg_q = (_reg_q << 1) | PQ0;
			break;

		case 12:
			_reg_y = F << 1;
			if (!slice)
			{
				PF3 = (~(opR ^ F) & 0b1111) >> 3;
			}
			else
			{
				PF3 = F >> 3;
			}
			_reg_y |= PF0;
			logmessage += "Log 1F -> Y;\n";

			PQ3 = _reg_q >> 3;
			_reg_q = (_reg_q << 1) | PQ0;
			break;

		case 14:
			_reg_y = F;
			PF3 = F >> 3;
			logmessage += "F -> Y;\n";

			PQ3 = _reg_q >> 3;
			_reg_q = (_reg_q << 1) | PQ0;
			break;

		}

		uint8_t Galu = genValue(_pin_I, 9, 1);
		uint8_t GaluO = isHigh(_pin_I[0]);
		uint8_t G = 0;
		uint8_t P = 0;
		if (!GaluO)
		{
			G = 0;
			_reg_c4 = 0;
			_reg_ovr = 0;
		}
		else
		{
			switch (Galu) {
			case 1:
				G = opS & (~opR & 0b1111);
				logmessage += "G = ~R | S = " + std::to_string(G) + ";\n";
				P = G;
				break;

			case 2:
				G = opR & opS;
				logmessage += "G = R | S = " + std::to_string(G) + ";\n";

				P = opR & (~opS & 0b1111);
				break;

			case 3:
				G = opR & opS;
				logmessage += "G = R | S = " + std::to_string(G) + ";\n";
				P = G;
				break;

			case 4:
				P = opS;
				break;

			case 5:
				P = ~opS & 0b1111;
				break;

			case 6:
				P = opR;
				break;

			case 7:
				P = ~opR & 0b1111;
				break;

			case 8:
			case 9:
				G = opS & (~opR & 0b1111);
				logmessage += "G = ~R | S = " + std::to_string(G) + ";\n";
				P = 0b1111;
				break;

			case 10:
				G = opR & opS;
				logmessage += "G = R | S = " + std::to_string(G) + ";\n";
				P = G;
				break;

			case 11:
				G = opS & (~opR & 0b1111);
				logmessage += "G = ~R | S = " + std::to_string(G) + ";\n";
				P = G;
				break;

			case 12:
				G = opR & opS;
				logmessage += "G = R | S = " + std::to_string(G) + ";\n";
				P = 0b1111;
				break;

			case 13:
				G = opS & (~opR & 0b1111);
				logmessage += "G = ~R | S = " + std::to_string(G) + ";\n";
				P = 0b1111;
				break;

			case 14:
				G = opR & opS;
				logmessage += "G = R | S = " + std::to_string(G) + ";\n";
				P = 0b1111;
				break;

			case 15:
				G = opS & (~opR & 0b1111);
				logmessage += "G = ~R | S = " + std::to_string(G) + ";\n";
				P = 0b1111;
				break;

			case 0:
			case 32:

				if (!Z)
				{
					G = 0;
					logmessage += "G = 0;\n";
					P = opS;
				}
				else
				{
					G = opR & opS;
					logmessage += "G = R | S = " + std::to_string(G) + ";\n";
					P = G;
				}
				
				break;

			case 64:
				if (slice == 2)
				{
					G = opS & 0b0001;
					P = 0b0001 | (opS & 0b1110);
				}
				else
				{
					G = 0;
					P = opS;
				}
				
				logmessage += "G = (opS & 0b0001) | (G & 0b0110)= " + std::to_string(G) + ";\n";
				
				break;

			case 80:
				if (!Z)
				{
					P = opS;
				}
				else
				{
					G = ~opS & 0b1111;
				}

				break;
			case 96:
				if (!Z)
				{
					G = 0;
					logmessage += "G = 0;\n";
					P = opS;
				}
				else
				{
					G = opS & (~opR & 0b1111);
					logmessage += "G = ~R | S = " + std::to_string(G) + ";\n";
					G = P;
				}

				break;

			case 128:
			case 160:
				P = opS;
				break;
			case 192:
			case 224:
				if (!Z)
				{
					G = opR & opS;
					logmessage += "G = R | S = " + std::to_string(G) + ";\n";
					P = G;
				}
				else
				{
					G = opS & (~opR & 0b1111);
					logmessage += "G = ~R | S = " + std::to_string(G) + ";\n";
					P = G;
				}
				break;
			}
			uint8_t C3 = (G & 0b0010) >> 1 | ((G >> 1) & (P >> 2)) | (G & (P >> 2) & (P >> 1));
			C3 &= 0b1111;
			switch (Galu) {
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 0:
			case 32:
			case 64:
			case 80:
			case 96:
			case 192:
			case 224:
				_reg_c4 = G | P & c0;
				if (slice == 0)
				{
					_reg_ovr = C3 ^ _reg_c4;
				}
				else
				{
					_reg_ovr = ~P;
				}
				break;

			case 128:
				if (slice == 0)
				{
					_reg_c4 = (_reg_q >> 3) ^ ((_reg_q >> 2) & 0b0001);
				}
				else
				{
					_reg_c4 = G | P & c0;
				}
				break;
			case 160:
				if (slice == 0)
				{
					_reg_c4 = (F >> 3) ^ ((F >> 2) & 0b0001);
				}
				else
				{
					_reg_c4 = G | P & c0;
				}
				break;

			default:
				_reg_c4 = 0;
			}

			uint8_t GN = 0;
			if (slice == 0)
			{
				GN = F >> 3;
			}
			else
			{
				GN = ~G;
			}
			_reg_z = ~_reg_y;
		}
		logmessage += "Z = " + std::to_string(_reg_z) + "; ";
		logmessage += "F3 = " + std::to_string(_reg_f3) + "; ";
		logmessage += "C4 = " + std::to_string(_reg_c4) + "; ";
		logmessage += "OVR = " + std::to_string(_reg_ovr) + ";\n";

		_reg_y = F & 0b1111;
		logmessage += "Y = " + std::to_string(_reg_y) + "; ";

		_inst->log((CHAR*)logmessage.c_str());

	}

	if (isHigh(_pin_OEY)) {
		for (size_t i = 0; i < 4; ++i)
			setState(time, _pin_Y[i], _reg_y & (1 << i));
		setState(time, _pin_Z, _reg_z);
		setState(time, _pin_PF0, PF0);
		setState(time, _pin_PF3, PF3);
		setState(time, _pin_PQ0, PQ0);
		setState(time, _pin_PQ3, PQ3);
		setState(time, _pin_C4, _reg_c4);
		setState(time, _pin_POVR, _reg_ovr);
		setState(time, _pin_GN, GN);
	}
	else {
		for (size_t i = 0; i < 4; ++i)
			setState(time, _pin_Y[i], false);
		setState(time, _pin_Z, false);
		setState(time, _pin_PF0, false);
		setState(time, _pin_PF3, false);
		setState(time, _pin_PQ0, false);
		setState(time, _pin_PQ3, false);
		setState(time, _pin_C4, false);
		setState(time, _pin_POVR, false);
		setState(time, _pin_GN, false);
	}

}

bool DsimModel::isHigh(IDSIMPIN* pin) {
	return ishigh(pin->istate());
}
bool DsimModel::isLow(IDSIMPIN* pin) {
	return islow(pin->istate());
}
bool DsimModel::isNegedge(IDSIMPIN* pin) {
	return pin->isnegedge();
}
bool DsimModel::isPosedge(IDSIMPIN* pin) {
	return pin->isposedge();
}
void DsimModel::setState(ABSTIME time, IDSIMPIN* pin, bool set) {
	pin->setstate(time, 1, set ? SHI : SLO);
}

INT DsimModel::isdigital(CHAR* pinname) {
	return 1;
}
VOID DsimModel::runctrl(RUNMODES mode) {}
VOID DsimModel::actuate(REALTIME time, ACTIVESTATE newstate) {}
VOID DsimModel::callback(ABSTIME time, EVENTID eventid) {}
BOOL DsimModel::indicate(REALTIME time, ACTIVEDATA* data) {
	return FALSE;
}
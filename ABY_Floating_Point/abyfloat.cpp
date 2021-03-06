/**
 \file 		abyfloat.cpp
 \author	daniel.demmler@ec-spride.de
 \copyright	ABY - A Framework for Efficient Mixed-protocol Secure Two-party Computation
 Copyright (C) 2017 Engineering Cryptographic Protocols Group, TU Darmstadt
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Affero General Public License for more details.
 You should have received a copy of the GNU Affero General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include "../../abycore/aby/abyparty.h"
#include "../../abycore/circuit/share.h"
#include "../../abycore/circuit/booleancircuits.h"
#include "../../abycore/sharing/sharing.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <vector>

//#define FLOAT //Toggle between floating and fixed point


/**************************************
 * EzPC Functionalitites
 * ***********************************/
share* put_cons32_gate(Circuit* c, uint32_t val) {
  uint32_t x = val;
  return c->PutCONSGate(x, (uint32_t)32);
}

share* put_cons64_gate(Circuit* c, uint64_t val) {
  uint64_t x = val;
  return c->PutCONSGate(x, (uint32_t)64);
}

share* put_cons1_gate(Circuit* c, uint64_t val) {
  uint64_t x = val;
  return c->PutCONSGate(x, (uint32_t)1);
}

share* left_shift(Circuit* c, share* val, uint32_t shift_factor) {
  uint32_t share_wire_count = val->get_bitlength();
  share* fresh_zero_share = put_cons64_gate(c, 0);
  std::vector<uint32_t> val_wires = val->get_wires();
  if(share_wire_count == 1){
	  std::cout<<"Error. Share not padded. A share cannot exist with just 1 wire.\n";
  }
  // Note here the assumption is that if we receive the val share as a share of size 32, we output the share as a share of size 32 only and drop the MSBs which overflow the 32 bit constraint.
  for(int i=0; i+shift_factor<share_wire_count; i++){
    fresh_zero_share->set_wire_id(shift_factor+i, val_wires[i]);
  }
  return fresh_zero_share;
}

share* logical_right_shift(Circuit* c, share* val, uint32_t shift_factor) {
  std::vector<uint32_t> tmp = val->get_wires();
  tmp.erase(tmp.begin(), tmp.begin() + shift_factor);
  // tmp.insert(tmp.end(), shift_factor, 0);
  return create_new_share( tmp , c );
}

/*******************************************
 * Main functions for float vs fixed 
 * ****************************************/

void read_test_options(int32_t* argcp, char*** argvp, e_role* role,
	uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* address,
	uint16_t* port, int32_t* test_op, uint32_t* test_bit, std::string* circuit, uint64_t* fpa, uint64_t* fpb) {

	uint32_t int_role = 0, int_port = 0, int_testbit = 0;

	parsing_ctx options[] =
	{ {(void*) &int_role, T_NUM, "r", "Role: 0/1", true, false },
	{(void*) &int_testbit, T_NUM, "i", "test bit", false, false },
	{(void*) nvals, T_NUM, "n",	"Number of parallel operation elements", false, false },
	{(void*) bitlen, T_NUM, "b", "Bit-length, default 32", false,false },
	{(void*) secparam, T_NUM, "s", "Symmetric Security Bits, default: 128", false, false },
	{(void*) address, T_STR, "a", "IP-address, default: localhost", false, false },
	{(void*) circuit, T_STR, "c", "circuit file name", false, false },
	{(void*) &int_port, T_NUM, "p", "Port, default: 7766", false, false },
	{(void*) test_op, T_NUM, "t", "Single test (leave out for all operations), default: off", false, false },
	{(void*) fpa, T_NUM, "x", "FP a", false, false },
	{(void*) fpb, T_NUM, "y", "FP b", false, false }

	};

	if (!parse_options(argcp, argvp, options,
		sizeof(options) / sizeof(parsing_ctx))) {
		print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
	std::cout << "Exiting" << std::endl;
	exit(0);
	}

	assert(int_role < 2);
	*role = (e_role) int_role;

	if (int_port != 0) {
		assert(int_port < 1 << (sizeof(uint16_t) * 8));
		*port = (uint16_t) int_port;
	}

	*test_bit = int_testbit;
}


void read_test_options(int32_t* argcp, char*** argvp, e_role* role,
	uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* address,
	uint16_t* port, int32_t* test_op, uint32_t* test_bit, std::string* circuit, double* fpa, double* fpb) {

	uint32_t int_role = 0, int_port = 0, int_testbit = 0;

	parsing_ctx options[] =
	{ {(void*) &int_role, T_NUM, "r", "Role: 0/1", true, false },
	{(void*) &int_testbit, T_NUM, "i", "test bit", false, false },
	{(void*) nvals, T_NUM, "n",	"Number of parallel operation elements", false, false },
	{(void*) bitlen, T_NUM, "b", "Bit-length, default 32", false,false },
	{(void*) secparam, T_NUM, "s", "Symmetric Security Bits, default: 128", false, false },
	{(void*) address, T_STR, "a", "IP-address, default: localhost", false, false },
	{(void*) circuit, T_STR, "c", "circuit file name", false, false },
	{(void*) &int_port, T_NUM, "p", "Port, default: 7766", false, false },
	{(void*) test_op, T_NUM, "t", "Single test (leave out for all operations), default: off", false, false },
	{(void*) fpa, T_DOUBLE, "x", "FP a", false, false },
	{(void*) fpb, T_DOUBLE, "y", "FP b", false, false }

	};

	if (!parse_options(argcp, argvp, options,
		sizeof(options) / sizeof(parsing_ctx))) {
		print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
	std::cout << "Exiting" << std::endl;
	exit(0);
	}

	assert(int_role < 2);
	*role = (e_role) int_role;

	if (int_port != 0) {
		assert(int_port < 1 << (sizeof(uint16_t) * 8));
		*port = (uint16_t) int_port;
	}

	*test_bit = int_testbit;
}

void test_floating_point(e_role role, const std::string& address, uint16_t port, seclvl seclvl, uint32_t nvals, uint32_t nthreads,
	e_mt_gen_alg mt_alg, e_sharing sharing, double afp, double bfp) {

	// for addition we operate on doubles, so set bitlen to 64 bits
	uint32_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg);

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* circ = (BooleanCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

	// point a uint64_t pointer to the two input floats without casting the content
	uint64_t *aptr = (uint64_t*) &afp;
	uint64_t *bptr = (uint64_t*) &bfp;

	// use 32 bits for the sqrt example, so cast afp to float
	//float afloat = (float) afp;
	//uint32_t *afloatptr = (uint32_t*) &afloat;

	// for this example we need at least 4 values, since we do at least 4 example operations (see lines 100-102)
	//assert(nvals > 3);
	//std::cout<<nvals<<std::endl;
	// array of 64 bit values
	uint64_t avals[nvals];
	uint64_t bvals[nvals];

	// fill array with input values nvals times.
	std::fill(avals, avals + nvals, *aptr);
	std::fill(bvals, bvals + nvals, *bptr);

	// set some specific values differently for testing
	//bvals[1] = 0;
	//bvals[2] = *(uint64_t*) &afp;
	//avals[3] = *(uint64_t*) &bfp;

	// SIMD input gates
	share* ain = circ->PutSIMDINGate(nvals, avals, bitlen, SERVER);
	share* bin = circ->PutSIMDINGate(nvals, bvals, bitlen, CLIENT);

	// 32 bit input gate (non SIMD)
	//share* asqrtin = circ->PutINGate(afloatptr, 32, SERVER);

	// FP addition gate
	//share* sum = circ->PutFPGate(ain, bin, ADD, bitlen, nvals, no_status);
	share* mul = circ->PutFPGate(ain, bin, MUL, bitlen, nvals, no_status);
	
	for(int i=0; i<999; i++){
		mul = circ->PutFPGate(mul, bin, MUL, bitlen, nvals, no_status);
	}

	// 32-bit FP addition gate (bitlen, nvals, no_status are omitted)
	//share* sqrt_share = circ->PutFPGate(asqrtin, SQRT);

	// output gate
	//share* add_out = circ->PutOUTGate(sum, ALL);
	share* mul_out = circ->PutOUTGate(mul, ALL);

	//share* sqrt_out = circ->PutOUTGate(sqrt_share, ALL);

	// run SMPC
	party->ExecCircuit();

	// retrieve plain text output
	uint32_t out_bitlen, out_nvals;
	uint64_t *out_vals;

	//add_out->get_clear_value_vec(&out_vals, &out_bitlen, &out_nvals);

	// print every output
	//for (uint32_t i = 0; i < nvals; i++) {

		 //dereference output value as double without casting the content
		//double val = *((double*) &out_vals[i]);

		//std::cout << "ADD RES: " << val << " = " << *(double*) &avals[i] << " + " << *(double*) &bvals[i] << " | nv: " << out_nvals
		//<< " bitlen: " << out_bitlen << std::endl;
	//}

	mul_out->get_clear_value_vec(&out_vals, &out_bitlen, &out_nvals);

	// print every output
	for (uint32_t i = 0; i < nvals; i++) {

		// dereference output value as double without casting the content
		double val = *((double*) &out_vals[i]);

		std::cout << "MUL RES: " << val << " = " << *(double*) &avals[i] << " * " << *(double*) &bvals[i] << " | nv: " << out_nvals
		<< " bitlen: " << out_bitlen << std::endl;
	}

	//uint32_t *sqrt_out_vals = (uint32_t*) sqrt_out->get_clear_value_ptr();

	//float val = *((float*) sqrt_out_vals);

	//std::cout << "SQRT RES: " << val << " = " << sqrt(afloat) << std::endl;
}

void test_fixed_point(e_role role, const std::string& address, uint16_t port, seclvl seclvl, uint32_t nvals, uint32_t nthreads,
	e_mt_gen_alg mt_alg, e_sharing sharing, uint64_t afp, uint64_t bfp, uint32_t prec) {

	uint32_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg);

	std::vector<Sharing*>& sharings = party->GetSharings();
	
	//BooleanCircuit* circ2 = (BooleanCircuit*)sharings[S_BOOL]->GetCircuitBuildRoutine();
	//Circuit* circ1 = sharings[S_YAO]->GetCircuitBuildRoutine();
	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();

	// point a uint64_t pointer to the two input floats without casting the content
	//uint64_t *aptr = (uint64_t*) &afp;
	//uint64_t *bptr = (uint64_t*) &bfp;

	//uint64_t avals[nvals];
	//uint64_t bvals[nvals];

	// fill array with input values nvals times.
	//std::fill(avals, avals + nvals, *aptr);
	//std::fill(bvals, bvals + nvals, *bptr);

	// SIMD input gates
	// TODO: Remove SIMDINGate with INGate
	//uint64_t aval = 1;
	//if(role == SERVER)
		//aval = 8589934594;
 
	share* ain = circ->PutINGate(&afp, bitlen, SERVER);
	share* bin = circ->PutINGate(&bfp, bitlen, CLIENT);
	
	//ain = circ2->PutA2BGate(ain, circ1);
	//bin = circ2->PutA2BGate(bin, circ1);

	//ain = left_shift(circ2, ain, prec);
	//bin = left_shift(circ2, bin, prec);

	//ain = circ->PutB2AGate(ain);
	//bin = circ->PutB2AGate(bin);

	share* mul = circ->PutMULGate(ain, bin);

	//mul = circ2->PutA2BGate(mul, circ1);

	//mul = logical_right_shift(circ2, mul, prec);

	for(int i=0; i<999; i++){
		//mul = circ->PutB2AGate(mul);
		mul = circ->PutMULGate(mul, bin);
		//mul = circ2->PutA2BGate(mul, circ1);
		//mul = logical_right_shift(circ2, mul, prec);
	}

	// output gate
	share* mul_out = circ->PutOUTGate(mul, ALL);
	
	//mul_out = logical_right_shift(circ2, mul_out, prec);

	// run SMPC
	party->ExecCircuit();

	// retrieve plain text output
	uint32_t out_bitlen, out_nvals;
	uint64_t *out_vals;

	mul_out->get_clear_value_vec(&out_vals, &out_bitlen, &out_nvals);

	// print every output
	for (uint32_t i = 0; i < nvals; i++) {

		// dereference output value as double without casting the content
		uint64_t val = out_vals[i];

		std::cout << "MUL RES: " << val << " = " << afp << " * " << bfp << " | nv: " << out_nvals
		<< " bitlen: " << out_bitlen << std::endl;
	}

}

int main(int argc, char** argv) {

	e_role role;
	uint32_t bitlen = 1, nvals = 1, secparam = 128, nthreads = 1;

	uint16_t port = 7766;
	std::string address = "127.0.0.1";
	std::string circuit = "none.aby";
	int32_t test_op = -1;
	e_mt_gen_alg mt_alg = MT_OT;
	uint32_t test_bit = 0;
	double fpa = 0, fpb = 0;
	uint64_t aval = 0, bval = 0;

	int prec = 15;

#ifdef FLOAT
	read_test_options(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address,
		&port, &test_op, &test_bit, &circuit, &fpa, &fpb);

	std::cout << std::fixed << std::setprecision(prec); //Set precision
	std::cout << "FLOATING POINT RUNNING: double input values: " << fpa << " ; " << fpb << std::endl;
#else
	read_test_options(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address,
		&port, &test_op, &test_bit, &circuit, &aval, &bval);
	
	//aval = 8589934592;
	//bval = 200;

	//std::cout << std::fixed << std::setprecision(prec); //Set precision
	std::cout << "FIXED POINT RUNNING: uint64_tinput values: " << aval << " ; " << bval << std::endl;

#endif
	seclvl seclvl = get_sec_lvl(secparam);
	

#ifdef FLOAT
	test_floating_point(role, address, port, seclvl, nvals, nthreads, mt_alg, S_BOOL, fpa, fpb);
#else
	test_fixed_point(role, address, port, seclvl, nvals, nthreads, mt_alg, S_ARITH, aval, bval, prec);
#endif
	return 0;
}

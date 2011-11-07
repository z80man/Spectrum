/* =====================================================================
 * instructions.h
 * provides standard routines for SH4A cpu instructions
 * 
 * Copyright 2011 Jack Moore (z80man) Omnimaga CoT Team
 * 
 * Spectrum Prizm emulator project
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *    
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *    
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * ===================================================================*/
 
 //declare registers used in registers.h
 extern unsigned long R[16];
 extern unsigned long SR, GBR, VBR;
 extern unsigned long MACH, MACL, PR;
 extern unsigned long PC;
 
 //bits of SR updated with updateSR()
 extern unsigned long T, M, S, Q;
 void updateSR();
 
 unsigned long tmp0, tmp1;
 
 //SH4A instruction prototypes
 void ADD(unsigned long m, unsigned long n);    //ADD Rm, Rn
 void ADDI(unsigned long i, unsigned long n);   //ADD #imm, Rn
 void ADDC(unsigned long m, unsigned long n);   //ADDC Rm, Rn
 void ADDV(unsigned long m, unsigned long n);   //ADDV Rm, Rn
 void AND(unsigned long m, unsigned long n);    //AND Rm, Rn
 void ANDI(unsigned long i);                    //AND #imm, R0
 void ANDM(unsigned long i);                    //AND.B #imm, @(R0, GBR)
 void BF(unsigned long d);                      //BF label
 void BFSunsigned long d);                      //BF/S label
 
 //instruction code 
 void ADD(unsigned long m, unsigned long n) //ADD Rm, Rn
 {
	 R[n] += R[n];
	 PC += 2;
 }   

 void ADDI(unsigned long i, unsigned long n) //ADD #imm, Rn
 {
	 if (i >= 0)
		R[n] += i;
	 else R[n] += 0xffffff00 | i;
	 PC += 2;
 }
 
 void ADDC(unsigned long m, unsigned long n) //ADDC Rm, Rn
 {
	tmp1 = R[n] + R[m];
	tmp0 = R[n];
	R[n] = tmp1 + T;
	if (tmp0>tmp1) T = 1;
	else T = 0;
	if (tmp1>R[n]) T = 1;
	PC += 2;
 }
 
 void ADDV(unsigned long m, unsigned long n) //ADDV Rm, Rn
 {
	long dest,src,ans;
	if ((long)R[n]>=0) dest = 0;
	else dest = 1;
	if ((long)R[m]>=0) src = 0;
	else src = 1;
	src += dest;
	R[n] += R[m];
	if ((long)R[n]>=0) ans = 0;
	else ans = 1;
	ans += dest;
	if (src==0 || src==2) 
	{
		if (ans==1) T = 1;
		else T = 0;
	}
	else T = 0;
	PC += 2;
 }
 
 void AND(unsigned long m, unsigned long n) //AND Rm, Rn
 {
	 R[n] &= R[m];
	 PC += 2;
 }
 
 void ANDI(unsigned long i) //AND #imm, R0
 {
	 R[0] &= i;
	 PC += 2;
 }

 void ANDM(unsigned long i) //AND.B #imm, @(R0, GBR)
 {
	 Write_Byte(GBR + R[0], (unsigned char)i & ReadByte(GBR + R[0]))
	 PC += 2;
 }
 
 void BF(unsigned long d) //BF Label
 {
	 if (T == 0) PC += 4 + (d << 1);
	 else PC += 2;
 }
 
 void BFS(unsigned long d) //BF/S Label
 {
 	
 }
		
	 
		


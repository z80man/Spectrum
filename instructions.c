/* =====================================================================
 * instructions.c
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
 
 #include "registers.h"
 #include "instructions.h"  //todo : will contain below referenced global variables and prototypes

 
 //bits of SR updated with updateSR()
 extern unsigned long T, M, S, Q;
 void updateSR();
 
 unsigned long tmp0, tmp1;
 long dest, src, ans, imm;
 unsigned int temp;
 unsigned long HH,HL,LH,LL;
 
 //SH4A instruction prototypes
 void ADD(unsigned long m, unsigned long n);    //ADD Rm, Rn
 void ADDI(unsigned long i, unsigned long n);   //ADD #imm, Rn
 void ADDC(unsigned long m, unsigned long n);   //ADDC Rm, Rn
 void ADDV(unsigned long m, unsigned long n);   //ADDV Rm, Rn
 void AND(unsigned long m, unsigned long n);    //AND Rm, Rn
 void ANDI(unsigned long i);                    //AND #imm, R0
 void ANDM(unsigned long i);                    //AND.B #imm, @(R0, GBR)
 void BF(unsigned long d);                      //BF Label
 void BFS(unsigned long d);                     //BF/S Label
 void BRA(unsigned long d);                     //BRA Label
 void BRAF(unsigned long n);                    //BRAF Rn
 void BT(unsigned long d);                      //BT Label
 void BTS(unsigned long d);                     //BTS Label
 void CLRMAC();                                 //CLRMAC
 void CLRS();                                   //CLRS
 void CLRT();                                   //CLRT
 void CMPEQ(unsigned long m, unsigned long n);  //CMP_EQ Rm, Rn
 void CMPGE(unsigned long m, unsigned long n);  //CMP_GE Rm, Rn
 void CMPGT(unsigned long m, unsigned long n);  //CMP_GT Rm, Rn
 void CMPHI(unsigned long m, unsigned long n);  //CMP_HI Rm, Rn
 void CMPHS(unsigned long m, unsigned long n);  //CMP_HS Rm, Rn
 void CMPIM(unsigned long i);                   //CMP_EQ #imm,R0
 void CMPPL(unsigned long n);                   //CMP_PL Rn
 void CMPPZ(unsigned long n);                   //CMP_PZ Rn
 void CMPSTR(unsigned long m, unsigned long n); //CMP_STR Rm, Rn
 
 //instruction code 
 void ADD(unsigned long m, unsigned long n) //ADD Rm, Rn  //add Rm and Rn into Rn; unsigned and signed data
 {
	 R[n] += R[n];
	 PC += 2;
 }   

 void ADDI(unsigned long i, unsigned long n) //ADD #imm, Rn  //add signed immediate to Rn
 {
	 if (i >= 0)
		R[n] += i;
	 else R[n] += 0xffffff00 | i;
	 PC += 2;
 }
 
 void ADDC(unsigned long m, unsigned long n) //ADDC Rm, Rn  : unsigned addition of Rn, Rm, and T bit into Rn; check for carry
 {
	tmp1 = R[n] + R[m];
	tmp0 = R[n];
	R[n] = tmp1 + T;
	if (tmp0>tmp1) T = 1;
	else T = 0;
	if (tmp1>R[n]) T = 1;
	PC += 2;
 }
 
 void ADDV(unsigned long m, unsigned long n) //ADDV Rm, Rn  : signed addition of Rn and Rm into Rn; check for overflow
 {
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
 
 void AND(unsigned long m, unsigned long n) //AND Rm, Rn  //bitwise 'and Rn with Rm into Rn
 {
	 R[n] &= R[m];
	 PC += 2;
 }
 
 void ANDI(unsigned long i) //AND #imm, R0  : bitwise 'and' immediate with R0
 {
	 R[0] &= i;
	 PC += 2;
 }

 void ANDM(unsigned long i) //AND.B #imm, @(R0, GBR)  : bitwise 'and' immediate with byte at GBR + R0
 {
	 Write_Byte(GBR + R[0], (unsigned char)i & ReadByte(GBR + R[0]))
	 PC += 2;
 }
 
 void BF(unsigned long d) //BF Label  : if false 8 bit disp jump
 {
	if ((d&0x80)==0)
	disp = (0x000000FF & d);
	else
	disp = (0xFFFFFF00 | d);
	if (T==0)
	PC = PC+4+(disp<<1);
	else
	PC += 2;
 }
 
 void BFS(unsigned long d) //BF/S Label  : if false 8 bit disp jump with delay slot
 {
	temp = PC;
	if ((d&0x80)==0)
	disp = (0x000000FF & d);
	else
	disp = (0xFFFFFF00 | d);
	if (T==0)
	PC = PC + 4 + (disp<<1);
	else PC += 4;
	Delay_Slot(temp+2);
 }
 
 void BRA(unsigned long d) //BRA Label  : 12 bit disp jump with delay slot
 {
	temp = PC;
	if ((d&0x800)==0)
	disp = (0x00000FFF & d);
	else
	disp = (0xFFFFF000 | d);
	PC = PC + 4 + (disp<<1);
	Delay_Slot(temp+2);
 }
 
 void BRAF(unsigned long n) //BRAF Rn  : jump to Rn with delay slot
 {
	temp = PC;
	PC = PC + 4 + R[n];
	Delay_Slot(temp+2);
 }
 
 void BT(unsigned long d) //BT Label  : if true 8 bit disp jump
 {
	if ((d&0x80)==0)
	disp = (0x000000FF & d);
	else disp = (0xFFFFFF00 | d);
	if (T==1)
	PC = PC + 4 + (disp<<1);
	else PC += 2;
 }
 
 void BTS(unsigned long d) //BTS Label  : if true 8 bit disp jump with delay slot
 {
	temp = PC;
	if ((d&0x80)==0)
	disp = (0x000000FF & d);
	else disp = (0xFFFFFF00 | d);
	if (T==1)
	PC = PC + 4 + (disp<<1);
	else PC += 4;
	Delay_Slot(temp+2);
 }
 
 void CLRMAC() //CLRMAC  //clear the MAC register
 {
	MACL = MACH = 0;
	PC += 2;
 }
 
 void CLRS() //CLRS  : clear the S bit of the SR register
 {
	S = 0;
	PC += 2;
 }
 
  void CLRT() //CLRT  : clear the T bit or the SR register
 {
	 T = 0;
	 PC += 2;
 }
 
 CMPEQ(unsigned long m, unsigned long n) //CMP_EQ Rm,Rn  : if Rn is == Rm; unsigned and signed
 {
	if (R[n]==R[m]) T = 1;
	else T = 0;
	PC += 2;
 }
 
 CMPGE(unsigned long m, unsigned long n) //CMP_GE Rm,Rn  : if Rn is >= Rm; signed data
 {
	if ((long)R[n] >= (long)R[m]) T = 1;  //sign type cast
	else T = 0;
	PC += 2;
 }
 
 CMPGT(unsigned long m, unsigned long n) //CMP_GT Rm,Rn  : if Rn is > Rm; signed data
 {
	if ((long)R[n] > (long)R[m]) T = 1; //sign type cast
	else T = 0;
	PC += 2;
 }

 CMPHI(unsigned long m, unsigned long n) //CMP_HI Rm,Rn  : if Rn is > Rm; unsigned data
 {
	if (R[n] > R[m]) T = 1;  //no type cast due to unsigned default
	else T = 0;
	PC += 2;
 }

 CMPHS(unsigned long m, unsigned long n) //CMP_HS Rm,Rn  : if Rn is >= Rm; unsigned data
 {
	if (R[n] >= R[m]) T = 1;  //no type cast due to unsigned default
	else T = 0;
	PC += 2;
 }
 
 CMPIM(unsigned long i) //CMP_EQ #imm,R0  : if immediate is equal to R0; signed data
 {
	if ((i&0x80)==0) imm=(0x000000FF & (long i));
	else imm=(0xFFFFFF00 | (long i));
	if (R[0]==imm) T = 1;
	else T = 0;
	PC += 2;
 }

 CMPPL(unsigned long n) //CMP_PL Rn  : if Rn > 0; signed data
 {
	if ((long)R[n]>0) T = 1;  //sign type cast
	else T = 0;
	PC += 2;
 }

 CMPPZ(unsigned long n) //CMP_PZ Rn  : if Rn >= 0; signed data
 {
	if ((long)R[n]>=0) T = 1;  //sign type cast
	else T = 0;
	PC += 2;
 }
 
 CMPSTR(unsigned long m, unsigned long n) //CMP_STR Rm,Rn  : if a byte of Rn is == a byte of Rm; unsigned and signed data
 {
	temp=R[n]^R[m];
	HH = (temp & 0xFF000000) >> 24;
	HL = (temp & 0x00FF0000) >> 16;
	LH = (temp & 0x0000FF00) >> 8;
	LL = temp & 0x000000FF;
	HH = HH && HL && LH && LL;
	if (HH==0) T = 1;
	else T = 0;
	PC += 2;
 }

 

 




		
	 
		


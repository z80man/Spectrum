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

 //declare registers used in registers.h
 extern unsigned long R[16];
 extern unsigned long SR, GBR, VBR;
 extern unsigned long MACH, MACL, PR;
 extern unsigned long PC;
 
 //bits of SR updated with updateSR()
 extern unsigned long T, M, S, Q;
 void updateSR();
 
 unsigned long tmp0, tmp1;
 long dest, src, ans;
 unsigned int temp;
 
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
	if ((d&0x80)==0)
	disp = (0x000000FF & d);
	else
	disp = (0xFFFFFF00 | d);
	if (T==0)
	PC = PC+4+(disp<<1);
	else
	PC += 2;
 }
 
 void BFS(unsigned long d) //BF/S Label
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
 
 void BRA(unsigned long d) //BRA Label
 {
	temp = PC;
	if ((d&0x800)==0)
	disp = (0x00000FFF & d);
	else
	disp = (0xFFFFF000 | d);
	PC = PC + 4 + (disp<<1);
	Delay_Slot(temp+2);
 }
 
 void BRAF(unsigned long n) //BRAF Rn
 {
	temp = PC;
	PC = PC + 4 + R[n];
	Delay_Slot(temp+2);
 }
 
 void BT(unsigned long d) //BT Label
 {
	if ((d&0x80)==0)
	disp = (0x000000FF & d);
	else disp = (0xFFFFFF00 | d);
	if (T==1)
	PC = PC + 4 + (disp<<1);
	else PC += 2;
 }
 
 void BTS(unsigned long d) //BTS Label
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
 
 void CLRMAC() //CLRMAC
 {
	MACL = MACH = 0;
	PC += 2;
 }
 
 void CLRS() //CLRS
 {
	S = 0;
	PC += 2;
 }
 
 CMPEQ(unsigned long m, unsigned long n) //CMP_EQ Rm,Rn
 {
	if (R[n]==R[m]) T = 1;
	else T = 0;
	PC += 2;
 }
 
 CMPGE(long m, long n) //CMP_GE Rm,Rn
 {
	if ((long)R[n]>=(long)R[m]) T = 1;
	else T = 0;
	PC += 2;
 }
 
 CMPGT(long m, long n) //CMP_GT Rm,Rn
 {
	if ((long)R[n]>(long)R[m]) T = 1;
	else T = 0;
	PC += 2;
 }

 
 void CLRT() //CLRT
 {
	 T = 0;
	 PC += 2;
 }
 
 CMPHI(long m, long n) //CMP_HI Rm,Rn
 {
	if ((unsigned long)R[n]>(unsigned long)R[m]) T = 1;
	else T = 0;
	PC += 2;
 }

 CMPHS(long m, long n) //CMP_HS Rm,Rn
 {
	if ((unsigned long)R[n]>=(unsigned long)R[m]) T = 1;
	else T = 0;
	PC += 2;
 }
CMPPL(long n)
/* CMP_PL Rn */
{
if ((long)R[n]>0) T = 1;
else T = 0;
PC += 2;
}
CMPPZ(long n)
/* CMP_PZ Rn */
{
if ((long)R[n]>=0) T = 1;
else T = 0;
PC += 2;
}
CMPSTR(long m, long n)
{
unsigned long temp;
long HH,HL,LH,LL;
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
CMPIM(long i)
/* CMP_EQ #imm,R0 */
{
long imm;
if ((i&0x80)==0) imm=(0x000000FF & (long i));
else imm=(0xFFFFFF00 | (long i));
if (R[0]==imm) T = 1;
else T = 0;
PC += 2;
}

 




		
	 
		


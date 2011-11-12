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
 #include "memory.h"        //todo : will contain prototypes for memory functions

  
 unsigned long tmp0, tmp1, tmp2;
 unsigned long long tmp64;
 long dest, src, ans, imm;
 unsigned int temp;
 unsigned long HH,HL,LH,LL;
 unsigned char old_q;
 
 //SH4A instruction prototypes
 void ADD(unsigned long m, unsigned long n);        //ADD Rm, Rn
 void ADDI(unsigned long i, unsigned long n);       //ADD #imm, Rn
 void ADDC(unsigned long m, unsigned long n);       //ADDC Rm, Rn
 void ADDV(unsigned long m, unsigned long n);       //ADDV Rm, Rn
 void AND(unsigned long m, unsigned long n);        //AND Rm, Rn
 void ANDI(unsigned long i);                        //AND #imm, R0
 void ANDM(unsigned long i);                        //AND.B #imm, @(R0, GBR)
 void BF(unsigned long d);                          //BF Label
 void BFS(unsigned long d);                         //BF/S Label
 void BRA(unsigned long d);                         //BRA Label
 void BRAF(unsigned long n);                        //BRAF Rn
 void BT(unsigned long d);                          //BT Label
 void BTS(unsigned long d);                         //BTS Label
 void CLRMAC();                                     //CLRMAC
 void CLRS();                                       //CLRS
 void CLRT();                                       //CLRT
 void CMPEQ(unsigned long m, unsigned long n);      //CMP_EQ Rm, Rn
 void CMPGE(unsigned long m, unsigned long n);      //CMP_GE Rm, Rn
 void CMPGT(unsigned long m, unsigned long n);      //CMP_GT Rm, Rn
 void CMPHI(unsigned long m, unsigned long n);      //CMP_HI Rm, Rn
 void CMPHS(unsigned long m, unsigned long n);      //CMP_HS Rm, Rn
 void CMPIM(unsigned long i);                       //CMP_EQ #imm,R0
 void CMPPL(unsigned long n);                       //CMP_PL Rn
 void CMPPZ(unsigned long n);                       //CMP_PZ Rn
 void CMPSTR(unsigned long m, unsigned long n);     //CMP_STR Rm, Rn
 void DIV0S(unsigned long m, unsigned long n);      //DIV0S Rm, Rn
 void DIV0U();                                      //DIV0U
 void DIV1(unsigned long m, unsigned long n);       //DIV1 Rm, Rn
 void DMULS(unsigned long m, unsigned long n);      //DMULS.L Rm, Rn
 void DMULU(unsigned long m, unsigned long n);      //DMULU.L Rm, Rn
 void DT(unsigned long n);                          //DT Rn
 void EXTSB(unsigned long m, unsigned long n);      //EXTS.B Rm, Rn
 void EXTSW(unsigned long m, unsigned long n);      //EXTS.W Rm, Rn
 void EXTUB(unsigned long m, unsigned long n);      //EXTU.B Rm, Rn
 void EXTUW(unsigned long m, unsigned long n);      //EXTU.W Rm, Rn
 void ICBI(unsigned long n);                        //ICBI @Rn
 void JMP(unsigned long n);                         //JMP @Rn
 void LDCGBR(unsigned long m);                      //LDC Rm, GBR
 void LDCVBR(unsigned long m);                      //LDC Rm, VBR
 void LDCSGR(unsigned long m);                      //LDC Rm, SGR
 void LDCSSR(unsigned long m);                      //LDC Rm, SSR
 void LDCSPC(unsigned long m);                      //LDC Rm, SPC
 void LDCDBR(unsigned long m);                      //LDC Rm, DBR
 void LDC_BANK(unsigned long m, unsigned long n);   //LDC Rm, Rn_BANK
 void LDCMGBR(unsigned long m);                     //LDC.L @Rm+, GBR
 void LDCMVBR(unsigned long m);                     //LDC.L @Rm+, VBR
 void LDCMSGR(unsigned long m);                     //LDC.L @Rm+, SGR
 void LDCMSSR(unsigned long m);                     //LDC.L @Rm+, SSR
 void LDCMSPC(unsigned long m);                     //LDC.L @Rm+, SPC
 void LDCMDBR(unsigned long m);                     //LDC.L @Rm+, DBR
 void LDCM_BANK(unsigned long m, unsigned long n);  //LDC.L @Rm+, Rn_BANK
 
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

 void DIV0S(unsigned long m, unsigned long n)  //DIV0S Rm, Rn  : initialization of signed division
 {
	if ((R[n] & 0x80000000)==0) Q = 0;
	else Q = 1;
	if ((R[m] & 0x80000000)==0) M = 0;
	else M = 1;
	T = !(M==Q);
	PC += 2;
 }
 
 void DIV0U ()  //DIV0U  : initialization of unsigned division
 {
	M = Q = T = 0;
	PC += 2;
 }
 
 void DIV1(unsigned long m, unsigned long n) //DIV1 Rm, Rn  : single step division; sign set by either DIV0U or DIV0S
 {
	static unsigned char tmp1;
	old_q = Q;
	Q = (unsigned char)((0x80000000 & R[n])!=0);
	tmp2 = R[m];
	R[n] <<= 1;
	R[n] |= (unsigned long)T;
	switch(old_q){
	case 0:switch(M){
		case 0:tmp0 = R[n];
			R[n] -= tmp2;
			tmp1 = (R[n]>tmp0);
			switch(Q){
			case 0:Q = tmp1;
				break;
			case 1:Q = (unsigned char)(tmp1==0);
				break;
			}
			break;
		case 1:tmp0 = R[n];
			R[n] += tmp2;
			tmp1 = (R[n]<tmp0);
			switch(Q){
			case 0:Q = (unsigned char)(tmp1==0);
				break;
			case 1:Q = tmp1;
				break;
			}
			break;
		}
		break;
	case 1:switch(M){
		case 0:tmp0 = R[n];
			R[n] += tmp2;
			tmp1 = (R[n]<tmp0);
			switch(Q){
			case 0:Q = tmp1;
				break;
			case 1:Q = (unsigned char)(tmp1==0);
				break;
			}
			break;
		case 1:tmp0 = R[n];
			R[n] -= tmp2;
			tmp1 = (R[n]>tmp0);
			switch(Q){
			case 0:Q = (unsigned char)(tmp1==0);
				break;
			case 1:Q = tmp1;
				break;
			}
			break;
		}
		break;
	}
	T = (Q==M);
	PC += 2;
 }
 
 void DMULS (unsigned long m, unsigned long n)  //DMULS.L Rm, Rn  : 32 bit * 32 bit = 64 bit signed multiplication
 {
	tmp64 = (long long)R[m] * (long long)R[n];  //64 bit signed casting
	MACL = tmp64 & 0x00000000ffffffffLL;
	MACH = tmp64 >> 32;
	PC += 2;
 }
 
 void DMULU (unsigned long m, unsigned long m)  //DMULU.L Rm, Rn  : 32 bit * 32 bit = 64 bit unsigned multiplication
 {
	tmp64 = (unsigned long long)R[m] * (unsigned long long)R[n];  //64 bit unsigned casting
	MACL = tmp64 & 0x00000000ffffffffLL;  //lowercase l looks to much like a 1 imho
	MACH = tmp64 >> 32;
	PC += 2;
 }
 
 void DT (unsigned long n)  //DT Rn  : decrement and test if 0
 {
	--R[n];  //prefix decrement optimization ftw!!
	if (R[n] == 0) T = 1;
	else T = 0;
	PC += 2;
 }
 
 void EXTSB (unsigned long m, unsigned long n)  //EXTS.B Rm, Rn  : type cast Rm to signed byte and write to Rn
 {
	R[n] = (unsigned long)(char)(((long)R)[m]);  //pointer to signed long; result to signed char to unsigned long
	PC += 2;
 }
 
 void EXTSW (unsigned long m, unsigned long n)  //EXTS.W Rm, Rn  : type cast Rm to signed word and write to Rn
 {
	R[n] = (unsigned long)(short)(((long)R)[m]);  //pointer to signed long; result to signed short to unsigned long
	PC += 2;
 }
 
 void EXTUB (unsigned long m, unsigned long n)  //EXTU.B Rm, Rn  : type cast Rm to unsigned byte and write to Rn
 {
	R[n] = (unsigned long)(unsigned char)R[m];  //to unsigned char then type cast back to register
	PC += 2;
 }
 
 void EXTUW (unsigned long m, unsigned long n)  //EXTU.W Rm, Rn  : type cast Rm to unsigned word and write to Rn
 {
	R[n] = (unsigned long)(unsigned short)R[m];  //to unsigned short then type cast back to register
	PC += 2;
 }
 
 void ICBI (unsigned long n)  //ICBI @Rn  : invalidiate instruction cache @Rn; does nothing in this context as cache is disabled in standard mode
 {
	PC += 2;  //aka NOP
 }
 
 void JMP (unsigned long n)  //JMP @Rn  : jump with delay branch to Rn
 {
	temp = PC;
	PC = R[n];
	Delay_Slot(temp + 2);
 }
 
 void LDCGBR (unsigned long m)  //LDC Rm, GBR  : load Rm into GBR  : Global Base Register
 {
	GBR = R[m];
	PC += 2;
 }
 
 void LDCVBR (unsigned long m)  //LDC Rm, VBR  : load Rm into VBR  : Vector Base Register; ignore privileged status
 {
	VBR = R[m];
	PC += 2;
 }
 
 void LDCSGR (unsigned long m)  //LDC Rm, SGR  : load Rm into SGR  : saved stack pointer; ignore privileged status
 {
	SGR = R[m];
	PC += 2;
 }
 
 void LDCSSR (unsigned long m)  //LDC Rm, SSR  : load Rm into SSR  : Saved Status Register; ignore privileged status
 {
	SSR = R[m];
	PC += 2;
 }
 
 void LDCSPC (unsigned long m)  //LDC Rm, SPC  : load Rm into SPC  : Saved Program Counter; ignore privileged status
 {
	SPC = R[m];
	PC += 2;
 }
 
 void LDCDBR (unsigned long m)  //LDC Rm, DBR  : load Rm into DBR  : Debug Base Register; ignore privileged status
 {
	DBR = R[m];
	PC += 2;
 }
 
 void LDC_BANK (unsigned long m, unsigned long n)  //LDC Rm, Rn_BANK  : load Rm into Rn_BANK; ignore privileged status
 {
	R_bank[n] = R[m];  //may need to separate into 8 functions for consistency with decoder
	PC += 2;
 }
 
 void LDCMGBR (unsigned long m)  //LDC.L @Rm+, GBR  : pop what is @Rm and into the GBR
 {
	GBR = Read_Long(R[m]);
	R[m] += 4;
	PC += 2;
 }
 
 void LDCMVBR (unsigned long m)  //LDC.L @Rm+, VBR  : pop what is @Rm and into the VBR; ignore privileged status
 {
	VBR = Read_Long(R[m]);
	R[m] += 4;
	PC += 2;
 }
 
 void LDCMSGR (unsigned long m)  //LDC.L @Rm+, SGR  : pop what is @Rm and into the SGR; ignore privileged status
 {
	SGR = Read_Long(R[m]);
	R[m] += 4;
	PC += 2;
 }
 
 void LDCMSSR (unsigned long m)  //LDC.L @Rm+, SSR  : pop what is @Rm and into the SSR; ignore privileged status
 {
	SSR = Read_Long(R[m]);
	R[m] += 4;
	PC += 2;
 }
 
 void LDCMSPC (unsigned long m)  //LDC.L @Rm+, SPC  : pop what is @Rm and into the SPC; ignore privileged status
 {
	SPC = Read_Long(R[m]);
	R[m] += 4;
	PC += 2;
 }
 
 void LDCMDBR (unsigned long m) //LDC.L @Rm+, DBR  : pop what is @Rm and into the DBR; ignore privileged status
 {
	DBR = Read_Long(R[m]);
	R[m] += 4;
	PC += 2;
 }
 
 void LDCM_BANK (unsigned long m, unsigned long n)  //LDC.L @Rm+, Rn_BANK  : pop what is @Rm and into Rn_BANK; ignore privileged status
 {
	R_Bank[n] = Read_Long(R[m]);  //may need to separate into 8 functions for consistency with decoder
	R[m] += 4;
	PC += 2;
 }
	
   
 
 
	




 
 
 

 




		
	 
		


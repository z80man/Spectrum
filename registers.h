 /* =====================================================================
 * registers.h
 * provides declarations for SH4A register set
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
 
 unsigned long R[16];
 unsigned long SR, GBR, VBR, SGR, SPC, SSR;
 unsigned long PR;
 unsigned long PC;
 unsigned long R_Bank[8];
 unsigned long T, M, S, Q;
 unsigned long long = MAC64;
 const unsigned long * MACH = ((char*)&MACH64 + 4);  //seems correct for a little endian system
 const unsigned long * MACL = &MACH64;
 



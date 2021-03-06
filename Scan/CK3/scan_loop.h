/* Copyright (C) 2014-2016 by Jacob Alexander, Crystal Hammer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

// ----- Includes -----

// Compiler Includes
#include <stdint.h>



// ----- Functions -----

// Functions to be called by main.c
void Scan_setup( void );
uint8_t Scan_loop( void );

// Call-backs
void Scan_finishedWithMacro( uint8_t sentKeys );  // Called by Macro Module
void Scan_finishedWithOutput( uint8_t sentKeys ); // Called by Output Module

void Scan_currentChange( unsigned int current ); // Called by Output Module


// ----- Capabilities -----

#define key(k)   (kk[KEY_##k] && !kko[KEY_##k])   //  key pressed macro
#define keyp(k)  (kk[KEYPAD_##k] && !kko[KEYPAD_##k])

extern uint8_t kk[0xFF], kko[0xFF];  //  all keys state, old state, 0,1

extern int8_t id_seq;  // sequence id, to execute
extern int8_t shift;  // mouse slow modifier

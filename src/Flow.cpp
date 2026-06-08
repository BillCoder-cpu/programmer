//*/
/*
//
//					Flow.cpp : 
//	
//   This source file is part of programmer, a coder's utility program.
//		
//	Author: William P. Foster (bill@software0imagination.com)
//
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 William P. Foster
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

//*/
/*

NOTE!!!: This will only work on pure 7 bit data, such as all ASCII text.

*/
#include "stdafx.h"
#include <BFC/BFC.h>
using namespace CBfc;

typedef struct {
	char code;
	char value_1, value_2;
} ReplaceKey;


class BytePairEncoding {
public:
	int Compress (const char *data, const unsigned int length, char **out_pp);	// returns bytecount of new data
	char *DeCompress (const char *data);
private:
	bool				FindCommonPair();
	void				CompressKey ();

	int							m_length;
	char							*m_data , *m_buffer;
	ReplaceKey				m_tempkey;
	Array<ReplaceKey>	m_keys;
};

int BytePairEncoding::Compress (const char *data, const unsigned int length, char **out_pp)
{
	m_keys.Empty();
	uchar cNextKeycode = 0x80;
	m_data = new char[length];
	m_buffer = new char[length];		// temp storage used in CompressKey
	memcpy (m_data, data, length);
	m_length = length;

	while (cNextKeycode < 0xff) {
		if (!FindCommonPair ()) break;
		m_tempkey.code = cNextKeycode++;
		m_keys.Add(&m_tempkey);
		CompressKey ();
	}
	return 1;
}

bool BytePairEncoding::FindCommonPair()
{
	m_tempkey.value_1 = 1;
	return 0;
}

// Called to reduce data during compression
void BytePairEncoding::CompressKey ()
{
/*	char *cp = data;
	int i = 0;
	while (i < m_length)
	*/
}

char *BytePairEncoding::DeCompress (const char *data)
{
	return NULL;
}



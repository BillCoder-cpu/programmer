//*/
/*
//
//					AIFeeder.cpp :
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

//
#include "stdafx.h"
#include "AIFeeder.h"

using namespace CBfcScript;

AIFeeder::AIFeeder(const Filename& prompt_fname)
	: m_prompt_fname(prompt_fname)
{
}

int	AIFeeder::Operate(const Filename& fname)	// part of FileOperationClass
{
	return GeneratePrompt(fname);
}

int AIFeeder::GeneratePrompt(const Filename& fname)
{
	Script_Text	s_prompt(m_prompt_fname);
	Script_Text	s_outfile;
	CBfc_String	s_projectName;			// Blank for now

	int prompt_max_length = s_prompt.FileLength() + 4096;
	char* p_memory = (char *)malloc(prompt_max_length);
	s_outfile.AttachMemory(p_memory, prompt_max_length);

	_tprintf(_TXT("Gen prompt for file: %ls.\n"), fname.GetPtr());

	//    inline bool Write (void *srcMem, const unsigned int uCount, unsigned int *p_uBytesWritten=NULL)
	UINT	u_BytesPerCharacter = s_prompt.DetectBytesPerCharacter();
	s_outfile.SetBytesPerCharacter(u_BytesPerCharacter);

	CBfc::StringArray	substitute_array;
	CBfc::StringArray	replace_array;

	CBfc_String  csFN(_TXT("!current_filename"));
	CBfc_String  csPN(_TXT("!project_name"));

	CBfc_String csNameEXT = fname.GetNameExtension();

	substitute_array.Add(&csFN);
	replace_array.Add(&csNameEXT);
	substitute_array.Add(&csPN);
	replace_array.Add(&s_projectName);

	if (s_outfile.AppendStreamWithSubstitution(s_prompt, substitute_array, replace_array))
	{
		s_outfile.Close();
		// now copy the memory stream to the clipboard.

		return true;
	}
	return false;
}


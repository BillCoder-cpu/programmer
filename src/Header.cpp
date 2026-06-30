//*/
/*
//
//					Header.cpp : 
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
//   C++ Header has this format
//
//*/


//
#include "stdafx.h"
#include "Header.h"

/// PLATFORM DEPENDENT!!!!!!!!!!!!
#define	UP_FOLDER_PREPEND			"../";

using namespace CBfcScript;

Header::Header(const Filename &header_fname, const bool b_phantomfolder)
	: m_Language(C_FAMILY)
	, m_header_fname(header_fname)
	, m_b_phantomfolder(b_phantomfolder)
{
}
void Header::SetLanguageBasedOnFiletype(const Filename &fname)
{
	CBfc_String cs_path, cs_name, cs_ext;

	fname.GetComponents (cs_path, cs_name, cs_ext);
	m_Language = C_FAMILY;			// Default to C++ type files
	if (cs_ext.CompareNoCase(_TXT("PHP")) == 0)
		m_Language = PHP;
}

int	Header::Operate(const Filename &fname)	// part of FileOperationClass
{
	return SetHeader(fname);
}


///  Find the Header file and attach it to the argument script.
///
/// If phantom folder is active, look up the directory tree until it finds the file
bool Header::AttachHeader(Script_Text &script)
{
	if (script.IsAttached())
		return true;
	script.SetSkippingLinefeeds(true);		// Don't feed us linefeeds during ReadLine()
	if (m_b_phantomfolder)
	{
		STRING	prepend;
		
		int count=4;		// go up a few - need to regulate this!!!!!!!!!!!
		do {
			Filename	fn(m_header_fname);
			prepend += UP_FOLDER_PREPEND;
			fn.SetPath (prepend);
			script.AttachFile(fn);
			if (script.IsAttached())
				return true;
		} while (count--);
	}			
	return false;
}


int Header::SetHeader (const Filename &fname)
{
	Script_Text	s_header(m_header_fname);

	if (!AttachHeader (s_header))
	{
		  _tprintf (_TXT("Cannot locate Header file: %ls\n"), m_header_fname.GetPtr());
		return false;
	}
	Filename	temp_fname(_TXT("t.fn"));
	Script_Text	s_outfile(temp_fname, false);
	Script_Text	s_infile(fname);

	CBfc_String		s_projectName;		// Blank for now

	_tprintf (_TXT("Set Header on file: %ls.\n"), fname.GetPtr());

//    inline bool Write (void *srcMem, const unsigned int uCount, unsigned int *p_uBytesWritten=NULL)
	UINT	u_BytesPerCharacter = s_infile.DetectBytesPerCharacter();
	s_outfile.SetBytesPerCharacter (u_BytesPerCharacter);

	SetLanguageBasedOnFiletype (fname);

	CBfc::StringArray	substitute_array;
	CBfc::StringArray	replace_array;

	CBfc_String  csFN (_TXT("!current_filename"));
	CBfc_String  csPN (_TXT("!project_name"));
	
	CBfc_String csEXT = fname.GetNameExtension();
	
	substitute_array.Add(&csFN);
	replace_array.Add(&csEXT);
	substitute_array.Add(&csPN);
	replace_array.Add(&s_projectName);

	CBfc_String		cs(GetHeaderStart());
	s_outfile.WriteLine (cs);
	if (s_outfile.AppendStreamWithSubstitution (s_header, substitute_array, replace_array))
	{
		cs = GetHeaderEnd();
		s_outfile.WriteLine (cs);

		SkipHeader (s_infile);
		s_outfile.AppendStream(s_infile);
		s_outfile.Close();
		s_infile.Close();
		Filename	backup_fname(fname);
		backup_fname.SetExtention (CBfc_String(_TXT("BAK")));
		backup_fname.UnLink();
		Filename::Rename (fname, backup_fname);
		Filename::Rename (temp_fname, fname);
		return true;
	}
	return false;
}

// Read the first part of the file, and skip to the end of the current header
//  leave the stream positioned just after it's header, if any.
// Returns true if a header was detected, else false.
bool Header::SkipHeader (Script_Text &s)
{
	__uint64			header_pos = 0;
	Script_Text			ts(s);
	CBfc_String			cs;
	const unsigned long	ulOrigin = 0;
	bool				b_BadHeader = false, b_HeaderFound=false;

	ts.SetSkippingLinefeeds(true);	// Don't feed us linefeeds in ReadLine()
	do
	{
		if (!ts.ReadLine (cs))
		{
			cs.Empty();
			break;
		}
	} while (cs.GetLength() == 0);
	if (RecognizeHeaderStart(cs))
	{
		b_HeaderFound = b_BadHeader = true;
		while (ts.ReadLine (cs))
		{
			if (RecognizeHeaderEnd(cs))
			{
				header_pos = ts.Tell();
				b_BadHeader = false;
				break;
			}
		}
	}
	s.Seek(header_pos, ulOrigin);
//	_tprintf (_TXT("Located Header:%d, Bad:%d (string %ls)\n"), b_HeaderFound, b_BadHeader, cs.GetPtr());
	return b_HeaderFound;
}

character	*Header::GetHeaderStart()
{
	character	*hs = NULL;
	switch (m_Language)
	{
		case C_FAMILY:
			hs = _TXT("//*/");
			break;
		case PHP:
			hs = _TXT("<?php /*");
			break;
	}
	return hs;
}

character	*Header::GetHeaderEnd()
{
	character	*he = NULL;
	switch (m_Language)
	{
		case C_FAMILY:
			he = _TXT("//*/");
			break;
		case PHP:
			he = _TXT("*/?>");
			break;
	}
	return he;
}

bool Header::RecognizeHeaderStart (CBfc_String &cs)
{
	character	*looks=GetHeaderStart();
	if (looks)
	{
		int length = wcslen(looks);
		if (cs.GetLengthChars() == length && cs.Compare(looks)==0)
			return true;
	}
	return false;
}

bool Header::RecognizeHeaderEnd (CBfc_String &cs)
{
	character	*looks=GetHeaderEnd();

	if (looks)
	{
		int length = wcslen(looks);
		if (cs.GetLengthChars() == length && cs.Compare(looks)==0)
			return true;
	}
	return false;
}






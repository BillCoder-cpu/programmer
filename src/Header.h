//*/
/*
//
//					Header.h : 
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


using namespace CBfc;

typedef enum {
	C_FAMILY,
	PHP,
	HTML
} LANGUAGE;


class Header : public CBfcOS::FileOperationClass
{
public:
	Header(const Filename &header_fname, const bool b_header_phantomfolder=false);
	virtual int	Operate(const Filename &fname);	// part of FileOperationClass

	int SetHeader (const Filename &fname);
private:
	bool AttachHeader(CBfcScript::Script_Text &script);
	bool SkipHeader (CBfcScript::Script_Text &s);
	bool RecognizeHeaderStart (STRING &cs);
	bool RecognizeHeaderEnd (STRING &cs);
	void SetLanguageBasedOnFiletype(const Filename &fname);

	character	*GetHeaderStart();
	character	*GetHeaderEnd();

	LANGUAGE		m_Language;
	Filename		m_header_fname;
	bool			m_b_phantomfolder;
};


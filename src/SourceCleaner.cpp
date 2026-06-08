//*/
/*
//
//					SourceCleaner.cpp : 
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

#include "stdafx.h"
#include "SourceCleaner.h"

/*	
	Todo :
			remove debug and release folders.
				remove .o files

*/

SourceCleaner::SourceCleaner()
{
}

#ifdef WINDOWS
#	define ALL_WILD	_TXT("*.*")
#else
#	define ALL_WILD	_TXT("*")
#endif
// Traverse each file and find it's dependencies (search for #include)
// Generate a makefile listing all of the .cpp files (-+that don't end in .inc.cpp), with make rules
int SourceCleaner::Operate (const Filename &folder)
{						// baseFile.SetPath (folder);
	Filename fname(folder.GetName());
	_tprintf (_TXT("  Cleaning folder %ls\n"), folder.GetPtr());
	
	// If this folder is named Release or Debug, delete all files else, delete *.o and *~
	if (!fname.CompareNoCase(_TXT("debug")) || !fname.CompareNoCase(_TXT("release")))
	{
		Filename	Del(ALL_WILD);
		Del.SetPath (folder);
		// delete all files in this folder
		DeleteFilespec(Del);
	} else
	{
		// delete *.o from this folder
		Filename	Del(_TXT("*.o"));
		Del.SetPath (folder);
		DeleteFilespec(Del);
	
		// delete *~ from this folder
		Filename	Del2(_TXT("*~"));
		Del2.SetPath (folder);
		DeleteFilespec(Del2);
		
		// delete *.BAK from this folder
		Filename	Del3(_TXT("*.BAK"));
		Del3.SetPath (folder);
		DeleteFilespec(Del3);
	}
	return true;
}

void SourceCleaner::DeleteFilespec(Filename &cs_TargetFilespec)
{
	DelFiles	df;

	_tprintf (_TXT("  Deletefile: %ls\n"), cs_TargetFilespec.GetPtr());
	CBfcOS::FileOperation	fop(cs_TargetFilespec, false);
	fop.Operate(&df);
}

int DelFiles::Operate(const Filename &fname)
{
//	_tprintf (_TXT(" delete file %ls\n"), fname.GetPtr());
	return fname.UnLink();
}



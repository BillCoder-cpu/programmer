//*/
/*
//
//					BackupSources.cpp : 
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
#include "BackupSources.h"


BackupSources::BackupSources()
{
	m_BackupFolder = "/home/bill/Backup";
}

// Traverse each file and find it's dependencies (search for #include)
// Generate a makefile listing all of the .cpp files (-+that don't end in .inc.cpp), with make rules
int BackupSources::Operate (const Filename &folder_const)
{						// baseFile.SetPath (folder);
	Filename folder(folder_const);
	CBfcOS::OS_Static::MakePathAbsolute (folder);
	
	Filename base = m_BackupFolder.GetPath();
	
	m_archive_name = folder;
	m_archive_name.TrimLeft (base);
	m_archive_name.PrependPath(m_BackupFolder);
	m_archive_name.RemoveTrailingSlash();
	m_archive_name += ".tar";
	
	Filename fname(folder.GetName());
	_tprintf (_TXT("  Source Backup of folder %ls\n"), folder.GetPtr());
	
	CreateArchive ();

	CBfc::Filename	folder_fspec(ALL_WILD);
	folder_fspec.SetPath(folder);
	
	CBfcOS::FolderSelector	fs(&folder_fspec);
	CBfc::Filename	sub_folder;

	fs.StartIteration();
	while (fs.GetNext(sub_folder))
	{
		Filename fname(sub_folder.GetName());
		// If this folder is named Release, Debug, or CVS, skip it!
		if (fname.CompareNoCase(_TXT("debug")) && fname.CompareNoCase(_TXT("release")) && fname.CompareNoCase(_TXT("cvs")))
		{
			AddToArchive (folder, fname);
		}
	}
	return true;
}

bool BackupSources::CreateArchive ()
{
	STRING	s_exe;
	CBfcOS::OS_Static::ForceDirectory (m_archive_name.GetPath());
	s_exe.Format ("tar --no-recursion -cf \"%s\" *", m_archive_name.GetPtr());
	_tprintf (s_exe.GetPtr());
	_tprintf (_TXT("\n"));
	return CBfcOS::OS_Static::SysCommand (s_exe);
}

bool BackupSources::AddToArchive (const Filename &folder, const Filename &sub_fname)
{
	STRING	s_exe;
	s_exe.Format ("tar -rf \"%s\" %s/*", m_archive_name.GetPtr(), sub_fname.GetPtr());
	_tprintf (s_exe.GetPtr());
	_tprintf (_TXT("\n"));
	return CBfcOS::OS_Static::SysCommand (s_exe);
}






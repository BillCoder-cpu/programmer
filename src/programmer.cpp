//*/
/*
//
//					programmer.cpp : 
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
	Example usages:
		-h h -s *.cpp
*/
/*
	Todo - 	preserve date stamp.
			option to skip .BAK generation

			-h
			somehow use a different header file for each project:
				Method 1) if certain header file found in folder, use that on this and all subfolders
				Method 2) 

			look for project name (*.sln) automatically and do !substitutions in header file
				to insert the project name for the current folder.

	Done -
			do !current_filename to specify where to put the current filename (source.cpp) into the	header.

	The CBfcScript_Text source files detect if they are single or multiple byte
	format,	and write accordingly.

	Feb 14,2011 - 1.2.8 Fixed SourceCleaner to act on Debug and Release folders as well as current folder
	Jan 17,2011 - 1.2.5 Moved to Visual Studio 2010, added w flag for windows in MakeMakefile
	
*/

#include "stdafx.h"
#include "Header.h"
#include "Codify.h"
#include "MakeMakefile.h"
#include "SourceCleaner.h"
#include "BackupSources.h"
#include "PLex.h"

static void usage ();

	bool		b_C_plus_plus = false;
	bool		b_windows = false;
	bool		b_mingw = false;

void CheckForMakeArgs(char *argv[], int a, int ii)
{
	while (argv[a][ii])
	{
		switch (argv[a][ii])
		{
		case 'p': b_C_plus_plus = true; break;
		case 'w': b_windows = true; break;
		case 'g': b_mingw = true; break;
		}
		ii++;
	}
}


int main(int argc, char* argv[])
{
	bool		b_workdone = false;
	bool		b_operate = false;
	
	bool		b_verbose = true;
	bool		b_backup = false;
	bool		b_clean = false;
	bool		b_codify = false;
	bool		b_set_headers = false;
	bool		b_run_plex = false;
	bool		b_make_makefiles = false;
	bool		b_recurse_subfolders = false;
	bool		b_header_phantomfolder = false;
	bool		b_grep = false;
	bool		b_help = false;
	bool		b_dump_csv = false;
	Filename csvfilename;
	Filename	header_fname;
	Filename	makebase;
	Filename	plexfilename;
	STRING		grep_string;
	int			a;

	for (a=1; a < argc && *argv[a]=='-'; a++)
	{
		switch (tolower(argv[a][1]))
		{
			case 'b':
				b_backup = true;
				b_operate = true;
				break;
			case 'c':		// -clean : clean folder
				CheckForMakeArgs(argv, a, 2);
				b_clean = true;
				b_operate = true;
				break;
			case 'd':
				b_codify = true;
				b_operate = true;
				break;
			case 'g':
				if (argc > a+2)
				{
					grep_string = argv[++a];
					b_grep = true;
					b_operate = true;
				}
				break;
			case 'h':		// -h set headers
				if (argc > a+2)
				{
					if (tolower(argv[a][2]) == 's')
						b_header_phantomfolder = true;
					header_fname = argv[++a];
					b_set_headers = true;
					b_operate = true;
				}
				break;
			case 'l':
				if (argc > a+1 && *argv[a+1] != '-')
				{
					b_run_plex = true;
					plexfilename = argv[++a];
					b_operate = true;
				}
				break;
			case 'm':
				{
					CheckForMakeArgs(argv, a, 2);
					b_make_makefiles = true;
					b_operate = true;
				}
				if (argc > a+1 && *argv[a+1] != '-')
				{
					makebase = argv[++a];
				}
				break;
			case 'q':		// quiet mode
				b_verbose=false;
				break;
			case 's':
				b_recurse_subfolders = true;
				break;
			case 'v':		// -v means dump a file to csv
				if (argc > a+1)
				{
					b_dump_csv = b_operate = true;
					csvfilename = argv[++a];
				}
				break;
			case '?':
				b_verbose=true;
				b_help = true;
				b_operate = true;
				break;
		}
	}
	if (b_verbose)
	{
		STRING	sCredits;
		CBfcOS::OS_Static::GetCreditsString(sCredits);
		_tprintf (_TXT("\n"));
		_tprintf (_TXT("Programmer version 1.7 : September 13, 2020\n"));
		_tprintf (_TXT("   Copyright (C) 2004-2020 by William P. Foster, all rights reserved.\n"));
		_tprintf (sCredits.GetPtr());
		_tprintf (_TXT("\n"));
	}
	if (b_set_headers)
	{
		Header			h(header_fname, b_header_phantomfolder);

		while (a < argc)
		{
#if 0
			const Filename	cs_TargetFilespec (argv[a++]);
			CBfcOS::FolderOperation	fop(cs_TargetFilespec, b_recurse_subfolders);
			b_workdone |= (fop.Operate(&h) > 0);
#else
			const Filename	cs_TargetFilespec (argv[a++]);

			CBfcOS::FileOperation	fop(cs_TargetFilespec, b_recurse_subfolders);
			b_workdone |= (fop.Operate(&h) > 0);
#endif
		}
	}
	if (b_grep)
	{
//		grep_string
	}

	// Always have a MakeMakefile, since we may need it for the -m or the -c option, and it doesn not access anything but
	// a tiny bit of memory and a few wasted cpu clock cycles to do this.
	MakeMakefile	maker(makebase, b_recurse_subfolders);
	maker.SetLanguageType(b_C_plus_plus?C_PLUS_PLUS:C_SHARP);
	maker.SetWindowsOS(b_windows,b_mingw);

	if (b_make_makefiles)
	{
		const bool b_recurse_makefiles = false;
		CBfcOS::FolderOperation	fop(b_recurse_makefiles);		// Don't recurse here, recurse flag is used within to recurse source folders.
		b_workdone = fop.Operate(&maker) > 0;
	}
	if (b_run_plex)
	{
		PLex	pl(plexfilename);
		while (a < argc) {
#if 0
			const Filename	cs_TargetFilespec (argv[a++]);
			CBfcOS::FolderOperation	fop(cs_TargetFilespec, b_recurse_subfolders);
			b_workdone |= (fop.Operate(&pl) > 0);
#else
			const Filename	cs_TargetFilespec (argv[a++]);

			CBfcOS::FileOperation	fop(cs_TargetFilespec, b_recurse_subfolders);
			fop.m_cs_ignoreFolderSpec = ".*";
			b_workdone |= (fop.Operate(&pl) > 0);
#endif
		}
		pl.CleanUp();
	}
	if (b_dump_csv)
	{
		bool bRead = true;
		Stream fin (csvfilename, bRead);
		if (fin.IsAttached())
		{
			String cs;
			csvfilename.SetExtention("csv");
			bRead = false;
			csvfilename.UnLink();
			Script_Text fout (csvfilename, bRead);
			if (fout.IsAttached())
			{
				fout.SetBytesPerCharacter (1);
				cs.Format ("static uint8 GLX_backdrop_data[%d]={",fin.GetFileHandle()->FileLength() );
				fout.Write(cs);

				int uc = 0;
				while (fin.Read(&uc, 1))
				{
					cs.Format (",%d", uc);
					fout.Write(cs);
				}
				fout.Close ();
				b_workdone = true;
			} else
				_tprintf (_TXT("Can't open output file for CSV: %ls.\n"), csvfilename.GetPtr());
			fin.Close();
		} else
			_tprintf (_TXT("Can't open file for CSV: %ls.\n"), csvfilename.GetPtr());
//		Script_Text	s_outfile(csvfilename, false);
//					csvfilename = ;
	}
	if (b_clean)
	{
		SourceCleaner	cl;

		CBfcOS::FolderOperation	fop(b_recurse_subfolders);
		b_workdone = fop.Operate(&cl) > 0;
		if (!b_recurse_subfolders)	{
			Filename fnDebug;
			Filename fnRelease;

			maker.GetDebugAndReleaseFolderNames_Literal (fnDebug, fnRelease);

			fop.SetFolder (fnDebug);
			fop.Operate (&cl);
			fop.SetFolder (fnRelease);
			fop.Operate (&cl);
		} 
	}
	if (b_codify)
	{
		Codify			cod;

			const Filename	cs_TargetFilespec (argv[a++]);

			CBfcOS::FileOperation	fop(cs_TargetFilespec, b_recurse_subfolders);
			b_workdone |= (fop.Operate(&cod) > 0);
	}
	if (b_backup)
	{
		while (a < argc)
		{
			const Filename	cs_SourceFolder (argv[a++]);

			BackupSources	bs;
			CBfcOS::FolderOperation	fop(cs_SourceFolder, b_recurse_subfolders);
			b_workdone |= (fop.Operate(&bs) > 0);
		}
	}
	if (b_help)
	{
		usage ();
		PLex::manual();
	}
	if (!b_operate)
	{
		if (b_verbose) 
			usage();
		else
			_tprintf(_TXT(" programmer: bad parameters\n\n"));
		return -1;
	}
	if (!b_workdone)
			_tprintf(_TXT(" programmer: No work was done!\n"));
	
	_tprintf (_TXT("\n"));
	return 0;
}


int	Codify::Operate(const Filename &fname)
{
	Filename outFname (fname);
	outFname.SetExtention ("codify");
	outFname.UnLink();
	Script_Text	s_outfile(outFname, false);
	Script_Text	s_infile(fname);

	_tprintf (_TXT("Codify file: %ls.\n"), fname.GetPtr());

//    inline bool Write (void *srcMem, const unsigned int uCount, unsigned int *p_uBytesWritten=NULL)
	UINT	u_BytesPerCharacter = s_infile.DetectBytesPerCharacter();
	s_outfile.SetBytesPerCharacter (u_BytesPerCharacter);

	STRING	cs;
	while (s_infile.ReadLine (cs))
	{
		CodifyString (cs);
		s_outfile.Write ("\"");
		s_outfile.Write (cs);
		s_outfile.WriteLine ("\" +");
	}
	s_infile.Close ();
	s_outfile.Close ();
	return 1;
}

void Codify::CodifyString (STRING &cs)
{
	STRING   csOut;
	int index = 0;
	while (cs[index] != '\0')
	{
		switch (cs[index])
		{
			case '"':
				csOut += '\\';
				break;
			case '\n':
				csOut += '\\';
				csOut += 'n';
				index++;
				continue;
				break;
		}
		csOut += cs[index];
		index++;
	}
	cs = csOut;
}


static void usage ()
{
	_tprintf(_TXT(" Usage:    programmer [options] target_filespec\n"));
	_tprintf(_TXT("\n"));
	_tprintf(_TXT("    [-b dir] Backup Non-Object files from specifed folder (recursive).\n"));
	_tprintf(_TXT("    [-c[w|g] or -clean]  Remove *.* from Debug/Release folders, *.o and *~ and *.BAK from current folder\n"));
	_tprintf(_TXT("    [-d] Create a sourcefile string from the contents of the file.\n"));
	_tprintf(_TXT("    [-f performfile] Perform commands in a script file\n"));
	_tprintf(_TXT("    [-g \"grep string\"] Grep\n"));
	_tprintf(_TXT("    [-h[s] headerfile]  Set Headers to given headerfile, use -hs to get headerfile from above folders if not present\n"));
	_tprintf(_TXT("    [-l plexfile] Run a set of plex commands\n"));
	_tprintf(_TXT("    [-m[w|g]] [sourcedir] Makefile generation - C# mode, w=MSwindows, g=MinGW\n"));
	_tprintf(_TXT("    [-mp[w|g]] [sourcedir] Makefile generation - C++ mode, w=MSwindows, g=MinGW\n"));
	_tprintf(_TXT("    [-q] quiet mode\n"));
	_tprintf(_TXT("    [-s] specifies recurse subfolders\n"));
	_tprintf(_TXT("    [-u outfolder] searches given sources for .h includes & deletes .o files from outfolder\n"));
	_tprintf(_TXT("    [-v file] Dumps a file to csv (comma seperated values) format into filename.csv\n"));
	_tprintf(_TXT("    [-?] help\n"));
	_tprintf(_TXT("\n"));
}

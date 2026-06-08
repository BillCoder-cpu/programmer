//*/
/*
//
//					MakeMakefile.cpp : 
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
#include "MakeMakefile.h"

using namespace CBfcScript;

#define DEBUG_FOLDER_STRING "Debug"
#define	RELEASE_FOLDER_STRING "Release"
Filename		MakeMakefile::m_DebugDir="Debug", MakeMakefile::m_ReleaseDir="Release";

/*	
Works from an online file named MakefileBase (or MakefileBase.win or MakefileBase.mingw) - a requirement.

	 example MakefileBase file:
#
# FreeBSD Makefile
#
#  This is the gcc Makefile for BFC
#
#  Copyright (C) 2004 by William P. Foster, all rights reserved
#

srcdir=..
includes=../include
DEFS = -D UNIX

CCPP = c++
CDEBUG = -g -D _DEBUG
CPPFLAGS = $(CDEBUG) -I$(srcdir) -I$(includes) -fPIC $(DEFS)

LINKER = ld
LINKEROPTIONS = --export-dynamic

BFC : check_headers $(ALL_OBJ)
	g++ -shared -Wl,-soname,BFC.so.1 $(ALL_OBJ) -lc

	#	$(LINKER) $(LINKEROPTIONS) -o BFC.lib $(ALL_OBJ)
*/

/* Tis is what we must generate here:
.cpp.o:

	# these guys have their rules listed below
ALL_CPP = 
ALL_OBJ = 
ALL_H = 

	ETC:

*/

MakeMakefile::MakeMakefile()
	: m_bWindows(false), m_bMingw(false), m_bRecurseSources(false)
{
}

MakeMakefile::MakeMakefile(const Filename &basefolder, const bool b_recurse_sources)
:	m_BaseFolder (basefolder), m_bWindows(false), m_bMingw(false), m_bRecurseSources(b_recurse_sources)
{
}


bool MakeMakefile::GetDebugAndReleaseFolderNames_Literal (Filename &fnDebug,  Filename &fnRelease)
{
	CBfc::StringArray	ignoreList;
//	CBfcScript::Script&
	Script *p_baseScript = OpenTheBase (".", false);
	if (p_baseScript)
		LocateProgrammerKeywords (*p_baseScript, ignoreList);

	fnDebug = m_DebugDir;
	fnRelease = m_ReleaseDir;
	return true;
}

///////////////////////////////////////////////////////////
// Look through the MakefileBase to pull out special information intended for this program
// looks for includes=path;path;path 
// also looks for ##PROGRAMMER_IGNORE_LIST
void MakeMakefile::AnalyzeBase (Script& base, CBfc::StringArray& includePaths, CBfc::StringArray &ignoreList)
{
	CBfc_String			s;
	LocateProgrammerKeywords (base, ignoreList);
	base.Rewind();
//	CBfc_String			search("includes=");
	while (base.ReadScriptWord (s, '='))
	{
		base.SkipDeliminator('=');
		if (s == "includes")
		{
			while (!base.IsEOL() && base.ReadScriptWord (s, ";\n"))
			{
				base.SkipDeliminator(';');
				_tprintf (_TXT("     Include Path %ls detected.\n"), s.GetPtr());
				includePaths.Add (&s);
			}
			break;
		}
	}
// 	_tprintf (_TXT("* No Include Paths detected!\n"), s.GetPtr());
	base.Rewind();
}

/// Fills in ignireList
void MakeMakefile::LocateProgrammerKeywords (Script& base, CBfc::StringArray &ignoreList)
{
	CBfc_String			s;
	// First, locate any ##PROGRAMMER_IGNORE_LIST
	CBfc_String			search1("##PROGRAMMER_IGNORE_LIST");
	CBfc_String			search2("##DebugDir=");
	CBfc_String			search3("##ReleaseDir=");
	character c;
	while (base.ReadAheadScript_Character (c))
	{
		if (c == '#') {
			if (base.ReadAheadScriptWord (s, '\n')) {
				if (s == search1)
				{	// Found our ignore list, now load the list
					CBfc_String			search_end("##END_PROGRAMMER_IGNORE_LIST");
					base.ReadScriptLine (s);		// Skip over the initializing line
					while (base.ReadAheadScript_Character (c))
					{
						if (c == '#')
						{
							if (base.ReadAheadScriptWord (s, '\n'))
								if (s == search_end)
									break;		// Found end of ignores

							base.SkipScriptCharacter ();		// THE PROGRAMMER_IGNORE_LIST is all in comments, skip them!
							base.SkipWhiteSpace();
							if (base.AtEOL()) continue;
						}
						if (c == '"') base.ReadScriptString (s);
									else base.ReadScriptWord (s);
						ignoreList.Add (&s);
//		_tprintf (_TXT("     Ignore file %ls detected.\n"), s.GetPtr());
					}
					break;		// Stop looking for ignores
				} else if (s.Left(search2.GetLength()) == search2)
				{
					m_DebugDir = s.RightFrom(search2.GetLength());
				} else if (s.Left(search3.GetLength()) == search3)
				{
					m_ReleaseDir = s.RightFrom(search3.GetLength());
				}
			}
		}
		if (!base.ReadScriptLine (s)) {
//		_tprintf (_TXT("     Throw away %ls.\n"), s.GetPtr());
			break;		// Ignore (throw away result)
		}
	}
}

Script * MakeMakefile::OpenTheBase (const Filename &folder, const bool verbose)
{
	Filename	baseFile("MakefileBase");
	if (m_bWindows)		baseFile += ".win";
	else if (m_bMingw)	baseFile += ".mingw";

	baseFile.SetPath (folder);
	Stream		*p_baseStream = new Stream(baseFile);

	if (!p_baseStream->IsAttached()) {
		_tprintf (_TXT("\n  ERROR!!!!: the file %ls was not found for folder %ls\n\n"), baseFile.GetPtr(), folder.GetPtr());
		return NULL;
	}

	Filename	cwd;
	CBfcOS::OS_Static::GetCurrentPath(cwd);
	_tprintf (_TXT(" Operating from folder %ls\n"), cwd.GetPtr());
	if (verbose)
		_tprintf (_TXT("  Generating Makefile from %ls in folder %ls\n"), baseFile.GetPtr(), folder.GetPtr());
	if (!m_BaseFolder.IsEmpty())	_tprintf (_TXT("    Project Base is in folder %ls\n"), m_BaseFolder.GetPtr());
	_tprintf (_TXT("\n"));

	Script *p_out_Script = new Script (*p_baseStream);
	return p_out_Script;
}

// Traverse each file and find it's dependencies (search for #include)
// Generate a makefile listing all of the .cpp files (-+that don't end in .inc.cpp), with make rules
int MakeMakefile::Operate (const Filename &folder)
{
	Filename::OSFolderChar = '/';
	if (m_bWindows || m_bMingw)
		Filename::OSFolderChar = '\\';

	Script *p_base = OpenTheBase (folder, true);
	if (!p_base) return true;
	CBfc::StringArray	includePaths;
	CBfc::StringArray	ignoreList;

	p_base->SetMidlineCommentIndicator ("");		// Take all characters in the input MakefileBase

	Filename	source_folder;
	if (m_BaseFolder.IsEmpty())		source_folder = folder;
	else {
		source_folder = m_BaseFolder;
		if (folder != ".")			source_folder.SetPath(folder);
	}
	Filename	include_folder(source_folder);
	Filename	debug_folder( "$(DebugDir)");
	Filename	release_folder("$(ReleaseDir)");

	debug_folder.SetPath(folder);
	release_folder.SetPath(folder);

	CBfc::StringArray	headers, sources;

	if (m_eLanguage == C_PLUS_PLUS)
	{
		AnalyzeBase	(*p_base, includePaths, ignoreList);
		includePaths.Add (&include_folder);

		if (!FindHeaders(includePaths, headers)) {
			_tprintf (_TXT("\n   ********* ERROR *********    The C++ Headers could not be located!.\n\n"));
			return true;
		}
	}
	if (!FindSources(source_folder, sources, ignoreList))
		_tprintf (_TXT("\n   ********* Warning *********    The Sources could not be located!.\n\n"));
//	else
	{
		CBfc::StringArray	objects;

		Filename	outfilename("Makefile");
		outfilename.SetPath(folder);
		outfilename.UnLink();
		Stream		outStream(outfilename, false);

		if (!outStream.IsAttached()) {
			_tprintf (_TXT("\n  ERROR!!!!: the file %ls could not be opened for writing!\n\n"), outfilename.GetPtr());
			return true;
		}
		Script			out (outStream);
		out.SetBytesPerCharacter(p_base->GetBytesPerCharacter());

		p_base->Rewind();
		AppendComments (out, *p_base);	// Get top comments of makefilebase

		out.WriteLine ("#");
		out.WriteLine ("# Generated via Programmer, a programmer's utility.");
		out.WriteLine ("#");
//		out.WriteLine (".cpp.o:\n");
		out.Write ("DebugDir=");		// Hey, just in case the user wants the defaults, put them at the very beginning, they may be redefined by the user later in this file.
		out.WriteLine (m_DebugDir);
		out.Write ("ReleaseDir=");
		out.WriteLine (m_ReleaseDir);

		MakeObjectlist_from_Sourcelist (sources, objects);
		if (m_eLanguage == C_PLUS_PLUS) {
			out.Write ("\nALL_H = ");
			WriteFilelist (out, headers);	// Error: not all of these came from include_folder!!!!!!!!!
		}
		out.Write ("\nALL_CPP = ");
		WriteFilelist (out, sources, &source_folder);
		out.Write ("\nALL_OBJ = ");
		WriteFilelist (out, objects, &source_folder);

		out.Write ("\nALL_DEBUG_OBJ = ");
		WriteFilelist (out, objects, &debug_folder);

		out.Write ("\nALL_RELEASE_OBJ = ");
		WriteFilelist (out, objects, &release_folder);

		out.WriteLine ("");

		// Now put in the rest of the Makefilebase
		out.AppendScript(*p_base);

		// Now put in exclusive dependencies
		CBfc_String	*p_s;

		ULONG	uIh = headers.StartIteration();
		while (p_s = headers.GetNext(uIh)) {
			CBfc::StringArray	dependencies; //, includePaths;
			dependencies.Empty();
			const Filename fn(*p_s);
			if (FindImmediateDependencies (fn, dependencies, includePaths)) {
/*				  Write this out foreach header filename:
			CBfc_ReferencedObject.h : CBfc_Object.h	Other_header.h
*/
				out.Write (include_folder.GetPtr());
				out.Write ("/");
				out.Write (p_s->GetPtr());
				out.Write (" : ");
				WriteFilelist(out, dependencies);
				out.WriteLine("");
			}
		}

		ULONG uIo=objects.StartIteration();
		ULONG uIs=sources.StartIteration();
		while (p_s = sources.GetNext(uIs))
		{
			CBfc_String	*p_sobj = objects.GetNext(uIo);
			if (p_sobj)
			{
				CBfc::StringArray	dependencies;
				dependencies.Empty();
				const Filename fn(*p_s);
				const bool bListDependencies = FindImmediateDependencies (fn, dependencies, includePaths);

				Filename	source(*p_s);
				Filename	object(*p_sobj);

				source.PrependPath(source_folder);
				object.SetPath(debug_folder);

				switch (m_eLanguage)
				{
					case C_PLUS_PLUS:
						WriteMakeRule (out, object, source, "$(CCPP) -c $(CPPFLAGS) $(CPPDEBUG)", bListDependencies, dependencies);
		object.SetPath(release_folder);
						WriteMakeRule (out, object, source, "$(CCPP) -c $(CPPFLAGS)", bListDependencies, dependencies);
						break;
					case C_SHARP:
						WriteMakeRule (out, object, source, "$(CCS) $(CSFLAGS) $(CSDEBUG)", bListDependencies, dependencies);
		object.SetPath(release_folder);
						WriteMakeRule (out, object, source, "$(CCS) $(CSFLAGS)", bListDependencies, dependencies);
						break;
				}
			}
		}
		_tprintf (_TXT("         - %ld Sources found.\n"),(long)sources.GetCount());
		if (m_eLanguage == C_PLUS_PLUS)
			_tprintf (_TXT("         - %ld C++ Headers found.\n"),(long)headers.GetCount());
 
		return true;
	}
	return true;
}

void MakeMakefile::WriteMakeRule(Script &out, Filename &object, Filename &source, STRING makestring, const bool bListDependencies, StringArray &dependencies)
{
	out.Write ("\n");
	out.Write (object.GetPtr());
	out.Write (" : ");
	out.Write (source.GetPtr());
	out.Write (" ");

	if (bListDependencies)
	{
//	Write this out foreach filename:
//
//		CBfc_Array.o : CBfc_Array.cpp CBfc_Array.h CBfc_Object.h
//				$(CCPP) -c $(CPPFLAGS) CBfc_Array.cpp
		WriteFilelist(out, dependencies);
	} else
		out.WriteLine ("");
	out.Write ("\t\t");
	out.Write (makestring.GetPtr());
	switch (m_eLanguage)
	{
		case C_PLUS_PLUS:
			if (m_bWindows)		out.Write (" /Fo");
				else			out.Write (" -o ");
			break;
		case C_SHARP:
			out.Write (" -out:");
			break;
	}
	out.Write (object.GetPtr());
	out.Write (" ");
	out.WriteLine (source);
}

// put any comment lines frin the base stream into the output stream.
bool MakeMakefile::AppendComments (Script &outScript, Script &baseScript)
{
	character c;
	while (baseScript.ReadAheadScript_Character (c) && c == '#')		// comment char
	{
		CBfc_Str	s;
		if (baseScript.ReadScriptLine (s))
			outScript.WriteLine(s);
	}
	return true;
}

bool MakeMakefile::MakeObjectlist_from_Sourcelist (CBfc::StringArray &sources, CBfc::StringArray &objects)
{
	CBfc_String	*p_s;

	ULONG uIs=sources.StartIteration();
	while (p_s = sources.GetNext(uIs))
	{
		Filename fn(*p_s);
		fn.RemovePath();
		fn.SetExtention ("o");
		objects.Add (&fn);
	}
	return true;
}

//
//  Fills in the entire path names
//
bool MakeMakefile::FindHeaders (CBfc::StringArray &folderList, CBfc::StringArray &headerList)
{
	char		*headexts[] = { "*.h", "*.hpp", "*.H", "*.HPP", NULL };

	ULONG		iter = folderList.StartIteration();
	STRING	*p_s;
	while (p_s = folderList.GetNext(iter)) {
		MakeMake_AccumulateFilenames::AddFilenamesToList_from_extlist (headexts, *p_s, headerList);
	}
	return headerList.GetCount() > 0;
}

char	*SharpExts[] = { "*.cs", NULL };
char	*PlusPlusExts[] = { "*.cpp", "*.cc", "*.c", "*.C", "*.CPP", "*.CC", NULL };

bool MakeMakefile::FindSources (const Filename &folder, CBfc::StringArray &List, const CBfc::StringArray &ignoreList)
{
	char		**srcexts;

	switch(m_eLanguage) {
		case C_SHARP:
			srcexts = SharpExts;
			break;
		case C_PLUS_PLUS:
			srcexts = PlusPlusExts;
			break;
	}

#if 1 // true
	MakeMake_AccumulateFilenames			maf(srcexts, List);
	CBfcOS::FolderOperation					fop(folder, m_bRecurseSources);		// possibly Recurse Source folders here.
	fop.m_cs_ignoreFolderSpec = ".*";		// ignore things like .svn (Hidden folders)

	if (fop.Operate(&maf) > 0)
#else
//	_tprintf ("Looking in %s for Sources.\n", folder.GetPtr());	
	if (AddFilenamesToList_from_extlist (srcexts, folder, List))
#endif
	{
		Remove_DotIncs_from_List (List, ".inc.c");
		Remove_Ignores_from_List (List, ignoreList);
		return List.GetCount() > 0;
	}
	return false;
}

void MakeMakefile::Remove_Ignores_from_List (CBfc::StringArray &List, const CBfc::StringArray &ignoreList)
{
		ULONG index;  STRING *p_cs;
		BFC_ITERATE (List, p_cs, index) {
			if (MatchesIgnore (p_cs, ignoreList))
				List.RemoveAt (--index);
		}
}

bool MakeMakefile::MatchesIgnore (const STRING *p_fn, const CBfc::StringArray &ignoreList)
{
		ULONG index;  STRING *p_cs;
		BFC_ITERATE (ignoreList, p_cs, index) {
			if (*p_cs == *p_fn) return true;
			Filename fn(*p_fn);
			if (fn.GetNameExtension() == *p_cs) return true;
		}
		return false;
}

void MakeMakefile::Remove_DotIncs_from_List (CBfc::StringArray &List, const CBfc_String match)
{
	for (UINT x=0; x < List.GetCount(); x++)
		if (List[x]->Find(match) >= 0)
			List.RemoveAt(x--);
}

// For now, only supports C and C++
// these file can be in any source dir listed in $includes
bool MakeMakefile::FindImmediateDependencies (const Filename &source_Filename, CBfc::StringArray &List, CBfc::StringArray &includePaths)
{
	Stream	sIn(source_Filename);
	if (!sIn.IsAttached()) return false;
	Script	src(sIn);					// strips out comments automatically

	src.SetMidlineCommentIndicator ("//");

	CBfc_String			s;
	CBfc_String			search("#include");
	while (src.ReadScriptLine (s))
	{
		int pos = s.Find(search);
		if (pos >= 0)
		{
			pos += 8;
			// If the next non-space character is not a # or an <, ignore this find
			while (SBFC_Character::isWhiteSpace(s[pos]))
				pos++;	// Skip end spaces
			if (s[pos] == '<' || s[pos] == '"')
			{
				character endchar;

				if (s[pos] == '<') endchar = '>';
				else endchar = s[pos];

				pos++;
				int	endpos = s.Find(endchar,pos);
				if (endpos > pos)
				{
					Filename sHeader = s.Mid(pos, endpos);

					if (sHeader.FileExists())
						List.Add(&sHeader);
					else
					{
						/// Now locate this file with qualified path and add to list.
						CBfc_String	*p_sPath;
						ULONG uIi=includePaths.StartIteration();
						while (p_sPath = includePaths.GetNext(uIi))
						{
							Filename	sFullHeader (sHeader);
							sFullHeader.SetPath(*p_sPath);
							if (sFullHeader.FileExists())
							{
								List.Add (&sFullHeader);
								break;
							}
						}
					}
				}
			}
		}
	}
	// Search for #include 
	return List.GetCount()>0;
}

/* Example:
folder/CBfc_2dRect.h folder/CBfc_2dShape.h folder/CBfc_2dSize.h folder/CBfc_3dPointSpace.h \
		CBfc_Angle.h CBfc_Array.h CBfc_CompressionAlgorythm.h CBfc_Distance.h \
		CBfc_Exception.h CBfc_File.h CBfc_Filename.h CBfc_LinkedList.h \
	CBfc_Matrix.h CBfc_Object.h CBfc_PointSpaceTime.h CBfc_PointTime.h \
	CBfc_ReferencedObject.h CBfc_ReferencedValue.h CBfc_ReferencedPointer.h 
*/
bool MakeMakefile::WriteFilelist (Script &out, CBfc::StringArray &list, const STRING *p_folder)
{
	CBfc_String	*p_s;

	ULONG uIl=list.StartIteration();
	p_s = list.GetNext(uIl);
	while (p_s)
	{
		for (int x=0; x < 4; x++)
		{
			if (p_folder)
			{
				out.Write (*p_folder);
				if (m_bWindows || m_bMingw)	out.Write ("\\");
					else					out.Write ("/");
			}
			out.Write (*p_s);
			out.Write (" ");
			p_s = list.GetNext(uIl);
			if (!p_s)
				break;
		}
		if (p_s) 
		{
			out.WriteLine (" \\");
			out.Write ("\t\t");
		} else out.WriteLine(" ");
	}
	return true;
}


MakeMake_AccumulateFilenames::MakeMake_AccumulateFilenames(char *p_exts[], CBfc::StringArray &List)
{
	m_p_exts = p_exts;
	m_p_List = &List;
}

// returns true if any files are added, and the FolderOperation class will accumulate and ADD these together.
int	MakeMake_AccumulateFilenames::Operate(const Filename &folder)	// part of FolderOperationClass
{
_tprintf (_TXT("     MakeMake_AccumulateFilenames Operating on %ls.\n"), folder.GetPtr());
Filename fnSubFolder(folder);
fnSubFolder.RemovePath (m_fn_BaseFolder);
	return AddFilenamesToList_from_extlist (m_p_exts, folder, *m_p_List, &fnSubFolder);
}

// returns true if any files are added
bool MakeMake_AccumulateFilenames::AddFilenamesToList_from_extlist (char *p_exts[], const Filename &folder, CBfc::StringArray &List, const STRING *p_AddFolderName)
{
	bool		bRval=false;
	UINT	uIndex=0;	
	
	while (p_exts[uIndex])
	{
		Filename fsel_spec(p_exts[uIndex++]);
		bRval |= AddFilesToList (folder, fsel_spec, List, p_AddFolderName);
	}
	return bRval;
}

// If p_folder is non-NULL, prepend it to the filenames
bool MakeMake_AccumulateFilenames::AddFilesToList (const Filename &folder, Filename &fsel_spec, CBfc::StringArray &List,const STRING *p_folder)
{
#ifdef WINDOWS		// Windows always adds a wildcard to the end of the search, so make sure they end right
	STRING checkspec = fsel_spec.RightFrom(1);
#endif
	fsel_spec.SetPath (folder);
	Filename fselSpec(fsel_spec);
	CBfcOS::FileSelector fsel(&fselSpec);
	bool	bRval=false;

//	_tprintf (_TXT("Adding files of name %s to a list!\n"), fselSpec.GetPtr());
	Filename name;
	fsel.StartIteration();
	while (fsel.GetNext(name))
	{
#ifdef WINDOWS		// Windows always adds a wildcard to the end of the search, so make sure they end right
		if (!name.EndsWith(checkspec))
			continue;
#endif
		if (p_folder)
			name.SetPath(*p_folder);
//_tprintf (_TXT("Adding name %s.\n"),name.GetPtr());
		List.Add(&name);
		bRval=true;
	}
	return bRval;
}



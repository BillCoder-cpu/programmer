//*/
/*
//
//					MakeMakefile.h : 
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

#include <BFC/BFC.h>
#include <BFC/BFCOS.h>

using namespace CBfc;

typedef enum {
	C_PLUS_PLUS,
	C_SHARP
} LanguageType;

class MakeMakefile  : public CBfcOS::FileOperationClass
{
public:
	MakeMakefile();
	MakeMakefile(const Filename &basefolder, const bool b_recurse_sources);
	virtual int	Operate(const Filename &folder);	// part of FolderOperationClass
	void SetLanguageType (const LanguageType eLang) { m_eLanguage = eLang; }
	void SetWindowsOS (const bool bWin, const bool bMingw) { m_bWindows = bWin; m_bMingw = bMingw; }
	bool GetDebugAndReleaseFolderNames_Literal (Filename &fnDebug,  Filename &fnRelease);
	static void LocateProgrammerKeywords (CBfcScript::Script& base, CBfc::StringArray &ignoreList);
	CBfcScript::Script  *OpenTheBase (const Filename &folder, const bool verbose);

private:
	bool WriteFilelist (CBfcScript::Script &out, CBfc::StringArray &list, const STRING *p_folder=NULL);
	void WriteMakeRule(CBfcScript::Script &out, Filename &object, Filename &source, STRING makestring, const bool bListDependencies, StringArray &dependencies);
	bool FindSources (const Filename &folder, CBfc::StringArray &List, const CBfc::StringArray &IgnoreList);
	static bool FindImmediateDependencies (const Filename &source_Filename, CBfc::StringArray &List, CBfc::StringArray &includePaths);
	static bool FindHeaders (CBfc::StringArray &folderList, CBfc::StringArray &headerList);
	static void Remove_DotIncs_from_List (CBfc::StringArray &List, const CBfc_String match);
	static void Remove_Ignores_from_List (CBfc::StringArray &List, const CBfc::StringArray &ignoreList);
	static bool MatchesIgnore (const STRING *p_fn, const CBfc::StringArray &ignoreList);

	static bool MakeObjectlist_from_Sourcelist (CBfc::StringArray &sources, CBfc::StringArray &objects);
	static void AnalyzeBase (CBfcScript::Script& base, CBfc::StringArray& includePaths, CBfc::StringArray &ignoreList);
	static bool AppendComments(CBfcScript::Script &outScript, CBfcScript::Script &baseScript);

private: // data
	LanguageType	m_eLanguage;
	Filename		m_BaseFolder;	// Sources base folder
	static Filename		m_DebugDir, m_ReleaseDir;
	bool			m_bWindows, m_bMingw;
	bool			m_bRecurseSources;
};

class MakeMake_AccumulateFilenames  : public CBfcOS::FileOperationClass
{
public:
	MakeMake_AccumulateFilenames(char *p_exts[], CBfc::StringArray &List);
	virtual int	Operate(const Filename &folder);	// part of FolderOperationClass
	static bool AddFilenamesToList_from_extlist (char *p_exts[], const Filename &folder, CBfc::StringArray &List, const STRING *p_AddFolderName=NULL);
private:
	static bool AddFilesToList (const Filename &folder, Filename &fsel_spec, CBfc::StringArray &List,const STRING *p_folder=NULL);
private:
	char				**m_p_exts;
	CBfc::StringArray	*m_p_List;
};


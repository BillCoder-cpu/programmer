//*/
/*
//
//					plex.cpp : 
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
	Copyright (C) 1990,93,2006 by William Paul Foster, all rights reserved
*/
/*
 LEX: - see lex.man	
	 9/11/09 - added grep suboptions (wholeword, nocase) - turn the operation representation from a simple integer to a class
	 5/ 7/90 - Recognizes double quotes as a single quote ("" --> ")
	10/17/90 grep for "string"
*/
#define	DEBUG

#include "stdafx.h"
#include "PLex.h"
#ifndef UNIX
#	include <windows.h>
#	include <strsafe.h>
#endif

characterPtr version_string	=	STATICBYTES ("Lex - Version 1.5.3 Dec 2,2012\n");

characterPtr    lexcommands[] = {
		STATICBYTES("replace"),
		STATICBYTES("grep"),
		STATICBYTES("rmo"),
		STATICBYTES("lftocr"),
		STATICBYTES("del"),
		STATICBYTES("rmdir"),
		STATICBYTES("cs_using"),
		STATICBYTES("comments"),
		STATICBYTES("list"),
		characterPtr(NULL)
};

PLex::PLex(const Filename &plexfilename)
	:	m_PlexFilename (plexfilename), m_b_DuringComments(true)
{
	always_modified = false;
	rewrite_perhaps_needed = false;
	rewrite_always_needed = false;
	logistisize_lexfile ();
// 	manual ();		/* Display manual file from the manual dir */
	if (!m_list_outfile.IsEmpty())
	{
		const bool bRO = false;
		if (!ListOut.AttachFile(m_list_outfile, bRO))
		{
			_tprintf(_TXT("Error: Unable to Open for writing list output file %ls\n"), m_list_outfile.GetPtr());
			m_list_outfile.Empty();
		}
		else
		{
			ListOut.SetBytesPerCharacter(1);
			_tprintf(_TXT("Generating list of files: %ls\n"), m_list_outfile.GetPtr());
		}
	}
}

void PLex::CleanUp()
{
	ListOut.Detach(); //  .Close();
}

void PLex::manual ()
{
	_tprintf (version_string);
//	_tprintf (_TXT("\nUsage:    lex wildfile[,wildfile] performfile[.lex]\n"));
//	_tprintf (_TXT("   Or:    lex wildfile[,wildfile] /c direct lex command\n\n"));
	_tprintf (_TXT(" Lex performfile commands include:\n"));
	_tprintf (_TXT("   cs_using identifier = value  // put a C# using statement @top of each file\n"));
	_tprintf (_TXT("   grep \"expression\"  // display with line #s, all occurances of expression\n"));
	_tprintf (_TXT("   replace [wholeword, nocase] \"a\" with \"b\",   // replace within files\n"));
	_tprintf (_TXT("   comments on/off     // replace inside of comments? (default=on)\n"));
	_tprintf (_TXT("   lftocr              // convert linefeeds to carriage-return/lf\n"));
	_tprintf (_TXT("   del filename        // remove specified file apon invocation\n"));
	_tprintf (_TXT("   rmdir dirname       // remove the specified directory apon invocation\n"));
	_tprintf (_TXT("   rmo searchstring    // remove corresponding object file if searchstring	in file\n"));
	_tprintf (_TXT("   list outfile       // List all source files into specified file\n"));
	_tprintf (_TXT("\n"));
}    
    
int PLex::Operate(const Filename& fname)	// part of FileOperationClass
{
	if (m_list_outfile.GetLength() > 0)
	{
		ListOut.WriteLine (fname);
	}
	int r;
	if (Operations.GetCount() > 0)
	{
		r = performLexOperations(fname);
	}
	return r;
}

#ifndef UNIX
void ShowError(LPTSTR lpszFunction) 
{ 	// Retrieve the system error message for the last-error code
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message
	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
	StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), 
		lpszFunction, dw, lpMsgBuf); 
	//MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
	_tprintf ((LPCTSTR)lpDisplayBuf);
	_tprintf (_TXT("\n"));

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
//	ExitProcess(dw); 
}
#endif

Filename		TempFilename("lex.tmp");


int PLex::performLexOperations(const Filename &lexfile)		// perform our operations quickly now!
{
	int			modified = 0, shouldRewrite = false;
	char		rmo_found = false;

	Script_Text		Output;

	{  // neccessary code block to eliminate instream and input before overwriting file (closes it only after Instream is destroyed)
		Stream		InStream(lexfile);
		Script		Input(InStream);

		if (rewrite_always_needed) {
			shouldRewrite = true;
		}
		_tprintf (_TXT("::::LEXing %ls::::\n"), lexfile.GetPtr());
		if (!InStream.IsAttached()) {
			_tprintf (_TXT(" *** Error: file not found!\n"));
			return false;
		}
rerun:
		String			sPad;
		bool			b_WithinComment;
		b_WithinComment = false;

		Input.SetSkipWhitespace(false);
		Input.SetSkipLinefeeds(false);
		Input.SetSkipComments(!m_b_DuringComments);

		if (shouldRewrite) 
			if (!CreateTemporaryFile (Input, Output))
				return false;
		while (1)
		{		/* NB: later look for everything this way */
			b_WithinComment = Input.WithinComment();
			if (!Input.ReadScriptLine (sPad))
				break;
#ifdef FAR_DEBUG
		_tprintf (_TXT("\r\tLine %d"), Input.GetLineNumber());
#endif
			PLexOperation	*p_o;
			UINT			uOpIndex=0;
			ULONG			oi=Operations.StartIteration();
			while ((p_o=Operations.GetNext(oi)))
			{
				bool bMod=false;
				try
				{
					switch (p_o->m_operation)
					{
					case LEX_REPLACE:
						if (p_o->m_b_DuringComments || !b_WithinComment)
							bMod = sPad.Replace(p_o->m_key, p_o->m_second, p_o->m_bWholeword, p_o->m_bCase) != 0;
						break;
					case LEX_GREP:
						if (sPad.Find(p_o->m_key) != -1)
							_tprintf(_TXT("%5d: %ls"), Input.GetLineNumber(), sPad.GetPtr());
						break;
					case LEX_RMO:
						if (sPad.Find(p_o->m_key) != -1)
							rmo_found = true;
						break;
					case LEX_CS_USING:
						if (!shouldRewrite)	// Skip this during second, or writing run
							if (sPad.Find(p_o->m_key) != -1)
								bMod = true;
						break;
					case LEX_LFTOCR:
						break;
					}
					if (bMod) {
						if (!modified) _tprintf(_TXT("\n"));
						modified = 1;
						if (!shouldRewrite && rewrite_perhaps_needed) {
							shouldRewrite = true;
							Input.Rewind();
							goto rerun;
						}
					}
				}
				catch (...)
				{
					_tprintf(_TXT("*** Exception processing operation! ***"));
				}
				uOpIndex++;
			}

			if (shouldRewrite) {
				STRING	comment;
				if (!m_b_DuringComments && Input.GetCommentCache(comment))
					Output.Write(comment);
				removeCarriageReturns (sPad);
	// 			Output.WriteLine(sPad);
				Output.Write(sPad);
			}
#ifdef FAR_DEBUG
		_tprintf (sPad.GetPtr());
#endif
		}
		Input.Detach();
	}
	if (rmo_found) {
		delete_file_object (lexfile);
	}
	if (shouldRewrite) {
		Output.Detach();
		if (modified || always_modified) {
			bool bdel = lexfile.UnLink();
			if (!bdel) {
#ifdef UNIX
				_tprintf (_TXT("Delete file %s gives %d\n"), lexfile.GetPtr(), bdel);
#else
				ShowError (_TXT("Unlink"));
#endif
			}
			Filename::Rename(TempFilename, lexfile);
		} else
			TempFilename.UnLink();
	}
	return true;
}

bool PLex::CreateTemporaryFile (Script &Input, Script_Text &Output)
{
	String			sPad;

#ifdef DEBUG
_tprintf (_TXT("Creating temporary file %ls..\n"), TempFilename.GetPtr());
#endif
	if (!Output.AttachFile (TempFilename,false))
	{
		_tprintf (_TXT("Error: Unable to Open for writing tempfile %ls\n"), TempFilename.GetPtr());
		return false;
	}
	Output.SetBytesPerCharacter(1);

	bool			b_cs_using = false;
	PLexOperation	*p_o;

	UINT	uOpIndex=0;
	ULONG	oi=Operations.StartIteration();
	while ((p_o=Operations.GetNext(oi)))
	{
		if (p_o->m_operation == LEX_CS_USING)		// Dump out any and all using statements at beginning of rewrite
		{
			b_cs_using = true;
			sPad.Format (_TXT("using %s = %s;"), p_o->m_key.GetPtr(), p_o->m_second.GetPtr() );
			Output.WriteLine(sPad);
		}
		uOpIndex++;
	}
	if (b_cs_using)	// If any using statements were required, remove the old ones from top of file;
	{
		while (Input.ReadAheadScript_Line (sPad))
		{
			if (sPad.Left(5) != "using") {
				Input.SkipScript_Line();
				break;
			}
		}
	}
	return true;
}


// remove corresponding object file
void PLex::delete_file_object (const Filename &fn) const
{
	Filename	delfile(fn);
	delfile.SetPath("");
	if (!AttemptDeleteObject (delfile)) {
		delfile.SetPath("Debug");
		AttemptDeleteObject (delfile);
		delfile.SetPath("Release");
		AttemptDeleteObject (delfile);
	}
}

bool PLex::AttemptDeleteObject (Filename &fn) const
{
//	printf ("\nLEX: Attempt object file %ls (", fn.GetPtr());
	fn.SetExtention ("obj");
	if (fn.FileExists()) { deleteFile (fn); return true; }
	fn.SetExtention ("o");
	if (fn.FileExists()) { deleteFile (fn); return true; }
	return false;
}

/* Take out carriage returns from a string */
void PLex::removeCarriageReturns(String &stringToProcess) const
{
    stringToProcess.Replace("\r", "");
}


int PLex::deleteFolder(const Filename &folderName) const
{
    printf("\nLEX: Deleting folder %ls (", folderName.GetPtr());
    printf("%d)\n", folderName.UnLink());
    return 1;
}

int PLex::deleteFile(const Filename &fileName) const
{
    printf("\nLEX: Deleting object file %ls (", fileName.GetPtr());
    printf("%d)\n", fileName.UnLink());
    return 1;
}

char * PLex::insertString(char *currentPosition, char *stringToInsert)
{
    char *destination = currentPosition + STRING::bfc_strlen(currentPosition);
    char *source = destination + STRING::bfc_strlen(stringToInsert);

    while (source >= destination) 
        *--destination = *--source;

    while (*stringToInsert) 
        *currentPosition++ = *stringToInsert++;

    return currentPosition;
}

bool PLex::logistisize_lexfile ()
{
    //m_PlexFilename.SetExtention("LEX");
    Script lexScript(m_PlexFilename);  
    if (lexScript.IsAttached())
    {
        m_b_DuringComments = true;
        while (logistisizeLexline(lexScript));
        return true;
    } 
    else
    {
        _tprintf(_TXT("Error: lex file %ls not found!\n"), m_PlexFilename.GetPtr());
        return false;
    }
}

//
// Read in a line of the input plex script
// and add it to the list of operations to perform on each file
bool PLex::logistisizeLexline (Script &lex_script)
{
	bool			inckey = false;
	int				operation_index;
	
	if (!lex_script.MatchScriptKey (lexcommands, operation_index))
		return false;
	
	if (!(operation_index == LEX_LFTOCR || !lex_script.IsEOL()))
		return true;
	
	PLexOperation	operation;
	
	operation.m_operation = (operation_index);
	operation.m_b_DuringComments = m_b_DuringComments;

	switch (operation_index) 
	{
		case LEX_REPLACE:      /* REPLACE : replace (wholeword,nocase) "string1" with "string2"  */
			while (1)
			{
				STRING	word;
				
				lex_script.ReadAheadScriptWord (word);
				if (word.CompareNoCase(STRING("wholeword")) == 0)
				{
					lex_script.SkipScriptWord();
					operation.m_bWholeword = true;
					continue;
				}
				if (word.CompareNoCase("nocase") == 0)
				{
					lex_script.SkipScriptWord();
					operation.m_bCase = false;
					continue;
				}
				break;
			}
			// fall through
		case LEX_CS_USING:
			lex_script.ReadScriptString(operation.m_key);
			lex_script.SkipScriptWord();		/* Skip past "with" to replacement string */
			lex_script.ReadScriptString(operation.m_second);
			inckey = true;
			rewrite_perhaps_needed = true; 		/* Replace and cs-using both may modify files */
#ifdef DEBUG
			if (operation.m_operation == LEX_REPLACE)
			{
				_tprintf (_TXT("Replace"));
				if (operation.m_bWholeword)
					_tprintf (_TXT(" WholeWord"));
				if (!operation.m_bCase)
					_tprintf (_TXT(" NoCase"));
				_tprintf (_TXT(" \"%ls\" with \"%ls\".\n"), operation.m_key.GetPtr(), operation.m_second.GetPtr());
			} else
				_tprintf (_TXT("Run c#_using on \"%ls\" equals \"%ls\".\n"), operation.m_key.GetPtr(), operation.m_second.GetPtr());
#endif
			break;
		case LEX_GREP:
			lex_script.ReadScriptString(operation.m_key);
#ifdef DEBUG
			_tprintf (_TXT("grep for %ls"), operation.m_key.GetPtr());
#endif
			inckey = true;
			if (lex_script.IsEOL()) break;
			lex_script.ReadScriptString(operation.m_second);
#ifdef DEBUG
			_tprintf (_TXT(" and %ls"), operation.m_second.GetPtr());
#endif
			break;
		case LEX_RMO:
			if (!lex_script.IsEOL())
			{
				inckey = true;
				lex_script.ReadScriptPhrase(operation.m_key);
#ifdef DEBUG
				_tprintf (_TXT("rmo for %ls"), operation.m_key.GetPtr());
#endif
			}
			break;
		case LEX_LFTOCR:
#ifdef DEBUG
_tprintf (_TXT("lftocr requested"));
#endif
			inckey = true;
			rewrite_perhaps_needed = true;
			rewrite_always_needed = true;		/* Must modify files with lftocr */
			always_modified = true;					/* Write out each file */
			break;
		case LEX_DELETE:
			{
				Filename fn;
				if (lex_script.ReadScriptPhrase(fn))
					deleteFile (fn);
			}
			break;
		case LEX_RMDIR:
			{
				Filename fn;
				if (lex_script.ReadScriptPhrase(fn))
					deleteFolder (fn);
			}
			break;
		case LEX_COMMENTS:
			m_b_DuringComments = false;
			break;
		case LEX_LIST:
			lex_script.ReadScriptFilename(m_list_outfile);	// Remember the outfilename
			break;
		case BFCSCRIPT_KEY_NONE:
			_tprintf (_TXT("LEX: Undefined lex command string on line %d\n"), lex_script.GetLineNumber());
			lex_script.SkipScriptLine();
		default:
			beep ();
			break;
	}
#ifdef DEBUG
_tprintf (_TXT("\n"));
#endif
	if (inckey) 
		Operations.Add(&operation);
	return true;
}

/****************************************************************************/
/*		Returns a pointer to the end of the string		    */
/* (either points to the terminating Nil character or the terminating quote */
/*									    */
char *PLex::quote_strcpy (char *d, char *s)
{
	while (1) {
		while (*s && *s != '"')
			*d++ = *s++;
		if (*s && s[1] == '"') {
			*d++ = *s++;
			s++;
			continue;
		}
		break;
	}
	*d = 0;
	return s;
}

void PLex::beep ()
{
/* puts ("beep");
	printf ("\n%c", 7);
*/
}







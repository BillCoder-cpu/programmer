//*/
/*
//
//					PLex.h : 
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

#include <BFC/BFCScript.h>

using namespace CBfc;
using namespace CBfcScript;

#define	String	STRING

#define	MAXKEYS	100
#define	KEYLEN	80

enum
{
	LEX_REPLACE	=	0,
	LEX_GREP	=	1,
	LEX_RMO		=	2,	/* Remove associated obect files */
	LEX_LFTOCR	=	3,
	LEX_DELETE	=	4,	/* Delete a specific file */
	LEX_RMDIR	=	5,	/* Remove a specific folder */
	LEX_CS_USING =	6,	// add C# using statements to the top of files that contain the key
 	LEX_COMMENTS =  7,	// turn on and off processing during comments (default on)
	LEX_LIST    =   8,
};
		
		
class PLexOperation
{
	public:
		// lexcommands (qc right below)

		PLexOperation()
			: m_bWholeword (false), m_bCase(true)
		{
		}
		int				m_operation;
		bool 			m_bWholeword, m_bCase;
		bool			m_b_DuringComments;
		CBfc::STRING	m_key, m_second;
};

class PLex  : public CBfcOS::FileOperationClass
{
public:
	PLex(const Filename &plexfilename);
	virtual int	Operate(const Filename &fname);	// part of FileOperationClass

	void CleanUp();

	static void		manual(void);

private:
	int			performLexOperations (const Filename &lexfile);
	bool		logistisizeLexline (Script &lex_script);
	char *		insertString (char *pos, char *istr);
	char *		quote_strcpy (char *, char *);
	void		beep(void);
	bool		logistisize_lexfile ();

	int 		deleteFile (const Filename &fn) const;
	int 		deleteFolder (const Filename &fn) const;
	void		delete_file_object (const Filename &fn) const;	// remove corresponding object file
	bool		AttemptDeleteObject (Filename &fn) const;

	bool 		CreateTemporaryFile (Script &Input, Script_Text &Output);
	void		removeCarriageReturns (String &s) const;

private: // data
	Filename				m_PlexFilename;
	Filename				m_list_outfile;	// if source list was requested
	Array<PLexOperation>	Operations;		// what is to be performed!

	Script_Text		ListOut;

		// track things		
	char	always_modified;
	bool	m_b_DuringComments;
	int		rewrite_perhaps_needed, rewrite_always_needed;
};


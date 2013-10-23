// Copyright 2013 Kevin Cox

/*******************************************************************************
*                                                                              *
*  This software is provided 'as-is', without any express or implied           *
*  warranty. In no event will the authors be held liable for any damages       *
*  arising from the use of this software.                                      *
*                                                                              *
*  Permission is granted to anyone to use this software for any purpose,       *
*  including commercial applications, and to alter it and redistribute it      *
*  freely, subject to the following restrictions:                              *
*                                                                              *
*  1. The origin of this software must not be misrepresented; you must not     *
*     claim that you wrote the original software. If you use this software in  *
*     a product, an acknowledgment in the product documentation would be       *
*     appreciated but is not required.                                         *
*                                                                              *
*  2. Altered source versions must be plainly marked as such, and must not be  *
*     misrepresented as being the original software.                           *
*                                                                              *
*  3. This notice may not be removed or altered from any source distribution.  *
*                                                                              *
*******************************************************************************/

/*
 * I apologize for having no functions, this program is simple and should
 * run through once in a linear fashion.  Functions aren't really useful.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <sysexits.h>
#include <getopt.h>

typedef enum {
	RLOC_KEEP,
	RLOC_ABS,
	RLOC_REL,
} Relocate;

typedef enum {
	SLASH_KEEP,
	SLASH_YES,
	SLASH_NO,
} Slash;

typedef enum {
	ACT_FULL,
	ACT_LAST,
	ACT_BASE,
	ACT_EXT,
} Action;

/// Check that memory allocation was successful.
/**
 * This function returns the pointer passed in unless it was NULL, in this
 * case the function does not return but prints an error message and aborts the
 * program.
 * 
 * @param p The pointer to check.
 * @return Returns p iff p is not NULL, otherwise this function does not return.
 */
static inline void *ca(void *p)
{
	if (p) return p;
	
	fprintf(stderr, "Error: malloc returned NULL!\n");
	exit(EX_OSERR);
}

static void help(char *argv1)
{
	fprintf(stderr, "Usage: %s [options] path\n", argv1);
	fputs("\n"
		"Action:\n"
		"  -F --Full --full\n"
		"    Return the full path.  This is the default action.\n"
		"  -L --Last --last\n"
		"    Return the last component of the path.\n"
		"  -B --Basename --basename\n"
		"    Return the filename without the extension.\n"
		"  -E --Extension --extension\n"
		"    Returns the extension.\n"
		"Options:\n"
		"  -a --absolute[=cwd]\n"
		"    Make the path absolute relative to `cwd` if provided, otherwise\n"
		"    use the current directory.\n"
		"  -r --relative[=to]\n"
		"    Make the path relative to `to` if provided, otherwise the current\n"
		"    directory.\n"
		"  -n --normalize\n"
		"    Normalize the given path as much as possible.  (Remove '/./', '/../'\n"
		"    '//' and similar.\n"
		"  -d --directory --trailing-slash\n"
		"    For --Full assume there is no file component and ensure path ends\n"
		"    with a slash.\n"
		"\n"
		"    For other options, treat a trailing slash as an empty filename (causes\n"
		"    them to return nothing).\n"
		"  -f --file --strip-slash\n"
		"    For --Full ensure there is no slash at the end of the path.\n"
		"\n"
		"    For other actions treat the last component as the filename even if\n"
		"    there is a trailing slash (The default).\n"
		"  -s --simple\n"
		"    Treat file extensions simply, for example treat \"foo.tar.gz\" as\n"
		"    \"foo.tar\" with extension \"gz\".\n"
		"  -h --help\n"
		"    Print this help message and exit.\n"
		"  -V --version\n"
		"    Print the version number and exit.\n"
	, stderr);
}

int main (int argc, char **argv)
{
	char *cwd = NULL;
	char *delim = "\n";
	
	unsigned norm  = 0;
	unsigned smart = 1;
	
	Action    act = ACT_FULL;
	Relocate rloc = RLOC_KEEP;
	Slash   slash = SLASH_KEEP;
	
	const static struct option longopts[] = {
		{ "Full",              no_argument,       NULL, 'F' },
		{ "full",              no_argument,       NULL, 'F' },
		{ "Last",              no_argument,       NULL, 'L' },
		{ "last",              no_argument,       NULL, 'L' },
		{ "Basename",          no_argument,       NULL, 'B' },
		{ "basename",          no_argument,       NULL, 'B' },
		{ "Extension",         no_argument,       NULL, 'E' },
		{ "extension",         no_argument,       NULL, 'E' },
		{ "absolute",          optional_argument, NULL, 'a' },
		{ "relative",          optional_argument, NULL, 'r' },
		{ "normalize",         no_argument,       NULL, 'n' },
		{ "directory",         no_argument,       NULL, 'd' },
		{ "trailing-slash",    no_argument,       NULL, 'd' },
		{ "file",              no_argument,       NULL, 'f' },
		{ "strip-slash",       no_argument,       NULL, 'f' },
		{ "simple",            no_argument,       NULL, 's' },
		{ "verbose",           no_argument,       NULL, 'v' },
		{ "help",              no_argument,       NULL, 'h' },
		{ "version",           no_argument,       NULL, 'V' },
		{ NULL,                0,                 NULL,  0  },
	};
	
	int i;
	while ((i = getopt_long(argc, argv, "FLBEa::r::ndfsvhV", longopts, NULL)) >= 0 )
	{
		switch (i)
		{
		case 'F':
			act = ACT_FULL;
			break;
		case 'L':
			act = ACT_LAST;
			break;
		case 'B':
			act = ACT_BASE;
			break;
		case 'E':
			act = ACT_EXT;
		case 'a':
			if (optarg)
			{
				cwd = optarg;
				size_t l = strlen(cwd);
				if ( cwd[l-1] == '/' )
					cwd[l-1] = '\0';
			}
			rloc = RLOC_ABS;
			break;
		case 'r':
			//if (optarg) cwd = optarg;
			rloc = RLOC_REL;
			break;
		case 'n':
			norm = 1;
			break;
		case 'd':
			slash = SLASH_YES;
			break;
		case 'f':
			slash = SLASH_NO;
			break;
		case 's':
			smart = 0;
		case 'v':
			break;
		case 'V':
			printf("%s version %d.%d.%d\n", argv[0], VER_MAJOR, VER_MINOR, VER_PATCH);
			exit(EX_OK);
		case 'h':
			help(argv[0]);
			exit(EX_OK);
		case '?': // Unknown argument.
		default:
			help(argv[0]);
			exit(EX_USAGE);
		}
	}
	
	while ( optind < argc )
	{
		char *i, *o, *s, *e;
		s = argv[optind++]; // Points at first character.
		e = s+strlen(s);    // Points at the null byte.
		
		///// Make path absolute.
		if ( rloc == RLOC_ABS && *s != '/' )
		{
			size_t rds = 0;
			char *rdb = NULL;
			while (!cwd)
			{
				rds += 1024;
				rdb = ca(realloc(rdb, rds));
				cwd = getcwd(rdb, rds);
			}
			
			rds = strlen(cwd);
			size_t nl = rds+e-s+1; // 1 is for the slash in the middle.
			char *tcwd = ca(malloc(nl+1));
			strcpy(tcwd, cwd);
			tcwd[rds++] = '/';
			strcpy(tcwd+rds, s);
			s = tcwd;
			e = s+nl;
		}
		
		///// Normalize.
		if (norm)
		{
			unsigned abs = ( *s == '/' );
			if (abs) s++;
			
			i = s-2;
			o = s;
			unsigned seg = 0;
			
			while ( ++i < e )
			{
				/* Add slash to print debug output.
				char t = *o;
				*o = '\0';
				printf("%40s | ", s);
				*o = t;
				printf("%-40s | ", i);
				printf("%d\n", seg + (*i == '/'));
				//*/
				
				if      (  i <   s  ) ;      // Beginning of path.
				else if ( *i == '/' ) seg++; // End of segment.
				else
				{
					// Just a normal character, pipe it through.
					*o++ = *i;
					continue;
				}
				
				while ( i[1] == '/' ) // Skip consecutive slashes.
					i++;
				
				if ( i[1] == '.' )
				{
					if ( i[2] == '/' )
					{
						i += 1;
						if (seg) seg--;
					}
					else if ( i[2] == '.' && i[3] == '/' )
					{
						i += 2;
						if (seg)
						{
							seg -= 2;
							while ( o > s )
							{
								if ( *--o == '/' )
								{
									break;
								}
							}
						}
						else if (!abs)
						{
							seg--;
							if ( o != s ) // No slash for first character.
								*o++ = '/';
							*o++ = '.';
							*o++ = '.';
						}
						else seg--;
					}
				}
				else if ( i < s ) ; // Only to check if it is a path.
				else
				{
					if ( o != s )
						*o++ = *i;
				}
			}
			*o = '\0';
			e = o;
			
			if (abs) s--;
		}
		
		if ( s == e )
		{
			printf("%s", delim);
			continue;
		}
		
		if ( act == ACT_FULL )
		{
			if ( slash == SLASH_NO && e[-1] == '/' )
				e[-1] = '\0';
			
			if ( rloc == RLOC_REL )
			{
				//@TODO: implement.
			}
			
			fputs(s, stdout);
			
			if ( slash == SLASH_YES && e[-1] != '/' )
				fputc('/', stdout);
			
			fputs(delim, stdout);
		}
		else
		{
			o = s;
			i = e;
			if ( slash != SLASH_YES )
				while ( i[-1] == '/' )
					*--i = '\0'; // Ignore trailing slash.
			
			char *ext = e;
			if ( act != ACT_LAST )
			{
				while ( --i >= s && *i != '/' )
				{
					if ( *i == '.' )
					{
						e = i;
						if ( smart )
						{
							char *l = i-4;
							if ( i-4 < s ) i = s;
							while ( --i >= l )
							{
								if ( *i == '/' || *i == '.' )
								{
									i++;
									l = NULL; // Mark that we found something.
									break;
								}
							}
							if (!l)
								continue;
							else
								break;
						}
						else break;
					}
				}
				i++; // By doing this we guarantee that i is not pointing at a /
			}
			
			if ( act == ACT_EXT )
			{
				s = e+1;
			}
			else // ACT_LAST || ACT_BASE
			{
				if ( act == ACT_BASE )
					*e = '\0';
				
				while ( --i >= s && *i != '/' )
					;
				
				s = i+1;
			}
			
			printf("%s%s", s, delim);
		}
	}
	exit(EX_OK);
}

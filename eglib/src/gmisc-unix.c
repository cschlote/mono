/*
 * gmisc.c: Misc functions with no place to go (right now)
 *
 * Author:
 *   Aaron Bockover (abockover@novell.com)
 *
 * (C) 2006 Novell, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <glib.h>
#include <pthread.h>

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif


const gchar *
g_getenv(const gchar *variable)
{
	return getenv(variable);
}

gboolean
g_setenv(const gchar *variable, const gchar *value, gboolean overwrite)
{
	return setenv(variable, value, overwrite) == 0;
}

void
g_unsetenv(const gchar *variable)
{
	unsetenv(variable);
}

gchar*
g_win32_getlocale(void)
{
	return NULL;
}

gboolean
g_path_is_absolute (const char *filename)
{
	g_return_val_if_fail (filename != NULL, FALSE);

	return (*filename == '/');
}

static pthread_mutex_t home_lock = PTHREAD_MUTEX_INITIALIZER;
static const gchar *home_dir;

/* Give preference to /etc/passwd than HOME */
const gchar *
g_get_home_dir (void)
{
	if (home_dir == NULL){
		pthread_mutex_lock (&home_lock);
		if (home_dir == NULL){
#ifdef HAVE_GETPWENT_R
			struct passwd pwbuf, *track;
			char buf [4096];
			uid_t uid;
			
			uid = getuid ();

			setpwent ();
			
			while (getpwent_r (&pwbuf, buf, sizeof (buf), &track) == 0){
				if (pwbuf.pw_uid == uid){
					home_dir = g_strdup (pwbuf.pw_dir);
					break;
				}
			}
			endpwent ();
#endif
			if (home_dir == NULL)
				home_dir = g_getenv ("HOME");
			pthread_mutex_unlock (&home_lock);
		}
	}
	return home_dir;
}

const char *
g_get_user_name (void)
{
	const char *retName = g_getenv ("USER");
	if (!retName)
		retName = "somebody";
	return retName;
}

static const char *tmp_dir;

static pthread_mutex_t tmp_lock = PTHREAD_MUTEX_INITIALIZER;

const gchar *
g_get_tmp_dir (void)
{
	if (tmp_dir == NULL){
		pthread_mutex_lock (&tmp_lock);
		if (tmp_dir == NULL){
			tmp_dir = g_getenv ("TMPDIR");
			if (tmp_dir == NULL){
				tmp_dir = g_getenv ("TMP");
				if (tmp_dir == NULL){
					tmp_dir = g_getenv ("TEMP");
					if (tmp_dir == NULL)
						tmp_dir = "/tmp";
				}
			}
		}
		pthread_mutex_unlock (&tmp_lock);
	}
	return tmp_dir;
}


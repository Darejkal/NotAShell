#include <windows.h>
#include <ios>
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#include <cstdint>
namespace ms
{

void RedirectStandardIo()
{
  /* This clever code have been found at:
  Adding Console I/O to a Win32 GUI App
  Windows Developer Journal, December 1997
  http://dslweb.nwnexus.com/~ast/dload/guicon.htm
  Andrew Tucker's Home Page */

  // redirect unbuffered STDOUT to the console
  intptr_t lStdHandle = (intptr_t)GetStdHandle(STD_OUTPUT_HANDLE);
  int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  FILE *fp = _fdopen(hConHandle, "w");
  *stdout = *fp;
  setvbuf(stdout, NULL, _IONBF, 4096);

  // redirect unbuffered STDIN to the console
  lStdHandle = (intptr_t)GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen( hConHandle, "r" );
  *stdin = *fp;
  setvbuf(stdin, NULL, _IONBF, 4096);

  // redirect unbuffered STDERR to the console
  lStdHandle = (intptr_t)GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen( hConHandle, "w" );
  *stderr = *fp;
  setvbuf(stderr, NULL, _IONBF, 4096);

  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
  std::ios::sync_with_stdio();
}
}
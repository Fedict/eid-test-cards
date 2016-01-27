#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#if defined(RTLD_NEXT)
#define REAL_LIBDL RTLD_NEXT
#else
#define REAL_LIBDL ((void *) -1L)
#endif

// NULL terminated string array containg the possible locations of the lib to be proxied
const char *REAL_LIBS[] = {
	"REALPCSCLIB",  // needed when we proxy dlopen() -- need if libpcsclite is loaded dynamically (see ../dlopenproxy/) 
#ifdef MAC_OS_X
	"/System/Library/Frameworks/PCSC.framework/PCSC",
#elif defined LINUX64BIT
	"/usr/lib64/libpcsclite.so.1",
	"/lib/x86_64-linux-gnu/libpcsclite.so.1",
	"/lib64/libpcsclite.so.1",
#else
	"/usr/lib/libpcsclite.so.1",
	"/lib/i386-linux-gnu/libpcsclite.so.1",
	"/lib/libpcsclite.so.1",
#endif
	NULL
};

#ifdef __APPLE__
#define PROXY_PCSC_PATH "/usr/share/eidtestinfra/PCSC"
#define PCSC_NAME "PCSC"
#else
#define PROXY_PCSC_PATH "/usr/share/eidtestinfra/libpcsclite.so"
#define PCSC_NAME "libpcsclite.so"
#endif

static const char *find_real_pcsc_path();

void* dlopen(const char *file, int mode)
{
	const char *loadfile = file;
	static void* (*o_dlopen) (const char *, int) = 0;

	// If an attempt is made to load the PCSC lib,
	// we'll change into loading the proxy PCSC lib
	if (file == NULL)
		loadfile == NULL;
	else if (strcmp(file, "REALPCSCLIB") == 0)
	{
		printf("==> proxy dlopen: PCSC proxy is loading the real PCSC lib\n");
		loadfile = find_real_pcsc_path();
	}
	else if (strstr(file, PCSC_NAME) != NULL)
	{
		printf("==> proxy dlopen: loading the proxy PCSC lib\n");
		loadfile = PROXY_PCSC_PATH;
	}

	o_dlopen = (void* (*) (const char *, int)) dlsym(REAL_LIBDL, "dlopen");

	return (*o_dlopen)(loadfile, mode);
}

static const char *find_real_pcsc_path()
{
	int i;
	for (i = 0; REAL_LIBS[i]; i++)
	{
	  FILE *f = fopen(REAL_LIBS[i], "r");
	  if (f)
	  {
	    fclose(f);
	    printf("  found the real PCSC lib: %s\n", REAL_LIBS[i]);
	    return REAL_LIBS[i];
	  }
	  
  }
  printf("ERR: Couldn't find the path to the real PCSC lib!\n");
  return NULL;
}


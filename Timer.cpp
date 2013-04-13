//=============================================================================
// adapted from vclibs/base/{time,printf}.cc -- (C) Christian Roessl 2010
//=============================================================================

#include "Timer.hpp"

#include <time.h>
#include <cstdio>
#include <stdarg.h>
//#include <cstdarg> // MinGW complains, fallback to old-style header
#include <cmath>
#include <cstdlib>

#if !defined(__APPLE__)
# include <malloc.h> // Linux, Windows
#else
# include <sys/malloc.h> // Mac OS X
#endif

//#include "printf.hh"

//== IMPLEMENTATION ===========================================================

using namespace std;

namespace util {
//=============================================================================


# if defined(_MSC_VER)
// Visual C++ does not want us to use vsnprintf.
#  pragma warning (push)
#  pragma warning (disable: 4290)
#  pragma warning (disable: 4996)
# endif

const unsigned MINBUFSIZE = 1024;

// sprintf into std::string
static string formatf(const char* _format,...) {
  va_list ap;
  va_start(ap,_format);

  char   buf[MINBUFSIZE];
  char*  p=buf;
  int    n=vsnprintf(p,sizeof(buf),_format,ap);

  if (n>int(sizeof(buf))) {
    p=(char*) alloca(n);
    n=vsnprintf(p,n,_format,ap); // do not dare to write into resized string
  }

  va_end(ap);

  if (n<0)
    return string("ERROR: formatf failed");

  return string(p);
}

# if defined(_MSC_VER)
#  pragma warning (pop)
# endif

//-----------------------------------------------------------------------------


#if __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_POSIX_RT_CLOCK

static inline double _get_cpu_time_resolution() {
  struct timespec res;
  int rv=clock_getres(CLOCK_PROCESS_CPUTIME_ID,&res);
  assert(rv==0 && "EINVAL?! clk_id specified is not supported on this system?!");

  return double(res.tv_sec)+double(res.tv_nsec)*1e-9;
}

#elif __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_POSIX_TIMES

long cpu_time_t::sm_clk_tck=sysconf(_SC_CLK_TCK);

static inline double _get_cpu_time_resolution() {
  return 1.0/double(sysconf(_SC_CLK_TCK));
}

#elif __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_POSIX_CLOCK

static inline double _get_cpu_time_resolution() {
  return 1.0/double(CLOCKS_PER_SEC);
}

#elif __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_WIN_HPC

static LARGE_INTEGER _get_win_hpc_frequency() {
  LARGE_INTEGER freq;
  int rv=-1;
  if ((rv=QueryPerformanceFrequency(&freq))==0 || freq.QuadPart==1000) {
    // Fail on emulation! (freq==1000)
    fprintf(stderr,
            "Failed to initialize high resolution timers%s.\n"
      "Recompile with -D__VC_DONT_USE_RT_CLOCK!\n"
            "Abort in %s (%d)",
            freq.QuadPart==1000 ? " (emulation)":"",__FILE__,__LINE__);
    abort();
  }
  // WARNING:
  // I don't know if the PC frequency remains constant as the returned
  // value seems to be related to the CPU clock frequency, which may
  // change during operation (power management).
  // CHECK RELIABILITY!

  return freq;
}

static inline double _get_cpu_time_resolution() {
  // undefined initialization order! (sm_frequency)
  LARGE_INTEGER freq=_get_win_hpc_frequency();
  return 1.0/double(freq.QuadPart);
}

LARGE_INTEGER  cpu_time_t::sm_frequency=_get_win_hpc_frequency();

#else
# error "invalid __VC_BASE_CPU_TIME_CLOCK"
#endif

double cpu_time_t::sm_resolution = _get_cpu_time_resolution();

//-----------------------------------------------------------------------------

string cpu_time_t::to_s(const char* _fpformat) const {
    if (_fpformat==0)
    _fpformat="%.5lg%s";

  double t=seconds();

  if (t<9.9999e-9) { // nanoseconds
    return formatf(_fpformat,t*1e9,"ns");
  }
  if (t<9.9999e-6) { // microseconds
    return formatf(_fpformat,t*1e6,"us");
  }
  else if (t<0.099999) { // milliseconds
    return formatf(_fpformat,t*1000.0,"ms");
  } else if (t<300.0) {
    return formatf(_fpformat,t,"s");
  }

  const double h1=3600.0;
  const double h2=2.0*3600.0;
  const double d1=24.0*3600.0;
  const double d2=48.0*3600.0;


  if (t>=d2) { // days
    double d=floor(t/d1);
    t-=d*d1;
    double h=floor(t/h1);
    t-=h*h1;
    double m=floor(t/60.0);
    t-=m*60.0;
    double s=t;
    return formatf("%dd:%dh:%dm:%lg.2s",int(d),int(h),int(m),s);
  }
  else if (t>=h2) { // hours
    double h=floor(t/h1);
    t-=h*h1;
    double m=floor(t/60.0);
    t-=m*60.0;
    double s=t;
    return formatf("%dh:%dm:%lg.2s",int(h),int(m),s);
  }
  else if (t>=300.0) { // minutes
    double m=floor(t/60.0);
    t-=m*60.0;
    double s=t;
    return formatf("%dm:%.2lgs",int(m),s);
  }

  assert("!unreachable");
  return formatf(_fpformat,t,"s"); // has been tested before (see above)
}

//-----------------------------------------------------------------------------

StopWatch __global_StopWatch_global;

cpu_time_diff_t StopWatch::toc(std::ostream& _os,const char* _msg) {
  cpu_time_diff_t dt=toc();
  _os << formatf(_msg,dt.to_s().c_str());
  return dt;
}

//-----------------------------------------------------------------------------


//=============================================================================
} // namespace util

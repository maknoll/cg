//=============================================================================
// adapted from vclibs/base/time.hh -- (C) Christian Roessl 2010
//=============================================================================

#ifndef AACGVC_BASE_TIME_HH
#define AACGVC_BASE_TIME_HH


//== INCLUDES =================================================================

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <string>
#include <iostream>

# if !(defined(_WIN32) || defined(_WIN64))
#  include <unistd.h>
#  include <time.h>
# endif

# if (defined(__MINGW32__) || defined(__MINGW64__))
#   define __VC_DONT_USE_RT_CLOCK
# endif

//== CLASS DEFINITION =========================================================

namespace util {

# define __VC_BASE_POSIX_RT_CLOCK 1
# define __VC_BASE_POSIX_TIMES 2
# define __VC_BASE_POSIX_CLOCK 0
# define __VC_BASE_WIN_HPC 3

# if (defined(_POSIX_TIMERS) && _POSIX_TIMERS>0) && !defined(__VC_DONT_USE_RT_CLOCK)
#   define __VC_BASE_CPU_TIME_CLOCK  __VC_BASE_POSIX_RT_CLOCK
# else
#  if !defined(_WIN32) && !defined(_WIN64) && 0 // AVOID THIS! resolution 1/100s
#    include <sys/times.h>
#    define __VC_BASE_CPU_TIME_CLOCK  __VC_BASE_POSIX_TIMES
#  endif
# endif

# if (defined(_WIN32) || defined(_WIN64)) && !defined(__VC_DONT_USE_RT_CLOCK)
#   include <windows.h>
#   define __VC_BASE_CPU_TIME_CLOCK __VC_BASE_WIN_HPC
# endif

# ifndef __VC_BASE_CPU_TIME_CLOCK
#  define __VC_BASE_CPU_TIME_CLOCK   __VC_BASE_POSIX_CLOCK
# endif


/** \defgroup vc_base_time Measuring time
    \ingroup vc_base
*/

class cpu_time_diff_t;

/** \class cpu_time_t time.hh
    \brief Yields per-process CPU time.
    \ingroup vc_base_time


    cpu_time_t measures timestamps (CPU time, not wall clock), it is
    designed for timing short intervals (\sa cpu_time_diff_t).

    The static function cpu_time_t::seconds() returns the time in
    seconds since an unspecified starting time.

    \arg High resolution timers are used if possible. For POSIX systems
    supporting \c _POSIX_TIMERS,
    \c clock_gettime(CLOCK_PROCESS_CPUTIME_ID) is called. (Mind the notes on
    SMP systems.)
    \arg On all other systems the (low resolution) POSIX \c clock() is used.
    Note that this is prone to overflow/wrap around even for descent time
    intervals!

    Use cpu_time_diff_t for measuring time \a differences. Note that
    differences of seconds() (\c double) may suffer from lack of
    numerical precision, wheras cpu_time_diff_t() does not.
 */
class cpu_time_t {
public:

  enum { CLOCK = __VC_BASE_CPU_TIME_CLOCK };

  /// create timestamp
  cpu_time_t();

  /// resultion() is meaningful only if high resolution timers are used
  static bool is_highres() {
    return (CLOCK==__VC_BASE_POSIX_RT_CLOCK) || (CLOCK==__VC_BASE_WIN_HPC);
  }
  /// get resolution in seconds
  static double resolution() { return sm_resolution; }

  /// get time
  double seconds() const;

  /// same as seconds
  operator double() const { return seconds(); }

  /** Get a human readable string representation.
      Selects the best unit
      \arg nanoseconds "ns"
      \arg milliseconds "ms"
      \arg seconds "s" (up to 300 before switching to minutes:seconds)
      \arg minutes "min" (up to 120 before switching to hours)
      \arg hours "h" (up to 48 before switching to days)
      \arg days "d"

      and constructs a string  "##d:#h:#m:#s" or "#{ns|ms}"
      (# stands for numbers), zero entries will not be suppressed.
      \param _fpformat sprintf-like format string for floating point
      numbers (double) and their unit (string) shown for s,ns,ms.
      Default is "%.4lg%s".
      \return formatted string
   */
  std::string to_s(const char* _fpformat=0) const;

  /** @name global stop watch

      tic() and toc() implement a simple global stop watch as in
      Matlab.

      <br>This simplified version is not thread-safe! Threads would
      share the global StopWatch.</b>

      Calls are delegated to StopWatch::global().

      \sa StopWatch

      @{
   */

  /// start global stop watch (StopWatch::tic())
  static void tic();

  /// stop global stop watch (StopWatch::toc())
  static cpu_time_diff_t toc();
  /// same, additonally base::oprintf() _msg to _os with %s as to_s()
  static cpu_time_diff_t toc(std::ostream& _os,
           const char* _msg="elapsed time %s\n");

  /// @}

private:
  friend class cpu_time_diff_t;

# if __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_POSIX_RT_CLOCK
  struct timespec m_tp;    //!< timestamp
# elif __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_POSIX_TIMES
  clock_t         m_clock;
  struct tms      m_tms;      //!< timestamp
  static long     sm_clk_tck; //!< clock ticks
# elif __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_POSIX_CLOCK
  clock_t         m_clock; //!< real time as returned by clock
# elif __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_WIN_HPC
  static LARGE_INTEGER sm_frequency;
  LARGE_INTEGER        m_count;
# else
#  error "invalid __VC_BASE_CPU_TIME_CLOCK"
# endif

  static double     sm_resolution; //!< clock resolution
};

//-----------------------------------------------------------------------------

/** \class cpu_time_diff_t time.hh
    \brief Difference of cpu_time_t time-stamps
    \ingroup vc_base_time
 */
class cpu_time_diff_t : public cpu_time_t {
public:
  /// get difference to _reference
  cpu_time_diff_t(const cpu_time_t& _reference);
};

//-----------------------------------------------------------------------------

# if __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_POSIX_RT_CLOCK

  //
  // POSIX real-time clock
  //

inline cpu_time_t::cpu_time_t() {
  //int rv=clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&m_tp);
  int rv=clock_gettime(CLOCK_THREAD_CPUTIME_ID,&m_tp);
  //int rv=clock_gettime(CLOCK_REALTIME,&m_tp);
  assert(rv==0 && "EINVAL?! clk_id specified is not supported on this system?!");
  rv=rv; // force use, avoid warning
}
inline cpu_time_diff_t::cpu_time_diff_t(const cpu_time_t& _reference) {
  m_tp.tv_sec-=_reference.m_tp.tv_sec;
  m_tp.tv_nsec-=_reference.m_tp.tv_nsec;
}
inline double cpu_time_t::seconds() const {
  return double(m_tp.tv_sec)+double(m_tp.tv_nsec)*1e-9;
}

# elif __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_POSIX_TIMES

  //
  // POSIX times()
  //

inline cpu_time_t::cpu_time_t() {
  m_clock=times(&m_tms);
}
inline cpu_time_diff_t::cpu_time_diff_t(const cpu_time_t& _reference) {
  m_clock-=_reference.m_clock;
  m_tms.tms_utime -=_reference.m_tms.tms_cutime;
  m_tms.tms_stime -=_reference.m_tms.tms_stime;
  m_tms.tms_cutime-=_reference.m_tms.tms_cutime;
  m_tms.tms_cstime-=_reference.m_tms.tms_cstime;
}
inline double cpu_time_t::seconds() const {
  return double(m_tms.tms_utime+m_tms.tms_stime)+double(sm_clk_tck);
}
# elif __VC_BASE_CPU_TIME_CLOCK == __VC_BASE_POSIX_CLOCK

  //
  // POSIX clock()
  //

inline cpu_time_t::cpu_time_t() {
  m_clock=std::clock();
}
inline cpu_time_diff_t::cpu_time_diff_t(const cpu_time_t& _reference) {
  m_clock-=_reference.m_clock;
}
inline double cpu_time_t::seconds() const {
  return double(m_clock)/double(CLOCKS_PER_SEC);
}

# elif __VC_BASE_CPU_TIME_CLOCK==__VC_BASE_WIN_HPC

  //
  // Windows performance counters
  //

inline cpu_time_t::cpu_time_t()  {
  int rv=QueryPerformanceCounter(&m_count);
  assert(rv!=0 && "failed to query performance counter");
}

inline cpu_time_diff_t::cpu_time_diff_t(const cpu_time_t& _reference) {
  m_count.QuadPart -= _reference.m_count.QuadPart;
}

inline double cpu_time_t::seconds() const {
  return double(m_count.QuadPart)/double(sm_frequency.QuadPart);
}

# else
#  error "invalid __VC_BASE_CPU_TIME_CLOCK"
# endif

//-----------------------------------------------------------------------------

/** \class StopWatch time.hh
    \brief Simple stop watch based on cpu_time_t and cpu_time_diff_t.
    \ingroup vc_base_time
 */
class StopWatch {
public:
  /// start stop watch
  void tic() { m_time=cpu_time_t(); }
  /// stop watch
  cpu_time_diff_t toc() const {
    return cpu_time_diff_t(m_time);
  }
  /// same, and base::oprintf() _msg to _os with %s as cpu_time_t::to_s()
  cpu_time_diff_t toc(std::ostream& _os,
          const char* _msg="elapsed time %s\n");

  /// get global stop watch used by cpu_time_t::tic() (per thread instance)
  static StopWatch& global();

private:
  cpu_time_t m_time;
};

# ifndef DOXYGEN_SKIP
  extern StopWatch __global_StopWatch_global;
# endif

inline StopWatch& StopWatch::global() {
  return __global_StopWatch_global;
}

//-----------------------------------------------------------------------------

inline void cpu_time_t::tic() { StopWatch::global().tic(); }
inline cpu_time_diff_t cpu_time_t::toc() { return StopWatch::global().toc(); }
inline cpu_time_diff_t cpu_time_t::toc(std::ostream& _os,const char* _msg) {
  return StopWatch::global().toc(_os,_msg);
}

//-----------------------------------------------------------------------------

/** Stream output of VC::base::cpu_time_t
    \ingroup vc_base_time
*/
template <typename T,unsigned K>
std::ostream& operator<<(std::ostream& _s,const ::util::cpu_time_t& _t) {
    return _s << _t.to_s();
}

//=============================================================================
} // namespace util
//=============================================================================
#endif // AACGVC_BASE_TIME_HH defined

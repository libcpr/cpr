#ifndef CPR_PROGRESSCB_H
#define CPR_PROGRESSCB_H

namespace cpr {

class ProgressCallback {
  #if LIBCURL_VERSION_NUM < 0x072000 // older than 7.32.0 uses the older callback.
  typedef int(*progress_cb_t)(void*, double, double, double, double);
  #else
  typedef int(*progress_cb_t)(void*, int64_t, int64_t, int64_t, int64_t);
  #endif

  public:
    ProgressCallback() = default;
    ProgressCallback(const progress_cb_t cb) : cb{cb} {}
    ProgressCallback(const progress_cb_t cb, void * userpt)
          : cb{cb}, userpt{userpt} {}

    progress_cb_t cb;
    void *userpt = nullptr;
};

} // namespace cpr

#endif
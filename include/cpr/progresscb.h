#ifndef CPR_PROGRESSCB_H
#define CPR_PROGRESSCB_H

namespace cpr {

class ProgressCallback {
  typedef int(*progress_cb_t)(void*, int64_t, int64_t, int64_t, int64_t);

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
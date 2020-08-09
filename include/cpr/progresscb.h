#ifndef CPR_PROGRESSCB_H
#define CPR_PROGRESSCB_H

#include "cprtypes.h"

namespace cpr {

typedef struct {
  void* userData = nullptr;
  #if LIBCURL_VERSION_NUM < 0x072000
  double downloadNow, downloadTotal, uploadNow, uploadTotal;
  #else
  cpr_off_t downloadNow, downloadTotal, uploadNow, uploadTotal;
  #endif
} ProgressCallbackUser;

typedef int(*progress_cb_t)(ProgressCallbackUser*);

class ProgressCallback {
  private:
    typedef struct {
    progress_cb_t cb;
    ProgressCallbackUser data;
  } ProgressCallbackData;

  #if LIBCURL_VERSION_NUM < 0x072000
  int ProgressCallbackFunction(ProgressCallbackData* data, double dlnow, double dltotal, double ulnow, double ultotal) {
  #else
  int ProgressCallbackFunction(ProgressCallbackData* data, cpr_off_t dlnow, cpr_off_t dltotal, cpr_off_t ulnow, cpr_off_t ultotal) {
  #endif
    data->data.downloadNow = dlnow;
    data->data.downloadTotal = dltotal;
    data->data.uploadNow = ulnow;
    data->data.uploadTotal = ultotal;
    return data->cb(&data->data);
  }

  public:
    ProgressCallback() = default;
    ProgressCallback(const progress_cb_t cb) { callback.cb = cb; }
    ProgressCallback(const progress_cb_t cb, void* userData) {
      callback.cb = cb;
      callback.data.userData = userData;
    }

    void AddCallback(const progress_cb_t cb) { callback.cb = cb; }
    void AddUserData(void* userData) { callback.data.userData = userData; }

    ProgressCallbackData callback;
};

} // namespace cpr

#endif
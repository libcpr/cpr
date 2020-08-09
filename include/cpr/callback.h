#ifndef CPR_CALLBACK_H
#define CPR_CALLBACK_H

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

typedef struct {
  void* userData = nullptr;
  char* buffer;
  size_t realsize, size, nmeb;
} WriteCallbackUser;

typedef struct {
  void* userData = nullptr;
  char* buffer;
  size_t realsize, size, nmeb;
} ReadCallbackUser;

typedef int(*progress_cb_t)(ProgressCallbackUser*);
typedef int(*write_cb_t)(WriteCallbackUser*);
typedef int(*read_cb_t)(ReadCallbackUser*);

class ReadCallback {
  private:
    typedef struct {
    read_cb_t cb;
    ReadCallbackUser data;
  } ReadCallbackData;
  
  size_t ReadCallbackFunction(char* ptr, size_t size, size_t nmemb, ReadCallbackData* data) {
    data->data.buffer = ptr;
    data->data.size = size;
    data->data.nmeb = nmemb;
    data->data.realsize = size * nmemb;
    return data->cb(&data->data);
  }

  public:
    ReadCallbackData callback;
};

class WriteCallback {
  private:
    typedef struct {
    write_cb_t cb;
    WriteCallbackUser data;
  } WriteCallbackData;

  size_t WriteCallbackFunction(char* ptr, size_t size, size_t nmemb, WriteCallbackData* data) {
    data->data.buffer = ptr;
    data->data.size = size;
    data->data.nmeb = nmemb;
    data->data.realsize = size * nmemb;
    return data->cb(&data->data);
  }

  public:
    WriteCallbackData callback;
};

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
#ifndef XLogModule_H
#define XLogModule_H

#include <napi.h>
#include <xlogger_interface.h>

using namespace std;
using namespace mars;

class XLogModule : public Napi::ObjectWrap<XLogModule>
{
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    explicit XLogModule(const Napi::CallbackInfo &info);

    Napi::Value SetLevel(const Napi::CallbackInfo &info);
    Napi::Value Info(const Napi::CallbackInfo &info);
    Napi::Value Debug(const Napi::CallbackInfo &info);
    Napi::Value Verbose(const Napi::CallbackInfo &info);
    Napi::Value Warn(const Napi::CallbackInfo &info);
    Napi::Value Error(const Napi::CallbackInfo &info);
    Napi::Value Fatal(const Napi::CallbackInfo &info);
    Napi::Value Release(const Napi::CallbackInfo &info);

  private:
    string namePrefix;
    comm::XloggerCategory *category;
    TLogLevel ConvertToLogLevel(const char *level);
    void Write(TLogLevel logLevel, const char *_log);
};

#endif // XLogModule
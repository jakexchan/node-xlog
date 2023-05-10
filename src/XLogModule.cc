#include "XLogModule.h"
#include "mars/comm/xlogger/xlogger_category.h"

Napi::Object XLogModule::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function func = DefineClass(
      env,
      "XLogModule",
      {InstanceMethod<&XLogModule::SetLevel>("setLevel"),
       InstanceMethod<&XLogModule::Info>("info"),
       InstanceMethod<&XLogModule::Debug>("debug"),
       InstanceMethod<&XLogModule::Verbose>("verbose"),
       InstanceMethod<&XLogModule::Warn>("warn"),
       InstanceMethod<&XLogModule::Error>("error"),
       InstanceMethod<&XLogModule::Fatal>("fatal"),
       InstanceMethod<&XLogModule::Release>("release")});
  Napi::FunctionReference *constructor = new Napi::FunctionReference();

  *constructor = Napi::Persistent(func);
  exports.Set("XLogModule", func);
  env.SetInstanceData<Napi::FunctionReference>(constructor);
  return exports;
}

XLogModule::XLogModule(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XLogModule>(info)
{
  Napi::Env env = info.Env();

  if (info[0].IsUndefined())
  {
    Napi::TypeError::New(env, "options is required").ThrowAsJavaScriptException();
  }

  Napi::Object options = info[0].As<Napi::Object>().ToObject();
  if (options.Get("namePrefix").IsUndefined() || !options.Get("namePrefix").IsString())
  {
    Napi::TypeError::New(env, "options.namePrefix is required, string type").ThrowAsJavaScriptException();
  }
  if (options.Get("logDir").IsUndefined() || !options.Get("logDir").IsString())
  {
    Napi::TypeError::New(env, "options.logDir is required, string type").ThrowAsJavaScriptException();
  }

  string namePrefix = options.Get("namePrefix").As<Napi::String>().ToString();
  string logdir_ = options.Get("logDir").As<Napi::String>().ToString();

  this->namePrefix = namePrefix;
  xlog::XLogConfig config;
  config.logdir_ = logdir_;
  config.nameprefix_ = namePrefix;

  // set mode by async configuration
  if (options.Get("async").IsBoolean())
  {
    bool isAsync = options.Get("async").As<Napi::Boolean>().ToBoolean();
    config.mode_ = isAsync ? xlog::TAppenderMode::kAppenderAsync : xlog::TAppenderMode::kAppenderSync;
  }
  else
  {
    // default mode
    config.mode_ = xlog::TAppenderMode::kAppenderAsync;
  }

  if (options.Get("pubKey").IsString())
  {
    config.pub_key_ = options.Get("pubKey").As<Napi::String>().ToString();
  }
  else
  {
    config.pub_key_ = "";
  }

  config.compress_mode_ = xlog::TCompressMode::kZlib;
  config.compress_level_ = 0;
  config.cachedir_ = "";
  config.cache_days_ = 0;

  this->category = xlog::NewXloggerInstance(config, TLogLevel::kLevelAll);
}

TLogLevel XLogModule::ConvertToLogLevel(const char *level)
{
  TLogLevel logLevel = this->category->GetLevel();
  if (std::strcmp(level, "all") == 0)
  {
    logLevel = TLogLevel::kLevelAll;
  }
  else if (std::strcmp(level, "verbose") == 0)
  {
    logLevel = TLogLevel::kLevelVerbose;
  }
  else if (std::strcmp(level, "debug") == 0)
  {
    logLevel = TLogLevel::kLevelDebug;
  }
  else if (std::strcmp(level, "info") == 0)
  {
    logLevel = TLogLevel::kLevelInfo;
  }
  else if (std::strcmp(level, "warn") == 0)
  {
    logLevel = TLogLevel::kLevelWarn;
  }
  else if (std::strcmp(level, "error") == 0)
  {
    logLevel = TLogLevel::kLevelError;
  }
  else if (std::strcmp(level, "fatal") == 0)
  {
    logLevel = TLogLevel::kLevelFatal;
  }
  else if (std::strcmp(level, "none") == 0)
  {
    logLevel = TLogLevel::kLevelNone;
  }

  return logLevel;
}

Napi::Value XLogModule::SetLevel(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (this->category != nullptr)
  {
    comm::XloggerCategory *category = this->category;
    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "The argument must be one of all, verbose, debug, info, warn, error, fatal").ThrowAsJavaScriptException();
    }
    else
    {
      std::string levelStr = info[0].As<Napi::String>().ToString();
      TLogLevel logLevel = this->ConvertToLogLevel(levelStr.c_str());
      category->SetLevel(logLevel);
    }
  }
  return env.Undefined();
}

void XLogModule::Write(TLogLevel logLevel, const char *_log)
{
  comm::XloggerCategory *category = this->category;
  XLoggerInfo logInfo;
  logInfo.pid = xlogger_pid();
  logInfo.maintid = xlogger_maintid();
  logInfo.tid = xlogger_tid();
  logInfo.level = logLevel;
  category->Write(&logInfo, _log);
}

Napi::Value XLogModule::Info(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::string logStr = info[0].As<Napi::String>().ToString();
  this->Write(TLogLevel::kLevelInfo, logStr.c_str());
  return env.Undefined();
}

Napi::Value XLogModule::Debug(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::string logStr = info[0].As<Napi::String>().ToString();
  this->Write(TLogLevel::kLevelDebug, logStr.c_str());
  return env.Undefined();
}

Napi::Value XLogModule::Verbose(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::string logStr = info[0].As<Napi::String>().ToString();
  this->Write(TLogLevel::kLevelVerbose, logStr.c_str());
  return env.Undefined();
}

Napi::Value XLogModule::Warn(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::string logStr = info[0].As<Napi::String>().ToString();
  this->Write(TLogLevel::kLevelWarn, logStr.c_str());
  return env.Undefined();
}

Napi::Value XLogModule::Error(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::string logStr = info[0].As<Napi::String>().ToString();
  this->Write(TLogLevel::kLevelError, logStr.c_str());
  return env.Undefined();
}

Napi::Value XLogModule::Fatal(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::string logStr = info[0].As<Napi::String>().ToString();
  this->Write(TLogLevel::kLevelFatal, logStr.c_str());
  return env.Undefined();
}

Napi::Value XLogModule::Release(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  xlog::ReleaseXloggerInstance(this->namePrefix.c_str());
  return env.Undefined();
}

Napi::Object InitNodeModule(Napi::Env env, Napi::Object exports)
{
  return XLogModule::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitNodeModule);
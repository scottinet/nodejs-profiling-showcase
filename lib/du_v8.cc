#include <v8.h>
#include <node.h>
#include <nan.h>
#include "du.h"

NAN_METHOD(scan) {
  v8::Local<v8::String> path = info[0].As<v8::String>();
  v8::Local<v8::Array> returnValue = Nan::New<v8::Array>();

  Nan::Utf8String c_pathstr(path);
  unsigned long long *res = scan(*c_pathstr);

  // convert 64 bits int to string
  char buffer[128];
  snprintf(buffer, 128, "%llu", res[0]);

  Nan::Set(returnValue, 0, Nan::New(buffer).ToLocalChecked());
  Nan::Set(returnValue, 1, Nan::New((int32_t)res[1]));

  free(res);
  info.GetReturnValue().Set(returnValue);
}

NAN_MODULE_INIT(init) {
  Nan::Set(target, Nan::New("scan").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(scan)).ToLocalChecked());
}

NODE_MODULE(du, init)

#pragma once

#include <string>
using std::string;

#include "http_protocol.h"

class Foo 
{
  public:
    int RunHandler(request_rec *req) 
    {
      if (!req->handler || module_name != req->handler) return DECLINED;

      counter++;
      ap_rputs(message.c_str(), req);
      return OK;
    }
  private:
    int counter = 0 ;
    string module_name = "foo";
    string message = "Hello world from " + module_name + " " + counter;
};

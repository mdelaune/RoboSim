#include "run.h"

Run::Run() {}

QString Run::getRuntimeString(){
    return runtime[0] + ":" + runtime[1] + "." + runtime[2];
}

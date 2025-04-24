#ifndef RUN_H
#define RUN_H

#include <QString>

class Run
{
public:
    Run();
    QString alg;
    bool exists;
    QString runtime[3];
    float coversf;
    float coverPer;
    QString getRuntimeString();
};

#endif // RUN_H

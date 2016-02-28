#ifndef SAFERELEASES_H
#define SAFERELEASES_H



template <class F1, class F2>class safereleases
{
    F1 _f1;
    F2 _f2;
public:
    safereleases(F1 f1, F2 f2):_f1(f1),_f2(f2){(f1)();};
    ~safereleases(){(_f2)();};
};

#endif // SAFERELEASES_H

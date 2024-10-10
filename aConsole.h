#ifndef ACONSOLE_H
#define ACONSOLE_H

class AConsole {
public:
    virtual ~AConsole() {}
    virtual void initialize() = 0;
};

#endif // ACONSOLE_H

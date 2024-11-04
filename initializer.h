#ifndef INITIALIZER_H
#define INITIALIZER_H

class Initializer {
public:
    Initializer();
    void setInitialized();
    bool isInitialized() const;

private:
    bool initialized;
};

#endif // INITIALIZER_H

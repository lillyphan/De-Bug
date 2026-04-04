#ifndef TERMINAL_H
#define TERMINAL_H

class Terminal {
public:
    virtual ~Terminal() = default;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual bool isFinished() const = 0;
};

#endif
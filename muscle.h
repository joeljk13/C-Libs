#ifndef MUSCLE_H_
#define MUSCLE_H_ 1

enum muscle_props {

};

struct muscle {
    struct ptrvec *exercises;
    enum muscle_props props;
};

#endif

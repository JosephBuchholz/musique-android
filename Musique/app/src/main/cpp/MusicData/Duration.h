#ifndef MUSIQUE_DURATION_H
#define MUSIQUE_DURATION_H

struct Duration {

    Duration() {}
    Duration(float duration) : duration(duration) {}

    float duration = 1.0f;
};

#endif // MUSIQUE_DURATION_H
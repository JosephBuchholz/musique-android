#ifndef MUSIQUE_CHORDDEGREE_H
#define MUSIQUE_CHORDDEGREE_H

#include "../BaseElements/PrintableElement.h"
#include "../String.h"
#include "../Pitch.h"
#include "../Types.h"

class Degree : public VisibleElement {

public:
    unsigned int degree = 1;
};

class DegreeAlter : public VisibleElement {

public:
    Alter alter = 0.0f;
};

class DegreeType : public VisibleElement {

public:
    enum class Type {
        None = 0, Add, Alter, Subtract
    } type = Type::None;
};

class ChordDegree : public PrintableElement {

public:
    ChordDegree() {}

public:
    Degree degree;
    DegreeAlter degreeAlter;
    DegreeType degreeType;
};

#endif // MUSIQUE_CHORDDEGREE_H
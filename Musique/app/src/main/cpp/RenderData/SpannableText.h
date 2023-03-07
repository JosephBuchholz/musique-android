#ifndef MUSIQUE_SPANNABLETEXT_H
#define MUSIQUE_SPANNABLETEXT_H

#include <string>
#include <vector>

#include "Paint.h"

/**
 * An object that represents a span for spannable text
 */
struct TextSpan
{
    TextSpan() {}

    TextSpan(unsigned int startIndex, unsigned int endIndex, Paint paint = Paint())
            : startIndex(startIndex), endIndex(endIndex), paint(paint) {
    }

    unsigned int startIndex = 0;
    unsigned int endIndex = 0;
    Paint paint = Paint();
};

/**
 * An object that holds data needed to render a spannable piece of text.
 */
struct SpannableText {

    SpannableText() {}

    SpannableText(std::vector<uint16_t> text, float x, float y, std::vector<TextSpan> spans, Paint mainPaint = Paint())
            : x(x), y(y), spans(spans), mainPaint(mainPaint)
            {

        textSize = text.size();

        // allocate memory for the utf16 string 'text'
        this->text = new uint16_t[text.size()];

        // copy the vector 'text' in to the utf16 string 'text'
        uint16_t* tempPtr = this->text;
        for (uint16_t c : text)
        {
            (*tempPtr) = c;
            tempPtr++;
        }
    }

    ~SpannableText()
    {
        delete[] text;
    }

    uint16_t* text;
    unsigned int textSize = 0;
    //std::string text = "";
    float x = 0.0f;
    float y = 0.0f;
    Paint mainPaint = Paint();
    std::vector<TextSpan> spans;
};

#endif // MUSIQUE_SPANNABLETEXT_H
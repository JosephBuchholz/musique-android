#ifndef MUSIQUE_SPANNABLETEXT_H
#define MUSIQUE_SPANNABLETEXT_H

#include <string>
#include <vector>

#include "Paint.h"
#include "../Collisions/Vec2.h"

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

    SpannableText(std::vector<uint16_t> text, Vec2<float> position, std::vector<TextSpan> spans, Paint mainPaint = Paint())
            : position(position), spans(spans), mainPaint(mainPaint)
    {

        textSize = text.size();

        // allocate memory for the utf16 string 'text'
        this->text = new uint16_t[text.size()];

        // copy the vector 'text' in to the utf16 string 'text'
        uint16_t* tempPtr = this->text;
        for (uint16_t c: text)
        {
            (*tempPtr) = c;
            tempPtr++;
        }
    }

    SpannableText(const SpannableText& spannableText)
            : position(spannableText.position), spans(spannableText.spans), mainPaint(spannableText.mainPaint)
    {
        textSize = spannableText.textSize;

        // allocate memory for the utf16 string 'text'
        this->text = new uint16_t[textSize];

        // copy the text in spannableText to this->text
        memcpy(this->text, spannableText.text, textSize * sizeof(uint16_t));
    }

    SpannableText(SpannableText&& spannableText)
            : position(spannableText.position), spans(spannableText.spans), mainPaint(spannableText.mainPaint),
            textSize(spannableText.textSize), text(spannableText.text)
    {
        spannableText.text = nullptr;
    }

    ~SpannableText()
    {
        delete[] text;
    }

    uint16_t* text;
    unsigned int textSize = 0;
    Vec2<float> position;
    Paint mainPaint = Paint();
    std::vector<TextSpan> spans;
};

#endif // MUSIQUE_SPANNABLETEXT_H
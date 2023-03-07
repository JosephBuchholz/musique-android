#ifndef MUSIQUE_SYSTEM_H
#define MUSIQUE_SYSTEM_H

/**
 * A class that holds data for a system on a page of sheet music
 */
class System
{
public:
    struct SystemLayout {
        float systemDistance = 127.0f;
        float topSystemDistance = 127.0f;

        float systemLeftMargin = 0.0f;
        float systemRightMargin = 0.0f;
    };

public:
    SystemLayout layout = SystemLayout();

    bool showTimeSignature = false;
    float timeSignaturePositionX = 0.0f; // relative to the start of the start of the system/start of first measure

    bool showClef = true;
    float clefPositionX = 0.0f; // relative to the start of the start of the system/start of first measure

    bool showKeySignature = true;
    float keySignaturePositionX = 0.0f; // relative to the start of the start of the system/start of first measure
};

#endif // MUSIQUE_SYSTEM_H
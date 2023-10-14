/**
 * This file contains the definition of the 'SoundEvent' class.
 */

#ifndef MUSIQUE_SOUNDEVENT_H
#define MUSIQUE_SOUNDEVENT_H

#include <vector>

class SwingTempo
{
public:
    float first = 2.0f;
    float second = 1.0f;
    enum class SwingType
    {
        None = 0, Straight, Eighth, Sixteenth
    } swingType = SwingType::Straight;
};

/**
 * This class represents a event that happens in time
 * (usually associated with a direction), like 'pizz.',
 * 'Swing', 'arco', dynamic markings, and any tempo markings.
 */
class SoundEvent
{
public:

    float beatPosition = 0.0f; // the position in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position in the song(not counting repeats) in beats(quarter notes)

    /**
     * Modifies tempo if event needs to.
     *
     * @param tempo The value to modify.
     */
    virtual void ModifyTempo(float& tempo) {}

    /**
     * Modifies the velocity if event needs to.
     *
     * @param velocity The value to modify.
     */
    virtual void ModifyVelocity(float& velocity) {}

    /**
     * Modifies the swing tempo if event needs to.
     *
     * @param swing The value to modify.
     */
    virtual void ModifySwingTempo(SwingTempo& swing) {}

    /**
     * Modifies pizzicato state if event needs to.
     *
     * @param pizzicato The value to modify.
     */
    virtual void ModifyPizzicato(bool& pizzicato) {}

    /**
     * Modifies capo fret if event needs to.
     *
     * @param capo The value to modify.
     */
    virtual void ModifyCapo(uint32_t& capo) {}

    std::vector<int> timeOnly; // These values indicate what times this event should happen for multiple repeats. If empty, then this event happens on all repeats.
};

class TempoSoundEvent : public SoundEvent
{
public:

    void ModifyTempo(float& tempo) override { tempo = this->tempo; }

    float tempo = 120.0f; // beats per minute
};

class DynamicsSoundEvent : public SoundEvent
{
public:
    void ModifyVelocity(float& velocity) override { velocity = this->velocity; }

    void SetDynamics(float d) { dynamics = d; velocity = d * 90.0f; }
    void SetVelocity(float v) { velocity = v; dynamics = v / 90.0f; }

private:
    float dynamics = 1.0f; // 1.0 is about a forte
    float velocity = 90.0f; // 90 is about a forte
};

class SwingSoundEvent : public SoundEvent
{
public:
    void ModifySwingTempo(SwingTempo& swing) override { swing = this->swing; }

    SwingTempo swing;
};

class PizzicatoSoundEvent : public SoundEvent
{
public:
    void ModifyPizzicato(bool& pizzicato) override { pizzicato = this->pizzicato; }

    bool pizzicato = false;
};

class CapoSoundEvent : public SoundEvent
{
public:
    void ModifyCapo(uint32_t& capo) override { capo = this->capo; }

    uint32_t capo = 0;
};

/*class JumpSoundEvent : public SoundEvent
{
public:
    bool daCapo = false; // jump to the beginning
    bool dalSegno = false; // jump to segno

    // coda
    // fine
    // segno

    bool toCoda = false; // jump to coda
};*/

#endif // MUSIQUE_SOUNDEVENT_H
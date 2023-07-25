/**
 * This file contains the definition of the 'SoundEvent' class.
 */

#ifndef MUSIQUE_SOUNDEVENT_H
#define MUSIQUE_SOUNDEVENT_H

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

    virtual void ModifyTempo(float& tempo) {}
    virtual void ModifyVelocity(float& velocity) {}
    virtual void ModifySwingTempo(SwingTempo& swing) {}
    virtual void ModifyPizzicato(bool& pizzicato) {}
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

#endif // MUSIQUE_SOUNDEVENT_H
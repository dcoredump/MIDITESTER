#include <MIDI.h>
#include <looper.h>
#include <LinkedList.h>
#include "midinotes.h"

#define LED 13
#define LED_DECAY_MS 50
#define MIDI_EVENT_SCHED_MS 100

#define TEST_NOTE_MIN MIDI_A0
#define TEST_NOTE_MAX MIDI_B5
#define TEST_VEL_MIN 60
#define TEST_VEL_MAX 120
#define TEST_DUR_MIN 200
#define TEST_DUR_MAX 1500

#define MASTER_KEY_MIDI    MIDI_C6
#define MASTER_NUM1        MIDI_C1

class midi_event {
  public:
    int8_t note;
    int8_t chan;
    uint32_t dur;
};

looper sched;
MIDI_CREATE_DEFAULT_INSTANCE();
LinkedList<midi_event*> MIDIEvents = LinkedList<midi_event*>();

void setup()
{
  /*Serial.begin(38400);*/
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  MIDI.begin(1);

  sched.addJob(ledDecay, LED_DECAY_MS);
  sched.addJob(do_midi_events, MIDI_EVENT_SCHED_MS);

  randomSeed(analogRead(A0));

  /*
    // Sound change 17
    MIDI.sendNoteOn(MASTER_KEY_MIDI, 99, 1);
    delay(500);
    MIDI.sendNoteOn(MIDI_E3, 66, 1);
    delay(500);
    MIDI.sendNoteOff(MIDI_E3, 0, 1);
    delay(500);
    MIDI.sendNoteOff(MASTER_KEY_MIDI, 0, 1);

    delay(2000);

    // Volume change 0.5
    MIDI.sendNoteOn(MASTER_KEY_MIDI, 99, 1);
    delay(500);
    MIDI.sendNoteOn(MIDI_AIS1, 66, 1);
    delay(500);
    MIDI.sendNoteOff(MIDI_AIS1, 0, 1);
    delay(500);
    MIDI.sendNoteOff(MASTER_KEY_MIDI, 0, 1);

    delay(2000);

    // Bank change 3
    MIDI.sendNoteOn(MASTER_KEY_MIDI, 99, 1);
    delay(500);
    MIDI.sendNoteOn(MIDI_FIS3, 66, 1);
    delay(500);
    MIDI.sendNoteOff(MIDI_FIS3, 0, 1);
    delay(500);
    MIDI.sendNoteOff(MASTER_KEY_MIDI, 0, 1);

    delay(2000);

    // Sound change 17
    MIDI.sendNoteOn(MASTER_KEY_MIDI, 99, 1);
    delay(500);
    MIDI.sendNoteOn(MIDI_E3, 66, 1);
    delay(500);
    MIDI.sendNoteOff(MIDI_E3, 0, 1);
    delay(500);
    MIDI.sendNoteOff(MASTER_KEY_MIDI, 0, 1);

    delay(2000);

    // Volume change 0.1
    MIDI.sendNoteOn(MASTER_KEY_MIDI, 99, 1);
    delay(500);
    MIDI.sendNoteOn(MIDI_DIS1, 66, 1);
    delay(500);
    MIDI.sendNoteOff(MIDI_DIS1, 0, 1);
    delay(500);
    MIDI.sendNoteOff(MASTER_KEY_MIDI, 0, 1);

    delay(5000);
  */
}

void loop()
{
  if (millis() % 300 == 0)
  {
    sendMIDI(random(TEST_NOTE_MIN, TEST_NOTE_MAX), random(TEST_VEL_MIN, TEST_VEL_MAX), 1, random(TEST_DUR_MIN, TEST_DUR_MAX));
    delay(1);
  }
  sched.scheduler();
}

void sendMIDI(int8_t note, int8_t vel, int8_t chan, uint32_t dur)
{
  midi_event *m = new midi_event();
  m->note = note;
  m->chan = chan;
  m->dur = millis() + dur;
  MIDIEvents.add(m);
  digitalWrite(LED, HIGH);
  MIDI.sendNoteOn(note, vel, chan);
  /*
    Serial.print(F("Sending MIDI on channel "));
    Serial.print(chan);
    Serial.print(F(" note "));
    Serial.print(note);
    Serial.print(F(" velocity "));
    Serial.print(vel);
    Serial.print(F(" duration "));
    Serial.print(dur);
    Serial.println();*/
}

void ledDecay(void)
{
  digitalWrite(LED, LOW);
}

void do_midi_events(void)
{
  uint8_t i;

  for (i = 0; i < MIDIEvents.size(); i++)
  {
    midi_event *m = MIDIEvents.get(i);

    if (millis() > m->dur)
    {
      MIDI.sendNoteOff(m->note, 0, m->chan);
      MIDIEvents.remove(i);
    }
  }
}

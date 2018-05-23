#include <MIDI.h>
#include <looper.h>
#include <LinkedList.h>

#define LED 13
#define LED_DECAY_MS 50
#define MIDI_EVENT_SCHED_MS 100

#define TEST_NOTE_MIN 40
#define TEST_NOTE_MAX 110
#define TEST_VEL_MIN 60
#define TEST_VEL_MAX 120
#define TEST_DUR_MIN 150
#define TEST_DUR_MAX 1000

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

void sendMIDI(int8_t note, int8_t vel, int8_t chan, int8_t dur)
{
  midi_event *m = new midi_event();
  m->note = note;
  m->chan = chan;
  m->dur = millis() + dur;
  MIDIEvents.add(m);
  digitalWrite(LED, HIGH);
  MIDI.sendNoteOn(note, vel, chan);
/*  Serial.print(F("Sending MIDI on channel "));
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

    // If its a mammal, then print it's name
    if (m->dur < millis())
    {
      MIDI.sendNoteOff(m->note, 0, m->chan);
      MIDIEvents.remove(i);
    }
  }
}

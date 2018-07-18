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

#define MASTER_KEY_MIDI    MIDI_C7
#define MASTER_NUM1        MIDI_C2

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

  // Bank change 1
  change_bank(1);
  delay(2000);
  
  // Sound change 17
  change_sound(17);
  delay(2000);

  // Volume change 0.5
  change_volume(5);
  delay(2000);

  // Bank change 3
  change_bank(3);
  delay(2000);

  // Sound change 17
  change_sound(17);
  delay(2000);

  // Volume change 0.1
  change_volume(1);
  delay(5000);
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

void change_sound(uint8_t sound)
{
  uint8_t sound_key = base_c_num_key(sound);

  MIDI.sendNoteOn(MASTER_KEY_MIDI, 99, 1);
  delay(500);
  MIDI.sendNoteOn(sound_key, 66, 1);
  delay(500);
  MIDI.sendNoteOff(sound_key, 0, 1);
  delay(500);
  MIDI.sendNoteOff(MASTER_KEY_MIDI, 0, 1);
}

void change_volume(uint8_t volume)
{
  uint8_t volume_key = base_c_num_key(volume*-1);

  MIDI.sendNoteOn(MASTER_KEY_MIDI, 99, 1);
  delay(500);
  MIDI.sendNoteOn(volume_key, 66, 1);
  delay(500);
  MIDI.sendNoteOff(volume_key, 0, 1);
  delay(500);
  MIDI.sendNoteOff(MASTER_KEY_MIDI, 0, 1);
}

void change_bank(uint8_t bank)
{
  uint8_t bank_key = base_c_num_key(bank*-1)+24;

  MIDI.sendNoteOn(MASTER_KEY_MIDI, 99, 1);
  delay(500);
  MIDI.sendNoteOn(bank_key, 66, 1);
  delay(500);
  MIDI.sendNoteOff(bank_key, 0, 1);
  delay(500);
  MIDI.sendNoteOff(MASTER_KEY_MIDI, 0, 1);
}

uint8_t base_c_num_key(int8_t number)
{
  uint8_t base_c = 0;

  if (number < 0)
  {
    number = abs(number)-1;
    base_c = int(number / 5) * 12 + MASTER_NUM1;
    switch (number % 5)
    {
      case 0:
        base_c += 1;
        break;
      case 1:
        base_c += 3;
        break;
      case 2:
        base_c += 6;
        break;
      case 3:
        base_c += 8;
        break;
      case 4:
        base_c += 10;
        break;
    }
  }
  else
  {
    number -= 1;
    base_c = (int(number / 7) * 12) + MASTER_NUM1;
    switch (number % 7)
    {
      case 0:
        base_c += 0;
        break;
      case 1:
        base_c += 2;
        break;
      case 2:
        base_c += 4;
        break;
      case 3:
        base_c += 5;
        break;
      case 4:
        base_c += 7;
        break;
      case 5:
        base_c += 9;
        break;
      case 6:
        base_c += 11;
        break;
    }
  }

  return (base_c);
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

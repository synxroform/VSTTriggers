#include "SC_PlugIn.h"
#include "SC_UnitDef.h"

using namespace std;

static InterfaceTable* ft;


#define POLY 16

struct MessageUnit : public Unit {
    Unit* _uvst;
    UnitCmd* _ucmd;
};


struct VSTNote : public MessageUnit {
    int _active_dur[POLY];
    int _active_num[POLY];
    int _active_chn[POLY];
    int _step;
    float _prevtrig;
};

struct VSTProgram : public MessageUnit {
    float _prevtrig;
};

extern "C" {

void VSTNote_Ctor(VSTNote *unit);
void VSTNote_next(VSTNote *unit, int nsamp);

void VSTProgram_Ctor(VSTProgram *unit);
void VSTProgram_next(VSTProgram *unit, int nsamp);

}

////////////////////////////////////////////////////////////////////////////////////////////////

int next_active(int* notes, int start, size_t len) {
    for (int n = 0; n <= len; n++) {
        if (notes[(start + n) % len] <= 0) return (start + n) % len;
    }
    return -1;
}


void send_midi(MessageUnit* unit, char status, char data1, char data2) {
    char msg[8] {',', 'm', '\0', '\0', status, data1, data2, '\0'};
    sc_msg_iter arg(8, msg);
    if (unit->_ucmd->mFunc)
        (unit->_ucmd->mFunc)(unit->_uvst, &arg);
}


void MessageUnit_Ctor(MessageUnit *unit) {
    int32 cmdname[kSCNameLen];
    memset(cmdname, 0, kSCNameByteLen);
    strcpy((char*)cmdname, "/midi_msg");

    unit->_uvst = unit->mInput[0]->mFromUnit;
    unit->_ucmd = unit->_uvst->mUnitDef->mCmds->Get(cmdname);
}


void VSTNote_Ctor(VSTNote *unit) {
    SETCALC(VSTNote_next);
    MessageUnit_Ctor(unit);    
    memset(unit->_active_dur, 0, sizeof(float) * POLY);
    memset(unit->_active_num, 0, sizeof(float) * POLY);
    memset(unit->_active_chn, 0, sizeof(float) * POLY);
    unit->_step = 0;
    unit->_prevtrig = 0.f;
}


void VSTNote_next(VSTNote *unit, int nsamp) {
    float trig = ZIN0(1);

    if (trig > 0.f && unit->_prevtrig <= 0.f) {
        char velo = (char)(abs(trig * 255));
        char note = (char)(ZIN0(2));
        char chan = (char)(ZIN0(4));
        float dur = ZIN0(3);

        int idx = next_active(unit->_active_dur, unit->_step, POLY);
        if (idx >= 0) {
            unit->_active_dur[idx] = 
                (int)((dur * SAMPLERATE) / BUFLENGTH);
            unit->_active_num[idx] = note;
            unit->_active_chn[idx] = chan;

            send_midi(unit, 0x90 | chan, note, velo);
            unit->_step += 1;
        }
    }
    for (int n = 0; n < POLY; n++) {
        if (unit->_active_dur[n] > 0) {
            if (unit->_active_dur[n] - 1 == 0) {
                send_midi(unit, 0x80 | unit->_active_chn[n], unit->_active_num[n], 0x00);
            }
            unit->_active_dur[n] -= 1;
        }
    }
    unit->_prevtrig = trig;
}


void VSTProgram_Ctor(VSTProgram *unit) {
    SETCALC(VSTProgram_next);
    MessageUnit_Ctor(unit);
    unit->_prevtrig = 0.f;
}

void VSTProgram_next(VSTProgram *unit, int nsamp) {
    float trig = ZIN0(1);
    if (trig > 0.f && unit->_prevtrig <= 0.f) {
        char prog = (char)(ZIN0(2));
        char chan = (char)(ZIN0(3));

        send_midi(unit, 0xc0 | chan, prog, 0x00);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


PluginLoad(VSTTriggers) {
    ft = inTable;

    DefineSimpleUnit(VSTNote);
    DefineSimpleUnit(VSTProgram);
}

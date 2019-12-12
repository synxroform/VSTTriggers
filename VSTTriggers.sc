VSTNote : UGen {
    *kr { | vsti, trig, note, dur=0.5, chan=0|
        ^this.multiNew('control', vsti, trig, note, dur, chan);
    }
}

VSTProgram : UGen {
    *kr { | vsti, trig, prog, chan=0 |
        ^this.multiNew('control', vsti, trig, prog, chan);
    }
}

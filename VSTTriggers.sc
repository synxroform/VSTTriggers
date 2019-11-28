VSTNote : UGen {
    *kr { | vsti, trig, note, dur, chan|
        ^this.multiNew('control', vsti, trig, note, dur, chan);
    }
}

VSTProgram : UGen {
    *kr { | vsti, trig, prog, chan |
        ^this.multiNew('control', vsti, trig, prog, chan);
    }
}
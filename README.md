# VSTTriggers
This units allows you to send note and program messages to VSTPlugin from within synth.

```supercollider
(SynthDef(\test, {
	var vsti = VSTPlugin.ar(nil, 2);
	var trig = Impulse.kr(4);
	VSTNote.kr(vsti, trig * WhiteNoise.kr, TChoose.kr(trig, 170 + [10, 13, 15, 20, 8]), 0.1, 0);
  VSTProgram.kr(vsti, trig, TChoose.kr(trig, [1, 2, 3, 4]), 0);
	Out.ar(0, Greyhole.ar(vsti) * 0.3);
}).add)

~pc = VSTPluginController.new(Synth.new(\test)).open("VOPMex", editor: true);
```

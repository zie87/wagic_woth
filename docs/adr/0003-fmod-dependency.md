# 3. Fmod dependency

Date: 2023-04-05

## Status

Accepted

## Context

Currently the audio output for windows 32-bit is implemented via FMod. FMod is 
a proprietery sound effects engine. Currently only Windows builds use this engine 
and the binaries are only included for 32 bit builds. All desktop builds should 
use the same engine and the main architecture should be 64 bit.

## Decision

Replace FMod with an open source alternative which supports at least linux and 
windows and 32 and 64 bit.

## Consequences

Until the replacement is done neither windows or linux will have audio output.

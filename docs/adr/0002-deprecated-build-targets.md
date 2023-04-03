# 2. Deprecated build targets

Date: 2023-04-03

## Status

Accepted

## Context

Currently multiple deprecated build targets are supported. This contains outdated 
Android builds, support for Meego/Maemo and iOS with a deprecated toolchain.

## Decision

The effort to support/update this systems in the current point in time is to 
high. The targets can be removed from the source directories and the dependencies 
can be removed from the code. This means following targets:

* Android
* iOS
* Meego/Maemo
* Qt4 build

## Consequences

If the targets should be supported in the future, they need to be integrated again.

=============
RELEASE NOTES
=============
.. This work is licensed under a Creative Commons Attribution 4.0 International License.
.. SPDX-License-Identifier: CC-BY-4.0
..
.. CAUTION: this document is generated from source in doc/src/*
.. To make changes edit the source and recompile the document.
.. Do NOT make changes directly to .rst or .md files.



C++ Framework Release Notes
===========================
The following is a list of release highlights for the C++
xAPP Framework.

Cherry Release
==============

2020 July 30; version 2.2.0
---------------------------
Add support for configuration file reading and change
notification.


2020 July 22; version 2.1.0
---------------------------
Added metrics support (RIC-381).


2020 July 17; version 2.0.0
---------------------------
Add alarm support.

Add xapp namespace for Message, Msg_component, Alarm and Jhah
objects. This is a BREAKING change and thus the major version
number was bumped to 2.


2020 June 29; version 1.2.0
---------------------------
Add support for json parsing


2020 June 26; version 1.1.0
---------------------------
Change the port type in constructors to indicate "const"

Version bump to 1.1.0 to allow patches to bronze code to
continue to be done on the 1.0.* level.



Bronze Release
==============

2020 April 28; version 1.0.0
----------------------------
Bump version to force package build (old CI version added
incorrect install prefix). Bump to 1.0.0 for release.


2020 March 27; version 0.1.2
----------------------------
Changes identified by sonar (missing copy/move builders)
rmr_dump example programme Improvements to code for better
test coverage

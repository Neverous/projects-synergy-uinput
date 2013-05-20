Synergy-uinput 0.2.1
==========

Relatively simple implementation of [Synergy](http://synergy-foss.org/pl/) protocol for use with uinput as client.
It allows to use keyboard and mouse in pure terminal without need for Xorg - useful for example for [Raspberry Pi](http://www.raspberrypi.org/) where you can use it with XBMC.

Requirements
----------

* uinput kernel module.

Install
----------

Use `make` and it will generate `synergy-uinput` binary in main directory.


Usage
----------

Type `synergy-uinput -h` to get usage information, they are pretty straightforward.

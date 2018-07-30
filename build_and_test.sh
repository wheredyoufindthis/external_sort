#!/usr/bin/env bash
git submodule update --init
mkdir build; cd build; cmake .. && make && make test

#!/bin/bash

if [[ -e "./RigelEngineDebug" ]]; then
  rm RigelEngineDebug 
fi
  make && ./RigelEngineDebug


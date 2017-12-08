#!/bin/bash
export PATH="$HOME/opt/cross/bin:$PATH"
stack runhaskell -- shake.hs $@

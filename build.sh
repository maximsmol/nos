#!/bin/bash
# export PATH"/usr/local/opt/llvm:$PATH"
stack runhaskell -- shake.hs $@

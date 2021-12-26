#!/usr/bin/env python

# Copyright (c) 2021, StarGate, Inc.
# All rights reserved.
#
# Ibrahim ERTURK <ierturk@ieee.org>
#
# SPDX-License-Identifier: BSD-3-Clause

import os
# Yocto and python27 combination
if "IS_YOCTO" in os.environ:
    from . import erpc_version
else:
    from . import transport_ext



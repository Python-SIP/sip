# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from importlib.metadata import version
from packaging.version import parse


SIP_VERSION_STR = version('sip')

_parsed = parse(SIP_VERSION_STR)
SIP_VERSION = (_parsed.major << 16) + (_parsed.minor << 8) + _parsed.micro

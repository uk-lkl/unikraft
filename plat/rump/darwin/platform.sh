#!/bin/sh

export OS=darwin

appendvar EXTRA_CPPFLAGS ' '
appendvar EXTRA_CFLAGS ' '
appendvar EXTRA_LDFLAGS '-Wl,-L.'
appendvar EXTRA_CWARNFLAGS ''
appendvar F_DBG ' '

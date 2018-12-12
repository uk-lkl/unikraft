# Makefile for Unikraft
#
# Copyright (C) 1999-2005 by Erik Andersen <andersen@codepoet.org>
# Copyright (C) 2006-2014 by the Buildroot developers <buildroot@uclibc.org>
# Copyright (C) 2014-2016 by the Buildroot developers <buildroot@buildroot.org>
# Copyright (C) 2016-2017 by NEC Europe Ltd. <simon.kuenzer@neclab.eu>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

# Trick for always running with a fixed umask
UMASK = 0022
ifneq ($(shell umask),$(UMASK))
.PHONY: _all $(MAKECMDGOALS)

$(MAKECMDGOALS): _all
	@:

_all:
	@umask $(UMASK) && $(MAKE) --no-print-directory $(MAKECMDGOALS)

else # umask

# This is our default rule, so must come first
.PHONY: all
all:

# Save running make version
RUNNING_MAKE_VERSION := $(MAKE_VERSION)

# Check for minimal make version (note: this check will break at make 10.x)
MIN_MAKE_VERSION = 4.1
ifneq ($(firstword $(sort $(RUNNING_MAKE_VERSION) $(MIN_MAKE_VERSION))),$(MIN_MAKE_VERSION))
$(error You have make '$(RUNNING_MAKE_VERSION)' installed. GNU make >= $(MIN_MAKE_VERSION) is required)
endif

# Strip quotes and then whitespaces
qstrip = $(strip $(subst ",,$(1)))
#"))

# Variables for use in Make constructs
comma := ,
empty :=
space := $(empty) $(empty)

# bash prints the name of the directory on 'cd <dir>' if CDPATH is
# set, so unset it here to not cause problems. Notice that the export
# line doesn't affect the environment of $(shell ..) calls, so
# explictly throw away any output from 'cd' here.
export CDPATH :=

# Use current directory as base
CONFIG_UK_BASE := $(CURDIR)
override CONFIG_UK_BASE := $(realpath $(CONFIG_UK_BASE))
ifeq ($(CONFIG_UK_BASE),)
$(error "Invalid base directory (CONFIG_UK_BASE)")
endif

# A // APP_DIR
# Set A variable if not already done on the command line;
ifneq ("$(origin A)", "command line")
A := $(CONFIG_UK_BASE)
endif
# Remove the trailing '/.'
# Also remove the trailing '/' the user can set when on the command line.
override A := $(realpath $(patsubst %/,%,$(patsubst %.,%,$(A))))
ifeq ($(A),)
$(error "Invalid app directory (A)")
endif
override CONFIG_UK_APP   := $(A)
override APP_DIR  := $(A)
override APP_BASE := $(A)

# BUILD_DIR
# Use O variable if set on the command line, otherwise use $(A)/build;
ifneq ("$(origin O)", "command line")
BUILD_DIR := $(shell mkdir -p $(CONFIG_UK_APP)/build && cd $(CONFIG_UK_APP)/build >/dev/null && pwd)
$(if $(BUILD_DIR),, $(error could not create directory "$(A)/build"))
else
BUILD_DIR := $(shell mkdir -p $(O) && cd $(O) >/dev/null && pwd)
$(if $(BUILD_DIR),, $(error could not create directory "$(O)"))
endif
override BUILD_DIR := $(realpath $(patsubst %/,%,$(patsubst %.,%,$(BUILD_DIR))))

# ELIB_DIR (list of external libraries)
# Retrieved from L variable from the command line (paths separated by colon)
ifeq ("$(origin L)", "command line")
# library path exists?
$(foreach E,$(subst :, ,$(L)), \
$(if $(wildcard $(E)), \
	$(eval ELIB_DIR += $(E)) \
, \
	$(error Cannot find library: $(E)) \
) \
)
endif
ELIB_DIR := $(realpath $(patsubst %/,%,$(patsubst %.,%,$(ELIB_DIR))))

# KConfig settings
CONFIG_DIR            := $(CONFIG_UK_APP)
CONFIG_CONFIG_IN      := $(CONFIG_UK_BASE)/Config.uk
CONFIG                := $(CONFIG_UK_BASE)/support/kconfig
UK_CONFIG             := $(CONFIG_DIR)/.config
UK_CONFIG_OUT         := $(BUILD_DIR)/config
UK_GENERATED_INCLUDES := $(BUILD_DIR)/include
KCONFIG_DIR           := $(BUILD_DIR)/kconfig
UK_FIXDEP             := $(KCONFIG_DIR)/fixdep
KCONFIG_AUTOCONFIG    := $(KCONFIG_DIR)/auto.conf
KCONFIG_TRISTATE      := $(KCONFIG_DIR)/tristate.config
KCONFIG_AUTOHEADER    := $(UK_GENERATED_INCLUDES)/uk/_config.h
KCONFIG_APP_IN        := $(KCONFIG_DIR)/app.uk
KCONFIG_ELIB_IN       := $(KCONFIG_DIR)/elib.uk

# Makefile support scripts
SCRIPTS_DIR := $(CONFIG_UK_BASE)/support/scripts

# # Set and export the version string
include $(CONFIG_UK_BASE)/version.mk

# Compute the full local version string so packages can use it as-is
# Need to export it, so it can be got from environment in children (eg. mconf)
export UK_FULLVERSION := $(UK_VERSION).$(UK_SUBVERSION)$(UK_EXTRAVERSION)$(shell $(SCRIPTS_DIR)/gitsha1)

# Default image name
export CONFIG_UK_NAME ?= $(notdir $(APP_DIR))

export DATE := $(shell date +%Y%m%d)

# Makefile targets
null_targets		:= print-version help
noconfig_targets	:= menuconfig nconfig gconfig xconfig config oldconfig randconfig \
			   defconfig %_defconfig allyesconfig allnoconfig silentoldconfig release \
			   olddefconfig $(null_targets)

# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands
ifeq ("$(origin V)", "command line")
  BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
  BUILD_VERBOSE = 0
endif

ifeq ($(KBUILD_VERBOSE),1)
  Q =
ifndef VERBOSE
  VERBOSE = 1
endif
export VERBOSE
else
   Q = @
endif

# we want bash as shell
SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	 else if [ -x /bin/bash ]; then echo /bin/bash; \
	 else echo sh; fi; fi)

# kconfig uses CONFIG_SHELL
CONFIG_SHELL := $(SHELL)
export SHELL CONFIG_SHELL Q KBUILD_VERBOSE

################################################################################
# .config
################################################################################
# Initialize important internal variables
UK_FETCH:=
UK_FETCH-y:=
UK_PREPARE:=
UK_PREPATE-y:=
UK_PLATS:=
UK_PLATS-y:=
UK_LIBS:=
UK_LIBS-y:=
UK_ALIBS:=
UK_ALIBS-y:=
UK_OLIBS:=
UK_OLIBS-y:=
UK_SRCS:=
UK_SRCS-y:=
UK_DEPS:=
UK_DEPS-y:=
UK_OBJS:=
UK_OBJS-y:=
UK_IMAGES:=
UK_IMAGES-y:=
UK_CLEAN :=
UK_CLEAN-y :=
ASFLAGS :=
ASFLAGS-y :=
ASINCLUDES :=
ASINCLUDES-y :=
CFLAGS :=
CFLAGS-y :=
CINCLUDES :=
CINCLUDES-y :=
CXXFLAGS :=
CXXFLAGS-y :=
CXXINCLUDES :=
CXXINCLUDES-y :=
LDFLAGS :=
LDFLAGS-y :=
IMAGE_LDFLAGS :=
IMAGE_LDFLAGS-y :=

# Pull in the user's configuration file
ifeq ($(filter $(noconfig_targets),$(MAKECMDGOALS)),)
ifneq ("$(wildcard $(UK_CONFIG))","")
-include $(UK_CONFIG)
UK_HAVE_DOT_CONFIG := y
endif
endif

# remove quotes from CONFIG_UK_NAME
CONFIG_UK_NAME := $(call qstrip,$(CONFIG_UK_NAME))

################################################################################
# Host compiler and linker tools
################################################################################
ifndef HOSTAR
HOSTAR := ar
endif
ifndef HOSTAS
HOSTAS := as
endif
ifndef HOSTCC
HOSTCC := gcc
HOSTCC := $(shell which $(HOSTCC) || type -p $(HOSTCC) || echo gcc)
endif
HOSTCC_NOCCACHE := $(HOSTCC)
ifndef HOSTCXX
HOSTCXX := g++
HOSTCXX := $(shell which $(HOSTCXX) || type -p $(HOSTCXX) || echo g++)
endif
HOSTCXX_NOCCACHE := $(HOSTCXX)
ifndef HOSTCPP
HOSTCPP := cpp
endif
ifndef HOSTLD
HOSTLD := ld
endif
ifndef HOSTLN
HOSTLN := ln
endif
ifndef HOSTNM
HOSTNM := nm
endif
ifndef HOSTOBJCOPY
HOSTOBJCOPY := objcopy
endif
ifndef HOSTRANLIB
HOSTRANLIB := ranlib
endif
HOSTAR		:= $(shell which $(HOSTAR) || type -p $(HOSTAR) || echo ar)
HOSTAS		:= $(shell which $(HOSTAS) || type -p $(HOSTAS) || echo as)
HOSTCPP		:= $(shell which $(HOSTCPP) || type -p $(HOSTCPP) || echo cpp)
HOSTLD		:= $(shell which $(HOSTLD) || type -p $(HOSTLD) || echo ld)
HOSTLN		:= $(shell which $(HOSTLN) || type -p $(HOSTLN) || echo ln)
HOSTNM		:= $(shell which $(HOSTNM) || type -p $(HOSTNM) || echo nm)
HOSTOBJCOPY	:= $(shell which $(HOSTOBJCOPY) || type -p $(HOSTOBJCOPY) || echo objcopy)
HOSTRANLIB	:= $(shell which $(HOSTRANLIB) || type -p $(HOSTRANLIB) || echo ranlib)
HOSTCC_VERSION	:= $(shell $(HOSTCC_NOCCACHE) --version | \
		   sed -n -r 's/^.* ([0-9]*)\.([0-9]*)\.([0-9]*)[ ]*.*/\1 \2/p')

# For gcc >= 5.x, we only need the major version.
ifneq ($(firstword $(HOSTCC_VERSION)),4)
HOSTCC_VERSION	:= $(firstword $(HOSTCC_VERSION))
endif

# Determine the userland we are running on.
#
export HOSTARCH := $(shell LC_ALL=C $(HOSTCC_NOCCACHE) -v 2>&1 | \
		   sed -e '/^Target: \([^-]*\).*/!d' \
		       -e 's//\1/' \
		       -e 's/i.86/x86/' \
		       -e 's/sun4u/sparc64/' \
		       -e 's/arm64.*/arm64/' -e 's/aarch64.*/arm64/' \
		       -e '/arm64/! s/arm.*/arm/' \
		       -e 's/sa110/arm/' \
		       -e 's/ppc64/powerpc64/' \
		       -e 's/ppc/powerpc/' \
		       -e 's/macppc/powerpc/' \
		       -e 's/sh.*/sh/' )
export HOSTAR HOSTAS HOSTCC HOSTCC_VERSION HOSTCXX HOSTLD HOSTARCH
export HOSTCC_NOCCACHE HOSTCXX_NOCCACHE


################################################################################
# Unikraft Architecture
################################################################################
# Set target archicture as set in config
include $(CONFIG_UK_BASE)/arch/Arch.uk
ifeq ($(CONFIG_UK_ARCH),)
# Set target archicture as set in environment
ifneq ($(ARCH),)
export CONFIG_UK_ARCH	?= $(shell echo "$(call qstrip,$(ARCH))" | \
		   sed -e "s/-.*//" \
		       -e 's//\1/' \
		       -e 's/i.86/x86/' \
		       -e 's/sun4u/sparc64/' \
		       -e 's/arm64.*/arm64/' -e 's/aarch64.*/arm64/' \
		       -e '/arm64/! s/arm.*/arm/' \
		       -e 's/sa110/arm/' \
		       -e 's/ppc64/powerpc64/' \
		       -e 's/ppc/powerpc/' \
		       -e 's/macppc/powerpc/' \
		       -e 's/sh.*/sh/' )
else
# Nothing set, use detected host architecture
export CONFIG_UK_ARCH	?= $(shell echo "$(HOSTARCH)" | \
		   sed -e "s/-.*//" \
		       -e 's//\1/' \
		       -e 's/i.86/x86/' \
		       -e 's/sun4u/sparc64/' \
		       -e 's/arm64.*/arm64/' -e 's/aarch64.*/arm64/' \
		       -e '/arm64/! s/arm.*/arm/' \
		       -e 's/sa110/arm/' \
		       -e 's/ppc64/powerpc64/' \
		       -e 's/ppc/powerpc/' \
		       -e 's/macppc/powerpc/' \
		       -e 's/sh.*/sh/' )
endif
endif
override ARCH := $(CONFIG_UK_ARCH)
export CONFIG_UK_ARCH ARCH

export UK_FAMILY ?= $(shell echo "$(CONFIG_UK_ARCH)" | \
		   sed -e "s/-.*//" \
		       -e 's//\1/' \
		       -e 's/x86.*/x86/' \
		       -e 's/sparc64/sparc/' \
		       -e 's/arm.*/arm/' \
		       -e 's/powerpc.*/powerpc/' \
		       -e 's/sh.*/sh/' )


# Quick-check if architecture exists
ifeq ($(filter $(null_targets) print-vars,$(MAKECMDGOALS)),)
ifeq ($(wildcard $(CONFIG_UK_BASE)/arch/$(UK_FAMILY)/$(ARCH)/Makefile.uk),)
$(error Target architecture ($(ARCH)) is currently not supported (could not find $(CONFIG_UK_BASE)/arch/$(UK_FAMILY)/$(ARCH)/Makefile.uk).)
endif

ifeq ($(wildcard $(CONFIG_UK_BASE)/arch/$(UK_FAMILY)/$(ARCH)/Compiler.uk),)
$(error Target architecture ($(ARCH)) is currently not supported (could not find $(CONFIG_UK_BASE)/arch/$(UK_FAMILY)/$(ARCH)/Compiler.uk).)
endif
endif

################################################################################
# Compiler and linker tools
################################################################################
ifeq ($(UK_HAVE_DOT_CONFIG),y)
# Hide troublesome environment variables from sub processes
unexport CONFIG_CROSS_COMPILE
#unexport CC
#unexport LD
#unexport AR
#unexport CXX
#unexport CPP
unexport RANLIB
unexport CFLAGS
unexport CXXFLAGS
unexport ARFLAGS
unexport GREP_OPTIONS
unexport TAR_OPTIONS
unexport CONFIG_SITE
unexport QMAKESPEC
unexport TERMINFO
unexport MACHINE
#unexport O

# CONFIG_CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CONFIG_CROSS_COMPILE).
# CONFIG_CROSS_COMPILE can be set on the command line
# make CROSS_COMPILE=ia64-linux-
# Alternatively CONFIG_CROSS_COMPILE can be set in the environment.
# A third alternative is to store a setting in .config so that plain
# "make" in the configured kernel build directory always uses that.
# Default value for CONFIG_CROSS_COMPILE is not to prefix executables
# Note: Some architectures assign CONFIG_CROSS_COMPILE in their arch/*/Makefile.uk

ifneq ("$(origin CROSS_COMPILE)","undefined")
CONFIG_CROSS_COMPILE := $(CROSS_COMPILE:"%"=%)
endif

include $(CONFIG_UK_BASE)/arch/$(UK_FAMILY)/Compiler.uk

# Make variables (CC, etc...)
LD		:= $(CONFIG_CROSS_COMPILE)gcc
CC		:= $(CONFIG_CROSS_COMPILE)gcc
CPP		:= $(CC)
CXX		:= $(CPP)
AS		:= $(CC)
AR		:= $(CONFIG_CROSS_COMPILE)gcc-ar
NM		:= $(CONFIG_CROSS_COMPILE)gcc-nm
READELF		:= $(CONFIG_CROSS_COMPILE)readelf
ifeq ($(CONFIG_UK_ARCH),lkl)
STRIP		:= strip
else
STRIP		:= $(CONFIG_CROSS_COMPILE)strip
endif
OBJCOPY		:= $(CONFIG_CROSS_COMPILE)objcopy
OBJDUMP		:= $(CONFIG_CROSS_COMPILE)objdump
AR		:= ar
MV		:= mv -f
RM		:= rm -f
CP		:= cp -f
MKDIR		:= mkdir
CAT		:= cat
SED		:= sed
AWK		:= awk
PATCH		:= patch
GZIP		:= gzip
TAR		:= tar
UNZIP		:= unzip -qq -u
WGET		:= wget
TOUCH		:= touch
# Time requires the full path so that subarguments are handled correctly
TIME		:= $(shell which time)
LIFTOFF		:= liftoff -e -s
override ARFLAGS:= rcs
CC_VERSION	:= $(shell $(CC) --version | \
		   sed -n -r 's/^.* ([0-9]*)\.([0-9]*)\.([0-9]*)[ ]*.*/\1.\2/p')
# Retrieve GCC major and minor number from CC_VERSION. They would be used
# to select correct optimization parameters for target CPUs.
CC_VER_MAJOR   := $(word 1,$(subst ., ,$(CC_VERSION)))
CC_VER_MINOR   := $(word 2,$(subst ., ,$(CC_VERSION)))

ASFLAGS		+= -DCC_VERSION=$(CC_VERSION)
CFLAGS		+= -DCC_VERSION=$(CC_VERSION)
CXXFLAGS	+= -DCC_VERSION=$(CC_VERSION)

# Common Makefile definitions we need for building Unikraft
include $(CONFIG_UK_BASE)/support/build/Makefile.rules

# ensure $(BUILD_DIR)/include and $(BUILD_DIR)/include/uk exists
$(call mk_sub_build_dir,include)
$(call mk_sub_build_dir,include/uk)

ASINCLUDES            += -I$(UK_GENERATED_INCLUDES)
CINCLUDES             += -I$(UK_GENERATED_INCLUDES)
CXXINCLUDES           += -I$(UK_GENERATED_INCLUDES)

################################################################################
# Build rules
################################################################################
# external application
ifneq ($(CONFIG_UK_BASE),$(CONFIG_UK_APP))
$(eval $(call _import_lib,$(CONFIG_UK_APP)));
endif

# external libraries
$(foreach E,$(ELIB_DIR), \
	$(eval $(call _import_lib,$(E))); \
)
$(eval $(call _import_lib,$(CONFIG_UK_BASE)/arch/$(UK_FAMILY))) # architecture libraries
include $(CONFIG_UK_BASE)/plat/Makefile.uk # platform libraries
include $(CONFIG_UK_BASE)/lib/Makefile.uk # libraries
include $(CONFIG_UK_BASE)/Makefile.uk # Unikraft base

ifeq ($(call qstrip,$(UK_PLATS) $(UK_PLATS-y)),)
$(warning You did not choose any target platform.)
$(warning Please choose at least one target platform in the configuration!)
endif
ifneq ($(CONFIG_HAVE_BOOTENTRY),y)
$(error You did not select a library that handles bootstrapping! (e.g., ukboot))
endif

# Generate build rules
include $(CONFIG_UK_BASE)/support/build/Makefile.build

ifneq ($(call qstrip,$(UK_DEPS) $(UK_DEPS-y)),)
-include $(UK_DEPS) $(UK_DEPS-y) # include header dependencies
endif

include $(CONFIG_UK_BASE)/plat/Linker.uk

.PHONY: prepare image libs objs clean

fetch: $(UK_FETCH) $(UK_FETCH-y)

# Copy current configuration in order to detect changes
$(UK_CONFIG_OUT): $(UK_CONFIG)
	$(call verbose_cmd,CP,config,$(CP) \
		$(UK_CONFIG) \
		$(UK_CONFIG_OUT))

prepare: $(KCONFIG_AUTOHEADER) $(UK_CONFIG_OUT) $(UK_PREPARE) $(UK_PREPARE-y)
prepare: $(UK_FIXDEP) | fetch

objs: $(UK_OBJS) $(UK_OBJS-y)

libs: $(UK_ALIBS) $(UK_ALIBS-y) $(UK_OLIBS) $(UK_OLIBS-y)

images: $(UK_IMAGES) $(UK_IMAGES-y)

all: images

################################################################################
# Cleanup rules
################################################################################
# Generate cleaning rules
include $(CONFIG_UK_BASE)/support/build/Makefile.clean

clean: $(addprefix clean-,\
	$(foreach P,$(UK_PLATS) $(UK_PLATS-y),\
	$(if $(call qstrip,$($(call uc,$(P))_LIBS) $($(call uc,$(P))_LIBS-y)),\
	$(foreach L,$($(call uc,$(P))_LIBS) $($(call uc,$(P))_LIBS-y), $(L)))) $(UK_LIBS) $(UK_LIBS-y))
	$(call verbose_cmd,CLEAN,build/,$(RM) \
		$(UK_CONFIG_OUT) \
		$(call build_clean,$(UK_IMAGES-y)) \
		$(UK_CLEAN) $(UK_CLEAN-y))

else # !($(UK_HAVE_DOT_CONFIG),y)

all: menuconfig

.PHONY: prepare image libs objs clean

fetch: menuconfig

prepare: menuconfig

objs: menuconfig

libs: menuconfig

images: menuconfig

clean:

endif

.PHONY: distclean properclean
properclean:
	$(call verbose_cmd,RM,build/,$(RM) -r \
		$(BUILD_DIR))

distclean: properclean
	$(call verbose_cmd,RM,config,$(RM) \
		$(UK_CONFIG) $(UK_CONFIG).old $(CONFIG_DIR)/..config.tmp \
		$(CONFIG_DIR)/.auto.deps)

.PHONY: print-vars print-libs help outputmakefile list-defconfigs

# Configuration
# ---------------------------------------------------------------------------
HOSTCFLAGS = $(CFLAGS_FOR_BUILD)
export HOSTCFLAGS

# auto-generated KConfig files for including external app
$(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN): %: %.new
	@cmp -s $^ $@; if [ $$? -ne 0 ]; then cp $^ $@; fi

$(KCONFIG_APP_IN).new:
	@echo '# external application' > $@
ifneq ($(CONFIG_UK_BASE),$(CONFIG_UK_APP))
	@echo 'source "$(APP_DIR)/Config.uk"' >> $@
else
	@echo 'comment "No external application specified"' >> $@
endif

# auto-generated KConfig files for including external libraries
$(KCONFIG_ELIB_IN).new:
	@echo '# external libraries' > $@
	@$(foreach E,$(ELIB_DIR), \
		echo 'source "$(E)/Config.uk"' >> $@; \
	)

# enforce execution
.PHONY: $(KCONFIG_APP_IN).new $(KCONFIG_ELIB_IN).new

KCONFIG_TOOLS = conf mconf gconf nconf fixdep
KCONFIG_TOOLS := $(addprefix $(KCONFIG_DIR)/,$(KCONFIG_TOOLS))

$(KCONFIG_TOOLS):
	mkdir -p $(@D)/lxdialog
	$(MAKE) CC="$(HOSTCC_NOCCACHE)" HOSTCC="$(HOSTCC_NOCCACHE)" \
	    obj=$(@D) -C $(CONFIG) -f Makefile.br $(@F)

DEFCONFIG = $(call qstrip,$(UK_DEFCONFIG))

# We don't want to fully expand UK_DEFCONFIG here, so Kconfig will
# recognize that if it's still at its default $(CONFIG_DIR)/defconfig
COMMON_CONFIG_ENV = \
	CONFIG_="CONFIG_" \
	BR2_CONFIG="$(UK_CONFIG)" \
	KCONFIG_AUTOCONFIG="$(KCONFIG_AUTOCONFIG)" \
	KCONFIG_AUTOHEADER="$(KCONFIG_AUTOHEADER)" \
	KCONFIG_TRISTATE="$(KCONFIG_TRISTATE)" \
	HOST_GCC_VERSION="$(HOSTCC_VERSION)" \
	BUILD_DIR="$(BUILD_DIR)" \
	UK_BASE="$(CONFIG_UK_BASE)" \
	UK_APP="$(CONFIG_UK_APP)" \
	UK_CONFIG="$(UK_CONFIG)" \
	UK_FULLVERSION="$(UK_FULLVERSION)" \
	UK_CODENAME="$(UK_CODENAME)" \
	UK_ARCH="$(CONFIG_UK_ARCH)" \
	KCONFIG_APP_IN="$(KCONFIG_APP_IN)" \
	KCONFIG_ELIB_IN="$(KCONFIG_ELIB_IN)" \
	UK_NAME="$(CONFIG_UK_NAME)"

xconfig: $(KCONFIG_DIR)/qconf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $< $(CONFIG_CONFIG_IN)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

gconfig: $(KCONFIG_DIR)/gconf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) srctree=$(CONFIG_UK_BASE) $< $(CONFIG_CONFIG_IN)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

menuconfig: $(KCONFIG_DIR)/mconf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $< $(CONFIG_CONFIG_IN)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

nconfig: $(KCONFIG_DIR)/nconf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $< $(CONFIG_CONFIG_IN)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

config: $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $< $(CONFIG_CONFIG_IN)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

# For the config targets that automatically select options, we pass
# SKIP_LEGACY=y to disable the legacy options. However, in that case
# no values are set for the legacy options so a subsequent oldconfig
# will query them. Therefore, run an additional olddefconfig.
oldconfig: $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $< --oldconfig $(CONFIG_CONFIG_IN)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

randconfig: $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) SKIP_LEGACY=y $< --randconfig $(CONFIG_CONFIG_IN)
	@$(COMMON_CONFIG_ENV) $< --olddefconfig $(CONFIG_CONFIG_IN) >/dev/null
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

allyesconfig: $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) SKIP_LEGACY=y $< --allyesconfig $(CONFIG_CONFIG_IN)
	@$(COMMON_CONFIG_ENV) $< --olddefconfig $(CONFIG_CONFIG_IN) >/dev/null
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

allnoconfig: $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) SKIP_LEGACY=y $< --allnoconfig $(CONFIG_CONFIG_IN)
	@$(COMMON_CONFIG_ENV) $< --olddefconfig $(CONFIG_CONFIG_IN) >/dev/null
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

silentoldconfig: $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $< --silentoldconfig $(CONFIG_CONFIG_IN)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

olddefconfig: $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $< --olddefconfig $(CONFIG_CONFIG_IN)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

defconfig: $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $< --defconfig$(if $(DEFCONFIG),=$(DEFCONFIG)) $(CONFIG_CONFIG_IN)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

# Override the UK_DEFCONFIG from COMMON_CONFIG_ENV with the new defconfig
%_defconfig: $(KCONFIG_DIR)/conf $(A)/configs/%_defconfig $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) UK_DEFCONFIG=$(A)/configs/$@ \
		$< --defconfig=$(A)/configs/$@ $(CONFIG_CONFIG_IN)

savedefconfig: $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $< \
		--savedefconfig=$(if $(DEFCONFIG),$(DEFCONFIG),$(CONFIG_DIR)/defconfig) \
		$(CONFIG_CONFIG_IN)
	@$(SED) '/UK_DEFCONFIG=/d' $(if $(DEFCONFIG),$(DEFCONFIG),$(CONFIG_DIR)/defconfig)
	@$(SCRIPTS_DIR)/configupdate $(UK_CONFIG) $(UK_CONFIG_OUT)

.PHONY: defconfig savedefconfig silentoldconfig

# Regenerate $(KCONFIG_AUTOHEADER) whenever $(UK_CONFIG) changed
$(KCONFIG_AUTOHEADER): $(UK_CONFIG) $(KCONFIG_DIR)/conf $(KCONFIG_APP_IN) $(KCONFIG_ELIB_IN)
	@$(COMMON_CONFIG_ENV) $(KCONFIG_DIR)/conf --silentoldconfig $(CONFIG_CONFIG_IN)


# Misc stuff
# ---------------------------------------------------------------------------
print-vars:
	@$(foreach V, \
		$(sort $(if $(VARS),$(filter $(VARS),$(.VARIABLES)),$(.VARIABLES))), \
		$(if $(filter-out environment% default automatic, \
				$(origin $V)), \
		$(info $V=$($V) ($(value $V)))))

print-version:
	@echo $(UK_FULLVERSION)

print-libs:
	@echo 	$(foreach P,$(UK_PLATS) $(UK_PLATS-y),\
		$(if $(call qstrip,$($(call uc,$(P))_LIBS) $($(call uc,$(P))_LIBS-y)),\
		$(foreach L,$($(call uc,$(P))_LIBS) $($(call uc,$(P))_LIBS-y), \
		$(if $(call qstrip,$($(call vprefix_lib,$(L),SRCS)) $($(call vprefix_lib,$(L),SRCS-y))), \
		$(L) \
		)))) \
		$(UK_LIBS) $(UK_LIBS-y)

print-objs:
	@echo -e \
		$(foreach P,$(UK_PLATS) $(UK_PLATS-y),\
		$(if $(call qstrip,$($(call uc,$(P))_LIBS) $($(call uc,$(P))_LIBS-y)),\
		$(foreach L,$($(call uc,$(P))_LIBS) $($(call uc,$(P))_LIBS-y), \
		$(if $(call qstrip,$($(call vprefix_lib,$(L),OBJS)) $($(call vprefix_lib,$(L),OBJS-y))), \
		'$(L):\n   $($(call vprefix_lib,$(L),OBJS)) $($(call vprefix_lib,$(L),OBJS-y))\n'\
		))))\
		$(foreach L,$(UK_LIBS) $(UK_LIBS-y),\
		$(if $(call qstrip,$($(call vprefix_lib,$(L),OBJS)) $($(call vprefix_lib,$(L),OBJS-y))),\
		'$(L):\n   $($(call vprefix_lib,$(L),OBJS)) $($(call vprefix_lib,$(L),OBJS-y))\n'\
		))

print-srcs:
	@echo -e \
		$(foreach P,$(UK_PLATS) $(UK_PLATS-y),\
		$(if $(call qstrip,$($(call uc,$(P))_LIBS) $($(call uc,$(P))_LIBS-y)),\
		$(foreach L,$($(call uc,$(P))_LIBS) $($(call uc,$(P))_LIBS-y), \
		$(if $(call qstrip,$($(call vprefix_lib,$(L),SRCS)) $($(call vprefix_lib,$(L),SRCS-y))), \
		'$(L):\n   $($(call vprefix_lib,$(L),SRCS)) $($(call vprefix_lib,$(L),SRCS-y))\n'\
		))))\
		$(foreach L,$(UK_LIBS) $(UK_LIBS-y),\
		$(if $(call qstrip,$($(call vprefix_lib,$(L),SRCS)) $($(call vprefix_lib,$(L),SRCS-y))),\
		'$(L):\n   $($(call vprefix_lib,$(L),SRCS)) $($(call vprefix_lib,$(L),SRCS-y))\n'\
		))

help:
	@echo 'Cleaning:'
	@echo '  clean-[LIBNAME]        - delete all files created by build for a single library'
	@echo '                           (e.g., clean-libfdt)'
	@echo '  clean                  - delete all files created by build for all libraries'
	@echo '                           but keep fetched files'
	@echo '  properclean            - delete build directory'
	@echo '  distclean              - delete build directory and configurations (including .config)'
	@echo ''
	@echo 'Building:'
	@echo '* all                    - build everything (default target)'
	@echo '  images                 - build kernel images for selected platforms'
	@echo '  libs                   - build libraries and objects'
	@echo '  [LIBNAME]              - build a single library'
	@echo '  objs                   - build objects only'
	@echo '  prepare                - run preparation steps'
	@echo '  fetch                  - fetch, extract, and patch remote code'
	@echo ''
	@echo 'Configuration:'
	@echo '* menuconfig             - interactive curses-based configurator'
	@echo '                           (default target when no config exists)'
	@echo '  nconfig                - interactive ncurses-based configurator'
	@echo '  xconfig                - interactive Qt-based configurator'
	@echo '  gconfig                - interactive GTK-based configurator'
	@echo '  oldconfig              - resolve any unresolved symbols in .config'
	@echo '  silentoldconfig        - Same as oldconfig, but quietly, additionally update deps'
	@echo '  olddefconfig           - Same as silentoldconfig but sets new symbols to their default value'
	@echo '  randconfig             - New config with random answer to all options'
	@echo '  defconfig              - New config with default answer to all options'
	@echo '                             UK_DEFCONFIG, if set, is used as input'
	@echo '  savedefconfig          - Save current config to UK_DEFCONFIG (minimal config)'
	@echo '  allyesconfig           - New config where all options are accepted with yes'
	@echo '  allnoconfig            - New config where all options are answered with no'
	@echo
	@echo 'Miscellaneous:'
	@echo '  print-version          - print Unikraft version'
	@echo '  print-libs             - print library names enabled for build'
	@echo '  print-objs             - print object file names enabled for build'
	@echo '  print-srcs             - print source file names enabled for build'
	@echo '  print-vars             - prints all the variables currently defined in Makefile'
	@echo '  make V=0|1             - 0 => quiet build (default), 1 => verbose build'
	@echo ''

endif #umask

#!/usr/bin/env python

#----------------------------------------------------------------------
# Copyright (c) 2012-2015 Raytheon BBN Technologies
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and/or hardware specification (the "Work") to
# deal in the Work without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Work, and to permit persons to whom the Work
# is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Work.
#
# THE WORK IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE WORK OR THE USE OR OTHER DEALINGS
# IN THE WORK.
#----------------------------------------------------------------------

# Renew your slice and slivers for 2 months
# To be run from a cron job that runs at least every 2 months, and better every 7 days or more
# to handle aggregates that limit slivers to 14 day duration.

# Then call this script as "renewSliceAndSlivers.py <slicename>"
# Assumes that the GENI Clearinghouse correctly knows all slivers in your slice.
# To ensure this, use Omni 2.7+ to do 'sliverstatus' at all aggregates with resources in your slice.

# Return code will be non-0 if there were errors.

import datetime
import os
import pprint
import re
import sys

import gcf.oscript as omni
from gcf.omnilib.util.omnierror import OmniError
from gcf.omnilib.util.files import *
  

################################################################################
# Requires that you have omni installed and the path to gcf/src in your
# PYTHONPATH.
#
# For example put the following in your bashrc:
#     export PYTHONPATH=${PYTHONPATH}:path/to/gcf/src
#
################################################################################

def main(argv=None):
  ##############################################################################
  # Get a parser from omni that understands omni options
  ##############################################################################
  parser = omni.getParser()
  # update usage for help message
  omni_usage = parser.get_usage()
  parser.set_usage(omni_usage+"\nrenewSliceAndSlivers.py renews the given slice and all slivers at aggregates known to the GENI CH for 60 days.\n " +
                   "Takes slice name as argument")

  # options is an optparse.Values object, and args is a list
  options, args = omni.parse_args(sys.argv[1:], parser=parser)

  sliceName = args[0]
  newDate = datetime.datetime.utcnow() + datetime.timedelta(days=60)
  # Strip fractional seconds from times to avoid errors at PG AMs
  newDate = newDate.replace(microsecond=0)
  retcode = 0

  for command in ['renewslice', 'renewsliver']:
    # Here we use --raise-error-on-v2-amapi-error. Note though that if 1 AM has a problem, the script stops. Is that what we want?
    # IE will all AMs return code 0 if they renew the slice alap?
    # Could supply arg '--warn' to turn down logging. But then we'd want this script to have Omni write to a log file.
    omniargs = ['--alap', '--useSliceAggregates', '--raise-error-on-v2-amapi-error', command, sliceName, "'%s'" % newDate.isoformat()]

    print "Calling Omni to renew slice %s%s until %sZ\n" % (sliceName, (" slivers" if command=="renewsliver" else ""), newDate.isoformat())
    try:
      text, retItem = omni.call(omniargs, options)
    except OmniError, oe:
      print "\n ***** Omni call failed: %s\n" % oe
      retcode = str(oe)
      continue
    print text
    print "\n"
  return retcode

if __name__ == "__main__":
  sys.exit(main())


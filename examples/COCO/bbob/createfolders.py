#!/usr/bin/env python

"""
    This module generates the folder structure for doing a BBOB experiment.
    To use this module either do from the shell command line:
       $ python createfolders.py DATAPATH

    or from the python command line:
       >>> import createfolders
       >>> createfolders.main('DATAPATH')
"""

import os
import sys



class Usage(Exception):
    def __init__(self, msg):
        self.msg = msg

def main(argv=None):
    """Creates the whole folder structure for an experiment using fgeneric.

    This function, provided with a folder name, creates the whole folder
    structure needed by the fgeneric interface for the BBOB experiments.
    """

    if argv is None:
        argv = sys.argv[1:]

    try:
        if len(argv) < 1:
            raise Usage('Give as input to this script a disk location to '
                        'create the whole folder structure for fgeneric.')

        if isinstance(argv, basestring):
            outputdir = argv
        else:
            outputdir = argv[0] #only process the first argument.

        verbose = True

        if not os.path.exists(outputdir):
            os.mkdir(outputdir)
            if verbose:
                print '%s was created.' % (outputdir)

        for i in range(1, 25):
            datapath = os.path.join(outputdir, 'data_f%d' % i)
            if not os.path.exists(datapath):
                os.mkdir(datapath)

        for i in range(101, 131):
            datapath = os.path.join(outputdir, 'data_f%d' % i)
            if not os.path.exists(datapath):
                os.mkdir(datapath)

    except Usage, err:
        print >>sys.stderr, err.msg
        #print >>sys.stderr, "for help use -h or --help"
        return 2

if __name__ == "__main__":
   sys.exit(main())


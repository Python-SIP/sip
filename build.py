#!/usr/bin/env python

# Copyright (c) 2004
#	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
#
# This script does all the work of building the SIP distribution.


import sys
import os
import shutil
import stat
import time


Package = "sip"


def clean():
    """Clean the build environment.
    """
    global Package

    try:
        shutil.rmtree(Package,1)
    except:
        pass


def lcopy(lfile,src,dst):
    """Copy a file from source to destination inserting a license if required.

    lfile is the name of the license file.
    src  is the name of the source file.
    dst is the name of the destination file.
    """
    # Use the short version of the license if there is one.
    if os.access(lfile + ".short",os.F_OK):
        lfile += ".short"

    sf = open(src,"r")
    df = open(dst,"w")

    line = sf.readline()

    while line:
        lpos = line.find("@BS_LICENSE@")

        if lpos >= 0:
            print "Licensing", dst

            prefix = line[:lpos]

            lf = open(lfile)

            lline = lf.readline()

            while lline:
                df.write(prefix + lline)
                lline = lf.readline()

            lf.close()
        else:
            df.write(line)

        line = sf.readline()

    sf.close()
    df.close()

    shutil.copymode(src,dst)


def lcopydir(lfile,src,dst):
    """Copy the files from a source directory to a destination directory
    inserting a license if required.

    lfile is the name of the license file.
    src is the name of the source directory.
    dst is the name of the destination directory which is created
    automatically.
    """
    os.mkdir(dst)

    for f in os.listdir(src):
        sf = src + "/" + f
        df = dst + "/" + f

        if not stat.S_ISDIR(os.stat(sf).st_mode):
            lcopy(lfile,sf,df)


def copydir(dir):
    """Copy a source directory to the package directory without any changes.

    dir is the name of the directory.
    """
    ddir = os.path.join(Package, dir)
    os.mkdir(ddir)

    for f in os.listdir(dir):
        if f != ".svn":
            shutil.copy(os.path.join(dir, f), os.path.join(ddir, f))


def mkdistdir(lfile):
    """Create the distribution directory and copy in all the required files.

    lfile is the license file to use.
    """
    print "Creating the distribution directory"

    global Package

    os.mkdir(Package)

    shutil.copy("NEWS",Package)
    shutil.copy("TODO",Package)
    shutil.copy("ChangeLog",Package)
    shutil.copy("lib/README",Package)
    shutil.copy("lib/README.Fink",Package)
    shutil.copy("lib/README.HP-UX",Package)
    shutil.copy("lib/THANKS",Package)
    shutil.copy("lib/sipdistutils.py",Package)
    os.system("srepo release <%s >%s/LICENSE" % (lfile, Package))

    lcopy(lfile,"lib/configure.py","configure.py.in")
    os.system("srepo release <configure.py.in >%s/configure.py" % Package)
    os.remove("configure.py.in")

    lcopy(lfile,"lib/siputils.py",Package + "/siputils.py")

    print "Installing qmake configuration files"
    copydir("specs")

    print "Installing custom interpreter skeleton"
    copydir("custom")

    lcopydir(lfile,"sipgen",Package + "/sipgen")
    os.system("srepo release <%s/sipgen/main.c >%s/sipgen/main.c.new" % (Package,Package))
    old = os.getcwd()
    os.chdir(Package + "/sipgen")
    os.rename("main.c.new","main.c")

    # Some makes (HP-UX seems to be one) seem to consider a file is out of date
    # if it has the same modification time.  Therefore we wait a while to make
    # sure the generated files have later timestamps.
    time.sleep(2)

    os.system("flex -olexer.c.tmp lexer.l")
    os.system("sed -e 's/#include <unistd.h>//' <lexer.c.tmp >lexer.c")
    os.remove("lexer.c.tmp")
    os.system("bison -y -d -o parser.c parser.y")

    os.chdir(old)

    lcopydir(lfile,"siplib",Package + "/siplib")
    os.system("srepo release <%s/siplib/sip.h >%s/siplib/sip.h.new" % (Package,Package))
    os.rename(Package + "/siplib/sip.h.new",Package + "/siplib/sip.h")

    print "Installing the documentation"
    doc = os.path.join(Package, "doc")
    os.mkdir(doc)
    shutil.copy("doc/default.css",doc)
    os.system("srepo release <doc/sipref.txt >%s/sipref.txt" % doc)
    os.system("docutils-rst2html.py %s/sipref.txt %s/sipref.html" % (doc, doc))


def tgzdist(root):
    """Create a tarball distribution.

    root is the root directory of the distribution.
    """
    print "Creating .tar.gz file"

    os.system("tar zcf %s.tar.gz %s" % (root,root))


def zipdist(root):
    """Create a zip distribution.

    root is the root directory of the distribution.
    """
    print "Creating .zip file"

    os.system("zip -r -9 %s.zip %s" % (root,root))


# Parse the command line.

import getopt

PkgFormat = None

try:
    optlist, args = getopt.getopt(sys.argv[1:],"p:")
except getopt.GetoptError:
    print "Invalid option"
    sys.exit(1)

for opt, arg in optlist:
    if opt == "-p":
        if arg not in ("zip", "tgz"):
            print "Invalid package format"
            sys.exit(1)

        PkgFormat = arg


# Do the build.

clean()
mkdistdir("lib/LICENSE")


if PkgFormat:
    p = os.popen("srepo query")
    vers = p.readline().strip()
    p.close()

    pkgroot = Package + "-" + vers

    os.rename(Package,pkgroot)

    if PkgFormat == "zip":
        zipdist(pkgroot)
    else:
        tgzdist(pkgroot)

    os.rename(pkgroot,Package)


sys.exit(0)

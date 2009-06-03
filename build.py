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


def copydir(src, dst=None):
    """Copy a source directory to the package directory without any changes.

    src is the name of the directory.
    """
    if not dst:
        dst = os.path.join(Package, src)

    slen = len(src.split(os.path.sep))

    for sroot, sdirs, sfiles in os.walk(src):
        if ".svn" in sdirs:
            sdirs.remove(".svn")

        droot = dst
        for p in sroot.split(os.path.sep)[slen:]:
            droot = os.path.join(droot, p)

        os.mkdir(droot)

        for sf in sfiles:
            sfname = os.path.join(sroot, sf)
            dfname = os.path.join(droot, sf)
            shutil.copy(sfname, dfname)


def mkdistdir(lfile):
    """Create the distribution directory and copy in all the required files.

    lfile is the license file to use.
    """
    print "Creating the distribution directory"

    root_dir = os.getcwd()

    os.mkdir(Package)

    shutil.copy("NEWS",Package)
    shutil.copy("ChangeLog",Package)
    shutil.copy("lib/README",Package)
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

    os.chdir(root_dir)

    lcopydir(lfile,"siplib",Package + "/siplib")
    os.system("srepo release <%s/siplib/sip.h >%s/siplib/sip.h.new" % (Package,Package))
    os.rename(Package + "/siplib/sip.h.new",Package + "/siplib/sip.h")


def mkdocs():
    print "Installing the documentation"

    root_dir = os.getcwd()

    copydir("sphinx")
    sphinx = os.path.join(Package, "sphinx")
    os.system("srepo release <sphinx/conf.py >%s/conf.py" % sphinx)

    os.chdir(sphinx)
    os.system("make html")
    os.chdir(root_dir)

    doc_dir = os.path.join(Package, 'doc')
    os.mkdir(doc_dir)
    html_dir = os.path.join(doc_dir, 'html')
    copydir(os.path.join(sphinx, 'build', 'html'), html_dir)
    os.remove(os.path.join(html_dir, '.buildinfo'))

    os.chdir(sphinx)
    os.system("make clean")
    os.chdir(root_dir)


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
    mkdocs()

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

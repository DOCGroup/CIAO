#!/usr/bin/env python
# -*- coding: utf-8 -*-

# @file make_release.py
# @author William R. Otte <wotte@dre.vanderbilt.edu>
# @author Johnny Willemsen <jwillemsen@remedy.nl>
#
# Packaging script for CIAO

from __future__ import with_statement
from time import strftime
import re
import tempfile
import shutil
import subprocess
import shlex
import multiprocessing

##################################################
#### Global variables
##################################################
""" Options from the command line """
opts=None

""" Arguments from the command line """
args=None

""" Absolute path from the git workspace to be used for the
release"""
doc_root=None

""" A dict containing version information used for the release.
This dict contains entries of the form
COMPONENT_version
COMPONENT_minor
COMPONENT_major
COMPONENT_code """
comp_versions = dict ()
old_comp_versions = dict ()

release_date = strftime (# ie: Mon Jan 23 00:35:37 CST 2006
                              "%a %b %d %H:%M:%S %Z %Y")
cpu_count = multiprocessing.cpu_count()

# Packaging configuration

""" This is a regex that detects files that SHOULD NOT have line endings
converted to CRLF when being put into a ZIP file """
bin_regex = re.compile ("\.(mak|mdp|ide|exe|ico|gz|zip|xls|sxd|gif|vcp|vcproj|vcw|sln|dfm|jpg|png|vsd|bz2|pdf|ppt|graffle|pptx|odt|sh)$")

##################################################
#### Utility Methods
##################################################
def parse_args ():
    from optparse import OptionParser

    parser = OptionParser ("usage %prog [options]")

    parser.add_option ("--major", dest="release_type", action="store_const",
                       help="Create a major release.", default=None, const="major")
    parser.add_option ("--minor", dest="release_type", action="store_const",
                       help="Create a minor release.", default=None, const="minor")
    parser.add_option ("--micro", dest="release_type", action="store_const",
                       help="Create a micro release.", default=None, const="micro")

    parser.add_option ("--tag", dest="tag", action="store_true",
                       help="Tag the repositorie with all needed tags", default=False)
    parser.add_option ("--update", dest="update", action="store_true",
                       help="Update the version numbers", default=False)
    parser.add_option ("--push", dest="push", action="store_true",
                       help="Push all changes to remote", default=False)

    parser.add_option ("--kit", dest="action", action="store_const",
                       help="Create kits. DO NOT USE WITH --tag", default=None, const="kit")
    parser.add_option ("--dest", dest="package_dir", action="store",
                       help="Specify destination for the created packages.", default=None)

    parser.add_option ("--root", dest="repo_root", action="store",
                       help="Specify an alternate repository root",
                       default="https://github.com/DOCGroup/CIAO.git")

    parser.add_option ("-n", dest="take_action", action="store_false",
                       help="Take no action", default=True)
    parser.add_option ("--verbose", dest="verbose", action="store_true",
                       help="Print out actions as they are being performed",
                       default=False)
    (options, arguments) = parser.parse_args ()

    if options.tag:
        if options.release_type is None:
            parser.error ("When tagging, must specify a release type")

        if options.update is False:
            print "Warning: You are tagging a release, but not requesting a version increment"

        if options.push is False:
            print "Warning: You are tagging a release, but not requesting a push to remote"

    return (options, arguments)


def ex (command):
    from os import system
    global opts
    vprint ("Executing " + command)

    if not opts.take_action:
        print "Executing " + command
        return

    status = system(command)
    if status != 0:
        print "ERROR: Nonzero return value from " + command
        raise Exception

###
# Checks that the users environment is sane.
#
def check_environment ():
    from os import getenv

    global doc_root, opts

    doc_root = getenv ("DOC_ROOT")
    if (doc_root is None):
        print "ERROR: Environment DOC_ROOT must be defined."
        return False

    return True

def vprint (string):
    """ Prints the supplied message if verbose is enabled"""
    global opts

    if opts.verbose:
        print string

##################################################
#### Tagging methods
##################################################
def commit (files):
    """ Commits the supplied list of files to the repository. """
    import shutil, os
    global comp_versions

    version = "CIAO-%d_%d_%d" % (comp_versions["CIAO_major"],
                                 comp_versions["CIAO_minor"],
                                 comp_versions["CIAO_micro"])
    vprint ("Committing the following files for " + version + " " + " ".join (files))

    if opts.take_action:
        for file in files:
            print "Adding file " + file + " to commit"
            ex ("git add " + file)

        ex ("git commit -m\"" + version + "\"")

#        print "Checked in files, resulting in revision ", rev.number

def check_workspace ():
    """ Checks that the CIAO repository is up to date.  """
    global opts, doc_root
    try:
        ex ("git pull -p")
        print "Successfully updated CIAO working copy"
    except:
        print "Unable to update CIAO workspace at " + doc_root
        raise

    vprint ("Repos root URL = " + opts.repo_root + "\n")

def update_version_files (component):
    """ Updates the version files for a given component.  This includes
    Version.h, the PRF, and the VERSION file."""

    global comp_versions, opts, release_date

    vprint ("Updating version files for " + component)

    import re

    retval = list ()

    ## Update VERSION
    with open ("VERSION", "r+") as version_file:
        new_version = re.sub (component + " version .*",
                              "%s version %s, released %s" % (component,
                                                              comp_versions[component + "_version"],
                                                              release_date),
                              version_file.read ())
        if opts.take_action:
            version_file.seek (0)
            version_file.truncate (0)
            version_file.write (new_version)
        else:
            print "New version file for " + component
            print new_version

        vprint ("Updating Version.h for " + component)

    retval += ["VERSION"]

    ## Update component/Version.h
    version_header = """
// -*- C++ -*-
// This is file was automatically generated by $CIAO_ROOT/bin/make_release.py

#define %s_MAJOR_VERSION %s
#define %s_MINOR_VERSION %s
#define %s_MICRO_VERSION %s
#define %s_VERSION \"%s\"
#define %s_VERSION_CODE %s
#define %s_MAKE_VERSION_CODE(a,b,c) (((a) << 16) + ((b) << 8) + (c))
""" % (component, comp_versions[component + "_major"],
       component, comp_versions[component + "_minor"],
       component, comp_versions[component + "_micro"],
       component, comp_versions[component + "_version"],
       component, comp_versions[component + "_code"],
       component)

    if opts.take_action:
        with open (component.lower () + "/Version.h", 'r+') as version_h:
            version_h.write (version_header)
    else:
        print "New Version.h for " + component
        print version_header

    retval += [component.lower () + "/Version.h"]

    # Update component/PROBLEM-REPORT-FORM
    vprint ("Updating PRF for " + component)

    version_string = re.compile ("^\s*(\w+) +VERSION ?:")

    with open ("PROBLEM-REPORT-FORM", 'r+') as prf:
        new_prf = ""
        for line in prf.readlines ():
            match = None
            match = version_string.search (line)
            if match is not None:
                vprint ("Found PRF Version for " + match.group (1))
                line = re.sub ("(\d\.)+\d?",
                               comp_versions[match.group(1) + "_version"],
                               line)

            new_prf += line

        if opts.take_action:
            prf.seek (0)
            prf.truncate (0)
            prf.writelines (new_prf)
        else:
            print "New PRF for " + component
            print "".join (new_prf)

    retval += ["PROBLEM-REPORT-FORM"]
    return retval


def get_and_update_versions ():
    """ Gets current version information for each component,
    updates the version files, creates changelog entries,
    and commit the changes into the repository."""
    global comp_versions, opts

    try:
        get_comp_versions ("CIAO")

        if opts.update:
            files = list ()
            files += update_version_files ("CIAO")
            files += create_changelog ("CIAO")

            print "Committing " + str(files)
            commit (files)

    except:
        print "Fatal error in get_and_update_versions."
        raise

def create_changelog (component):
    """ Creates a changelog entry for the supplied component that includes
    the version number being released"""
    vprint ("Creating ChangeLog entry for " + component)

    global old_comp_versions, comp_versions, opts

    old_tag = "CIAO-%d_%d_%d" % (old_comp_versions["CIAO_major"],
                                 old_comp_versions["CIAO_minor"],
                                 old_comp_versions["CIAO_micro"])

    # Generate changelogs per component
    ex ("git log " + old_tag + "..HEAD " + " > " + "ChangeLogs/" + component + "-" + comp_versions[component + "_version_"])

    return ["ChangeLogs/%s-%s" % (component, comp_versions[component + "_version_"])]

def get_comp_versions (component):
    """ Extracts the current version number from the VERSION
    file and increments it appropriately for the release type
    requested."""
    vprint ("Detecting current version for " + component)

    import re

    global old_comp_versions, comp_versions, opts

    micro = re.compile ("version (\d+)\.(\d+)\.(\d+)")
    minor = re.compile ("version (\d+)\.(\d+)[^\.]")
    major = re.compile ("version (\d+)[^\.]")

    with open ("VERSION") as version_file:
        for line in version_file:
            match = None

            match = micro.search (line)
            if match is not None:
                vprint ("Detected micro version %s.%s.%s" %
                           (match.group (1), match.group (2), match.group (3)))

                comp_versions[component + "_major"] = int (match.group (1))
                comp_versions[component + "_minor"] = int (match.group (2))
                comp_versions[component + "_micro"] = int (match.group (3))
                break

            match = minor.search (line)
            if match is not None:
                vprint ("Detected minor version %s.%s" %
                            (match.group (1), match.group (2)))

                comp_versions[component + "_major"] = int (match.group (1))
                comp_versions[component + "_minor"] = int (match.group (2))
                comp_versions[component + "_micro"] = 0
                break

            match = major.search (line)
            if match is not None:
                vprint ("Detected major version " + match.group (1) + ".0")

                comp_versions[component + "_major"] = int (match.group (1))
                comp_versions[component + "_minor"] = 0
                comp_versions[component + "_micro"] = 0
                break

            print "FATAL ERROR: Unable to locate current version for " + component
            raise Exception

    # Also store the current release (old from now)
    old_comp_versions[component + "_major"] = comp_versions[component + "_major"]
    old_comp_versions[component + "_minor"] = comp_versions[component + "_minor"]
    old_comp_versions[component + "_micro"] = comp_versions[component + "_micro"]

    if opts.update:
        if opts.release_type == "major":
            comp_versions[component + "_major"] += 1
            comp_versions[component + "_minor"] = 0
            comp_versions[component + "_micro"] = 0
        elif opts.release_type == "minor":
            comp_versions[component + "_minor"] += 1
            comp_versions[component + "_micro"] = 0
        elif opts.release_type == "micro":
            comp_versions[component + "_micro"] += 1

    #if opts.release_type == "micro":
    comp_versions [component + "_version"] = \
        str (comp_versions[component + "_major"])  + '.' + \
        str (comp_versions[component + "_minor"])  + '.' + \
        str (comp_versions[component + "_micro"])
    comp_versions [component + "_version_"] = \
        str (comp_versions[component + "_major"])  + '_' + \
        str (comp_versions[component + "_minor"])  + '_' + \
        str (comp_versions[component + "_micro"])

    comp_versions [component + "_code"] = \
        str((comp_versions[component + "_major"] << 16) + \
            (comp_versions[component + "_minor"] << 8) + \
            comp_versions[component + "_micro"])

    old_comp_versions [component + "_version"] = \
        str (old_comp_versions[component + "_major"])  + '.' + \
        str (old_comp_versions[component + "_minor"])  + '.' + \
        str (old_comp_versions[component + "_micro"])
    old_comp_versions [component + "_version_"] = \
        str (old_comp_versions[component + "_major"])  + '_' + \
        str (old_comp_versions[component + "_minor"])  + '_' + \
        str (old_comp_versions[component + "_micro"])

    if opts.update:
      vprint ("Updating from version %s to version %s" %
                  (old_comp_versions [component + "_version"], comp_versions [component + "_version"]))
    else:
      vprint ("Found version %s" %
                  (comp_versions [component + "_version"]))

    # else:
    #     comp_versions [component + "_version"] = \
    #                   str (comp_versions[component + "_major"])  + '.' + \
    #                   str (comp_versions[component + "_minor"])


def update_latest_tag (which, branch):
    """ Update one of the Latest_* tags externals to point the new release """
    global opts
    tagname = "Latest_" + which

    # Remove tag locally
    vprint ("Removing tag %s" % (tagname))
    ex ("git tag -d " + tagname)

    vprint ("Placing tag %s" % (tagname))
    ex ("git tag -a " + tagname + " -m\"" + tagname + "\"")


def push_latest_tag (which, branch):
    """ Update one of the Latest_* tags externals to point the new release """
    global opts
    tagname = "Latest_" + which

    if opts.push:
        # Remove tag in the remote orgin
        ex ("git push origin :refs/tags/" + tagname)

        vprint ("Pushing tag %s" % (tagname))
        ex ("git push origin " + tagname)

def tag ():
    """ Tags the CIAO repository for the version and push that remote """
    global comp_versions, opts

    tagname = "CIAO-%d_%d_%d" % (comp_versions["CIAO_major"],
                                 comp_versions["CIAO_minor"],
                                 comp_versions["CIAO_micro"])

    if opts.tag:
        if opts.take_action:
            vprint ("Placing tag %s on CIAO" % (tagname))
            ex ("git tag -a " + tagname + " -m\"" + tagname + "\"")

            # Update latest tag
            if opts.release_type == "major":
                update_latest_tag ("Major", tagname)
                update_latest_tag ("Minor", tagname)
                update_latest_tag ("Micro", tagname)
            elif opts.release_type == "minor":
                update_latest_tag ("Minor", tagname)
                update_latest_tag ("Micro", tagname)
            elif opts.release_type == "micro":
                update_latest_tag ("Micro", tagname)
        else:
            vprint ("Placing tag %s on CIAO" % (tagname))
            print "Creating tags:\n"
            print "Placing tag " + tagname + "\n"

def push ():
    """ Tags the CIAO repository for the version and push that remote """
    global comp_versions, opts

    tagname = "CIAO-%d_%d_%d" % (comp_versions["CIAO_major"],
                                 comp_versions["CIAO_minor"],
                                 comp_versions["CIAO_micro"])

    if opts.push:
        if opts.take_action:
            vprint ("Pushing CIAO master to origin")
            ex ("git push origin master")

            vprint ("Pushing tag %s on CIAO" % (tagname))
            ex ("git push origin tag " + tagname)

            # Update latest tag
            if opts.release_type == "major":
                push_latest_tag ("Major", tagname)
            elif opts.release_type == "minor":
                push_latest_tag ("Minor", tagname)
            elif opts.release_type == "micro":
                push_latest_tag ("Micro", tagname)
        else:
            vprint ("Pushing tag %s on CIAO" % (tagname))
            print "Pushing tags:\n"
            print "Pushing tag " + tagname + "\n"

##################################################
#### Packaging methods
##################################################
def export_wc (stage_dir):

    global doc_root, comp_versions

    tag = "CIAO-%d_%d_%d" % (comp_versions["CIAO_major"],
                             comp_versions["CIAO_minor"],
                             comp_versions["CIAO_micro"])

    # Clone the CIAO repository with the needed tag
    print ("Retrieving CIAO with tag " + tag)
    ex ("git clone --depth 1 --branch " + tag + " " + opts.repo_root + " " + stage_dir + "/CIAO")

def update_packages (text_files, bin_files, stage_dir, package_dir):
    import os

    print "Updating packages...."
    os.chdir (stage_dir + "/CIAO")

    # -g appends, -q for quiet operation
    zip_base_args = " -gqu "
    # -l causes line ending conversion for windows
    zip_text_args = " -l "
    zip_file = stage_dir + "/zip-archive.zip"

    # -r appends, -f specifies file.
    tar_args = "-uf "
    tar_file = stage_dir + "/tar-archive.tar"

    # Zip binary files
    print "\tAdding binary files to zip...."
    p = subprocess.Popen (shlex.split ("xargs zip " + zip_base_args + zip_file), stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)
    instream, outstream = (p.stdin, p.stdout)

    instream.write (bin_files)

    instream.close ()
    outstream.close ()

    # Need to wait for zip process spawned by popen2 to complete
    # before proceeding.
    os.wait ()

    print "\tAdding text files to zip....."
    p = subprocess.Popen (shlex.split ("xargs zip " + zip_base_args + zip_text_args + zip_file), stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)
    instream, outstream = (p.stdin, p.stdout)

    instream.write (text_files)

    instream.close ()
    outstream.close ()

    # Need to wait for zip process spawned by popen2 to complete
    # before proceeding.
    os.wait ()

    # Tar files
    print "\tAdding to tar file...."
    if (not os.path.exists (tar_file)):
        open(tar_file, 'w').close ()

    p = subprocess.Popen (shlex.split ("xargs tar " + tar_args + tar_file), stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)
    instream, outstream = (p.stdin, p.stdout)
    instream.write (' ' + bin_files + ' ' + text_files)

    instream.close ()

    print outstream.read ()
    outstream.close ()

    os.wait ()

def move_packages (name, stage_dir, package_dir):
    """ Copies the temporary files from the stage_dir to the package_dir.
        Renames them to name.tar and name.zip, respectively, and compresses
        the tarfile with gzip and bzip2. """
    import shutil, os
    from os.path import join

    print "Storing packages for ", name

    # Take care of the zip file
    print "\tZip file..."
    target_file = join (package_dir, name + ".zip")
    shutil.copy (join (stage_dir, "zip-archive.zip"),
                 target_file)
    ex ("md5sum " + target_file + " > " + target_file + ".md5")

    tar_file = join (stage_dir, "tar-archive.tar")
    target_file = join (package_dir, name + ".tar")

    # bzip
    print "\tBzip2 file....."
    shutil.copy (tar_file,
                 target_file)
    ex ("bzip2 " + target_file)
    ex ("md5sum " + target_file + ".bz2 > " + target_file + ".bz2.md5")

    print "\tgzip file....."
    shutil.copy (tar_file,
                 target_file)
    ex ("gzip " + target_file)
    ex ("md5sum " + target_file + ".gz > " + target_file + ".gz.md5")

def create_file_lists (base_dir, prefix, exclude):
    """ Creates two lists of files:  files that need CR->CRLF
    conversions (useful for zip files) and those that don't,
    excluding files/directories found in exclude. """
    import os

    text_files = list ()
    bin_files = list ()

    for root, dirs, files in os.walk (base_dir, topdown=True):
#        print "root", root

        relroot = root.replace (base_dir, "")

#        print "relroot", relroot

        if len(relroot) and relroot[0] == '/':
            relroot = relroot [1:]

        excluded = False
        for item in exclude:
            dir_item = item + '/'
            if relroot.startswith (dir_item) or relroot.startswith (item):
#                print "excluding", relroot
                excluded = True
#            else:
#                print relroot, "does not start with", dir_item, "or", item

        if excluded:
            continue

        # Remove dirs that are listed in our exclude pattern
        for item in dirs:
#            print "item", item
            # Remove our excludes
            if (item) in exclude:
#                print "Removing " + item + " from consideration...."
                dirs.remove (item)

        # Remove files that are listed in our exclude pattern
        for item in files:
            fullitem = os.path.join (relroot, item)
            if fullitem in exclude or item in exclude:
#                print "Removing " + fullitem + " from consideration...."
                continue
            else:
                if bin_regex.search (fullitem) is not None:
                    bin_files.append ('"' + os.path.join (prefix, fullitem) + '"')
                else:
                    text_files.append ('"' + os.path.join (prefix, fullitem) + '"')

    return (text_files, bin_files)

def write_file_lists (comp, text, bin):
    outfile = open (comp + ".files", 'w')

    outfile.write ("\n".join (text))
    outfile.write (".............\nbin files\n.............\n")
    outfile.write ("\n".join (bin))

    outfile.close ()

def package (stage_dir, package_dir, decorator):
    """ Packages CIAO releases of current
        staged tree, with decorator appended to the name of the archive. """
    from os.path import join
    from os import remove
    from os import chdir

    chdir (stage_dir)

    text_files = list ()
    bin_files = list ()

    # Erase our old temp files
    try:
#        print "removing files", join (stage_dir, "zip-archive.zip"), join (stage_dir, "tar-archive.tar")
        remove (join (stage_dir, "zip-archive.zip"))
        remove (join (stage_dir, "tar-archive.tar"))
    except:
        print "error removing files", join (stage_dir, "zip-archive.zip"), join (stage_dir, "tar-archive.tar")
        pass # swallow any errors

    text_files, bin_files = create_file_lists (join (stage_dir, "CIAO"),
                                               "", [".gitignore", ".git"])

#    write_file_lists ("fACE" + decorator, text_files, bin_files)
    update_packages ("\n".join (text_files),
                     "\n".join (bin_files),
                     stage_dir,
                     package_dir)

    move_packages ("CIAO" + decorator, stage_dir, package_dir)

def create_kit ():
    """ Creates kits """
    import os
    from os.path import join
    # Get version numbers for this working copy, note this will
    # not update the numbers.
    print "Getting current version information...."

    get_comp_versions ("CIAO")

    print "Creating working directories...."
    stage_dir, package_dir = make_working_directories ()

    print "Exporting working copy..."
    export_wc (stage_dir)

    version = "%d_%d_%d" % (comp_versions["CIAO_major"],
                            comp_versions["CIAO_minor"],
                            comp_versions["CIAO_micro"])

    ### make source only packages
    package (stage_dir, package_dir, "-" + version + "-src")

def make_working_directories ():
    """ Creates directories that we will be working in.
    In particular, we will have DOC_ROOT/stage-PID and
    DOC_ROOT/packages-PID """
    global doc_root
    import os.path, os

    stage_dir = os.path.join (doc_root, "stage-" + str (os.getpid ()))
    package_dir = os.path.join (doc_root, "package-" + str (os.getpid ()))

    os.mkdir (stage_dir)
    os.mkdir (package_dir)

    return (stage_dir, package_dir)

def main ():
    global opts

    if opts.action == "kit":
        print "Creating a kit."
        raw_input ("Press enter to continue")

        create_kit ()

    else:
        print "Making a " + opts.release_type + " release."
        raw_input ("Press enter to continue")

        get_and_update_versions ()
        tag ()
        push ()

if __name__ == "__main__":
    (opts, args) = parse_args ()

    if check_environment() is not True:
        exit (1)

    main ()
